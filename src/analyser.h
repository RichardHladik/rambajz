#ifndef RAMBAJZ_ANALYSER_H
#define RAMBAJZ_ANALYSER_H

#include <unistd.h>
#include "buffer.h"
#include "fourier.h"
#include "scale.h"

struct analysis_data {
	struct point *plot;
	size_t plot_size;
	double guessed_frequency;
	struct tone guessed_tone;
};

enum analysis_distribution {
	DISTRIBUTION_LINEAR,
	DISTRIBUTION_LOGSCALE,
	DISTRIBUTION_CNT,
};

struct analysis_params {
	enum analysis_distribution dist;
	double min_freq;
	double max_freq;
};

struct analysis_data *analyse(struct analysis_data *, struct buffer *, const struct analysis_params *);
void analysis_free(struct analysis_data data);

#endif
