/*
 *	BIRD Internet Routing Daemon -- The Internet Protocol
 *
 *	(c) 1998 Martin Mares <mj@ucw.cz>
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#ifndef _BIRD_IP_H_
#define _BIRD_IP_H_

#include "lib/lists.h"
#include "sysdep/unix/endian.h"
#include "lib/string.h"
#include "lib/bitops.h"
#include "lib/unaligned.h"
#include <netinet/in.h>
#include "lib/ipswen.h"

#define IP4_ALL_NODES		ipa_build4(224, 0, 0, 1)
#define IP4_ALL_ROUTERS		ipa_build4(224, 0, 0, 2)
#define IP4_OSPF_ALL_ROUTERS	ipa_build4(224, 0, 0, 5)
#define IP4_OSPF_DES_ROUTERS	ipa_build4(224, 0, 0, 6)
#define IP4_RIP_ROUTERS		ipa_build4(224, 0, 0, 9)

#define IP6_ALL_NODES		ipa_build6(0xFF020000, 0, 0, 1)
#define IP6_ALL_ROUTERS		ipa_build6(0xFF020000, 0, 0, 2)
#define IP6_OSPF_ALL_ROUTERS	ipa_build6(0xFF020000, 0, 0, 5)
#define IP6_OSPF_DES_ROUTERS	ipa_build6(0xFF020000, 0, 0, 6)
#define IP6_RIP_ROUTERS		ipa_build6(0xFF020000, 0, 0, 9)
#define IP6_BABEL_ROUTERS	ipa_build6(0xFF020000, 0, 0, 0x00010006)

#define IP4_NONE		_MI4(0)
#define IP6_NONE		_MI6(0,0,0,0)

#define IP4_MAX_PREFIX_LENGTH	32
#define IP6_MAX_PREFIX_LENGTH	128

#ifdef HAVE_IPSWEN
#define IP4_MAX_TEXT_LENGTH	45	/* "255.255.255.255(7)255.255.255.255.255.255.255" */
#define IP6_MAX_TEXT_LENGTH	39	/* "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff" */
#define IPA_MAX_TEXT_LENGTH	45
#else
#define IP4_MAX_TEXT_LENGTH	15	/* "255.255.255.255" */
#define IP6_MAX_TEXT_LENGTH	39	/* "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff" */
#define IPA_MAX_TEXT_LENGTH	39
#endif


#define IP4_MIN_MTU		576
#define IP6_MIN_MTU		1280

#define IP_PREC_INTERNET_CONTROL 0xc0

#define IP4_HEADER_LENGTH	20
#define IP6_HEADER_LENGTH	40
#define UDP_HEADER_LENGTH	8

#define MPLS_NULL		3


/* IANA Address Family Numbers */
/* https://www.iana.org/assignments/address-family-numbers/address-family-numbers.xhtml */
/* Would use AF_ prefix, but that collides with POSIX address family numbers */
#define AFI_IPV4		1
#define AFI_IPV6		2


#ifdef DEBUGGING

typedef struct ip4_addr {
  u32 addr;
} ip4_addr;

#define _MI4(x) ((struct ip4_addr) { x })
#define _I(x) (x).addr

#else

#ifdef HAVE_IPSWEN

typedef union {
  struct ipswen_addr addr;
  u32 d[3];
} ip4_addr;

#define _MI4(x) _MISWEN(x)
#define _MISWEN_D(a, b, c) ((ip4_addr) { .d = {a, b, c} })

#define _I(x) (x).addr.base
#define _LEVEL(x) (x).addr.ext.level
#define _IS0(x) (x).d[0]
#define _IS1(x) (x).d[1]
#define _IS2(x) (x).d[2]

#define _MISWEN_IMPL_0(x) ((struct ipswen_addr) { .base = (x), .ext = { .level = 0 } })
#define _MISWEN_IMPL_1(x, ...) ((struct ipswen_addr) { .base = (x), .ext = { .level = 1, .addr = { __VA_ARGS__ } } })
#define _MISWEN_IMPL_2(x, ...) ((struct ipswen_addr) { .base = (x), .ext = { .level = 2, .addr = { __VA_ARGS__ } } })
#define _MISWEN_IMPL_3(x, ...) ((struct ipswen_addr) { .base = (x), .ext = { .level = 3, .addr = { __VA_ARGS__ } } })
#define _MISWEN_IMPL_4(x, ...) ((struct ipswen_addr) { .base = (x), .ext = { .level = 4, .addr = { __VA_ARGS__ } } })
#define _MISWEN_IMPL_5(x, ...) ((struct ipswen_addr) { .base = (x), .ext = { .level = 5, .addr = { __VA_ARGS__ } } })
#define _MISWEN_IMPL_6(x, ...) ((struct ipswen_addr) { .base = (x), .ext = { .level = 6, .addr = { __VA_ARGS__ } } })
#define _MISWEN_IMPL_7(x, ...) ((struct ipswen_addr) { .base = (x), .ext = { .level = 7, .addr = { __VA_ARGS__ } } })
#define _MISWEN_SELECT_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, N, ...) _MISWEN_IMPL_##N
#define _MISWEN(...) ((ip4_addr)(_MISWEN_SELECT_IMPL(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0, 0)(__VA_ARGS__)))

#define IPSWEN_VALID_LEVEL(x) (_LEVEL(x)  <= 7 && _LEVEL(x)  >= -7)
#define IPSWEN_MAX_PREFIX_LENGTH(x) ((unsigned)(_LEVEL(x) > 0 ? 32 + 8 * _LEVEL(x)  : 32 - 8 * _LEVEL(x) ))

#else

typedef u32 ip4_addr;

#define _MI4(x) ((u32) (x))
#define _I(x) (x)

#endif
#endif


typedef struct ip6_addr {
  u32 addr[4];
} ip6_addr;

#define _MI6(a,b,c,d) ((struct ip6_addr) {{ a, b, c, d }})
#define _I0(a) ((a).addr[0])
#define _I1(a) ((a).addr[1])
#define _I2(a) ((a).addr[2])
#define _I3(a) ((a).addr[3])


/* Structure ip_addr may contain both IPv4 and IPv6 addresses */
typedef ip6_addr ip_addr;
#define IPA_NONE IP6_NONE

static inline int ip6_is_v4mapped(ip6_addr a)
{ return _I0(a) == 0 && _I1(a) == 0 && _I2(a) == 0xffff; }

#ifdef HAVE_IPSWEN
static inline int ip6_is_swenmapped(ip6_addr a)
{ return _I0(a) == 0xff; }
#endif

#ifdef HAVE_IPSWEN

#define ipa_from_ip4(x) _MI6(0x000000ff,_IS1(x),_IS2(x),_IS0(x))
#define ipa_from_ip6(x) x
#define ipa_from_u32(x) ipa_from_ip4(ip4_from_u32(x))

#define ipa_to_ip4(x) _MISWEN_D(_I3(x), _I1(x), _I2(x))
#define ipa_to_ip6(x) x
#define ipa_to_u32(x) ip4_to_u32(ipa_to_ip4(x))

#define ipa_is_ipswen(a) (ipa_is_ip4(a) && _LEVEL(ipa_to_ip4(a)) != 0)
#define ipa_is_ip4(a) (ip6_is_v4mapped(a) || ip6_is_swenmapped(a))
#define ipa_is_ip6(a) (! ip6_is_v4mapped(a) && ! ip6_is_swenmapped(a))

#define IPA_NONE4 ipa_from_ip4(IP4_NONE)
#define IPA_NONE6 ipa_from_ip6(IP6_NONE)

#else

#define ipa_from_ip4(x) _MI6(0,0,0xffff,_I(x))
#define ipa_from_ip6(x) x
#define ipa_from_u32(x) ipa_from_ip4(ip4_from_u32(x))

#define ipa_to_ip4(x) _MI4(_I3(x))
#define ipa_to_ip6(x) x
#define ipa_to_u32(x) ip4_to_u32(ipa_to_ip4(x))

#define ipa_is_ip4(a) ip6_is_v4mapped(a)
#define ipa_is_ip6(a) (! ip6_is_v4mapped(a))

#define IPA_NONE4 ipa_from_ip4(IP4_NONE)
#define IPA_NONE6 ipa_from_ip6(IP6_NONE)

#endif

/*
 *	Public constructors
 */

#define ip4_from_u32(x) _MI4(x)
#define ip4_to_u32(x) _I(x)

#define ip4_build(a,b,c,d) _MI4(((a) << 24) | ((b) << 16) | ((c) << 8) | (d))
#define ip6_build(a,b,c,d) _MI6(a,b,c,d)

#define ipa_build4(a,b,c,d) ipa_from_ip4(ip4_build(a,b,c,d))
#define ipa_build6(a,b,c,d) ipa_from_ip6(ip6_build(a,b,c,d))


/*
 *	Basic algebraic functions
 */

#ifdef HAVE_IPSWEN

static inline int ip4_equal(ip4_addr a, ip4_addr b)
{ return _IS0(a) == _IS0(b) && _IS1(a) == _IS1(b) && _IS2(a) == _IS2(b); }

static inline int ip4_zero(ip4_addr a)
{ _LEVEL(a) = 0; return _IS0(a) == 0 && _IS1(a) == 0 && _IS2(a) == 0; }

static inline int ip4_nonzero(ip4_addr a)
{ _LEVEL(a) = ~0; return _IS0(a) != 0 || _IS1(a) != 0 || _IS2(a) != 0; }

/* The level field is undetermined in the following bit operators. */
static inline ip4_addr ip4_and(ip4_addr a, ip4_addr b)
{ return _MISWEN_D(_IS0(a) & _IS0(b), _IS1(a) & _IS1(b), _IS2(a) & _IS2(b)); }

static inline ip4_addr ip4_or(ip4_addr a, ip4_addr b)
{ return _MISWEN_D(_IS0(a) | _IS0(b), _IS1(a) | _IS1(b), _IS2(a) | _IS2(b)); }

static inline ip4_addr ip4_xor(ip4_addr a, ip4_addr b)
{ return _MISWEN_D(_IS0(a) ^ _IS0(b), _IS1(a) ^ _IS1(b), _IS2(a) ^ _IS2(b)); }

static inline ip4_addr ip4_not(ip4_addr a)
{ return _MISWEN_D(~_IS0(a), ~_IS1(a), ~_IS2(a)); }

#else

static inline int ip4_equal(ip4_addr a, ip4_addr b)
{ return _I(a) == _I(b); }

static inline int ip4_zero(ip4_addr a)
{ return _I(a) == 0; }

static inline int ip4_nonzero(ip4_addr a)
{ return _I(a) != 0; }

static inline ip4_addr ip4_and(ip4_addr a, ip4_addr b)
{ return _MI4(_I(a) & _I(b)); }

static inline ip4_addr ip4_or(ip4_addr a, ip4_addr b)
{ return _MI4(_I(a) | _I(b)); }

static inline ip4_addr ip4_xor(ip4_addr a, ip4_addr b)
{ return _MI4(_I(a) ^ _I(b)); }

static inline ip4_addr ip4_not(ip4_addr a)
{ return _MI4(~_I(a)); }

#endif

static inline int ip6_equal(ip6_addr a, ip6_addr b)
{ return _I0(a) == _I0(b) && _I1(a) == _I1(b) && _I2(a) == _I2(b) && _I3(a) == _I3(b); }

static inline int ip6_zero(ip6_addr a)
{ return  !_I0(a) && !_I1(a) && !_I2(a) && !_I3(a); }

static inline int ip6_nonzero(ip6_addr a)
{ return _I0(a) || _I1(a) || _I2(a) || _I3(a); }

static inline ip6_addr ip6_and(ip6_addr a, ip6_addr b)
{ return _MI6(_I0(a) & _I0(b), _I1(a) & _I1(b), _I2(a) & _I2(b), _I3(a) & _I3(b)); }

static inline ip6_addr ip6_or(ip6_addr a, ip6_addr b)
{ return _MI6(_I0(a) | _I0(b), _I1(a) | _I1(b), _I2(a) | _I2(b), _I3(a) | _I3(b)); }

static inline ip6_addr ip6_xor(ip6_addr a, ip6_addr b)
{ return _MI6(_I0(a) ^ _I0(b), _I1(a) ^ _I1(b), _I2(a) ^ _I2(b), _I3(a) ^ _I3(b)); }

static inline ip6_addr ip6_not(ip6_addr a)
{ return _MI6(~_I0(a), ~_I1(a), ~_I2(a), ~_I3(a)); }


#define ipa_equal(x,y) ip6_equal(x,y)
#define ipa_zero(x) ip6_zero(x)
#define ipa_nonzero(x) ip6_nonzero(x)
#define ipa_and(x,y) ip6_and(x,y)
#define ipa_or(x,y) ip6_or(x,y)
#define ipa_xor(x,y) ip6_xor(x,y)
#define ipa_not(x) ip6_not(x)


/*
 * A zero address is either a token for invalid/unused, or the prefix of default
 * routes. These functions should be used in the second case, where both IPv4
 * and IPv6 zero addresses should be checked.
 */
#ifdef HAVE_IPSWEN
static inline int ipa_zero2(ip_addr a)
{ return (ipa_is_ip4(a) && ip4_zero(ipa_to_ip4(a))) || ip6_zero(a); }
#else
static inline int ipa_zero2(ip_addr a)
{ return  !_I0(a) && !_I1(a) && ((_I2(a) == 0) || (_I2(a) == 0xffff)) && !_I3(a); }
#endif

static inline int ipa_nonzero2(ip_addr a)
{ return !ipa_zero2(a); }


/*
 *	Hash and compare functions
 */

static inline u32 ip6_hash(ip6_addr a)
{
  /* Returns a 32-bit hash key, although low-order bits are not mixed */
  u32 x = _I0(a) ^ _I1(a) ^ _I2(a) ^ _I3(a);
  return x ^ (x << 16) ^ (x << 24);
}

#ifdef HAVE_IPSWEN
static inline u32 ip4_hash(ip4_addr a)
{ 
  return ip6_hash(ipa_from_ip4(a));
}
#else
static inline u32 ip4_hash(ip4_addr a)
{
  return u32_hash(_I(a));
}
#endif

int ip6_compare(ip6_addr a, ip6_addr b);

#ifdef HAVE_IPSWEN
static inline int ip4_compare(ip4_addr a, ip4_addr b)
{ 
  return ip6_compare(_MI6(_IS0(a), _IS1(a), _IS2(a), 0), _MI6(_IS0(b), _IS1(b), _IS2(b), 0)); 
}
#else
static inline int ip4_compare(ip4_addr a, ip4_addr b)
{ 
  return (_I(a) > _I(b)) - (_I(a) < _I(b)); 
}
#endif

#define ipa_hash(x) ip6_hash(x)
#define ipa_compare(x,y) ip6_compare(x,y)


/*
 *	IP address classification
 */

/* Address class */
#define IADDR_INVALID		(-1)
#define IADDR_SCOPE_MASK       	0xfff
#define IADDR_HOST		0x1000
#define IADDR_BROADCAST		0x2000
#define IADDR_MULTICAST		0x4000

/* Address scope */
#define SCOPE_HOST		0
#define SCOPE_LINK		1
#define SCOPE_SITE		2
#define SCOPE_ORGANIZATION	3
#define SCOPE_UNIVERSE		4
#define SCOPE_UNDEFINED		5

int ip4_classify(ip4_addr ad);
int ip6_classify(ip6_addr *a);

static inline int ip6_is_link_local(ip6_addr a)
{ return (_I0(a) & 0xffc00000) == 0xfe800000; }

#define ipa_classify(x) ip6_classify(&(x))
#define ipa_is_link_local(x) ip6_is_link_local(x)

static inline int ip4_is_unicast(ip4_addr a)
{ return _I(a) < 0xe0000000; } // FIXME: ?

/* XXXX remove */
static inline int ipa_classify_net(ip_addr a)
{ return ipa_zero2(a) ? (IADDR_HOST | SCOPE_UNIVERSE) : ipa_classify(a); }


/*
 *	Miscellaneous IP prefix manipulation
 */

#ifdef HAVE_IPSWEN

ip4_addr ip4_mkmask(uint n);
uint ip4_masklen(ip4_addr a);

#else

static inline ip4_addr ip4_mkmask(uint n)
{ return _MI4(u32_mkmask(n)); }

static inline uint ip4_masklen(ip4_addr a)
{ return u32_masklen(_I(a)); }

#endif

ip6_addr ip6_mkmask(uint n);
uint ip6_masklen(ip6_addr *a);

/* ipX_pxlen() requires that x != y */
#ifdef HAVE_IPSWEN

/* NOTE: callers MUST check the level by themselves. */
static inline uint ip4_pxlen(ip4_addr a, ip4_addr b)
{
  _LEVEL(a) = ~_LEVEL(b); /* Shouldn't be counted */
  int i = 0;
  i += (a.d[i] == b.d[i]);
  i += (a.d[i] == b.d[i]);
  i += (a.d[i] == b.d[i]);
  return 32 * i + 31 - u32_log2((i ? ntohl(a.d[i]) : a.d[i]) ^ (i ? ntohl(b.d[i]) : b.d[i]));
}

static inline int ip4_prefix_equal(ip4_addr a, ip4_addr b, uint n)
{
  _LEVEL(a) = _LEVEL(b); /* Shouldn't mismatch */
  uint n0 = n / 32;
  uint n1 = n % 32;

  return
    ((n0 <= 0) || (_IS0(a) == _IS0(b))) &&
    ((n0 <= 1) || (_IS1(a) == _IS1(b))) &&
    ((n0 <= 2) || (_IS2(a) == _IS2(b))) &&
    ((n0 <= 3) || (a.d[3] == b.d[3])) &&
    (!n1 || ((a.d[n0] ^ b.d[n0]) < (1u << (32 - n1))));
}

#else

static inline uint ip4_pxlen(ip4_addr a, ip4_addr b)
{ return 31 - u32_log2(_I(a) ^ _I(b)); }

static inline int ip4_prefix_equal(ip4_addr a, ip4_addr b, uint n)
{
  return (_I(a) ^ _I(b)) < ((u64) 1 << (32 - n));
}

#endif

static inline uint ip6_pxlen(ip6_addr a, ip6_addr b)
{
  int i = 0;
  i += (a.addr[i] == b.addr[i]);
  i += (a.addr[i] == b.addr[i]);
  i += (a.addr[i] == b.addr[i]);
  i += (a.addr[i] == b.addr[i]);
  return 32 * i + 31 - u32_log2(a.addr[i] ^ b.addr[i]);
}

static inline int ip6_prefix_equal(ip6_addr a, ip6_addr b, uint n)
{
  uint n0 = n / 32;
  uint n1 = n % 32;

  return
    ((n0 <= 0) || (_I0(a) == _I0(b))) &&
    ((n0 <= 1) || (_I1(a) == _I1(b))) &&
    ((n0 <= 2) || (_I2(a) == _I2(b))) &&
    ((n0 <= 3) || (_I3(a) == _I3(b))) &&
    (!n1 || ((a.addr[n0] ^ b.addr[n0]) < (1u << (32 - n1))));
}

static inline u32 ip6_getbit(ip6_addr a, uint pos)
{ return (a.addr[pos / 32] >> (31 - (pos % 32))) & 0x1; }

static inline u32 ip6_getbits(ip6_addr a, uint pos, uint n)
{ return (a.addr[pos / 32] >> ((32 - n) - (pos % 32))) & ((1u << n) - 1); }

static inline u32 ip6_setbit(ip6_addr *a, uint pos)
{ return a->addr[pos / 32] |= (0x80000000 >> (pos % 32)); }

static inline u32 ip6_clrbit(ip6_addr *a, uint pos)
{ return a->addr[pos / 32] &= ~(0x80000000 >> (pos % 32)); }

static inline ip6_addr ip6_setbits(ip6_addr a, uint pos, uint val)
{ a.addr[pos / 32] |= val << (31 - pos % 32); return a; }

#ifdef HAVE_IPSWEN

static inline u32 ip4_getbit(ip4_addr a, uint pos)
{ return pos < 32 ? (a.addr.base >> (31 - pos)) & 1 : (a.addr.ext.addr[(pos - 32) / 8] >> (7 - (pos % 8))) & 1; }

static inline u32 ip4_getbits(ip4_addr a, uint pos, uint n)
{ return pos < 32 ? (a.addr.base >> ((32 - n) - pos)) & ((1u << n) - 1) : (a.addr.ext.addr[(pos - 32) / 8] >> ((8 - n) - (pos % 8))) & ((1u << n) - 1); }

static inline u32 ip4_setbit(ip4_addr *a, uint pos)
{ return pos < 32 ? (a->addr.base |= (0x80000000 >> pos)) : (a->addr.ext.addr[(pos - 32) / 8] |= (0x80 >> (pos % 8))); }

static inline u32 ip4_clrbit(ip4_addr *a, uint pos)
{ return pos < 32 ? (a->addr.base &= ~(0x80000000 >> pos)) : (a->addr.ext.addr[(pos - 32) / 8] &= ~(0x80 >> (pos % 8))); }

static inline ip4_addr ip4_setbits(ip4_addr a, uint pos, uint val)
{ 
  if (pos < 32)
    a.addr.base |= val << (31 - pos);
  else
    a.addr.ext.addr[(pos - 32) / 8] |= val << (7 - (pos % 8));
  return a;
}

#else

static inline u32 ip4_getbit(ip4_addr a, uint pos)
{ return (_I(a) >> (31 - pos)) & 1; }

static inline u32 ip4_getbits(ip4_addr a, uint pos, uint n)
{ return (_I(a) >> ((32 - n) - pos)) & ((1u << n) - 1); }

static inline u32 ip4_setbit(ip4_addr *a, uint pos)
{ return _I(*a) |= (0x80000000 >> pos); }

static inline u32 ip4_clrbit(ip4_addr *a, uint pos)
{ return _I(*a) &= ~(0x80000000 >> pos); }

static inline ip4_addr ip4_setbits(ip4_addr a, uint pos, uint val)
{ _I(a) |= val << (31 - pos); return a; }

#endif

#ifdef HAVE_IPSWEN

static inline ip4_addr ip4_opposite_m1(ip4_addr a)
{ a.addr.ext.level ^= 1; return a; }

static inline ip4_addr ip4_opposite_m2(ip4_addr a)
{ a.addr.ext.level ^= 1; return a; }

#else

static inline ip4_addr ip4_opposite_m1(ip4_addr a)
{ return _MI4(_I(a) ^ 1); }

static inline ip4_addr ip4_opposite_m2(ip4_addr a)
{ return _MI4(_I(a) ^ 3); }

#endif

static inline ip6_addr ip6_opposite_m1(ip6_addr a)
{ return _MI6(_I0(a), _I1(a), _I2(a), _I3(a) ^ 1); }

static inline ip6_addr ip6_opposite_m2(ip6_addr a)
{ return _MI6(_I0(a), _I1(a), _I2(a), _I3(a) ^ 3); }

ip4_addr ip4_class_mask(ip4_addr ad); // FIXME

#define ipa_opposite_m1(x) ip6_opposite_m1(x)
#define ipa_opposite_m2(x) ip6_opposite_m2(x)


/*
 *	Host/network order conversions
 */

#ifdef HAVE_IPSWEN

static inline ip4_addr ip4_hton(ip4_addr a)
{ return (ip4_addr)((struct ipswen_addr){.base = htonl(a.addr.base), .ext = a.addr.ext}); }

static inline ip4_addr ip4_ntoh(ip4_addr a)
{ return (ip4_addr)((struct ipswen_addr){.base = ntohl(a.addr.base), .ext = a.addr.ext}); }

#else

static inline ip4_addr ip4_hton(ip4_addr a)
{ return _MI4(htonl(_I(a))); }

static inline ip4_addr ip4_ntoh(ip4_addr a)
{ return _MI4(ntohl(_I(a))); }

#endif

static inline ip6_addr ip6_hton(ip6_addr a)
{ return _MI6(htonl(_I0(a)), htonl(_I1(a)), htonl(_I2(a)), htonl(_I3(a))); }

static inline ip6_addr ip6_ntoh(ip6_addr a)
{ return _MI6(ntohl(_I0(a)), ntohl(_I1(a)), ntohl(_I2(a)), ntohl(_I3(a))); }

#define MPLS_MAX_LABEL_STACK 8
typedef struct mpls_label_stack {
  uint len;
  u32 stack[MPLS_MAX_LABEL_STACK];
} mpls_label_stack;

static inline int
mpls_get(const char *buf, int buflen, u32 *stack)
{
  for (int i=0; (i<MPLS_MAX_LABEL_STACK) && (i*4+3 < buflen); i++)
  {
    u32 s = get_u32(buf + i*4);
    stack[i] = s >> 12;
    if (s & 0x100)
      return i+1;
  }
  return -1;
}

static inline int
mpls_put(char *buf, int len, u32 *stack)
{
  for (int i=0; i<len; i++)
    put_u32(buf + i*4, stack[i] << 12 | (i+1 == len ? 0x100 : 0));

  return len*4;
}

/*
 *	Unaligned data access (in network order)
 */

static inline ip4_addr get_ip4(const void *buf)
{
  return _MI4(get_u32(buf));
}

static inline ip6_addr get_ip6(const void *buf)
{
  ip6_addr a;
  memcpy(&a, buf, 16);
  return ip6_ntoh(a);
}

static inline void * put_ip4(void *buf, ip4_addr a)
{
  put_u32(buf, _I(a));
  return buf+4;
}

static inline void * put_ip6(void *buf, ip6_addr a)
{
  a = ip6_hton(a);
  memcpy(buf, &a, 16);
  return buf+16;
}

#ifdef HAVE_IPSWEN

static inline uint get_ipswen(const void *buf, ip4_addr * restrict a, uint maxlen)
{
  if (maxlen < 5)
    return 0;

  ip4_addr addr = get_ip4(buf);
  _LEVEL(addr) = ((char*)buf)[4];
  if (!IPSWEN_VALID_LEVEL(addr))
    return 0;

  uint sz = _LEVEL(addr) > 0 ? _LEVEL(addr) : -_LEVEL(addr);
  if (maxlen < 5 + sz)
    return 0;

  memcpy(addr.addr.ext.addr, buf+5, sz);
  *a = addr;
  return 5 + sz;
}

static inline void * put_ipswen(void *buf, ip4_addr a)
{
  put_ip4(buf, a);
  ((char*)buf)[4] = _LEVEL(a);
  int sz = _LEVEL(a) > 0 ? _LEVEL(a) : -_LEVEL(a);
  memcpy(buf+5, a.addr.ext.addr, sz);
  return buf + 5 + sz;
}

#endif

/*
 *	Binary(host endian)/text form conversions
 */

char *ip4_ntop(ip4_addr a, char *b);
char *ip6_ntop(ip6_addr a, char *b);

#ifndef HAVE_IPSWEN

static inline char * ip4_ntox(ip4_addr a, char *b)
{ return b + bsprintf(b, "%08x", _I(a)); }

static inline char * ip6_ntox(ip6_addr a, char *b)
{ return b + bsprintf(b, "%08x.%08x.%08x.%08x", _I0(a), _I1(a), _I2(a), _I3(a)); }

#endif

int ip4_pton(const char *a, ip4_addr *o);
int ip6_pton(const char *a, ip6_addr *o);


/*
 *	Miscellaneous
 */

char *ip_scope_text(uint);

#endif
