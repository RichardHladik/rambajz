#include "gui.h"
#include <stdbool.h>
#include "analyser.h"
#include "sdl.h"

bool interact(struct analysis_params *params, struct viewport_t *viewport, const struct viewport_t *limits){
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT)
			return false;

		if (ev.type == SDL_WINDOWEVENT) {
			if (ev.window.event == SDL_WINDOWEVENT_RESIZED || ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				sdl_state.w = ev.window.data1, sdl_state.h = ev.window.data2;
		}

		const int blacklist = KMOD_MODE | KMOD_CTRL | KMOD_SHIFT | KMOD_ALT | KMOD_GUI;
		const int modstate = SDL_GetModState(); // FIXME: Hack
		double center = (viewport->A + viewport->B) / 2;
		double width = viewport->B - viewport->A;
		if (ev.type == SDL_KEYDOWN && (modstate & blacklist) == 0) {
			static const double zoom = 2;
			static const double shift = 1/4.;
			SDL_Scancode key = ev.key.keysym.scancode;
			switch (key) {
			case SDL_SCANCODE_J:
				width /= zoom;
				break;
			case SDL_SCANCODE_K:
				width *= zoom;
				break;
			case SDL_SCANCODE_H:
				center -= width * shift;
				break;
			case SDL_SCANCODE_L:
				center += width * shift;
				break;
			case SDL_SCANCODE_D:
				params->dist = (params->dist + 1) % DISTRIBUTION_CNT;
				break;
			default:
				break;
			}
		}

		if (ev.type == SDL_MOUSEWHEEL) {
			static const double xf = 1/32.;
			static const double yf = 1/8.;
			double x = -ev.wheel.x * xf;
			double y = -ev.wheel.y * yf;
			center += width * x;
			width *= exp(y);
		}

		width = fmin(width, limits->B - limits->A);
		center = fmax(fmin(center, limits->B - width / 2), limits->A + width / 2);
		viewport->A = center - width / 2;
		viewport->B = center + width / 2;
	}

	params->min_freq = exp(viewport->A);
	params->max_freq = exp(viewport->B);
}
