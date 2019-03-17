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

#include "creature.h"
#include "util.h"

void creature_init(struct magruka *m, struct creature *c, char *name, int hp) {
    // do name
    strncpy(c->name, name, MAX_NAME_LEN);
    c->nameimg = gentext(m, c->name, COLOR_WHITE);

    // do health
    c->hp = c->maxhp = hp;
    char s[12];
    snprintf(s, 12, "%d", hp);
    c->hpimg = gentext(m, s, COLOR_HPTEXT);     // this gets destroyed separately
    c->maxhpimg = gentext(m, s, COLOR_HPTEXT);  // so this needs to be different

    // do data
    c->data = 0;
}

void creature_dmg(struct magruka *m, struct creature *c, int dmg) {
    c->hp -= dmg;
    if (c->hp < 0) c->hp = 0;
    char s[12];
    snprintf(s, 12, "%d", c->hp);
    SDL_DestroyTexture(c->hpimg.texture);
    c->hpimg = gentext(m, s, COLOR_HPTEXT);
}

void creature_draw(struct magruka *m, struct creature *c) {
    if (c->flip) {
        animex(m, c->x, c->y, c->img, c->frame, 0, 0, SDL_FLIP_HORIZONTAL);
    } else {
        anim(m, c->x, c->y, c->img, c->frame);
    }
}

void creature_destroy(struct creature *c) {
    SDL_DestroyTexture(c->hpimg.texture);
    SDL_DestroyTexture(c->maxhpimg.texture);
    free(c->data);
}

struct creature_animate_data {
    struct creature *c;
    int delta, endframe;
    Uint32 speed, timer;
};

int creature_animate_func(struct creature_animate_data *cad) {
    Uint32 now = SDL_GetTicks();
    if (SDL_TICKS_PASSED(now, cad->timer + cad->speed)) {
        if (cad->c->frame == cad->endframe) return 1;
        cad->c->frame += cad->delta;
        cad->timer = now;
    }
    return 0;
}

struct taskfunc creature_animate(struct creature *c, int delta, int endframe, Uint32 speed) {
    struct creature_animate_data *cad = malloc(sizeof *cad);
    cad->c = c;
    cad->delta = delta;
    cad->endframe = endframe;
    cad->speed = speed;
    cad->timer = SDL_GetTicks();
    return (struct taskfunc){creature_animate_func, cad};
}
