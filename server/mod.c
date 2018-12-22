#include "mod.h"
#include "def.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

char *names[MAX_MODULE];

int prepare_modloader()
{
        DIR *d = NULL;
	if(!(d = open_dir("/.scmd/modules/", 15)))
		die(ENOENT, "Could not create module directory.");
	
	struct dirent *dir;

	if(!d)
		die(ENOENT, "Could not open module directory. Something is really wrong.");
	
	for (unsigned int i = 0; (dir = readdir(d)) != NULL && i < MAX_MODULE; ++i) {
		names[i] = dir->d_name;
	}

	return 0;
}

int mod_query(mod_t *mod)
{
	void *handle;
	char *err;
	
	short res = -1;
	for (size_t i = 0; names[i] != NULL && i < MAX_MODULE; ++i) { // Check if the module exists.
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
