#include "network.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static struct sockaddr_in serv_addr;

int open_connection(int port, char *addr) // Returns the socket
{
	int sock = 0;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return -1;

	memset((char *)&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_aton(addr, &serv_addr.sin_addr) == 0)
		return -1;

	return sock;
}

int send_cmd(int fd, cmd_proto_t *cmd)
{
	socklen_t len = sizeof(struct sockaddr_in);
	int buflen = strlen(cmd->module) + strlen(cmd->cmd) + 4;
	char buf[buflen], exitbuf[2];

	// Module
	strncpy(buf, cmd->module, strlen(cmd->module));
	strcat(buf, "/");

	// Cmd
	strncat(buf, cmd->cmd, strlen(cmd->cmd));
	strcat(buf, "/");

	// Exit
	sprintf(exitbuf, "%d", cmd->exit);
	strncat(buf, exitbuf, strlen(exitbuf));

	if (!sendto(fd, &buf, buflen, 0, (struct sockaddr *)&serv_addr, len))
		return -1;

	memset(buf, 0, sizeof(buf));

	if (!recvfrom(fd, &buf, buflen, 0, (struct sockaddr *)&serv_addr, &len))
		return -1;

	close(fd);

	return atoi(&buf[buflen - 1]);
}
