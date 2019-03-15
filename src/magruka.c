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
            dest.x += 2*SCALE2 + SCALE2/2;
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
            dest.x += dest.w + SCALE2/2;
        }
    }
}

void writebig(struct magruka *m, int x, int y, char *text) {
    SDL_Rect src = {m->img.bigletters.x, m->img.bigletters.y, 0, m->img.bigletterh};
    SDL_Rect dest = {x, y, 0, src.h};
    for (; *text; ++text) {
        if (*text == ' ') {
            dest.x += 2*SCALE1 + SCALE1/2;
        } else {
            int ch = *text - 'A';
            src.x = m->img.bigletters.x + m->img.bigletterx[ch];
            src.w = dest.w = m->img.bigletterw[ch];
            SDL_RenderCopy(m->rend, m->img.spritesheet, &src, &dest);
            dest.x += dest.w + SCALE1/2;
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

    m->img.wiz      = R1(0,  0,  16, 37);
    m->img.wall     = R1(0,  37, 18, 16);
    m->img.floor    = R1(18, 37, 18, 16);
    m->img.floortop = R1(36, 37, 18, 16);

    m->img.letters     = P2(0, 0);
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
    m->img.letterh = 10 * SCALE2;
    for (int x = 0, i = 0; i < 62; x += m->img.letterw[i] + SCALE2, i++) {
        m->img.letterw[i] *= SCALE2;
        m->img.letterx[i] = x;
    }

    m->img.bigletters     = P1(0, 53);
    m->img.bigletterw[0]  = 3; m->img.bigletterw[1]  = 3; m->img.bigletterw[2]  = 3;
    m->img.bigletterw[3]  = 3; m->img.bigletterw[4]  = 3; m->img.bigletterw[5]  = 3;
    m->img.bigletterw[6]  = 3; m->img.bigletterw[7]  = 3; m->img.bigletterw[8]  = 1;
    m->img.bigletterw[9]  = 3; m->img.bigletterw[10] = 3; m->img.bigletterw[11] = 3;
    m->img.bigletterw[12] = 5; m->img.bigletterw[13] = 4; m->img.bigletterw[14] = 3;
    m->img.bigletterw[15] = 3; m->img.bigletterw[16] = 4; m->img.bigletterw[17] = 3;
    m->img.bigletterw[18] = 3; m->img.bigletterw[19] = 3; m->img.bigletterw[20] = 3;
    m->img.bigletterw[21] = 3; m->img.bigletterw[22] = 5; m->img.bigletterw[23] = 3;
    m->img.bigletterw[24] = 3; m->img.bigletterw[25] = 3;
    m->img.bigletterh = 5 * SCALE1;
    for (int x = 0, i = 0; i < 26; x += m->img.bigletterw[i], i++) {
        m->img.bigletterw[i] *= SCALE1;
        m->img.bigletterx[i] = x;
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
        write(m, 10, 10, "C D P W      Dispel magic               P S D F          Charm person\nC S W W S    Summon elemental           P S F W          Summon ogre\nC w          Magic mirror               P W P F S S S D  Finger of death\nD F F D D    Lightning bolt             P W P W W C      Haste\nD F P W      Cure heavy wounds          S D              Missile\nD F W        Cure light wounds          S F W            Summon goblin\nD P P        Amnesia                    S P F            Anti spell\nD S F        Confusion                  S P F P S D W    Permanency\nD S F F F C  Disease                    S P P C          Time stop\nD W F F d    Blindness                  S S F P          Resist cold\nD W S S S P  Delayed effect             S W D            Fear\nD W W F W C  Raise dead                 S W W C          Fire storm\nD W W F W D  Poison                     W D D C          Lightning bolt\nF F F        Paralysis                  W F P            Cause light wounds\nF P S F W    Summon troll               W F P S F W      Summon giant\nF S S D D    Fireball                   W P F D          Cause heavy wounds\nP            Shield                     W P P            Counter spell\np            Surrender                  W S S C          Ice storm\nP D W P      Remove enchantment         W W F P          Resist heat\nP P w s      Invisibility               W W P            Protection from evil\nP S D D      Charm monster              W W S            Counter spell");

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
