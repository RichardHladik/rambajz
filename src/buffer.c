#include "buffer.h"

void buffer_push(struct buffer *buf, const double *data, size_t n)
{
	size_t e = buf->e;
	for (size_t i = 0; i < n; i++)
		buf->data[(e + i) & (buf->size - 1)] = data[i];
	buf->e = e + n;
}

double *buffer_peek_back(const struct buffer *buf, double *out, size_t n)
{
	if (buf->e < n)
		return NULL;

	size_t s = buf->e - n;
	for (size_t i = 0; i < n; i++)
		out[i] = buf->data[(s + i) & (buf->size - 1)];

	return out;
}
