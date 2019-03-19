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

#ifndef __BATTLE_H__
#define __BATTLE_H__

#include "magruka.h"
#include "creature.h"

// current and other player
#define CP(b) ((b)->turn == 1 ? (b)->p1 : (b)->p2)
#define CPD(b) ((struct playerdata*)(CP(b).data))
#define OP(b) ((b)->turn == 1 ? (b)->p2 : (b)->p1)
#define OPD(b) ((struct playerdata*)(OP(b).data))

struct battlestate {
    struct task *tasks;
    struct particle *particles;
    int lh, rh, lhf, rhf;
    int page;
    struct creature p1, p2;
    struct creature *mons;
    int nmons;
    int turn;
    int polling, casting;
};

struct battlestate *battle_init(struct magruka*);
int battle_main_loop(struct magruka*, struct battlestate*);
void battle_destroy(struct battlestate*);

#endif
