#include "mod.h"
#include "network.h"

#include <stdio.h>
#include <stdlib.h>

#define VER "v0.01"

int main() {
	printf("scmd_serv %s\n", VER);

	// Startup
	prepare_log();
	write_log("Starting scmd server %s", VER);

	if(!open_dir("/.scmd/", 7))
		die(ENOENT, "Could not create config dir.");

	prepare_modloader();

	// Programmflow

	open_connection(PORT);

	// Cleanup
	close_log();
}
