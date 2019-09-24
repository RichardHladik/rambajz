#include <math.h>
#include <malloc.h>
#include <memory.h>
#include "analyser.h"

const int FRAME_SIZE = 1 << 14;
static const double PI = 3.14159265358979323846;

static void window_function(double *data, size_t n);
static double estimate_frequency(double *v, size_t n, double mean, double radius);

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
	data->plot_size = n / 2;
	data->plot = malloc(data->plot_size * sizeof(*data->plot));
	plot_frequencies(n, v, data->plot);

	static struct {
		struct point *plot;
		struct analysis_params params;
	} old = {NULL, {0}};

	if (!old.plot)
		old.plot = malloc(data->plot_size * sizeof(*data->plot));

	old.params = *params;
	memcpy(old.plot, data->plot, data->plot_size * sizeof(*data->plot));

	double guess = top_frequency(data->plot, data->plot_size);
	data->guessed_frequency = estimate_frequency(v, n, guess, 10);
	data->guessed_tone = calc_tone(data->guessed_frequency);

finish:
	free(v);
	return data;
}

static double estimate_frequency(double *v, size_t n, double mean, double radius)
{
	static const double start_resolution = 10; // Hz
	static const double target_resolution = 1e-6;
	static const double step = 4;
	for (double res = start_resolution; res > target_resolution; res /= step) {
		double low = mean - radius, high = mean + radius;
		size_t cnt = (high - low) / res + 2;
		double best = mean, bestval = frequency_strength(n, v, mean);
		for (size_t i = 0; i < cnt; i++) {
			double freq = low + (high - low) * i / cnt;
			double strength = frequency_strength(n, v, freq);
			if (strength > bestval)
				best = freq, bestval = strength;
		}

		radius = 2 * res;
		mean = best;
	}

	return mean;
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
