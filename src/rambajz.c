#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "analyser.h"
#include "buffer.h"
#include "jack.h"
#include "sdl.h"

const size_t BUFSIZE = (1 << 20);

// Assumption: nframes is constant throughout execution
int record(jack_nframes_t nframes, void *arg)
{
	struct buffer *buf = arg;
	jack_default_audio_sample_t *in, *out;
	in = jack_port_get_buffer(jack_state.in_port, nframes);
	out = jack_port_get_buffer(jack_state.out_port, nframes);
	double in_double[nframes];
	for (int i = 0; i < nframes; i++)
		in_double[i] = in[i];

	buffer_push(buf, in_double, nframes);

	for (int i = 0; i < nframes; i++)
		out[i] = 0;

	return 0;
}

void draw(size_t n, struct point *data);

bool process(struct buffer *buf)
{
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT)
			return false;

		if (ev.type == SDL_WINDOWEVENT) {
			if (ev.window.event == SDL_WINDOWEVENT_RESIZED || ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				sdl_state.w = ev.window.data1, sdl_state.h = ev.window.data2;
		}
	}

	int e = buf->e;
	printf("%d\n", e);

	struct analysis_data data;
	if (!analyse(&data, buf))
		return true;

	SDL_Rect rect = {.h = sdl_state.h, .w = sdl_state.w, .x = 0, .y = 0};
	SDL_SetRenderDrawColor(sdl_state.ren, 0, 0, 0, 255);
	SDL_RenderClear(sdl_state.ren);

	SDL_SetRenderDrawColor(sdl_state.ren, 255, 255, 255, 255);
	draw(data.plot_size, data.plot);
	SDL_RenderPresent(sdl_state.ren);

	analysis_free(data);
	return true;
}

void draw(size_t n, struct point *data)
{
	for (size_t i = 0; i < n; i++)
		data[i].x = fmin(1, fmax(0, data[i].x));

	SDL_Rect rect;
	for (int i = 0; i < n; i++) {
		rect.h = sqrt(data[i].y) * sdl_state.h;
		rect.y = sdl_state.h - rect.h;
		rect.x = sdl_state.w * ((i) ? data[i - 1].x : 0);
		rect.w = sdl_state.w * data[i].x - rect.x;
		if (rect.w <= 0)
			rect.w = 1;
		SDL_RenderFillRect(sdl_state.ren, &rect);
	}

	//SDL_Delay(100);
}

int main(void)
{
	struct buffer buf = {.s = 0, .e = 0, .size = BUFSIZE};
	buf.data = malloc(buf.size * sizeof(*buf.data));

	sdl_init();
	jack_init_client();
	jack_setup(record, &buf);
	jack_connect_ports();
	while (process(&buf))
		;
}
