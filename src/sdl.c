#include "misc.h"
#include "sdl.h"
#include "util.h"

struct sdl_state_t sdl_state;

void sdl_init(void)
{
	sdl_state.w = 1024;
	sdl_state.h = 768;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		die("sdl_init: SDL_Init failed: %s\n", SDL_GetError());

	sdl_state.win = SDL_CreateWindow(PROGRAM_FULLNAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sdl_state.w, sdl_state.h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (!sdl_state.win)
		die("sdl_init: SDL_CreateWindow failed: %s\n", SDL_GetError());

	sdl_state.ren = SDL_CreateRenderer(sdl_state.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!sdl_state.ren)
		die("sdl_init: SDL_CreateRenderer failed: %s\n", SDL_GetError());
}

void sdl_teardown(void)
{
	SDL_DestroyWindow(sdl_state.win);
	SDL_Quit();
}
