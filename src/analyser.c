#include <math.h>
#include <malloc.h>
#include <memory.h>
#include "analyser.h"

const int FRAME_SIZE = (1 << 13);
static const double PI = 3.14159265358979323846;

static void window_function(double *data, size_t n);
static double estimate_frequency(const struct analysis_data *data);

static size_t argmax(const struct point *plot, size_t size) {
	size_t res = 0;
	for (size_t i = 0; i < size; i++)
		if (plot[i].y > plot[res].y)
			res = i;
	return res;
}

struct analysis_data *analyse(struct analysis_data *data, struct buffer *buf, const struct analysis_params *params)
{
	const size_t n = FRAME_SIZE;
	double *v = malloc(n * sizeof(*v));
	if (!buffer_peek_back(buf, v, n)) {
		data = NULL;
		goto finish;
	}

	window_function(v, n);

	data->guessed_frequency = NAN;
	data->plot_size = (1 << 23) / n;
	data->plot = malloc(data->plot_size * sizeof(*data->plot));

	plot_frequencies(n, v, data->plot_size, data->plot, params->min_freq, params->max_freq);

	const double SMOOTHING = .5;

	static struct {
		struct point *plot;
		struct analysis_params params;
	} old = {NULL, {0}};

	if (!old.plot) {
		old.plot = malloc(data->plot_size * sizeof(*data->plot));
	} else if (params->max_freq == old.params.max_freq && params->min_freq == old.params.min_freq) {
		for (int i = 0; i < data->plot_size; i++)
			data->plot[i].y = data->plot[i].y * (1 - SMOOTHING) + old.plot[i].y * SMOOTHING;
	}

	old.params = *params;
	memcpy(old.plot, data->plot, data->plot_size * sizeof(*data->plot));

	data->guessed_frequency = data->plot[argmax(data->plot, data->plot_size)].x;

finish:
	free(v);
	return data;
}

void analysis_free(struct analysis_data data)
{
	free(data.plot);
}

// Assumes n doesn't change during a single run.
static void window_function(double *data, size_t n)
{
	static double *window = NULL;
	if (!window) {
		window = malloc(n * sizeof(*window));
		for (size_t i = 0; i < n; i++)
			window[i] = sin(PI * i / n), window[i] *= window[i];
	}

	for (size_t i = 0; i < n; i++)
		data[i] *= window[i];
}
