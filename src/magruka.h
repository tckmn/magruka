#ifndef __MAGRUKA_H__
#define __MAGRUKA_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

struct magruka {
    SDL_Window *win;
    SDL_Surface *surf;
    SDL_Renderer *rend;
    struct {
        SDL_Texture *spritesheet;
        SDL_Rect wizclip;
    } img;
};

struct magruka *magruka_init(void);
void magruka_main_loop(struct magruka*);
void magruka_destroy(struct magruka*);

#endif
