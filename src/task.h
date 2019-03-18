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

#ifndef __TASK_H__
#define __TASK_H__

struct taskfunc {
    int (*func)(void*, void*, void*);
    void *m;
    void *state;
    void *data;
};

struct task {
    struct taskfunc tf;
    struct task *callback;
    struct task *next;
    struct task *prev;
};

struct task *task_init(void);
void task_add(struct task*, struct taskfunc, struct task*);
struct task *task_callback(struct taskfunc, struct task*);
struct task *task_seq(struct task*, struct task*);
void task_update(struct task*);
void task_destroy(struct task*);

struct taskfunc set_int(int*, int);

#endif
