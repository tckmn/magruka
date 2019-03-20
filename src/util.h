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

#ifndef __UTIL_H__
#define __UTIL_H__

#include "magruka.h"

void draw(struct magruka*, int, int, SDL_Rect);
void drawex(struct magruka*, int, int, SDL_Rect, double, SDL_Point*, SDL_RendererFlip);
void anim(struct magruka*, int, int, SDL_Rect, int);
void animex(struct magruka*, int, int, SDL_Rect, int, double, SDL_Point*, SDL_RendererFlip);
void alph(struct magruka*, Uint8);
/* void write(struct magruka*, int, int, char*); */
struct textimg gentext(struct magruka*, char*, SDL_Color);
void drawtext(struct magruka*, int, int, struct textimg);
int g2n(char);
double rd();
double rd2(double, double);
int ri1(int);
int ri2(int, int);

#endif
