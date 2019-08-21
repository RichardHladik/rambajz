#ifndef RAMBAJZ_UTIL_H
#define RAMBAJZ_UTIL_H

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static void die(const char fmt[], ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(1);
}

inline double logscale(double x, double from, double to) {
	return log(x / from) / log(to / from);
}

inline double inv_logscale(double x, double from, double to) {
	return exp(log(to / from) * x) * from;
}

#endif
