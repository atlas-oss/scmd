#include <stdio.h>
#include <stdlib.h>

#include "network.h"

#define VER "scmd v0.01"
#define HELP                                                                   \
	"USAGE: scmd IP PORT CMD"

static char* cmd = "";
static char* addr = "";
static int port = 0;

int main(int argc, char **argv)
{
	if(argc < 4) {
		puts(HELP);
		exit(EXIT_SUCCESS);
	}

	addr = argv[1];
	port = atoi(argv[2]);
	cmd = argv[3];

	if(addr[0] == '\0' || cmd[0] == '\0' || port == 0) {
		puts(HELP);
		exit(EXIT_SUCCESS);
	}

	// Starting program flow

	int sock = open_connection(port, addr);

	if(!sock)
	{
		fprintf(stderr, "Could not connect server on %s with port %d", addr, port);
		exit(EXIT_FAILURE);
		
	}
	
	send_cmd(sock, cmd_proto_t *cmd)
	return 0;
}
