#include "mod.h"
#include "network.h"

#include <stdio.h>
#include <stdlib.h>

#define VER "v0.01"

int main() {
	printf("scmd_serv %s\n", VER);

	// Startup
	if(!open_dir("/.scmd/", 7))
		die(ENOENT, "Could not create config dir.");

	prepare_log();
	write_log("Starting scmd server %s", VER);
	prepare_modloader();

	// Programmflow
	open_connection(PORT);

	mod_t mod = {.name = "test.so"};

	if(0 == mod_query(&mod))
	{
		mod.init();
	} else
		write_log("Could not find module %s", mod.name);
		
	// Cleanup
	close_log();
}
