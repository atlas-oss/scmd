#include "network.h"
#include "crypt.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/random.h>

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
	int buflen = strlen(cmd->module) + strlen(cmd->cmd) + 5,
	    cryptlen = buflen + 12 + 16 + 16, aes_len;
	char request[buflen], exitbuf[2];

	unsigned char crypt[cryptlen], iv[12], aad[16], tag[16],
		*key = getAESKey();

	// Prepare
	getrandom(iv, 12, 0);
	getrandom(aad, 16, 0);
	getrandom(tag, 16, 0);

	memset(crypt, 0, sizeof(crypt));

	// Module
	strlcpy(request, cmd->module, buflen);
	strcat(request, "/");

	// Cmd
	strlcat(request, cmd->cmd, buflen);
	strcat(request, "/");
	strcat(request, "/");

	// Exit
	sprintf(exitbuf, "%d", cmd->exit);
	strlcat(request, exitbuf, buflen);

	aes_len = aes_encrypt((unsigned char *)request, buflen, aad,
			      sizeof(aad), key, iv, sizeof(iv), crypt, tag);

	strlcat((char *)crypt, (char *)iv, cryptlen);
	strlcat((char *)crypt, (char *)aad, cryptlen);
	strlcat((char *)crypt, (char *)tag, cryptlen);

	if (!sendto(fd, &crypt, cryptlen, 0, (struct sockaddr *)&serv_addr,
		    len))
		return -1;

	// Cleanup
	memset(crypt, 0, sizeof(crypt));
	memset(iv, 0, sizeof(iv));
	memset(aad, 0, sizeof(aad));
	memset(tag, 0, sizeof(tag));

	// Recv

	cryptlen = 65507;

	if (!recvfrom(fd, &crypt, cryptlen, 0, (struct sockaddr *)&serv_addr,
		      &len))
		return -1;

	cryptlen = strlen((char *)crypt) -
		   43; // Here is a filthy bug. I need a alternative for that,
		       // else my decryption is unstable.

	unsigned char decrypt[cryptlen];
	char result[cryptlen];

	{
		int i = cryptlen;
		for (int j = 0; j < 12; ++j, ++i)
			iv[j] = crypt[i];
		for (int j = 0; j < 16; ++j, ++i)
			aad[j] = crypt[i];
		for (int j = 0; j < 16; ++j, ++i)
			tag[j] = crypt[i];
	}

	strlcpy((char *)decrypt, (char *)crypt, cryptlen + 1);

	aes_len = aes_decrypt(decrypt, cryptlen, aad, sizeof(aad), tag, key, iv,
			      sizeof(iv), (unsigned char *)result);

	strtok(result, "/");
	strtok(NULL, "/");
	cmd->answer = strtok(NULL, "/");
	puts(cmd->answer);

	close(fd);

	return atoi(&result[strlen(result)]);
}
