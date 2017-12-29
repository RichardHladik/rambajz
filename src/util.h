#ifndef RAMBAJZ_UTIL_H
#define RAMBAJZ_UTIL_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void die(const char fmt[], ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(1);
}

#endif
