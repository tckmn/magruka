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
#include "task.h"

int drawhpbar(struct magruka *m, int x, int y, int flip, struct creature c, double prop) {
    // negative proportion = automatically calculate
    if (prop < 0) prop = (double)c.hp / c.maxhp;

    int circx = flip ? x - m->img.healthcirc.w : x,
        hbarx = flip ? circx - m->img.healthbar.w + SCALE1 : circx + m->img.healthcirc.w - SCALE1,
        capx = flip ? hbarx - m->img.healthcapl.w : hbarx + m->img.healthbar.w,
        endx = flip ? capx + m->img.healthend.w : capx,
        namex = flip ? circx - c.nameimg.w : hbarx + SCALE1,
        barw = prop * (m->img.healthbar.w + SCALE1),
        barx = flip ? circx + SCALE1 - barw : hbarx;

    draw(m, endx, y, m->img.healthend);
    draw(m, hbarx, y, m->img.healthbar);

    // NOTE: this is hardcoded to the spritesheet
    SDL_Rect bar = {barx, y + 4*SCALE1, barw, 7*SCALE1};
    SDL_SetRenderDrawColor(m->rend, COLOR_HPBAR.r, COLOR_HPBAR.g, COLOR_HPBAR.b, COLOR_HPBAR.a);
    SDL_RenderFillRect(m->rend, &bar);

    draw(m, circx, y, m->img.healthcirc);
    draw(m, capx, y, flip ? m->img.healthcapl : m->img.healthcapr);
    drawtext(m, circx + m->img.healthcirc.w/2 - c.hpimg.w - 1, y + m->img.healthcirc.h/2 - c.hpimg.h - 1, c.hpimg);
    drawtext(m, circx + m->img.healthcirc.w/2 + 1, y + m->img.healthcirc.h/2 + 1, c.maxhpimg);

    drawtext(m, namex, y, c.nameimg);

    return y + m->img.healthcirc.h;
}

int drawgest(struct magruka *m, int x, int y, struct playerdata *p) {
    int y1 = y, y2 = y + m->img.handind.h + 4;

    anim(m, x, y1, m->img.handind, 1);
    anim(m, x, y2, m->img.handind, 0);

    int xpos = x + m->img.handind.w*2;
    for (int *lg = p->lh, *rg = p->rh; *lg != SPELL_END; ++lg, ++rg) {
        anim(m, xpos, y1, m->img.key, *lg);
        anim(m, xpos, y2, m->img.key, *rg);
    }

    return y2 + m->img.handind.h;
}

struct add_gestures_data {
    struct playerdata *pd;
    int lh, rh, n;
};

int add_gestures_func(struct add_gestures_data *agd) {
    if (agd->pd->timer == 0) {
        agd->pd->timer = SDL_GetTicks();
        agd->pd->lh[agd->n] = agd->lh;
        agd->pd->rh[agd->n] = agd->rh;
        agd->pd->lh[agd->n+1] = SPELL_END;
        agd->pd->rh[agd->n+1] = SPELL_END;
    }
    if (SDL_TICKS_PASSED(SDL_GetTicks(), agd->pd->timer + 1000)) {
        agd->pd->timer = 0;
        return 1;
    }
    return 0;
}

struct taskfunc add_gestures(struct playerdata *pd, int lh, int rh) {
    struct add_gestures_data *agd = malloc(sizeof *agd);
    agd->pd = pd;
    agd->lh = lh;
    agd->rh = rh;
    int n = 0;
    for (int *lg = pd->lh; *lg != SPELL_END; ++lg) ++n;
    agd->n = n;
    return (struct taskfunc){add_gestures_func, agd};
}

struct battlestate *battle_init(struct magruka *m) {
    struct battlestate *b = malloc(sizeof *b);

    b->tasks = task_init();

    b->lh = b->rh = -1;
    b->lhf = b->rhf = 0;
    b->page = 0;

    creature_init(m, &b->p1, "Player 1", 15);
    creature_init(m, &b->p2, "Player 2", 15);
    struct playerdata *pd1 = malloc(sizeof *pd1),
                      *pd2 = malloc(sizeof *pd2);
    pd1->lh[0] = pd1->rh[0] = pd2->lh[0] = pd2->rh[0] = SPELL_END;
    pd1->timer = pd2->timer = 0;
    b->p1.data = pd1;
    b->p2.data = pd2;

    b->p1.img = b->p2.img = m->img.wiz;
    b->p1.x = 100; b->p1.y = FLOOR_POS - b->p1.img.h;
    b->p2.x = SCREEN_WIDTH - 100 - b->p2.img.w; b->p2.y = FLOOR_POS - b->p2.img.h;
    b->p1.frame = b->p2.frame = 0;
    b->p1.flip = 0; b->p2.flip = 1;

    b->polling = 1;

    return b;
}

int battle_main_loop(struct magruka *m, struct battlestate *b) {
    task_update(b->tasks);

    while (SDL_PollEvent(&m->e)) {
        switch (m->e.type) {

        case SDL_QUIT:
            return -1;

        case SDL_KEYDOWN:
            if (m->e.key.keysym.sym == SDLK_ESCAPE) return -1;
            if (b->polling) {
                switch (m->e.key.keysym.sym) {

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
            }
            break;

        case SDL_KEYUP:
            if (b->polling) {
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
            }
            break;

        }
    }

    int ypos;

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

    if (b->polling) {
        // draw spell list
        ypos = 10;
        struct spell *sp = m->spells;
        for (int i = 0; i < b->page * SPELLS_PER_PAGE; ++i) ++sp;
        for (int i = 0; sp->name[0] && i < SPELLS_PER_PAGE; ++sp, ++i) {
            // draw spell name
            drawtext(m, SCREEN_WIDTH/2 - m->spellnamew - SPELL_LIST_PAD, ypos, sp->nameimg);
            // draw gesture list
            int xpos = SCREEN_WIDTH/2 + SPELL_LIST_PAD;
            for (int *g = sp->gesture; *g != SPELL_END; ++g) {
                anim(m, xpos, ypos, *g > 0 ? m->img.key : m->img.keyboth, abs(*g));
                xpos += m->img.key.w + 2;
            }
            // next line
            ypos += m->spellnameh + 4;
        }
    }

    // draw wizards
    creature_draw(m, &b->p1);
    creature_draw(m, &b->p2);

    // draw health bars
    ypos = 5;
    ypos = drawhpbar(m, 5, ypos, 0, b->p1, -1) + STATUS_PAD;
    ypos = drawgest(m, 5, ypos, b->p1.data) + STATUS_PAD;
    drawhpbar(m, SCREEN_WIDTH - 5, 5, 1, b->p2, -1);

    // draw held gestures (TODO: positioning)
    if (b->lh != -1) anim(m, SCREEN_WIDTH/2 - m->spellnamew - m->img.gesture.w - 20, 200, b->lhf ? m->img.gesture : m->img.gesturefinal, b->lh);
    if (b->rh != -1) anim(m, SCREEN_WIDTH/2 + m->spellnamew + 20, 200, b->rhf ? m->img.gesture : m->img.gesturefinal, b->rh);

    // check for finalized turn
    if (b->polling && b->lh != -1 && b->rh != -1 && !b->lhf && !b->rhf) {
        b->polling = 0;
        task_add(b->tasks,
                creature_animate(&b->p1, 1, 3, 100), task_callback(
                add_gestures(b->p1.data, b->lh, b->rh), task_callback(
                creature_animate(&b->p1, -1, 0, 100), task_callback(
                set_int(&b->polling, 1), 0
                ))));
    }

    // render everything
    SDL_RenderPresent(m->rend);

    return 0;
}

void battle_destroy(struct battlestate *b) {
    task_destroy(b->tasks);
    creature_destroy(&b->p1);
    creature_destroy(&b->p2);
    free(b);
}
