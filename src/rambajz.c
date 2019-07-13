#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "jack.h"
#include "fft.c"
#include "sdl.c"

const size_t BUFSIZE = (1 << 20);

struct buf {
	double *data;
	size_t s;
	_Atomic size_t e;
	size_t size;
};

int process(jack_nframes_t nframes, void *arg)
{
	struct buf *buf = arg;
	jack_default_audio_sample_t *in, *out;
	in = jack_port_get_buffer(jack_state.in_port, nframes);
	out = jack_port_get_buffer(jack_state.out_port, nframes);
	int n = nframes;

	for (int i = 0; i < n; i++)
		buf->data[(buf->e + i) % buf->size] = in[i];
	buf->e = (buf->e + n) % buf->size;

	for (int i = 0; i < nframes; i++)
		out[i] = 0;

	return 0;
}

bool draw(struct buf *buf)
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

	const int frame = (1 << 10);
	int e = buf->e;
	int s = buf->s;
	printf("%d %d\n", e, s);
	if (e - s >= 0 && e - s < frame)
		return true;

	int n = frame;
	double v[n];
	s = (e + buf->size - frame) % buf->size;
	buf->s = s;
	for (int i = 0; i < n; i++)
		v[i] = buf->data[(s + i) % buf->size];

	fft(n, v);
	for (int i = 0; i < n; i++)
		v[i] /= n;

	double m = 0;
	n = n / 2 + 1;
	for (int i = 0; i < n; i++)
		if (fabs(v[i]) > m)
			m = fabs(v[i]);
	printf("%f ", m);

	SDL_Rect rect = {.h = sdl_state.h, .w = sdl_state.w, .x = 0, .y = 0};
	SDL_SetRenderDrawColor(sdl_state.ren, 0, 0, 0, 255);
	SDL_RenderClear(sdl_state.ren);
	SDL_SetRenderDrawColor(sdl_state.ren, 255, 255, 255, 255);
	for (int i = 0; i < n; i++) {
		rect.h = sqrt(v[i]) * sdl_state.h; // (1 + v[i]) / 2 * sdl_state.h;
		rect.y = 0;
		rect.x = sdl_state.w * (long long)i / n;
		rect.w = sdl_state.w * (long long)(i + 1) / n - rect.x;
		if (!rect.w)
			rect.w = 1;
//		SDL_FillRect(sdl_state.sur, &rect, SDL_MapRGB(sdl_state.sur->format, 255, 255, 255));
		SDL_RenderFillRect(sdl_state.ren, &rect);
	}

	//SDL_Delay(100);
	SDL_RenderPresent(sdl_state.ren);
	return true;
}

int main(void)
{
	struct buf buf = {.s = 0, .e = 0, .size = BUFSIZE};
	buf.data = malloc(buf.size * sizeof(*buf.data));

	sdl_init();
	jack_init_client();
	jack_setup(process, &buf);
	jack_connect_ports();
	while (draw(&buf))
		;
}
