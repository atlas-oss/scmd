#ifndef DEF_H
#define DEF_H

enum SERVER_EXIT {
	SUCCESS = 0x00,
	UNKOWN_FAILURE = 0x01,
	TIMEOUT = 0x02,
	UNKOWN_CMD = 0x03,
	MODULE_CRASH = 0x04
};

typedef enum SERVER_EXIT server_exit_t;
typedef struct {
	server_exit_t exit;
} cmd_proto_t;

#endif
