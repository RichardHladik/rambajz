#include <stdbool.h>
#include "analyser.h"

struct viewport_t {
	double A;
	double B;
};

bool interact(struct analysis_params *params, struct viewport_t *viewport, const struct viewport_t *limits);
