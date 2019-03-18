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
#include "particle.h"

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

#define DRAWGEST_PAD 4
#define DRAWGEST_LEN 15
int drawgest(struct magruka *m, int x, int y, struct playerdata *p) {
    int y1 = y, y2 = y + m->img.handind.h + DRAWGEST_PAD;

    anim(m, x + m->img.handind.w/2, y1, m->img.handind, 1);
    anim(m, x + m->img.handind.w/2, y2, m->img.handind, 0);

    int xpos = x + m->img.handind.w*2;
    // for animations
    int diff = SDL_GetTicks() - p->timer;
    if (diff > GESTURE_DURATION) diff = GESTURE_DURATION;

    // slide all gestures out if we've reached the cap
    if (p->timer && p->n > SPELL_BUF) {
        xpos -= diff * (m->img.key.w + DRAWGEST_PAD) / GESTURE_DURATION;
        alph(m, 0xff - diff * 0xff / GESTURE_DURATION);
    }

    for (int i = 0; i < p->n; ++i) {
        if (p->timer && i + 1 == p->n) {
            xpos += DRAWGEST_LEN - diff * DRAWGEST_LEN / GESTURE_DURATION;
            alph(m, diff * 0xff / GESTURE_DURATION);
        }
        anim(m, xpos, y1, m->img.key, p->lh[i]);
        anim(m, xpos, y2, m->img.key, p->rh[i]);
        xpos += m->img.key.w + DRAWGEST_PAD;
        if (p->timer && !i) alph(m, 0xff);
    }
    if (p->timer) alph(m, 0xff);

    return y2 + m->img.handind.h;
}

#define EXPLODE_PARAMS 1, 0, rd2(-2,2), rd2(-3,0), -0.01, rd2(-0.01,0.01), 0, 0.1, 0, 0
void explode_c(struct magruka *m, struct particle *particles, int xpos) {
    particle_add(particles, (struct particledata){xpos + SCALE1*15, HAND_Y - SCALE1*1,  EXPLODE_PARAMS, m->img.c_particles, 0});
    particle_add(particles, (struct particledata){xpos + SCALE1*9,  HAND_Y + SCALE1*0,  EXPLODE_PARAMS, m->img.c_particles, 1});
    particle_add(particles, (struct particledata){xpos + SCALE1*4,  HAND_Y + SCALE1*2,  EXPLODE_PARAMS, m->img.c_particles, 2});
    particle_add(particles, (struct particledata){xpos + SCALE1*1,  HAND_Y + SCALE1*6,  EXPLODE_PARAMS, m->img.c_particles, 3});
    particle_add(particles, (struct particledata){xpos + SCALE1*1,  HAND_Y + SCALE1*11, EXPLODE_PARAMS, m->img.c_particles, 4});
    particle_add(particles, (struct particledata){xpos + SCALE1*1,  HAND_Y + SCALE1*19, EXPLODE_PARAMS, m->img.c_particles, 5});
    particle_add(particles, (struct particledata){xpos + SCALE1*5,  HAND_Y + SCALE1*22, EXPLODE_PARAMS, m->img.c_particles, 6});
    particle_add(particles, (struct particledata){xpos + SCALE1*12, HAND_Y + SCALE1*21, EXPLODE_PARAMS, m->img.c_particles, 7});
    particle_add(particles, (struct particledata){xpos + SCALE1*16, HAND_Y + SCALE1*19, EXPLODE_PARAMS, m->img.c_particles, 8});
}
void explode_d(struct magruka *m, struct particle *particles, int xpos) {
    particle_add(particles, (struct particledata){xpos + SCALE1*0,  HAND_Y - SCALE1*2,  EXPLODE_PARAMS, m->img.d_particles, 0});
    particle_add(particles, (struct particledata){xpos + SCALE1*6,  HAND_Y - SCALE1*2,  EXPLODE_PARAMS, m->img.d_particles, 1});
    particle_add(particles, (struct particledata){xpos + SCALE1*11, HAND_Y + SCALE1*0,  EXPLODE_PARAMS, m->img.d_particles, 2});
    particle_add(particles, (struct particledata){xpos + SCALE1*16, HAND_Y + SCALE1*2,  EXPLODE_PARAMS, m->img.d_particles, 3});
    particle_add(particles, (struct particledata){xpos + SCALE1*16, HAND_Y + SCALE1*10, EXPLODE_PARAMS, m->img.d_particles, 4});
    particle_add(particles, (struct particledata){xpos + SCALE1*14, HAND_Y + SCALE1*16, EXPLODE_PARAMS, m->img.d_particles, 5});
    particle_add(particles, (struct particledata){xpos + SCALE1*7,  HAND_Y + SCALE1*20, EXPLODE_PARAMS, m->img.d_particles, 6});
    particle_add(particles, (struct particledata){xpos + SCALE1*2,  HAND_Y + SCALE1*19, EXPLODE_PARAMS, m->img.d_particles, 7});
    particle_add(particles, (struct particledata){xpos + SCALE1*2,  HAND_Y + SCALE1*11, EXPLODE_PARAMS, m->img.d_particles, 8});
    particle_add(particles, (struct particledata){xpos + SCALE1*0,  HAND_Y + SCALE1*5,  EXPLODE_PARAMS, m->img.d_particles, 9});
}
void explode_f(struct magruka *m, struct particle *particles, int xpos) {
    particle_add(particles, (struct particledata){xpos + SCALE1*16, HAND_Y - SCALE1*1,  EXPLODE_PARAMS, m->img.f_particles, 0});
    particle_add(particles, (struct particledata){xpos + SCALE1*9,  HAND_Y - SCALE1*0,  EXPLODE_PARAMS, m->img.f_particles, 1});
    particle_add(particles, (struct particledata){xpos + SCALE1*1,  HAND_Y - SCALE1*1,  EXPLODE_PARAMS, m->img.f_particles, 2});
    particle_add(particles, (struct particledata){xpos + SCALE1*4,  HAND_Y + SCALE1*2,  EXPLODE_PARAMS, m->img.f_particles, 3});
    particle_add(particles, (struct particledata){xpos + SCALE1*4,  HAND_Y + SCALE1*7,  EXPLODE_PARAMS, m->img.f_particles, 4});
    particle_add(particles, (struct particledata){xpos + SCALE1*9,  HAND_Y + SCALE1*11, EXPLODE_PARAMS, m->img.f_particles, 5});
    particle_add(particles, (struct particledata){xpos + SCALE1*14, HAND_Y + SCALE1*11, EXPLODE_PARAMS, m->img.f_particles, 6});
    particle_add(particles, (struct particledata){xpos + SCALE1*3,  HAND_Y + SCALE1*14, EXPLODE_PARAMS, m->img.f_particles, 7});
    particle_add(particles, (struct particledata){xpos + SCALE1*2,  HAND_Y + SCALE1*20, EXPLODE_PARAMS, m->img.f_particles, 8});
}
void explode_p(struct magruka *m, struct particle *particles, int xpos) {
    particle_add(particles, (struct particledata){xpos + SCALE1*2,  HAND_Y - SCALE1*0,  EXPLODE_PARAMS, m->img.p_particles, 0});
    particle_add(particles, (struct particledata){xpos + SCALE1*8,  HAND_Y - SCALE1*1,  EXPLODE_PARAMS, m->img.p_particles, 1});
    particle_add(particles, (struct particledata){xpos + SCALE1*14, HAND_Y - SCALE1*1,  EXPLODE_PARAMS, m->img.p_particles, 2});
    particle_add(particles, (struct particledata){xpos + SCALE1*17, HAND_Y + SCALE1*2,  EXPLODE_PARAMS, m->img.p_particles, 3});
    particle_add(particles, (struct particledata){xpos + SCALE1*15, HAND_Y + SCALE1*9,  EXPLODE_PARAMS, m->img.p_particles, 4});
    particle_add(particles, (struct particledata){xpos + SCALE1*10, HAND_Y + SCALE1*10, EXPLODE_PARAMS, m->img.p_particles, 5});
    particle_add(particles, (struct particledata){xpos + SCALE1*3,  HAND_Y + SCALE1*7,  EXPLODE_PARAMS, m->img.p_particles, 6});
    particle_add(particles, (struct particledata){xpos + SCALE1*3,  HAND_Y + SCALE1*12, EXPLODE_PARAMS, m->img.p_particles, 7});
    particle_add(particles, (struct particledata){xpos + SCALE1*2,  HAND_Y + SCALE1*17, EXPLODE_PARAMS, m->img.p_particles, 8});
    particle_add(particles, (struct particledata){xpos + SCALE1*2,  HAND_Y + SCALE1*21, EXPLODE_PARAMS, m->img.p_particles, 9});
}
void explode_s(struct magruka *m, struct particle *particles, int xpos) {
    particle_add(particles, (struct particledata){xpos + SCALE1*15, HAND_Y - SCALE1*1,  EXPLODE_PARAMS, m->img.s_particles, 0});
    particle_add(particles, (struct particledata){xpos + SCALE1*8,  HAND_Y - SCALE1*2,  EXPLODE_PARAMS, m->img.s_particles, 1});
    particle_add(particles, (struct particledata){xpos + SCALE1*3,  HAND_Y + SCALE1*1,  EXPLODE_PARAMS, m->img.s_particles, 2});
    particle_add(particles, (struct particledata){xpos + SCALE1*4,  HAND_Y + SCALE1*6,  EXPLODE_PARAMS, m->img.s_particles, 3});
    particle_add(particles, (struct particledata){xpos + SCALE1*7,  HAND_Y + SCALE1*10, EXPLODE_PARAMS, m->img.s_particles, 4});
    particle_add(particles, (struct particledata){xpos + SCALE1*14, HAND_Y + SCALE1*11, EXPLODE_PARAMS, m->img.s_particles, 5});
    particle_add(particles, (struct particledata){xpos + SCALE1*15, HAND_Y + SCALE1*17, EXPLODE_PARAMS, m->img.s_particles, 6});
    particle_add(particles, (struct particledata){xpos + SCALE1*14, HAND_Y + SCALE1*20, EXPLODE_PARAMS, m->img.s_particles, 7});
    particle_add(particles, (struct particledata){xpos + SCALE1*8,  HAND_Y + SCALE1*21, EXPLODE_PARAMS, m->img.s_particles, 8});
    particle_add(particles, (struct particledata){xpos + SCALE1*2,  HAND_Y + SCALE1*21, EXPLODE_PARAMS, m->img.s_particles, 9});
}
void explode_w(struct magruka *m, struct particle *particles, int xpos) {
    particle_add(particles, (struct particledata){xpos - SCALE1*1,  HAND_Y + SCALE1*0,  EXPLODE_PARAMS, m->img.w_particles, 0});
    particle_add(particles, (struct particledata){xpos - SCALE1*1,  HAND_Y + SCALE1*8,  EXPLODE_PARAMS, m->img.w_particles, 1});
    particle_add(particles, (struct particledata){xpos + SCALE1*0,  HAND_Y + SCALE1*14, EXPLODE_PARAMS, m->img.w_particles, 2});
    particle_add(particles, (struct particledata){xpos + SCALE1*4,  HAND_Y + SCALE1*20, EXPLODE_PARAMS, m->img.w_particles, 3});
    particle_add(particles, (struct particledata){xpos + SCALE1*8,  HAND_Y + SCALE1*10, EXPLODE_PARAMS, m->img.w_particles, 4});
    particle_add(particles, (struct particledata){xpos + SCALE1*9,  HAND_Y + SCALE1*18, EXPLODE_PARAMS, m->img.w_particles, 5});
    particle_add(particles, (struct particledata){xpos + SCALE1*15, HAND_Y + SCALE1*20, EXPLODE_PARAMS, m->img.w_particles, 6});
    particle_add(particles, (struct particledata){xpos + SCALE1*17, HAND_Y + SCALE1*13, EXPLODE_PARAMS, m->img.w_particles, 7});
    particle_add(particles, (struct particledata){xpos + SCALE1*17, HAND_Y + SCALE1*6,  EXPLODE_PARAMS, m->img.w_particles, 8});
    particle_add(particles, (struct particledata){xpos + SCALE1*17, HAND_Y + SCALE1*1,  EXPLODE_PARAMS, m->img.w_particles, 9});
}
void explode_stab(struct magruka *m, struct particle *particles, int xpos) {
    particle_add(particles, (struct particledata){xpos + SCALE1*9,  HAND_Y + SCALE1*3,  EXPLODE_PARAMS, m->img.stab_particles, 0});
    particle_add(particles, (struct particledata){xpos + SCALE1*0,  HAND_Y + SCALE1*10, EXPLODE_PARAMS, m->img.stab_particles, 1});
    particle_add(particles, (struct particledata){xpos + SCALE1*7,  HAND_Y + SCALE1*10, EXPLODE_PARAMS, m->img.stab_particles, 2});
    particle_add(particles, (struct particledata){xpos + SCALE1*17, HAND_Y + SCALE1*11, EXPLODE_PARAMS, m->img.stab_particles, 3});
    particle_add(particles, (struct particledata){xpos + SCALE1*11, HAND_Y + SCALE1*11, EXPLODE_PARAMS, m->img.stab_particles, 4});
    particle_add(particles, (struct particledata){xpos + SCALE1*8,  HAND_Y + SCALE1*15, EXPLODE_PARAMS, m->img.stab_particles, 5});
    particle_add(particles, (struct particledata){xpos + SCALE1*8,  HAND_Y + SCALE1*21, EXPLODE_PARAMS, m->img.stab_particles, 6});
}
void explode_none(struct magruka *m, struct particle *particles, int xpos) {
    particle_add(particles, (struct particledata){xpos + SCALE1*0,  HAND_Y + SCALE1*12, EXPLODE_PARAMS, m->img.none_particles, 0});
    particle_add(particles, (struct particledata){xpos + SCALE1*6,  HAND_Y + SCALE1*9,  EXPLODE_PARAMS, m->img.none_particles, 1});
    particle_add(particles, (struct particledata){xpos + SCALE1*5,  HAND_Y + SCALE1*14, EXPLODE_PARAMS, m->img.none_particles, 2});
    particle_add(particles, (struct particledata){xpos + SCALE1*12, HAND_Y + SCALE1*12, EXPLODE_PARAMS, m->img.none_particles, 3});
    particle_add(particles, (struct particledata){xpos + SCALE1*17, HAND_Y + SCALE1*12, EXPLODE_PARAMS, m->img.none_particles, 4});
}
void explode(struct magruka *m, struct particle *particles, int xpos, int g) {
    switch (g) {
    case 0: explode_c(m, particles, xpos); break;
    case 1: explode_d(m, particles, xpos); break;
    case 2: explode_f(m, particles, xpos); break;
    case 3: explode_p(m, particles, xpos); break;
    case 4: explode_s(m, particles, xpos); break;
    case 5: explode_w(m, particles, xpos); break;
    case 6: explode_stab(m, particles, xpos); break;
    case 7: explode_none(m, particles, xpos); break;
    }
}

#define NPOOFS 8
void dopoof(struct magruka *m, struct particle *particles, int xpos, int finalized) {
    for (int f = 0; f < 3; ++f) {
        for (int i = 0; i < NPOOFS; ++i) {
            particle_add(particles, (struct particledata){
                    xpos + ri1(m->img.gesture.w),
                    HAND_Y + ri1(m->img.gesture.h),
                    EXPLODE_PARAMS,
                    finalized ? m->img.gfparticles : m->img.gparticles, f
                    });
        }
    }
}

struct add_gestures_data {
    struct playerdata *pd;
};

int add_gestures_func(struct magruka *m, struct battlestate *b, struct add_gestures_data *agd) {
    if (agd->pd->timer == 0) {
        agd->pd->timer = SDL_GetTicks();
        agd->pd->lh[agd->pd->n] = b->lh;
        agd->pd->rh[agd->pd->n] = b->rh;
        ++agd->pd->n;

        explode(m, b->particles, LH_POS(m), b->lh);
        explode(m, b->particles, RH_POS(m), b->rh);

        b->lh = -1; b->rh = -1;
    } else if (SDL_TICKS_PASSED(SDL_GetTicks(), agd->pd->timer + GESTURE_DURATION)) {
        agd->pd->timer = 0;
        if (agd->pd->n > SPELL_BUF) {
            --agd->pd->n;
            for (int i = 0; i < SPELL_BUF; ++i) {
                agd->pd->lh[i] = agd->pd->lh[i+1];
                agd->pd->rh[i] = agd->pd->rh[i+1];
            }
        }
        return 1;
    }
    return 0;
}

struct taskfunc add_gestures(struct magruka *m, struct battlestate *b, struct playerdata *pd) {
    struct add_gestures_data *agd = malloc(sizeof *agd);
    agd->pd = pd;
    return (struct taskfunc){add_gestures_func, m, b, agd};
}

struct battlestate *battle_init(struct magruka *m) {
    struct battlestate *b = malloc(sizeof *b);

    b->tasks = task_init();
    b->particles = particle_init();

    b->lh = b->rh = -1;
    b->lhf = b->rhf = 0;
    b->page = 0;

    creature_init(m, &b->p1, "Player 1", 15);
    creature_init(m, &b->p2, "Player 2", 15);
    struct playerdata *pd1 = malloc(sizeof *pd1),
                      *pd2 = malloc(sizeof *pd2);
    pd1->n = pd2->n = 0;
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
    particle_update(b->particles);

    int poof = 0;

    while (SDL_PollEvent(&m->e)) {
        if (m->e.type == SDL_KEYDOWN && m->e.key.repeat) continue;

        switch (m->e.type) {

        case SDL_QUIT:
            return -1;

        case SDL_KEYDOWN:
            if (m->e.key.keysym.sym == SDLK_ESCAPE) return -1;
            if (b->polling) {
                switch (m->e.key.keysym.sym) {

                case 'q': b->lhf |= 1 << GESTURE_STAB; b->lh = GESTURE_STAB; poof |= 1; break;
                case 'w': b->lhf |= 1 << GESTURE_W;    b->lh = GESTURE_W;    poof |= 1; break;
                case 'e': b->lhf |= 1 << GESTURE_C;    b->lh = GESTURE_C;    poof |= 1; break;
                case 'r': b->lhf |= 1 << GESTURE_P;    b->lh = GESTURE_P;    poof |= 1; break;
                case 'a': b->lhf |= 1 << GESTURE_NONE; b->lh = GESTURE_NONE; poof |= 1; break;
                case 's': b->lhf |= 1 << GESTURE_S;    b->lh = GESTURE_S;    poof |= 1; break;
                case 'd': b->lhf |= 1 << GESTURE_D;    b->lh = GESTURE_D;    poof |= 1; break;
                case 'f': b->lhf |= 1 << GESTURE_F;    b->lh = GESTURE_F;    poof |= 1; break;

                case 'p': b->rhf |= 1 << GESTURE_STAB; b->rh = GESTURE_STAB; poof |= 2; break;
                case 'o': b->rhf |= 1 << GESTURE_W;    b->rh = GESTURE_W;    poof |= 2; break;
                case 'i': b->rhf |= 1 << GESTURE_C;    b->rh = GESTURE_C;    poof |= 2; break;
                case 'u': b->rhf |= 1 << GESTURE_P;    b->rh = GESTURE_P;    poof |= 2; break;
                case ';': b->rhf |= 1 << GESTURE_NONE; b->rh = GESTURE_NONE; poof |= 2; break;
                case 'l': b->rhf |= 1 << GESTURE_S;    b->rh = GESTURE_S;    poof |= 2; break;
                case 'k': b->rhf |= 1 << GESTURE_D;    b->rh = GESTURE_D;    poof |= 2; break;
                case 'j': b->rhf |= 1 << GESTURE_F;    b->rh = GESTURE_F;    poof |= 2; break;

                case '.': if (b->page < m->nspells / SPELLS_PER_PAGE - 1) ++b->page; break;
                case ',': if (b->page > 0) --b->page; break;

                case ' ': b->lhf = b->rhf = 0; b->lh = b->rh = -1; break;

                }
            }
            break;

        case SDL_KEYUP:
            if (b->polling) {
                switch (m->e.key.keysym.sym) {

                case 'q': b->lhf &= ~(1 << GESTURE_STAB); poof |= 4; break;
                case 'w': b->lhf &= ~(1 << GESTURE_W);    poof |= 4; break;
                case 'e': b->lhf &= ~(1 << GESTURE_C);    poof |= 4; break;
                case 'r': b->lhf &= ~(1 << GESTURE_P);    poof |= 4; break;
                case 'a': b->lhf &= ~(1 << GESTURE_NONE); poof |= 4; break;
                case 's': b->lhf &= ~(1 << GESTURE_S);    poof |= 4; break;
                case 'd': b->lhf &= ~(1 << GESTURE_D);    poof |= 4; break;
                case 'f': b->lhf &= ~(1 << GESTURE_F);    poof |= 4; break;

                case 'p': b->rhf &= ~(1 << GESTURE_STAB); poof |= 8; break;
                case 'o': b->rhf &= ~(1 << GESTURE_W);    poof |= 8; break;
                case 'i': b->rhf &= ~(1 << GESTURE_C);    poof |= 8; break;
                case 'u': b->rhf &= ~(1 << GESTURE_P);    poof |= 8; break;
                case ';': b->rhf &= ~(1 << GESTURE_NONE); poof |= 8; break;
                case 'l': b->rhf &= ~(1 << GESTURE_S);    poof |= 8; break;
                case 'k': b->rhf &= ~(1 << GESTURE_D);    poof |= 8; break;
                case 'j': b->rhf &= ~(1 << GESTURE_F);    poof |= 8; break;

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

        // draw poofy particles, if any
        if (poof & 1) dopoof(m, b->particles, LH_POS(m), 0);
        if (poof & 2) dopoof(m, b->particles, RH_POS(m), 0);
        if ((poof & 4) && !b->lhf) dopoof(m, b->particles, LH_POS(m), 1);
        if ((poof & 8) && !b->rhf) dopoof(m, b->particles, RH_POS(m), 1);
    }

    // draw wizards
    creature_draw(m, &b->p1);
    creature_draw(m, &b->p2);

    // draw health bars
    ypos = 5;
    ypos = drawhpbar(m, 5, ypos, 0, b->p1, -1) + STATUS_PAD;
    ypos = drawgest(m, 5, ypos, b->p1.data) + STATUS_PAD;
    drawhpbar(m, SCREEN_WIDTH - 5, 5, 1, b->p2, -1);

    // draw held gestures
    if (b->lh != -1) anim(m, LH_POS(m), HAND_Y, b->lhf ? m->img.gesture : m->img.gesturefinal, b->lh);
    if (b->rh != -1) anim(m, RH_POS(m), HAND_Y, b->rhf ? m->img.gesture : m->img.gesturefinal, b->rh);

    // draw particles
    particle_draw(m, b->particles);

    // check for finalized turn
    if (b->polling && b->lh != -1 && b->rh != -1 && !b->lhf && !b->rhf) {
        b->polling = 0;
        task_add(b->tasks,
                creature_animate(&b->p1, 1, 3, 100), task_callback(
                add_gestures(m, b, b->p1.data), task_callback(
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
    particle_destroy(b->particles);
    creature_destroy(&b->p1);
    creature_destroy(&b->p2);
    free(b);
}
