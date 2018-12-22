#ifndef DEF_H
#define DEF_H

#include <dirent.h>
#include <errno.h>

#define MAX_MODULE 1024
#define PORT 1098

#define write_log(msg, ...)                                                    \
	_write_log(__FUNCTION__, __FILE__, (msg), __VA_ARGS__)

void prepare_log();

void close_log();

void _write_log(const char *func, const char *file, const char *msg, ...);

void die(int err, char *msg, ...);

DIR *open_dir(const char *cat_path, const int len);

#endif
