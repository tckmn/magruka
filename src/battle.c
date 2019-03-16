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

#include "battle.h"
#include "util.h"

struct battlestate *battle_init(void) {
    struct battlestate *b = malloc(sizeof *b);
    b->lh = b->rh = -1;
    b->lhf = b->rhf = 0;
    b->page = 0;
    return b;
}

int battle_main_loop(struct magruka *m, struct battlestate *b) {
    while (SDL_PollEvent(&m->e)) {
        switch (m->e.type) {

        case SDL_QUIT:
            return -1;

        case SDL_KEYDOWN:
            switch (m->e.key.keysym.sym) {
            case SDLK_ESCAPE: return -1;

            case 'q': b->lhf |= 1 << GESTURE_STAB; b->lh = GESTURE_STAB; break;
            case 'w': b->lhf |= 1 << GESTURE_W;    b->lh = GESTURE_W;    break;
            case 'e': b->lhf |= 1 << GESTURE_C;    b->lh = GESTURE_C;    break;
            case 'r': b->lhf |= 1 << GESTURE_P;    b->lh = GESTURE_P;    break;
            case 'a': b->lhf |= 1 << GESTURE_NONE; b->lh = GESTURE_NONE; break;
            case 's': b->lhf |= 1 << GESTURE_S;    b->lh = GESTURE_S;    break;
            case 'd': b->lhf |= 1 << GESTURE_D;    b->lh = GESTURE_D;    break;
            case 'f': b->lhf |= 1 << GESTURE_F;    b->lh = GESTURE_F;    break;

            case 'p': b->rhf |= 1 << GESTURE_STAB; b->rh = GESTURE_STAB; break;
            case 'o': b->rhf |= 1 << GESTURE_W;    b->rh = GESTURE_W;    break;
            case 'i': b->rhf |= 1 << GESTURE_C;    b->rh = GESTURE_C;    break;
            case 'u': b->rhf |= 1 << GESTURE_P;    b->rh = GESTURE_P;    break;
            case ';': b->rhf |= 1 << GESTURE_NONE; b->rh = GESTURE_NONE; break;
            case 'l': b->rhf |= 1 << GESTURE_S;    b->rh = GESTURE_S;    break;
            case 'k': b->rhf |= 1 << GESTURE_D;    b->rh = GESTURE_D;    break;
            case 'j': b->rhf |= 1 << GESTURE_F;    b->rh = GESTURE_F;    break;

            case '.': if (b->page < m->nspells / SPELLS_PER_PAGE - 1) ++b->page; break;
            case ',': if (b->page > 0) --b->page; break;

            case ' ': b->lhf = b->rhf = 0; b->lh = b->rh = -1; break;

            }
            break;

        case SDL_KEYUP:
            switch (m->e.key.keysym.sym) {

            case 'q': b->lhf &= ~(1 << GESTURE_STAB); break;
            case 'w': b->lhf &= ~(1 << GESTURE_W);    break;
            case 'e': b->lhf &= ~(1 << GESTURE_C);    break;
            case 'r': b->lhf &= ~(1 << GESTURE_P);    break;
            case 'a': b->lhf &= ~(1 << GESTURE_NONE); break;
            case 's': b->lhf &= ~(1 << GESTURE_S);    break;
            case 'd': b->lhf &= ~(1 << GESTURE_D);    break;
            case 'f': b->lhf &= ~(1 << GESTURE_F);    break;

            case 'p': b->rhf &= ~(1 << GESTURE_STAB); break;
            case 'o': b->rhf &= ~(1 << GESTURE_W);    break;
            case 'i': b->rhf &= ~(1 << GESTURE_C);    break;
            case 'u': b->rhf &= ~(1 << GESTURE_P);    break;
            case ';': b->rhf &= ~(1 << GESTURE_NONE); break;
            case 'l': b->rhf &= ~(1 << GESTURE_S);    break;
            case 'k': b->rhf &= ~(1 << GESTURE_D);    break;
            case 'j': b->rhf &= ~(1 << GESTURE_F);    break;

            }
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

    // draw spell list
    int ypos = 10;
    struct spell *sp = m->spells;
    for (int i = 0; i < b->page * SPELLS_PER_PAGE; ++i) ++sp;
    for (int i = 0; sp->name[0] && i < SPELLS_PER_PAGE; ++sp, ++i) {
        drawtext(m, SCREEN_WIDTH/2 - m->spellnamew, ypos, sp->nameimg);
        ypos += m->spellnameh + 4;
    }

    // draw wizard
    anim(m, 100, FLOOR_POS - m->img.wiz.h, m->img.wiz, 0);

    // draw held gestures
    if (b->lh != -1) anim(m, SCREEN_WIDTH/2 - m->spellnamew - m->img.gesture.w - 20, 200, b->lhf ? m->img.gesture : m->img.gesturefinal, b->lh);
    if (b->rh != -1) anim(m, SCREEN_WIDTH/2 + m->spellnamew + 20, 200, b->rhf ? m->img.gesture : m->img.gesturefinal, b->rh);

    // check for finalized turn
    if (b->lh != -1 && b->rh != -1 && !b->lhf && !b->rhf) {
        printf("it went\n");
    }

    /* // draw temporary text */
    /* write(m, 10, 10, "Player 1"); */

    // render everything
    SDL_RenderPresent(m->rend);

    return 0;
}

void battle_destroy(struct battlestate *b) {
    free(b);
}
