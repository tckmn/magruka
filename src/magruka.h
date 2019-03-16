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
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540

#define FLOOR_POS ((SCREEN_HEIGHT)*7/8)

#define SCALE1 4
#define SCALE2 2
#define HEIGHT1 (185*SCALE1)
#define P1(x,y)      ((SDL_Point){(x)*SCALE1,(y)*SCALE1})
#define R1(x,y,w,h)  ((SDL_Rect){(x)*SCALE1,(y)*SCALE1,(w)*SCALE1,(h)*SCALE1})
#define P2(x,y)      ((SDL_Point){(x)*SCALE2,HEIGHT1+(y)*SCALE2})
#define R2(x,y,w,h)  ((SDL_Rect){(x)*SCALE2,HEIGHT1+(y)*SCALE2,(w)*SCALE2,(h)*SCALE2})

struct textimg {
    SDL_Texture *texture;
    int w;
    int h;
};

struct spell {
    char name[30];
    struct textimg nameimg;
    char gesture[9];
    int damage;
};

struct magruka {
    SDL_Window *win;
    SDL_Surface *surf;
    SDL_Renderer *rend;
    TTF_Font *font;
    struct {
        SDL_Texture *spritesheet;
        SDL_Rect wiz;
        SDL_Rect wall;
        SDL_Rect floor;
        SDL_Rect floortop;
        SDL_Rect gesture;
        SDL_Rect gesturefinal;
        SDL_Point letters;
        int letterw[62];
        int letterh;
        int letterx[62];
    } img;
    struct spell *spells;
};

struct magruka *magruka_init(void);
void magruka_main_loop(struct magruka*);
void magruka_destroy(struct magruka*);

#endif
