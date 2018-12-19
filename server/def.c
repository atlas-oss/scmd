#include "def.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void die(int err, char *msg, int argc, ...)
{
	va_list va;
	va_start(va, argc);
	vfprintf(stderr, msg, va);
	va_end(va);
	exit(err);
}
