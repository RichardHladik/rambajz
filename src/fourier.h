#ifndef RAMBAJZ_FFT_H
#define RAMBAJZ_FFT_H

struct point {
	double x;
	double y;
};

void fft(int n, double *v);
void plot_frequencies(int n, double *v, int m, struct point *data);

#endif
