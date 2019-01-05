#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

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
	cmd = argv[3]; // Format: module?cmd

	if (addr[0] == '\0' || cmd[0] == '\0' || port == 0) {
		puts(HELP);
		exit(EXIT_SUCCESS);
	}

	// Starting program flow

	int res;
	int sock = open_connection(port, addr);

	if (!sock)
		die(EXIT_FAILURE,
		    "Could not open connection for %s on port %d\n", addr,
		    port);

	cmd_proto_t request = {.exit = SUCCESS};

	request.module = strtok(argv[3], "?");
	request.cmd = strtok(NULL, "?");

	if ((res = send_cmd(sock, &request)) == -1)
		die(EXIT_FAILURE, "Could not send command\n");

	switch (res) {
	case SUCCESS:
		puts("Programm exited with result: SUCCESS");
		break;

	case UNKOWN_FAILURE:
		puts("Programm exited with result: UNKOWN_FAILURE. Something is wrong.");
		break;

	case TIMEOUT:
		puts("The connection timed out: TIMEOUT");
		break;

	case UNKOWN_CMD:
		puts("The module doesn't know the requested command: UNKOWN_CMD");
		break;

	case UNKOWN_MODULE:
		puts("The server cannot find the requested module: UNKOWN_MODULE");
		break;

	case MODULE_CRASH:
		puts("The requested module crashed: MODULE_CRASH");
		break;

	default:
		puts("Unkown server reply. Your client is maybe outdated.");
	}

	return 0;
}
