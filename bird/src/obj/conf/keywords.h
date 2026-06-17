#line 9 "conf/confbase.Y"


#define PARSER 1

#include "nest/bird.h"
#include "conf/conf.h"
#include "lib/resource.h"
#include "lib/socket.h"
#include "lib/timer.h"
#include "lib/string.h"
#include "nest/protocol.h"
#include "nest/iface.h"
#include "nest/route.h"
#include "nest/bfd.h"
#include "nest/cli.h"
#include "filter/filter.h"

/* FIXME: Turn on YYERROR_VERBOSE and work around lots of bison bugs? */

#line 9 "conf/flowspec.Y"


#define PARSER 1

#include "nest/bird.h"
#include "lib/flowspec.h"


#line 11 "filter/config.Y"


#include "filter/f-inst.h"
#include "filter/data.h"

#line 10 "nest/config.Y"


#include "nest/rt-dev.h"
#include "nest/password.h"
#include "nest/cmds.h"
#include "lib/lists.h"
#include "lib/mac.h"

#line 8 "proto/bfd/config.Y"


#include "proto/bfd/bfd.h"

#line 13 "proto/babel/config.Y"


#include "proto/babel/babel.h"
#include "nest/iface.h"

#line 9 "proto/bgp/config.Y"


#include "proto/bgp/bgp.h"

#line 10 "proto/mrt/config.Y"


#include "proto/mrt/mrt.h"

#line 9 "proto/ospf/config.Y"


#include "proto/ospf/ospf.h"

#line 9 "proto/perf/config.Y"


#include "filter/filter.h"
#include "proto/perf/perf.h"

#line 9 "proto/pipe/config.Y"


#include "proto/pipe/pipe.h"

#line 10 "proto/radv/config.Y"


#include "proto/radv/radv.h"

#line 12 "proto/rip/config.Y"


#include "proto/rip/rip.h"
#include "nest/iface.h"

#line 9 "proto/rpki/config.Y"


#include "proto/rpki/rpki.h"

#line 9 "proto/static/config.Y"


#include "proto/static/static.h"

#line 9 "sysdep/linux/netlink.Y"


#line 9 "sysdep/unix/config.Y"


#include "sysdep/unix/unix.h"
#include <stdio.h>

#line 9 "sysdep/unix/krt.Y"


#include "sysdep/unix/krt.h"

#line 44 "conf/gen_keywords.m4"

#line 44
static struct keyword keyword_list[] = {
#line 131 "conf/confbase.Y"
{ "define", DEFINE, NULL },
#line 131 "conf/confbase.Y"
{ "on", ON, NULL },
#line 131 "conf/confbase.Y"
{ "off", OFF, NULL },
#line 131 "conf/confbase.Y"
{ "yes", YES, NULL },
#line 131 "conf/confbase.Y"
{ "no", NO, NULL },
#line 131 "conf/confbase.Y"
{ "s", S, NULL },
#line 131 "conf/confbase.Y"
{ "ms", MS, NULL },
#line 131 "conf/confbase.Y"
{ "us", US, NULL },
#line 131 "conf/confbase.Y"
{ "port", PORT, NULL },
#line 131 "conf/confbase.Y"
{ "vpn", VPN, NULL },
#line 131 "conf/confbase.Y"
{ "mpls", MPLS, NULL },
#line 131 "conf/confbase.Y"
{ "from", FROM, NULL },
#line 27 "conf/flowspec.Y"
{ "flow4", FLOW4, NULL },
#line 27 "conf/flowspec.Y"
{ "flow6", FLOW6, NULL },
#line 27 "conf/flowspec.Y"
{ "dst", DST, NULL },
#line 27 "conf/flowspec.Y"
{ "src", SRC, NULL },
#line 27 "conf/flowspec.Y"
{ "proto", PROTO, NULL },
#line 27 "conf/flowspec.Y"
{ "next", NEXT, NULL },
#line 27 "conf/flowspec.Y"
{ "header", HEADER, NULL },
#line 27 "conf/flowspec.Y"
{ "dport", DPORT, NULL },
#line 27 "conf/flowspec.Y"
{ "sport", SPORT, NULL },
#line 27 "conf/flowspec.Y"
{ "icmp", ICMP, NULL },
#line 27 "conf/flowspec.Y"
{ "type", TYPE, NULL },
#line 27 "conf/flowspec.Y"
{ "code", CODE, NULL },
#line 27 "conf/flowspec.Y"
{ "tcp", TCP, NULL },
#line 27 "conf/flowspec.Y"
{ "flags", FLAGS, NULL },
#line 27 "conf/flowspec.Y"
{ "length", LENGTH, NULL },
#line 27 "conf/flowspec.Y"
{ "dscp", DSCP, NULL },
#line 27 "conf/flowspec.Y"
{ "dont_fragment", DONT_FRAGMENT, NULL },
#line 27 "conf/flowspec.Y"
{ "is_fragment", IS_FRAGMENT, NULL },
#line 27 "conf/flowspec.Y"
{ "first_fragment", FIRST_FRAGMENT, NULL },
#line 27 "conf/flowspec.Y"
{ "last_fragment", LAST_FRAGMENT, NULL },
#line 27 "conf/flowspec.Y"
{ "fragment", FRAGMENT, NULL },
#line 27 "conf/flowspec.Y"
{ "label", LABEL, NULL },
#line 27 "conf/flowspec.Y"
{ "offset", OFFSET, NULL },
#line 9 "client/cmds.Y"
{ "quit", QUIT, NULL },
#line 10 "client/cmds.Y"
{ "exit", EXIT, NULL },
#line 11 "client/cmds.Y"
{ "help", HELP, NULL },
#line 301 "filter/config.Y"
{ "function", FUNCTION, NULL },
#line 301 "filter/config.Y"
{ "print", PRINT, NULL },
#line 301 "filter/config.Y"
{ "printn", PRINTN, NULL },
#line 301 "filter/config.Y"
{ "unset", UNSET, NULL },
#line 301 "filter/config.Y"
{ "return", RETURN, NULL },
#line 301 "filter/config.Y"
{ "accept", ACCEPT, NULL },
#line 301 "filter/config.Y"
{ "reject", REJECT, NULL },
#line 301 "filter/config.Y"
{ "error", ERROR, NULL },
#line 301 "filter/config.Y"
{ "int", INT, NULL },
#line 301 "filter/config.Y"
{ "bool", BOOL, NULL },
#line 301 "filter/config.Y"
{ "ip", IP, NULL },
#line 301 "filter/config.Y"
{ "prefix", PREFIX, NULL },
#line 301 "filter/config.Y"
{ "rd", RD, NULL },
#line 301 "filter/config.Y"
{ "pair", PAIR, NULL },
#line 301 "filter/config.Y"
{ "quad", QUAD, NULL },
#line 301 "filter/config.Y"
{ "ec", EC, NULL },
#line 301 "filter/config.Y"
{ "lc", LC, NULL },
#line 301 "filter/config.Y"
{ "set", SET, NULL },
#line 301 "filter/config.Y"
{ "string", STRING, NULL },
#line 301 "filter/config.Y"
{ "bgpmask", BGPMASK, NULL },
#line 301 "filter/config.Y"
{ "bgppath", BGPPATH, NULL },
#line 301 "filter/config.Y"
{ "clist", CLIST, NULL },
#line 301 "filter/config.Y"
{ "eclist", ECLIST, NULL },
#line 301 "filter/config.Y"
{ "lclist", LCLIST, NULL },
#line 301 "filter/config.Y"
{ "if", IF, NULL },
#line 301 "filter/config.Y"
{ "then", THEN, NULL },
#line 301 "filter/config.Y"
{ "else", ELSE, NULL },
#line 301 "filter/config.Y"
{ "case", CASE, NULL },
#line 301 "filter/config.Y"
{ "for", FOR, NULL },
#line 301 "filter/config.Y"
{ "in", IN, NULL },
#line 301 "filter/config.Y"
{ "do", DO, NULL },
#line 301 "filter/config.Y"
{ "true", TRUE, NULL },
#line 301 "filter/config.Y"
{ "false", FALSE, NULL },
#line 301 "filter/config.Y"
{ "rt", RT, NULL },
#line 301 "filter/config.Y"
{ "ro", RO, NULL },
#line 301 "filter/config.Y"
{ "unknown", UNKNOWN, NULL },
#line 301 "filter/config.Y"
{ "generic", GENERIC, NULL },
#line 301 "filter/config.Y"
{ "gw", GW, NULL },
#line 301 "filter/config.Y"
{ "net", NET, NULL },
#line 301 "filter/config.Y"
{ "mask", MASK, NULL },
#line 301 "filter/config.Y"
{ "source", SOURCE, NULL },
#line 301 "filter/config.Y"
{ "scope", SCOPE, NULL },
#line 301 "filter/config.Y"
{ "dest", DEST, NULL },
#line 301 "filter/config.Y"
{ "ifname", IFNAME, NULL },
#line 301 "filter/config.Y"
{ "ifindex", IFINDEX, NULL },
#line 301 "filter/config.Y"
{ "weight", WEIGHT, NULL },
#line 301 "filter/config.Y"
{ "gw_mpls", GW_MPLS, NULL },
#line 301 "filter/config.Y"
{ "onlink", ONLINK, NULL },
#line 301 "filter/config.Y"
{ "preference", PREFERENCE, NULL },
#line 301 "filter/config.Y"
{ "roa_check", ROA_CHECK, NULL },
#line 301 "filter/config.Y"
{ "asn", ASN, NULL },
#line 301 "filter/config.Y"
{ "is_v4", IS_V4, NULL },
#line 301 "filter/config.Y"
{ "is_v6", IS_V6, NULL },
#line 301 "filter/config.Y"
{ "len", LEN, NULL },
#line 301 "filter/config.Y"
{ "maxlen", MAXLEN, NULL },
#line 301 "filter/config.Y"
{ "data", DATA, NULL },
#line 301 "filter/config.Y"
{ "data1", DATA1, NULL },
#line 301 "filter/config.Y"
{ "data2", DATA2, NULL },
#line 301 "filter/config.Y"
{ "defined", DEFINED, NULL },
#line 301 "filter/config.Y"
{ "add", ADD, NULL },
#line 301 "filter/config.Y"
{ "delete", DELETE, NULL },
#line 301 "filter/config.Y"
{ "contains", CONTAINS, NULL },
#line 301 "filter/config.Y"
{ "reset", RESET, NULL },
#line 301 "filter/config.Y"
{ "prepend", PREPEND, NULL },
#line 301 "filter/config.Y"
{ "first", FIRST, NULL },
#line 301 "filter/config.Y"
{ "last", LAST, NULL },
#line 301 "filter/config.Y"
{ "last_nonaggregated", LAST_NONAGGREGATED, NULL },
#line 301 "filter/config.Y"
{ "match", MATCH, NULL },
#line 301 "filter/config.Y"
{ "min", MIN, NULL },
#line 301 "filter/config.Y"
{ "max", MAX, NULL },
#line 301 "filter/config.Y"
{ "empty", EMPTY, NULL },
#line 301 "filter/config.Y"
{ "filter", FILTER, NULL },
#line 301 "filter/config.Y"
{ "where", WHERE, NULL },
#line 301 "filter/config.Y"
{ "eval", EVAL, NULL },
#line 301 "filter/config.Y"
{ "attribute", ATTRIBUTE, NULL },
#line 301 "filter/config.Y"
{ "bt_assert", BT_ASSERT, NULL },
#line 301 "filter/config.Y"
{ "bt_test_suite", BT_TEST_SUITE, NULL },
#line 301 "filter/config.Y"
{ "bt_check_assign", BT_CHECK_ASSIGN, NULL },
#line 301 "filter/config.Y"
{ "bt_test_same", BT_TEST_SAME, NULL },
#line 301 "filter/config.Y"
{ "format", FORMAT, NULL },
#line 115 "nest/config.Y"
{ "router", ROUTER, NULL },
#line 115 "nest/config.Y"
{ "id", ID, NULL },
#line 115 "nest/config.Y"
{ "hostname", HOSTNAME, NULL },
#line 115 "nest/config.Y"
{ "protocol", PROTOCOL, NULL },
#line 115 "nest/config.Y"
{ "template", TEMPLATE, NULL },
#line 115 "nest/config.Y"
{ "disabled", DISABLED, NULL },
#line 115 "nest/config.Y"
{ "debug", DEBUG, NULL },
#line 115 "nest/config.Y"
{ "all", ALL, NULL },
#line 115 "nest/config.Y"
{ "direct", DIRECT, NULL },
#line 116 "nest/config.Y"
{ "interface", INTERFACE, NULL },
#line 116 "nest/config.Y"
{ "import", IMPORT, NULL },
#line 116 "nest/config.Y"
{ "export", EXPORT, NULL },
#line 116 "nest/config.Y"
{ "none", NONE, NULL },
#line 116 "nest/config.Y"
{ "vrf", VRF, NULL },
#line 116 "nest/config.Y"
{ "default", DEFAULT, NULL },
#line 116 "nest/config.Y"
{ "table", TABLE, NULL },
#line 116 "nest/config.Y"
{ "states", STATES, NULL },
#line 116 "nest/config.Y"
{ "routes", ROUTES, NULL },
#line 116 "nest/config.Y"
{ "filters", FILTERS, NULL },
#line 117 "nest/config.Y"
{ "ipv4", IPV4, NULL },
#line 117 "nest/config.Y"
{ "ipv6", IPV6, NULL },
#line 117 "nest/config.Y"
{ "vpn4", VPN4, NULL },
#line 117 "nest/config.Y"
{ "vpn6", VPN6, NULL },
#line 117 "nest/config.Y"
{ "roa4", ROA4, NULL },
#line 117 "nest/config.Y"
{ "roa6", ROA6, NULL },
#line 117 "nest/config.Y"
{ "sadr", SADR, NULL },
#line 118 "nest/config.Y"
{ "receive", RECEIVE, NULL },
#line 118 "nest/config.Y"
{ "limit", LIMIT, NULL },
#line 118 "nest/config.Y"
{ "action", ACTION, NULL },
#line 118 "nest/config.Y"
{ "warn", WARN, NULL },
#line 118 "nest/config.Y"
{ "block", BLOCK, NULL },
#line 118 "nest/config.Y"
{ "restart", RESTART, NULL },
#line 118 "nest/config.Y"
{ "disable", DISABLE, NULL },
#line 118 "nest/config.Y"
{ "keep", KEEP, NULL },
#line 118 "nest/config.Y"
{ "filtered", FILTERED, NULL },
#line 118 "nest/config.Y"
{ "rpki", RPKI, NULL },
#line 119 "nest/config.Y"
{ "password", PASSWORD, NULL },
#line 119 "nest/config.Y"
{ "key", KEY, NULL },
#line 119 "nest/config.Y"
{ "passive", PASSIVE, NULL },
#line 119 "nest/config.Y"
{ "to", TO, NULL },
#line 119 "nest/config.Y"
{ "events", EVENTS, NULL },
#line 119 "nest/config.Y"
{ "packets", PACKETS, NULL },
#line 119 "nest/config.Y"
{ "protocols", PROTOCOLS, NULL },
#line 119 "nest/config.Y"
{ "channels", CHANNELS, NULL },
#line 119 "nest/config.Y"
{ "interfaces", INTERFACES, NULL },
#line 120 "nest/config.Y"
{ "algorithm", ALGORITHM, NULL },
#line 120 "nest/config.Y"
{ "keyed", KEYED, NULL },
#line 120 "nest/config.Y"
{ "hmac", HMAC, NULL },
#line 120 "nest/config.Y"
{ "md5", MD5, NULL },
#line 120 "nest/config.Y"
{ "sha1", SHA1, NULL },
#line 120 "nest/config.Y"
{ "sha256", SHA256, NULL },
#line 120 "nest/config.Y"
{ "sha384", SHA384, NULL },
#line 120 "nest/config.Y"
{ "sha512", SHA512, NULL },
#line 120 "nest/config.Y"
{ "blake2s128", BLAKE2S128, NULL },
#line 120 "nest/config.Y"
{ "blake2s256", BLAKE2S256, NULL },
#line 120 "nest/config.Y"
{ "blake2b256", BLAKE2B256, NULL },
#line 120 "nest/config.Y"
{ "blake2b512", BLAKE2B512, NULL },
#line 121 "nest/config.Y"
{ "primary", PRIMARY, NULL },
#line 121 "nest/config.Y"
{ "stats", STATS, NULL },
#line 121 "nest/config.Y"
{ "count", COUNT, NULL },
#line 121 "nest/config.Y"
{ "by", BY, NULL },
#line 121 "nest/config.Y"
{ "commands", COMMANDS, NULL },
#line 121 "nest/config.Y"
{ "preexport", PREEXPORT, NULL },
#line 121 "nest/config.Y"
{ "noexport", NOEXPORT, NULL },
#line 121 "nest/config.Y"
{ "exported", EXPORTED, NULL },
#line 121 "nest/config.Y"
{ "generate", GENERATE, NULL },
#line 122 "nest/config.Y"
{ "bgp", BGP, NULL },
#line 122 "nest/config.Y"
{ "passwords", PASSWORDS, NULL },
#line 122 "nest/config.Y"
{ "description", DESCRIPTION, NULL },
#line 123 "nest/config.Y"
{ "reload", RELOAD, NULL },
#line 123 "nest/config.Y"
{ "out", OUT, NULL },
#line 123 "nest/config.Y"
{ "mrtdump", MRTDUMP, NULL },
#line 123 "nest/config.Y"
{ "messages", MESSAGES, NULL },
#line 123 "nest/config.Y"
{ "restrict", RESTRICT, NULL },
#line 123 "nest/config.Y"
{ "memory", MEMORY, NULL },
#line 123 "nest/config.Y"
{ "igp_metric", IGP_METRIC, NULL },
#line 123 "nest/config.Y"
{ "class", CLASS, NULL },
#line 124 "nest/config.Y"
{ "timeformat", TIMEFORMAT, NULL },
#line 124 "nest/config.Y"
{ "iso", ISO, NULL },
#line 124 "nest/config.Y"
{ "short", SHORT, NULL },
#line 124 "nest/config.Y"
{ "long", LONG, NULL },
#line 124 "nest/config.Y"
{ "route", ROUTE, NULL },
#line 124 "nest/config.Y"
{ "base", BASE, NULL },
#line 124 "nest/config.Y"
{ "log", LOG, NULL },
#line 125 "nest/config.Y"
{ "graceful", GRACEFUL, NULL },
#line 125 "nest/config.Y"
{ "wait", WAIT, NULL },
#line 125 "nest/config.Y"
{ "flush", FLUSH, NULL },
#line 125 "nest/config.Y"
{ "as", AS, NULL },
#line 126 "nest/config.Y"
{ "idle", IDLE, NULL },
#line 126 "nest/config.Y"
{ "rx", RX, NULL },
#line 126 "nest/config.Y"
{ "tx", TX, NULL },
#line 126 "nest/config.Y"
{ "interval", INTERVAL, NULL },
#line 126 "nest/config.Y"
{ "multiplier", MULTIPLIER, NULL },
#line 127 "nest/config.Y"
{ "check", CHECK, NULL },
#line 127 "nest/config.Y"
{ "link", LINK, NULL },
#line 128 "nest/config.Y"
{ "sorted", SORTED, NULL },
#line 128 "nest/config.Y"
{ "trie", TRIE, NULL },
#line 128 "nest/config.Y"
{ "settle", SETTLE, NULL },
#line 128 "nest/config.Y"
{ "time", TIME, NULL },
#line 128 "nest/config.Y"
{ "gc", GC, NULL },
#line 128 "nest/config.Y"
{ "threshold", THRESHOLD, NULL },
#line 128 "nest/config.Y"
{ "period", PERIOD, NULL },
#line 131 "nest/config.Y"
{ "ipv4_mc", IPV4_MC, NULL },
#line 131 "nest/config.Y"
{ "ipv4_mpls", IPV4_MPLS, NULL },
#line 131 "nest/config.Y"
{ "ipv6_mc", IPV6_MC, NULL },
#line 131 "nest/config.Y"
{ "ipv6_mpls", IPV6_MPLS, NULL },
#line 131 "nest/config.Y"
{ "ipv6_sadr", IPV6_SADR, NULL },
#line 131 "nest/config.Y"
{ "vpn4_mc", VPN4_MC, NULL },
#line 131 "nest/config.Y"
{ "vpn4_mpls", VPN4_MPLS, NULL },
#line 131 "nest/config.Y"
{ "vpn6_mc", VPN6_MC, NULL },
#line 131 "nest/config.Y"
{ "vpn6_mpls", VPN6_MPLS, NULL },
#line 131 "nest/config.Y"
{ "pri", PRI, NULL },
#line 131 "nest/config.Y"
{ "sec", SEC, NULL },
#line 133 "nest/config.Y"
{ "RTS_STATIC", -((T_ENUM_RTS<<16) | RTS_STATIC), NULL },
#line 133 "nest/config.Y"
{ "RTS_INHERIT", -((T_ENUM_RTS<<16) | RTS_INHERIT), NULL },
#line 133 "nest/config.Y"
{ "RTS_DEVICE", -((T_ENUM_RTS<<16) | RTS_DEVICE), NULL },
#line 133 "nest/config.Y"
{ "RTS_STATIC_DEVICE", -((T_ENUM_RTS<<16) | RTS_STATIC_DEVICE), NULL },
#line 133 "nest/config.Y"
{ "RTS_REDIRECT", -((T_ENUM_RTS<<16) | RTS_REDIRECT), NULL },
#line 133 "nest/config.Y"
{ "RTS_RIP", -((T_ENUM_RTS<<16) | RTS_RIP), NULL },
#line 133 "nest/config.Y"
{ "RTS_OSPF", -((T_ENUM_RTS<<16) | RTS_OSPF), NULL },
#line 133 "nest/config.Y"
{ "RTS_OSPF_IA", -((T_ENUM_RTS<<16) | RTS_OSPF_IA), NULL },
#line 133 "nest/config.Y"
{ "RTS_OSPF_EXT1", -((T_ENUM_RTS<<16) | RTS_OSPF_EXT1), NULL },
#line 133 "nest/config.Y"
{ "RTS_OSPF_EXT2", -((T_ENUM_RTS<<16) | RTS_OSPF_EXT2), NULL },
#line 133 "nest/config.Y"
{ "RTS_BGP", -((T_ENUM_RTS<<16) | RTS_BGP), NULL },
#line 133 "nest/config.Y"
{ "RTS_PIPE", -((T_ENUM_RTS<<16) | RTS_PIPE), NULL },
#line 133 "nest/config.Y"
{ "RTS_BABEL", -((T_ENUM_RTS<<16) | RTS_BABEL), NULL },
#line 135 "nest/config.Y"
{ "SCOPE_HOST", -((T_ENUM_SCOPE<<16) | SCOPE_HOST), NULL },
#line 135 "nest/config.Y"
{ "SCOPE_LINK", -((T_ENUM_SCOPE<<16) | SCOPE_LINK), NULL },
#line 135 "nest/config.Y"
{ "SCOPE_SITE", -((T_ENUM_SCOPE<<16) | SCOPE_SITE), NULL },
#line 135 "nest/config.Y"
{ "SCOPE_ORGANIZATION", -((T_ENUM_SCOPE<<16) | SCOPE_ORGANIZATION), NULL },
#line 135 "nest/config.Y"
{ "SCOPE_UNIVERSE", -((T_ENUM_SCOPE<<16) | SCOPE_UNIVERSE), NULL },
#line 135 "nest/config.Y"
{ "SCOPE_UNDEFINED", -((T_ENUM_SCOPE<<16) | SCOPE_UNDEFINED), NULL },
#line 136 "nest/config.Y"
{ "RTD_UNICAST", -((T_ENUM_RTD<<16) | RTD_UNICAST), NULL },
#line 136 "nest/config.Y"
{ "RTD_BLACKHOLE", -((T_ENUM_RTD<<16) | RTD_BLACKHOLE), NULL },
#line 136 "nest/config.Y"
{ "RTD_UNREACHABLE", -((T_ENUM_RTD<<16) | RTD_UNREACHABLE), NULL },
#line 136 "nest/config.Y"
{ "RTD_PROHIBIT", -((T_ENUM_RTD<<16) | RTD_PROHIBIT), NULL },
#line 137 "nest/config.Y"
{ "ROA_UNKNOWN", -((T_ENUM_ROA<<16) | ROA_UNKNOWN), NULL },
#line 137 "nest/config.Y"
{ "ROA_VALID", -((T_ENUM_ROA<<16) | ROA_VALID), NULL },
#line 137 "nest/config.Y"
{ "ROA_INVALID", -((T_ENUM_ROA<<16) | ROA_INVALID), NULL },
#line 138 "nest/config.Y"
{ "AF_IPV4", -((T_ENUM_AF<<16) | AFI_IPV4), NULL },
#line 138 "nest/config.Y"
{ "AF_IPV6", -((T_ENUM_AF<<16) | AFI_IPV6), NULL },
#line 204 "nest/config.Y"
{ "NET_IP4", -((T_ENUM_NETTYPE<<16) | NET_IP4), NULL },
#line 204 "nest/config.Y"
{ "NET_IP6", -((T_ENUM_NETTYPE<<16) | NET_IP6), NULL },
#line 204 "nest/config.Y"
{ "NET_VPN4", -((T_ENUM_NETTYPE<<16) | NET_VPN4), NULL },
#line 204 "nest/config.Y"
{ "NET_VPN6", -((T_ENUM_NETTYPE<<16) | NET_VPN6), NULL },
#line 204 "nest/config.Y"
{ "NET_ROA4", -((T_ENUM_NETTYPE<<16) | NET_ROA4), NULL },
#line 204 "nest/config.Y"
{ "NET_ROA6", -((T_ENUM_NETTYPE<<16) | NET_ROA6), NULL },
#line 204 "nest/config.Y"
{ "NET_FLOW4", -((T_ENUM_NETTYPE<<16) | NET_FLOW4), NULL },
#line 204 "nest/config.Y"
{ "NET_FLOW6", -((T_ENUM_NETTYPE<<16) | NET_FLOW6), NULL },
#line 204 "nest/config.Y"
{ "NET_IP6_SADR", -((T_ENUM_NETTYPE<<16) | NET_IP6_SADR), NULL },
#line 611 "nest/config.Y"
{ "show", SHOW, NULL },
#line 611 "nest/config.Y"
{ "status", STATUS, NULL },
#line 631 "nest/config.Y"
{ "summary", SUMMARY, NULL },
#line 826 "nest/config.Y"
{ "symbols", SYMBOLS, NULL },
#line 843 "nest/config.Y"
{ "dump", DUMP, NULL },
#line 843 "nest/config.Y"
{ "resources", RESOURCES, NULL },
#line 845 "nest/config.Y"
{ "sockets", SOCKETS, NULL },
#line 851 "nest/config.Y"
{ "neighbors", NEIGHBORS, NULL },
#line 853 "nest/config.Y"
{ "attributes", ATTRIBUTES, NULL },
#line 866 "nest/config.Y"
{ "echo", ECHO, NULL },
#line 887 "nest/config.Y"
{ "enable", ENABLE, NULL },
#line 24 "proto/bfd/config.Y"
{ "bfd", BFD, NULL },
#line 24 "proto/bfd/config.Y"
{ "multihop", MULTIHOP, NULL },
#line 24 "proto/bfd/config.Y"
{ "neighbor", NEIGHBOR, NULL },
#line 24 "proto/bfd/config.Y"
{ "dev", DEV, NULL },
#line 24 "proto/bfd/config.Y"
{ "local", LOCAL, NULL },
#line 24 "proto/bfd/config.Y"
{ "authentication", AUTHENTICATION, NULL },
#line 24 "proto/bfd/config.Y"
{ "simple", SIMPLE, NULL },
#line 24 "proto/bfd/config.Y"
{ "meticulous", METICULOUS, NULL },
#line 24 "proto/bfd/config.Y"
{ "strict", STRICT, NULL },
#line 24 "proto/bfd/config.Y"
{ "bind", BIND, NULL },
#line 186 "proto/bfd/config.Y"
{ "sessions", SESSIONS, NULL },
#line 25 "proto/babel/config.Y"
{ "babel", BABEL, NULL },
#line 25 "proto/babel/config.Y"
{ "metric", METRIC, NULL },
#line 25 "proto/babel/config.Y"
{ "rxcost", RXCOST, NULL },
#line 25 "proto/babel/config.Y"
{ "hello", HELLO, NULL },
#line 25 "proto/babel/config.Y"
{ "update", UPDATE, NULL },
#line 25 "proto/babel/config.Y"
{ "wired", WIRED, NULL },
#line 25 "proto/babel/config.Y"
{ "wireless", WIRELESS, NULL },
#line 25 "proto/babel/config.Y"
{ "buffer", BUFFER, NULL },
#line 25 "proto/babel/config.Y"
{ "priority", PRIORITY, NULL },
#line 25 "proto/babel/config.Y"
{ "hop", HOP, NULL },
#line 25 "proto/babel/config.Y"
{ "babel_metric", BABEL_METRIC, NULL },
#line 25 "proto/babel/config.Y"
{ "entries", ENTRIES, NULL },
#line 25 "proto/babel/config.Y"
{ "randomize", RANDOMIZE, NULL },
#line 25 "proto/babel/config.Y"
{ "mac", MAC, NULL },
#line 25 "proto/babel/config.Y"
{ "permissive", PERMISSIVE, NULL },
#line 25 "proto/babel/config.Y"
{ "extended", EXTENDED, NULL },
#line 20 "proto/bgp/config.Y"
{ "hold", HOLD, NULL },
#line 20 "proto/bgp/config.Y"
{ "connect", CONNECT, NULL },
#line 20 "proto/bgp/config.Y"
{ "retry", RETRY, NULL },
#line 20 "proto/bgp/config.Y"
{ "keepalive", KEEPALIVE, NULL },
#line 20 "proto/bgp/config.Y"
{ "startup", STARTUP, NULL },
#line 20 "proto/bgp/config.Y"
{ "via", VIA, NULL },
#line 20 "proto/bgp/config.Y"
{ "self", SELF, NULL },
#line 20 "proto/bgp/config.Y"
{ "path", PATH, NULL },
#line 20 "proto/bgp/config.Y"
{ "start", START, NULL },
#line 20 "proto/bgp/config.Y"
{ "delay", DELAY, NULL },
#line 20 "proto/bgp/config.Y"
{ "forget", FORGET, NULL },
#line 20 "proto/bgp/config.Y"
{ "after", AFTER, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_path", BGP_PATH, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_local_pref", BGP_LOCAL_PREF, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_med", BGP_MED, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_origin", BGP_ORIGIN, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_next_hop", BGP_NEXT_HOP, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_atomic_aggr", BGP_ATOMIC_AGGR, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_aggregator", BGP_AGGREGATOR, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_community", BGP_COMMUNITY, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_ext_community", BGP_EXT_COMMUNITY, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_large_community", BGP_LARGE_COMMUNITY, NULL },
#line 20 "proto/bgp/config.Y"
{ "address", ADDRESS, NULL },
#line 20 "proto/bgp/config.Y"
{ "rr", RR, NULL },
#line 20 "proto/bgp/config.Y"
{ "rs", RS, NULL },
#line 20 "proto/bgp/config.Y"
{ "client", CLIENT, NULL },
#line 20 "proto/bgp/config.Y"
{ "cluster", CLUSTER, NULL },
#line 20 "proto/bgp/config.Y"
{ "as4", AS4, NULL },
#line 20 "proto/bgp/config.Y"
{ "advertise", ADVERTISE, NULL },
#line 20 "proto/bgp/config.Y"
{ "capabilities", CAPABILITIES, NULL },
#line 20 "proto/bgp/config.Y"
{ "prefer", PREFER, NULL },
#line 20 "proto/bgp/config.Y"
{ "older", OLDER, NULL },
#line 20 "proto/bgp/config.Y"
{ "missing", MISSING, NULL },
#line 20 "proto/bgp/config.Y"
{ "lladdr", LLADDR, NULL },
#line 20 "proto/bgp/config.Y"
{ "drop", DROP, NULL },
#line 20 "proto/bgp/config.Y"
{ "ignore", IGNORE, NULL },
#line 20 "proto/bgp/config.Y"
{ "refresh", REFRESH, NULL },
#line 20 "proto/bgp/config.Y"
{ "interpret", INTERPRET, NULL },
#line 20 "proto/bgp/config.Y"
{ "communities", COMMUNITIES, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_originator_id", BGP_ORIGINATOR_ID, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_cluster_list", BGP_CLUSTER_LIST, NULL },
#line 20 "proto/bgp/config.Y"
{ "igp", IGP, NULL },
#line 20 "proto/bgp/config.Y"
{ "gateway", GATEWAY, NULL },
#line 20 "proto/bgp/config.Y"
{ "recursive", RECURSIVE, NULL },
#line 20 "proto/bgp/config.Y"
{ "med", MED, NULL },
#line 20 "proto/bgp/config.Y"
{ "ttl", TTL, NULL },
#line 20 "proto/bgp/config.Y"
{ "security", SECURITY, NULL },
#line 20 "proto/bgp/config.Y"
{ "deterministic", DETERMINISTIC, NULL },
#line 20 "proto/bgp/config.Y"
{ "secondary", SECONDARY, NULL },
#line 20 "proto/bgp/config.Y"
{ "allow", ALLOW, NULL },
#line 20 "proto/bgp/config.Y"
{ "paths", PATHS, NULL },
#line 20 "proto/bgp/config.Y"
{ "aware", AWARE, NULL },
#line 20 "proto/bgp/config.Y"
{ "setkey", SETKEY, NULL },
#line 20 "proto/bgp/config.Y"
{ "confederation", CONFEDERATION, NULL },
#line 20 "proto/bgp/config.Y"
{ "member", MEMBER, NULL },
#line 20 "proto/bgp/config.Y"
{ "multicast", MULTICAST, NULL },
#line 20 "proto/bgp/config.Y"
{ "lived", LIVED, NULL },
#line 20 "proto/bgp/config.Y"
{ "stale", STALE, NULL },
#line 20 "proto/bgp/config.Y"
{ "ibgp", IBGP, NULL },
#line 20 "proto/bgp/config.Y"
{ "ebgp", EBGP, NULL },
#line 20 "proto/bgp/config.Y"
{ "mandatory", MANDATORY, NULL },
#line 20 "proto/bgp/config.Y"
{ "internal", INTERNAL, NULL },
#line 20 "proto/bgp/config.Y"
{ "external", EXTERNAL, NULL },
#line 20 "proto/bgp/config.Y"
{ "sets", SETS, NULL },
#line 20 "proto/bgp/config.Y"
{ "dynamic", DYNAMIC, NULL },
#line 20 "proto/bgp/config.Y"
{ "range", RANGE, NULL },
#line 20 "proto/bgp/config.Y"
{ "name", NAME, NULL },
#line 20 "proto/bgp/config.Y"
{ "digits", DIGITS, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_aigp", BGP_AIGP, NULL },
#line 20 "proto/bgp/config.Y"
{ "aigp", AIGP, NULL },
#line 20 "proto/bgp/config.Y"
{ "originate", ORIGINATE, NULL },
#line 20 "proto/bgp/config.Y"
{ "cost", COST, NULL },
#line 20 "proto/bgp/config.Y"
{ "enforce", ENFORCE, NULL },
#line 20 "proto/bgp/config.Y"
{ "free", FREE, NULL },
#line 20 "proto/bgp/config.Y"
{ "validate", VALIDATE, NULL },
#line 20 "proto/bgp/config.Y"
{ "role", ROLE, NULL },
#line 20 "proto/bgp/config.Y"
{ "roles", ROLES, NULL },
#line 20 "proto/bgp/config.Y"
{ "peer", PEER, NULL },
#line 20 "proto/bgp/config.Y"
{ "provider", PROVIDER, NULL },
#line 20 "proto/bgp/config.Y"
{ "customer", CUSTOMER, NULL },
#line 20 "proto/bgp/config.Y"
{ "rs_server", RS_SERVER, NULL },
#line 20 "proto/bgp/config.Y"
{ "rs_client", RS_CLIENT, NULL },
#line 20 "proto/bgp/config.Y"
{ "require", REQUIRE, NULL },
#line 20 "proto/bgp/config.Y"
{ "bgp_otc", BGP_OTC, NULL },
#line 20 "proto/bgp/config.Y"
{ "global", GLOBAL, NULL },
#line 40 "proto/bgp/config.Y"
{ "cease", CEASE, NULL },
#line 40 "proto/bgp/config.Y"
{ "hit", HIT, NULL },
#line 40 "proto/bgp/config.Y"
{ "administrative", ADMINISTRATIVE, NULL },
#line 40 "proto/bgp/config.Y"
{ "shutdown", SHUTDOWN, NULL },
#line 40 "proto/bgp/config.Y"
{ "configuration", CONFIGURATION, NULL },
#line 40 "proto/bgp/config.Y"
{ "change", CHANGE, NULL },
#line 40 "proto/bgp/config.Y"
{ "deconfigured", DECONFIGURED, NULL },
#line 40 "proto/bgp/config.Y"
{ "connection", CONNECTION, NULL },
#line 40 "proto/bgp/config.Y"
{ "rejected", REJECTED, NULL },
#line 40 "proto/bgp/config.Y"
{ "collision", COLLISION, NULL },
#line 40 "proto/bgp/config.Y"
{ "of", OF, NULL },
#line 370 "proto/bgp/config.Y"
{ "ORIGIN_IGP", -((T_ENUM_BGP_ORIGIN<<16) | ORIGIN_IGP), NULL },
#line 370 "proto/bgp/config.Y"
{ "ORIGIN_EGP", -((T_ENUM_BGP_ORIGIN<<16) | ORIGIN_EGP), NULL },
#line 370 "proto/bgp/config.Y"
{ "ORIGIN_INCOMPLETE", -((T_ENUM_BGP_ORIGIN<<16) | ORIGIN_INCOMPLETE), NULL },
#line 20 "proto/mrt/config.Y"
{ "mrt", MRT, NULL },
#line 20 "proto/mrt/config.Y"
{ "filename", FILENAME, NULL },
#line 20 "proto/mrt/config.Y"
{ "always", ALWAYS, NULL },
#line 193 "proto/ospf/config.Y"
{ "ospf", OSPF, NULL },
#line 193 "proto/ospf/config.Y"
{ "v2", V2, NULL },
#line 193 "proto/ospf/config.Y"
{ "v3", V3, NULL },
#line 193 "proto/ospf/config.Y"
{ "ospf_metric1", OSPF_METRIC1, NULL },
#line 193 "proto/ospf/config.Y"
{ "ospf_metric2", OSPF_METRIC2, NULL },
#line 193 "proto/ospf/config.Y"
{ "ospf_tag", OSPF_TAG, NULL },
#line 193 "proto/ospf/config.Y"
{ "ospf_router_id", OSPF_ROUTER_ID, NULL },
#line 194 "proto/ospf/config.Y"
{ "area", AREA, NULL },
#line 194 "proto/ospf/config.Y"
{ "rfc1583compat", RFC1583COMPAT, NULL },
#line 194 "proto/ospf/config.Y"
{ "stub", STUB, NULL },
#line 194 "proto/ospf/config.Y"
{ "tick", TICK, NULL },
#line 194 "proto/ospf/config.Y"
{ "cost2", COST2, NULL },
#line 194 "proto/ospf/config.Y"
{ "retransmit", RETRANSMIT, NULL },
#line 195 "proto/ospf/config.Y"
{ "transmit", TRANSMIT, NULL },
#line 195 "proto/ospf/config.Y"
{ "dead", DEAD, NULL },
#line 195 "proto/ospf/config.Y"
{ "broadcast", BROADCAST, NULL },
#line 195 "proto/ospf/config.Y"
{ "bcast", BCAST, NULL },
#line 196 "proto/ospf/config.Y"
{ "nonbroadcast", NONBROADCAST, NULL },
#line 196 "proto/ospf/config.Y"
{ "nbma", NBMA, NULL },
#line 196 "proto/ospf/config.Y"
{ "pointopoint", POINTOPOINT, NULL },
#line 196 "proto/ospf/config.Y"
{ "ptp", PTP, NULL },
#line 196 "proto/ospf/config.Y"
{ "pointomultipoint", POINTOMULTIPOINT, NULL },
#line 196 "proto/ospf/config.Y"
{ "ptmp", PTMP, NULL },
#line 197 "proto/ospf/config.Y"
{ "cryptographic", CRYPTOGRAPHIC, NULL },
#line 198 "proto/ospf/config.Y"
{ "eligible", ELIGIBLE, NULL },
#line 198 "proto/ospf/config.Y"
{ "poll", POLL, NULL },
#line 198 "proto/ospf/config.Y"
{ "networks", NETWORKS, NULL },
#line 198 "proto/ospf/config.Y"
{ "hidden", HIDDEN, NULL },
#line 198 "proto/ospf/config.Y"
{ "virtual", VIRTUAL, NULL },
#line 198 "proto/ospf/config.Y"
{ "only", ONLY, NULL },
#line 199 "proto/ospf/config.Y"
{ "large", LARGE, NULL },
#line 199 "proto/ospf/config.Y"
{ "normal", NORMAL, NULL },
#line 199 "proto/ospf/config.Y"
{ "stubnet", STUBNET, NULL },
#line 199 "proto/ospf/config.Y"
{ "tag", TAG, NULL },
#line 200 "proto/ospf/config.Y"
{ "lsadb", LSADB, NULL },
#line 200 "proto/ospf/config.Y"
{ "ecmp", ECMP, NULL },
#line 200 "proto/ospf/config.Y"
{ "nssa", NSSA, NULL },
#line 200 "proto/ospf/config.Y"
{ "translator", TRANSLATOR, NULL },
#line 200 "proto/ospf/config.Y"
{ "stability", STABILITY, NULL },
#line 201 "proto/ospf/config.Y"
{ "lsid", LSID, NULL },
#line 201 "proto/ospf/config.Y"
{ "instance", INSTANCE, NULL },
#line 201 "proto/ospf/config.Y"
{ "real", REAL, NULL },
#line 201 "proto/ospf/config.Y"
{ "netmask", NETMASK, NULL },
#line 202 "proto/ospf/config.Y"
{ "merge", MERGE, NULL },
#line 202 "proto/ospf/config.Y"
{ "lsa", LSA, NULL },
#line 202 "proto/ospf/config.Y"
{ "suppression", SUPPRESSION, NULL },
#line 202 "proto/ospf/config.Y"
{ "rfc5838", RFC5838, NULL },
#line 202 "proto/ospf/config.Y"
{ "pe", PE, NULL },
#line 525 "proto/ospf/config.Y"
{ "topology", TOPOLOGY, NULL },
#line 533 "proto/ospf/config.Y"
{ "state", STATE, NULL },
#line 20 "proto/perf/config.Y"
{ "perf", PERF, NULL },
#line 20 "proto/perf/config.Y"
{ "exp", EXP, NULL },
#line 20 "proto/perf/config.Y"
{ "repeat", REPEAT, NULL },
#line 20 "proto/perf/config.Y"
{ "mode", MODE, NULL },
#line 19 "proto/pipe/config.Y"
{ "pipe", PIPE, NULL },
#line 31 "proto/radv/config.Y"
{ "radv", RADV, NULL },
#line 31 "proto/radv/config.Y"
{ "ra", RA, NULL },
#line 31 "proto/radv/config.Y"
{ "solicited", SOLICITED, NULL },
#line 31 "proto/radv/config.Y"
{ "unicast", UNICAST, NULL },
#line 31 "proto/radv/config.Y"
{ "managed", MANAGED, NULL },
#line 31 "proto/radv/config.Y"
{ "other", OTHER, NULL },
#line 31 "proto/radv/config.Y"
{ "config", CONFIG, NULL },
#line 31 "proto/radv/config.Y"
{ "linger", LINGER, NULL },
#line 31 "proto/radv/config.Y"
{ "mtu", MTU, NULL },
#line 31 "proto/radv/config.Y"
{ "reachable", REACHABLE, NULL },
#line 31 "proto/radv/config.Y"
{ "retrans", RETRANS, NULL },
#line 31 "proto/radv/config.Y"
{ "timer", TIMER, NULL },
#line 31 "proto/radv/config.Y"
{ "current", CURRENT, NULL },
#line 31 "proto/radv/config.Y"
{ "valid", VALID, NULL },
#line 31 "proto/radv/config.Y"
{ "preferred", PREFERRED, NULL },
#line 31 "proto/radv/config.Y"
{ "mult", MULT, NULL },
#line 31 "proto/radv/config.Y"
{ "lifetime", LIFETIME, NULL },
#line 31 "proto/radv/config.Y"
{ "skip", SKIP, NULL },
#line 31 "proto/radv/config.Y"
{ "autonomous", AUTONOMOUS, NULL },
#line 31 "proto/radv/config.Y"
{ "rdnss", RDNSS, NULL },
#line 31 "proto/radv/config.Y"
{ "dnssl", DNSSL, NULL },
#line 31 "proto/radv/config.Y"
{ "ns", NS, NULL },
#line 31 "proto/radv/config.Y"
{ "domain", DOMAIN, NULL },
#line 31 "proto/radv/config.Y"
{ "trigger", TRIGGER, NULL },
#line 31 "proto/radv/config.Y"
{ "sensitive", SENSITIVE, NULL },
#line 31 "proto/radv/config.Y"
{ "low", LOW, NULL },
#line 31 "proto/radv/config.Y"
{ "medium", MEDIUM, NULL },
#line 31 "proto/radv/config.Y"
{ "high", HIGH, NULL },
#line 31 "proto/radv/config.Y"
{ "propagate", PROPAGATE, NULL },
#line 31 "proto/radv/config.Y"
{ "ra_preference", RA_PREFERENCE, NULL },
#line 31 "proto/radv/config.Y"
{ "ra_lifetime", RA_LIFETIME, NULL },
#line 38 "proto/radv/config.Y"
{ "RA_PREF_LOW", -((T_ENUM_RA_PREFERENCE<<16) | RA_PREF_LOW), NULL },
#line 38 "proto/radv/config.Y"
{ "RA_PREF_MEDIUM", -((T_ENUM_RA_PREFERENCE<<16) | RA_PREF_MEDIUM), NULL },
#line 38 "proto/radv/config.Y"
{ "RA_PREF_HIGH", -((T_ENUM_RA_PREFERENCE<<16) | RA_PREF_HIGH), NULL },
#line 35 "proto/rip/config.Y"
{ "rip", RIP, NULL },
#line 35 "proto/rip/config.Y"
{ "ng", NG, NULL },
#line 35 "proto/rip/config.Y"
{ "infinity", INFINITY, NULL },
#line 35 "proto/rip/config.Y"
{ "timeout", TIMEOUT, NULL },
#line 35 "proto/rip/config.Y"
{ "garbage", GARBAGE, NULL },
#line 35 "proto/rip/config.Y"
{ "version", VERSION, NULL },
#line 35 "proto/rip/config.Y"
{ "split", SPLIT, NULL },
#line 35 "proto/rip/config.Y"
{ "horizon", HORIZON, NULL },
#line 35 "proto/rip/config.Y"
{ "poison", POISON, NULL },
#line 35 "proto/rip/config.Y"
{ "reverse", REVERSE, NULL },
#line 35 "proto/rip/config.Y"
{ "zero", ZERO, NULL },
#line 35 "proto/rip/config.Y"
{ "plaintext", PLAINTEXT, NULL },
#line 35 "proto/rip/config.Y"
{ "demand", DEMAND, NULL },
#line 35 "proto/rip/config.Y"
{ "circuit", CIRCUIT, NULL },
#line 35 "proto/rip/config.Y"
{ "rip_metric", RIP_METRIC, NULL },
#line 35 "proto/rip/config.Y"
{ "rip_tag", RIP_TAG, NULL },
#line 34 "proto/rpki/config.Y"
{ "remote", REMOTE, NULL },
#line 34 "proto/rpki/config.Y"
{ "bird", BIRD, NULL },
#line 34 "proto/rpki/config.Y"
{ "private", PRIVATE, NULL },
#line 34 "proto/rpki/config.Y"
{ "public", PUBLIC, NULL },
#line 34 "proto/rpki/config.Y"
{ "ssh", SSH, NULL },
#line 34 "proto/rpki/config.Y"
{ "transport", TRANSPORT, NULL },
#line 34 "proto/rpki/config.Y"
{ "user", USER, NULL },
#line 34 "proto/rpki/config.Y"
{ "expire", EXPIRE, NULL },
#line 48 "proto/static/config.Y"
{ "static", STATIC, NULL },
#line 48 "proto/static/config.Y"
{ "prohibit", PROHIBIT, NULL },
#line 49 "proto/static/config.Y"
{ "blackhole", BLACKHOLE, NULL },
#line 49 "proto/static/config.Y"
{ "unreachable", UNREACHABLE, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "kernel", KERNEL, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "netlink", NETLINK, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_prefsrc", KRT_PREFSRC, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_realm", KRT_REALM, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_scope", KRT_SCOPE, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_mtu", KRT_MTU, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_window", KRT_WINDOW, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_rtt", KRT_RTT, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_rttvar", KRT_RTTVAR, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_sstresh", KRT_SSTRESH, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_cwnd", KRT_CWND, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_advmss", KRT_ADVMSS, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_reordering", KRT_REORDERING, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_hoplimit", KRT_HOPLIMIT, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_initcwnd", KRT_INITCWND, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_rto_min", KRT_RTO_MIN, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_initrwnd", KRT_INITRWND, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_quickack", KRT_QUICKACK, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_lock_mtu", KRT_LOCK_MTU, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_lock_window", KRT_LOCK_WINDOW, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_lock_rtt", KRT_LOCK_RTT, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_lock_rttvar", KRT_LOCK_RTTVAR, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_lock_sstresh", KRT_LOCK_SSTRESH, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_lock_cwnd", KRT_LOCK_CWND, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_lock_advmss", KRT_LOCK_ADVMSS, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_lock_reordering", KRT_LOCK_REORDERING, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_lock_hoplimit", KRT_LOCK_HOPLIMIT, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_lock_rto_min", KRT_LOCK_RTO_MIN, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_feature_ecn", KRT_FEATURE_ECN, NULL },
#line 13 "sysdep/linux/netlink.Y"
{ "krt_feature_allfrag", KRT_FEATURE_ALLFRAG, NULL },
#line 20 "sysdep/unix/config.Y"
{ "syslog", SYSLOG, NULL },
#line 20 "sysdep/unix/config.Y"
{ "trace", TRACE, NULL },
#line 20 "sysdep/unix/config.Y"
{ "info", INFO, NULL },
#line 20 "sysdep/unix/config.Y"
{ "warning", WARNING, NULL },
#line 20 "sysdep/unix/config.Y"
{ "auth", AUTH, NULL },
#line 20 "sysdep/unix/config.Y"
{ "fatal", FATAL, NULL },
#line 20 "sysdep/unix/config.Y"
{ "bug", BUG, NULL },
#line 20 "sysdep/unix/config.Y"
{ "stderr", STDERR, NULL },
#line 20 "sysdep/unix/config.Y"
{ "soft", SOFT, NULL },
#line 21 "sysdep/unix/config.Y"
{ "confirm", CONFIRM, NULL },
#line 21 "sysdep/unix/config.Y"
{ "undo", UNDO, NULL },
#line 21 "sysdep/unix/config.Y"
{ "latency", LATENCY, NULL },
#line 21 "sysdep/unix/config.Y"
{ "watchdog", WATCHDOG, NULL },
#line 118 "sysdep/unix/config.Y"
{ "configure", CONFIGURE, NULL },
#line 140 "sysdep/unix/config.Y"
{ "down", DOWN, NULL },
#line 32 "sysdep/unix/krt.Y"
{ "persist", PERSIST, NULL },
#line 32 "sysdep/unix/krt.Y"
{ "scan", SCAN, NULL },
#line 32 "sysdep/unix/krt.Y"
{ "learn", LEARN, NULL },
#line 32 "sysdep/unix/krt.Y"
{ "device", DEVICE, NULL },
#line 32 "sysdep/unix/krt.Y"
{ "krt_source", KRT_SOURCE, NULL },
#line 32 "sysdep/unix/krt.Y"
{ "krt_metric", KRT_METRIC, NULL },
#line 44 "conf/gen_keywords.m4"
{ NULL, -1, NULL } };
