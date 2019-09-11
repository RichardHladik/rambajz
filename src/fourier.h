#ifndef RAMBAJZ_FFT_H
#define RAMBAJZ_FFT_H
#include <complex.h>

typedef complex double C;
struct point {
	double x;
	double y;
};

void fft(int n, C *v);
void plot_frequencies_logscale(int n, double *v, int m, struct point *data, double min_freq, double max_freq);

#endif
