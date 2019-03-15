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

#ifndef __MAGRUKA_H__
#define __MAGRUKA_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define FLOOR_POS ((SCREEN_HEIGHT)*7/8)

#define SCALE_FACTOR 4
#define P(x,y)     ((SDL_Point){(x)*SCALE_FACTOR,(y)*SCALE_FACTOR})
#define R(x,y,w,h) ((SDL_Rect){(x)*SCALE_FACTOR,(y)*SCALE_FACTOR,(w)*SCALE_FACTOR,(h)*SCALE_FACTOR})

struct magruka {
    SDL_Window *win;
    SDL_Surface *surf;
    SDL_Renderer *rend;
    struct {
        SDL_Texture *spritesheet;
        SDL_Rect wiz;
        SDL_Rect wall;
        SDL_Rect floor;
        SDL_Rect floortop;
        SDL_Point letters;
        int letterw[26];
        int letterh;
        int letterx[26];
    } img;
};

struct magruka *magruka_init(void);
void magruka_main_loop(struct magruka*);
void magruka_destroy(struct magruka*);

#endif
