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

#include <stdlib.h>

#include "task.h"

struct task *task_init() {
    struct task *root = malloc(sizeof *root);
    root->next = 0;
    root->callback = 0;
    return root;
}

void task_add(struct task *task, int (*func)(void*), void *data, struct task *callback) {
    while (task->next) task = task->next;
    task->next = malloc(sizeof *task->next);
    task->next->prev = task;
    task->next->next = 0;

    task->next->func = func;
    task->next->data = data;
    task->next->callback = callback;
}

struct task *task_callback(int (*func)(void*), void *data, struct task *callback) {
    struct task *task = malloc(sizeof *task);
    task->func = func;
    task->data = data;
    task->callback = callback;
    return task;
}

void task_update(struct task *task) {
    while ((task = task->next)) {
        if ((*task->func)(task->data)) {
            free(task->data);
            struct task *cb = task->callback;
            if (cb) {
                // note that callback is run the frame *after* original finishes
                task->func = cb->func;
                task->data = cb->data;
                task->callback = cb->callback;
                free(cb);
            } else {
                struct task *prev = task->prev;
                prev->next = task->next;
                if (task->next) task->next->prev = prev;
                free(task);
                task = prev;
            }
        }
    }
}

void task_destroy(struct task *task) {
    while (task) {
        struct task *next = task->next, *cb = task->callback;
        while (cb) {
            struct task *cb2 = cb->callback;
            free(cb->data);
            free(cb);
            cb = cb2;
        }
        free(task->data);
        free(task);
        task = next;
    }
}
