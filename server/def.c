#include "def.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>

static char *phome = NULL;
static FILE *f = NULL;

DIR *open_dir(const char *cat_path, const int len)
{
	static DIR *d = NULL;
	char *home = getenv("HOME");

	char path[strlen(home) + 1 + len];
	strncpy(path, home, strlen(home) + 1);

	if (!strncat(path, cat_path, len))
		return NULL;

	d = opendir(path);

	if (!d) {
		write_log("Could not find directory %s.", path);

		if (0 != mkdir(path, S_IRWXU))
			return NULL;

		write_log("Created directory %s.", path);

		d = opendir(path);
	}
	return d;
}

void prepare_log()
{
	phome = getenv("HOME");

	if (!phome)
		die(EXIT_FAILURE, "Could not find $HOME variable.");

	char path[strlen(phome) + 10];
	strncpy(path, phome, strlen(phome) + 1);
	strcat(path, "/.scmd/log");

	f = fopen(path, "w");

	if (!f)
		die(ENOENT, "Could not open log file.");
}

void _write_log(const char *func, const char *file, const char *msg, ...)
{
	va_list va;
	va_start(va, msg);

	if (f) {
		fprintf(f, "%s:[%s]: ", func, file);
		vfprintf(f, msg, va);
		fprintf(f, "\n");
	}

#ifdef VERBOSE
	printf("%s:[%s]: ", func, file);
	vprintf(msg, va);
	puts("");
#endif

	va_end(va);
}

void close_log()
{
	fclose(f);
}

void _die(const char *func, const char *file, const int err, const char *msg,
	  ...)
{
	va_list va;
	va_start(va, msg);

	printf("FATAL ERROR: %s:[%s]: ", func, file);
	vprintf(msg, va);
	puts("");

	va_end(va);
	exit(err);
}
