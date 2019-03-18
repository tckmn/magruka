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

#include "particle.h"
#include "util.h"

struct particle *particle_init() {
    struct particle *p = malloc(sizeof *p);
    p->next = 0;
    return p;
}

void particle_add(struct particle *p, struct particledata d) {
    struct particle *n = malloc(sizeof *n);
    n->d = d;

    if (p->next) p->next->prev = n;
    n->next = p->next;
    n->prev = p;
    p->next = n;
}

void particle_update(struct particle *p) {
    while ((p = p->next)) {
        p->d.x  += p->d.xv; p->d.y  += p->d.yv; p->d.a  += p->d.av;
        p->d.xv += p->d.xa; p->d.yv += p->d.ya; p->d.av += p->d.aa;
        if (p->d.a <= 0) {
            p->prev->next = p->next;
            if (p->next) p->next->prev = p->prev;
            struct particle *prev = p->prev;
            free(p);
            p = prev;
        }
    }
}

void particle_draw(struct magruka *m, struct particle *p) {
    while ((p = p->next)) {
        alph(m, p->d.a*0xff);
        anim(m, p->d.x, p->d.y, p->d.img, p->d.frame);
    }
    alph(m, 0xff);
}

void particle_destroy(struct particle *p) {
    while (p) {
        struct particle *next = p->next;
        free(p);
        p = next;
    }
}
