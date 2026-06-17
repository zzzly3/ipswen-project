#undef HAVE_IPSWEN
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

#define IPSWEN_ADDRSTRLEN 47
#define IPSWEN_MAX_LEVEL 7
#define IPSWEN_MAX_ADDRLEN ((sizeof(in_addr_t) + IPSWEN_MAX_LEVEL) * 8)


#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define inet_aton	__inet_aton_exact

inline int ipswen_aton(struct ipswen_addr *addr, const char *text)
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

	for (i = 0; i < j; i++)
	{
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

inline int ipswen_ntoa(const struct ipswen_addr *addr, char buf[], int buflen)
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
		|| addr->ext.level < -1 * IPSWEN_MAX_LEVEL)
	{
		///__set_errno(ENOSPC);
		return 0;
	}

	if (addr->ext.level != 0)
	{
		if (addr->ext.level < 0)
			extlevel = -1 * addr->ext.level;
		else
			extlevel = addr->ext.level;

		p = &tmp[i];

		for (i = 0; i < extlevel; i++)
		{
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

inline void ipswen_bzero(struct ipswen_addr *addr)
{
	memset(addr, 0, sizeof(struct ipswen_addr));
}

inline int ipswen_eq(const struct ipswen_addr *a, const struct ipswen_addr *b)
{
	return memcmp(a, b, sizeof(struct ipswen_addr)) == 0;
}

inline int ipswen_valid(struct ipswen_addr *addr, const int level, const int prefixlen)
{
	if (!addr ||
	    !((level >= 0 && level <= IPSWEN_MAX_LEVEL) ||
		(level <= -1 && level >= -1 * IPSWEN_MAX_LEVEL)) ||
	    prefixlen < 0 || prefixlen > IPSWEN_MAX_ADDRLEN)
		return 0;
	else
		return 1;
}

inline int ipswen_mask(struct ipswen_addr *addr, const int level, const int prefixlen)
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

	for (i = 0; i < (prefixlen - 32) / 8; i++)
	{
		addr->ext.addr[i] = (uint8_t) - 1;
	}

	j = (prefixlen - 32) % 8;
	if (j)
		addr->ext.addr[i] = ~((uint8_t)((1 << (8 - j)) - 1));

	return 1;
}

inline int ipswen_masking(struct ipswen_addr *addr, struct ipswen_addr *mask)
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

	for (i = 0; i < j; i++)
	{
		if (mask->ext.addr[i] == 255)
			continue;

		addr->ext.addr[i] &= mask->ext.addr[i];
		break;
	}

	return 1;
}

inline int ipswen_prefix(struct ipswen_addr *addr, int level, int prefixlen)
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
inline __always_inline unsigned long ffz(unsigned long word)
{
	asm("rep; bsf %1,%0"
		: "=r" (word)
		: "r" (~word));
	return word;
}

//int __ipswen_masklen(const struct ipswen_addr *mask)
inline int ipswen_masklen(const struct ipswen_addr *mask)
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

	for (i = k = 0; i < j; i++)
	{
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

