#include <stdatomic.h>
#include <unistd.h>

struct buffer {
	double *data;
	size_t s;
	_Atomic size_t e;
	size_t size; // Always a power of two
};

void buffer_push(struct buffer *buf, const double *data, size_t n);
double *buffer_peek_back(const struct buffer *buf, double *out, size_t n);
