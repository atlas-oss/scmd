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
	if (!(d = open_dir("/.scmd/modules/", 15)))
		die(ENOENT, "Could not create module directory.");

	struct dirent *dir;

	if (!d)
		die(ENOENT,
		    "Could not open module directory. Something is really wrong.");

	for (unsigned int i = 0; (dir = readdir(d)) != NULL && i < MAX_MODULE;
	     ++i) {
		names[i] = dir->d_name;
	}

	return 0;
}

int mod_query(mod_t *mod)
{
	short res = -1;
	void *handle;
	char *home = getenv("HOME");
	if (!home)
		die(EXIT_FAILURE, "Could not find $HOME variable.");

	char *err, path[strlen(home) + 16 + strlen(mod->name)];
	strncpy(path, home, strlen(home) + 1);
	strncat(path, "/.scmd/modules/", 15);

	for (size_t i = 0; names[i] != NULL && i < MAX_MODULE;
	     ++i) { // Check if the module exists.
		if (!strcmp(mod->name, names[i])) {
			strncat(path, names[i], strlen(names[i]));
			res = 0;
			break;
		}
	}

	if (-1 == res)
		return res;

	handle = dlopen(path, RTLD_LAZY);

	dlerror();

	*(void **)(&(mod->init)) = dlsym(handle, "init");
	if ((err = dlerror()) != NULL)
		die(EXIT_FAILURE, err);

	write_log("Found symbol init in module %s on %p.", mod->name,
		  mod->init);
	dlerror();

	*(void **)(&(mod->cmd)) = dlsym(handle, "cmd");
	if ((err = dlerror()) != NULL)
		die(EXIT_FAILURE, err);

	write_log("Found symbol cmd in module %s on %p.", mod->name, mod->cmd);
	dlclose(handle);
	return 0;
}
