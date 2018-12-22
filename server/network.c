#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

static struct sockaddr_in serv_addr, cli_addr;
static int sock;

int open_connection(const unsigned short port)
{
	if (!(sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)))
		die(EXIT_FAILURE, "Could not open socket.");

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		die(EXIT_FAILURE, "Could not bind socket.");

	write_log("Connection is open on port %d.", port);
	return 0;
}
