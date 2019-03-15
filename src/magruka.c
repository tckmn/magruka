/*
 * magruka - a game based on Richard Bartle's Waving Hands
 * Copyright (C) 2019  Andy Tockman <andy@tck.mn>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <time.h>

#include "magruka.h"

void draw(struct magruka *m, int x, int y, SDL_Rect clip) {
    SDL_Rect dest = {x, y, clip.w, clip.h};
    SDL_RenderCopy(m->rend, m->img.spritesheet, &clip, &dest);
}

void anim(struct magruka *m, int x, int y, SDL_Rect clip, int offs) {
    SDL_Rect src = {clip.x + offs*clip.w, clip.y, clip.w, clip.h};
    SDL_Rect dest = {x, y, clip.w, clip.h};
    SDL_RenderCopy(m->rend, m->img.spritesheet, &src, &dest);
}

void write(struct magruka *m, int x, int y, char *text) {
    SDL_Rect src = {m->img.letters.x, m->img.letters.y, 0, m->img.letterh * SCALE_FACTOR};
    SDL_Rect dest = {x, y, 0, src.h};
    for (; *text; ++text) {
        if (*text == ' ') {
            dest.x += 2*SCALE_FACTOR + SCALE_FACTOR/2;
        } else {
            int ch = *text - 'A';
            src.x = m->img.letters.x + m->img.letterx[ch] * SCALE_FACTOR;
            src.w = dest.w = m->img.letterw[ch] * SCALE_FACTOR;
            SDL_RenderCopy(m->rend, m->img.spritesheet, &src, &dest);
            dest.x += dest.w + SCALE_FACTOR/2;
        }
    }
}

/*
 * load all the data in the assets directory into memory
 */
int load_assets(struct magruka *m) {
    SDL_Surface *tmp;
    tmp = IMG_Load("assets/img/spritesheet.png");
    if (tmp) {
        SDL_SetColorKey(tmp, SDL_TRUE, SDL_MapRGB(tmp->format, 0xff, 0x00, 0xff));
        m->img.spritesheet = SDL_CreateTextureFromSurface(m->rend, tmp);
        SDL_FreeSurface(tmp);
        if (!m->img.spritesheet) {
            fprintf(stderr, "could not optimize spritesheet image\n(SDL error: %s)\n",
                    SDL_GetError());
            return 1;
        }
    } else {
        fprintf(stderr, "could not load spritesheet image\n(SDL/IMG error: %s)\n",
                IMG_GetError());
        return 1;
    }

    m->img.wiz      = R(0,  0,  16, 37);
    m->img.wall     = R(0,  37, 18, 16);
    m->img.floor    = R(18, 37, 18, 16);
    m->img.floortop = R(36, 37, 18, 16);
    m->img.letters  = P(0, 53);
    m->img.letterw[0] = 3;
    m->img.letterw[1] = 3;
    m->img.letterw[2] = 3;
    m->img.letterw[3] = 3;
    m->img.letterw[4] = 3;
    m->img.letterw[5] = 3;
    m->img.letterw[6] = 3;
    m->img.letterw[7] = 3;
    m->img.letterw[8] = 1;
    m->img.letterw[9] = 3;
    m->img.letterw[10] = 3;
    m->img.letterw[11] = 3;
    m->img.letterw[12] = 5;
    m->img.letterw[13] = 4;
    m->img.letterw[14] = 3;
    m->img.letterw[15] = 3;
    m->img.letterw[16] = 4;
    m->img.letterw[17] = 3;
    m->img.letterw[18] = 3;
    m->img.letterw[19] = 3;
    m->img.letterw[20] = 3;
    m->img.letterw[21] = 3;
    m->img.letterw[22] = 5;
    m->img.letterw[23] = 3;
    m->img.letterw[24] = 3;
    m->img.letterw[25] = 3;
    m->img.letterh = 5;
    for (int x = 0, i = 0; i < 26; x += m->img.letterw[i], i++) {
        m->img.letterx[i] = x;
    }

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
    m->rend = SDL_CreateRenderer(m->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m->rend) FAIL("could not create hardware renderer");
    SDL_SetRenderDrawColor(m->rend, 0x00, 0x00, 0x00, 0xff);

    // load assets
    if (load_assets(m)) FAIL("failed to load assets");

    return m;
}

void magruka_main_loop(struct magruka *m) {
    SDL_Event e;
    int frame = 0;
    for (;;) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                return;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                case 'q': return;
                }
                break;
            default:
                // unknown event, ignore it
                break;
            }
        }

        // start rendering stuff
        SDL_RenderClear(m->rend);

        // draw background tiles
        for (int x = 0; x < SCREEN_WIDTH; x += m->img.wall.w) {
            for (int y = 0; y < SCREEN_HEIGHT; y += m->img.wall.h) {
                draw(m, x, y, m->img.wall);
            }
        }

        // draw foreground floor
        int top = 1;
        for (int y = FLOOR_POS; y < SCREEN_HEIGHT; y += m->img.floor.h) {
            for (int x = 0; x < SCREEN_WIDTH; x += m->img.floor.w) {
                draw(m, x, y, top ? m->img.floortop : m->img.floor);
            }
            top = 0;
        }

        // draw temporary wizard
        int asdf = 8;
        ++frame;
        frame %= 7*asdf;
        anim(m, 100, FLOOR_POS - m->img.wiz.h, m->img.wiz, abs(3-frame/asdf));

        // draw temporary text
        write(m, 100, 100, "PLAYER ONE");

        // render everything
        SDL_RenderPresent(m->rend);
    }
}

void magruka_destroy(struct magruka *m) {
    SDL_DestroyTexture(m->img.spritesheet);
    SDL_DestroyWindow(m->win);
    SDL_Quit();
    free(m);
}
