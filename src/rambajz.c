#include <stdio.h>
#include <unistd.h>
#include "jack.h"

static long long t = 0;

int process(jack_nframes_t nframes, void *arg)
{
	jack_default_audio_sample_t *in, *out;
	in = jack_port_get_buffer(jack_state.in_port, nframes);
	out = jack_port_get_buffer(jack_state.out_port, nframes);
	for (int i = 0; i < nframes; i++, t++)
		out[i] = in[i];
	return 0;
}

int main(void)
{
	jack_init_client();
	jack_setup(process);
	jack_connect_ports();
	sleep(-1);
}
