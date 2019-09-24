#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "analyser.h"
#include "buffer.h"
#include "jack.h"
#include "gui.h"
#include "scale.h"
#include "sdl.h"
#include "util.h"

const size_t BUFSIZE = (1 << 20);

// Assumption: nframes is constant throughout execution
int record(jack_nframes_t nframes, void *arg)
{
	struct buffer *buf = arg;
	jack_default_audio_sample_t *in;
	in = jack_port_get_buffer(jack_state.in_port, nframes);
	double in_double[nframes];
	for (int i = 0; i < nframes; i++)
		in_double[i] = in[i];

	buffer_push(buf, in_double, nframes);
	return 0;
}

bool process(struct buffer *buf)
{
	const double min_freq = 20;
	const double max_freq = jack_state.sample_rate / 2;
	static struct analysis_params params = {.dist = DISTRIBUTION_LOGSCALE};
	static struct viewport_t viewport = {NAN, NAN}, limits = {NAN, NAN};
	if (isnan(viewport.A)) {
		limits.A = viewport.A = log(min_freq);
		limits.B = viewport.B = log(max_freq);
	}
	
	if (!interact(&params, &viewport, &limits))
		return false;

	struct analysis_data data;
	if (!analyse(&data, buf, &params))
		return true;

	printf("%lf %s %lf\n", data.guessed_frequency, tone_name(data.guessed_tone), data.guessed_tone.cents);
	draw(&data, &params);

	analysis_free(data);
	return true;
}

int main(void)
{
	struct buffer buf = {.s = 0, .e = 0, .size = BUFSIZE};
	buf.data = malloc(buf.size * sizeof(*buf.data));

	sdl_init();
	jack_init_client();
	jack_setup(record, &buf);
	jack_connect_ports();
	while (process(&buf))
		;
}
