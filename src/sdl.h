#ifndef RAMBAJZ_SDL_H
#define RAMBAJZ_SDL_H

#include <SDL.h>

struct sdl_state_t {
	SDL_Window *win;
	SDL_Renderer *ren;
	int w, h;
};

extern struct sdl_state_t sdl_state;

void sdl_init(void);
void sdl_teardown(void);

#endif
