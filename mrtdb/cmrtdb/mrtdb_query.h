#ifndef MRTDB_QUERY_H
#define MRTDB_QUERY_H

#include "mrtdb_format.h"
#include "mrtdb_load.h"

int mrtdb_route_dump(struct mrtdb* mrtdb);

int mrtdb_route_add(char *file_name);

mrtdb_list_t* mrtdb_query_prefix(struct mrtdb* mrtdb, char *q);

mrtdb_list_t* mrtdb_query_as(struct mrtdb* mrtdb, uint32_t q);

mrtdb_value_t* list_entry_value_func(struct list_head *ptr);

mrtdb_list_t* list_entry_key_func(struct list_head *ptr);

#endif // !MRTDB_QUERY_H




