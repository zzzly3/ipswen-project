#include "mrtdb_format.h"

#if USER

#include <stdint.h>

#ifdef HAVE_IPSWEN

int ipswen_aton(struct ipswen_addr *addr, const char *text)
{
	char tmp[INET_ADDRSTRLEN];	/* INET_ADDRSTRLEN 16: strlen + 1*/
	const char *p;
	int textlen = (int) strlen(text);
	int i, j;

	if (!addr || textlen < sizeof("1.1.1.1") - 1)
		return 0;
	memset(addr, 0, sizeof(*addr));

	p = strchr(text, '(');
	if (!p) {
		/* just an dotted decimal IPv4 representation */
		return inet_aton(text, (struct in_addr *)&addr->base);
	}

	if (p - text > INET_ADDRSTRLEN)
		return 0;
	strncpy(tmp, text, p - text);
	tmp[p - text] = '\0';

	if (!inet_aton(tmp, (struct in_addr *)&addr->base))
		return 0;

	p++;
	i = atoi(p);
	if (!((i >= 0 && i <= IPSWEN_MAX_LEVEL) || (i <= -1 && i >= -1 * IPSWEN_MAX_LEVEL)))
		return 0;

	addr->ext.level = i;
	if (!i) {
		/* level 0 */
		return 1;
	}

	while (*p != ')')
		p++;
	if (p >= text + textlen - 1 || *p == '\0') {
		/* ill-formatted */
		return 0;
	}
	p++;

	if (addr->ext.level < 0)
		j = -1 * addr->ext.level;
	else
		j = addr->ext.level;

	for (i = 0; i < j; i++) {
		while (!isdigit(*p))
			p++;
		if (p >= text + textlen - 1)
			break;
		addr->ext.addr[i] = atoi(p);

		while (isdigit(*p))
			p++;
		//if (*p == '.')
		//	p++;
	}

	/* TODO: A more general text representation: "a.b.c.d(level)x:i.y:j.z:k" */

	return 1;
}

int ipswen_ntoa(const struct ipswen_addr *addr, char buf[], int buflen)
{
	static const char fmt[] = "%u.%u.%u.%u(%d)";
	char tmp[IPSWEN_ADDRSTRLEN], /* IPSWEN_ADDRSTRLEN 47: strlen + 1*/
		*p;
	int i, extlevel;

	i = sprintf(tmp,
		fmt,
		addr->bytes[0],
		addr->bytes[1],
		addr->bytes[2],
		addr->bytes[3],
		addr->ext.level);

	if (i >= buflen
		|| addr->ext.level > IPSWEN_MAX_LEVEL
		|| addr->ext.level < -1 * IPSWEN_MAX_LEVEL) {
		///__set_errno(ENOSPC);
		return 0;
	}

	if (addr->ext.level != 0) {
		if (addr->ext.level < 0)
			extlevel = -1 * addr->ext.level;
		else
			extlevel = addr->ext.level;

		p = &tmp[i];

		for (i = 0; i < extlevel; i++) {
			p += sprintf(p, "%u", addr->bytes[4 + i]);

			if (p - tmp + 1 >= buflen)
				break;

			if (i != extlevel - 1) {
				*p = '.';
				p++;
			}
		}
		* ++p = '\0';
	}

	buf[buflen - 1] = '\0';
	strncpy(buf, tmp, buflen);
	return 1;
}

void ipswen_bzero(struct ipswen_addr *addr)
{
	memset(addr, 0, sizeof(struct ipswen_addr));
}

int ipswen_eq(const struct ipswen_addr *a, const struct ipswen_addr *b)
{
	return memcmp(a, b, sizeof(struct ipswen_addr)) == 0;
}

int ipswen_valid(struct ipswen_addr *addr, const int level, const int prefixlen)
{
	if (!addr ||
	    !((level >= 0 && level <= IPSWEN_MAX_LEVEL) ||
		(level <= -1 && level >= -1 * IPSWEN_MAX_LEVEL)) ||
	    prefixlen < 0 || prefixlen > IPSWEN_MAX_ADDRLEN)
		return 0;
	else
		return 1;
}

int ipswen_mask(struct ipswen_addr *addr, const int level, const int prefixlen)
{
	int i, j;

	if (!ipswen_valid(addr, level, prefixlen))
		return 0;

	ipswen_bzero(addr);
	addr->ext.level = level;

	if (level == 0 || prefixlen <= 32) {
		addr->base = htonl(~((1U << (32 - prefixlen)) - 1));
		return 1;
	}

	addr->base = -1;

	for (i = 0; i < (prefixlen - 32) / 8; i++) {
		addr->ext.addr[i] = (uint8_t) - 1;
	}

	j = (prefixlen - 32) % 8;
	if (j)
		addr->ext.addr[i] = ~((uint8_t)((1 << (8 - j)) - 1));

	return 1;
}

int ipswen_masking(struct ipswen_addr *addr, struct ipswen_addr *mask)
{
	int i, j;

	if (addr->ext.level != mask->ext.level)
		return 0;

	if (addr->ext.level == 0) {
		addr->base &= mask->base;
		return 1;
	}

	j = addr->ext.level;
	if (j < 0)
		j *= -1;

	for (i = 0; i < j; i++) {
		if (mask->ext.addr[i] == 255)
			continue;

		addr->ext.addr[i] &= mask->ext.addr[i];
		break;
	}

	return 1;
}

int ipswen_prefix(struct ipswen_addr *addr, int level, int prefixlen)
{
	struct ipswen_addr mask;

	if (!ipswen_mask(&mask, level, prefixlen))
		return 0;

	return ipswen_masking(addr, &mask);
}

/**
 * ffz - find first zero bit in word
 * @word: The word to search
 *
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
static __always_inline unsigned long ffz(unsigned long word)
{
	asm("rep; bsf %1,%0"
		: "=r" (word)
		: "r" (~word));
	return word;
}

//int __ipswen_masklen(const struct ipswen_addr *mask)
int ipswen_masklen(const struct ipswen_addr *mask)
{
	uint32_t hmask = ntohl(mask->base);	// a.b.c.d(0) ==> d.c.b.a(0)
	int i, j, k;

	if (!hmask)
		return 0;

	i = ffz(~hmask);
	if (i <= 32)
		return 32 - i;

	j = mask->ext.level;
	if (j < 0)
		j *= -1;

	for (i = k = 0; i < j; i++) {
		hmask = mask->ext.addr[i];
		if (hmask == 0) {
			break;
		}
		else if (hmask == 255) {
			k += 8;
		}
		else {
			hmask <<= 24;
			k += ffz(hmask);
			break;
		}
	}

	return 32 + k;
}

#endif

long get_system_time_nanosecond()
{
	struct timespec timestamp = {};
	if (0 == clock_gettime(CLOCK_REALTIME, &timestamp))
		return timestamp.tv_sec * 1000000000 + timestamp.tv_nsec;
	else
		return 0;
}

int ip2string(mrtdb_addr_t* addr, char* buf)
{
	if (ADDR_IS_IPV4(addr))
	{
		if (inet_ntop(AF_INET, &addr->data[3], buf, 20) == NULL)
		{
			printf("ipv4 to string error\n");
			return -1;
		}
	}
	else if (ADDR_IS_IPSWEN(addr))
	{
		sprintf(buf, "%u.%u.%u.%u", addr->bytes[4], addr->bytes[5], addr->bytes[6], addr->bytes[7]);

		uint8_t level = addr->sbytes[15] >= 0 ? addr->sbytes[15] : -addr->sbytes[15];
		if (level == 0) return 0;

		sprintf(buf, "%s(%d)", buf, addr->sbytes[15]);
		for (int i = 0; i < level; ++i)
		{
			sprintf(buf, "%s%u", buf, addr->bytes[i + 8]);
			if (i < level - 1) sprintf(buf, "%s.", buf);
		}
	}
	else if (ADDR_IS_IPV6(addr))
	{
		if (inet_ntop(AF_INET6, addr->data, buf, 50) == NULL)
		{
			printf("ipv6 to string error\n");
			return -1;
		}
	}
	else
	{
		printf("unknown addr type\n");
		return -1;
	}

	return 0;
}

int prefix2string(mrtdb_prefix_t* prefix, char* buf)
{
	if (ip2string(&prefix->addr, buf) < 0) return -1;
	sprintf(buf, "%s/%u", buf, prefix->plen);
	return 0;
}

int string2ip(mrtdb_addr_t* addr, char* buf)
{
	memset(addr, 0, sizeof(mrtdb_addr_t));
	if (ipswen_aton((void *)&addr->data[1], buf))
	{
		addr->bytes[3] = 0xff;
		return 0;
	}
	else if (inet_pton(AF_INET6, buf, addr) == 1)
	{
		return 0;
	}
	printf("ip string format error\n");
	return -1;
}

int string2prefix(mrtdb_prefix_t* prefix, char* buf)
{
	memset(prefix, 0, sizeof(mrtdb_prefix_t));
	if (strlen(buf) >= 64)
	{
		printf("prefix string too long\n");
		return -1;
	}
	char tmp[64] = { 0 };
	memcpy(tmp, buf, strlen(buf));
	char *p = strchr(tmp, '/');
	if (p == NULL)
	{
		printf("prefix string error\n");
		return -1;
	}
	prefix->plen = atoi(p + 1);
	if (prefix->plen < 0 || prefix->plen > 128)
	{
		printf("prefix len error\n");
		return -1;
	}
	*p = '\0';
	if (string2ip(&prefix->addr, tmp) < 0) return -1;
	return 0;
}

static uint64_t SIP64(const uint8_t *in,
	const size_t inlen,
	uint64_t seed0,
	uint64_t seed1)
{
#define U8TO64_LE(p) \
    {  (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) | \
        ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) | \
        ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) | \
        ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56)) }
#define U64TO8_LE(p, v) \
    { U32TO8_LE((p), (uint32_t)((v))); \
      U32TO8_LE((p) + 4, (uint32_t)((v) >> 32)); }
#define U32TO8_LE(p, v) \
    { (p)[0] = (uint8_t)((v)); \
      (p)[1] = (uint8_t)((v) >> 8); \
      (p)[2] = (uint8_t)((v) >> 16); \
      (p)[3] = (uint8_t)((v) >> 24); }
#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define SIPROUND \
    { v0 += v1; v1 = ROTL(v1, 13); \
      v1 ^= v0; v0 = ROTL(v0, 32); \
      v2 += v3; v3 = ROTL(v3, 16); \
      v3 ^= v2; \
      v0 += v3; v3 = ROTL(v3, 21); \
      v3 ^= v0; \
      v2 += v1; v1 = ROTL(v1, 17); \
      v1 ^= v2; v2 = ROTL(v2, 32); }
	uint64_t k0 = U8TO64_LE((uint8_t*)&seed0);
	uint64_t k1 = U8TO64_LE((uint8_t*)&seed1);
	uint64_t v3 = UINT64_C(0x7465646279746573) ^ k1;
	uint64_t v2 = UINT64_C(0x6c7967656e657261) ^ k0;
	uint64_t v1 = UINT64_C(0x646f72616e646f6d) ^ k1;
	uint64_t v0 = UINT64_C(0x736f6d6570736575) ^ k0;
	const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
	for (; in != end; in += 8) {
		uint64_t m = U8TO64_LE(in);
		v3 ^= m;
		SIPROUND; SIPROUND;
		v0 ^= m;
	}
	const int left = inlen & 7;
	uint64_t b = ((uint64_t)inlen) << 56;
	switch (left) {
	case 7: b |= ((uint64_t)in[6]) << 48; /* fall through */
	case 6: b |= ((uint64_t)in[5]) << 40; /* fall through */
	case 5: b |= ((uint64_t)in[4]) << 32; /* fall through */
	case 4: b |= ((uint64_t)in[3]) << 24; /* fall through */
	case 3: b |= ((uint64_t)in[2]) << 16; /* fall through */
	case 2: b |= ((uint64_t)in[1]) << 8; /* fall through */
	case 1: b |= ((uint64_t)in[0]); break;
	case 0: break;
	}
	v3 ^= b;
	SIPROUND; SIPROUND;
	v0 ^= b;
	v2 ^= 0xff;
	SIPROUND; SIPROUND; SIPROUND; SIPROUND;
	b = v0 ^ v1 ^ v2 ^ v3;
	uint64_t out = 0;
	U64TO8_LE((uint8_t*)&out, b);
	return out;
}

#endif // USER



#if KERNEL

int ip2string(mrtdb_addr_t* addr, char* buf)
{
	if (ADDR_IS_IPV4(addr))
	{
		sprintf(buf, "%pI4", &addr->ip4);
	}
	else if (ADDR_IS_IPSWEN(addr))
	{
		uint8_t level = addr->ips.ext.level >= 0 ? addr->ips.ext.level : -addr->ips.ext.level;
		int i;

		sprintf(buf, "%u.%u.%u.%u(%d)", addr->ips.bytes[0], addr->ips.bytes[1], addr->ips.bytes[2], addr->ips.bytes[3], addr->ips.ext.level);

		if (level == 0) return 0;
		for (i = 0; i < level; ++i)
		{
			sprintf(buf, "%s%u", buf, addr->ips.ext.addr[i]);
			if (i + 1 < level) sprintf(buf, "%s.", buf);
		}
	}
	else if (ADDR_IS_IPV6(addr))
	{
		sprintf(buf, "%pI6c", &addr->ip6);
	}
	else
	{
		printk("mrtdb_kernel: unknown type\n");
		return -1;
	}

	return 0;
}

int prefix2string(mrtdb_prefix_t* prefix, char* buf)
{
	if (ip2string(&prefix->addr, buf) < 0) return -1;
	sprintf(buf, "%s/%u", buf, prefix->plen);
	return 0;
}

static uint64_t SIP64(const uint8_t* in, const size_t inlen, uint64_t seed0, uint64_t seed1)
{
#define U8TO64_LE(p) \
    {  (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) | \
        ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) | \
        ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) | \
        ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56)) }
#define U64TO8_LE(p, v) \
    { U32TO8_LE((p), (uint32_t)((v))); \
      U32TO8_LE((p) + 4, (uint32_t)((v) >> 32)); }
#define U32TO8_LE(p, v) \
    { (p)[0] = (uint8_t)((v)); \
      (p)[1] = (uint8_t)((v) >> 8); \
      (p)[2] = (uint8_t)((v) >> 16); \
      (p)[3] = (uint8_t)((v) >> 24); }
#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))
#define SIPROUND \
    { v0 += v1; v1 = ROTL(v1, 13); \
      v1 ^= v0; v0 = ROTL(v0, 32); \
      v2 += v3; v3 = ROTL(v3, 16); \
      v3 ^= v2; \
      v0 += v3; v3 = ROTL(v3, 21); \
      v3 ^= v0; \
      v2 += v1; v1 = ROTL(v1, 17); \
      v1 ^= v2; v2 = ROTL(v2, 32); }
	uint64_t k0 = U8TO64_LE((uint8_t*)&seed0);
	uint64_t k1 = U8TO64_LE((uint8_t*)&seed1);
	uint64_t v3 = U64_C(0x7465646279746573) ^ k1;
	uint64_t v2 = U64_C(0x6c7967656e657261) ^ k0;
	uint64_t v1 = U64_C(0x646f72616e646f6d) ^ k1;
	uint64_t v0 = U64_C(0x736f6d6570736575) ^ k0;
	const uint8_t* end = in + inlen - (inlen % sizeof(uint64_t));
	const int left = inlen & 7;
	uint64_t b = ((uint64_t)inlen) << 56;
	uint64_t out = 0;
	for (; in != end; in += 8) {
		uint64_t m = U8TO64_LE(in);
		v3 ^= m;
		SIPROUND; SIPROUND;
		v0 ^= m;
	}

	switch (left) {
	case 7: b |= ((uint64_t)in[6]) << 48;
	case 6: b |= ((uint64_t)in[5]) << 40;
	case 5: b |= ((uint64_t)in[4]) << 32;
	case 4: b |= ((uint64_t)in[3]) << 24;
	case 3: b |= ((uint64_t)in[2]) << 16;
	case 2: b |= ((uint64_t)in[1]) << 8;
	case 1: b |= ((uint64_t)in[0]); break;
	case 0: break;
	}
	v3 ^= b;
	SIPROUND; SIPROUND;
	v0 ^= b;
	v2 ^= 0xff;
	SIPROUND; SIPROUND; SIPROUND; SIPROUND;
	b = v0 ^ v1 ^ v2 ^ v3;

	U64TO8_LE((uint8_t*)&out, b);
	return out;
}
#endif // KERNEL


uint64_t get_hash(const void* data, size_t len)
{
	return SIP64((uint8_t*)data, len, 0, 0);
}

mrtdb_key_t* rbtree_search(struct rb_root* root, mrtdb_key_t* x)
{
	struct rb_node *node = root->rb_node;

	while (node)
	{
		mrtdb_key_t *data = container_of(node, mrtdb_key_t, node);

		if (x->key < data->key)
			node = node->rb_left;
		else if (x->key > data->key)
			node = node->rb_right;
		else
			return data;
	}
	return NULL;
}

int rbtree_insert(struct rb_root* root, mrtdb_key_t* x)
{
	struct rb_node **new = &(root->rb_node),
	*parent = NULL ;

	/* Figure out where to put new node */
	while (*new)
	{
		mrtdb_key_t *this = container_of(*new, mrtdb_key_t, node);

		parent = *new;
		if (x->key < this->key)
			new = &((*new)->rb_left);
		else if (x->key > this->key)
			new = &((*new)->rb_right);
		else
			return -1 ;
	}

	/* Add new node and rebalance tree. */
	rb_link_node(&x->node, parent, new);
	rb_insert_color(&x->node, root);

	return 0 ;
}

/*
void my_delete(struct rb_root* root, uint64_t key)
{
	struct mytype *data = my_search(root, key) ;
	if(!data)
	{
		printk("mrtdb_kernel: Not found %llu.\n", key) ;
		return ;
	}

	rb_erase(&data->node, root) ;
}
*/

int rbtree_count(struct rb_root* root)
{
	struct rb_node *node;
	int cnt = 0;
	for (node = rb_first(root); node; node = rb_next(node)) ++cnt;
	return cnt;
}

#if USER

int rbtree_free(struct rb_root* root)
{
	struct rb_node *node;
	struct rb_node *tmp;
	mrtdb_key_t *data;
	for (node = rb_first(root); node;)
	{
		tmp = rb_next(node);
		data = container_of(node, mrtdb_key_t, node);
		rb_erase(node, root);
		free(data);
		node = tmp;
	}
	return 0;
}

#endif  // USER

#if KERNEL

int rbtree_free_prefix(struct rb_root* root)
{
	struct rb_node *node;
	struct rb_node *tmp;
	mrtdb_key_prefix_t *data;
	for (node = rb_first(root); node;)
	{
		tmp = rb_next(node);
		data = container_of(node, mrtdb_key_prefix_t, node);
		rb_erase(node, root);
		kfree(data);
		node = tmp;
	}
	return 0;
}

int rbtree_free_asn(struct rb_root* root)
{
	struct rb_node *node;
	struct rb_node *tmp;
	mrtdb_key_asn_t *data;
	for (node = rb_first(root); node;)
	{
		tmp = rb_next(node);
		data = container_of(node, mrtdb_key_asn_t, node);
		rb_erase(node, root);
		kfree(data);
		node = tmp;
	}
	return 0;
}

int rbtree_free_nh(struct rb_root* root)
{
	struct rb_node *node;
	struct rb_node *tmp;
	mrtdb_key_nh_t *data;
	for (node = rb_first(root); node;)
	{
		tmp = rb_next(node);
		data = container_of(node, mrtdb_key_nh_t, node);
		rb_erase(node, root);
		kfree(data);
		node = tmp;
	}
	return 0;
}

int rbtree_free_as_path(struct rb_root* root)
{
	struct rb_node *node;
	struct rb_node *tmp;
	mrtdb_key_as_path_t *data;
	for (node = rb_first(root); node;)
	{
		tmp = rb_next(node);
		data = container_of(node, mrtdb_key_as_path_t, node);
		rb_erase(node, root);
		kfree(data);
		node = tmp;
	}
	return 0;
}

#endif  // KERNEL


