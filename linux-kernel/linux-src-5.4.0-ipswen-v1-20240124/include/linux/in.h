/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Definitions of the Internet Protocol.
 *
 * Version:	@(#)in.h	1.0.1	04/21/93
 *
 * Authors:	Original taken from the GNU Project <netinet/in.h> file.
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 */
#ifndef _LINUX_IN_H
#define _LINUX_IN_H

#include <linux/errno.h>
#include <uapi/linux/in.h>

static inline int proto_ports_offset(int proto)
{
	switch (proto) {
	case IPPROTO_TCP:
	case IPPROTO_UDP:
	case IPPROTO_DCCP:
	case IPPROTO_ESP: /* SPI */
	case IPPROTO_SCTP:
	case IPPROTO_UDPLITE:
		return 0;
	case IPPROTO_AH: /* SPI */
		return 4;
	default:
		return -EINVAL;
	}
}

static inline bool ipv4_is_loopback(__be32 addr)
{
	return (addr & htonl(0xff000000)) == htonl(0x7f000000);
}

static inline bool ipv4_is_multicast(__be32 addr)
{
	return (addr & htonl(0xf0000000)) == htonl(0xe0000000);
}

static inline bool ipv4_is_local_multicast(__be32 addr)
{
	return (addr & htonl(0xffffff00)) == htonl(0xe0000000);
}

static inline bool ipv4_is_lbcast(__be32 addr)
{
	/* limited broadcast */
	return addr == htonl(INADDR_BROADCAST);
}

static inline bool ipv4_is_all_snoopers(__be32 addr)
{
	return addr == htonl(INADDR_ALLSNOOPERS_GROUP);
}

static inline bool ipv4_is_zeronet(__be32 addr)
{
	return (addr == 0);
}

/* Special-Use IPv4 Addresses (RFC3330) */

static inline bool ipv4_is_private_10(__be32 addr)
{
	return (addr & htonl(0xff000000)) == htonl(0x0a000000);
}

static inline bool ipv4_is_private_172(__be32 addr)
{
	return (addr & htonl(0xfff00000)) == htonl(0xac100000);
}

static inline bool ipv4_is_private_192(__be32 addr)
{
	return (addr & htonl(0xffff0000)) == htonl(0xc0a80000);
}

static inline bool ipv4_is_linklocal_169(__be32 addr)
{
	return (addr & htonl(0xffff0000)) == htonl(0xa9fe0000);
}

static inline bool ipv4_is_anycast_6to4(__be32 addr)
{
	return (addr & htonl(0xffffff00)) == htonl(0xc0586300);
}

static inline bool ipv4_is_test_192(__be32 addr)
{
	return (addr & htonl(0xffffff00)) == htonl(0xc0000200);
}

static inline bool ipv4_is_test_198(__be32 addr)
{
	return (addr & htonl(0xfffe0000)) == htonl(0xc6120000);
}

#if IS_ENABLED(CONFIG_IP_SWEN)

/* IPswen (so far) is only a unicast extension to the IPv4 addressing
 * scheme and does not extend the multicast addressing scheme of IPv4,
 * as per the IFIP Networking 2022 documentation.
 */

static inline bool ipswen_is_loopback(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_loopback(addr->base);
}

static inline bool ipswen_is_mcast(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_multicast(addr->base);
}

static inline bool ipswen_is_local_multicast(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_local_multicast(addr->base);
}

static inline bool ipswen_is_lbcast(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_lbcast(addr->base);
}

static inline bool ipswen_is_all_snoopers(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_all_snoopers(addr->base);
}

static inline bool ipswen_is_zeronet(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_zeronet(addr->base);
}

/* Special-Use IPswen Base Addresses */

static inline bool ipswen_is_private10(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_private_10(addr->base);
}

static inline bool ipswen_is_private172(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_private_172(addr->base);
}

static inline bool ipswen_is_private192(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_private_192(addr->base);
}

static inline bool ipswen_is_linklocal169(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_linklocal_169(addr->base);
}

static inline bool ipswenaddr__isanycast6to4(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_anycast_6to4(addr->base);
}

static inline bool ipswen_is_test192(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_test_192(addr->base);
}

static inline bool ipswen_is_test198(struct ipswen_addr *addr)
{
	return addr != NULL && ipv4_is_test_198(addr->base);
}

static inline int ipswen_is_none(const struct ipswen_addr *addr)
{
	return addr->base == 0 && addr->ext.level == 0;
}

/* Simple inline ipswen_addr{} handling routines */

static inline void ipswen_addr_bzero(struct ipswen_addr *addr)
{
	memset(addr, 0, sizeof(struct ipswen_addr));
}

static inline int ipswen_is_eq(const struct ipswen_addr *a,
			       const struct ipswen_addr *b)
{
	return memcmp(a, b, sizeof(struct ipswen_addr)) == 0;
}

static inline int ipswen_addr_is_valid(struct ipswen_addr *addr,
				       const int level, const int prefixlen)
{
	if (!addr ||
	    !((level >= 0 && level <= __IPSWEN_MAX_LEVEL) ||
	      (level <= -1 && level >= -1 * __IPSWEN_MAX_LEVEL)) ||
	    prefixlen < 0 || prefixlen > __IPSWEN_MAX_ADDRLEN)
		return 0;
	else
		return 1;
}

static inline int ipswen_addr_mask(struct ipswen_addr *addr, const int level,
				   const int prefixlen)
{
	int i, j;

	if (!ipswen_addr_is_valid(addr, level, prefixlen))
		return 0;

	ipswen_addr_bzero(addr);
	addr->ext.level = level;

	if (level == 0 || prefixlen <= 32) {
		addr->base = htonl(~((1U << (32 - prefixlen)) - 1));
		return 1;
	}

	addr->base = -1;

	for (i = 0; i < (prefixlen - 32) / 8; i++) {
		addr->ext.addr[i] = (__u8)-1;
	}

	j = (prefixlen - 32) % 8;
	if (j)
		addr->ext.addr[i] = ~((__u8)((1 << (8 - j)) - 1));

	return 1;
}

static inline int ipswen_addr_masking(struct ipswen_addr *addr,
				      struct ipswen_addr *mask)
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

static inline int ipswen_addr_prefix(struct ipswen_addr *addr, int level,
				     int prefixlen)
{
	struct ipswen_addr mask;

	if (!ipswen_addr_mask(&mask, level, prefixlen))
		return 0;

	return ipswen_addr_masking(addr, &mask);
}

static inline int ipswen_addr_masklen(const struct ipswen_addr *mask)
{
	__u32 hmask = ntohl(mask->base); // a.b.c.d(0) ==> d.c.b.a(0)
	int i, j, k;

	if (!hmask)
		return 0;

	i = ffz(~hmask);

	if (i)
		return 32 - i;
	/*
	if (i <= 32)
		return 32 - i;
	*/

	j = mask->ext.level;
	if (j < 0)
		j *= -1;

	for (i = k = 0; i < j; i++) {
		hmask = mask->ext.addr[i];
		if (hmask == 0) {
			break;
		} else if (hmask == 255) {
			k += 8;
		} else {
			hmask <<= 24;
			k += 32 - ffz(~hmask);
			// k += ffz(hmask);
			break;
		}
	}

	return 32 + k;
}

#endif

#endif /* _LINUX_IN_H */
