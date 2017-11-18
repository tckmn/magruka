#include <stdio.h>
#include <time.h>

#include "magruka.h"

void draw_hex(struct magruka *m, int x, int y, int n) {
    m->img.hexclip.y = 26*n;
    m->img.hexdest.x = 10 + 22*x;
    m->img.hexdest.y = 10 + 13*x + 26*y;
    SDL_RenderCopy(m->rend, m->img.hex, &m->img.hexclip, &m->img.hexdest);
}

/*
 * load all the data in the assets directory into memory
 */
int load_assets(struct magruka *m) {
    SDL_Surface *tmp;
    tmp = IMG_Load("assets/img/hex.png");
    if (tmp) {
        SDL_SetColorKey(tmp, SDL_TRUE, SDL_MapRGB(tmp->format, 0xff, 0x00, 0xff));
        m->img.hex = SDL_CreateTextureFromSurface(m->rend, tmp);
        SDL_FreeSurface(tmp);
        if (!m->img.hex) {
            fprintf(stderr, "could not optimize hex image\n(SDL error: %s)\n",
                    SDL_GetError());
            return 1;
        }
    } else {
        fprintf(stderr, "could not load hex image\n(SDL/IMG error: %s)\n",
                IMG_GetError());
        return 1;
    }
    m->img.hexclip.w = m->img.hexdest.w = 30;
    m->img.hexclip.h = m->img.hexdest.h = 26;
    m->img.hexclip.x = m->img.hexdest.x = 0;
    m->img.hexclip.y = m->img.hexdest.y = 0;
    return 0;
}

#define FAIL(msg) do { \
    magruka_destroy(m); \
    fprintf(stderr, msg "\n(SDL error: %s)\n", SDL_GetError()); \
    return NULL; \
} while (0)

struct magruka *magruka_init() {
    srand(time(NULL));

    // main game struct - basically all information is kept in here
    struct magruka *m = malloc(sizeof *m);

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) FAIL("failed to initialize SDL");

    // create game window
    m->win = SDL_CreateWindow("magruka",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);
    if (!m->win) FAIL("window could not be created");

    // get surface
    m->surf = SDL_GetWindowSurface(m->win);

    // create renderer
    m->rend = SDL_CreateRenderer(m->win, -1, SDL_RENDERER_ACCELERATED);
    if (!m->rend) FAIL("could not create hardware renderer");
    SDL_SetRenderDrawColor(m->rend, 0x00, 0x00, 0x00, 0xff);

    // generate map
    m->mapw = 9;
    m->maph = 9;
    m->map = malloc(m->mapw * sizeof *m->map);
    for (int x = 0; x < m->mapw; ++x) {
        m->map[x] = malloc(m->maph * sizeof *m->map[x]);
        for (int y = 0; y < m->maph; ++y) {
            if (x+y < m->mapw/2 || x+y >= m->mapw+m->mapw/2) {
                m->map[x][y] = 0;
            } else {
                m->map[x][y] = 1 + rand()%3;
            }
        }
    }

    // place player
    m->player.x = m->mapw / 2;
    m->player.y = 0;

    // load assets
    if (load_assets(m)) FAIL("failed to load assets");

    return m;
}

void magruka_main_loop(struct magruka *m) {
    SDL_Event e;
    for (;;) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                return;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                case 'q': return;
                case 'u': m->player.x -= 1; break;
                case 'i': m->player.y -= 1; break;
                case 'o': m->player.x += 1; m->player.y -= 1; break;
                case 'j': m->player.x -= 1; m->player.y += 1; break;
                case 'k': m->player.y += 1; break;
                case 'l': m->player.x += 1; break;
                }
                break;
            default:
                // unknown event, ignore it
                break;
            }
        }

        // start rendering stuff
        SDL_RenderClear(m->rend);

        // draw map
        for (int x = 0; x < m->mapw; ++x) {
            for (int y = 0; y < m->maph; ++y) {
                draw_hex(m, x, y, m->map[x][y]);
                draw_hex(m, x, y + 10, m->map[x][y] ? (m->map[x][y]%3)+1 : 0);
                draw_hex(m, x + 10, y, m->map[x][y] ? ((m->map[x][y]+1)%3)+1 : 0);
            }
        }

        // draw player
        draw_hex(m, m->player.x, m->player.y, 4);
        draw_hex(m, m->player.x + 10, m->player.y, 4);
        draw_hex(m, m->player.x, m->player.y + 10, 4);

        // render everything
        SDL_RenderPresent(m->rend);
    }
}

void magruka_destroy(struct magruka *m) {
    SDL_DestroyTexture(m->img.hex);
    SDL_DestroyWindow(m->win);
    SDL_Quit();
    for (int x = 0; x < m->mapw; ++x) {
        free(m->map[x]);
    }
    free(m->map);
    free(m);
}
