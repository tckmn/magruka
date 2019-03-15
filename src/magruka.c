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
    SDL_Rect src = {m->img.letters.x, m->img.letters.y, 0, m->img.letterh};
    SDL_Rect dest = {x, y, 0, src.h};
    for (; *text; ++text) {
        if (*text == ' ') {
            dest.x += 2*SCALE1 + SCALE1/2;
        } else if (*text == '\n') {
            dest.x = x;
            dest.y += m->img.letterh;
        } else {
            int ch = 'A' <= *text && *text <= 'Z' ? *text - 'A' :
                     'a' <= *text && *text <= 'z' ? *text - 'a' + 26 :
                     '0' <= *text && *text <= '9' ? *text - '0' + 52 : 0;
            src.x = m->img.letters.x + m->img.letterx[ch];
            src.w = dest.w = m->img.letterw[ch];
            SDL_RenderCopy(m->rend, m->img.spritesheet, &src, &dest);
            dest.x += dest.w + SCALE1/2;
        }
    }
}

SDL_Texture* gentext(struct magruka *m, char *text, int *w, int *h) {
    // TODO error handling?
    SDL_Surface *tmp = TTF_RenderText_Solid(m->font, text, (SDL_Color){0xff, 0xff, 0xff, 0xff});
    *w = tmp->w;
    *h = tmp->h;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(m->rend, tmp);
    SDL_FreeSurface(tmp);
    return texture;
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

    if (TTF_Init() == -1) {
        printf("could not initialize TTF library\n(SDL/TTF error: %s)\n",
                TTF_GetError());
        return 1;
    }
    m->font = TTF_OpenFont("assets/font/November.ttf", 16);
    if (!m->font) {
        fprintf(stderr, "could not open font\n(SDL/TTF error: %s)\n",
                TTF_GetError());
        return 1;
    }

    m->img.wiz      = R1(0,  0,  16, 37);
    m->img.wall     = R1(0,  37, 18, 16);
    m->img.floor    = R1(18, 37, 18, 16);
    m->img.floortop = R1(36, 37, 18, 16);

    m->img.letters     = P1(0, 53);
    m->img.letterw[0]  = 4; m->img.letterw[1]  = 4; m->img.letterw[2]  = 4;
    m->img.letterw[3]  = 4; m->img.letterw[4]  = 4; m->img.letterw[5]  = 4;
    m->img.letterw[6]  = 4; m->img.letterw[7]  = 4; m->img.letterw[8]  = 3;
    m->img.letterw[9]  = 4; m->img.letterw[10] = 4; m->img.letterw[11] = 4;
    m->img.letterw[12] = 5; m->img.letterw[13] = 5; m->img.letterw[14] = 4;
    m->img.letterw[15] = 4; m->img.letterw[16] = 4; m->img.letterw[17] = 4;
    m->img.letterw[18] = 4; m->img.letterw[19] = 3; m->img.letterw[20] = 4;
    m->img.letterw[21] = 5; m->img.letterw[22] = 5; m->img.letterw[23] = 5;
    m->img.letterw[24] = 5; m->img.letterw[25] = 4; m->img.letterw[26] = 4;
    m->img.letterw[27] = 3; m->img.letterw[28] = 3; m->img.letterw[29] = 3;
    m->img.letterw[30] = 3; m->img.letterw[31] = 3; m->img.letterw[32] = 3;
    m->img.letterw[33] = 3; m->img.letterw[34] = 1; m->img.letterw[35] = 3;
    m->img.letterw[36] = 3; m->img.letterw[37] = 2; m->img.letterw[38] = 5;
    m->img.letterw[39] = 3; m->img.letterw[40] = 3; m->img.letterw[41] = 3;
    m->img.letterw[42] = 3; m->img.letterw[43] = 3; m->img.letterw[44] = 3;
    m->img.letterw[45] = 3; m->img.letterw[46] = 4; m->img.letterw[47] = 3;
    m->img.letterw[48] = 5; m->img.letterw[49] = 3; m->img.letterw[50] = 3;
    m->img.letterw[51] = 3; m->img.letterw[52] = 4; m->img.letterw[53] = 3;
    m->img.letterw[54] = 4; m->img.letterw[55] = 4; m->img.letterw[56] = 4;
    m->img.letterw[57] = 4; m->img.letterw[58] = 4; m->img.letterw[59] = 4;
    m->img.letterw[60] = 4; m->img.letterw[61] = 4;
    m->img.letterh = 10 * SCALE1;
    for (int x = 0, i = 0; i < 62; x += m->img.letterw[i] + SCALE1, i++) {
        m->img.letterw[i] *= SCALE1;
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

    int thingw, thingh;
    SDL_Texture *thing = gentext(m, "C D P W      Dispel magic               P S D F          Charm personC S W W S    Summon elemental           P S F W          Summon ogreC w          Magic mirror               P W P F S S S D  Finger of deathD F F D D    Lightning bolt             P W P W W C      HasteD F P W      Cure heavy wounds          S D              MissileD F W        Cure light wounds          S F W            Summon goblinD P P        Amnesia                    S P F            Anti spellD S F        Confusion                  S P F P S D W    PermanencyD S F F F C  Disease                    S P P C          Time stopD W F F d    Blindness                  S S F P          Resist coldD W S S S P  Delayed effect             S W D            FearD W W F W C  Raise dead                 S W W C          Fire stormD W W F W D  Poison                     W D D C          Lightning boltF F F        Paralysis                  W F P            Cause light woundsF P S F W    Summon troll               W F P S F W      Summon giantF S S D D    Fireball                   W P F D          Cause heavy woundsP            Shield                     W P P            Counter spellp            Surrender                  W S S C          Ice stormP D W P      Remove enchantment         W W F P          Resist heatP P w s      Invisibility               W W P            Protection from evilP S D D      Charm monster              W W S            Counter spell", &thingw, &thingh);

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
        write(m, 10, 10, "Player 1");
        SDL_Rect src = (SDL_Rect){0, 0, thingw, thingh};
        SDL_Rect dest = (SDL_Rect){200, 10, thingw, thingh};
        SDL_RenderCopy(m->rend, thing, &src, &dest);

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
