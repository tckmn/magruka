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

#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "magruka.h"

struct particle {
    struct particledata {
        double x, y, a, r, xv, yv, av, rv, xa, ya, aa, ra;
        SDL_Rect img;
        int frame;
    } d;
    struct particle *next;
    struct particle *prev;
};

struct particle *particle_init(void);
void particle_add(struct particle*, struct particledata);
void particle_update(struct particle*);
void particle_draw(struct magruka*, struct particle*);
void particle_destroy(struct particle*);

#endif
