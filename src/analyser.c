#include <math.h>
#include <malloc.h>
#include <memory.h>
#include <stdbool.h>
#include "analyser.h"

const int FRAME_SIZE = 1 << 14;
const int PLOT_SIZE = 1 << 9;
static const double PI = 3.14159265358979323846;

static void window_function(double *data, size_t n);
static double estimate_frequency(double *v, size_t n, double mean, double radius);

static double top_frequency(const struct point *plot, size_t size, const struct analysis_params *params)
{
	size_t res = 0;
	bool nonempty = false;
	for (size_t i = 0; i < size; i++) {
		if (plot[i].x >= params->min_freq && plot[i].x <= params->max_freq) {
			nonempty = true;
			if (plot[i].y > plot[res].y)
				res = i;
		}
	}

	if (!nonempty)
		return NAN;
	return plot[res].x;
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

	if (params->dist == DISTRIBUTION_LOGSCALE) {
		data->plot_size = n / 2;
		data->plot = malloc(data->plot_size * sizeof(*data->plot));
		plot_frequencies(n, v, data->plot);
	} else if (params->dist == DISTRIBUTION_LINEAR) {
		data->plot_size = PLOT_SIZE;
		data->plot = malloc(data->plot_size * sizeof(*data->plot));
		plot_frequencies_logscale(n, v, data->plot_size, data->plot, params->min_freq, params->max_freq);
	}

	static const double radius = 10;
	double guess = top_frequency(data->plot, data->plot_size, params);
	guess = fmax(fmin(guess, params->max_freq - radius), params->min_freq + radius);
	data->guessed_frequency = estimate_frequency(v, n, guess, radius);
	data->guessed_tone = calc_tone(data->guessed_frequency);

finish:
	free(v);
	return data;
}

static double estimate_frequency(double *v, size_t n, double mean, double radius)
{
	if (isnan(mean) || isnan(radius))
		return NAN;

	static const double start_resolution = 10; // Hz
	static const double target_resolution = 1e-6;
	static const double step = 4;
	const double a = mean, b = radius;
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
		if (high - low >= 2 * radius) // Should happen almost every time
			mean = fmax(fmin(mean, high - radius), low + radius);
		else
			mean = (high + low) / 2;
	}

	if (mean < 0)
		printf("%lf %lf %lf\n", mean, a, b);
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
