#include <math.h>
#include <malloc.h>
#include <memory.h>
#include "analyser.h"

const int FRAME_SIZE = (1 << 12);

struct analysis_data *analyse(struct analysis_data *data, struct buffer *buf)
{
	static struct point *oldplot = NULL;

	const size_t n = FRAME_SIZE;
	double *v = malloc(n * sizeof(*v));
	if (!buffer_peek_back(buf, v, n)) {
		data = NULL;
		goto finish;
	}

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
