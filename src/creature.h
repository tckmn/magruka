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

#ifndef __CREATURE_H__
#define __CREATURE_H__

#include "magruka.h"
#include "task.h"

#define MAX_NAME_LEN 50
#define MAX_SPELL_COUNT 4

struct playerdata {
    int lh[SPELL_BUF+1], rh[SPELL_BUF+1];
    int n;
    struct spell *lhs[MAX_SPELL_COUNT+1], *rhs[MAX_SPELL_COUNT+1];
    int lha, lhb, rha, rhb;
    Uint32 timer;
};

struct creature {
    char name[MAX_NAME_LEN];
    struct textimg nameimg;
    int hp, maxhp;
    struct textimg hpimg, maxhpimg;
    int x, y;
    SDL_Rect img;
    int frame;
    int flip;
    void *data;
};

void creature_init(struct magruka*, struct creature*, char*, int);
void creature_dmg(struct magruka*, struct creature*, int);
void creature_draw(struct magruka*, struct creature*);
void creature_destroy(struct creature*);

struct taskfunc creature_animate(struct creature*, int, int, Uint32);

#endif
