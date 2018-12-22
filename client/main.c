#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "network.h"

#define VER "scmd v0.01"
#define HELP "USAGE: scmd IP PORT CMD"

static char *cmd = "";
static char *addr = "";
static int port = 0;

void die(int err, char *msg, ...)
{
	va_list va;
	va_start(va, msg);
	vfprintf(stderr, msg, va);
	va_end(va);
	exit(err);
}

int main(int argc, char **argv)
{
	if (argc < 4) {
		puts(HELP);
		exit(EXIT_SUCCESS);
	}

	addr = argv[1];
	port = atoi(argv[2]);
	cmd = argv[3];

	if (addr[0] == '\0' || cmd[0] == '\0' || port == 0) {
		puts(HELP);
		exit(EXIT_SUCCESS);
	}

	// Starting program flow

	int sock = open_connection(port, addr);

	if (!sock)
		die(EXIT_FAILURE,
		    "Could not open connection for %s on port %d\n", addr,
		    port);

	cmd_proto_t request = {.exit = SUCCESS};

	if (!send_cmd(sock, &request))
		die(EXIT_FAILURE, "Could not send command\n");

	return 0;
}
