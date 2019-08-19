#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "buffer.h"
#include "fft.h"
#include "jack.h"
#include "sdl.h"

const size_t BUFSIZE = (1 << 20);

int record(jack_nframes_t nframes, void *arg)
{
	struct buffer *buf = arg;
	jack_default_audio_sample_t *in, *out;
	in = jack_port_get_buffer(jack_state.in_port, nframes);
	out = jack_port_get_buffer(jack_state.out_port, nframes);
	int n = nframes;

	size_t e = buf->e;
	for (int i = 0; i < n; i++)
		buf->data[(e + i) % buf->size] = in[i];
	buf->e = (e + n) % buf->size;

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

	static struct point *olddata = NULL;

	const int frame = (1 << 12);
	int e = buf->e;
	int s = buf->s;
	printf("%d %d\n", e, s);
	if (e - s >= 0 && e - s < frame)
		return true;

	int n = frame;
	double *v = malloc(n * sizeof(*v));
	double *v1 = malloc(n * sizeof(*v1));
	s = (e + buf->size - frame) % buf->size;
	buf->s = s;
	for (int i = 0; i < n; i++)
		v[i] = v1[i] = buf->data[(s + i) % buf->size];

	int nn = (n + 1) / 2;
	struct point *data = malloc(nn * sizeof(*data));
	struct point *data1 = malloc(nn * sizeof(*data1));
	fft(n, v);
	fft_slow(n, v1, nn, data1);
	double logn = log(nn + 1);
	for (int i = 0; i < nn; i++)
		data[i] = (struct point){.x = (double)log(i + 1) / logn, .y = v[i] / n};

	const double SMOOTHING = .5;
	if (olddata)
		for (int i = 0; i < nn; i++)
			data1[i].y = data1[i].y * (1 - SMOOTHING) + olddata[i].y * SMOOTHING;

	free(v);
	free(v1);

	SDL_Rect rect = {.h = sdl_state.h, .w = sdl_state.w, .x = 0, .y = 0};
	SDL_SetRenderDrawColor(sdl_state.ren, 0, 0, 0, 255);
	SDL_RenderClear(sdl_state.ren);

	SDL_SetRenderDrawColor(sdl_state.ren, 255, 0, 0, 255);
	draw(nn, data);
	SDL_SetRenderDrawColor(sdl_state.ren, 255, 255, 255, 255);
	draw(nn, data1);
	SDL_RenderPresent(sdl_state.ren);

	free(olddata);
	olddata = data1;
	free(data);

	return true;
}

void draw(size_t n, struct point *data)
{
	SDL_Rect rect;
	double logn = log(n + 1);
	for (int i = 0; i < n; i++) {
		rect.h = sqrt(data[i].y) * sdl_state.h; // (1 + v[i]) / 2 * sdl_state.h;
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
