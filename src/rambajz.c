#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "analyser.h"
#include "buffer.h"
#include "jack.h"
#include "gui.h"
#include "scale.h"
#include "sdl.h"
#include "util.h"

const size_t BUFSIZE = (1 << 20);

// Assumption: nframes is constant throughout execution
int record(jack_nframes_t nframes, void *arg)
{
	struct buffer *buf = arg;
	jack_default_audio_sample_t *in;
	in = jack_port_get_buffer(jack_state.in_port, nframes);
	double in_double[nframes];
	for (int i = 0; i < nframes; i++)
		in_double[i] = in[i];

	buffer_push(buf, in_double, nframes);
	return 0;
}

void draw(const struct analysis_data *, const struct analysis_params *);
void draw_plot(size_t n, const struct point *data, double from, double to);

bool process(struct buffer *buf)
{
	const double min_freq = 20;
	const double max_freq = jack_state.sample_rate / 2;
	static struct analysis_params params = {.dist = DISTRIBUTION_LOGSCALE};
	static struct viewport_t viewport = {NAN, NAN}, limits = {NAN, NAN};
	if (isnan(viewport.A)) {
		limits.A = viewport.A = log(min_freq);
		limits.B = viewport.B = log(max_freq);
	}
	
	if (!interact(&params, &viewport, &limits))
		return false;

	struct analysis_data data;
	if (!analyse(&data, buf, &params))
		return true;

	printf("%lf %s %lf\n", data.guessed_frequency, tone_name(data.guessed_tone), data.guessed_tone.cents);
	draw(&data, &params);

	analysis_free(data);
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

void draw(const struct analysis_data *data, const struct analysis_params *params) {
	SDL_Rect rect = {.h = sdl_state.h, .w = sdl_state.w, .x = 0, .y = 0};
	SDL_SetRenderDrawColor(sdl_state.ren, 0, 0, 0, 255);
	SDL_RenderClear(sdl_state.ren);

	SDL_SetRenderDrawColor(sdl_state.ren, 255, 255, 255, 255);
	draw_plot(data->plot_size, data->plot, params->min_freq, params->max_freq);
	SDL_SetRenderDrawColor(sdl_state.ren, 255, 0, 0, 255);
	double guessed = logscale(data->guessed_frequency, params->min_freq, params->max_freq);
	plot_interval(guessed, guessed, 1);
	SDL_RenderPresent(sdl_state.ren);
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
