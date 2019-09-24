#include <stdbool.h>
#include "analyser.h"

struct viewport_t {
	double A;
	double B;
};

bool interact(struct analysis_params *params, struct viewport_t *viewport, const struct viewport_t *limits);
void plot_interval(double x0, double x1, double y);
void draw_plot(size_t n, const struct point *data, double A, double B);
void draw(const struct analysis_data *data, const struct analysis_params *params);
