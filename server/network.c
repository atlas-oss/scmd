#include "network.h"
#include "mod.h"

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

int wait_for_cmd()
{
	while (1) {
		socklen_t len = sizeof(struct sockaddr_in);
		cmd_proto_t cmd;
		int buflen = 65507; // Maximal UDP size
		char buf[buflen], exitbuf[2];

		memset(&cmd, 0, sizeof(cmd));

		if (!recvfrom(sock, &buf, buflen, 0,
			      (struct sockaddr *)&cli_addr, &len))
			die(EXIT_FAILURE, "Could not recv from client");

		write_log("Received command from %s:%d",
			  inet_ntoa(cli_addr.sin_addr),
			  ntohs(cli_addr.sin_port));

		cmd.module = strtok(buf, "/");
		cmd.cmd = strtok(NULL, "/");
		cmd.exit = atoi(strtok(NULL, "/"));

		cmd.exit = process_cmd(&cmd);

		buflen = strlen(cmd.module) + strlen(cmd.cmd) + 4;
		char replybuf[buflen];

		// Module
		strncpy(replybuf, cmd.module, strlen(cmd.module));
		strcat(replybuf, "/");

		// Cmd
		strncat(replybuf, cmd.cmd, strlen(cmd.cmd));
		strcat(replybuf, "/");

		// Exit
		sprintf(exitbuf, "%d", cmd.exit);
		strncat(replybuf, exitbuf, strlen(exitbuf));

		if (!sendto(sock, &replybuf, buflen, 0,
			    (struct sockaddr *)&cli_addr, len))
			die(EXIT_FAILURE, "Could not reply to client");
	}
	return 0;
}
