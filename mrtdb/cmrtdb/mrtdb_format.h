#ifndef MRTDB_FORMAT_H
#define MRTDB_FORMAT_H

#define USER 1
#define KERNEL 0

#if USER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "list.h"
#include "rbtree.h"


#endif // USER


#if KERNEL

#include <net/route.h>
#include <net/ip6_route.h>
#include <linux/slab.h>
#include <linux/rbtree.h>
#include <linux/list.h>

#endif // KERNEL

#define ADDR_IS_IPSWEN(x) addr_is_ipswen(x)
#define ADDR_IS_IPV4(x) addr_is_ipv4(x)
#define ADDR_IS_IPV6(x) (!addr_is_ipswen(x) && !addr_is_ipv4(x))


#ifndef HAVE_IPSWEN
#define HAVE_IPSWEN 1

/* Maximum length of the extended IP address bytes. */
#define __IPSWEN_MAX_LEVEL 7

/* Fixed-length (8 bytes) storage representation of the varialbe length
 * IPswen extended addresses, only used by local protocol stack code.
 */
struct ipswen_ext {
	union {
		struct {
			uint8_t addr[__IPSWEN_MAX_LEVEL];
			int8_t level; /* address extension level */
		};
		uint8_t bytes[__IPSWEN_MAX_LEVEL + 1];
	};
};

/* Internet address with 1~7 level extensions. */
struct ipswen_addr {
	union {
		struct {
			uint32_t base;
			struct ipswen_ext ext;
		};
		/* layout for longest prefix match */
		uint8_t bytes[0];
		uint32_t ints[0];
	};
};

/* Variable-length (1 ~ 8 bytes) extended address structure, encoded in
 * address fields in protocol messages.
 */
struct ipswen_var {
	union {
		struct {
			int8_t level; /* address extension level */
			uint8_t addr[0];
		};
		uint8_t bytes[1];
	};
};
struct ipswen_vaddr {
	in_addr_t base;
	struct ipswen_var vext;
};

/* Variable length address prefix representation.
 *	e.g., 1.1.1.1(7)1:7:1/80 ==> {231 80 1...(10 bytes)}
 */
struct ipswen_prefix {
	/* Mark byte {1110 sxxx} for extension level xxx = 0~7 and sign s = 0/1 or +/-. */
	uint8_t level_mark;
	/* Bit number n for u8 prefix[n/8] */
	int8_t length;
	/* Byte aligned top n bits of a level xxx address prefix */
	uint8_t prefix[0];
};

/* Convert IPswen host address from numbers-and-dots notation in text
   into binary data and store the result in the address structure.
   Return 1 for a success, and 0 otherwise.  */
int ipswen_aton(struct ipswen_addr *addr, const char *text);

/* Convert Internet number in IPswen address storage form to ASCII text
   representation. Return 1 for a success, and 0 otherwise.  */
int ipswen_ntoa(const struct ipswen_addr *addr, char buf[], int buflen);

/* Clear all bits of an IPswen address storage structure.  */
void ipswen_bzero(struct ipswen_addr *addr);

/* Compare to IPswen addresses. Return 1 if the two are equal.  */
int ipswen_eq(const struct ipswen_addr *a, const struct ipswen_addr *b);

/* Verify the correct format of an IPswen address.  */
int ipswen_valid(struct ipswen_addr *addr, const int level, const int prefixlen);

/* Generate an IPswen address mask of specified extension level and prefix length.  */
int ipswen_mask(struct ipswen_addr *addr, const int level, const int prefixlen);

/* Get the prefix of the IPswen address with an address mask  */
int ipswen_masking(struct ipswen_addr *addr, struct ipswen_addr *mask);

/* Get the prefix of the IPswen address at the specified length  */
int ipswen_prefix(struct ipswen_addr *addr, int level, int prefixlen);

/* Get the length of the IPswen address mask.  */
int ipswen_masklen(const struct ipswen_addr *mask);

#define IPSWEN_ADDRSTRLEN 47
#define IPSWEN_MAX_LEVEL 7
#define IPSWEN_MAX_ADDRLEN ((sizeof(in_addr_t) + IPSWEN_MAX_LEVEL) * 8)


#endif	/* __USE_IPSWEN */

typedef union
{
	uint8_t bytes[16];
	int8_t sbytes[16];
	uint32_t data[4];

} mrtdb_addr_t;

typedef struct
{
	uint32_t plen;
	mrtdb_addr_t addr;
	// here 20 bytes

} mrtdb_prefix_t;

typedef struct
{
	uint32_t len;
	uint32_t *asn;

} mrtdb_as_path_t;

enum mrtdb_key_type_t
{
	KEY_PREFIX,
	KEY_AS,
	KEY_ASPATH,
	KEY_NEXTHOP,
	__KEY_MAX
};

#define KEY_MAX (__KEY_MAX - 1)

enum mrtdb_value_type_t
{
	VALUE_NEXTHOP,
	VALUE_ASPATH,
	VALUE_PREFIX,
	VALUE_UPSTREAMS,
	VALUE_DOWNSTREAMS,
	__VALUE_MAX
};

#define VALUE_MAX (__VALUE_MAX - 1)

typedef struct
{
	union
	{
		mrtdb_prefix_t prefix;
		uint32_t asn;
		mrtdb_as_path_t as_path;
		mrtdb_addr_t next_hop;
	};

	union
	{
		uint8_t tier;
		uint32_t as_path_cnt;
		uint32_t origin_asn_len;
	};

	uint32_t origin_asn[32];
	int8_t key_type;
	uint32_t index;
	long int offset;
	uint32_t value_list_len[VALUE_MAX + 1];
	struct list_head value_list[VALUE_MAX + 1];
	struct list_head list;

} mrtdb_list_t;

typedef struct
{
	int8_t key_type;
	mrtdb_list_t *ptr;
	uint64_t key;
	struct rb_node node;

} mrtdb_key_t;

typedef struct
{
	int8_t value_type;
	uint32_t index;
	mrtdb_list_t *ptr;
	struct list_head list;

} mrtdb_value_t;

struct mrtdb
{
	char dir_name[32];

	int prefix_ipv4_cnt;
	int prefix_ipv6_cnt;
	int prefix_ipswen_cnt;
	int route_ipv4_cnt;
	int route_ipv6_cnt;
	int route_ipswen_cnt;

	uint32_t key_list_len[KEY_MAX + 1];
	struct list_head key_list[KEY_MAX + 1];
	struct rb_root key_rbtree[KEY_MAX + 1];

};


#if USER

long get_system_time_nanosecond();

#endif // USER


#if KERNEL

typedef struct
{
	struct file* f;
	loff_t pos;
	ssize_t ret;
} FileIO;

#endif // KERNEL

static inline int addr_is_ipv4(mrtdb_addr_t *x)
{ return x->data[0] == 0 && x->data[1] == 0 && x->data[2] == htonl(0xffff); }

static inline int addr_is_ipswen(mrtdb_addr_t *x)
{ return x->bytes[0] == 0 && x->bytes[1] == 0 && x->bytes[2] == 0 && x->bytes[3] == 0xff; }

int ip2string(mrtdb_addr_t* addr, char* buf);
int prefix2string(mrtdb_prefix_t* prefix, char* buf);
int string2ip(mrtdb_addr_t* addr, char* buf);
int string2prefix(mrtdb_prefix_t* prefix, char* buf);


uint64_t get_hash(const void* data, size_t len);
mrtdb_key_t* rbtree_search(struct rb_root* root, mrtdb_key_t* x);
int rbtree_insert(struct rb_root* root, mrtdb_key_t* x);

//void my_delete(struct rb_root* root, uint64_t key);
int rbtree_count(struct rb_root* root);
int rbtree_free(struct rb_root* root);

#endif
