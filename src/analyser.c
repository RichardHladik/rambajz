#include <math.h>
#include <malloc.h>
#include <memory.h>
#include "analyser.h"

const int FRAME_SIZE = (1 << 12);
static const double PI = 3.14159265358979323846;

static void window_function(double *data, size_t n);

struct analysis_data *analyse(struct analysis_data *data, struct buffer *buf)
{
	static struct point *oldplot = NULL;

	const size_t n = FRAME_SIZE;
	double *v = malloc(n * sizeof(*v));
	if (!buffer_peek_back(buf, v, n)) {
		data = NULL;
		goto finish;
	}

	window_function(v, n);

	data->guessed_frequency = -1;
	data->plot_size = (n + 1) / 2;
	data->plot = malloc(data->plot_size * sizeof(*data->plot));

	// for now, assume fft_slow returns the "same" plot every time (only with different y values)
	fft_slow(n, v, data->plot_size, data->plot);
	const double SMOOTHING = .5;
	if (!oldplot) {
		oldplot = malloc(data->plot_size * sizeof(*data->plot));
	} else {
		for (int i = 0; i < data->plot_size; i++)
			data->plot[i].y = data->plot[i].y * (1 - SMOOTHING) + oldplot[i].y * SMOOTHING;
	}

	memcpy(oldplot, data->plot, data->plot_size * sizeof(*data->plot));

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
