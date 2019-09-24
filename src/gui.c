#include "gui.h"
#include <stdbool.h>
#include "SDL_FontCache.h"
#include "analyser.h"
#include "sdl.h"
#include "util.h"

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
			SDL_Keycode key = ev.key.keysym.sym;
			switch (key) {
			case SDLK_q:
			case SDLK_ESCAPE:
				return false;
			case SDLK_j:
				width /= zoom;
				break;
			case SDLK_k:
				width *= zoom;
				break;
			case SDLK_h:
				center -= width * shift;
				break;
			case SDLK_l:
				center += width * shift;
				break;
			case SDLK_d:
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
	return true;
}

void plot_interval(double x0, double x1, double y) {
	x0 = fmin(1, fmax(0, x0));
	x1 = fmin(1, fmax(0, x1));
	if ((x0 <= 0 && x1 <= 0) || (x0 >= 1 && x1 >= 1))
		return;

	SDL_Rect rect;
	rect.h = round(sdl_state.h * y);
	rect.y = sdl_state.h - rect.h;
	rect.x = round(sdl_state.w * x0);
	rect.w = round(sdl_state.w * x1) - rect.x;
	rect.w = (rect.w <= 0) ? 1 : rect.w;
	SDL_RenderFillRect(sdl_state.ren, &rect);
}

void draw_plot(size_t n, const struct point *data, double A, double B)
{
	for (int i = 0; i < n; i++) {
		double x0 = (i) ? data[i - 1].x : 0;
		double x1 = data[i].x;
		double y = sqrt(data[i].y);
		plot_interval(logscale(x0, A, B), logscale(x1, A, B), y);
	}
}

void draw_note(const struct analysis_data *data)
{
	static FC_Font *font = NULL;
	if (!font) {
		font = FC_CreateFont();
		FC_LoadFont(font, sdl_state.ren, "/usr/share/fonts/TTF/DejaVuSans.ttf", 50, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
	}

	double split = (data->guessed_tone.cents + 50) / 100;
	SDL_Rect rect = {.h = 60, .w = sdl_state.w, .x = 0, .y = 0};
	SDL_SetRenderDrawColor(sdl_state.ren, 0, 127, 0, 255);
	SDL_RenderFillRect(sdl_state.ren, &rect);
	rect.w = fabs(split - .5) * sdl_state.w;
	rect.x = fmin(split, .5) * sdl_state.w;
	SDL_SetRenderDrawColor(sdl_state.ren, 0, 255, 0, 255);
	SDL_RenderFillRect(sdl_state.ren, &rect);

	FC_DrawAlign(font, sdl_state.ren, 0, 0, FC_ALIGN_LEFT, "%s (%+d cents)", tone_name(data->guessed_tone), (int)round(data->guessed_tone.cents));
	FC_DrawAlign(font, sdl_state.ren, sdl_state.w, 0, FC_ALIGN_RIGHT, "%.2f Hz", data->guessed_frequency);
}

void draw(const struct analysis_data *data, const struct analysis_params *params) {
	SDL_SetRenderDrawColor(sdl_state.ren, 0, 0, 0, 255);
	SDL_RenderClear(sdl_state.ren);

	SDL_SetRenderDrawColor(sdl_state.ren, 255, 255, 255, 255);
	draw_plot(data->plot_size, data->plot, params->min_freq, params->max_freq);
	SDL_SetRenderDrawColor(sdl_state.ren, 255, 0, 0, 255);
	double guessed = logscale(data->guessed_frequency, params->min_freq, params->max_freq);
	plot_interval(guessed, guessed, 1);
	draw_note(data);
	SDL_RenderPresent(sdl_state.ren);
}
