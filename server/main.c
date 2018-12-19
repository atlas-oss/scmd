#include <stdio.h>

#include "mod.h"

#define VER "v0.01"

int main() {
	printf("scmd_serv %s\n", VER);

	prepare_modloader();
}
