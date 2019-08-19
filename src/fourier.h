#ifndef RAMBAJZ_FFT_H
#define RAMBAJZ_FFT_H

struct point {
	double x;
	double y;
};

void fft(int n, double *v);
void fft_slow(int n, double *v, int m, struct point *data);

#endif
