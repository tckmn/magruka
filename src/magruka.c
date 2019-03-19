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
#include <stdlib.h>
#include <time.h>

#include "magruka.h"
#include "util.h"
#include "battle.h"

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
    SDL_SetTextureBlendMode(m->img.spritesheet, SDL_BLENDMODE_BLEND);

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

    m->img.wiz            = R1(0,   0,   16, 37);
    m->img.wall           = R1(0,   37,  18, 16);
    m->img.floor          = R1(18,  37,  18, 16);
    m->img.floortop       = R1(36,  37,  18, 16);
    m->img.gesture        = R1(0,   63,  28, 32);
    m->img.gesturefinal   = R1(0,   95,  28, 32);
    m->img.healthcirc     = R1(0,   127, 15, 15);
    m->img.healthbar      = R1(15,  127, 40, 15);
    m->img.healthend      = R1(55,  127, 1,  15);
    m->img.healthcapr     = R1(56,  127, 2,  15);
    m->img.healthcapl     = R1(57,  127, 2,  15);
    m->img.spellchoose    = R1(0,   191, 50, 50);
    m->img.c_particles    = R1(0,   142, 11, 11);
    m->img.d_particles    = R1(110, 142, 11, 11);
    m->img.f_particles    = R1(0,   153, 11, 11);
    m->img.p_particles    = R1(110, 153, 11, 11);
    m->img.s_particles    = R1(0,   164, 11, 11);
    m->img.w_particles    = R1(110, 164, 11, 11);
    m->img.stab_particles = R1(0,   175, 11, 11);
    m->img.none_particles = R1(110, 175, 11, 11);
    m->img.gparticles     = R1(0,   186, 5,  5);
    m->img.gfparticles    = R1(15,  186, 5,  5);
    m->img.key            = R2(0,   0,   9,  9);
    m->img.keytop         = R2(0,   0,   9,  5);
    m->img.keybot         = R2(0,   4,   9,  5);
    m->img.keyall         = R2(0,   9,   9,  9);
    m->img.keyboth        = R2(0,   18,  9,  9);
    m->img.handind        = R2(0,   27,  9,  9);

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

    // data file format is as follows:
    //  * name followed by NUL
    //  * gestures followed by NUL
    //  * single byte with damage+128 (e.g. 0x80 = 0 damage)
    //  * newline (for convenience in editing the file)
    m->nspells = 0;
    m->spells = malloc(sizeof *m->spells);
    FILE *f = fopen("assets/data/spells.dat", "r");
    if (!f) {
        fprintf(stderr, "could not open spell data file\n");
        return 1;
    }
    int ch, pos;
    for (;;) {
        // read name
        pos = 0;
        while ((ch = getc(f))) {
            if (ch == EOF) goto done;
            m->spells[m->nspells].name[pos++] = ch;
        }
        m->spells[m->nspells].name[pos] = 0;
        // read gestures
        m->spells[m->nspells].ngest = 0;
        while ((ch = getc(f))) {
            m->spells[m->nspells].gesture[m->spells[m->nspells].ngest++] = g2n(ch);
        }
        // read damage
        m->spells[m->nspells].damage = getc(f) - 0x80;
        // discard newline
        if (getc(f) != '\n') {
            fprintf(stderr, "corrupted spell data file\n");
            return 1;
        }
        // append a new spell
        m->spells = realloc(m->spells, (++m->nspells + 1) * sizeof *m->spells);
    }
done:

    m->spellnamew = m->spellnameh = 0;
    for (struct spell *sp = m->spells; sp->name[0]; ++sp) {
        sp->nameimg = gentext(m, sp->name, COLOR_WHITE);
        if (sp->nameimg.w > m->spellnamew) m->spellnamew = sp->nameimg.w;
        if (sp->nameimg.h > m->spellnameh) m->spellnameh = sp->nameimg.h;
        if (!sp->nameimg.texture) return 1;
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
    // start in battle state
    m->state = STATE_BATTLE;
    struct battlestate *b = battle_init(m);

    int ret;
    for (;;) {
        switch (m->state) {
            case -1: return;
            case STATE_BATTLE: ret = battle_main_loop(m, b); break;
        }
        if (ret) m->state = ret;
    }
}

void magruka_destroy(struct magruka *m) {
    SDL_DestroyTexture(m->img.spritesheet);
    SDL_DestroyWindow(m->win);
    SDL_Quit();
    free(m);
}
