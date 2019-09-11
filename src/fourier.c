#include "fourier.h"
#include <malloc.h>
#include <math.h>
#include <string.h>
#include "util.h"
#include "jack.h"

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

void fft(int n, C *v)
{
	C *omega = malloc(n * sizeof(*omega));
	omega[0] = 1;
	omega[1] = cexp(2 * PI * I / n);
	for (int i = 2; i < n; i++)
		omega[i] = omega[i - 1] * omega[1];

	_fft(n, v, n, omega);
	free(omega);
}

static double fourier_point(int n, double *v, double x)
{
	C om = cexp(2 * PI * I * x);
	C ompow = 1;
	C res = 0;
	for (int i = 0; i < n; i++, ompow *= om)
		res += v[i] * ompow;
	return cabs(res);
}

double frequency_strength(int n, double *v, double freq)
{
	return fourier_point(n, v, freq / jack_state.sample_rate);
}

void plot_frequencies(int n, double *v, int m, struct point *data, double A, double B)
{
	for (size_t i = 0; i < m; i++) {
		double freq = inv_logscale((double)i / (m - 1), A, B);
		data[i] = (struct point){.x = freq, .y = frequency_strength(n, v, freq) / m};
	}
}
