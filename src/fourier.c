#include "fourier.h"
#include <complex.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

typedef complex double C;
const double PI = 3.14159265358979323;

static void _fft(int n, C *v, int m, C *omega) {
    if (n == 1) return;
    C *even, *odd;
	even = malloc(n * sizeof(*even));
	odd = even + (n / 2);
    for (int i = 0; i < n / 2; i++)
		even[i] = v[2 * i], odd[i] = v[2 * i + 1];

    _fft(n / 2, even, m, omega);
    _fft(n / 2, odd, m, omega);
	int step = m / n;
	for (int i = 0; i < n / 2; i++) {
		C om = omega[i * step];
        v[i] = even[i] + om * odd[i];
        v[i + n / 2] = even[i] - om * odd[i];
    }
	free(even);
}

void fft(int n, double *v)
{
	C *omega = malloc(n * sizeof(*omega));
	omega[0] = 1;
	omega[1] = cexp(2 * PI * I / n);
	for (int i = 2; i < n; i++)
		omega[i] = omega[i - 1] * omega[1];

	C *com = malloc(n * sizeof(*com));
	for (int i = 0; i < n; i++)
		com[i] = v[i];

	_fft(n, com, n, omega);
	free(omega);
	for (int i = 0; i < n; i++)
		v[i] = cabs(com[i]);
	free(com);
}

static double fft_point(int n, double *v, double x)
{
	C om = cexp(2 * PI * I * x);
	C ompow = 1;
	C res = 0;
	for (int i = 0; i < n; i++, ompow *= om)
		res += v[i] * ompow;
	return cabs(res);
}

void fft_slow(int n, double *v, int m, struct point *data)
{
	double logn = log(m + 1);
	for (int i = 0; i < m; i++) {
		double x = (exp((double)i * logn / (m + 1)) - 1) / (m + 1);
		data[i] = (struct point){.x = log(x * m + 1) / log(m + 1), .y = fft_point(n, v, x * m / n) / m};
	}
}
