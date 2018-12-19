#include "network.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

static struct sockaddr_in serv_addr;

int open_connection(int port, char *addr) // Returns the socket
{
	int sock = 0;

	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return -1;

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = port;

	if (inet_aton(addr, &serv_addr.sin_addr) == 0)
	    return -1;
	    
	return sock;
}

int send_cmd(int fd, cmd_proto_t* cmd)
{
	if(!sendto(fd, cmd, (1024 + sizeof(*cmd)), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))
		return -1;

	memset(cmd, 0, sizeof(*cmd));

	if(!recvfrom(fd, cmd, (1024 + sizeof(*cmd)), 0, (struct sockaddr*)&serv_addr, (socklen_t *)sizeof(serv_addr)))
		return -1;
	
	close(fd);
	
	return 0;
}
