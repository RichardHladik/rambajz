#include <stdatomic.h>
#include <unistd.h>

struct buffer {
	double *data;
	size_t s;
	_Atomic size_t e;
	size_t size; // Always a power of two
};

void buffer_push(struct buffer *buf, double *data, size_t n);
