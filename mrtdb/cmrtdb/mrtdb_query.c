#include "mrtdb_query.h"
#include <linux/rtnetlink.h>
#include <linux/netlink.h>

/*
int mrtdb_query_cmd(struct mrtdb* mrtdb)
{
	time_t st, et;

	while (1)
	{
		char input[64];
		printf("\nquery prefix, like 1.0.0.0/24\n");
		printf("query as_number, like 57866\n");
		printf("input q for exit\n");
		printf("input key for query: ");
		scanf("%s", input);
		if (input[0] == 'q') break;
		else
		{
			int flag = 0;
			for (int i = 0; input[i]; ++i)
				if (input[i] == '/') flag = 1;
			if (flag)
			{
				// query prefix
				char prefix_ip[64];
				int prefix_len;
				sscanf(input, "%[^/]/%d", prefix_ip, &prefix_len);

				uint8_t afi = 4;
				for (int i = 0; input[i]; ++i)
					if (input[i] == ':') afi = 6;

				mrtdb_key_prefix_t q;
				memset(&q.prefix, 0, sizeof(q.prefix));
				if (afi == 4)
				{
					int ret = inet_pton(AF_INET, prefix_ip, &q.prefix.prefix4.prefix);
					if (ret <= 0)
					{
						printf("%s format error\n", input);
						continue;
					}
					q.prefix.prefix4.prefix_len = prefix_len;
				}
				else
				{
					int ret = inet_pton(AF_INET6, prefix_ip, &q.prefix.ipv6);
					if (ret <= 0)
					{
						printf("%s format error\n", input);
						continue;
					}
					q.prefix.prefix6.prefix_len = prefix_len;
				}

				time(&st);

				mrtdb_key_prefix_t *ans = hashmap_get(mrtdb->map_prefix, &q);
				if (ans == NULL) printf("%s not found\n", input);
				else mrtdb_prefix_query_result(mrtdb, ans, NULL);

				time(&et);
				printf("query prefix time = %f s\n", difftime(et, st));
			}
			else
			{
				// query as_number
				time(&st);

				mrtdb_key_asn_t q;
				int ret = sscanf(input, "%u", &q.asn);
				if (ret != 1)
				{
					printf("%s format error\n", input);
					continue;
				}
				mrtdb_key_asn_t *ans = hashmap_get(mrtdb->map_asn, &q);
				if (ans == NULL) printf("%s not found\n", input);
				else mrtdb_asn_query_result(mrtdb, ans, NULL);

				time(&et);
				printf("query as_number time = %f s\n", difftime(et, st));
			}
		}
	}
	return 0;
}

*/
/*



int mrtdb_route_dump(struct mrtdb* mrtdb)
{
	long st, et;
	st = get_system_time_nanosecond();

	FILE* frecord = NULL;
	char file_name[64];
	sprintf(file_name, "%sMRT_record.bin", mrtdb->dir_name);

	frecord = fopen(file_name, "rb");
	if (frecord == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	FILE* fout = NULL;
	sprintf(file_name, "%sbgp_route.out", mrtdb->dir_name);

	fout = fopen(file_name, "w");
	if (fout == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	FILE* fbin = NULL;
	sprintf(file_name, "%sbgp_route.bin", mrtdb->dir_name);

	fbin = fopen(file_name, "wb");
	if (fbin == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	printf("sizeof(struct bgp_route_record) = %lu\n", sizeof(struct bgp_route_record));

	struct rb_node *node;
	int cnt = 0;
	for (node = rb_first(&mrtdb->map_prefix); node; node = rb_next(node))
	{
		mrtdb_key_prefix_t *prefix = rb_entry(node, mrtdb_key_prefix_t, node);

		mrtdb_map_prefix_load_value(mrtdb, prefix);

		mrtdb_attrs_list_t *pos;
		list_for_each_entry(pos, &prefix->attrs_list, list)
		{
			struct bgp_route_record rt;
			memset(&rt, 0, sizeof(rt));
			rt.lmark = 1;
			memcpy(&rt.dst, &prefix->prefix, sizeof(mrtdb_prefix_t));
			memcpy(&rt.nh, &pos->next_hop, sizeof(mrtdb_addr_t));

			for (int i = 0; i < pos->as_path_cnt; ++i)
			{
				rt.metric = pos->as_path_len;
				fwrite(&rt, sizeof(struct bgp_route_record), 1, fbin);
				fwrite(pos->as_path[i], sizeof(uint32_t), pos->as_path_len, fbin);

				char buf[128];
				prefix2string(&rt.dst, buf);
				fprintf(fout, "%s ", buf);
				ip2string(&pos->next_hop, buf);
				fprintf(fout, "%s %u ", buf, rt.metric);
				for (int j = 0; j < pos->as_path_len; ++j)
					fprintf(fout, "%u%c", pos->as_path[i][j], j == pos->as_path_len - 1 ? '\n' : ' ');

				++cnt;
				if (cnt % 1000000 == 0) printf("already deal with %d records\n", cnt);
			}

		}

		mrtdb_map_prefix_unload_value(prefix);

	}

	fclose(frecord);
	fclose(fbin);
	fclose(fout);

	printf("cnt = %d\n", cnt);
	et = get_system_time_nanosecond();
	printf("query_route time = %ld ms\n", (et - st) / 1000000);
	return 0;
}

*/

/*
struct bgp_route_record
{
	mrtdb_prefix_t dst;
	mrtdb_addr_t nh;
	uint32_t metric;
};


static uint64_t Key_ext2u64(struct ipswen_addr *key)
{
	uint64_t x = *(uint64_t *)&key->ext;
	return x;
}


static int addattr_l(struct nlmsghdr *n, int maxlen, int type, void *data, int alen)
{
	int len = RTA_LENGTH(alen);
	struct rtattr *rta;
	if (NLMSG_ALIGN(n->nlmsg_len) + len > maxlen)
		return -1;
	rta = (struct rtattr*)(((char*)n) + NLMSG_ALIGN(n->nlmsg_len));
	rta->rta_type = type;
	rta->rta_len = len;
	memcpy(RTA_DATA(rta), data, alen);
	n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + len;
	//fprintf(stderr, "attr len = %d\n", n->nlmsg_len);
	return 0;
}


static void route_add(int sock_fd, struct bgp_route_record *rt)
{
	uint32_t index = 2;
	struct
	{
		struct nlmsghdr n;
		struct rtmsg r;
		char buf[1024];
	} req;

	memset(&req, 0, sizeof(req));

	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
	req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | NLM_F_CREATE | NLM_F_EXCL;
	req.n.nlmsg_type = RTM_NEWROUTE;

	req.r.rtm_family = AF_INET;
	req.r.rtm_table = RT_TABLE_MAIN;
	req.r.rtm_protocol = RTPROT_STATIC;
	req.r.rtm_scope = RT_SCOPE_UNIVERSE;
	req.r.rtm_type = RTN_UNICAST;
	req.r.rtm_dst_len = rt->dst.plen;  // prefix_len
	req.r.rtm_src_len = 0;
	req.r.rtm_tos = 0;

	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));

	addattr_l(&req.n, sizeof(req), RTA_DST, &rt->dst.addr.data[1], 4);
	addattr_l(&req.n, sizeof(req), RTA_GATEWAY, &rt->nh.data[1], 4);
	addattr_l(&req.n, sizeof(req), RTA_PRIORITY, &rt->metric, 4);
	addattr_l(&req.n, sizeof(req), RTA_OIF, &index, 4);

	uint64_t x = *(uint64_t *)(void *)&rt->dst.addr.data[2];
	uint64_t y = *(uint64_t *)(void *)&rt->nh.data[2];
	addattr_l(&req.n, sizeof(req), RTA_DST_EXT, &x, 8);
	addattr_l(&req.n, sizeof(req), RTA_GATEWAY_EXT, &y, 8);


	send(sock_fd, &req, req.n.nlmsg_len, 0);
	//char buf[128];
	//prefix2string(&rt->dst, buf);
	//printf("send route: %s %u.%u.%u.%u %u\n", buf, nh.bytes[0], nh.bytes[1], nh.bytes[2], nh.bytes[3], rt->metric);
}

int mrtdb_route_add(char *file_name)
{
	int sock_fd;
	if ((sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0) {
		printf("creat socket error\n");
		return -1;
	}

	struct sockaddr_nl nladdr;
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pad = 0;
	nladdr.nl_pid = 0;
	nladdr.nl_groups = 0;
	bind(sock_fd, (struct sockaddr*)&nladdr, sizeof(nladdr));

	long st, et;
	st = get_system_time_nanosecond();

	FILE* f = NULL;
	f = fopen(file_name, "r");
	if (f == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	int route_num = 10;
	int cnt = 0;
	while (1)
	{
		struct bgp_route_record rt;
		char dst_str[64];
		char nh_str[64];
		int ret = fscanf(f, "%s %s %u", dst_str, nh_str, &rt.metric);
		string2prefix(&rt.dst, dst_str);
		string2ip(&rt.nh, nh_str);
		if (ret != 3) break;

		route_add(sock_fd, &rt);

		if (++cnt == route_num) break;
	}

	fclose(f);
	close(sock_fd);

	et = get_system_time_nanosecond();
	printf("route_add_IPv4 time = %ld ms\n", (et - st) / 1000000);

	return 0;
}

*/
mrtdb_value_t* list_entry_value_func(struct list_head *ptr)
{
	return ((mrtdb_value_t *)((char*)(ptr) - (unsigned long)(&((mrtdb_value_t *)0)->list)));
}

mrtdb_list_t* list_entry_key_func(struct list_head *ptr)
{
	return ((mrtdb_list_t*)((char*)(ptr) - (unsigned long)(&((mrtdb_list_t *)0)->list)));
}

/*
static int inet_addr_match(uint32_t *a, uint32_t *b, int bits)
{
	const uint32_t *a1 = a;
	const uint32_t *a2 = b;
	int words = bits >> 0x05;

	bits &= 0x1f;

	if (words)
		if (memcmp(a1, a2, words << 2))
			return -1;

	if (bits) {
		uint32_t w1, w2;
		uint32_t mask;

		w1 = a1[words];
		w2 = a2[words];

		mask = htonl((0xffffffff) << (0x20 - bits));

		if ((w1 ^ w2) & mask)
			return 1;
	}

	return 0;
}

mrtdb_list_t* mrtdb_query_prefix(struct mrtdb* mrtdb, char *q)
{
	mrtdb_addr_t ip;
	memset(&ip, 0, sizeof(ip));
	if (ipswen_aton((void *)&ip.data[1], q))
	{
		ip.bytes[3] = 0xff;
		if (ip.sbytes[15] == 0)
		{
			mrtdb_list_t *ans = NULL;
			mrtdb_list_t *pos;
			list_for_each_entry(pos, &mrtdb->key_list[KEY_PREFIX], list)
			{
				if (pos->prefix.plen > 32) continue;
				if (ans != NULL && pos->prefix.plen < pos->prefix.plen) continue;
				if (!inet_addr_match(&pos->prefix.addr.data[1], &ip.data[1], pos->prefix.plen))
					ans = pos;
			}
			return ans;
		}
		else
		{
			mrtdb_list_t *ans = NULL;
			mrtdb_list_t *pos;
			list_for_each_entry(pos, &mrtdb->key_list[KEY_PREFIX], list)
			{
				if (pos->prefix.plen > 88) continue;
				if (ADDR_IS_IPV6(&pos->prefix.addr)) continue;
				if (ans != NULL && pos->prefix.plen < pos->prefix.plen) continue;
				if (!inet_addr_match(&pos->prefix.addr.data[1], &ip.data[1], pos->prefix.plen))
					ans = pos;
			}
			return ans;
		}

	}
	else if (inet_pton(AF_INET6, q, &ip) == 1)
	{
		mrtdb_list_t *ans = NULL;
		mrtdb_list_t *pos;
		list_for_each_entry(pos, &mrtdb->key_list[KEY_PREFIX], list)
		{
			if (!ADDR_IS_IPV6(&pos->prefix.addr)) continue;
			if (ans != NULL && pos->prefix.plen < pos->prefix.plen) continue;
			if (!inet_addr_match(&pos->prefix.addr.data[1], &ip.data[1], pos->prefix.plen))
				ans = pos;
		}
		return ans;
	}
	return NULL;
}

*/

mrtdb_list_t* mrtdb_query_prefix(struct mrtdb* mrtdb, char *q)
{
	mrtdb_addr_t ip;
	memset(&ip, 0, sizeof(ip));
	if (ipswen_aton((void *)&ip.data[1], q))
	{
		ip.bytes[3] = 0xff;
		if (ip.sbytes[15] == 0)
		{
			mrtdb_list_t *ans = NULL;
			mrtdb_prefix_t prefix;
			memcpy(&prefix.addr, &ip, sizeof(mrtdb_addr_t));
			for (int bits = 32; bits; --bits)
			{
				uint32_t mask = htonl((0xffffffff) << (0x20 - bits));
				prefix.addr.data[1] = ip.data[1] & mask;
				prefix.plen = bits;
				mrtdb_key_t query;
				query.key_type = KEY_PREFIX;
				query.key = get_hash(&prefix, sizeof(mrtdb_prefix_t));
				mrtdb_key_t *res = rbtree_search(&mrtdb->key_rbtree[query.key_type], &query);
				if (res != NULL)
				{
					ans = res->ptr;
					break;
				}
			}
			return ans;
		}
		else
		{
			mrtdb_list_t *ans = NULL;
			return ans;
		}

	}
	else if (inet_pton(AF_INET6, q, &ip) == 1)
	{
		mrtdb_list_t *ans = NULL;
		return ans;
	}
	return NULL;
}

mrtdb_list_t* mrtdb_query_as(struct mrtdb* mrtdb, uint32_t q)
{
	mrtdb_key_t query;
	query.key_type = KEY_AS;
	query.key = get_hash(&q, sizeof(uint32_t));
	mrtdb_key_t *res = rbtree_search(&mrtdb->key_rbtree[query.key_type], &query);
	return res == NULL ? NULL : res->ptr;
}

/*
 *

int mrtdb_route_IPv4(struct mrtdb* mrtdb)
{
	long st, et;
	st = get_system_time_nanosecond();

	FILE* frecord = NULL;
	char file_name[64];
	sprintf(file_name, "%sMRT_record.bin", mrtdb->dir_name);

	frecord = fopen(file_name, "rb");
	if (frecord == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	FILE* fout = NULL;
	sprintf(file_name, "%sbgp_route_IPv4.out", mrtdb->dir_name);

	fout = fopen(file_name, "w");
	if (fout == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	FILE* fbin = NULL;
	sprintf(file_name, "%sbgp_route_IPv4.bin", mrtdb->dir_name);

	fbin = fopen(file_name, "wb");
	if (fbin == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	printf("sizeof(struct bgp_route_record) = %lu\n", sizeof(struct bgp_route_record));

	struct rb_node *node;
	int cnt = 0;
	for (node = rb_first(&mrtdb->map_prefix); node; node = rb_next(node))
	{
		mrtdb_key_prefix_t *prefix = rb_entry(node, mrtdb_key_prefix_t, node);

		mrtdb_map_prefix_load_value(mrtdb, prefix);

		mrtdb_value_nh_list_t *pos;
		list_for_each_entry(pos, &prefix->nh_list, list)
		{
			struct bgp_route_record rt;
			memset(&rt, 0, sizeof(rt));
			rt.lmark = 1;
			memcpy(&rt.dst, &prefix->prefix, sizeof(mrtdb_prefix_t));
			memcpy(&rt.nh, &pos->next_hop, sizeof(mrtdb_addr_t));

			mrtdb_record_t x;
			fseek(frecord, pos->record_offset, SEEK_SET);
			fread(&x, 112, 1, frecord);

			get_as_path(mrtdb->dir_name, &x);
			for (int i = 0; i < x.as_path_cnt; ++i)
			{
				rt.metric = x.as_path[i].len;
				fwrite(&rt, sizeof(struct bgp_route_record), 1, fbin);
				fwrite(x.as_path[i].asn, sizeof(uint32_t), x.as_path[i].len, fbin);

				char buf[128];
				prefix2string(&rt.dst, buf);
				fprintf(fout, "%s ", buf);
				ip2string(&pos->next_hop, buf);
				fprintf(fout, "%s %u ", buf, rt.metric);
				for (int j = 0; j < x.as_path[i].len; ++j)
					fprintf(fout, "%u%c", x.as_path[i].asn[j], j == x.as_path[i].len - 1 ? '\n' : ' ');

				++cnt;
				if (cnt % 1000000 == 0) printf("already deal with %d records\n", cnt);
			}
			free_record(&x);
		}

		mrtdb_map_prefix_unload_value(prefix);

	}

	fclose(frecord);
	fclose(fbin);
	fclose(fout);

	printf("cnt = %d\n", cnt);
	et = get_system_time_nanosecond();
	printf("query_route time = %ld ms\n", (et - st) / 1000000);
	return 0;
}

int mrtdb_route_IPswen(struct mrtdb* mrtdb)
{
	long st, et;
	st = get_system_time_nanosecond();

	FILE* frecord = NULL;
	char file_name[64];
	sprintf(file_name, "%sMRT_record.bin", mrtdb->dir_name);

	frecord = fopen(file_name, "rb");
	if (frecord == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	FILE* fout = NULL;
	sprintf(file_name, "%sbgp_route_IPswen.out", mrtdb->dir_name);

	fout = fopen(file_name, "w");
	if (fout == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	FILE* fbin = NULL;
	sprintf(file_name, "%sbgp_route_IPswen.bin", mrtdb->dir_name);

	fbin = fopen(file_name, "wb");
	if (fbin == NULL)
	{
		printf("open file %s error\n", file_name);
		return -1;
	}

	printf("sizeof(struct bgp_route_record) = %lu\n", sizeof(struct bgp_route_record));

	struct rb_node *node;
	int cnt = 0;
	for (node = rb_first(&mrtdb->map_asn); node; node = rb_next(node))
	{
		mrtdb_key_asn_t *asn = rb_entry(node, mrtdb_key_asn_t, node);
		if (asn->prefix_list_len == 0) continue;

		mrtdb_map_asn_load_value(mrtdb, asn);

		uint32_t num = 0;
		fprintf(fout, "asn: %u\n", asn->asn);
		fwrite(&asn->asn, sizeof(uint32_t), 1, fbin);
		fwrite(&num, sizeof(uint32_t), 1, fbin);

		mrtdb_value_prefix_list_t *pos;
		list_for_each_entry(pos, &asn->prefix_list, list)
		{
			if (ADDR_IS_IPV6(&pos->prefix.addr)) continue;

			mrtdb_key_prefix_t q;
			memcpy(&q.prefix, &pos->prefix, sizeof(mrtdb_prefix_t));
			q.key = get_hash(&q.prefix, sizeof(mrtdb_prefix_t));

			mrtdb_key_prefix_t* prefix = rbtree_search_prefix(&mrtdb->map_prefix, &q);
			if (prefix == NULL)
			{
				printf("cannot find prefix in rbtree\n");
				continue;
			}

			mrtdb_map_prefix_load_value(mrtdb, prefix);

			mrtdb_value_nh_list_t *pos1;
			list_for_each_entry(pos1, &prefix->nh_list, list)
			{
				struct bgp_route_record rt;
				memset(&rt, 0, sizeof(rt));
				rt.lmark = 1;
				memcpy(&rt.dst, &prefix->prefix, sizeof(mrtdb_prefix_t));
				memcpy(&rt.nh, &pos1->next_hop, sizeof(mrtdb_addr_t));

				mrtdb_record_t x;
				fseek(frecord, pos1->record_offset, SEEK_SET);
				fread(&x, 112, 1, frecord);

				rt.asn = x.peer_as;

				mrtdb_key_as_path_t as_path;
				as_path.key = pos1->as_path_hash;
				mrtdb_key_as_path_t *res = rbtree_search_as_path(&mrtdb->map_as_path, &as_path);
				rt.metric = res->as_path_len;

				rt.dst.addr.ips.ext.level = rand() % 8;
				for (int i = 0; i < rt.dst.addr.ips.ext.level; ++i)
				{
					rt.dst.addr.ips.ext.addr[i] = rand() % 256;
				}
				rt.dst.plen = rand() % 8 + 24 + rt.dst.addr.ips.ext.level * 8;

				++num;
				fwrite(&rt, sizeof(struct bgp_route_record), 1, fbin);

				char buf[128];
				prefix2string(&rt.dst, buf);
				fprintf(fout, "%s ", buf);
				ip2string(&pos1->next_hop, buf);
				fprintf(fout, "%s %u %u\n", buf, rt.asn, rt.metric);


				++cnt;
				if (cnt % 1000000 == 0) printf("already deal with %d records\n", cnt);
			}

			mrtdb_map_prefix_unload_value(prefix);
		}

		fseek(fbin, -(4 + num * sizeof(struct bgp_route_record)), SEEK_CUR);
		fwrite(&num, sizeof(uint32_t), 1, fbin);
		fseek(fbin, 0, SEEK_END);

		mrtdb_map_asn_unload_value(asn);
	}

	fclose(frecord);
	fclose(fbin);
	fclose(fout);

	printf("cnt = %d\n", cnt);
	et = get_system_time_nanosecond();
	printf("query_route time = %ld ms\n", (et - st) / 1000000);
	return 0;
}

*/