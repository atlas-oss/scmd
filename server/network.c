#include "network.h"
#include "mod.h"
#include "crypt.h"

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
		char request[buflen], exitbuf[2];
		unsigned char iv[12], aad[16], tag[16],
			*key = getAESKey();

		memset(&cmd, 0, sizeof(cmd));

		if (!recvfrom(sock, &request, buflen, 0,
			      (struct sockaddr *)&cli_addr, &len))
			die(EXIT_FAILURE, "Could not recv from client");

		write_log("Received command from %s:%d",
			  inet_ntoa(cli_addr.sin_addr),
			  ntohs(cli_addr.sin_port));

		buflen = strlen(request) - 43;
		unsigned char decrypt[buflen];
		char result[buflen];

		{
			int i = buflen;
			for(int j = 0; j < 12; ++j, ++i)
				iv[j] = request[i];
			for(int j = 0; j < 16; ++j, ++i)
				aad[j] = request[i];
			for(int j = 0; j < 16; ++j, ++i)
				tag[j] = request[i];			
		}

		strlcpy((char*)decrypt, request, buflen + 1);

		aes_decrypt(decrypt, buflen, aad, sizeof(aad), tag, key, iv,
			      sizeof(iv), (unsigned char *)result);

		cmd.module = strtok(result, "/");
		cmd.cmd = strtok(NULL, "/");
		cmd.answer = "None";
		cmd.exit = atoi(strtok(NULL, "/"));
		
		cmd.exit = process_cmd(&cmd);

		buflen = strlen(cmd.module) + strlen(cmd.cmd) + strlen(cmd.answer) + 5;
		char replybuf[buflen], replycrypt[buflen + 44];

		// Module
		strlcpy(replybuf, cmd.module, buflen);
		strcat(replybuf, "/");

		// Cmd
		strlcat(replybuf, cmd.cmd, buflen);
		strcat(replybuf, "/");

		// Answer
		strlcat(replybuf, cmd.answer, buflen);
		strcat(replybuf, "/");

		// Exit
		sprintf(exitbuf, "%d", cmd.exit);
		strlcat(replybuf, exitbuf, buflen);

		aes_encrypt((unsigned char *)replybuf, buflen, aad, sizeof(aad),
			    key, iv, sizeof(iv), (unsigned char *)replycrypt,
			    tag);

		strlcat((char *)replycrypt, (char *)iv, buflen);
		strlcat((char *)replycrypt, (char *)aad, buflen);
		strlcat((char *)replycrypt, (char *)tag, buflen);

		if (!sendto(sock, &replycrypt, buflen + 44, 0,
			    (struct sockaddr *)&cli_addr, len))
			die(EXIT_FAILURE, "Could not reply to client");
	}
	return 0;
}
