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

struct task {
    int (*func)(void*);
    void *data;
    struct task *callback;
    struct task *next;
    struct task *prev;
};

struct task *task_init(void);
void task_add(struct task*, int (*)(void*), void*, struct task*);
struct task *task_callback(int (*)(void*), void*, struct task*);
void task_update(struct task*);
void task_destroy(struct task*);

struct set_int_data {
    int *dest, src;
};
int set_int(struct set_int_data*);
struct set_int_data* set_int_new(int*, int);

#endif
