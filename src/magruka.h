#ifndef __MAGRUKA_H__
#define __MAGRUKA_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define FLOOR_POS ((SCREEN_HEIGHT)*7/8)

#define SCALE_FACTOR 4
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
    } img;
};

struct magruka *magruka_init(void);
void magruka_main_loop(struct magruka*);
void magruka_destroy(struct magruka*);

#endif
