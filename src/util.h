#ifndef RAMBAJZ_UTIL_H
#define RAMBAJZ_UTIL_H

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void die(const char fmt[], ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(1);
}

inline double logscale(double x, double from, double to) {
	return (log(x) - log(from)) / (log(to) - log(from));
}

inline double inv_logscale(double x, double from, double to) {
	return exp(log(to / from) * x) * from;
}

inline double now() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec / 1e9;
}

inline void sleep_till(double timestamp) {
	double start = now();
	double duration = timestamp - start;
	if (duration <= 0)
		return;
	struct timespec ts = {.tv_sec = (long long)duration};
	ts.tv_nsec = 1e9 * (duration - ts.tv_sec);
	nanosleep(&ts, NULL);
}

#endif
