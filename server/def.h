#ifndef DEF_H
#define DEF_H

#include <dirent.h>
#include <errno.h>

#define MAX_MODULE 1024
#define PORT 1098

#define write_log(msg, ...)                                                    \
	_write_log(__FUNCTION__, __FILE__, msg, ##__VA_ARGS__)

#define die(err, msg, ...)                                                     \
	_die(__FUNCTION__, __FILE__, err, msg, ##__VA_ARGS__)

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
	server_exit_t exit;
} cmd_proto_t;

typedef struct {
	char *name;
	int (*init)(); // init the module, called "init"
	int (*cmd)(char *cmd);  // handles commands from the client, called "cmd"
} mod_t;


void prepare_log();

void close_log();

void _write_log(const char *func, const char *file, const char *msg, ...);

void _die(const char *func, const char *file, const int err, const char *msg,
	  ...);

DIR *open_dir(const char *cat_path, const int len);

#endif
