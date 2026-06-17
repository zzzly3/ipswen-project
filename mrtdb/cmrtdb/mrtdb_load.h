#ifndef MRTDB_LOAD_H
#define MRTDB_LOAD_H

#include "mrtdb_format.h"

struct mrtdb* mrtdb_init(char* dir_name);
int mrtdb_exit(struct mrtdb* mrtdb);

int mrtdb_key_load_value(struct mrtdb* mrtdb, mrtdb_list_t *x);

int mrtdb_key_unload_value(mrtdb_list_t *x);

#endif // !MRTDB_LOAD_H
