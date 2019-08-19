#ifndef RAMBAJZ_ANALYSER_H
#define RAMBAJZ_ANALYSER_H

#include <unistd.h>
#include "buffer.h"
#include "fft.h"

struct analysis_data {
	struct point *plot;
	size_t plot_size;
	double guessed_frequency;
};

struct analysis_data *analyse(struct analysis_data *, struct buffer *);
void analysis_free(struct analysis_data data);

#endif
