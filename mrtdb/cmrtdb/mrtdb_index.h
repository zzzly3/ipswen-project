#ifndef MRTDB_INDEX_H
#define MRTDB_INDEX_H

#include "mrtdb_format.h"

int mrtdb_new_index(char *dir_name);

int mrtdb_update_index(struct mrtdb* mrtdb);

#endif // !MRTDB_INDEX_H
