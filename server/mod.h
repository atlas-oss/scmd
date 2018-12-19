#ifndef MOD_H
#define MOD_H

typedef struct {
	char *name; 
	void (*init) (); // init the module, called "init"
	void (*cmd) ();  // handles commands from the client, called "cmd"
} mod_t;

int prepare_modloader();   // Returns non-zero on failure
int mod_query(mod_t *mod); // Returns non-zero on failure

#endif
