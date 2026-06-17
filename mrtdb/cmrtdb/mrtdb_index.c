#include <time.h>
#include "mrtdb_index.h"
#include "cJSON.h"

static int mrtdb_key_collect(struct mrtdb* mrtdb, mrtdb_prefix_t *prefix, mrtdb_addr_t *next_hop, mrtdb_as_path_t *as_path)
{
	mrtdb_key_t q;
	q.key_type = KEY_PREFIX;
	q.key = get_hash(prefix, sizeof(mrtdb_prefix_t));
	mrtdb_key_t *res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
	if (res == NULL)
	{
		mrtdb_list_t *tmp = malloc(sizeof(mrtdb_list_t));
		tmp->key_type = q.key_type;
		memcpy(&tmp->prefix, prefix, sizeof(mrtdb_prefix_t));
		memset(tmp->origin_asn, 0, 32 * sizeof(uint32_t));
		tmp->origin_asn_len = 1;
		tmp->origin_asn[0] = as_path->asn[as_path->len - 1];
		for (int j = 0; j < VALUE_MAX + 1; ++j)
		{
			tmp->value_list_len[j] = 0;
			INIT_LIST_HEAD(&tmp->value_list[j]);
		}
		tmp->index = mrtdb->key_list_len[tmp->key_type];
		INIT_LIST_HEAD(&tmp->list);
		list_add_tail(&tmp->list, &mrtdb->key_list[tmp->key_type]);
		++mrtdb->key_list_len[tmp->key_type];

		res = malloc(sizeof(mrtdb_key_t));
		res->key_type = q.key_type;
		res->key = q.key;
		res->ptr = tmp;
		rbtree_insert(&mrtdb->key_rbtree[res->key_type], res);
	}
	else
	{
		if (res->ptr->origin_asn_len < 32)
		{
			int flag = 1;
			for (int j = 0; j < res->ptr->origin_asn_len; ++j)
				if (as_path->asn[as_path->len - 1] == res->ptr->origin_asn[j])
					flag = 0;
			if (flag) res->ptr->origin_asn[res->ptr->origin_asn_len++] = as_path->asn[as_path->len - 1];
		}
	}
	mrtdb_list_t *key_node = res->ptr;

	q.key_type = KEY_NEXTHOP;
	q.key = get_hash(next_hop, sizeof(mrtdb_addr_t));
	res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
	if (res == NULL)
	{
		mrtdb_list_t *tmp = malloc(sizeof(mrtdb_list_t));
		tmp->key_type = q.key_type;
		memcpy(&tmp->next_hop, next_hop, sizeof(mrtdb_addr_t));
		tmp->index = mrtdb->key_list_len[tmp->key_type];
		INIT_LIST_HEAD(&tmp->list);
		list_add_tail(&tmp->list, &mrtdb->key_list[tmp->key_type]);
		++mrtdb->key_list_len[tmp->key_type];

		res = malloc(sizeof(mrtdb_key_t));
		res->key_type = q.key_type;
		res->key = q.key;
		res->ptr = tmp;
		rbtree_insert(&mrtdb->key_rbtree[res->key_type], res);
	}

	mrtdb_value_t *value_node1 = malloc(sizeof(mrtdb_value_t));
	value_node1->value_type = VALUE_NEXTHOP;
	value_node1->ptr = res->ptr;
	value_node1->index = key_node->value_list_len[value_node1->value_type];
	INIT_LIST_HEAD(&value_node1->list);
	list_add_tail(&value_node1->list, &key_node->value_list[value_node1->value_type]);
	++key_node->value_list_len[value_node1->value_type];

	q.key_type = KEY_ASPATH;
	q.key = get_hash(as_path->asn, sizeof(uint32_t) * as_path->len);
	res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
	if (res == NULL)
	{
		mrtdb_list_t *tmp = malloc(sizeof(mrtdb_list_t));
		tmp->key_type = q.key_type;
		tmp->as_path.len = as_path->len;
		tmp->as_path.asn = malloc(sizeof(uint32_t) * tmp->as_path.len);
		memcpy(tmp->as_path.asn, as_path->asn, sizeof(uint32_t) * tmp->as_path.len);
		tmp->as_path_cnt = 0;
		tmp->index = mrtdb->key_list_len[tmp->key_type];
		INIT_LIST_HEAD(&tmp->list);
		list_add_tail(&tmp->list, &mrtdb->key_list[tmp->key_type]);
		++mrtdb->key_list_len[tmp->key_type];

		res = malloc(sizeof(mrtdb_key_t));
		res->key_type = q.key_type;
		res->key = q.key;
		res->ptr = tmp;
		rbtree_insert(&mrtdb->key_rbtree[res->key_type], res);
	}

	++res->ptr->as_path_cnt;
	mrtdb_value_t *value_node2 = malloc(sizeof(mrtdb_value_t));
	value_node2->value_type = VALUE_ASPATH;
	value_node2->ptr = res->ptr;
	value_node2->index = key_node->value_list_len[value_node2->value_type];
	INIT_LIST_HEAD(&value_node2->list);
	list_add_tail(&value_node2->list, &key_node->value_list[value_node2->value_type]);
	++key_node->value_list_len[value_node2->value_type];

	for (int i = 0; i < as_path->len; ++i)
	{
		q.key_type = KEY_AS;
		q.key = get_hash(&as_path->asn[i], sizeof(uint32_t));
		res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
		if (res == NULL)
		{
			mrtdb_list_t *tmp = malloc(sizeof(mrtdb_list_t));
			tmp->key_type = q.key_type;
			tmp->asn = as_path->asn[i];
			tmp->tier = i == 0 ? 1 : 0;
			for (int j = 0; j < VALUE_MAX + 1; ++j)
			{
				tmp->value_list_len[j] = 0;
				INIT_LIST_HEAD(&tmp->value_list[j]);
			}
			tmp->index = mrtdb->key_list_len[tmp->key_type];
			INIT_LIST_HEAD(&tmp->list);
			list_add_tail(&tmp->list, &mrtdb->key_list[tmp->key_type]);
			++mrtdb->key_list_len[tmp->key_type];

			res = malloc(sizeof(mrtdb_key_t));
			res->key_type = q.key_type;
			res->key = q.key;
			res->ptr = tmp;
			rbtree_insert(&mrtdb->key_rbtree[res->key_type], res);
		}
	}

	for (int i = 0; i < as_path->len; ++i)
	{
		q.key_type = KEY_AS;
		q.key = get_hash(&as_path->asn[i], sizeof(uint32_t));
		res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
		key_node = res->ptr;
		if (i - 1 >= 0)
		{
			int flag = 0;
			mrtdb_value_t *pos;
			list_for_each_entry(pos, &key_node->value_list[VALUE_UPSTREAMS], list)
			{
				if (as_path->asn[i - 1] == pos->ptr->asn)
				{
					flag = 1;
					break;
				}
			}
			if (!flag)
			{
				q.key_type = KEY_AS;
				q.key = get_hash(&as_path->asn[i - 1], sizeof(uint32_t));
				res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);

				mrtdb_value_t *value_node3 = malloc(sizeof(mrtdb_value_t));
				value_node3->value_type = VALUE_UPSTREAMS;
				value_node3->ptr = res->ptr;
				value_node3->index = key_node->value_list_len[value_node3->value_type];
				INIT_LIST_HEAD(&value_node3->list);
				list_add_tail(&value_node3->list, &key_node->value_list[value_node3->value_type]);
				++key_node->value_list_len[value_node3->value_type];
			}

		}
		if (i + 1 < as_path->len)
		{
			int flag = 0;
			mrtdb_value_t *pos;
			list_for_each_entry(pos, &key_node->value_list[VALUE_DOWNSTREAMS], list)
			{
				if (as_path->asn[i + 1] == pos->ptr->asn)
				{
					flag = 1;
					break;
				}
			}
			if (!flag)
			{
				q.key_type = KEY_AS;
				q.key = get_hash(&as_path->asn[i + 1], sizeof(uint32_t));
				res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);

				mrtdb_value_t *value_node3 = malloc(sizeof(mrtdb_value_t));
				value_node3->value_type = VALUE_DOWNSTREAMS;
				value_node3->ptr = res->ptr;
				value_node3->index = key_node->value_list_len[value_node3->value_type];
				INIT_LIST_HEAD(&value_node3->list);
				list_add_tail(&value_node3->list, &key_node->value_list[value_node3->value_type]);
				++key_node->value_list_len[value_node3->value_type];
			}
		}
		if (i == as_path->len - 1)
		{
			int flag = 0;
			mrtdb_value_t *pos;
			list_for_each_entry(pos, &key_node->value_list[VALUE_PREFIX], list)
			{
				if (!memcmp(prefix, &pos->ptr->prefix, sizeof(mrtdb_prefix_t)))
				{
					flag = 1;
					break;
				}
			}
			if (!flag)
			{
				q.key_type = KEY_PREFIX;
				q.key = get_hash(prefix, sizeof(mrtdb_prefix_t));
				res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);

				mrtdb_value_t *value_node3 = malloc(sizeof(mrtdb_value_t));
				value_node3->value_type = VALUE_PREFIX;
				value_node3->ptr = res->ptr;
				value_node3->index = key_node->value_list_len[value_node3->value_type];
				INIT_LIST_HEAD(&value_node3->list);
				list_add_tail(&value_node3->list, &key_node->value_list[value_node3->value_type]);
				++key_node->value_list_len[value_node3->value_type];
			}

			flag = 0;
			list_for_each_entry(pos, &key_node->value_list[VALUE_ASPATH], list)
			{
				if (as_path->len == pos->ptr->as_path.len && !memcmp(as_path->asn, pos->ptr->as_path.asn, sizeof(uint32_t) * as_path->len))
				{
					flag = 1;
					break;
				}
			}
			if (!flag)
			{
				q.key_type = KEY_ASPATH;
				q.key = get_hash(as_path->asn, sizeof(uint32_t) * as_path->len);
				res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);

				mrtdb_value_t *value_node4 = malloc(sizeof(mrtdb_value_t));
				value_node4->value_type = VALUE_ASPATH;
				value_node4->ptr = res->ptr;
				value_node4->index = key_node->value_list_len[value_node4->value_type];
				INIT_LIST_HEAD(&value_node4->list);
				list_add_tail(&value_node4->list, &key_node->value_list[value_node4->value_type]);
				++key_node->value_list_len[value_node4->value_type];
			}
		}
	}
	return 0;

}


static int mrtdb_key_dump_prefix(struct mrtdb* mrtdb)
{
	char file_name[64];
	FILE *fvalue = NULL;
	sprintf(file_name, "%s/prefix_value.bin", mrtdb->dir_name);
	fvalue = fopen(file_name, "rb+");
	if (fvalue == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	FILE *fkey = NULL;
	sprintf(file_name, "%s/prefix_key.bin", mrtdb->dir_name);
	fkey = fopen(file_name, "rb+");
	if (fkey == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}


	FILE *fout = NULL;
	sprintf(file_name, "%s/prefix.out", mrtdb->dir_name);
	fout = fopen(file_name, "r+");
	if (fout == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	mrtdb_list_t *pos1;
	list_for_each_entry(pos1, &mrtdb->key_list[KEY_PREFIX], list)
	{
		pos1->offset = ftell(fvalue);
		fwrite(&pos1->prefix, sizeof(mrtdb_prefix_t), 1, fkey);
		fwrite(&pos1->origin_asn_len, sizeof(uint32_t), 1, fkey);
		fwrite(pos1->origin_asn, sizeof(uint32_t), 32, fkey);
		fwrite(&pos1->key_type, sizeof(uint8_t), 1, fkey);
		fwrite(&pos1->index, sizeof(uint32_t), 1, fkey);
		fwrite(&pos1->offset, sizeof(long int), 1, fkey);
		fwrite(pos1->value_list_len, sizeof(uint32_t), VALUE_MAX + 1, fkey);

		mrtdb_value_t *pos2;
		list_for_each_entry(pos2, &pos1->value_list[VALUE_NEXTHOP], list)
		{
			fwrite(&pos2->value_type, sizeof(uint8_t), 1, fvalue);
			fwrite(&pos2->index, sizeof(uint32_t), 1, fvalue);
			fwrite(&pos2->ptr->next_hop, sizeof(mrtdb_addr_t), 1, fvalue);
		}

		list_for_each_entry(pos2, &pos1->value_list[VALUE_ASPATH], list)
		{
			fwrite(&pos2->value_type, sizeof(uint8_t), 1, fvalue);
			fwrite(&pos2->index, sizeof(uint32_t), 1, fvalue);
			fwrite(&pos2->ptr->as_path.len, sizeof(uint32_t), 1, fvalue);
			fwrite(pos2->ptr->as_path.asn, sizeof(uint32_t), pos2->ptr->as_path.len, fvalue);
		}

		char buf[64];
		prefix2string(&pos1->prefix, buf);
		fprintf(fout, "prefix: %s\n", buf);

		list_for_each_entry(pos2, &pos1->value_list[VALUE_NEXTHOP], list)
		{
			ip2string(&pos2->ptr->next_hop, buf);
			fprintf(fout, "next_hop[%u]: %s\n", pos2->index, buf);
		}
		list_for_each_entry(pos2, &pos1->value_list[VALUE_ASPATH], list)
		{
			fprintf(fout, "as_path[%u]: ", pos2->index);
			for (int i = 0; i < pos2->ptr->as_path.len; ++i)
				fprintf(fout, "%u%c", pos2->ptr->as_path.asn[i], i == pos2->ptr->as_path.len - 1 ? '\n' : ' ');
		}

	}

	fclose(fout);
	fclose(fkey);
	fclose(fvalue);

	return 0;
}


static int mrtdb_key_dump_as(struct mrtdb* mrtdb)
{
	char file_name[64];
	FILE *fvalue = NULL;
	sprintf(file_name, "%s/as_value.bin", mrtdb->dir_name);
	fvalue = fopen(file_name, "rb+");
	if (fvalue == NULL)
	{
		printf("open file .%s error\n", file_name);
		return -1;
	}

	FILE *fkey = NULL;
	sprintf(file_name, "%s/as_key.bin", mrtdb->dir_name);
	fkey = fopen(file_name, "rb+");
	if (fkey == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}


	FILE *fout = NULL;
	sprintf(file_name, "%s/as.out", mrtdb->dir_name);
	fout = fopen(file_name, "r+");
	if (fout == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	mrtdb_list_t *pos1;
	list_for_each_entry(pos1, &mrtdb->key_list[KEY_AS], list)
	{
		pos1->offset = ftell(fvalue);
		fwrite(&pos1->asn, sizeof(uint32_t), 1, fkey);
		fwrite(&pos1->tier, sizeof(uint8_t), 1, fkey);
		fwrite(&pos1->key_type, sizeof(uint8_t), 1, fkey);
		fwrite(&pos1->index, sizeof(uint32_t), 1, fkey);
		fwrite(&pos1->offset, sizeof(long int), 1, fkey);
		fwrite(pos1->value_list_len, sizeof(uint32_t), VALUE_MAX + 1, fkey);

		mrtdb_value_t *pos2;
		list_for_each_entry(pos2, &pos1->value_list[VALUE_PREFIX], list)
		{
			fwrite(&pos2->value_type, sizeof(uint8_t), 1, fvalue);
			fwrite(&pos2->index, sizeof(uint32_t), 1, fvalue);
			fwrite(&pos2->ptr->prefix, sizeof(mrtdb_prefix_t), 1, fvalue);
		}

		list_for_each_entry(pos2, &pos1->value_list[VALUE_UPSTREAMS], list)
		{
			fwrite(&pos2->value_type, sizeof(uint8_t), 1, fvalue);
			fwrite(&pos2->index, sizeof(uint32_t), 1, fvalue);
			fwrite(&pos2->ptr->asn, sizeof(uint32_t), 1, fvalue);
		}

		list_for_each_entry(pos2, &pos1->value_list[VALUE_DOWNSTREAMS], list)
		{
			fwrite(&pos2->value_type, sizeof(uint8_t), 1, fvalue);
			fwrite(&pos2->index, sizeof(uint32_t), 1, fvalue);
			fwrite(&pos2->ptr->asn, sizeof(uint32_t), 1, fvalue);
		}

		list_for_each_entry(pos2, &pos1->value_list[VALUE_ASPATH], list)
		{
			fwrite(&pos2->value_type, sizeof(uint8_t), 1, fvalue);
			fwrite(&pos2->index, sizeof(uint32_t), 1, fvalue);
			fwrite(&pos2->ptr->as_path.len, sizeof(uint32_t), 1, fvalue);
			fwrite(pos2->ptr->as_path.asn, sizeof(uint32_t), pos2->ptr->as_path.len, fvalue);
		}

		fprintf(fout, "asn: %u\n", pos1->asn);

		list_for_each_entry(pos2, &pos1->value_list[VALUE_PREFIX], list)
		{
			char buf[64];
			prefix2string(&pos2->ptr->prefix, buf);
			fprintf(fout, "prefix[%u]: %s\n", pos2->index, buf);
		}

		list_for_each_entry(pos2, &pos1->value_list[VALUE_UPSTREAMS], list)
		{
			fprintf(fout, "upstreams[%u]: %u\n", pos2->index, pos2->ptr->asn);
		}

		list_for_each_entry(pos2, &pos1->value_list[VALUE_DOWNSTREAMS], list)
		{
			fprintf(fout, "downstreams[%u]: %u\n", pos2->index, pos2->ptr->asn);
		}

		list_for_each_entry(pos2, &pos1->value_list[VALUE_ASPATH], list)
		{
			fprintf(fout, "as_path[%u]: ", pos2->index);
			for (int i = 0; i < pos2->ptr->as_path.len; ++i)
				fprintf(fout, "%u%c", pos2->ptr->as_path.asn[i], i == pos2->ptr->as_path.len - 1 ? '\n' : ' ');
		}

	}

	fclose(fout);
	fclose(fkey);
	fclose(fvalue);
	return 0;
}

static int mrtdb_key_dump_as_path(struct mrtdb* mrtdb)
{
	FILE *fbin = NULL;
	char file_name[64];
	sprintf(file_name, "%s/as_path.bin", mrtdb->dir_name);
	fbin = fopen(file_name, "rb+");
	if (fbin == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}
	fseek(fbin, 0, SEEK_END);

	FILE *fout = NULL;
	sprintf(file_name, "%s/as_path.out", mrtdb->dir_name);
	fout = fopen(file_name, "r+");
	if (fout == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}
	fseek(fout, 0, SEEK_END);

	mrtdb_list_t *pos;
	list_for_each_entry(pos, &mrtdb->key_list[KEY_ASPATH], list)
	{
		fwrite(&pos->as_path.len, sizeof(uint32_t), 1, fbin);
		fwrite(&pos->as_path_cnt, sizeof(uint32_t), 1, fbin);
		fwrite(&pos->key_type, sizeof(uint8_t), 1, fbin);
		fwrite(&pos->index, sizeof(uint32_t), 1, fbin);
		fwrite(pos->as_path.asn, sizeof(uint32_t), pos->as_path.len, fbin);

		fprintf(fout, "as_path_id: %u\n", pos->index);
		fprintf(fout, "as_path_cnt: %u\n", pos->as_path_cnt);
		fprintf(fout, "as_path_len: %u\n", pos->as_path.len);
		fprintf(fout, "as_path: ");
		for (int j = 0; j < pos->as_path.len; ++j)
			fprintf(fout, "%u%c", pos->as_path.asn[j], j == pos->as_path.len - 1 ? '\n' : ' ');
	}

	fclose(fout);
	fclose(fbin);
	return 0;
}

static int mrtdb_key_dump_next_hop(struct mrtdb* mrtdb)
{

	char file_name[64];
	FILE *fbin = NULL;
	sprintf(file_name, "%s/next_hop.bin", mrtdb->dir_name);
	fbin = fopen(file_name, "rb+");
	if (fbin == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	FILE *fout = NULL;
	sprintf(file_name, "%s/next_hop.out", mrtdb->dir_name);
	fout = fopen(file_name, "r+");
	if (fout == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	mrtdb_list_t *pos;
	list_for_each_entry(pos, &mrtdb->key_list[KEY_NEXTHOP], list)
	{
		fwrite(&pos->next_hop, sizeof(mrtdb_addr_t), 1, fbin);
		fwrite(&pos->key_type, sizeof(uint8_t), 1, fbin);
		fwrite(&pos->index, sizeof(uint32_t), 1, fbin);

		char buf[64];
		ip2string(&pos->next_hop, buf);
		fprintf(fout, "next_hop: %s\n", buf);
	}

	fclose(fout);
	fclose(fbin);
	return 0;
}

static int mrtdb_read_json(struct mrtdb *mrtdb, char *file_name)
{
	FILE *f = NULL;
	f = fopen(file_name, "r");
	if (f == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	fseek(f, 0, SEEK_END);
	long int length = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *buf = malloc(length + 1);
	fread(buf, length, 1, f);

	fclose(f);

	cJSON *root = cJSON_Parse(buf);
	free(buf);

	if (root == NULL)
	{
		printf("parse JSON error\n");
		return -1;
	}

	int cnt = 0;
	for (int i = 0; i < cJSON_GetArraySize(root); ++i)
	{
		cJSON *cjson_entry = cJSON_GetArrayItem(root, i);

		mrtdb_prefix_t prefix;
		if (string2prefix(&prefix, cJSON_GetObjectItem(cjson_entry, "prefix")->valuestring) < 0)
			continue;
		if (prefix.plen == 0) continue;

		cJSON *cjson_attrs = cJSON_GetObjectItem(cjson_entry, "attrs");
		for (int j = 0; j < cJSON_GetArraySize(cjson_attrs); ++j)
		{
			cJSON *cjson_attrs_item = cJSON_GetArrayItem(cjson_attrs, j);

			mrtdb_addr_t next_hop;
			if (string2ip(&next_hop, cJSON_GetObjectItem(cjson_attrs_item, "next_hop")->valuestring) < 0)
				continue;

			cJSON *cjson_as_path = cJSON_GetObjectItem(cjson_attrs_item, "as_path");
			mrtdb_as_path_t as_path;
			as_path.len = cJSON_GetArraySize(cjson_as_path);
			as_path.asn = malloc(as_path.len * sizeof(uint32_t));
			for (int k = 0; k < as_path.len; ++k)
				as_path.asn[k] = cJSON_GetArrayItem(cjson_as_path, k)->valueint;

			mrtdb_key_collect(mrtdb, &prefix, &next_hop, &as_path);
			++cnt;
			free(as_path.asn);
		}

	}

	cJSON_Delete(root);
	return cnt;
}

static int create_file(char *name)
{
	FILE* f = NULL;
	f = fopen(name, "w");
	if (f == NULL)
	{
		printf("create file %s error\n", name);
		return -1;
	}

	fclose(f);
	return 0;
}


int mrtdb_new_index(char *dir_name)
{
	long st, et;
	st = get_system_time_nanosecond();

	char file_name[64];
	sprintf(file_name, "%s/builder.out.test", dir_name);
	create_file(file_name);
	sprintf(file_name, "%s/prefix_value.bin", dir_name);
	create_file(file_name);
	sprintf(file_name, "%s/prefix.out", dir_name);
	create_file(file_name);
	sprintf(file_name, "%s/prefix_key.bin", dir_name);
	create_file(file_name);
	sprintf(file_name, "%s/as_value.bin", dir_name);
	create_file(file_name);
	sprintf(file_name, "%s/as.out", dir_name);
	create_file(file_name);
	sprintf(file_name, "%s/as_key.bin", dir_name);
	create_file(file_name);
	sprintf(file_name, "%s/as_path.bin", dir_name);
	create_file(file_name);
	sprintf(file_name, "%s/as_path.out", dir_name);
	create_file(file_name);
	sprintf(file_name, "%s/next_hop.out", dir_name);
	create_file(file_name);
	sprintf(file_name, "%s/next_hop.bin", dir_name);
	create_file(file_name);


	struct mrtdb* mrtdb = malloc(sizeof(struct mrtdb));
	strcpy(mrtdb->dir_name, dir_name);

	for (int i = 0; i < KEY_MAX + 1; ++i)
	{
		mrtdb->key_list_len[i] = 0;
		INIT_LIST_HEAD(&mrtdb->key_list[i]);
		mrtdb->key_rbtree[i] = RB_ROOT;
	}

	int tot = 0;
	for (int i = 0; 1; ++i)
	{
		sprintf(file_name, "%s/MRT_%d.json", mrtdb->dir_name, i);
		int cnt = mrtdb_read_json(mrtdb, file_name);
		if (cnt < 0) break;
		tot += cnt;

		printf("already deal with %d records\n", tot);
		printf("size of key_prefix = %d\n", mrtdb->key_list_len[KEY_PREFIX]);
		printf("size of key_as = %d\n", mrtdb->key_list_len[KEY_AS]);
		printf("size of key_as_path = %d\n", mrtdb->key_list_len[KEY_ASPATH]);
		printf("size of key_next_hop = %d\n", mrtdb->key_list_len[KEY_NEXTHOP]);

	}

	printf("end\n");
	printf("already deal with %d records\n", tot);
	printf("size of key_prefix = %d\n", mrtdb->key_list_len[KEY_PREFIX]);
	printf("size of key_as = %d\n", mrtdb->key_list_len[KEY_AS]);
	printf("size of key_as_path = %d\n", mrtdb->key_list_len[KEY_ASPATH]);
	printf("size of key_next_hop = %d\n", mrtdb->key_list_len[KEY_NEXTHOP]);

	mrtdb_key_dump_prefix(mrtdb);
	mrtdb_key_dump_as(mrtdb);
	mrtdb_key_dump_as_path(mrtdb);
	mrtdb_key_dump_next_hop(mrtdb);

	mrtdb_list_t *pos1, *n1;
	list_for_each_entry_safe(pos1, n1, &mrtdb->key_list[KEY_PREFIX], list)
	{
		mrtdb_value_t *pos2, *n2;
		list_for_each_entry_safe(pos2, n2, &pos1->value_list[VALUE_NEXTHOP], list)
		{
			list_del(&pos2->list);
			free(pos2);
		}
		list_for_each_entry_safe(pos2, n2, &pos1->value_list[VALUE_ASPATH], list)
		{
			list_del(&pos2->list);
			free(pos2);
		}
		list_del(&pos1->list);
		free(pos1);
	}
	rbtree_free(&mrtdb->key_rbtree[KEY_PREFIX]);

	list_for_each_entry_safe(pos1, n1, &mrtdb->key_list[KEY_AS], list)
	{
		mrtdb_value_t *pos2, *n2;
		list_for_each_entry_safe(pos2, n2, &pos1->value_list[VALUE_PREFIX], list)
		{
			list_del(&pos2->list);
			free(pos2);
		}
		list_for_each_entry_safe(pos2, n2, &pos1->value_list[VALUE_UPSTREAMS], list)
		{
			list_del(&pos2->list);
			free(pos2);
		}
		list_for_each_entry_safe(pos2, n2, &pos1->value_list[VALUE_DOWNSTREAMS], list)
		{
			list_del(&pos2->list);
			free(pos2);
		}
		list_for_each_entry_safe(pos2, n2, &pos1->value_list[VALUE_ASPATH], list)
		{
			list_del(&pos2->list);
			free(pos2);
		}
		list_del(&pos1->list);
		free(pos1);
	}
	rbtree_free(&mrtdb->key_rbtree[KEY_AS]);

	list_for_each_entry_safe(pos1, n1, &mrtdb->key_list[KEY_ASPATH], list)
	{
		free(pos1->as_path.asn);
		list_del(&pos1->list);
		free(pos1);
	}
	rbtree_free(&mrtdb->key_rbtree[KEY_ASPATH]);

	list_for_each_entry_safe(pos1, n1, &mrtdb->key_list[KEY_NEXTHOP], list)
	{
		list_del(&pos1->list);
		free(pos1);
	}
	rbtree_free(&mrtdb->key_rbtree[KEY_NEXTHOP]);

	free(mrtdb);


	et = get_system_time_nanosecond();
	printf("new index time = %ld ms\n", (et - st) / 1000000);

	return 0;
}


int mrtdb_update_index(struct mrtdb* mrtdb)
{

	return 0;
}