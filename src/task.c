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

void task_add(struct task *task, struct taskfunc tf, struct task *callback) {
    while (task->next) task = task->next;
    task->next = malloc(sizeof *task->next);
    task->next->prev = task;
    task->next->next = 0;

    task->next->tf = tf;
    task->next->callback = callback;
}

struct task *task_callback(struct taskfunc tf, struct task *callback) {
    struct task *task = malloc(sizeof *task);
    task->tf = tf;
    task->callback = callback;
    task->next = task->prev = 0;
    return task;
}

struct task *task_seq(struct task *head, struct task *rest) {
    head->next = rest;
    rest->prev = head;
    return head;
}

void task_update(struct task *task) {
    while ((task = task->next)) {
        if ((*task->tf.func)(task->tf.data)) {
            free(task->tf.data);
            struct task *cb = task->callback;
            if (cb) {
                // note that callback is run the same frame original finishes
                struct task *tail = cb;
                while (tail->next) tail = tail->next;

                cb->prev = task->prev;
                task->prev->next = cb;

                tail->next = task->next;
                if (task->next) task->next->prev = tail;

                free(task);
                task = cb->prev;
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

// TODO this does not free memory from multiple callbacks
void task_destroy(struct task *task) {
    while (task) {
        struct task *next = task->next, *cb = task->callback;
        while (cb) {
            struct task *cb2 = cb->callback;
            free(cb->tf.data);
            free(cb);
            cb = cb2;
        }
        free(task->tf.data);
        free(task);
        task = next;
    }
}

struct set_int_data {
    int *dest, src;
};

int set_int_func(struct set_int_data *sid) {
    *sid->dest = sid->src;
    return 1;
}

struct taskfunc set_int(int *dest, int src) {
    struct set_int_data *sid = malloc(sizeof *sid);
    sid->dest = dest;
    sid->src = src;
    return (struct taskfunc){set_int_func, sid};
}
