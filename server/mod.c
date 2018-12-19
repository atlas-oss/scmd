#include "mod.h"
#include "def.h"

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dlfcn.h>

char *names[MAX_MODULE];

int prepare_modloader()
{
	DIR *d;
	struct dirent *dir;
	char *mod_path = getenv("HOME"),
	     *cat = "/.scmd/modules/"; // TODO: Maybe let the user decide where
				       // to put modules.

	if (!mod_path)
		return -1;

	if (!(strncat(mod_path, cat, 15)))
		return -1;

	d = opendir(mod_path);

	if (!d) {
		puts("Couldn't found module directory.");
		char tmp[MAX_MOD_PATH];
		if (strlen(mod_path) >= 2045)
			return -1;

		strncpy(tmp, mod_path, strlen(mod_path) - 8);

		if (0 != mkdir(tmp, S_IRWXU | S_IRWXG))
			return -1;

		if (0 != mkdir(mod_path, S_IRWXU | S_IRWXG))
			return -1;

		puts("Created module directory.");

		d = opendir(mod_path);
	}

	for (unsigned int i = 0; (dir = readdir(d)) != NULL; ++i) {
		names[i] = dir->d_name;
	}

	return 0;
}

int mod_query(mod_t *mod)
{
	void *handle;
	char *err;
	
	short res = -1;
	for (size_t i = 0; names[i] != NULL; ++i) { // Check if the module exists.
		if (strcmp(mod->name, names[i])) {
			res = 0;
			break;
		}
	}

	if (!res)
		return res;

	handle = dlopen(mod->name, RTLD_LAZY);

	dlerror();

	*(void **) (&mod->init) = dlsym(handle, "init");
	if((err = dlerror()) != NULL)
		die(EXIT_FAILURE, "Could not load symbol init from module %s", 1, mod->name);

	dlerror();
	
	*(void **) (&mod->cmd) = dlsym(handle, "cmd");
	if((err = dlerror()) != NULL)
		die(EXIT_FAILURE, "Could not load symbol cmd from module %s", 1, mod->name);

	return 0;
}
