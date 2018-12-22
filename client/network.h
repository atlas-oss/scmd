#ifndef NETWORK_H
#define NETWORK_H

#include "def.h"

int open_connection(int port, char *addr); // Returns a decriptor for the socket
int send_cmd(int fd,
	     cmd_proto_t *cmd); // Returns if the server can handle the command

#endif
