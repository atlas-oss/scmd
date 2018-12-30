#ifndef MOD_H
#define MOD_H

#include "def.h"

int prepare_modloader();   // Returns non-zero on failure
int mod_query(mod_t *mod); // Returns non-zero on failure
int process_cmd(cmd_proto_t *cmd);

#endif
