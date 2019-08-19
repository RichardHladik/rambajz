#include "buffer.h"

void buffer_push(struct buffer *buf, double *data, size_t n)
{
	size_t e = buf->e;
	for (size_t i = 0; i < n; i++)
		buf->data[(e + i) % buf->size] = data[i];
	buf->e = (e + n) % buf->size;
}
