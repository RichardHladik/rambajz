#include <jack/jack.h>

void jack_init_client(void);
void jack_setup(JackProcessCallback, void *);
void jack_connect_ports(const char *wildcard);

void jack_cleanup(void);
static void jack_shutdown_callback(void *);

struct jack_state_t {
	jack_client_t *client;
	jack_port_t *in_port, *out_port;
	jack_nframes_t sample_rate;
};

extern struct jack_state_t jack_state;
