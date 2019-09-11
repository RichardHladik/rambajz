#include <math.h>
#include <malloc.h>
#include <memory.h>
#include "analyser.h"

const int FRAME_SIZE = (1 << 13);
const int OPERATIONS = (1 << 23);
const int PLOT_SIZE = OPERATIONS / FRAME_SIZE;
static const double PI = 3.14159265358979323846;

static void window_function(double *data, size_t n);
static double estimate_frequency(double *samples, size_t n, const struct analysis_params *params);

static size_t argmax(const struct point *plot, size_t size)
{
	size_t res = 0;
	for (size_t i = 0; i < size; i++)
		if (plot[i].y > plot[res].y)
			res = i;
	return res;
}

static double top_frequency(const struct point *plot, size_t size)
{
	return plot[argmax(plot, size)].x;
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
	data->plot_size = PLOT_SIZE;
	data->plot = malloc(data->plot_size * sizeof(*data->plot));

	plot_frequencies_logscale(n, v, data->plot_size, data->plot, params->min_freq, params->max_freq);

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

	data->guessed_frequency = estimate_frequency(v, n, params);

finish:
	free(v);
	return data;
}

static double estimate_frequency(double *samples, size_t n, const struct analysis_params *params)
{
	size_t size = PLOT_SIZE;
	double low = params->min_freq, high = params->max_freq;
	struct point *plot = malloc(size * sizeof(*plot));
	for (size_t it = 0; it < 10; it++) {
		printf("[%lf %lf]", low, high);
		plot_frequencies_logscale(n, samples, size, plot, low, high);
		double guess = top_frequency(plot, size);
		double width = (high - low) * .1;
		low = fmax(guess - width / 2, low);
		high = fmin(guess + width / 2, high);
		size /= 2;
	}
	printf("\n");

	free(plot);

	return (low + high) / 2;
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
