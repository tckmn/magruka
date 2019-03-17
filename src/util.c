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

#include "util.h"

void draw(struct magruka *m, int x, int y, SDL_Rect clip) {
    SDL_Rect dest = {x, y, clip.w, clip.h};
    SDL_RenderCopy(m->rend, m->img.spritesheet, &clip, &dest);
}

void drawex(struct magruka *m, int x, int y, SDL_Rect clip,
        double angle, SDL_Point *center, SDL_RendererFlip flip) {
    SDL_Rect dest = {x, y, clip.w, clip.h};
    SDL_RenderCopyEx(m->rend, m->img.spritesheet, &clip, &dest, angle, center, flip);
}

void anim(struct magruka *m, int x, int y, SDL_Rect clip, int offs) {
    SDL_Rect src = {clip.x + offs*clip.w, clip.y, clip.w, clip.h};
    SDL_Rect dest = {x, y, clip.w, clip.h};
    SDL_RenderCopy(m->rend, m->img.spritesheet, &src, &dest);
}

void animex(struct magruka *m, int x, int y, SDL_Rect clip, int offs,
        double angle, SDL_Point *center, SDL_RendererFlip flip) {
    SDL_Rect src = {clip.x + offs*clip.w, clip.y, clip.w, clip.h};
    SDL_Rect dest = {x, y, clip.w, clip.h};
    SDL_RenderCopyEx(m->rend, m->img.spritesheet, &src, &dest, angle, center, flip);
}

void write(struct magruka *m, int x, int y, char *text) {
    SDL_Rect src = {m->img.letters.x, m->img.letters.y, 0, m->img.letterh};
    SDL_Rect dest = {x, y, 0, src.h};
    for (; *text; ++text) {
        if (*text == ' ') {
            dest.x += 2*SCALE1 + SCALE1/2;
        } else if (*text == '\n') {
            dest.x = x;
            dest.y += m->img.letterh;
        } else {
            int ch = 'A' <= *text && *text <= 'Z' ? *text - 'A' :
                     'a' <= *text && *text <= 'z' ? *text - 'a' + 26 :
                     '0' <= *text && *text <= '9' ? *text - '0' + 52 : 0;
            src.x = m->img.letters.x + m->img.letterx[ch];
            src.w = dest.w = m->img.letterw[ch];
            SDL_RenderCopy(m->rend, m->img.spritesheet, &src, &dest);
            dest.x += dest.w + SCALE1/2;
        }
    }
}

/*
 * returns a textimg with a NULL texture on error
 */
struct textimg gentext(struct magruka *m, char *text, SDL_Color c) {
    struct textimg img;
    img.texture = 0;
    SDL_Surface *tmp = TTF_RenderText_Solid(m->font, text, c);
    if (!tmp) {
        fprintf(stderr, "could not render text image\n(SDL/TTF error: %s)\n",
                TTF_GetError());
        return img;
    }
    img.w = tmp->w;
    img.h = tmp->h;
    img.texture = SDL_CreateTextureFromSurface(m->rend, tmp);
    SDL_FreeSurface(tmp);
    if (!img.texture) {
        fprintf(stderr, "could not optimize text image\n(SDL error: %s)\n",
                SDL_GetError());
    }
    return img;
}

void drawtext(struct magruka *m, int x, int y, struct textimg text) {
    SDL_Rect src = {0, 0, text.w, text.h};
    SDL_Rect dest = {x, y, text.w, text.h};
    SDL_RenderCopy(m->rend, text.texture, &src, &dest);
}

/*
 * convert gesture (e.g. 'W') to number (e.g. GESTURE_W)
 * lowercase (both hands) becomes negative
 */
int g2n(char ch) {
    switch (ch) {
        case 'C': return GESTURE_C;
        case 'D': return GESTURE_D;
        case 'F': return GESTURE_F;
        case 'P': return GESTURE_P;
        case 'S': return GESTURE_S;
        case 'W': return GESTURE_W;
        case 'c': return -GESTURE_C;
        case 'd': return -GESTURE_D;
        case 'f': return -GESTURE_F;
        case 'p': return -GESTURE_P;
        case 's': return -GESTURE_S;
        case 'w': return -GESTURE_W;
    }
    return 0;
}
