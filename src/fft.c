#include <complex.h>
#include <malloc.h>
#include <math.h>

typedef complex double C;
const double PI = 3.14159265358979323;

void _fft(int n, C *v, int m, C *omega) {
    if (n == 1) return;
    C *even, *odd;
	even = malloc((n / 2) * sizeof(*even));
	odd = malloc((n / 2) * sizeof(*odd));
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
	free(odd);
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
