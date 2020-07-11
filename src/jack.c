#include <string.h>
#include "jack.h"
#include "util.h"
#include "misc.h"

struct jack_state_t jack_state;

/* Connects the client to the JACK server, dies on failure. */
void jack_init_client(void)
{
	jack_options_t options = JackNoStartServer;
	jack_status_t status;
	jack_state.client = jack_client_open(JACK_CLIENT_NAME, options, &status, NULL);
	if (!jack_state.client)
		die("jack_client_open() failed, status = 0x%2.0x.\n", status);

	jack_state.sample_rate = jack_get_sample_rate(jack_state.client);
}

/* Sets up the callback (process callback, shutdown callbacks), creates an
 * input and output port and activates the client. Dies on any failure. */
void jack_setup(JackProcessCallback callback, void *arg)
{
	if (!jack_state.client)
		die("jack_setup: not connected to JACK, call jack_init_client first.\n");

	int err = jack_set_process_callback(jack_state.client, callback, arg);
	if (err)
		die("jack_set_process_callback() failed, err = %d\n", err);

	jack_on_shutdown(jack_state.client, jack_shutdown_callback, NULL);
	atexit(jack_cleanup);

	jack_state.in_port = jack_port_register(jack_state.client, "in",
			JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

	if (!jack_state.in_port)
		die("jack_setup: jack_port_register failed.\n");

	err = jack_activate(jack_state.client);
	if (err)
		die("jack_activate failed, err = %d\n", err);
}

/* Connects the client to all physical inputs and outputs. Dies if there are
 * none. */
void jack_connect_ports(const char *wildcard)
{
	const char **ins = jack_get_ports(jack_state.client, wildcard, NULL,
			JackPortIsOutput);

	for (const char **port = ins; port && *port; port++) {
		int err = jack_connect(jack_state.client, *port,
				jack_port_name(jack_state.in_port));
		if (err)
			die("jack_connect failed for port %s: err = %d\n", *port, err);
	}

	jack_free(ins);
}

/* Tears down the JACK connection, dies on errors. */
void jack_cleanup(void)
{
	if (!jack_state.client)
		return; /* No client to tear down, everything is OK. */

	int err = jack_deactivate(jack_state.client);
	if (err)
		die("jack_deactivate failed, err = %d\n", err);

	err = jack_client_close(jack_state.client);
	if (err)
		die("jack_client_close failed, err = %d\n", err);

	memset(&jack_state, 0, sizeof(jack_state));
}

/* Gets called on JACK shutdown, exits the program. */
void jack_shutdown_callback(void *dummy)
{
	_Exit(1); /* Not exit, because of async-signal-safety. */
}
