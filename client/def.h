#ifndef DEF_H
#define DEF_H

enum SERVER_EXIT {
	SUCCESS = 0x00,
	UNKOWN_FAILURE = 0x01,
	TIMEOUT = 0x02,
	UNKOWN_CMD = 0x03,
	UNKOWN_MODULE = 0x04,
	MODULE_CRASH = 0x05
};

typedef enum SERVER_EXIT server_exit_t;
typedef struct {
	char *module;
	char *cmd;
	char *answer;
	server_exit_t exit;
} cmd_proto_t;

#endif
