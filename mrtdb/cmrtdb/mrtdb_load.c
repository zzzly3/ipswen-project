#include "mrtdb_load.h"


#if USER

static int mrtdb_load_key_prefix(struct mrtdb* mrtdb)
{
	FILE *fkey = NULL;
	char file_name[64];

	sprintf(file_name, "%s/prefix_key.bin", mrtdb->dir_name);
	fkey = fopen(file_name, "rb");
	if (fkey == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	int origin_asn_maxlen = 0;
	int value_maxlen[VALUE_MAX + 1] = { 0 };
	while (1)
	{
		mrtdb_list_t *x = malloc(sizeof(mrtdb_list_t));
		int ret = fread(&x->prefix, sizeof(mrtdb_prefix_t), 1, fkey);
		if (ret <= 0)
		{
			free(x);
			break;
		}
		fread(&x->origin_asn_len, sizeof(uint32_t), 1, fkey);
		fread(x->origin_asn, sizeof(uint32_t), 32, fkey);
		fread(&x->key_type, sizeof(uint8_t), 1, fkey);
		fread(&x->index, sizeof(uint32_t), 1, fkey);
		fread(&x->offset, sizeof(long int), 1, fkey);
		fread(x->value_list_len, sizeof(uint32_t), VALUE_MAX + 1, fkey);
		for (int i = 0; i < VALUE_MAX + 1; ++i)
		{
			INIT_LIST_HEAD(&x->value_list[i]);
		}
		INIT_LIST_HEAD(&x->list);
		list_add_tail(&x->list, &mrtdb->key_list[x->key_type]);
		++mrtdb->key_list_len[x->key_type];

		mrtdb_key_t *y = malloc(sizeof(mrtdb_key_t));
		y->key_type = x->key_type;
		y->key = get_hash(&x->prefix, sizeof(mrtdb_prefix_t));
		y->ptr = x;
		rbtree_insert(&mrtdb->key_rbtree[y->key_type], y);

		if (x->origin_asn_len > origin_asn_maxlen) origin_asn_maxlen = x->origin_asn_len;
		for (int i = 0; i < VALUE_MAX + 1; ++i)
			if (x->value_list_len[i] > value_maxlen[i]) value_maxlen[i] = x->value_list_len[i];

	}
	printf("maxlen of origin_asn = %d\n", origin_asn_maxlen);
	for (int i = 0; i < VALUE_MAX + 1; ++i)
		printf("maxlen of value_list[%d] = %d\n",i, value_maxlen[i]);

	fclose(fkey);

	return 0;
}


static int mrtdb_load_key_as(struct mrtdb* mrtdb)
{
	FILE *fkey = NULL;
	char file_name[64];

	sprintf(file_name, "%s/as_key.bin", mrtdb->dir_name);
	fkey = fopen(file_name, "rb");
	if (fkey == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	int value_maxlen[VALUE_MAX + 1] = { 0 };
	while (1)
	{
		mrtdb_list_t *x = malloc(sizeof(mrtdb_list_t));
		int ret = fread(&x->asn, sizeof(uint32_t), 1, fkey);
		if (ret <= 0)
		{
			free(x);
			break;
		}
		fread(&x->tier, sizeof(uint8_t), 1, fkey);
		fread(&x->key_type, sizeof(uint8_t), 1, fkey);
		fread(&x->index, sizeof(uint32_t), 1, fkey);
		fread(&x->offset, sizeof(long int), 1, fkey);
		fread(x->value_list_len, sizeof(uint32_t), VALUE_MAX + 1, fkey);
		for (int i = 0; i < VALUE_MAX + 1; ++i)
		{
			INIT_LIST_HEAD(&x->value_list[i]);
		}
		INIT_LIST_HEAD(&x->list);
		list_add_tail(&x->list, &mrtdb->key_list[x->key_type]);
		++mrtdb->key_list_len[x->key_type];

		mrtdb_key_t *y = malloc(sizeof(mrtdb_key_t));
		y->key_type = x->key_type;
		y->key = get_hash(&x->asn, sizeof(uint32_t));
		y->ptr = x;
		rbtree_insert(&mrtdb->key_rbtree[y->key_type], y);

		for (int i = 0; i < VALUE_MAX + 1; ++i)
			if (x->value_list_len[i] > value_maxlen[i]) value_maxlen[i] = x->value_list_len[i];

	}
	for (int i = 0; i < VALUE_MAX + 1; ++i)
		printf("maxlen of value_list[%d] = %d\n", i, value_maxlen[i]);

	fclose(fkey);

	return 0;
}


static int mrtdb_load_key_as_path(struct mrtdb* mrtdb)
{
	FILE* fbin = NULL;
	char file_name[64];

	sprintf(file_name, "%s/as_path.bin", mrtdb->dir_name);
	fbin = fopen(file_name, "rb");
	if (fbin == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	while (1)
	{
		mrtdb_list_t *x = malloc(sizeof(mrtdb_list_t));
		int ret = fread(&x->as_path.len, sizeof(uint32_t), 1, fbin);
		if (ret <= 0)
		{
			free(x);
			break;
		}
		fread(&x->as_path_cnt, sizeof(uint32_t), 1, fbin);
		fread(&x->key_type, sizeof(uint8_t), 1, fbin);
		fread(&x->index, sizeof(uint32_t), 1, fbin);
		x->as_path.asn = malloc(sizeof(uint32_t) * x->as_path.len);
		fread(x->as_path.asn, sizeof(uint32_t), x->as_path.len, fbin);
		INIT_LIST_HEAD(&x->list);
		list_add_tail(&x->list, &mrtdb->key_list[x->key_type]);
		++mrtdb->key_list_len[x->key_type];

		mrtdb_key_t *y = malloc(sizeof(mrtdb_key_t));
		y->key_type = x->key_type;
		y->key = get_hash(x->as_path.asn, sizeof(uint32_t) * x->as_path.len);
		y->ptr = x;
		rbtree_insert(&mrtdb->key_rbtree[y->key_type], y);
	}

	fclose(fbin);

	return 0;
}

static int mrtdb_load_key_next_hop(struct mrtdb* mrtdb)
{
	FILE *fbin = NULL;
	char file_name[64];

	sprintf(file_name, "%s/next_hop.bin", mrtdb->dir_name);
	fbin = fopen(file_name, "rb");
	if (fbin == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	while (1)
	{
		mrtdb_list_t *x = malloc(sizeof(mrtdb_list_t));
		int ret = fread(&x->next_hop, sizeof(mrtdb_addr_t), 1, fbin);
		if (ret <= 0)
		{
			free(x);
			break;
		}
		fread(&x->key_type, sizeof(uint8_t), 1, fbin);
		fread(&x->index, sizeof(uint32_t), 1, fbin);
		INIT_LIST_HEAD(&x->list);
		list_add_tail(&x->list, &mrtdb->key_list[x->key_type]);
		++mrtdb->key_list_len[x->key_type];

		mrtdb_key_t *y = malloc(sizeof(mrtdb_key_t));
		y->key_type = x->key_type;
		y->key = get_hash(&x->next_hop, sizeof(mrtdb_addr_t));
		y->ptr = x;
		rbtree_insert(&mrtdb->key_rbtree[y->key_type], y);

	}

	fclose(fbin);

	return 0;
}

int mrtdb_key_load_value(struct mrtdb* mrtdb, mrtdb_list_t *x)
{
	if (x->key_type == KEY_PREFIX)
	{
		FILE *fvalue = NULL;
		char file_name[64];
		sprintf(file_name, "%s/prefix_value.bin", mrtdb->dir_name);
		fvalue = fopen(file_name, "rb");
		if (fvalue == NULL)
		{
			printf("open file %s error\n", file_name);
			return -1;
		}

		fseek(fvalue, x->offset, SEEK_SET);

		INIT_LIST_HEAD(&x->value_list[VALUE_NEXTHOP]);
		for (int i = 0; i < x->value_list_len[VALUE_NEXTHOP]; ++i)
		{
			mrtdb_value_t *tmp = malloc(sizeof(mrtdb_value_t));
			fread(&tmp->value_type, sizeof(uint8_t), 1, fvalue);
			fread(&tmp->index, sizeof(uint32_t), 1, fvalue);
			mrtdb_addr_t next_hop;
			fread(&next_hop, sizeof(mrtdb_addr_t), 1, fvalue);

			mrtdb_key_t q;
			q.key_type = KEY_NEXTHOP;
			q.key = get_hash(&next_hop, sizeof(mrtdb_addr_t));
			mrtdb_key_t *res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
			tmp->ptr = res->ptr;

			INIT_LIST_HEAD(&tmp->list);
			list_add_tail(&tmp->list, &x->value_list[tmp->value_type]);
		}

		INIT_LIST_HEAD(&x->value_list[VALUE_ASPATH]);
		for (int i = 0; i < x->value_list_len[VALUE_ASPATH]; ++i)
		{
			mrtdb_value_t *tmp = malloc(sizeof(mrtdb_value_t));
			fread(&tmp->value_type, sizeof(uint8_t), 1, fvalue);
			fread(&tmp->index, sizeof(uint32_t), 1, fvalue);
			mrtdb_as_path_t as_path;
			fread(&as_path.len, sizeof(uint32_t), 1, fvalue);
			as_path.asn = malloc(sizeof(uint32_t) * as_path.len);
			fread(as_path.asn, sizeof(uint32_t), as_path.len, fvalue);

			mrtdb_key_t q;
			q.key_type = KEY_ASPATH;
			q.key = get_hash(as_path.asn, sizeof(uint32_t) * as_path.len);
			mrtdb_key_t *res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
			tmp->ptr = res->ptr;

			free(as_path.asn);

			INIT_LIST_HEAD(&tmp->list);
			list_add_tail(&tmp->list, &x->value_list[tmp->value_type]);
		}

		fclose(fvalue);
	}

	if (x->key_type == KEY_AS)
	{
		FILE *fvalue = NULL;
		char file_name[64];
		sprintf(file_name, "%s/as_value.bin", mrtdb->dir_name);
		fvalue = fopen(file_name, "rb");
		if (fvalue == NULL)
		{
			printf("open file %s error\n", file_name);
			return -1;
		}

		fseek(fvalue, x->offset, SEEK_SET);

		INIT_LIST_HEAD(&x->value_list[VALUE_PREFIX]);
		for (int i = 0; i < x->value_list_len[VALUE_PREFIX]; ++i)
		{
			mrtdb_value_t *tmp = malloc(sizeof(mrtdb_value_t));
			fread(&tmp->value_type, sizeof(uint8_t), 1, fvalue);
			fread(&tmp->index, sizeof(uint32_t), 1, fvalue);
			mrtdb_prefix_t prefix;
			fread(&prefix, sizeof(mrtdb_prefix_t), 1, fvalue);

			mrtdb_key_t q;
			q.key_type = KEY_PREFIX;
			q.key = get_hash(&prefix, sizeof(mrtdb_prefix_t));
			mrtdb_key_t *res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
			tmp->ptr = res->ptr;

			INIT_LIST_HEAD(&tmp->list);
			list_add_tail(&tmp->list, &x->value_list[tmp->value_type]);
		}

		INIT_LIST_HEAD(&x->value_list[VALUE_UPSTREAMS]);
		for (int i = 0; i < x->value_list_len[VALUE_UPSTREAMS]; ++i)
		{
			mrtdb_value_t *tmp = malloc(sizeof(mrtdb_value_t));
			fread(&tmp->value_type, sizeof(uint8_t), 1, fvalue);
			fread(&tmp->index, sizeof(uint32_t), 1, fvalue);
			uint32_t as;
			fread(&as, sizeof(uint32_t), 1, fvalue);

			mrtdb_key_t q;
			q.key_type = KEY_AS;
			q.key = get_hash(&as, sizeof(uint32_t));
			mrtdb_key_t *res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
			tmp->ptr = res->ptr;

			INIT_LIST_HEAD(&tmp->list);
			list_add_tail(&tmp->list, &x->value_list[tmp->value_type]);
		}

		INIT_LIST_HEAD(&x->value_list[VALUE_DOWNSTREAMS]);
		for (int i = 0; i < x->value_list_len[VALUE_DOWNSTREAMS]; ++i)
		{
			mrtdb_value_t *tmp = malloc(sizeof(mrtdb_value_t));
			fread(&tmp->value_type, sizeof(uint8_t), 1, fvalue);
			fread(&tmp->index, sizeof(uint32_t), 1, fvalue);
			uint32_t as;
			fread(&as, sizeof(uint32_t), 1, fvalue);

			mrtdb_key_t q;
			q.key_type = KEY_AS;
			q.key = get_hash(&as, sizeof(uint32_t));
			mrtdb_key_t *res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
			tmp->ptr = res->ptr;

			INIT_LIST_HEAD(&tmp->list);
			list_add_tail(&tmp->list, &x->value_list[tmp->value_type]);
		}

		INIT_LIST_HEAD(&x->value_list[VALUE_ASPATH]);
		for (int i = 0; i < x->value_list_len[VALUE_ASPATH]; ++i)
		{
			mrtdb_value_t *tmp = malloc(sizeof(mrtdb_value_t));
			fread(&tmp->value_type, sizeof(uint8_t), 1, fvalue);
			fread(&tmp->index, sizeof(uint32_t), 1, fvalue);
			mrtdb_as_path_t as_path;
			fread(&as_path.len, sizeof(uint32_t), 1, fvalue);
			as_path.asn = malloc(sizeof(uint32_t) * as_path.len);
			fread(as_path.asn, sizeof(uint32_t), as_path.len, fvalue);

			mrtdb_key_t q;
			q.key_type = KEY_ASPATH;
			q.key = get_hash(as_path.asn, sizeof(uint32_t) * as_path.len);
			mrtdb_key_t *res = rbtree_search(&mrtdb->key_rbtree[q.key_type], &q);
			tmp->ptr = res->ptr;

			free(as_path.asn);

			INIT_LIST_HEAD(&tmp->list);
			list_add_tail(&tmp->list, &x->value_list[tmp->value_type]);
		}

		fclose(fvalue);
	}
	return 0;
}

int mrtdb_key_unload_value(mrtdb_list_t *x)
{
	if (x->key_type == KEY_PREFIX)
	{
		mrtdb_value_t *pos, *n;
		list_for_each_entry_safe(pos, n, &x->value_list[VALUE_NEXTHOP], list)
		{
			list_del(&pos->list);
			free(pos);
		}
		list_for_each_entry_safe(pos, n, &x->value_list[VALUE_ASPATH], list)
		{
			list_del(&pos->list);
			free(pos);
		}
	}

	if (x->key_type == KEY_AS)
	{
		mrtdb_value_t *pos, *n;
		list_for_each_entry_safe(pos, n, &x->value_list[VALUE_PREFIX], list)
		{
			list_del(&pos->list);
			free(pos);
		}
		list_for_each_entry_safe(pos, n, &x->value_list[VALUE_UPSTREAMS], list)
		{
			list_del(&pos->list);
			free(pos);
		}
		list_for_each_entry_safe(pos, n, &x->value_list[VALUE_DOWNSTREAMS], list)
		{
			list_del(&pos->list);
			free(pos);
		}
		list_for_each_entry_safe(pos, n, &x->value_list[VALUE_ASPATH], list)
		{
			list_del(&pos->list);
			free(pos);
		}
	}

	return 0;
}

static int create_file(char *name)
{
	FILE* f = NULL;
	f = fopen(name, "wb");
	if (f == NULL)
	{
		printf("create file %s error\n", name);
		return -1;
	}

	fclose(f);
	return 0;
}

struct mrtdb* mrtdb_init(char* dir_name)
{
	struct mrtdb* mrtdb = malloc(sizeof(struct mrtdb));
	strcpy(mrtdb->dir_name, dir_name);

	for (int i = 0; i < KEY_MAX + 1; ++i)
	{
		mrtdb->key_list_len[i] = 0;
		INIT_LIST_HEAD(&mrtdb->key_list[i]);
		mrtdb->key_rbtree[i] = RB_ROOT;
	}

	mrtdb->prefix_ipv4_cnt = 0;
	mrtdb->prefix_ipv6_cnt = 0;
	mrtdb->prefix_ipswen_cnt = 0;
	mrtdb->route_ipv4_cnt = 0;
	mrtdb->route_ipv6_cnt = 0;
	mrtdb->route_ipswen_cnt = 0;

	printf("sizeof(struct list_head) = %lu\n", sizeof(struct list_head));
	printf("sizeof(struct rb_node) = %lu\n", sizeof(struct rb_node));
	printf("sizeof(mrtdb_list_t) = %lu\n", sizeof(mrtdb_list_t));
	printf("sizeof(mrtdb_key_t) = %lu\n", sizeof(mrtdb_key_t));
	printf("sizeof(mrtdb_value_t) = %lu\n", sizeof(mrtdb_value_t));

	long st, et;
	st = get_system_time_nanosecond();
	mrtdb_load_key_prefix(mrtdb);
	et = get_system_time_nanosecond();
	printf("load key_prefix time = %ld ms\n", (et - st) / 1000000);
	printf("size of key_prefix = %u\n", mrtdb->key_list_len[KEY_PREFIX]);

	st = get_system_time_nanosecond();
	mrtdb_load_key_as(mrtdb);
	et = get_system_time_nanosecond();
	printf("load key_as time = %ld ms\n", (et - st) / 1000000);
	printf("size of key_as = %u\n", mrtdb->key_list_len[KEY_AS]);

	st = get_system_time_nanosecond();
	mrtdb_load_key_as_path(mrtdb);
	et = get_system_time_nanosecond();
	printf("load key_as_path time = %ld ms\n", (et - st) / 1000000);
	printf("size of key_as_path = %u\n", mrtdb->key_list_len[KEY_ASPATH]);

	st = get_system_time_nanosecond();
	mrtdb_load_key_next_hop(mrtdb);
	et = get_system_time_nanosecond();
	printf("load key_next_hop time = %ld ms\n", (et - st) / 1000000);
	printf("size of key_next_hop = %d\n", mrtdb->key_list_len[KEY_NEXTHOP]);

	mrtdb_list_t *pos;
	list_for_each_entry(pos, &mrtdb->key_list[KEY_PREFIX], list)
	{
		if (addr_is_ipv4(&pos->prefix.addr))
		{
			++mrtdb->prefix_ipv4_cnt;
			mrtdb->route_ipv4_cnt += pos->value_list_len[0];
		}
		else if (addr_is_ipswen(&pos->prefix.addr))
		{
			++mrtdb->prefix_ipswen_cnt;
			mrtdb->route_ipswen_cnt += pos->value_list_len[0];
		}
		else
		{
			++mrtdb->prefix_ipv6_cnt;
			mrtdb->route_ipv6_cnt += pos->value_list_len[0];
		}
	}
	printf("prefix_ipv4_cnt = %d\n", mrtdb->prefix_ipv4_cnt);
	printf("prefix_ipv6_cnt = %d\n", mrtdb->prefix_ipv6_cnt);
	printf("prefix_ipswen_cnt = %d\n", mrtdb->prefix_ipswen_cnt);
	printf("route_ipv4_cnt = %d\n", mrtdb->route_ipv4_cnt);
	printf("route_ipv6_cnt = %d\n", mrtdb->route_ipv6_cnt);
	printf("route_ipswen_cnt = %d\n", mrtdb->route_ipswen_cnt);
/*
	char file_name[64];
	sprintf(file_name, "%smap_prefix.out.test", mrtdb->dir_name);
	create_file(file_name);
	sprintf(file_name, "%smap_asn.out.test", mrtdb->dir_name);
	create_file(file_name);
	sprintf(file_name, "%smap_as_path.out.test", mrtdb->dir_name);
	create_file(file_name);
	sprintf(file_name, "%smap_nh.out.test", mrtdb->dir_name);
	create_file(file_name);


	st = get_system_time_nanosecond();
	mrtdb_test_map_prefix(mrtdb);
	et = get_system_time_nanosecond();
	printf("test map_prefix time = %ld ms\n", (et - st) / 1000000);

	st = get_system_time_nanosecond();
	mrtdb_test_map_asn(mrtdb);
	et = get_system_time_nanosecond();
	printf("test map_asn time = %ld ms\n", (et - st) / 1000000);

	st = get_system_time_nanosecond();
	mrtdb_test_map_as_path(mrtdb);
	et = get_system_time_nanosecond();
	printf("test map_as_path time = %ld ms\n", (et - st) / 1000000);

	st = get_system_time_nanosecond();
	mrtdb_test_map_nh(mrtdb);
	et = get_system_time_nanosecond();
	printf("test map_nh time = %ld ms\n", (et - st) / 1000000);
*/

	return mrtdb;
}

int mrtdb_exit(struct mrtdb* mrtdb)
{
	long st, et;
	st = get_system_time_nanosecond();

	mrtdb_list_t *pos, *n;
	list_for_each_entry_safe(pos, n, &mrtdb->key_list[KEY_PREFIX], list)
	{
		list_del(&pos->list);
		free(pos);
	}
	rbtree_free(&mrtdb->key_rbtree[KEY_PREFIX]);

	list_for_each_entry_safe(pos, n, &mrtdb->key_list[KEY_AS], list)
	{
		list_del(&pos->list);
		free(pos);
	}
	rbtree_free(&mrtdb->key_rbtree[KEY_AS]);

	list_for_each_entry_safe(pos, n, &mrtdb->key_list[KEY_ASPATH], list)
	{
		free(pos->as_path.asn);
		list_del(&pos->list);
		free(pos);
	}
	rbtree_free(&mrtdb->key_rbtree[KEY_ASPATH]);

	list_for_each_entry_safe(pos, n, &mrtdb->key_list[KEY_NEXTHOP], list)
	{
		list_del(&pos->list);
		free(pos);
	}
	rbtree_free(&mrtdb->key_rbtree[KEY_NEXTHOP]);

	free(mrtdb);
	et = get_system_time_nanosecond();
	printf("exit time = %ld ms\n", (et - st) / 1000000);

	return 0;
}

#endif // USER

#if KERNEL

static int mrtdb_test_map_as_path(struct mrtdb* mrtdb)
{
	char file_name[64];
	FileIO fout;
	int i;
	mrtdb_index_as_path_t *pos;
	ktime_t st, et;
	ktime_t query_time = ktime_set(0, 0);

	fout.pos = 0;
	sprintf(file_name, "%smap_as_path.out.test", mrtdb->dir_name);
	fout.f = filp_open(file_name, O_CREAT | O_RDWR | O_LARGEFILE, 00664);
	if (IS_ERR(fout.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	i = 0;
	list_for_each_entry(pos, &mrtdb->as_path_index, list)
	{
		char buf[64];
		int j;

		mrtdb_key_as_path_t q;
		q.as_path = pos->as_path;
		q.as_path_len = pos->as_path_len;
		q.as_path_id = i;
		q.key = get_hash(q.as_path, sizeof(uint32_t) * q.as_path_len);
		st = ktime_get();
		rbtree_search_as_path(&mrtdb->map_as_path, &q);
		et = ktime_get();
		query_time = ktime_add(query_time, ktime_sub(et, st));

		sprintf(buf, "as_path_cnt = %u\n", pos->cnt);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "as_path_list_offset = %ld\n", pos->as_path_list_offset);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "as_path_len = %u\n", pos->as_path_len);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "as_path = ");
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		for (j = 0; j < pos->as_path_len; ++j)
		{
			sprintf(buf, "%u%c", pos->as_path[j], j == pos->as_path_len - 1 ? '\n' : ' ');
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		}
		++i;
	}

	filp_close(fout.f, NULL);
	printk("mrtdb_kernel: test_as_path_query time = %lld ms\n", query_time / 1000000);
	return 0;
}

static int mrtdb_test_map_nh(struct mrtdb* mrtdb)
{
	char file_name[64];
	FileIO fkey, fout;
	int i;
	ktime_t st, et;
	ktime_t query_time = ktime_set(0, 0);
	fkey.pos = fout.pos = 0;

	sprintf(file_name, "%smap_nh_key.bin", mrtdb->dir_name);
	fkey.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fkey.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	sprintf(file_name, "%smap_nh.out.test", mrtdb->dir_name);
	fout.f = filp_open(file_name, O_CREAT | O_RDWR | O_LARGEFILE, 00664);
	if (IS_ERR(fout.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	while (1)
	{
		mrtdb_key_nh_t q, *x;
		mrtdb_value_as_path_id_list_t *pos;
		mrtdb_value_prefix_list_t *pos1;
		char buf[64];
		fkey.ret = kernel_read(fkey.f, &q, 56, &fkey.pos);
		if (fkey.ret <= 0) break;

		q.key = get_hash(&q.nh, sizeof(mrtdb_addr_t));
		st = ktime_get();
		x = rbtree_search_nh(&mrtdb->map_nh, &q);
		et = ktime_get();
		query_time = ktime_add(query_time, ktime_sub(et, st));
		if (x == NULL)
		{
			printk("mrtdb_kernel: cannot find node in rbtree.(key_nh)\n");
			continue;
		}

		mrtdb_map_nh_load_value(mrtdb, x);

		sprintf(buf, "next_hop: ");
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		ip2string(&x->nh, buf);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "\n");
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "offset: %ld\n", x->as_path_id_list_offset);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);

		sprintf(buf, "id_list_len: %u\n", x->as_path_id_list_len);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "id_list: ");
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);


		i = 0;
		list_for_each_entry(pos, &x->as_path_id_list, list)
		{
			sprintf(buf, "%u%c", pos->as_path_id, i == x->as_path_id_list_len - 1 ? '\n' : ' ');
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
			++i;
		}

		sprintf(buf, "prefix_list_len: %u\n", x->prefix_list_len);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "prefix_list: ");
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);

		i = 0;
		list_for_each_entry(pos1, &x->prefix_list, list)
		{
			prefix2string(&pos1->prefix, buf);
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
			sprintf(buf, "%c", i == x->prefix_list_len - 1 ? '\n' : ' ');
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
			++i;
		}

		mrtdb_map_nh_unload_value(x);

	}
	filp_close(fkey.f, NULL);
	filp_close(fout.f, NULL);
	printk("mrtdb_kernel: test_nh_query time = %lld ms\n", query_time / 1000000);
	return 0;
}
static int mrtdb_test_map_asn(struct mrtdb* mrtdb)
{
	char file_name[64];
	FileIO fkey, fout;
	int i;
	ktime_t st, et;
	ktime_t query_time = ktime_set(0, 0);
	fkey.pos = fout.pos = 0;

	sprintf(file_name, "%smap_asn_key.bin", mrtdb->dir_name);
	fkey.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fkey.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	sprintf(file_name, "%smap_asn.out.test", mrtdb->dir_name);
	fout.f = filp_open(file_name, O_CREAT | O_RDWR | O_LARGEFILE, 00664);
	if (IS_ERR(fout.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	while(1)
	{
		mrtdb_key_asn_t q, *x;
		mrtdb_value_as_path_id_list_t *pos;
		mrtdb_value_prefix_list_t *pos1;
		char buf[64];
		fkey.ret = kernel_read(fkey.f, &q, 40, &fkey.pos);
		if (fkey.ret <= 0) break;

		q.key = get_hash(&q.asn, sizeof(uint32_t));
		st = ktime_get();
		x = rbtree_search_asn(&mrtdb->map_asn, &q);
		et = ktime_get();
		query_time = ktime_add(query_time, ktime_sub(et, st));
		if (x == NULL)
		{
			printk("mrtdb_kernel: cannot find node in rbtree.(key_asn)\n");
			continue;
		}

		mrtdb_map_asn_load_value(mrtdb, x);

		sprintf(buf, "asn: %u\n", x->asn);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "offset: %ld\n", x->as_path_id_list_offset);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);

		sprintf(buf, "id_list_len: %u\n", x->as_path_id_list_len);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "id_list: ");
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);

		i = 0;
		list_for_each_entry(pos, &x->as_path_id_list, list)
		{
			sprintf(buf, "%u%c", pos->as_path_id, i == x->as_path_id_list_len - 1 ? '\n' : ' ');
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
			++i;
		}

		sprintf(buf, "prefix_list_len: %u\n", x->prefix_list_len);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "prefix_list: ");
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);

		i = 0;
		list_for_each_entry(pos1, &x->prefix_list, list)
		{
			prefix2string(&pos1->prefix, buf);
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
			sprintf(buf, "%c", i == x->prefix_list_len - 1 ? '\n' : ' ');
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
			++i;
		}

		mrtdb_map_asn_unload_value(x);

	}
	filp_close(fkey.f, NULL);
	filp_close(fout.f, NULL);
	printk("mrtdb_kernel: test_asn_query time = %lld ms\n", query_time / 1000000);
	return 0;
}

static int mrtdb_test_map_prefix(struct mrtdb* mrtdb)
{
	char file_name[64];
	FileIO fkey, fout;
	int i;
	ktime_t st, et;
	ktime_t query_time = ktime_set(0, 0);
	fkey.pos = fout.pos = 0;

	sprintf(file_name, "%smap_prefix_key.bin", mrtdb->dir_name);
	fkey.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fkey.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	sprintf(file_name, "%smap_prefix.out.test", mrtdb->dir_name);
	fout.f = filp_open(file_name, O_CREAT | O_RDWR | O_LARGEFILE, 00664);
	if (IS_ERR(fout.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	while (1)
	{
		mrtdb_key_prefix_t q, *x;
		mrtdb_value_nh_list_t *pos;
		char buf[64];
		fkey.ret = kernel_read(fkey.f, &q, 72, &fkey.pos);
		if (fkey.ret <= 0) break;

		q.key = get_hash(&q.prefix, sizeof(mrtdb_prefix_t));
		st = ktime_get();
		x = rbtree_search_prefix(&mrtdb->map_prefix, &q);
		et = ktime_get();
		query_time = ktime_add(query_time, ktime_sub(et, st));
		if (x == NULL)
		{
			printk("mrtdb_kernel: cannot find node in rbtree.(key_prefix)\n");
			continue;
		}

		mrtdb_map_prefix_load_value(mrtdb, x);

		sprintf(buf, "prefix: ");
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		prefix2string(&x->prefix, buf);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		sprintf(buf, "\n");
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);

		sprintf(buf, "offset: %ld\n", x->nh_list_offset);
		fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);

		if (x->origin_asn_len == 1)
		{
			sprintf(buf, "asn: %u\n", x->origin_asn[0]);
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		}
		else
		{
			sprintf(buf, "asn: {");
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);

			for (i = 0; i < x->origin_asn_len; ++i)
			{
				sprintf(buf, "%u%c", x->origin_asn[i], i == x->origin_asn_len - 1 ? '}' : ',');
				fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
			}
			sprintf(buf, "\n");
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
		}


		i = 0;
		list_for_each_entry(pos, &x->nh_list, list)
		{
			sprintf(buf, "next_hop[%d]: ", i);
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
			ip2string(&pos->next_hop, buf);
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
			sprintf(buf, "\n");
			fout.ret = kernel_write(fout.f, buf, strlen(buf), &fout.pos);
			++i;
		}

		mrtdb_map_prefix_unload_value(x);

	}
	filp_close(fkey.f, NULL);
	filp_close(fout.f, NULL);
	printk("mrtdb_kernel: test_prefix_query time = %lld ms\n", query_time / 1000000);
	return 0;
}


static int mrtdb_load_map_prefix(struct mrtdb* mrtdb)
{
	char file_name[64];
	int maxlen = 0;
	FileIO fkey;
	fkey.pos = 0;

	sprintf(file_name, "%smap_prefix_key.bin", mrtdb->dir_name);
	fkey.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fkey.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}


	while (1)
	{
		mrtdb_key_prefix_t *x = kvmalloc(sizeof(mrtdb_key_prefix_t), GFP_KERNEL);
		fkey.ret = kernel_read(fkey.f, x, 72, &fkey.pos);
		if (fkey.ret <= 0) break;

		INIT_LIST_HEAD(&x->nh_list);

		x->key = get_hash(&x->prefix, sizeof(mrtdb_prefix_t));
		rbtree_insert_prefix(&mrtdb->map_prefix, x);

		if (x->nh_list_len > maxlen) maxlen = x->nh_list_len;

	}
	printk("mrtdb_kernel: maxlen of nh_list = %d\n", maxlen);

	filp_close(fkey.f, NULL);

	return 0;
}


static int mrtdb_load_map_asn(struct mrtdb* mrtdb)
{
	char file_name[64];
	int maxlen = 0;
	FileIO fkey;
	fkey.pos = 0;

	sprintf(file_name, "%smap_asn_key.bin", mrtdb->dir_name);
	fkey.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fkey.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	while (1)
	{
		mrtdb_key_asn_t *x = kvmalloc(sizeof(mrtdb_key_asn_t), GFP_KERNEL);
		fkey.ret = kernel_read(fkey.f, x, 40, &fkey.pos);
		if (fkey.ret <= 0) break;

		INIT_LIST_HEAD(&x->as_path_id_list);
		INIT_LIST_HEAD(&x->prefix_list);

		x->key = get_hash(&x->asn, sizeof(uint32_t));
		rbtree_insert_asn(&mrtdb->map_asn, x);

		if (x->as_path_id_list_len > maxlen) maxlen = x->as_path_id_list_len;

	}
	printk("mrtdb_kernel: maxlen of as_path_id_list = %d\n", maxlen);

	filp_close(fkey.f, NULL);

	return 0;
}


static int mrtdb_load_map_as_path(struct mrtdb* mrtdb)
{
	FileIO fkey, fvalue;
	int i;
	mrtdb_index_as_path_t *pos;
	char file_name[64];

	fkey.pos = fvalue.pos = 0;

	sprintf(file_name, "%smap_as_path_key.bin", mrtdb->dir_name);
	fkey.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fkey.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	fkey.ret = kernel_read(fkey.f, &mrtdb->as_path_index_len, sizeof(uint32_t), &fkey.pos);
	for (i = 0; i < mrtdb->as_path_index_len; ++i)
	{
		mrtdb_index_as_path_t *tmp = kvmalloc(sizeof(mrtdb_index_as_path_t), GFP_KERNEL);
		fkey.ret = kernel_read(fkey.f, tmp, 16, &fkey.pos);
		INIT_LIST_HEAD(&tmp->list);
		list_add_tail(&tmp->list, &mrtdb->as_path_index);
	}


	filp_close(fkey.f, NULL);


	sprintf(file_name, "%smap_as_path_value.bin", mrtdb->dir_name);
	fvalue.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fvalue.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}


	i = 0;
	list_for_each_entry(pos, &mrtdb->as_path_index, list)
	{
		mrtdb_key_as_path_t *x = kvmalloc(sizeof(mrtdb_key_as_path_t), GFP_KERNEL);

		pos->as_path = kvmalloc(sizeof(uint32_t) * pos->as_path_len, GFP_KERNEL);
		fvalue.ret = kernel_read(fvalue.f, pos->as_path, sizeof(uint32_t) * pos->as_path_len, &fvalue.pos);


		x->as_path = pos->as_path;
		x->as_path_len = pos->as_path_len;
		x->as_path_id = i;
		x->ptr = pos;
		x->key = get_hash(x->as_path, sizeof(uint32_t) * x->as_path_len);
		rbtree_insert_as_path(&mrtdb->map_as_path, x);
		++i;
	}

	filp_close(fvalue.f, NULL);
	return 0;
}

static int mrtdb_load_map_nh(struct mrtdb* mrtdb)
{
	char file_name[64];
	FileIO fkey;
	fkey.pos = 0;

	sprintf(file_name, "%smap_nh_key.bin", mrtdb->dir_name);
	fkey.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fkey.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	while (1)
	{
		mrtdb_key_nh_t *x = kvmalloc(sizeof(mrtdb_key_nh_t), GFP_KERNEL);
		fkey.ret = kernel_read(fkey.f, x, 56, &fkey.pos);
		if (fkey.ret <= 0) break;

		INIT_LIST_HEAD(&x->as_path_id_list);
		INIT_LIST_HEAD(&x->prefix_list);

		x->key = get_hash(&x->nh, sizeof(mrtdb_addr_t));
		rbtree_insert_nh(&mrtdb->map_nh, x);
	}

	filp_close(fkey.f, NULL);

	return 0;
}

int mrtdb_map_prefix_load_value(struct mrtdb* mrtdb, mrtdb_key_prefix_t *x)
{
	char file_name[64];
	int i;
	FileIO fvalue;
	fvalue.pos = 0;

	sprintf(file_name, "%smap_prefix_value.bin", mrtdb->dir_name);
	fvalue.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fvalue.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	fvalue.pos = x->nh_list_offset;

	INIT_LIST_HEAD(&x->nh_list);
	for (i = 0; i < x->nh_list_len; ++i)
	{
		mrtdb_value_nh_list_t* tmp = kvmalloc(sizeof(mrtdb_value_nh_list_t), GFP_KERNEL);
		fvalue.ret = kernel_read(fvalue.f, tmp, 40, &fvalue.pos);
		INIT_LIST_HEAD(&tmp->list);
		list_add_tail(&tmp->list, &x->nh_list);
	}

	filp_close(fvalue.f, NULL);
	return 0;
}

int mrtdb_map_prefix_unload_value(mrtdb_key_prefix_t *x)
{
	mrtdb_value_nh_list_t *pos1, *n1;

	list_for_each_entry_safe(pos1, n1, &x->nh_list, list)
	{
		list_del(&pos1->list);
		kvfree(pos1);
	}

	return 0;
}

int mrtdb_map_asn_load_value(struct mrtdb* mrtdb, mrtdb_key_asn_t *x)
{
	char file_name[64];
	int i;
	FileIO fvalue;
	fvalue.pos = 0;

	sprintf(file_name, "%smap_asn_value.bin", mrtdb->dir_name);
	fvalue.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fvalue.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	fvalue.pos = x->as_path_id_list_offset;

	INIT_LIST_HEAD(&x->as_path_id_list);
	for (i = 0; i < x->as_path_id_list_len; ++i)
	{
		mrtdb_value_as_path_id_list_t* tmp = kvmalloc(sizeof(mrtdb_value_as_path_id_list_t), GFP_KERNEL);
		fvalue.ret = kernel_read(fvalue.f, tmp, 16, &fvalue.pos);
		INIT_LIST_HEAD(&tmp->list);
		list_add_tail(&tmp->list, &x->as_path_id_list);
	}

	fvalue.pos = x->prefix_list_offset;

	INIT_LIST_HEAD(&x->prefix_list);
	for (i = 0; i < x->prefix_list_len; ++i)
	{
		mrtdb_value_prefix_list_t* tmp = kvmalloc(sizeof(mrtdb_value_prefix_list_t), GFP_KERNEL);
		fvalue.ret = kernel_read(fvalue.f, tmp, 24, &fvalue.pos);
		INIT_LIST_HEAD(&tmp->list);
		list_add_tail(&tmp->list, &x->prefix_list);
	}

	filp_close(fvalue.f, NULL);
	return 0;
}

int mrtdb_map_asn_unload_value(mrtdb_key_asn_t *x)
{
	mrtdb_value_prefix_list_t *pos, *n;
	mrtdb_value_as_path_id_list_t *pos1, *n1;

	list_for_each_entry_safe(pos, n, &x->prefix_list, list)
	{
		list_del(&pos->list);
		kvfree(pos);
	}

	list_for_each_entry_safe(pos1, n1, &x->as_path_id_list, list)
	{
		list_del(&pos1->list);
		kvfree(pos1);
	}

	return 0;
}

int mrtdb_map_nh_load_value(struct mrtdb* mrtdb, mrtdb_key_nh_t *x)
{
	char file_name[64];
	int i;
	FileIO fvalue;
	fvalue.pos = 0;

	sprintf(file_name, "%smap_nh_value.bin", mrtdb->dir_name);
	fvalue.f = filp_open(file_name, O_RDONLY | O_LARGEFILE, 0);
	if (IS_ERR(fvalue.f))
	{
		printk("mrtdb_kernel: cannot open file \"%s\"\n", file_name);
		return -1;
	}

	fvalue.pos = x->as_path_id_list_offset;

	INIT_LIST_HEAD(&x->as_path_id_list);
	for (i = 0; i < x->as_path_id_list_len; ++i)
	{
		mrtdb_value_as_path_id_list_t* tmp = kvmalloc(sizeof(mrtdb_value_as_path_id_list_t), GFP_KERNEL);
		fvalue.ret = kernel_read(fvalue.f, tmp, 16, &fvalue.pos);
		INIT_LIST_HEAD(&tmp->list);
		list_add_tail(&tmp->list, &x->as_path_id_list);
	}

	fvalue.pos = x->prefix_list_offset;

	INIT_LIST_HEAD(&x->prefix_list);
	for (i = 0; i < x->prefix_list_len; ++i)
	{
		mrtdb_value_prefix_list_t* tmp = kvmalloc(sizeof(mrtdb_value_prefix_list_t), GFP_KERNEL);
		fvalue.ret = kernel_read(fvalue.f, tmp, 24, &fvalue.pos);
		INIT_LIST_HEAD(&tmp->list);
		list_add_tail(&tmp->list, &x->prefix_list);
	}

	filp_close(fvalue.f, NULL);
	return 0;
}

int mrtdb_map_nh_unload_value(mrtdb_key_nh_t *x)
{
	mrtdb_value_prefix_list_t *pos, *n;
	mrtdb_value_as_path_id_list_t *pos1, *n1;

	list_for_each_entry_safe(pos, n, &x->prefix_list, list)
	{
		list_del(&pos->list);
		kvfree(pos);
	}

	list_for_each_entry_safe(pos1, n1, &x->as_path_id_list, list)
	{
		list_del(&pos1->list);
		kvfree(pos1);
	}

	return 0;
}


struct mrtdb* mrtdb_init(char* dir_name)
{
	struct mrtdb* mrtdb = kvmalloc(sizeof(struct mrtdb), GFP_KERNEL);
	ktime_t st, et, diff;
	strcpy(mrtdb->dir_name, dir_name);

	mrtdb->as_path_index_len = 0;
	INIT_LIST_HEAD(&mrtdb->as_path_index);
	mrtdb->map_as_path = RB_ROOT;
	mrtdb->map_prefix = RB_ROOT;
	mrtdb->map_asn = RB_ROOT;
	mrtdb->map_nh = RB_ROOT;

	printk("mrtdb_kernel: sizeof(struct rb_node) = %lu\n", sizeof(struct rb_node));
	printk("mrtdb_kernel: sizeof(mrtdb_key_prefix_t) = %lu\n", sizeof(mrtdb_key_prefix_t));
	printk("mrtdb_kernel: sizeof(mrtdb_key_asn_t) = %lu\n", sizeof(mrtdb_key_asn_t));
	printk("mrtdb_kernel: sizeof(mrtdb_key_as_path_t) = %lu\n", sizeof(mrtdb_key_as_path_t));
	printk("mrtdb_kernel: sizeof(mrtdb_key_nh_t) = %lu\n", sizeof(mrtdb_key_nh_t));



	st = ktime_get();
	mrtdb_load_map_prefix(mrtdb);
	printk("mrtdb_kernel: size of map_prefix = %d\n", rbtree_count(&mrtdb->map_prefix));
	et = ktime_get();
	diff = ktime_sub(et, st);
	printk("mrtdb_kernel: load map_prefix time = %lld ms\n", diff / 1000000);

	st = ktime_get();
	mrtdb_load_map_asn(mrtdb);
	printk("mrtdb_kernel: size of map_asn = %d\n", rbtree_count(&mrtdb->map_asn));
	et = ktime_get();
	diff = ktime_sub(et, st);
	printk("mrtdb_kernel: load map_asn time = %lld ms\n", diff / 1000000);


	st = ktime_get();
	mrtdb_load_map_as_path(mrtdb);
	printk("mrtdb_kernel: size of map_as_path = %d\n", rbtree_count(&mrtdb->map_as_path));
	et = ktime_get();
	diff = ktime_sub(et, st);
	printk("mrtdb_kernel: load map_as_path time = %lld ms\n", diff / 1000000);


	st = ktime_get();
	mrtdb_load_map_nh(mrtdb);
	printk("mrtdb_kernel: size of map_nh = %d\n", rbtree_count(&mrtdb->map_nh));
	et = ktime_get();
	diff = ktime_sub(et, st);
	printk("mrtdb_kernel: load map_nh time = %lld ms\n", diff / 1000000);


/*
	st = ktime_get();
	mrtdb_test_map_prefix(mrtdb);
	et = ktime_get();
	diff = ktime_sub(et, st);
	printk("mrtdb_kernel: test map_prefix time = %lld ms\n", diff / 1000000);

	st = ktime_get();
	mrtdb_test_map_asn(mrtdb);
	et = ktime_get();
	diff = ktime_sub(et, st);
	printk("mrtdb_kernel: test map_asn time = %lld ms\n", diff / 1000000);

	st = ktime_get();
	mrtdb_test_map_as_path(mrtdb);
	et = ktime_get();
	diff = ktime_sub(et, st);
	printk("mrtdb_kernel: test map_as_path time = %lld ms\n", diff / 1000000);

	st = ktime_get();
	mrtdb_test_map_nh(mrtdb);
	et = ktime_get();
	diff = ktime_sub(et, st);
	printk("mrtdb_kernel: test map_nh time = %lld ms\n", diff / 1000000);
*/

	return mrtdb;
}

int mrtdb_exit(struct mrtdb* mrtdb)
{
	ktime_t st, et, diff;
	mrtdb_index_as_path_t *pos, *n;

	st = ktime_get();

	rbtree_free_prefix(&mrtdb->map_prefix);
	rbtree_free_asn(&mrtdb->map_asn);


	list_for_each_entry_safe(pos, n, &mrtdb->as_path_index, list)
	{
		list_del(&pos->list);
		kvfree(pos->as_path);
		kvfree(pos);
	}
	rbtree_free_as_path(&mrtdb->map_as_path);

	rbtree_free_nh(&mrtdb->map_nh);

	kvfree(mrtdb);

	et = ktime_get();
	diff = ktime_sub(et, st);
	printk("mrtdb_kernel: exit time = %lld ms\n", diff / 1000000);

	return 0;
}

#endif // KERNEL
