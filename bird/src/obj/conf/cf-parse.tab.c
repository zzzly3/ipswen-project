/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         cf_parse
#define yylex           cf_lex
#define yyerror         cf_error
#define yydebug         cf_debug
#define yynerrs         cf_nerrs
#define yylval          cf_lval
#define yychar          cf_char

/* First part of user prologue.  */
#line 50 "conf/gen_parser.m4"

#line 9 "conf/confbase.Y"
/* Headers from conf/confbase.Y */

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

/* Defines from conf/confbase.Y */

static void
check_u16(uint val)
{
  if (val > 0xFFFF)
    cf_error("Value %u out of range (0-65535)", val);
}

#define cf_assert(cond, ...) do { if (!(cond)) cf_error(__VA_ARGS__); } while (0)
static inline void cf_assert_symbol(const struct symbol *sym, uint class) {
  switch (class) {
    case SYM_PROTO: cf_assert(sym->class == SYM_PROTO, "Protocol name required"); break;
    case SYM_TEMPLATE: cf_assert(sym->class == SYM_TEMPLATE, "Protocol template name required"); break;
    case SYM_FUNCTION: cf_assert(sym->class == SYM_FUNCTION, "Function name required"); break;
    case SYM_FILTER: cf_assert(sym->class == SYM_FILTER, "Filter name required"); break;
    case SYM_TABLE: cf_assert(sym->class == SYM_TABLE, "Table name required"); break;
    case SYM_ATTRIBUTE: cf_assert(sym->class == SYM_ATTRIBUTE, "Custom attribute name required"); break;
    case SYM_VARIABLE: cf_assert((sym->class & ~0xff) == SYM_VARIABLE, "Variable name required"); break;
    case SYM_CONSTANT: cf_assert((sym->class & ~0xff) == SYM_CONSTANT, "Constant name required"); break;
    default: bug("This shall not happen");
  }
}

#line 9 "conf/flowspec.Y"
/* Headers from conf/flowspec.Y */

#define PARSER 1

#include "nest/bird.h"
#include "lib/flowspec.h"


/* Defines from conf/flowspec.Y */

struct flow_builder *this_flow;


#line 11 "filter/config.Y"
/* Headers from filter/config.Y */

#include "filter/f-inst.h"
#include "filter/data.h"

/* Defines from filter/config.Y */

static inline u32 pair(u32 a, u32 b) { return (a << 16) | b; }
static inline u32 pair_a(u32 p) { return p >> 16; }
static inline u32 pair_b(u32 p) { return p & 0xFFFF; }

#define f_generate_complex(fi_code, da, arg) \
  f_new_inst(FI_EA_SET, f_new_inst(fi_code, f_new_inst(FI_EA_GET, da), arg), da)

static int
f_new_var(struct sym_scope *s)
{
  /*
   * - A variable is an offset on vstack from vbase.
   * - Vbase is set on filter start / function call.
   * - Scopes contain (non-frame) block scopes inside filter/function scope
   * - Each scope knows number of vars in that scope
   * - Offset is therefore a sum of 'slots' up to filter/function scope
   * - New variables are added on top of vstk, so intermediate values cannot
   *   be there during FI_VAR_INIT. I.e. no 'var' inside 'term'.
   * - Also, each f_line must always have its scope, otherwise a variable may
   *   be defined but not initialized if relevant f_line is not executed.
   */

  int offset = s->slots++;

  while (s->block)
  {
    s = s->next;
    ASSERT(s);
    offset += s->slots;
  }

  if (offset >= 0xff)
    cf_error("Too many variables, at most 255 allowed");

  return offset;
}

/*
 * Sets and their items are during parsing handled as lists, linked
 * through left ptr. The first item in a list also contains a pointer
 * to the last item in a list (right ptr). For convenience, even items
 * are handled as one-item lists. Lists are merged by f_merge_items().
 */
static int
f_valid_set_type(int type)
{
  switch (type)
  {
  case T_INT:
  case T_PAIR:
  case T_QUAD:
  case T_ENUM:
  case T_IP:
  case T_EC:
  case T_LC:
  case T_RD:
    return 1;

  default:
    return 0;
  }
}

static inline struct f_tree *
f_new_item(struct f_val from, struct f_val to)
{
  struct f_tree *t = f_new_tree();
  t->right = t;
  t->from = from;
  t->to = to;
  return t;
}

static inline struct f_tree *
f_merge_items(struct f_tree *a, struct f_tree *b)
{
  if (!a) return b;
  a->right->left = b;
  a->right = b->right;
  b->right = NULL;
  return a;
}

static inline struct f_tree *
f_new_pair_item(int fa, int ta, int fb, int tb)
{
  check_u16(fa);
  check_u16(ta);
  check_u16(fb);
  check_u16(tb);

  if ((ta < fa) || (tb < fb))
    cf_error( "From value cannot be higher that To value in pair sets");

  struct f_tree *t = f_new_tree();
  t->right = t;
  t->from.type = t->to.type = T_PAIR;
  t->from.val.i = pair(fa, fb);
  t->to.val.i = pair(ta, tb);
  return t;
}

static inline struct f_tree *
f_new_pair_set(int fa, int ta, int fb, int tb)
{
  check_u16(fa);
  check_u16(ta);
  check_u16(fb);
  check_u16(tb);

  if ((ta < fa) || (tb < fb))
    cf_error( "From value cannot be higher that To value in pair sets");

  struct f_tree *lst = NULL;
  int i;

  for (i = fa; i <= ta; i++)
    lst = f_merge_items(lst, f_new_pair_item(i, i, fb, tb));

  return lst;
}

#define CC_ALL 0xFFFF
#define EC_ALL 0xFFFFFFFF
#define LC_ALL 0xFFFFFFFF

static struct f_tree *
f_new_ec_item(u32 kind, u32 ipv4_used, u32 key, u32 vf, u32 vt)
{
  u64 fm, to;

  if ((kind != EC_GENERIC) && (ipv4_used || (key >= 0x10000))) {
    check_u16(vf);
    if (vt == EC_ALL)
      vt = 0xFFFF;
    else
      check_u16(vt);
  }

  if (kind == EC_GENERIC) {
    fm = ec_generic(key, vf);
    to = ec_generic(key, vt);
  }
  else if (ipv4_used) {
    fm = ec_ip4(kind, key, vf);
    to = ec_ip4(kind, key, vt);
  }
  else if (key < 0x10000) {
    fm = ec_as2(kind, key, vf);
    to = ec_as2(kind, key, vt);
  }
  else {
    fm = ec_as4(kind, key, vf);
    to = ec_as4(kind, key, vt);
  }

  struct f_tree *t = f_new_tree();
  t->right = t;
  t->from.type = t->to.type = T_EC;
  t->from.val.ec = fm;
  t->to.val.ec = to;
  return t;
}

static struct f_tree *
f_new_lc_item(u32 f1, u32 t1, u32 f2, u32 t2, u32 f3, u32 t3)
{
  struct f_tree *t = f_new_tree();
  t->right = t;
  t->from.type = t->to.type = T_LC;
  t->from.val.lc = (lcomm) {f1, f2, f3};
  t->to.val.lc = (lcomm) {t1, t2, t3};
  return t;
}

static inline struct f_inst *
f_generate_empty(struct f_dynamic_attr dyn)
{
  struct f_val empty;

  switch (dyn.type & EAF_TYPE_MASK) {
    case EAF_TYPE_AS_PATH:
      empty = f_const_empty_path;
      break;
    case EAF_TYPE_INT_SET:
      empty = f_const_empty_clist;
      break;
    case EAF_TYPE_EC_SET:
      empty = f_const_empty_eclist;
      break;
    case EAF_TYPE_LC_SET:
      empty = f_const_empty_lclist;
      break;
    default:
      cf_error("Can't empty that attribute");
  }

  return f_new_inst(FI_EA_SET, f_new_inst(FI_CONSTANT, empty), dyn);
}

/*
 * Remove all new lines and doubled whitespaces
 * and convert all tabulators to spaces
 * and return a copy of string
 */
char *
assert_copy_expr(const char *start, size_t len)
{
  /* XXX: Allocates maybe a little more memory than we really finally need */
  char *str = cfg_alloc(len + 1);

  char *dst = str;
  const char *src = start - 1;
  const char *end = start + len;
  while (++src < end)
  {
    if (*src == '\n')
      continue;

    /* Skip doubled whitespaces */
    if (src != start)
    {
      const char *prev = src - 1;
      if ((*src == ' ' || *src == '\t') && (*prev == ' ' || *prev == '\t'))
	continue;
    }

    if (*src == '\t')
      *dst = ' ';
    else
      *dst = *src;

    dst++;
  }
  *dst = '\0';

  return str;
}

/*
 * assert_done - create f_instruction of bt_assert
 * @expr: expression in bt_assert()
 * @start: pointer to first char of test expression
 * @end: pointer to the last char of test expression
 */
static struct f_inst *
assert_done(struct f_inst *expr, const char *start, const char *end)
{
  return f_new_inst(FI_ASSERT, expr,
    (end >= start) ?
      assert_copy_expr(start, end - start + 1)
    : "???");
}

static struct f_inst *
assert_assign(struct f_lval *lval, struct f_inst *expr, const char *start, const char *end)
{
  struct f_inst *setter, *getter, *checker;
  switch (lval->type) {
    case F_LVAL_VARIABLE:
      setter = f_new_inst(FI_VAR_SET, expr, lval->sym);
      getter = f_new_inst(FI_VAR_GET, lval->sym);
      break;
    case F_LVAL_SA:
      setter = f_new_inst(FI_RTA_SET, expr, lval->sa);
      getter = f_new_inst(FI_RTA_GET, lval->sa);
      break;
    case F_LVAL_EA:
      setter = f_new_inst(FI_EA_SET, expr, lval->da);
      getter = f_new_inst(FI_EA_GET, lval->da);
      break;
    default:
      bug("Unknown lval type");
  }

  checker = f_new_inst(FI_EQ, expr, getter);
  setter->next = checker;

  return assert_done(setter, start, end);
}

#line 10 "nest/config.Y"
/* Headers from nest/config.Y */

#include "nest/rt-dev.h"
#include "nest/password.h"
#include "nest/cmds.h"
#include "lib/lists.h"
#include "lib/mac.h"

/* Defines from nest/config.Y */

static struct rtable_config *this_table;
static struct proto_config *this_proto;
static struct channel_config *this_channel;
static struct iface_patt *this_ipatt;
static struct iface_patt_node *this_ipn;
/* static struct roa_table_config *this_roa_table; */
static list *this_p_list;
static struct password_item *this_p_item;
static int password_id;
static struct bfd_options *this_bfd_opts;

static void
iface_patt_check(void)
{
  struct iface_patt_node *pn;

  WALK_LIST(pn, this_ipatt->ipn_list)
    if (!pn->pattern || pn->prefix.type)
      cf_error("Interface name/mask expected, not IP prefix");
}

static inline void
init_password_list(void)
{
   if (!this_p_list) {
      this_p_list = cfg_allocz(sizeof(list));
      init_list(this_p_list);
      password_id = 1;
   }
}

static inline void
init_password(const void *key, uint length, uint id)
{
   this_p_item = cfg_allocz(sizeof (struct password_item));
   this_p_item->password = key;
   this_p_item->length = length;
   this_p_item->genfrom = 0;
   this_p_item->gento = TIME_INFINITY;
   this_p_item->accfrom = 0;
   this_p_item->accto = TIME_INFINITY;
   this_p_item->id = id;
   this_p_item->alg = ALG_UNDEFINED;
   add_tail(this_p_list, &this_p_item->n);
}

static inline void
reset_passwords(void)
{
  this_p_list = NULL;
}

static inline list *
get_passwords(void)
{
  list *rv = this_p_list;
  this_p_list = NULL;
  return rv;
}

static inline void
init_bfd_opts(struct bfd_options **opts)
{
  cf_check_bfd(1);

  if (! *opts)
    *opts = bfd_new_options();
}

static inline void
open_bfd_opts(struct bfd_options **opts)
{
  init_bfd_opts(opts);
  this_bfd_opts = *opts;
}

static inline void
close_bfd_opts(void)
{
  this_bfd_opts = NULL;
}

static void
proto_postconfig(void)
{
  CALL(this_proto->protocol->postconfig, this_proto);
  this_channel = NULL;
  this_proto = NULL;
}


#define DIRECT_CFG ((struct rt_dev_config *) this_proto)

#line 8 "proto/bfd/config.Y"
/* Headers from proto/bfd/config.Y */

#include "proto/bfd/bfd.h"

/* Defines from proto/bfd/config.Y */

#define BFD_CFG ((struct bfd_config *) this_proto)
#define BFD_IFACE ((struct bfd_iface_config *) this_ipatt)
#define BFD_NEIGHBOR this_bfd_neighbor

static struct bfd_neighbor *this_bfd_neighbor;

extern struct bfd_config *bfd_cf;

#line 13 "proto/babel/config.Y"
/* Headers from proto/babel/config.Y */

#include "proto/babel/babel.h"
#include "nest/iface.h"

/* Defines from proto/babel/config.Y */

#define BABEL_CFG ((struct babel_config *) this_proto)
#define BABEL_IFACE ((struct babel_iface_config *) this_ipatt)

#line 9 "proto/bgp/config.Y"
/* Headers from proto/bgp/config.Y */

#include "proto/bgp/bgp.h"

/* Defines from proto/bgp/config.Y */

#define BGP_CFG ((struct bgp_config *) this_proto)
#define BGP_CC ((struct bgp_channel_config *) this_channel)

#line 10 "proto/mrt/config.Y"
/* Headers from proto/mrt/config.Y */

#include "proto/mrt/mrt.h"

/* Defines from proto/mrt/config.Y */

#define MRT_CFG ((struct mrt_config *) this_proto)

#line 9 "proto/ospf/config.Y"
/* Headers from proto/ospf/config.Y */

#include "proto/ospf/ospf.h"

/* Defines from proto/ospf/config.Y */

#define OSPF_CFG ((struct ospf_config *) this_proto)
#define OSPF_PATT ((struct ospf_iface_patt *) this_ipatt)

static struct ospf_area_config *this_area;
static struct nbma_node *this_nbma;
static list *this_nets;
static struct area_net_config *this_pref;
static struct ospf_stubnet_config *this_stubnet;

static inline int ospf_cfg_is_v2(void) { return OSPF_CFG->ospf2; }
static inline int ospf_cfg_is_v3(void) { return ! OSPF_CFG->ospf2; }

static void
ospf_iface_finish(void)
{
  struct ospf_iface_patt *ip = OSPF_PATT;

  if (ip->deadint == 0)
    ip->deadint = ip->deadc * ip->helloint;

  if (ip->waitint == 0)
    ip->waitint = ip->deadc * ip->helloint;

  ip->passwords = get_passwords();

  if (ospf_cfg_is_v2() && (ip->autype == OSPF_AUTH_CRYPT) && (ip->helloint < 5))
    log(L_WARN "Hello or poll interval less that 5 makes cryptographic authenication prone to replay attacks");

  if ((ip->autype == OSPF_AUTH_NONE) && (ip->passwords != NULL))
    log(L_WARN "Password option without authentication option does not make sense");

  if (ip->passwords)
  {
    struct password_item *pass;
    WALK_LIST(pass, *ip->passwords)
    {
      if (pass->alg && (ip->autype != OSPF_AUTH_CRYPT))
	cf_error("Password algorithm option requires cryptographic authentication");

      /* Set default OSPF crypto algorithms */
      if (!pass->alg && (ip->autype == OSPF_AUTH_CRYPT))
	pass->alg = ospf_cfg_is_v2() ? ALG_MD5 : ALG_HMAC_SHA256;

      if (ospf_cfg_is_v3() && ip->autype && (pass->alg < ALG_HMAC))
	cf_error("Keyed hash algorithms are not allowed, use HMAC algorithms");
    }
  }
}

static void
ospf_area_finish(void)
{
  if ((this_area->areaid == 0) && (this_area->type != OPT_E))
    cf_error("Backbone area cannot be stub/NSSA");

  if (this_area->summary && (this_area->type == OPT_E))
    cf_error("Only stub/NSSA areas can use summary propagation");

  if (this_area->default_nssa && ((this_area->type != OPT_N) || ! this_area->summary))
    cf_error("Only NSSA areas with summary propagation can use NSSA default route");

  if ((this_area->default_cost & LSA_EXT3_EBIT) && ! this_area->default_nssa)
    cf_error("Only NSSA default route can use type 2 metric");
}

static void
ospf_proto_finish(void)
{
  struct ospf_config *cf = OSPF_CFG;
  struct ospf_area_config *ac;
  struct ospf_iface_patt *ic;

  /* Define default channel */
  if (! proto_cf_main_channel(this_proto))
  {
    uint net_type = this_proto->net_type = ospf_cfg_is_v2() ? NET_IP4 : NET_IP6;
    channel_config_new(NULL, net_label[net_type], net_type, this_proto);
  }

  /* Propagate global instance ID to interfaces */
  if (cf->instance_id_set)
  {
    WALK_LIST(ac, cf->area_list)
      WALK_LIST(ic, ac->patt_list)
	if (!ic->instance_id_set)
	{ ic->instance_id = cf->instance_id; ic->instance_id_set = 1; }

    WALK_LIST(ic, cf->vlink_list)
      if (!ic->instance_id_set)
      { ic->instance_id = cf->instance_id; ic->instance_id_set = 1; }
  }

  if (ospf_cfg_is_v3())
  {
    uint ipv4 = (this_proto->net_type == NET_IP4);
    uint base = (ipv4 ? 64 : 0) + (cf->af_mc ? 32 : 0);

    /* RFC 5838 - OSPFv3-AF */
    if (cf->af_ext)
    {
      /* RFC 5838 2.1 - instance IDs based on AFs */
      WALK_LIST(ac, cf->area_list)
	WALK_LIST(ic, ac->patt_list)
	{
	  if (!ic->instance_id_set)
	    ic->instance_id = base;
	  else if (ic->instance_id >= 128)
	    log(L_WARN "Instance ID %d from unassigned/private range", ic->instance_id);
	  else if ((ic->instance_id < base) || (ic->instance_id >= (base + 32)))
	    cf_error("Instance ID %d invalid for given channel type", ic->instance_id);
	}

      /* RFC 5838 2.8 - vlinks limited to IPv6 unicast */
      if ((ipv4 || cf->af_mc) && !EMPTY_LIST(cf->vlink_list))
	cf_error("Vlinks not supported in AFs other than IPv6 unicast");
    }
    else
    {
      if (ipv4 || cf->af_mc)
	cf_error("Different channel type");
    }
  }

  if (EMPTY_LIST(cf->area_list))
    cf_error("No configured areas in OSPF");

  int areano = 0;
  int backbone = 0;
  int nssa = 0;
  WALK_LIST(ac, cf->area_list)
  {
    areano++;
    if (ac->areaid == 0)
      backbone = 1;
    if (ac->type == OPT_N)
      nssa = 1;
  }

  cf->abr = areano > 1;

  /* Route export or NSSA translation (RFC 3101 3.1) */
  cf->asbr = (proto_cf_main_channel(this_proto)->out_filter != FILTER_REJECT) || (nssa && cf->abr);

  if (cf->abr && !backbone)
  {
    struct ospf_area_config *ac = cfg_allocz(sizeof(struct ospf_area_config));
    ac->type = OPT_E; /* Backbone is non-stub */
    add_head(&cf->area_list, NODE ac);
    init_list(&ac->patt_list);
    init_list(&ac->net_list);
    init_list(&ac->enet_list);
    init_list(&ac->stubnet_list);
  }

  if (!cf->abr && !EMPTY_LIST(cf->vlink_list))
    cf_error("Vlinks cannot be used on single area router");

  if (cf->asbr && (areano == 1) && (this_area->type == 0))
    cf_error("ASBR must be in non-stub area");
}

static inline void
ospf_check_defcost(int cost)
{
  if ((cost <= 0) || (cost >= LSINFINITY))
   cf_error("Default cost must be in range 1-%u", LSINFINITY-1);
}

static inline void
ospf_check_auth(void)
{
  if (ospf_cfg_is_v3())
    cf_error("Plaintext authentication not supported in OSPFv3");
}


#line 9 "proto/perf/config.Y"
/* Headers from proto/perf/config.Y */

#include "filter/filter.h"
#include "proto/perf/perf.h"

/* Defines from proto/perf/config.Y */

#define PERF_CFG ((struct perf_config *) this_proto)

#line 9 "proto/pipe/config.Y"
/* Headers from proto/pipe/config.Y */

#include "proto/pipe/pipe.h"

/* Defines from proto/pipe/config.Y */

#define PIPE_CFG ((struct pipe_config *) this_proto)

#line 10 "proto/radv/config.Y"
/* Headers from proto/radv/config.Y */

#include "proto/radv/radv.h"

/* Defines from proto/radv/config.Y */

#define RADV_CFG ((struct radv_config *) this_proto)
#define RADV_IFACE ((struct radv_iface_config *) this_ipatt)
#define RADV_PREFIX this_radv_prefix
#define RADV_RDNSS (&this_radv_rdnss)
#define RADV_DNSSL (&this_radv_dnssl)

static struct radv_prefix_config *this_radv_prefix;
static struct radv_rdnss_config this_radv_rdnss;
static struct radv_dnssl_config this_radv_dnssl;
static list radv_dns_list;	/* Used by radv_rdnss and radv_dnssl */
static u8 radv_mult_val;	/* Used by radv_mult for second return value */


#line 12 "proto/rip/config.Y"
/* Headers from proto/rip/config.Y */

#include "proto/rip/rip.h"
#include "nest/iface.h"

/* Defines from proto/rip/config.Y */

#define RIP_CFG ((struct rip_config *) this_proto)
#define RIP_IFACE ((struct rip_iface_config *) this_ipatt)

static inline int rip_cfg_is_v2(void) { return RIP_CFG->rip2; }
static inline int rip_cfg_is_ng(void) { return ! RIP_CFG->rip2; }

static inline void
rip_check_auth(void)
{
  if (rip_cfg_is_ng())
    cf_error("Authentication not supported in RIPng");
}


#line 9 "proto/rpki/config.Y"
/* Headers from proto/rpki/config.Y */

#include "proto/rpki/rpki.h"

/* Defines from proto/rpki/config.Y */

#define RPKI_CFG ((struct rpki_config *) this_proto)
#define RPKI_TR_SSH_CFG ((struct rpki_tr_ssh_config *) RPKI_CFG->tr_config.spec)

static void
rpki_check_unused_hostname(void)
{
  if (RPKI_CFG->hostname != NULL)
    cf_error("Only one cache server per protocol allowed");
}

static void
rpki_check_unused_transport(void)
{
  if (RPKI_CFG->tr_config.spec != NULL)
    cf_error("At the most one transport per protocol allowed");
}

#line 9 "proto/static/config.Y"
/* Headers from proto/static/config.Y */

#include "proto/static/static.h"

/* Defines from proto/static/config.Y */

#define STATIC_CFG ((struct static_config *) this_proto)
static struct static_route *this_srt, *this_snh;
static struct f_inst *this_srt_cmds, *this_srt_last_cmd;

static struct static_route *
static_nexthop_new(void)
{
  struct static_route *nh = this_srt;

  if (this_snh)
  {
    /* Additional next hop */
    nh = cfg_allocz(sizeof(struct static_route));
    nh->net = this_srt->net;
    this_snh->mp_next = nh;
  }

  nh->dest = RTD_UNICAST;
  nh->mp_head = this_srt;
  return nh;
};

static void
static_route_finish(void)
{
  if (net_type_match(this_srt->net, NB_DEST) == !this_srt->dest)
    cf_error("Unexpected or missing nexthop/type");

  this_srt->cmds = f_linearize(this_srt_cmds, 0);
}

#line 9 "sysdep/linux/netlink.Y"
/* Headers from sysdep/linux/netlink.Y */

#line 9 "sysdep/unix/config.Y"
/* Headers from sysdep/unix/config.Y */

#include "sysdep/unix/unix.h"
#include <stdio.h>

/* Defines from sysdep/unix/config.Y */

static struct log_config *this_log;

#line 9 "sysdep/unix/krt.Y"
/* Headers from sysdep/unix/krt.Y */

#include "sysdep/unix/krt.h"

/* Defines from sysdep/unix/krt.Y */

#define THIS_KRT ((struct krt_config *) this_proto)
#define THIS_KIF ((struct kif_config *) this_proto)
#define KIF_IFACE ((struct kif_iface_config *) this_ipatt)

static void
kif_set_preferred(ip_addr ip)
{
  if (ipa_is_ip4(ip))
    KIF_IFACE->pref_v4 = ip;
  else if (!ipa_is_link_local(ip))
    KIF_IFACE->pref_v6 = ip;
  else
    KIF_IFACE->pref_ll = ip;
}

#line 50 "conf/gen_parser.m4"

#line 918 "obj/conf/cf-parse.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_CF_OBJ_CONF_CF_PARSE_TAB_H_INCLUDED
# define YY_CF_OBJ_CONF_CF_PARSE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int cf_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 258,
    CLI_MARKER = 259,
    INVALID_TOKEN = 260,
    ELSECOL = 261,
    DDOT = 262,
    GEQ = 263,
    LEQ = 264,
    NEQ = 265,
    AND = 266,
    OR = 267,
    PO = 268,
    PC = 269,
    NUM = 270,
    ENUM = 271,
    IP4 = 272,
    IP6 = 273,
    VPN_RD = 274,
    CF_SYM_KNOWN = 275,
    CF_SYM_UNDEFINED = 276,
    TEXT = 277,
    BYTESTRING = 278,
    PREFIX_DUMMY = 279,
    NMA = 280,
    DEFINE = 281,
    ON = 282,
    OFF = 283,
    YES = 284,
    NO = 285,
    S = 286,
    MS = 287,
    US = 288,
    PORT = 289,
    VPN = 290,
    MPLS = 291,
    FROM = 292,
    FLOW4 = 293,
    FLOW6 = 294,
    DST = 295,
    SRC = 296,
    PROTO = 297,
    NEXT = 298,
    HEADER = 299,
    DPORT = 300,
    SPORT = 301,
    ICMP = 302,
    TYPE = 303,
    CODE = 304,
    TCP = 305,
    FLAGS = 306,
    LENGTH = 307,
    DSCP = 308,
    DONT_FRAGMENT = 309,
    IS_FRAGMENT = 310,
    FIRST_FRAGMENT = 311,
    LAST_FRAGMENT = 312,
    FRAGMENT = 313,
    LABEL = 314,
    OFFSET = 315,
    QUIT = 316,
    EXIT = 317,
    HELP = 318,
    FUNCTION = 319,
    PRINT = 320,
    PRINTN = 321,
    UNSET = 322,
    RETURN = 323,
    ACCEPT = 324,
    REJECT = 325,
    ERROR = 326,
    INT = 327,
    BOOL = 328,
    IP = 329,
    PREFIX = 330,
    RD = 331,
    PAIR = 332,
    QUAD = 333,
    EC = 334,
    LC = 335,
    SET = 336,
    STRING = 337,
    BGPMASK = 338,
    BGPPATH = 339,
    CLIST = 340,
    ECLIST = 341,
    LCLIST = 342,
    IF = 343,
    THEN = 344,
    ELSE = 345,
    CASE = 346,
    FOR = 347,
    IN = 348,
    DO = 349,
    TRUE = 350,
    FALSE = 351,
    RT = 352,
    RO = 353,
    UNKNOWN = 354,
    GENERIC = 355,
    GW = 356,
    NET = 357,
    MASK = 358,
    SOURCE = 359,
    SCOPE = 360,
    DEST = 361,
    IFNAME = 362,
    IFINDEX = 363,
    WEIGHT = 364,
    GW_MPLS = 365,
    ONLINK = 366,
    PREFERENCE = 367,
    ROA_CHECK = 368,
    ASN = 369,
    IS_V4 = 370,
    IS_V6 = 371,
    LEN = 372,
    MAXLEN = 373,
    DATA = 374,
    DATA1 = 375,
    DATA2 = 376,
    DEFINED = 377,
    ADD = 378,
    DELETE = 379,
    CONTAINS = 380,
    RESET = 381,
    PREPEND = 382,
    FIRST = 383,
    LAST = 384,
    LAST_NONAGGREGATED = 385,
    MATCH = 386,
    MIN = 387,
    MAX = 388,
    EMPTY = 389,
    FILTER = 390,
    WHERE = 391,
    EVAL = 392,
    ATTRIBUTE = 393,
    BT_ASSERT = 394,
    BT_TEST_SUITE = 395,
    BT_CHECK_ASSIGN = 396,
    BT_TEST_SAME = 397,
    FORMAT = 398,
    ROUTER = 399,
    ID = 400,
    HOSTNAME = 401,
    PROTOCOL = 402,
    TEMPLATE = 403,
    DISABLED = 404,
    DEBUG = 405,
    ALL = 406,
    DIRECT = 407,
    INTERFACE = 408,
    IMPORT = 409,
    EXPORT = 410,
    NONE = 411,
    VRF = 412,
    DEFAULT = 413,
    TABLE = 414,
    STATES = 415,
    ROUTES = 416,
    FILTERS = 417,
    IPV4 = 418,
    IPV6 = 419,
    VPN4 = 420,
    VPN6 = 421,
    ROA4 = 422,
    ROA6 = 423,
    SADR = 424,
    RECEIVE = 425,
    LIMIT = 426,
    ACTION = 427,
    WARN = 428,
    BLOCK = 429,
    RESTART = 430,
    DISABLE = 431,
    KEEP = 432,
    FILTERED = 433,
    RPKI = 434,
    PASSWORD = 435,
    KEY = 436,
    PASSIVE = 437,
    TO = 438,
    EVENTS = 439,
    PACKETS = 440,
    PROTOCOLS = 441,
    CHANNELS = 442,
    INTERFACES = 443,
    ALGORITHM = 444,
    KEYED = 445,
    HMAC = 446,
    MD5 = 447,
    SHA1 = 448,
    SHA256 = 449,
    SHA384 = 450,
    SHA512 = 451,
    BLAKE2S128 = 452,
    BLAKE2S256 = 453,
    BLAKE2B256 = 454,
    BLAKE2B512 = 455,
    PRIMARY = 456,
    STATS = 457,
    COUNT = 458,
    BY = 459,
    COMMANDS = 460,
    PREEXPORT = 461,
    NOEXPORT = 462,
    EXPORTED = 463,
    GENERATE = 464,
    BGP = 465,
    PASSWORDS = 466,
    DESCRIPTION = 467,
    RELOAD = 468,
    OUT = 469,
    MRTDUMP = 470,
    MESSAGES = 471,
    RESTRICT = 472,
    MEMORY = 473,
    IGP_METRIC = 474,
    CLASS = 475,
    TIMEFORMAT = 476,
    ISO = 477,
    SHORT = 478,
    LONG = 479,
    ROUTE = 480,
    BASE = 481,
    LOG = 482,
    GRACEFUL = 483,
    WAIT = 484,
    FLUSH = 485,
    AS = 486,
    IDLE = 487,
    RX = 488,
    TX = 489,
    INTERVAL = 490,
    MULTIPLIER = 491,
    CHECK = 492,
    LINK = 493,
    SORTED = 494,
    TRIE = 495,
    SETTLE = 496,
    TIME = 497,
    GC = 498,
    THRESHOLD = 499,
    PERIOD = 500,
    IPV4_MC = 501,
    IPV4_MPLS = 502,
    IPV6_MC = 503,
    IPV6_MPLS = 504,
    IPV6_SADR = 505,
    VPN4_MC = 506,
    VPN4_MPLS = 507,
    VPN6_MC = 508,
    VPN6_MPLS = 509,
    PRI = 510,
    SEC = 511,
    SHOW = 512,
    STATUS = 513,
    SUMMARY = 514,
    SYMBOLS = 515,
    DUMP = 516,
    RESOURCES = 517,
    SOCKETS = 518,
    NEIGHBORS = 519,
    ATTRIBUTES = 520,
    ECHO = 521,
    ENABLE = 522,
    BFD = 523,
    MULTIHOP = 524,
    NEIGHBOR = 525,
    DEV = 526,
    LOCAL = 527,
    AUTHENTICATION = 528,
    SIMPLE = 529,
    METICULOUS = 530,
    STRICT = 531,
    BIND = 532,
    SESSIONS = 533,
    BABEL = 534,
    METRIC = 535,
    RXCOST = 536,
    HELLO = 537,
    UPDATE = 538,
    WIRED = 539,
    WIRELESS = 540,
    BUFFER = 541,
    PRIORITY = 542,
    HOP = 543,
    BABEL_METRIC = 544,
    ENTRIES = 545,
    RANDOMIZE = 546,
    MAC = 547,
    PERMISSIVE = 548,
    EXTENDED = 549,
    HOLD = 550,
    CONNECT = 551,
    RETRY = 552,
    KEEPALIVE = 553,
    STARTUP = 554,
    VIA = 555,
    SELF = 556,
    PATH = 557,
    START = 558,
    DELAY = 559,
    FORGET = 560,
    AFTER = 561,
    BGP_PATH = 562,
    BGP_LOCAL_PREF = 563,
    BGP_MED = 564,
    BGP_ORIGIN = 565,
    BGP_NEXT_HOP = 566,
    BGP_ATOMIC_AGGR = 567,
    BGP_AGGREGATOR = 568,
    BGP_COMMUNITY = 569,
    BGP_EXT_COMMUNITY = 570,
    BGP_LARGE_COMMUNITY = 571,
    ADDRESS = 572,
    RR = 573,
    RS = 574,
    CLIENT = 575,
    CLUSTER = 576,
    AS4 = 577,
    ADVERTISE = 578,
    CAPABILITIES = 579,
    PREFER = 580,
    OLDER = 581,
    MISSING = 582,
    LLADDR = 583,
    DROP = 584,
    IGNORE = 585,
    REFRESH = 586,
    INTERPRET = 587,
    COMMUNITIES = 588,
    BGP_ORIGINATOR_ID = 589,
    BGP_CLUSTER_LIST = 590,
    IGP = 591,
    GATEWAY = 592,
    RECURSIVE = 593,
    MED = 594,
    TTL = 595,
    SECURITY = 596,
    DETERMINISTIC = 597,
    SECONDARY = 598,
    ALLOW = 599,
    PATHS = 600,
    AWARE = 601,
    SETKEY = 602,
    CONFEDERATION = 603,
    MEMBER = 604,
    MULTICAST = 605,
    LIVED = 606,
    STALE = 607,
    IBGP = 608,
    EBGP = 609,
    MANDATORY = 610,
    INTERNAL = 611,
    EXTERNAL = 612,
    SETS = 613,
    DYNAMIC = 614,
    RANGE = 615,
    NAME = 616,
    DIGITS = 617,
    BGP_AIGP = 618,
    AIGP = 619,
    ORIGINATE = 620,
    COST = 621,
    ENFORCE = 622,
    FREE = 623,
    VALIDATE = 624,
    ROLE = 625,
    ROLES = 626,
    PEER = 627,
    PROVIDER = 628,
    CUSTOMER = 629,
    RS_SERVER = 630,
    RS_CLIENT = 631,
    REQUIRE = 632,
    BGP_OTC = 633,
    GLOBAL = 634,
    CEASE = 635,
    HIT = 636,
    ADMINISTRATIVE = 637,
    SHUTDOWN = 638,
    CONFIGURATION = 639,
    CHANGE = 640,
    DECONFIGURED = 641,
    CONNECTION = 642,
    REJECTED = 643,
    COLLISION = 644,
    OF = 645,
    MRT = 646,
    FILENAME = 647,
    ALWAYS = 648,
    OSPF = 649,
    V2 = 650,
    V3 = 651,
    OSPF_METRIC1 = 652,
    OSPF_METRIC2 = 653,
    OSPF_TAG = 654,
    OSPF_ROUTER_ID = 655,
    AREA = 656,
    RFC1583COMPAT = 657,
    STUB = 658,
    TICK = 659,
    COST2 = 660,
    RETRANSMIT = 661,
    TRANSMIT = 662,
    DEAD = 663,
    BROADCAST = 664,
    BCAST = 665,
    NONBROADCAST = 666,
    NBMA = 667,
    POINTOPOINT = 668,
    PTP = 669,
    POINTOMULTIPOINT = 670,
    PTMP = 671,
    CRYPTOGRAPHIC = 672,
    ELIGIBLE = 673,
    POLL = 674,
    NETWORKS = 675,
    HIDDEN = 676,
    VIRTUAL = 677,
    ONLY = 678,
    LARGE = 679,
    NORMAL = 680,
    STUBNET = 681,
    TAG = 682,
    LSADB = 683,
    ECMP = 684,
    NSSA = 685,
    TRANSLATOR = 686,
    STABILITY = 687,
    LSID = 688,
    INSTANCE = 689,
    REAL = 690,
    NETMASK = 691,
    MERGE = 692,
    LSA = 693,
    SUPPRESSION = 694,
    RFC5838 = 695,
    PE = 696,
    TOPOLOGY = 697,
    STATE = 698,
    PERF = 699,
    EXP = 700,
    REPEAT = 701,
    MODE = 702,
    PIPE = 703,
    RADV = 704,
    RA = 705,
    SOLICITED = 706,
    UNICAST = 707,
    MANAGED = 708,
    OTHER = 709,
    CONFIG = 710,
    LINGER = 711,
    MTU = 712,
    REACHABLE = 713,
    RETRANS = 714,
    TIMER = 715,
    CURRENT = 716,
    VALID = 717,
    PREFERRED = 718,
    MULT = 719,
    LIFETIME = 720,
    SKIP = 721,
    AUTONOMOUS = 722,
    RDNSS = 723,
    DNSSL = 724,
    NS = 725,
    DOMAIN = 726,
    TRIGGER = 727,
    SENSITIVE = 728,
    LOW = 729,
    MEDIUM = 730,
    HIGH = 731,
    PROPAGATE = 732,
    RA_PREFERENCE = 733,
    RA_LIFETIME = 734,
    RIP = 735,
    NG = 736,
    INFINITY = 737,
    TIMEOUT = 738,
    GARBAGE = 739,
    VERSION = 740,
    SPLIT = 741,
    HORIZON = 742,
    POISON = 743,
    REVERSE = 744,
    ZERO = 745,
    PLAINTEXT = 746,
    DEMAND = 747,
    CIRCUIT = 748,
    RIP_METRIC = 749,
    RIP_TAG = 750,
    REMOTE = 751,
    BIRD = 752,
    PRIVATE = 753,
    PUBLIC = 754,
    SSH = 755,
    TRANSPORT = 756,
    USER = 757,
    EXPIRE = 758,
    STATIC = 759,
    PROHIBIT = 760,
    BLACKHOLE = 761,
    UNREACHABLE = 762,
    KERNEL = 763,
    NETLINK = 764,
    KRT_PREFSRC = 765,
    KRT_REALM = 766,
    KRT_SCOPE = 767,
    KRT_MTU = 768,
    KRT_WINDOW = 769,
    KRT_RTT = 770,
    KRT_RTTVAR = 771,
    KRT_SSTRESH = 772,
    KRT_CWND = 773,
    KRT_ADVMSS = 774,
    KRT_REORDERING = 775,
    KRT_HOPLIMIT = 776,
    KRT_INITCWND = 777,
    KRT_RTO_MIN = 778,
    KRT_INITRWND = 779,
    KRT_QUICKACK = 780,
    KRT_LOCK_MTU = 781,
    KRT_LOCK_WINDOW = 782,
    KRT_LOCK_RTT = 783,
    KRT_LOCK_RTTVAR = 784,
    KRT_LOCK_SSTRESH = 785,
    KRT_LOCK_CWND = 786,
    KRT_LOCK_ADVMSS = 787,
    KRT_LOCK_REORDERING = 788,
    KRT_LOCK_HOPLIMIT = 789,
    KRT_LOCK_RTO_MIN = 790,
    KRT_FEATURE_ECN = 791,
    KRT_FEATURE_ALLFRAG = 792,
    SYSLOG = 793,
    TRACE = 794,
    INFO = 795,
    WARNING = 796,
    AUTH = 797,
    FATAL = 798,
    BUG = 799,
    STDERR = 800,
    SOFT = 801,
    CONFIRM = 802,
    UNDO = 803,
    LATENCY = 804,
    WATCHDOG = 805,
    CONFIGURE = 806,
    DOWN = 807,
    PERSIST = 808,
    SCAN = 809,
    LEARN = 810,
    DEVICE = 811,
    KRT_SOURCE = 812,
    KRT_METRIC = 813
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 54 "conf/confbase.Y"

  uint i;
  u32 i32;
  u64 i64;
  ip_addr a;
  ip4_addr ip4;
  ip6_addr ip6;
  net_addr net;
  net_addr *net_ptr;
  struct symbol *s;
  const char *t;
  struct rtable_config *r;
  struct channel_config *cc;
  struct channel *c;
  struct f_inst *x;
  struct {
    struct f_inst *begin, *end;
  } xp;
  enum filter_return fret;
  enum ec_subtype ecs;
  struct f_dynamic_attr fda;
  struct f_static_attr fsa;
  struct f_lval flv;
  struct f_line *fl;
  struct f_arg *fa;
  const struct filter *f;
  struct f_tree *e;
  struct f_trie *trie;
  struct f_val v;
  struct password_item *p;
  struct rt_show_data *ra;
  struct sym_show_data *sd;
  struct lsadb_show_data *ld;
  struct mrt_dump_data *md;
  struct iface *iface;
  void *g;
  btime time;
  struct f_prefix px;
  struct proto_spec ps;
  struct channel_limit cl;
  struct timeformat *tf;
  mpls_label_stack *mls;
  struct bytestring *bs;

#line 1574 "obj/conf/cf-parse.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE cf_lval;

int cf_parse (void);

#endif /* !YY_CF_OBJ_CONF_CF_PARSE_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
# define YYCOPY_NEEDED 1
#endif


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  79
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   7348

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  580
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  439
/* YYNRULES -- Number of rules.  */
#define YYNRULES  1492
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  2738

#define YYUNDEFTOK  2
#define YYMAXUTOK   813


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    35,     2,     2,     2,    34,     2,     2,
     571,   572,    32,    30,   573,    31,    36,    33,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   576,   570,
      26,    25,    27,   579,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   577,     2,   578,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   574,     2,   575,    28,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      29,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,   255,
     256,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   308,   309,   310,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   362,   363,   364,   365,
     366,   367,   368,   369,   370,   371,   372,   373,   374,   375,
     376,   377,   378,   379,   380,   381,   382,   383,   384,   385,
     386,   387,   388,   389,   390,   391,   392,   393,   394,   395,
     396,   397,   398,   399,   400,   401,   402,   403,   404,   405,
     406,   407,   408,   409,   410,   411,   412,   413,   414,   415,
     416,   417,   418,   419,   420,   421,   422,   423,   424,   425,
     426,   427,   428,   429,   430,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   449,   450,   451,   452,   453,   454,   455,
     456,   457,   458,   459,   460,   461,   462,   463,   464,   465,
     466,   467,   468,   469,   470,   471,   472,   473,   474,   475,
     476,   477,   478,   479,   480,   481,   482,   483,   484,   485,
     486,   487,   488,   489,   490,   491,   492,   493,   494,   495,
     496,   497,   498,   499,   500,   501,   502,   503,   504,   505,
     506,   507,   508,   509,   510,   511,   512,   513,   514,   515,
     516,   517,   518,   519,   520,   521,   522,   523,   524,   525,
     526,   527,   528,   529,   530,   531,   532,   533,   534,   535,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   548,   549,   550,   551,   552,   553,   554,   555,
     556,   557,   558,   559,   560,   561,   562,   563,   564,   565,
     566,   567,   568,   569
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   137,   137,   138,   141,   143,   146,   151,   154,   162,
     163,   164,   170,   171,   172,   175,   175,   180,   181,   182,
     183,   184,   185,   192,   193,   194,   201,   202,   209,   215,
     225,   238,   257,   263,   269,   277,   285,   291,   291,   292,
     292,   293,   293,   296,   297,   298,   299,   300,   301,   308,
     309,   317,   318,   326,   327,   335,   336,   344,   345,   346,
     347,   348,   358,   366,   367,   376,   384,   385,   392,   393,
      37,    38,    39,    40,    41,    42,    43,    44,    48,    49,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      65,    66,    67,    70,    71,    75,    79,    83,    84,    88,
      92,   101,   102,   106,   110,   114,   121,   122,   126,   127,
     128,   129,   133,   136,   139,   145,   149,   150,   151,   155,
     159,   165,   166,   167,   170,   172,   175,   177,   181,   188,
     189,   191,   197,   203,   203,     9,     9,    10,    10,    11,
      11,   346,   348,   348,   358,   360,   363,   364,   368,   370,
     381,   383,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   435,   436,
     445,   446,   454,   455,   461,   464,   468,   468,   478,   485,
     491,   493,   497,   493,   521,   522,   525,   525,   527,   527,
     529,   537,   538,   555,   556,   569,   570,   571,   572,   573,
     577,   585,   586,   587,   588,   592,   595,   596,   597,   598,
     599,   600,   601,   602,   603,   604,   609,   610,   611,   612,
     616,   617,   618,   622,   623,   624,   625,   626,   627,   628,
     629,   634,   635,   636,   637,   638,   642,   643,   644,   645,
     646,   650,   651,   655,   656,   660,   661,   662,   663,   671,
     672,   675,   676,   683,   693,   694,   698,   702,   703,   704,
     705,   709,   710,   711,   712,   713,   717,   718,   719,   720,
     721,   722,   723,   724,   725,   730,   731,   735,   736,   737,
     738,   743,   744,   745,   748,   767,   786,   787,   788,   789,
     790,   791,   792,   793,   794,   795,   796,   797,   798,   802,
     803,   804,   805,   806,   807,   808,   809,   810,   811,   812,
     813,   814,   815,   816,   817,   818,   820,   821,   822,   824,
     826,   828,   829,   830,   831,   832,   833,   834,   835,   836,
     837,   838,   839,   840,   841,   842,   843,   844,   845,   855,
     856,   857,   858,   859,   860,   861,   862,   864,   865,   867,
     871,   875,   876,   877,   880,   881,   882,   891,   892,   896,
     902,   903,   907,   910,   913,   916,   922,   922,   916,   928,
     940,   944,   947,   952,   955,   966,   970,   973,   974,   978,
     979,   980,   981,   982,   983,   984,   988,   993,   994,   995,
     159,   162,   163,   167,   168,   169,   170,   180,   182,   184,
     186,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   209,   211,   213,   218,   220,   224,   225,   230,   231,
     232,   233,   236,   238,   241,   243,   249,   252,   253,   257,
     263,   267,   277,   287,   289,   290,   291,   292,   293,   294,
     295,   299,   305,   310,   311,   312,   313,   314,   315,   316,
     317,   322,   323,   326,   328,   331,   333,   337,   344,   347,
     350,   351,   352,   353,   357,   358,   359,   360,   361,   365,
     366,   370,   373,   374,   375,   380,   383,   384,   385,   386,
     390,   391,   392,   393,   394,   395,   396,   397,   401,   408,
     416,   417,   421,   422,   426,   431,   432,   436,   438,   446,
     450,   451,   456,   458,   465,   466,   467,   468,   469,   473,
     481,   487,   488,   489,   493,   494,   498,   499,   500,   501,
     502,   503,   509,   510,   511,   515,   516,   520,   521,   527,
     531,   532,   535,   537,   541,   542,   545,   545,   548,   549,
     553,   554,   555,   556,   557,   558,   559,   560,   561,   565,
     566,   567,   568,   569,   570,   571,   572,   573,   574,   575,
     576,   577,   578,   582,   590,   591,   592,   593,   594,   595,
     596,   599,   601,   605,   611,   611,   614,   614,   617,   617,
     620,   620,   624,   625,   628,   628,   631,   631,   635,   635,
     639,   645,   651,   657,   664,   670,   677,   682,   687,   692,
     697,   701,   705,   709,   719,   726,   735,   739,   746,   750,
     751,   752,   753,   757,   761,   765,   779,   780,   781,   782,
     785,   786,   787,   788,   793,   794,   795,   796,   797,   798,
     799,   800,   801,   802,   803,   804,   805,   806,   807,   808,
     809,   810,   811,   812,   816,   826,   826,   830,   833,   834,
     835,   836,   837,   838,   843,   843,   845,   845,   847,   847,
     849,   849,   851,   851,   853,   853,   855,   855,   857,   857,
     859,   859,   862,   862,   866,   866,   872,   873,   874,   878,
     879,   885,   885,   887,   887,   889,   889,   891,   891,   893,
     893,   895,   895,   899,   899,   903,   904,   908,   908,   911,
     911,   915,   916,   917,   921,   922,   923,   926,    35,    37,
      47,    48,    49,    50,    51,    52,    55,    57,    61,    65,
      66,    67,    68,    72,    76,    80,    94,   114,   115,   116,
     117,   118,   119,   120,   121,   125,   126,   127,   128,   129,
     130,   133,   135,   138,   140,   144,   147,   152,   153,   154,
     158,   159,   163,   164,   167,   186,   186,    33,    35,    43,
      44,    45,    46,    49,    51,    55,    59,    76,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   155,   157,
     160,   162,   167,   169,   173,   173,   176,   176,   179,   179,
     182,   182,    49,    49,    49,    49,    49,    49,    51,    53,
      85,    87,    88,    91,    93,    94,    95,    96,   101,   102,
     106,   107,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   123,   124,   125,   126,   127,   131,   132,   133,   134,
     135,   139,   140,   146,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   214,   214,   215,   216,   217,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   235,
     261,   262,   263,   265,   265,   265,   268,   269,   270,   271,
     272,   273,   274,   275,   276,   277,   278,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     306,   318,   320,   323,   325,   329,   336,   339,   341,   343,
     345,   347,   349,   351,   353,   355,   357,   359,   361,   363,
     365,    26,    28,    34,    35,    36,    37,    38,    39,    40,
      41,    44,    46,    50,    53,    53,    57,    58,    59,    60,
      61,    62,   211,   214,   215,   216,   219,   235,   236,   240,
     241,   245,   254,   257,   258,   259,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   274,   289,
     292,   294,   298,   299,   300,   301,   302,   303,   304,   305,
     306,   307,   307,   308,   308,   309,   310,   311,   315,   316,
     320,   328,   330,   334,   335,   336,   340,   341,   344,   346,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   362,   381,   382,   383,   384,   385,   386,   387,   388,
     389,   390,   391,   392,   393,   394,   395,   396,   397,   398,
     399,   400,   401,   402,   403,   404,   405,   406,   407,   408,
     409,   410,   411,   412,   413,   414,   415,   416,   417,   418,
     419,   422,   424,   427,   429,   437,   439,   440,   443,   445,
     449,   450,   453,   463,   485,   487,   491,   491,   494,   496,
     499,   501,   505,   508,   509,   510,   511,   514,   514,   517,
     517,   520,   520,   525,   525,   528,   528,   533,   533,   536,
     536,   540,   540,   549,   552,   553,   554,   555,   556,   557,
     558,   559,    24,    26,    40,    41,    42,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    23,    26,    25,
      40,    41,    42,    43,    44,    46,    58,    59,    60,    61,
      62,    62,    63,    63,    64,    65,    68,    70,    74,    78,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   116,   120,   121,   122,   123,   124,   125,   125,   126,
     126,   127,   128,   132,   133,   134,   137,   175,   177,   180,
     182,   186,   189,   201,   202,   203,   204,   208,   215,   223,
     225,   228,   230,   234,   238,   248,   253,   254,   255,   259,
     271,   273,   277,   278,   282,   295,   300,   301,   302,   306,
     318,   320,   324,   325,   330,   331,   335,   336,   339,   340,
      46,    49,    50,    53,    67,    68,    69,    70,    71,    72,
      75,    77,    81,    85,   110,   140,   141,   142,   143,   144,
     145,   146,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   173,   174,   175,   176,   179,   181,   184,   186,
     190,   193,   194,   198,   198,   201,   201,    41,    43,    50,
      52,    54,    58,    59,    60,    61,    62,    63,    64,    70,
      76,    82,    86,    87,    90,    93,    97,   108,   109,   113,
     120,   130,   132,   136,   137,   138,   142,    54,    56,    63,
      64,    65,    66,    67,    75,    79,    84,    89,    92,    95,
      99,   105,   106,   109,   121,   122,   126,   131,   132,   133,
     134,   135,   136,   140,   149,   151,   154,   156,   160,   160,
      23,    26,    27,    28,    31,    32,    33,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      62,    63,    31,    33,    35,    42,    43,    46,    48,    52,
      62,    63,    67,    68,    72,    73,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    89,    92,    93,   104,   107,
     108,   109,   110,   118,   118,   121,   121,   128,   128,   131,
     131,   134,   134,   137,   137,   140,   140,   145,   145,   150,
     151,   155,   156,   157,    41,    43,    48,    49,    52,    53,
      57,    58,    59,    60,    64,    71,    72,    83,    85,    88,
      89,    92,    93,    94,   101,   108,   111,   113,   116,   118,
     122,   125,   126
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "END", "CLI_MARKER", "INVALID_TOKEN",
  "ELSECOL", "DDOT", "GEQ", "LEQ", "NEQ", "AND", "OR", "PO", "PC", "NUM",
  "ENUM", "IP4", "IP6", "VPN_RD", "CF_SYM_KNOWN", "CF_SYM_UNDEFINED",
  "TEXT", "BYTESTRING", "PREFIX_DUMMY", "'='", "'<'", "'>'", "'~'", "NMA",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'!'", "'.'", "DEFINE", "ON", "OFF",
  "YES", "NO", "S", "MS", "US", "PORT", "VPN", "MPLS", "FROM", "FLOW4",
  "FLOW6", "DST", "SRC", "PROTO", "NEXT", "HEADER", "DPORT", "SPORT",
  "ICMP", "TYPE", "CODE", "TCP", "FLAGS", "LENGTH", "DSCP",
  "DONT_FRAGMENT", "IS_FRAGMENT", "FIRST_FRAGMENT", "LAST_FRAGMENT",
  "FRAGMENT", "LABEL", "OFFSET", "QUIT", "EXIT", "HELP", "FUNCTION",
  "PRINT", "PRINTN", "UNSET", "RETURN", "ACCEPT", "REJECT", "ERROR", "INT",
  "BOOL", "IP", "PREFIX", "RD", "PAIR", "QUAD", "EC", "LC", "SET",
  "STRING", "BGPMASK", "BGPPATH", "CLIST", "ECLIST", "LCLIST", "IF",
  "THEN", "ELSE", "CASE", "FOR", "IN", "DO", "TRUE", "FALSE", "RT", "RO",
  "UNKNOWN", "GENERIC", "GW", "NET", "MASK", "SOURCE", "SCOPE", "DEST",
  "IFNAME", "IFINDEX", "WEIGHT", "GW_MPLS", "ONLINK", "PREFERENCE",
  "ROA_CHECK", "ASN", "IS_V4", "IS_V6", "LEN", "MAXLEN", "DATA", "DATA1",
  "DATA2", "DEFINED", "ADD", "DELETE", "CONTAINS", "RESET", "PREPEND",
  "FIRST", "LAST", "LAST_NONAGGREGATED", "MATCH", "MIN", "MAX", "EMPTY",
  "FILTER", "WHERE", "EVAL", "ATTRIBUTE", "BT_ASSERT", "BT_TEST_SUITE",
  "BT_CHECK_ASSIGN", "BT_TEST_SAME", "FORMAT", "ROUTER", "ID", "HOSTNAME",
  "PROTOCOL", "TEMPLATE", "DISABLED", "DEBUG", "ALL", "DIRECT",
  "INTERFACE", "IMPORT", "EXPORT", "NONE", "VRF", "DEFAULT", "TABLE",
  "STATES", "ROUTES", "FILTERS", "IPV4", "IPV6", "VPN4", "VPN6", "ROA4",
  "ROA6", "SADR", "RECEIVE", "LIMIT", "ACTION", "WARN", "BLOCK", "RESTART",
  "DISABLE", "KEEP", "FILTERED", "RPKI", "PASSWORD", "KEY", "PASSIVE",
  "TO", "EVENTS", "PACKETS", "PROTOCOLS", "CHANNELS", "INTERFACES",
  "ALGORITHM", "KEYED", "HMAC", "MD5", "SHA1", "SHA256", "SHA384",
  "SHA512", "BLAKE2S128", "BLAKE2S256", "BLAKE2B256", "BLAKE2B512",
  "PRIMARY", "STATS", "COUNT", "BY", "COMMANDS", "PREEXPORT", "NOEXPORT",
  "EXPORTED", "GENERATE", "BGP", "PASSWORDS", "DESCRIPTION", "RELOAD",
  "OUT", "MRTDUMP", "MESSAGES", "RESTRICT", "MEMORY", "IGP_METRIC",
  "CLASS", "TIMEFORMAT", "ISO", "SHORT", "LONG", "ROUTE", "BASE", "LOG",
  "GRACEFUL", "WAIT", "FLUSH", "AS", "IDLE", "RX", "TX", "INTERVAL",
  "MULTIPLIER", "CHECK", "LINK", "SORTED", "TRIE", "SETTLE", "TIME", "GC",
  "THRESHOLD", "PERIOD", "IPV4_MC", "IPV4_MPLS", "IPV6_MC", "IPV6_MPLS",
  "IPV6_SADR", "VPN4_MC", "VPN4_MPLS", "VPN6_MC", "VPN6_MPLS", "PRI",
  "SEC", "SHOW", "STATUS", "SUMMARY", "SYMBOLS", "DUMP", "RESOURCES",
  "SOCKETS", "NEIGHBORS", "ATTRIBUTES", "ECHO", "ENABLE", "BFD",
  "MULTIHOP", "NEIGHBOR", "DEV", "LOCAL", "AUTHENTICATION", "SIMPLE",
  "METICULOUS", "STRICT", "BIND", "SESSIONS", "BABEL", "METRIC", "RXCOST",
  "HELLO", "UPDATE", "WIRED", "WIRELESS", "BUFFER", "PRIORITY", "HOP",
  "BABEL_METRIC", "ENTRIES", "RANDOMIZE", "MAC", "PERMISSIVE", "EXTENDED",
  "HOLD", "CONNECT", "RETRY", "KEEPALIVE", "STARTUP", "VIA", "SELF",
  "PATH", "START", "DELAY", "FORGET", "AFTER", "BGP_PATH",
  "BGP_LOCAL_PREF", "BGP_MED", "BGP_ORIGIN", "BGP_NEXT_HOP",
  "BGP_ATOMIC_AGGR", "BGP_AGGREGATOR", "BGP_COMMUNITY",
  "BGP_EXT_COMMUNITY", "BGP_LARGE_COMMUNITY", "ADDRESS", "RR", "RS",
  "CLIENT", "CLUSTER", "AS4", "ADVERTISE", "CAPABILITIES", "PREFER",
  "OLDER", "MISSING", "LLADDR", "DROP", "IGNORE", "REFRESH", "INTERPRET",
  "COMMUNITIES", "BGP_ORIGINATOR_ID", "BGP_CLUSTER_LIST", "IGP", "GATEWAY",
  "RECURSIVE", "MED", "TTL", "SECURITY", "DETERMINISTIC", "SECONDARY",
  "ALLOW", "PATHS", "AWARE", "SETKEY", "CONFEDERATION", "MEMBER",
  "MULTICAST", "LIVED", "STALE", "IBGP", "EBGP", "MANDATORY", "INTERNAL",
  "EXTERNAL", "SETS", "DYNAMIC", "RANGE", "NAME", "DIGITS", "BGP_AIGP",
  "AIGP", "ORIGINATE", "COST", "ENFORCE", "FREE", "VALIDATE", "ROLE",
  "ROLES", "PEER", "PROVIDER", "CUSTOMER", "RS_SERVER", "RS_CLIENT",
  "REQUIRE", "BGP_OTC", "GLOBAL", "CEASE", "HIT", "ADMINISTRATIVE",
  "SHUTDOWN", "CONFIGURATION", "CHANGE", "DECONFIGURED", "CONNECTION",
  "REJECTED", "COLLISION", "OF", "MRT", "FILENAME", "ALWAYS", "OSPF", "V2",
  "V3", "OSPF_METRIC1", "OSPF_METRIC2", "OSPF_TAG", "OSPF_ROUTER_ID",
  "AREA", "RFC1583COMPAT", "STUB", "TICK", "COST2", "RETRANSMIT",
  "TRANSMIT", "DEAD", "BROADCAST", "BCAST", "NONBROADCAST", "NBMA",
  "POINTOPOINT", "PTP", "POINTOMULTIPOINT", "PTMP", "CRYPTOGRAPHIC",
  "ELIGIBLE", "POLL", "NETWORKS", "HIDDEN", "VIRTUAL", "ONLY", "LARGE",
  "NORMAL", "STUBNET", "TAG", "LSADB", "ECMP", "NSSA", "TRANSLATOR",
  "STABILITY", "LSID", "INSTANCE", "REAL", "NETMASK", "MERGE", "LSA",
  "SUPPRESSION", "RFC5838", "PE", "TOPOLOGY", "STATE", "PERF", "EXP",
  "REPEAT", "MODE", "PIPE", "RADV", "RA", "SOLICITED", "UNICAST",
  "MANAGED", "OTHER", "CONFIG", "LINGER", "MTU", "REACHABLE", "RETRANS",
  "TIMER", "CURRENT", "VALID", "PREFERRED", "MULT", "LIFETIME", "SKIP",
  "AUTONOMOUS", "RDNSS", "DNSSL", "NS", "DOMAIN", "TRIGGER", "SENSITIVE",
  "LOW", "MEDIUM", "HIGH", "PROPAGATE", "RA_PREFERENCE", "RA_LIFETIME",
  "RIP", "NG", "INFINITY", "TIMEOUT", "GARBAGE", "VERSION", "SPLIT",
  "HORIZON", "POISON", "REVERSE", "ZERO", "PLAINTEXT", "DEMAND", "CIRCUIT",
  "RIP_METRIC", "RIP_TAG", "REMOTE", "BIRD", "PRIVATE", "PUBLIC", "SSH",
  "TRANSPORT", "USER", "EXPIRE", "STATIC", "PROHIBIT", "BLACKHOLE",
  "UNREACHABLE", "KERNEL", "NETLINK", "KRT_PREFSRC", "KRT_REALM",
  "KRT_SCOPE", "KRT_MTU", "KRT_WINDOW", "KRT_RTT", "KRT_RTTVAR",
  "KRT_SSTRESH", "KRT_CWND", "KRT_ADVMSS", "KRT_REORDERING",
  "KRT_HOPLIMIT", "KRT_INITCWND", "KRT_RTO_MIN", "KRT_INITRWND",
  "KRT_QUICKACK", "KRT_LOCK_MTU", "KRT_LOCK_WINDOW", "KRT_LOCK_RTT",
  "KRT_LOCK_RTTVAR", "KRT_LOCK_SSTRESH", "KRT_LOCK_CWND",
  "KRT_LOCK_ADVMSS", "KRT_LOCK_REORDERING", "KRT_LOCK_HOPLIMIT",
  "KRT_LOCK_RTO_MIN", "KRT_FEATURE_ECN", "KRT_FEATURE_ALLFRAG", "SYSLOG",
  "TRACE", "INFO", "WARNING", "AUTH", "FATAL", "BUG", "STDERR", "SOFT",
  "CONFIRM", "UNDO", "LATENCY", "WATCHDOG", "CONFIGURE", "DOWN", "PERSIST",
  "SCAN", "LEARN", "DEVICE", "KRT_SOURCE", "KRT_METRIC", "';'", "'('",
  "')'", "','", "'{'", "'}'", "':'", "'['", "']'", "'?'", "$accept",
  "config", "conf_entries", "conf", "definition", "expr", "expr_us",
  "symbol", "bool", "ipa", "ipa_scope", "pxlen4", "net_ip4_", "net_ip6_",
  "net_ip6_sadr_", "net_vpn4_", "net_vpn6_", "net_roa4_", "net_roa6_",
  "net_mpls_", "net_ip_", "net_vpn_", "net_roa_", "net_", "net_ip4",
  "net_ip6", "net_ip", "net_any", "net_or_ipa", "label_stack_start",
  "label_stack", "time", "text", "opttext", "flow_num_op", "flow_logic_op",
  "flow_num_type_", "flow_num_type", "flow_flag_type", "flow_frag_type",
  "flow_srcdst", "flow_num_opts", "flow_num_opt_ext_expr",
  "flow_num_opt_ext", "flow_bmk_opts", "flow_neg", "flow_frag_val",
  "flow_frag_opts", "flow4_item", "flow6_item", "flow4_opts", "flow6_opts",
  "flow_builder_init", "flow_builder_set_ipv4", "flow_builder_set_ipv6",
  "net_flow4_", "net_flow6_", "net_flow_", "cli_cmd", "cmd_QUIT",
  "cmd_EXIT", "cmd_HELP", "filter_def", "$@1", "filter_eval",
  "custom_attr", "bt_test_suite", "bt_test_same", "type", "function_argsn",
  "function_args", "function_vars", "filter_body", "filter", "$@2",
  "where_filter", "function_body", "function_def", "$@3", "$@4", "cmds",
  "cmds_scoped", "$@5", "cmd_var", "cmd_prep", "cmds_int", "fipa",
  "set_atom", "switch_atom", "cnum", "pair_item", "ec_kind", "ec_item",
  "lc_item", "set_item", "switch_item", "set_items", "switch_items",
  "fprefix", "fprefix_set", "switch_body", "bgp_path_expr", "bgp_path",
  "bgp_path_tail", "constant", "constructor", "var_list", "function_call",
  "symbol_value", "static_attr", "term", "break_command", "print_list",
  "var_init", "var", "for_var", "cmd", "$@6", "$@7", "$@8",
  "get_cf_position", "lvalue", "rtrid", "idval", "hostname_override",
  "gr_opts", "net_type", "table", "table_start", "table_sorted",
  "table_opt", "table_opts", "table_opt_list", "proto_start", "proto_name",
  "proto_item", "channel_start", "channel_item_", "channel_item",
  "channel_opts", "channel_opt_list", "channel_end", "proto_channel",
  "rtable", "imexport", "limit_action", "limit_spec", "debug_default",
  "timeformat_which", "timeformat_spec", "timeformat_base",
  "iface_patt_node_init", "iface_patt_node_body", "iface_negate",
  "iface_patt_node", "iface_patt_list", "iface_patt_list_nopx",
  "iface_patt_init", "iface_patt", "tos", "proto", "dev_proto_start",
  "dev_proto", "dev_iface_init", "dev_iface_patt", "debug_mask",
  "debug_list", "debug_flag", "mrtdump_mask", "mrtdump_list",
  "mrtdump_flag", "password_list", "password_list_body", "password_items",
  "password_item", "pass_key", "password_item_begin",
  "password_item_params", "password_algorithm", "password_item_end",
  "bfd_item", "bfd_items", "bfd_opts", "cmd_SHOW_STATUS",
  "cmd_SHOW_MEMORY", "cmd_SHOW_PROTOCOLS", "cmd_SHOW_PROTOCOLS_ALL",
  "optproto", "cmd_SHOW_INTERFACES", "cmd_SHOW_INTERFACES_SUMMARY",
  "cmd_SHOW_ROUTE", "r_args", "r_args_for", "r_args_for_val",
  "export_mode", "channel_sym", "channel_arg", "cmd_SHOW_SYMBOLS",
  "sym_args", "cmd_DUMP_RESOURCES", "cmd_DUMP_SOCKETS", "cmd_DUMP_EVENTS",
  "cmd_DUMP_INTERFACES", "cmd_DUMP_NEIGHBORS", "cmd_DUMP_ATTRIBUTES",
  "cmd_DUMP_ROUTES", "cmd_DUMP_PROTOCOLS", "cmd_DUMP_FILTER_ALL",
  "cmd_EVAL", "cmd_ECHO", "echo_mask", "echo_size", "cmd_DISABLE",
  "cmd_ENABLE", "cmd_RESTART", "cmd_RELOAD", "cmd_RELOAD_IN",
  "cmd_RELOAD_OUT", "cmd_DEBUG", "debug_args", "cmd_MRTDUMP",
  "cmd_RESTRICT", "proto_patt", "proto_patt2", "dynamic_attr",
  "bfd_proto_start", "bfd_proto_item", "bfd_proto_opts", "bfd_proto",
  "bfd_accept_item", "bfd_accept", "bfd_iface_start", "bfd_iface_finish",
  "bfd_iface_item", "bfd_auth_type", "bfd_iface_opts",
  "bfd_iface_opt_list", "bfd_iface", "bfd_multihop", "bfd_neigh_iface",
  "bfd_neigh_local", "bfd_neigh_multihop", "bfd_neighbor",
  "cmd_SHOW_BFD_SESSIONS", "babel_proto_start", "babel_proto_item",
  "babel_proto_opts", "babel_proto", "babel_iface_start",
  "babel_iface_finish", "babel_iface_item", "babel_iface_opts",
  "babel_iface_opt_list", "babel_iface", "cmd_SHOW_BABEL_INTERFACES",
  "cmd_SHOW_BABEL_NEIGHBORS", "cmd_SHOW_BABEL_ENTRIES",
  "cmd_SHOW_BABEL_ROUTES", "bgp_proto_start", "bgp_loc_opts",
  "bgp_nbr_opts", "bgp_cease_mask", "bgp_cease_list", "bgp_cease_flag",
  "bgp_role_name", "bgp_proto", "$@9", "$@10", "bgp_afi",
  "bgp_channel_start", "bgp_nh", "bgp_lladdr", "bgp_channel_item",
  "bgp_channel_opts", "bgp_channel_opt_list", "bgp_channel_end",
  "bgp_proto_channel", "mrt_proto_start", "mrt_proto_item",
  "mrt_proto_opts", "mrt_proto", "cmd_MRT_DUMP", "mrt_dump_args",
  "ospf_variant", "ospf_proto_start", "ospf_proto", "ospf_af_mc",
  "ospf_channel_start", "ospf_channel", "ospf_proto_item",
  "ospf_area_start", "ospf_area", "ospf_area_opts", "ospf_area_item",
  "$@11", "$@12", "ospf_stubnet", "ospf_stubnet_start",
  "ospf_stubnet_opts", "ospf_stubnet_item", "ospf_vlink",
  "ospf_vlink_opts", "ospf_vlink_item", "ospf_vlink_start",
  "ospf_iface_item", "pref_list", "pref_item", "pref_base", "pref_opt",
  "nbma_list", "nbma_eligible", "nbma_item", "ospf_iface_start",
  "ospf_instance_id", "ospf_iface_patt_list", "$@13", "ospf_iface_opts",
  "ospf_iface_opt_list", "ospf_iface", "cmd_SHOW_OSPF",
  "cmd_SHOW_OSPF_NEIGHBORS", "cmd_SHOW_OSPF_INTERFACE",
  "cmd_SHOW_OSPF_TOPOLOGY", "cmd_SHOW_OSPF_TOPOLOGY_ALL",
  "cmd_SHOW_OSPF_STATE", "cmd_SHOW_OSPF_STATE_ALL", "cmd_SHOW_OSPF_LSADB",
  "lsadb_args", "perf_proto_start", "perf_proto", "perf_proto_item",
  "pipe_proto_start", "$@14", "pipe_proto", "radv_proto_start",
  "radv_proto_item", "$@15", "$@16", "radv_proto_opts", "radv_proto",
  "radv_iface_start", "radv_iface_item", "$@17", "$@18", "radv_preference",
  "radv_iface_finish", "radv_iface_opts", "radv_iface_opt_list",
  "radv_iface", "radv_prefix_start", "radv_prefix_item",
  "radv_prefix_finish", "radv_prefix_opts", "radv_prefix_opt_list",
  "radv_prefix", "radv_rdnss_node", "radv_rdnss_start", "radv_rdnss_item",
  "radv_rdnss_finish", "radv_rdnss_opts", "radv_rdnss", "radv_dnssl_node",
  "radv_dnssl_start", "radv_dnssl_item", "radv_dnssl_finish",
  "radv_dnssl_opts", "radv_dnssl", "radv_mult", "radv_sensitive",
  "rip_variant", "rip_proto_start", "rip_proto_item", "rip_proto_opts",
  "rip_proto", "rip_iface_start", "rip_iface_finish", "rip_iface_item",
  "rip_auth", "rip_iface_opts", "rip_iface_opt_list", "rip_iface",
  "cmd_SHOW_RIP_INTERFACES", "cmd_SHOW_RIP_NEIGHBORS", "rpki_proto_start",
  "rpki_proto", "rpki_proto_opts", "rpki_proto_item", "rpki_keep_interval",
  "rpki_proto_item_port", "rpki_cache_addr", "rpki_transport",
  "rpki_transport_tcp_init", "rpki_transport_ssh_init",
  "rpki_transport_ssh_opts", "rpki_transport_ssh_item",
  "rpki_transport_ssh_check", "static_proto_start", "static_proto",
  "stat_nexthop", "stat_nexthops", "stat_route0", "stat_route",
  "stat_route_item", "stat_route_opts", "stat_route_opt_list",
  "cmd_SHOW_STATIC", "kern_proto", "kern_sys_item", "log_begin",
  "log_config", "syslog_name", "log_limit", "log_file", "log_mask",
  "log_mask_list", "log_cat", "mrtdump_base", "debug_unix",
  "cmd_CONFIGURE", "cmd_CONFIGURE_SOFT", "cmd_CONFIGURE_CONFIRM",
  "cmd_CONFIGURE_UNDO", "cmd_CONFIGURE_STATUS", "cmd_CONFIGURE_CHECK",
  "cmd_DOWN", "cmd_GRACEFUL_RESTART", "cfg_name", "cfg_timeout",
  "kern_proto_start", "kern_mp_limit", "kern_item", "kif_proto_start",
  "kif_proto", "kif_item", "kif_iface_start", "kif_iface_item",
  "kif_iface_opts", "kif_iface_opt_list", "kif_iface", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,    61,    60,    62,   126,   280,
      43,    45,    42,    47,    37,    33,    46,   281,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   292,   293,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,   339,   340,   341,   342,   343,
     344,   345,   346,   347,   348,   349,   350,   351,   352,   353,
     354,   355,   356,   357,   358,   359,   360,   361,   362,   363,
     364,   365,   366,   367,   368,   369,   370,   371,   372,   373,
     374,   375,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   396,   397,   398,   399,   400,   401,   402,   403,
     404,   405,   406,   407,   408,   409,   410,   411,   412,   413,
     414,   415,   416,   417,   418,   419,   420,   421,   422,   423,
     424,   425,   426,   427,   428,   429,   430,   431,   432,   433,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,   445,   446,   447,   448,   449,   450,   451,   452,   453,
     454,   455,   456,   457,   458,   459,   460,   461,   462,   463,
     464,   465,   466,   467,   468,   469,   470,   471,   472,   473,
     474,   475,   476,   477,   478,   479,   480,   481,   482,   483,
     484,   485,   486,   487,   488,   489,   490,   491,   492,   493,
     494,   495,   496,   497,   498,   499,   500,   501,   502,   503,
     504,   505,   506,   507,   508,   509,   510,   511,   512,   513,
     514,   515,   516,   517,   518,   519,   520,   521,   522,   523,
     524,   525,   526,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,   540,   541,   542,   543,
     544,   545,   546,   547,   548,   549,   550,   551,   552,   553,
     554,   555,   556,   557,   558,   559,   560,   561,   562,   563,
     564,   565,   566,   567,   568,   569,   570,   571,   572,   573,
     574,   575,   576,   577,   578,   579,   580,   581,   582,   583,
     584,   585,   586,   587,   588,   589,   590,   591,   592,   593,
     594,   595,   596,   597,   598,   599,   600,   601,   602,   603,
     604,   605,   606,   607,   608,   609,   610,   611,   612,   613,
     614,   615,   616,   617,   618,   619,   620,   621,   622,   623,
     624,   625,   626,   627,   628,   629,   630,   631,   632,   633,
     634,   635,   636,   637,   638,   639,   640,   641,   642,   643,
     644,   645,   646,   647,   648,   649,   650,   651,   652,   653,
     654,   655,   656,   657,   658,   659,   660,   661,   662,   663,
     664,   665,   666,   667,   668,   669,   670,   671,   672,   673,
     674,   675,   676,   677,   678,   679,   680,   681,   682,   683,
     684,   685,   686,   687,   688,   689,   690,   691,   692,   693,
     694,   695,   696,   697,   698,   699,   700,   701,   702,   703,
     704,   705,   706,   707,   708,   709,   710,   711,   712,   713,
     714,   715,   716,   717,   718,   719,   720,   721,   722,   723,
     724,   725,   726,   727,   728,   729,   730,   731,   732,   733,
     734,   735,   736,   737,   738,   739,   740,   741,   742,   743,
     744,   745,   746,   747,   748,   749,   750,   751,   752,   753,
     754,   755,   756,   757,   758,   759,   760,   761,   762,   763,
     764,   765,   766,   767,   768,   769,   770,   771,   772,   773,
     774,   775,   776,   777,   778,   779,   780,   781,   782,   783,
     784,   785,   786,   787,   788,   789,   790,   791,   792,   793,
     794,   795,   796,   797,   798,   799,   800,   801,   802,   803,
     804,   805,   806,   807,   808,   809,   810,   811,   812,   813,
      59,    40,    41,    44,   123,   125,    58,    91,    93,    63
};
# endif

#define YYPACT_NINF (-2185)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-902)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     190,  3229,   117,    73,   240,   270,   357,  5142,   680,   817,
     817,  1035,   817,   383,   300,  2142,  1199,     4,   817,    98,
     445,   573, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185,  1078, -2185, -2185, -2185,  1078,  1078,  5142,  3068,   192,
     273,   457,   703, -2185, -2185,    78, -2185,   498, -2185, -2185,
   -2185, -2185,   519,   931, -2185,   624,  -157, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,   730,
   -2185,   659,  3451, -2185, -2185, -2185,  1031,  3264,  1031, -2185,
    1031, -2185,  1031,  6683,  1031, -2185,  1031,  2690,  1031,  3130,
     359,  1500,  1031, -2185,  1031, -2185,  1031, -2185,  1031,  2946,
    2911, -2185, -2185, -2185,  1031,  1031,  1811, -2185, -2185, -2185,
      68, -2185, -2185,   918,   930,   786,   436, -2185,   915,   184,
    5142,  1011, -2185,   506,   514, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,   526,
     547,   554,   561,   567,   575,   579, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
    3228,    89,  1015,  1064, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,  3999, -2185,  1201, -2185, -2185,   135,
    1252,   135, -2185,  1236,  1236,   817,   817,  1258,   168, -2185,
    1266,   818,   239,  1284, -2185,  1296, -2185,  1013,   770,   855,
     613,  1315,  1155,  1350,  1366,  1369,  1372,  1405,  1410,  1424,
    1428, -2185, -2185,   849,  1333,  1236, -2185, -2185,  1413,  1439,
    1413,  1446,  1455,   972, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185,  1451, -2185, -2185,  3918, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185,  1527,  1465,  1470,   147, -2185, -2185,   923,   135,
     135,   379,   370, -2185,   168,   939, -2185, -2185, -2185, -2185,
     268,   958,   205,  1299,   379,   379,  1078, -2185,   924, -2185,
   -2185, -2185, -2185, -2185, -2185,   485, -2185, -2185, -2185,  1051,
   -2185, -2185, -2185, -2185, -2185,  1535,  1516,  1012,  1462,   858,
     135, -2185,   622,   703,   168,  1346, -2185, -2185,  1050,  1032,
    1071,  1087,  1069,  1085,  1088, -2185, -2185,   768,  1339,   242,
    1099,  1602,   669,    82,   182,   188,   210,  1358,   703,   858,
    1318,  1499,  1469,   800,   444,   158,   364,   537,  1412,  1472,
     333,  1474,  1416,  1442,  1420,   709,  1408,  1584,   858,  1414,
    1409,  1476,  1479,  1434,  1464,   468,   858,   196,  1445,  1653,
    1531,  1436, -2185,  1250, -2185,  1247,  1262,  1259,  1261,  1370,
    1654,   203,   858,  1686,   379,   858,  1692,  1481,   858, -2185,
    1489, -2185,  1032, -2185,  1272,  1277, -2185,  1278,   858,  1106,
    1838,   113,  1840,  1107, -2185,  1286, -2185,  1287, -2185,   379,
    1770,  1345,  1849,  1676,  1646,  1702, -2185,  1303,  1304,  1301,
    1302,  1307,  1308,  1360,  1628,  1708, -2185,  1314,  1317,   671,
    1311,  1703,   379,  1544,  1720,  1658,   858,  1651,   858, -2185,
   -2185, -2185,  1335,  1336,  1344,  1347, -2185,  1678, -2185, -2185,
    1359,    54, -2185,    68,    68,  5142,   221,    68,    68,  1919,
   -2185,  1920, -2185,  1921,   918,  1915, -2185, -2185,  5142,  1923,
     632,  1918,  1924, -2185, -2185, -2185,  1849,  5142,  5142,  5142,
    5142,  5142,  5142, -2185, -2185,  1936, -2185,  1382,   160, -2185,
   -2185,  1915, -2185, -2185,  2685, -2185, -2185, -2185,   114, -2185,
    1954, -2185, -2185, -2185, -2185,   121, -2185,   283,  1953,  1955,
   -2185,  5142,  5142,  5142,  5142,  5142,  5142,  5142,  5142,  5142,
    5142,  5142,  5142,  5142,  5142,  2823,  3272, -2185, -2185,  1586,
   -2185, -2185, -2185, -2185,  1966,  1970,  1975,  1977, -2185, -2185,
   -2185,   486,  1978, -2185, -2185, -2185,   759,  1984, -2185,  1985,
   -2185,  4329, -2185,  2670,  1315,  1315,  1315,  1315,  1315, -2185,
    1315,  1315, -2185,   282,   367,  1987,  1315,  1315,  1988,  1989,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185,   449, -2185, -2185,
    1994,  2005,   689,  2006, -2185,   972, -2185, -2185,   379,  2010,
    5142,  1443, -2185, -2185,  1453,  1447,  1452, -2185, -2185, -2185,
   -2185,  5142,  1454, -2185, -2185, -2185, -2185, -2185,  5142, -2185,
   -2185, -2185, -2185, -2185,   379, -2185, -2185,  1456, -2185,   379,
    1046, -2185,  1655, -2185,   379,   -95,   379,  1248, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185,  1031, -2185, -2185,  2013, -2185,
     203, -2185, -2185, -2185, -2185, -2185, -2185, -2185,   858, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185,  1782,  1783,  1143,
    1785,  1786, -2185,  1471,   379,   379, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,   186,  1487,  1496,    95,   745,   858,
    1698,  1817,   858,  1497,  1502,  1485, -2185,  1504, -2185, -2185,
   -2185,  1178,  2035,   455,  1004,  2035,    -9,   858,   379,  1820,
    1823,   379,  1825,   858,  1829,   858,  1927,   858,   858,  1517,
     858,   858,   858,   858,   858,   858,  1701,  1844,   858,   858,
    1519,   858,  1520,   441,  1854,   858,   858, -2185, -2185, -2185,
   -2185, -2185, -2185,   858,   756, -2185, -2185,   858, -2185,  1909,
     379,   858, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
     379,   379, -2185,  2088,  2089, -2185, -2185, -2185, -2185, -2185,
   -2185,  2085,  5142, -2185, -2185,   289,  1917, -2185, -2185,   289,
   -2185, -2185, -2185,   289,   858,  1849, -2185, -2185, -2185, -2185,
   -2185, -2185,   930,   786, -2185, -2185, -2185,   858,  1849, -2185,
   -2185, -2185,  1340, -2185,  1143, -2185, -2185, -2185,   602,  1799,
   -2185,  1542,   858, -2185,   858,   379,  1816, -2185,   379, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185,   379, -2185,  2099, -2185,
   -2185, -2185,   172, -2185, -2185,    68,   531, -2185, -2185, -2185,
   -2185,  2076,  2112,   981,  3918, -2185,  1983,  2103, -2185, -2185,
   -2185,  1002,   255,   347,   421,   466,   495,   928, -2185,  5142,
   -2185,  5142,  1562,    42,  1566,  1145, -2185, -2185,  2140,   298,
     348, -2185,   786, -2185,  1916,  1922,  2212,  2212,  2212,  3596,
    3596,  2212,  2212,  2212,  2212,  2212,  1095,  1095,  1924,  1924,
   -2185, -2185, -2185, -2185, -2185,  1585, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185,   541, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185,   619, -2185, -2185,  2156, -2185, -2185, -2185,
    1202,  1360,  2085,  2147, -2185,  1998,  1999,   564, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185,  2150, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185,  2169,  1236,  1236,  1236,  1236,
    1236,  1236,  1681,  1315,  1236,  1315,  1236, -2185,  1236,  1236,
   -2185, -2185,   849, -2185, -2185, -2185, -2185,  2085,  1245,   703,
   -2185, -2185,  2173, -2185, -2185,  2267,  1605, -2185,  2486, -2185,
   -2185, -2185,   703,  2165, -2185,  1618,  1174, -2185,  1203, -2185,
   -2185,  2167,  1297,  1541,   703, -2185,   703, -2185, -2185,   849,
    1621,  1624, -2185, -2185, -2185,   218, -2185, -2185, -2185,  2171,
   -2185,  1627,  1637,   811, -2185,  1731,  3561,   379,   379,  1639,
     379,   379, -2185,  1641,  1644,   858,   496, -2185, -2185,  2029,
    1964,   379,  1649,  1656,  1664,   858,   858,  1665, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185,  1078, -2185,   379,   379,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,  1679, -2185,
     379,   379, -2185,  1682,  1687,   379,   379,  1689,   379,  1693,
     379,  1694,   203,  1697,  1699, -2185,  1700,  1710,  1711,  1712,
    1713,  1715,   858,   332,  1716,  1717, -2185,  1718, -2185,   379,
    1721,   858,  1732,  1734,  3988, -2185,  1733, -2185,   379, -2185,
   -2185, -2185,   379, -2185, -2185, -2185,  2489, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,  3918, -2185,  2023, -2185,   858, -2185,
   -2185, -2185,  1735,  2445,  3233,  2601,  1737,  1739, -2185,  2035,
    2176,  2231,   379,   858,   858,   602,  5423, -2185, -2185,  2065,
   -2185,   379, -2185,  1627,  1688, -2185,    68, -2185, -2185,    68,
    2233, -2185, -2185,  5142,  2222,  2225, -2185, -2185, -2185,  5142,
   -2185,  5142,  5142,  5142,  5142, -2185,   643,   313,  3547,  5142,
    3866,  5142, -2185,  1695,  5142, -2185, -2185, -2185,  2246,  2256,
    5142,  1586, -2185,   486, -2185, -2185,   918,   938,  1573, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
    2252,  2252, -2185, -2185,  1201, -2185, -2185,  2270,  2287,  2298,
    2307,  2308,  2311, -2185, -2185,  2300,   203, -2185, -2185, -2185,
     203,   203,  1236,  2314,  1236,  2316,  2317,  2318, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185,  3068, -2185,  5705,  1527,
    1751,  1752,  1627, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185,   749, -2185, -2185,  2072,  2074,   858,   858,
    1353, -2185,  1759, -2185,  2309, -2185, -2185,   135, -2185, -2185,
    1760, -2185, -2185, -2185,  1143,  2049, -2185, -2185,  1779, -2185,
    2183, -2185, -2185, -2185,  1780,  1778,  1784, -2185,  1789,  1790,
   -2185, -2185,  1805,  1168, -2185,  1807,   553,   379,  1812, -2185,
   -2185, -2185,  1813,  1815, -2185,  1401,  1819,   723, -2185,   728,
   -2185, -2185, -2185,    66, -2185, -2185, -2185, -2185,  1821,  1826,
   -2185,  1830, -2185,  1831, -2185,  1834, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,  1836, -2185,  1839, -2185, -2185, -2185,
    1845, -2185,  1846, -2185, -2185,  2053,  1997,  2016,  2046,  2025,
    2223,  2209,  2345,  2066,  2238,   132,   858,   858,   700,   379,
     858, -2185, -2185,  1847,  2085,  1399,   379,   703,  2280, -2185,
   -2185, -2185,  1848, -2185, -2185, -2185,   724,   858, -2185,   875,
   -2185,  2170,  1178, -2185,  1034, -2185,  1855, -2185,  1981, -2185,
    1268, -2185, -2185, -2185,  1406, -2185, -2185, -2185,  1406,  2255,
   -2185, -2185, -2185,  1861, -2185,   858,   379, -2185, -2185, -2185,
    1863,   379,  2248,  2290,  2248,  1636,     3,  2248, -2185, -2185,
   -2185,  1867, -2185, -2185, -2185, -2185,  2231, -2185, -2185,  2411,
   -2185, -2185, -2185,    69,  5142,  5142,  1877,  5142, -2185, -2185,
   -2185,  5142,  5142, -2185,  1878,  1879, -2185, -2185,  1881,  2427,
    5142, -2185,   980, -2185,   379, -2185, -2185, -2185, -2185, -2185,
   -2185,  2421,  3918,  2429, -2185,  1387,  1691,   822,  1215,  1493,
    1505,  1571,  5142, -2185,  5142,  1054,    38,  3918,  1888,  1073,
      46,  1889,  2448,  1145, -2185, -2185,  1603, -2185, -2185,  2446,
     918,  1915,  1201, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185,  2462, -2185,  2463, -2185, -2185,
   -2185,  1527, -2185,  1527,  1894, -2185, -2185,  5705, -2185, -2185,
    1900, -2185,  2466, -2185,  2232,  2240, -2185, -2185,   379,   379,
   -2185,  1364,  1450, -2185, -2185, -2185, -2185,   884, -2185, -2185,
    1910, -2185,   262, -2185,   858, -2185, -2185, -2185,  2340, -2185,
     379, -2185, -2185, -2185, -2185,  2315,  2097,  2116, -2185,    13,
    2118,  1251,   861, -2185, -2185,  1960,  1962,  1963, -2185, -2185,
   -2185,  1427,   858, -2185,  2289,   379,   379, -2185,  1965, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185,   881,   667,   858,   858,   249,  1849,   858,  2226,
     721,  1849, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185,  2224, -2185, -2185, -2185,
     379,   379,   858, -2185,  1967,   379, -2185,  1968,   203, -2185,
   -2185,  1971,   379, -2185, -2185,   379,  1976, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,  1979, -2185, -2185, -2185,   110,    55,
   -2185,   858, -2185, -2185, -2185,  2357, -2185, -2185, -2185, -2185,
     379,  2477,   379, -2185, -2185, -2185,  2499, -2185, -2185, -2185,
     379, -2185,  2411,  2533,  5142,   896,  1982,  1986,  6779,  2336,
    3802,   127,  4044, -2185, -2185,  1980,  5705, -2185,  5142,  1990,
    5142,  1349, -2185,  -265,  2534,  2526, -2185, -2185, -2185, -2185,
    2503, -2185, -2185,  1623,  2497, -2185, -2185, -2185, -2185, -2185,
   -2185,   106,  2529,  2529,  1036,  1991, -2185,   106,  2529,  2529,
    1406,  1995,  5142, -2185, -2185, -2185, -2185,  1683,  1768, -2185,
    2536,  5142, -2185,  4185, -2185,  2546,  5142,  2578,  4504,  4823,
    2014, -2185, -2185, -2185, -2185,   927,  2565, -2185, -2185, -2185,
    2019,   379,   379, -2185, -2185,   918,  1915, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185,  1910, -2185, -2185,  1078,
     703,  2312, -2185,  2022,   858,  2027,  2206,  2326, -2185, -2185,
   -2185, -2185, -2185, -2185,  1168, -2185, -2185, -2185, -2185,  2355,
    2356, -2185,  2358, -2185, -2185, -2185,   826,   826,  1143,  2213,
   -2185, -2185, -2185, -2185, -2185,  2422,  2354, -2185, -2185,   858,
   -2185, -2185, -2185, -2185, -2185,   858,  1627,  2036, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185,  2038, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185,  2040,   379, -2185,   858, -2185, -2185, -2185,  2041, -2185,
   -2185,  2447,  5142, -2185, -2185,  2044, -2185,  5986, -2185, -2185,
    1527,  2514,  5142,  6249, -2185, -2185,  2479, -2185,  2491,  2063,
    2064,  2067,  2071,  2073,  1143, -2185,  2075, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185,  2629,   379,  1677, -2185,  2069, -2185,    85,   379,  1503,
     107, -2185, -2185, -2185, -2185,  1677,    85,   107,  2566, -2185,
    1798, -2185, -2185,  2077,  2080,  1124,    48,  2082,  1126,  2092,
    2084,    50,  2087,    52, -2185, -2185, -2185,  5142,  2091, -2185,
   -2185, -2185, -2185,  1127, -2185, -2185, -2185,  1143,  2367, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185,   379,   379,   379, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185,   858,   379, -2185, -2185,
    1981, -2185, -2185,    20,   129,   155,  2371,   -37, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
    2095,  2559,    75, -2185, -2185,  3918, -2185, -2185, -2185, -2185,
   -2185, -2185,  5142,  5142,  5142, -2185,  5142, -2185, -2185,   379,
   -2185, -2185,   379,  2054,   379, -2185,  2529,  2529,  2633, -2185,
   -2185, -2185, -2185, -2185,  2529,  2529,  2652, -2185, -2185, -2185,
   -2185,  2637,  5142, -2185, -2185, -2185,  2638,  5142, -2185,  5142,
    2665, -2185,  5142, -2185,  3918, -2185, -2185,  1459, -2185, -2185,
     858,  2101,  2431,   379,   379,   862, -2185, -2185, -2185, -2185,
    1341,  2106,  2107, -2185,   858, -2185,  2136, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,  6650, -2185, -2185, -2185,   -26, -2185,
     858,   379,   858, -2185,  2111,   379,   202,   379,   379,  2368,
     316, -2185, -2185,  2114,   858,  2211,  2218,   858,   858, -2185,
    2115,  2302, -2185,   605,   392,  2395, -2185,   604, -2185,  5986,
   -2185, -2185, -2185,  2685, -2185, -2185,  2675, -2185, -2185, -2185,
   -2185,   534,  5142,  2123,  2124,  1835,  1914,  1974,  2020, -2185,
    2069,   379, -2185,   379,   379,   379,  1503,  1503, -2185,  2127,
    2134,  2137,  2139,  2141,  2117,  2146,  2450,  2468, -2185,  1540,
    2473, -2185, -2185, -2185,  1534, -2185,  2520, -2185, -2185, -2185,
    1530, -2185, -2185, -2185,   379,  2425,  1478,   379,  2430,   214,
    2480,   152,   379,  2482,  2484,  2677, -2185, -2185,  2172,  2205,
     379,  2444,   744,  2494,  2295,  2174,   858,   352,  2325,   379,
     379,  2397,   379,   858,   379,  2435,   450,   111,   379,  2631,
    2332, -2185, -2185,  2184, -2185,   379,  2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185,   379,   379, -2185,
   -2185, -2185,   379,   379, -2185, -2185, -2185,   123,   -23,  2292,
     -20,    83,  2297,  2296,   858,  2294,  2505,  2299,  2464,  2485,
    2490, -2185,  2214,   493,  1143, -2185,  2215,   493,  2761, -2185,
    2216,   379,   858,  2502,   920,  -150,   858,    43,   379,  2557,
    1143,  2459,  2560,  2667,   330,  2562,  2564,   377,  2324,  2330,
    2321, -2185, -2185,  2262,  2323,  2327,   703, -2185,  2264, -2185,
   -2185,  2090,   238,   244, -2185,  3918,  2268,  5142,  2271,  2272,
    2273,  2274, -2185,  2793,  2813, -2185, -2185, -2185, -2185, -2185,
   -2185,  5142, -2185,  5142, -2185,   379,   379,  2277,  2278,   379,
   -2185, -2185,  1580,  2825,   386,   379,  2825,  1102,   462,  2279,
   -2185,  1616, -2185, -2185, -2185,   379,   379,   379,   379,   379,
   -2185,   858, -2185,  2545, -2185,   379,   379,  2556, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,   254,   379,
     379, -2185,   858,  2406, -2185, -2185, -2185, -2185, -2185,   858,
   -2185, -2185,  1244, -2185, -2185, -2185,   379,   379, -2185,   858,
     858, -2185,   379,   858, -2185, -2185, -2185, -2185, -2185,  2375,
    2375,  2607, -2185,   379,  2615,  2617,   820,   379,   820,  2618,
     379,   379,  2407, -2185,   858,   379,   379,  2691,   858,   110,
     858,    55, -2185,   379, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185,   379,   379,   379, -2185,   858,
     858, -2185, -2185, -2185, -2185, -2185, -2185, -2185,   379, -2185,
    1327,   379,   379, -2185, -2185,   379,   379,   858, -2185,   858,
     858,   858, -2185,  2680,  2684, -2185, -2185, -2185, -2185,  5142,
   -2185, -2185, -2185,  2773, -2185,  2121, -2185, -2185, -2185, -2185,
     379,   379,  2313,  2306, -2185, -2185, -2185,  1540, -2185, -2185,
   -2185, -2185,  2329,  2825,  2825,  2331,  2334,  1275,  1559, -2185,
   -2185, -2185, -2185,  2335,  2825,  2825, -2185,  1143,  1143, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,   858, -2185,
   -2185, -2185, -2185, -2185, -2185,   858,   159, -2185,  2449, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185,   858, -2185, -2185,   379,
   -2185,   379,   379, -2185, -2185, -2185, -2185,  2375, -2185,   379,
    2375, -2185,   858, -2185, -2185, -2185,   379, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,  2452, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,   703,   703,
    2090,  5986,  2337, -2185, -2185, -2185,  5142, -2185,  1530,  2341,
    2342,  1530,  1530, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185,  1530,  2344,  2347, -2185, -2185, -2185, -2185,
   -2185, -2185,  2458, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,  2320,
   -2185,  1530,  1530, -2185, -2185, -2185,  1530,  1530, -2185,  2348,
    5142, -2185, -2185, -2185, -2185, -2185,  2328, -2185
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       4,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    1459,     0,     3,   135,   137,   139,   574,   576,   578,   580,
     584,   586,   588,   645,   654,   656,   658,   660,   662,   664,
     666,   668,   670,   672,   674,   681,   683,   685,   687,   689,
     691,   693,   697,   699,   755,   794,   796,   798,   800,   984,
    1127,  1129,  1131,  1133,  1135,  1137,  1139,  1141,  1313,  1315,
    1378,  1443,  1445,  1447,  1449,  1451,  1453,  1455,  1457,     1,
       2,     0,   410,   408,   409,     0,     0,     0,     0,     0,
       0,     0,     0,   427,   428,     0,   401,   402,   404,   405,
     406,   407,     0,     0,  1413,     0,     0,     6,     5,     7,
     141,   144,   146,   148,   150,   180,   390,   397,   399,     0,
     411,   414,     0,   471,   475,   426,   429,   433,   429,   708,
     429,   757,   429,   433,   429,   971,   429,   433,   429,   433,
       0,   433,   429,  1174,   429,  1260,   429,  1317,   429,   433,
     433,  1412,  1435,  1438,   429,   429,   433,   136,   138,   140,
     265,   266,   276,   193,   194,   271,   285,   269,     0,     0,
       0,     0,   286,     0,     0,   289,   267,   268,   287,   288,
     290,   291,   292,   293,   294,   295,   297,   298,   296,     0,
       0,     0,     0,     0,     0,     0,   707,   793,   958,   961,
     960,   957,   959,   962,   963,   964,   967,   969,   965,   966,
     968,   970,  1123,  1124,  1125,  1126,  1258,  1259,  1311,  1312,
    1384,  1385,  1386,  1387,  1388,  1389,  1390,  1391,  1392,  1393,
    1394,  1395,  1396,  1397,  1398,  1399,  1400,  1401,  1402,  1403,
    1404,  1405,  1406,  1407,  1408,  1409,  1410,  1411,  1491,  1492,
       0,     0,    37,    38,    47,    39,    40,    41,    42,    48,
      43,    44,    45,   272,   133,   134,    46,   270,   280,   317,
     318,   350,   316,   319,     0,   320,   701,   703,   702,     0,
       0,     0,   701,    69,    69,     0,     0,     0,     0,   700,
       0,   705,     0,     0,   590,     0,   647,     0,     0,   583,
       0,   583,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   677,   676,     0,   679,    69,   986,  1460,  1459,     0,
    1459,     0,     0,  1461,  1456,    16,    15,   802,   803,   804,
     805,   806,   807,     0,   181,   142,   145,   152,   153,   154,
     156,   155,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,     0,     0,     0,     0,    67,    66,     0,     0,
       0,     0,    22,   403,     0,     0,   477,   476,   478,   479,
       0,     0,     0,     0,     0,     0,     0,   415,   424,   503,
    1318,   809,   709,   758,   972,   993,  1153,  1168,  1175,  1261,
    1348,  1465,  1478,   996,  1263,     0,   430,     0,     0,    22,
       0,   509,     0,     0,     0,     0,   502,   441,     0,   455,
       0,     0,     0,     0,     0,   917,   918,     0,     0,     0,
       0,     0,     0,   907,   908,     0,     0,     0,     0,    22,
       0,     0,     0,     0,    22,     0,   813,   810,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    22,     0,
       0,     0,     0,     0,     0,     0,    22,     0,     0,     0,
       0,     0,   808,     0,   919,   953,     0,     0,     0,     0,
       0,     0,    22,     0,     0,    22,     0,     0,    22,   992,
     999,  1003,   455,  1004,     0,     0,  1017,     0,    22,     0,
       0,     0,     0,     0,  1152,     0,  1157,     0,  1170,     0,
       0,     0,     0,     0,     0,     0,  1167,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1347,     0,     0,  1367,
    1376,     0,     0,     0,     0,     0,    22,     0,    22,  1464,
    1470,  1471,     0,     0,     0,     0,  1484,     0,  1477,  1481,
       0,   265,   285,   265,   265,     0,     0,   265,   265,     0,
     254,     0,    29,     0,     0,     0,    32,    33,   281,     0,
       0,     0,   314,    36,   128,   128,     0,     0,     0,     0,
       0,     0,     0,   216,   217,     0,   219,     0,     0,   195,
     198,   194,   197,   200,     0,   273,    37,    38,   245,   196,
     234,   231,   232,   233,   241,     0,   249,     0,     0,     0,
     673,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   512,   511,     0,
     696,   694,   695,    68,     0,     0,     0,     0,   688,   523,
     522,     0,     0,  1458,   704,   706,   705,     0,   585,     0,
     577,     0,   575,     0,   583,   583,   583,   583,   583,   582,
     583,   583,  1143,   583,   583,     0,   583,   583,     0,     0,
     667,   659,   669,   661,   655,   657,   663,   665,  1431,  1426,
    1429,  1427,  1428,  1430,  1432,  1433,  1434,     0,  1424,   680,
       0,     0,     0,     0,  1452,  1461,  1448,  1450,  1462,     0,
       0,     0,   172,   167,     0,     0,     0,   393,   395,   396,
     498,     0,     0,   398,   472,   473,     9,    11,     0,   474,
      18,    20,    19,    21,     0,    17,  1439,     0,  1437,   480,
       0,   488,  1416,  1421,  1417,     0,     0,     0,  1442,  1441,
     413,   422,   412,   994,   995,   429,  1262,   431,     0,   504,
       0,   434,   435,   489,   440,   439,   438,   436,    22,   505,
     453,   457,   506,   507,   716,   763,   836,     0,     0,     0,
       0,     0,   853,     0,     0,     0,   911,   909,   912,   910,
     913,   915,   914,   916,     0,     0,     0,     0,    22,    22,
       0,     0,    22,     0,     0,     0,   854,     0,    23,    24,
      25,     0,    26,     0,     0,    26,     0,    22,     0,     0,
       0,     0,     0,    22,     0,    22,     0,    22,    22,     0,
      22,    22,    22,    22,    22,    22,     0,     0,    22,    22,
       0,    22,     0,     0,     0,    22,    22,   837,   951,   955,
     838,   981,   997,    22,    22,  1018,  1005,    22,  1015,  1012,
       0,    22,  1006,  1000,  1001,   457,   998,  1020,  1154,  1164,
       0,     0,  1163,     0,     0,  1160,  1165,  1166,  1156,  1155,
     448,   176,     0,   462,   463,     0,     0,   461,   443,     0,
     444,   459,   442,     0,    22,     0,  1171,  1172,  1186,  1270,
    1320,  1349,     0,     0,    56,    55,  1363,    22,     0,  1350,
    1351,  1369,     0,  1368,     0,  1372,  1370,  1371,  1361,  1364,
    1374,     0,    22,  1382,    22,     0,     0,  1472,     0,  1474,
    1380,  1467,  1466,  1479,   489,  1482,     0,  1480,     0,   257,
     263,   261,     0,   193,   194,   265,     0,   262,   264,   256,
      28,    30,     0,     0,   282,   339,     0,     0,   340,   129,
     130,     0,     0,     0,     0,     0,     0,     0,   218,     0,
     299,     0,     0,     0,     0,   205,   246,   247,     0,     0,
       0,   274,     0,   275,     0,     0,   311,   309,   307,   304,
     305,   306,   308,   310,   312,   313,   300,   301,   302,   303,
     329,   328,   322,   323,   324,     0,   327,   321,   325,   326,
     334,   335,   336,   331,   332,   333,   337,   338,   641,   639,
     640,   624,   627,   631,   634,   637,   638,   625,   626,   628,
     629,   630,   632,   633,   635,   636,   642,   643,   644,   516,
     517,   518,   520,   521,   519,     0,   514,   686,   682,   690,
     692,   527,   528,     0,   525,   698,     0,   579,   587,   589,
       0,     0,   176,     0,   600,     0,   621,     0,   602,   601,
     606,   607,   620,   622,   623,   591,   599,     0,   646,   649,
     650,   651,   652,   648,   653,     0,    69,    69,    69,    69,
      69,    69,     0,   583,    69,   583,    69,  1128,    69,    69,
    1379,   671,     0,   678,   675,   684,   985,   176,     0,     0,
     990,  1454,     0,  1463,  1444,     0,   168,   182,     0,   143,
     174,   147,     0,     0,   489,     0,     0,   391,     0,  1440,
    1436,     0,   482,   485,     0,  1420,     0,  1419,  1422,     0,
       0,     0,    12,    13,    14,     0,  1169,   432,   437,   492,
     495,   510,     0,     0,   458,   433,   433,     0,     0,     0,
       0,     0,   844,     0,     0,    22,    22,   882,   884,     0,
       0,     0,     0,     0,     0,    22,    22,     0,   900,   899,
     571,   902,   855,    54,    53,   813,     0,   813,     0,     0,
     816,   817,   841,   831,   832,   833,   834,   835,     0,   810,
       0,     0,   839,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   881,     0,     0,     0,     0,
       0,     0,    22,     0,     0,     0,   883,     0,   848,     0,
       0,    22,     0,     0,     0,   956,   433,  1007,     0,  1010,
    1009,  1008,     0,  1016,  1014,  1002,     0,  1161,  1162,  1158,
    1159,   175,   460,   172,   178,   470,   464,   446,    22,   447,
     445,   450,     0,   433,   433,   433,     0,     0,  1356,    26,
    1365,     0,     0,    22,    22,  1362,     0,  1354,  1475,  1468,
    1381,     0,  1473,   497,  1488,  1483,   265,   255,   259,   265,
       0,    30,   284,     0,     0,     0,   124,   126,   347,     0,
     315,     0,     0,     0,     0,   349,     0,     0,     0,     0,
       0,     0,   199,     0,     0,   235,   242,   250,     0,     0,
       0,     0,   513,     0,   524,   581,   618,   619,     0,   615,
     616,   617,   611,   609,   610,   592,   608,   593,   598,   605,
       0,     0,   594,   595,   603,   604,   756,     0,     0,     0,
       0,     0,     0,  1142,  1151,     0,     0,  1146,  1149,  1144,
       0,     0,    69,     0,    69,     0,     0,     0,  1425,   989,
     988,   987,   991,  1446,     8,   170,     0,   172,   184,     0,
       0,     0,   499,   392,   394,    10,   481,   483,   484,   486,
     487,  1415,  1418,     0,  1414,   400,     0,     0,    22,    22,
       0,   425,     0,   493,    69,   489,   508,     0,   456,   451,
       0,   723,   725,   725,     0,     0,   718,   710,     0,   766,
       0,   765,   759,   760,     0,     0,     0,   867,     0,     0,
     866,   865,     0,     0,   818,     0,    22,     0,     0,   892,
     891,   898,     0,     0,   878,     0,     0,     0,    27,     0,
     814,   815,   905,     0,   811,   812,   858,   850,     0,     0,
     871,     0,   860,     0,   846,     0,   847,   880,   863,   885,
     862,   861,   897,   864,     0,   886,     0,   888,   889,   849,
       0,   856,     0,   859,   906,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,    22,    22,     0,
      22,   954,   926,     0,   176,     0,     0,     0,     0,   983,
     977,   973,     0,  1011,  1013,  1113,     0,    22,  1033,    22,
    1031,     0,     0,  1023,    22,  1019,     0,  1037,  1114,   177,
       0,   469,   449,  1173,     0,  1189,  1180,  1182,     0,     0,
    1188,  1176,  1177,     0,  1273,    22,     0,  1272,  1264,  1265,
       0,     0,  1332,     0,  1332,     0,     0,  1332,  1319,  1322,
    1323,     0,  1326,  1352,  1353,  1355,     0,    62,    63,  1357,
    1359,  1358,  1360,     0,   354,   354,     0,     0,   351,   352,
     353,     0,     0,   365,     0,     0,   186,  1377,     0,     0,
     354,  1373,     0,  1375,     0,  1476,  1383,  1486,  1490,   258,
     260,     0,   283,     0,   341,     0,     0,     0,     0,     0,
       0,     0,     0,   277,     0,     0,     0,   205,     0,     0,
       0,     0,     0,     0,    34,    35,     0,   515,   526,     0,
     612,   613,     0,   596,   597,   801,   795,   797,   799,  1132,
    1130,  1147,  1150,  1145,  1148,     0,  1134,     0,  1138,  1314,
    1316,     0,   183,     0,     0,   190,   191,   185,   188,   189,
       0,   149,     0,  1423,     0,     0,   416,   417,     0,     0,
     423,   490,     0,   494,   496,   452,   454,   711,   489,   712,
     743,   713,   747,   714,    22,   717,   489,   761,     0,   764,
       0,   873,   851,   872,   875,     0,     0,     0,   822,     0,
       0,     0,     0,   820,   876,     0,     0,     0,   893,   877,
     879,     0,    22,   570,     0,     0,     0,   573,     0,   903,
     843,   842,   840,   870,   869,   852,   868,   845,   890,   887,
     857,   904,     0,    22,    22,    22,     0,     0,    22,     0,
       0,     0,   933,   934,   935,   931,   947,   946,   948,   936,
     952,   976,   975,   974,   979,   978,     0,   982,   489,  1036,
       0,     0,    22,  1024,     0,     0,  1022,     0,     0,  1040,
    1035,  1039,     0,  1029,  1021,     0,  1047,   465,   466,   467,
     468,    52,    51,  1222,  1231,  1179,   489,  1178,     0,     0,
    1184,    22,  1187,   489,  1269,  1266,  1268,  1271,  1334,  1333,
       0,     0,     0,    25,  1336,  1335,  1324,  1339,  1340,  1327,
       0,  1321,  1366,     0,     0,   355,     0,     0,     0,     0,
       0,     0,     0,   386,   386,     0,   184,   377,     0,     0,
       0,     0,  1469,     0,     0,     0,    82,    93,    94,    80,
       0,    83,    84,     0,     0,    87,    88,    92,    89,   131,
      90,     0,   106,   106,     0,     0,   132,     0,   106,   106,
       0,     0,     0,   344,   345,   343,   346,     0,     0,   214,
       0,     0,   212,     0,   208,     0,     0,   206,     0,     0,
       0,   330,   614,  1136,  1140,     0,   357,   179,   192,   173,
       0,     0,     0,   420,   421,    59,    60,    61,    57,    58,
     491,   721,   719,   720,   722,   724,   743,   741,   726,     0,
       0,   750,   715,   790,    22,     0,     0,     0,   825,   826,
     824,   828,   827,   829,     0,   819,   895,   894,   896,     0,
       0,   569,     0,   564,   568,   572,    22,    22,     0,     0,
     941,   942,   943,   944,   945,     0,     0,   950,   937,    22,
     923,   924,   925,   932,   949,    22,  1116,  1120,  1026,  1027,
    1025,  1101,  1028,  1101,  1061,  1041,  1030,  1115,  1048,  1229,
    1228,  1219,  1235,  1234,  1242,  1181,  1244,  1245,  1252,  1183,
    1185,  1308,     0,  1329,    22,  1328,  1325,  1337,     0,  1330,
      64,     0,   354,   375,   376,     0,   370,     0,   251,   361,
       0,     0,     0,     0,   362,   187,     0,   374,     0,     0,
       0,     0,     0,     0,     0,  1489,     0,    31,   342,    81,
      85,    86,    91,    76,    74,    72,    71,    73,    75,    70,
      77,    99,     0,   116,   101,    97,   107,   117,     0,     0,
     118,    50,    49,   115,   125,   121,   122,   123,   119,   127,
       0,   278,   279,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   248,   169,   171,     0,     0,   151,
     418,   419,   726,     0,   746,   748,   749,     0,   752,   788,
     767,   762,   874,   823,   830,   821,     0,     0,     0,   921,
     922,   920,   929,   928,   927,   930,    22,     0,   940,   980,
    1114,  1118,  1122,     0,     0,     0,  1050,     0,  1233,  1217,
    1216,  1240,  1250,  1306,  1274,  1267,  1331,  1341,   369,   356,
       0,   363,     0,   360,   366,   386,   387,   388,   386,   389,
     372,   371,     0,     0,     0,   379,     0,  1485,  1487,     0,
      95,    79,    78,     0,     0,    78,   106,   106,     0,   108,
     109,   110,   111,   112,   106,   106,     0,   348,   229,   213,
     211,     0,     0,   209,   227,   207,     0,     0,   225,     0,
     223,   222,     0,   220,   358,   359,   745,     0,   536,   537,
      22,     0,     0,     0,     0,     0,   744,   734,   529,   531,
       0,   563,     0,   751,    22,   754,   768,   792,   565,   566,
     567,   938,   939,  1117,     0,  1034,  1104,  1102,  1105,  1032,
      22,     0,    22,  1038,     0,     0,     0,     0,     0,     0,
       0,  1046,  1060,     0,    22,     0,     0,    22,    22,  1232,
       0,     0,  1221,  1236,  1246,     0,  1310,     0,   373,     0,
     186,   201,   204,     0,   378,   203,   239,   236,   237,   238,
     243,     0,     0,     0,     0,     0,     0,     0,     0,   100,
      98,     0,   102,     0,     0,     0,     0,     0,   120,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   732,   532,
       0,   727,   731,   735,     0,   736,     0,   733,   539,   538,
     540,   535,   742,   753,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   791,   787,     0,     0,
       0,     0,     0,     0,     0,     0,    22,     0,     0,     0,
       0,     0,     0,    22,     0,     0,     0,     0,     0,     0,
       0,  1121,  1100,     0,  1106,     0,     0,  1044,  1045,  1043,
    1042,  1054,  1057,  1058,  1059,  1051,  1052,     0,     0,  1055,
    1049,  1224,     0,     0,  1223,  1225,  1230,     0,     0,     0,
       0,     0,     0,     0,    22,     0,     0,     0,     0,  1207,
    1209,  1220,     0,     0,     0,  1239,     0,     0,     0,  1249,
       0,     0,    22,     0,     0,     0,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1309,  1301,     0,     0,     0,     0,  1346,     0,   364,
     253,   205,     0,     0,   186,   367,     0,     0,     0,     0,
       0,     0,    96,     0,     0,   103,   114,   113,   228,   210,
     226,     0,   224,     0,   221,     0,     0,     0,     0,     0,
     737,   738,     0,     0,     0,     0,     0,     0,     0,     0,
     769,     0,   772,   773,   771,     0,     0,     0,     0,     0,
     778,    22,   784,   785,   770,     0,     0,     0,   789,  1069,
    1070,  1071,  1072,  1073,  1074,  1075,  1076,  1066,     0,     0,
       0,  1094,    22,     0,  1108,  1099,  1088,  1089,  1090,    22,
    1063,  1081,    22,  1062,  1083,  1065,     0,     0,  1067,    22,
      22,  1064,     0,    22,  1119,  1107,  1103,  1053,  1056,  1256,
    1256,     0,  1206,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1194,    22,     0,     0,     0,    22,     0,
      22,     0,  1218,     0,  1254,  1238,  1237,  1243,  1241,  1248,
    1247,  1253,  1251,  1280,  1278,     0,     0,     0,  1294,    22,
      22,  1299,  1302,  1305,  1304,  1303,  1300,  1275,     0,  1279,
      22,     0,     0,  1276,  1277,     0,     0,    22,  1281,    22,
      22,    22,  1307,     0,     0,  1345,  1338,  1342,   202,     0,
     240,   244,   252,     0,   384,     0,   381,   382,   380,   383,
       0,     0,     0,     0,   728,   729,   530,   532,   730,   739,
     740,    65,     0,     0,     0,     0,     0,     0,     0,   559,
     560,   561,   562,     0,     0,     0,   563,     0,     0,   776,
     777,   501,   500,   779,   780,   786,   774,   775,    22,  1092,
    1091,  1093,  1096,  1095,  1084,    22,     0,  1082,     0,  1097,
    1080,  1068,  1079,  1078,  1085,  1077,    22,  1226,  1227,     0,
    1192,     0,     0,  1213,  1214,  1215,  1202,  1256,  1203,     0,
    1256,  1196,    22,  1195,  1197,  1198,     0,  1211,  1208,  1212,
    1210,  1255,  1292,  1293,  1295,  1298,  1285,  1287,     0,  1296,
    1290,  1291,  1288,  1289,  1282,  1283,  1284,  1286,     0,     0,
       0,     0,     0,   105,   104,   215,     0,   533,   540,     0,
       0,   540,   540,   549,   550,   551,   552,   553,   554,   555,
     556,   557,   558,   540,     0,     0,   534,   781,   782,   783,
    1086,  1087,  1110,  1109,  1098,  1257,  1204,  1190,  1191,  1200,
    1205,  1201,  1193,  1199,  1297,  1344,  1343,   368,   385,     0,
     545,   540,   540,   547,   546,   548,   540,   540,  1111,     0,
       0,   543,   544,   541,   542,  1112,     0,   230
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -2185, -2185, -2185, -2185, -2185,  -103,  -361,   -73,   518,  -432,
    -732, -2185,  -156,  -164,  1841,  1856,  1857, -2185, -2185, -2185,
    -245, -2185, -2185,  -424, -2185, -1405,  -756,  -568, -2185, -2185,
    1342, -1794,  -102,  -250,   772,  -764, -2185,  1316,  1337,  1348,
    1352,  1082,   797,   805,  1094, -1264,  -434,  1096, -2185, -2185,
   -2185, -2185,  2374, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,   -81, -2185,
   -2185, -2185,  1707,  -798, -2185,  -602,  1579, -2185, -2185, -2185,
    1135, -1914, -2185, -2185,  1310, -2185,  -248,  2009,   552, -1270,
   -1951,  2695, -1927, -1767,  1996,   557,  2419, -2185,  2007, -2185,
   -2185, -2185, -2185,  -497, -2185, -2185, -2185, -1246,  -143, -1234,
      -5, -2185, -1430, -2185, -2185, -2185, -1241, -2185, -2185, -2185,
   -1590, -2185, -2185,  -447, -2185, -2185,   544, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,   -76,  -108, -2185,  2816,  1757, -2185,
    2492,  2128,  -123,  -555,  -471, -2185,    76, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,  1587,  -715,  -723, -2185, -2185, -1942,
   -2185, -2185, -2185, -2185, -2185,  -258, -2185,  1674,   -16, -2185,
    1673, -1185, -2185,   400, -2184, -2185, -2185, -1105, -2185,   382,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185,  -230, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185,  -655, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185,   746,  2343, -1224, -2185, -2185, -2185, -2185, -2185,
   -2185,  1588,   932, -2185, -2185, -2185,  1097, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,  1803,
     341, -2185, -2185,  1084, -2185, -2185, -2185, -2185, -2185, -2185,
    1072, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185,  1043, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185,   911, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,   494, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,   657,   645, -2185, -2185, -2185, -2185,
     491,   644, -2185, -2185, -2185, -2185,   502,   658, -1862, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185,  -392,  1228, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,  2119, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185,  1897,  1937, -2185, -2185, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185,  1222,  2338, -2185, -2185,
   -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185, -2185
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,   108,   109,   715,   728,   396,  2071,  1953,
    1167,   552,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,  2023,  1763,  2186,   886,  1880,  1548,
    1549,  2582,   358,   624,  2012,  2123,  1830,  1831,  1832,  1833,
    1834,  2013,  2014,  2015,  2017,  2018,  2133,  2020,  1835,  1841,
    1585,  1586,   939,  1276,  1277,   264,   265,   266,    22,    23,
      24,    25,   110,   692,   111,   112,   113,   114,  1633,  1356,
    1097,  1098,  1099,  1232,  1233,   867,  1100,   115,   691,  1357,
    1634,  1805,  1806,  1635,  1636,  1637,   267,   590,  2226,   953,
     591,   954,   592,   593,   594,  2230,   595,  2231,   596,   597,
    2102,   548,   268,   549,   269,   270,   933,   271,   272,   273,
    1597,  1570,  1796,  2048,  1638,  1981,  1639,  1802,  2232,  2563,
    1982,  2108,   116,   702,   117,   118,   407,   120,   121,   378,
    1382,  1125,   732,   122,   397,   408,   409,  1389,  1390,  1133,
     751,  1134,   410,   872,   868,  1511,  1237,   123,   370,   371,
     124,  1129,  1653,  1384,  1130,  1263,  1264,  1104,  1105,  2440,
     125,   126,   127,   743,   411,   620,  1025,  1026,   632,  1033,
    1034,  2167,  2168,  2417,  2169,  2170,  2171,  2429,  2593,  2271,
    1698,  1425,  1161,    26,    27,    28,    29,   655,    30,    31,
      32,   641,  1315,  1316,  1057,  1018,   279,    33,   643,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
     314,   680,    45,    46,    47,    48,    49,    50,    51,   280,
      52,    53,   281,   637,   275,   128,  1398,  1135,   129,  1885,
    1657,  1658,  2054,  2172,  2267,  2053,  1888,  1659,  1661,  1891,
    2058,  2175,  1663,    54,   130,  1404,  1136,   131,  1666,  2177,
    2288,  2176,  2060,  1667,    55,    56,    57,    58,   132,   796,
     793,  1415,  1682,  1683,  1178,   133,   785,  1426,   464,   465,
    2072,  1933,  1483,  1214,   829,  1215,   466,   134,  1492,  1216,
     135,    59,   682,   393,   136,   137,   844,   482,   483,   484,
     485,   486,  1226,  1506,  1747,  1744,  1750,  1751,  2085,  2194,
    1507,  2086,  2203,  1508,  2313,  2083,  2187,  2188,  2316,  2616,
    2729,  2703,  1738,  1756,  1937,  2080,  2184,  2082,  1739,    60,
      61,    62,    63,    64,    65,    66,    67,  1072,   138,   139,
     497,   140,   735,   141,   142,  1523,  1768,  1769,  1243,   143,
    1766,  2352,  2509,  2511,  2636,  2212,  2211,  2090,  1767,  1764,
    2210,  2088,  2087,  1950,  1765,  1954,  2091,  2356,  2517,  2213,
    1955,  1958,  2092,  2360,  2521,  2214,  1959,  2515,  2627,   394,
     144,  1530,  1244,   145,  1773,  2216,  2383,  2536,  2215,  2094,
    1774,    68,    69,   146,   147,  1245,  1541,  1780,  1542,  1786,
    1789,  1967,  1968,  2217,  2388,  2556,   148,   149,   898,   899,
     519,   520,  1573,  1256,   901,    70,   150,   532,   372,   151,
    1115,  1117,   725,  1120,   677,   678,   152,   153,    71,    72,
      73,    74,    75,    76,    77,    78,   323,   689,   154,  1575,
     533,   155,   156,   540,   914,  1996,  1813,  1578,   915
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     365,   557,   274,   589,   792,   795,   588,   352,   333,   556,
    1568,   941,   334,   335,   729,  1571,   496,   550,  1596,  1598,
    1600,  1601,  1569,   622,   835,   463,   518,   531,  1131,   481,
     870,   495,  1572,   507,   625,  1165,  1180,   554,   555,  1056,
    1163,   517,   530,   311,   919,  1851,   920,   921,   539,  1289,
     927,   928,   412,  1856,   413,  2142,   414,  2149,   467,  2152,
     468,   918,   487,  1179,  1787,   681,   509,  1118,   510,   541,
     511,   658,   512,  1055,   542,  2418,    80,  1956,   534,   535,
    1090,  2220,   336,   541,   543,  2204,   544,   587,   542,   885,
    2221,  2222,   923,   924,  1794,   586,  2121,  2125,   543,  2529,
     544,   704,   705,  2496,   579,   580,   163,   581,   582,   583,
      81,  1180,  1568,  1770,  2003,  2004,  2005,    79,  2121,  2125,
      82,   706,    83,    84,  1569,  1797,   707,   788,   789,   766,
     790,  2006,  2007,  2008,  1572,   601,   602,   603,   604,   605,
    1809,   555,   742,  1761,   956,   957,   554,   555,    85,  1163,
    1899,  2227,   606,   607,   608,   609,   610,   611,   612,   613,
     614,   853,   697,   615,   698,   562,   312,   699,   601,   602,
     603,   604,   605,   706,   617,  2228,   788,   789,   707,   790,
     601,   602,   603,   604,   605,   606,   607,   608,   609,   610,
     611,   612,   613,   614,     1,   700,   615,   606,   607,   608,
     609,   610,   611,   612,   613,   614,  2498,   629,   615,  1994,
    2532,   706,  2009,  2010,  1983,   560,   707,   885,   697,    86,
     698,    87,    88,   699,    89,   356,    90,   357,    91,   768,
      92,    93,    94,  1181,    95,   770,   579,   580,   923,   924,
     582,   583,   638,   157,  1318,   578,  2533,    96,    97,    98,
      99,   100,   101,  2221,  2222,   923,   924,   772,   709,  2221,
    2222,   923,   924,   601,   602,   603,   604,   605,  1145,   706,
     724,   727,   727,   158,   707,   359,   360,  2436,  2437,   694,
     606,   607,   608,   609,   610,   611,   612,   613,   614,  1349,
     719,   615,  2493,  1128,   361,  1722,  1889,   618,   589,   102,
     745,   746,   649,   730,   706,   103,  2390,   854,  1181,   707,
    1995,   104,   105,   579,   580,   923,   924,   582,   583,  2442,
    1242,   601,   602,   603,   604,   605,   775,  1139,  1235,   561,
     630,   706,   787,  1247,  1149,  2229,   707,   374,   606,   607,
     608,   609,   610,   611,   612,   613,   614,   706,   717,   615,
    2461,  2530,   707,  1109,   822,   601,   602,   603,   604,   605,
     159,  1376,  1377,   579,   580,   923,   924,   582,   583,  2322,
     316,   838,   606,   607,   608,   609,   610,   611,   612,   613,
     614,   788,   789,   615,   790,   706,   289,   649,   747,  1362,
     707,  1568,   706,  2418,   706,   375,   860,   707,   550,   707,
     550,   550,  1325,  1569,   550,   550,  2314,  1900,   710,   711,
     712,   713,  2315,  1572,  1065,  1066,  1067,  1068,  1069,   903,
    1070,  1071,  2528,  1074,  1076,  2190,  1078,  1079,  1268,   601,
     602,   603,   604,   605,  2583,  2028,  2205,  2206,  2494,  2479,
    2207,  2208,  2227,   767,  1073,  2438,   606,   607,   608,   609,
     610,   611,   612,   613,   614,  2443,  2497,   615,  1150,   706,
    1249,   356,  1250,   357,   707,   706,  2228,   317,  1378,  1379,
     707,  2534,  1380,  1317,   601,   602,   603,   604,   605,  1119,
    2562,  1723,   710,   711,   712,   713,   290,  2323,  1925,  1227,
    1228,   606,   607,   608,   609,   610,   611,   612,   613,   614,
    1168,   720,   615,   601,   602,   603,   604,   605,   706,   639,
    2594,   706,  2439,   707,  1788,  2233,   707,  1545,  2234,  2466,
     606,   607,   608,   609,   610,   611,   612,   613,   614,  1075,
    2328,   615,  2191,  1351,   710,   711,   712,   713,  2209,   356,
     922,   357,  2099,   769,  1890,  2535,  1164,   119,   760,   771,
    2499,   761,   714,   934,   788,   789,   821,   790,  2480,  2500,
    1568,  1182,   942,   943,   944,   945,   946,   947,   706,  2019,
    1064,   773,  1569,   707,  1975,  2019,   324,  1146,   313,   955,
    2584,  2034,  1572,  2036,  1322,  1093,  2038,  2192,  2041,  2043,
    2491,   710,   711,   712,   713,  2185,   966,   967,   968,   969,
     970,   971,   972,   973,   974,   975,   976,   977,   978,   979,
    1852,   727,  1926,   355,  1490,  1290,  1111,   885,  1857,  1858,
    2143,  1116,  2150,  1121,  2153,   545,  2229,   587,  2628,  1957,
    2324,   546,  2586,   547,   106,   586,  1702,  2467,   362,   545,
     558,   799,   356,   107,   357,   546,  2223,   547,   800,  1251,
    2224,   601,   602,   603,   604,   605,  2595,  1031,  2126,  1126,
     584,  1143,  1144,   936,  2477,  1613,  1614,   585,   606,   607,
     608,   609,   610,   611,   612,   613,   614,   708,   363,   615,
    2134,   480,   706,   783,  1952,  1095,  1731,   707,   958,  2609,
    2610,  2543,  1086,   318,   960,  1184,  1106,  1169,  1187,   961,
     276,  1978,   277,  1108,  2189,   710,   711,   712,   713,   619,
     816,   589,   589,  1032,   319,   706,   364,   588,   701,   557,
     707,  1210,  1252,   356,  1253,   357,  1323,   556,   786,   708,
    2193,  1568,   950,   951,  2701,   791,  2101,  1223,   710,   711,
     712,   713,   631,  1569,  1267,  1445,  1749,   727,   727,  2107,
    2544,   817,   891,  1572,   722,   283,   284,   287,   288,  2109,
     706,   723,  1236,   353,   315,   707,  1236,   708,  1168,  1579,
    1236,   706,  1580,  1168,   701,  2709,   707,   937,  2711,   634,
    2468,   635,   550,   710,   711,   712,   713,   818,   819,  2679,
    2680,   744,   584,  1381,   710,   711,   712,   713,   587,   925,
    2694,  2695,  1260,   554,   555,  1262,   586,  2459,  2437,  2559,
     373,  2547,   656,  1265,  1209,  2223,  1327,  1328,  1329,  1330,
    1331,  1332,  1170,  1171,  1343,   708,  1345,  1280,  1346,  1347,
     601,   602,   603,   604,   605,  1087,   862,   282,   634,   277,
     635,   706,   278,  1342,   354,  1344,   707,   606,   607,   608,
     609,   610,   611,   612,   613,   614,   962,  1234,   615,  1088,
     708,   963,  2243,  2244,   710,   711,   712,   713,  2357,  1294,
    2246,  2247,  2250,   706,  2358,   649,  1311,  2252,   707,  2253,
     716,  1254,  2255,  1089,  1310,  1593,  1594,   708,   657,  1622,
     706,   733,   734,  1623,  1624,   707,   710,   711,   712,   713,
     376,  2202,  1455,   708,   601,   602,   603,   604,   605,   377,
    1685,  1920,  1921,   710,   711,   712,   713,   741,   794,   584,
    1281,   606,   607,   608,   609,   610,   611,   612,   613,   614,
    1733,   668,   615,   498,   499,  1886,   601,   602,   603,   604,
     605,   708,   645,  1893,  1286,  1239,  1287,   776,   708,  1240,
     708,   551,   784,   606,   607,   608,   609,   610,   611,   612,
     613,   614,  1662,   553,   615,  1169,   809,  2359,  2513,   646,
    1169,   553,  1387,  1568,   820,  2438,   500,   501,  2389,   278,
     636,   502,   892,  2526,  2437,  1569,  1609,  1352,   764,   765,
     836,  2287,   503,   839,  1282,  1572,   842,   870,  1151,  2312,
    1360,   504,   320,   321,   322,  1810,   849,   558,   757,  1218,
     669,   893,  1371,  1403,  1372,   708,  1811,  1359,  -901,   650,
     894,   708,  1082,  1936,  1083,  1172,   563,  1397,  1402,  2263,
    2382,   626,   627,  1930,  1405,  1406,   780,  1408,  1409,  1283,
     805,   806,  2460,  1951,   907,   647,   909,  1881,  1418,   706,
    1961,   325,   326,   554,   707,   282,  2021,   277,  1882,  1883,
     559,  1931,  1932,  2264,   708,  1430,  1431,   708,  1284,  1916,
    1413,   648,   710,   711,   712,   713,  1726,  1434,  1435,   395,
     564,  2353,  1438,  1439,   758,  1441,  2354,  1443,   565,   366,
    1170,  1171,  1625,  1428,  1627,  1170,  1171,   566,   325,   326,
    1456,  1740,  1152,  1784,   960,   781,  1460,  2393,  1491,  1269,
    2394,  2384,  2385,  1219,  1301,  1493,  1302,  2386,   567,  1494,
    1522,  1529,  1540,   550,   708,   568,   550,   613,   614,  1655,
     651,   615,   569,   782,  1652,  1521,  1528,  1539,   570,   285,
    1741,  2572,  1782,  2573,   557,  1790,   571,  2265,  2266,  1550,
     572,  2438,   556,   601,   602,   603,   604,   605,  1576,   598,
     788,   789,  1927,   790,  1884,  1742,  1934,   367,   368,   369,
     606,   607,   608,   609,   610,   611,   612,   613,   614,  2387,
    2355,   615,   601,   602,   603,   604,   605,   895,   896,   897,
    2069,  2070,  1303,  1917,  1304,   554,   555,   278,  1163,   606,
     607,   608,   609,   610,   611,   612,   613,   614,   599,  1918,
     615,   601,   602,   603,   604,   605,  1592,  1919,  2527,  1306,
    1307,  1308,  1309,   601,   602,   603,   604,   605,   606,   607,
     608,   609,   610,   611,   612,   613,   614,   616,   708,   615,
     606,   607,   608,   609,   610,   611,   612,   613,   614,   850,
     851,   615,  1745,  2127,  1675,   621,  2135,  1164,   623,   706,
     286,   628,  2127,  2135,   707,   871,  1132,  1350,  1582,   633,
    2157,   708,   856,   857,  1587,  1631,  1588,  1589,  1590,  1591,
    1112,  1113,   710,   711,   712,   713,  1640,   640,  1874,  1603,
    1122,  1123,  1124,  1700,   652,  1606,  1153,  1154,  1701,   642,
    1157,  1944,   644,  2587,  2588,  2633,  2634,  2635,   653,   654,
    2589,  2590,  2591,  2592,  1687,  1183,   708,   659,  2158,  2159,
    2160,  1189,  1082,  1191,  1643,  1193,  1194,   708,  1196,  1197,
    1198,  1199,  1200,  1201,  1913,   649,  1204,  1205,   587,  1207,
    1367,  1368,   706,  1212,  1213,   302,   586,   707,   679,  2161,
    1762,  1217,  1220,   660,  1762,  1221,   670,   788,   789,  1224,
     790,   356,  1248,   357,  2268,   710,   711,   712,   713,   661,
    2162,   303,   662,  2163,  2164,   663,  1728,   554,   882,   883,
     884,   -68,   -68,  1734,   -68,  1735,  1388,  1173,  1174,  1175,
    1176,  1177,  1241,  1676,   304,  1842,   305,   708,   306,   671,
     672,   673,   674,   675,   676,  1246,  2719,   171,   664,   173,
     174,  2165,   327,   665,   328,   329,   330,   331,   332,   871,
    1258,  1732,  1259,  1776,   555,  1568,  1761,   666,  1778,   708,
    2717,   667,  1816,  1785,  1904,   317,  1905,  1569,  1817,  1818,
    1819,  1820,   684,  1821,  1822,  1823,   708,  1572,  1824,   686,
    1825,  1826,  1757,  1758,  1759,  1760,  1827,  1828,   687,   327,
    2736,   328,   329,   330,   331,   332,   688,  1875,  1876,  1972,
    1877,  1812,   307,   308,   309,   310,   690,  1752,  2683,  2684,
    2685,  2686,  2687,  1989,  1990,   695,  2074,  1991,  1879,  2618,
     696,   861,   862,   703,  1992,  1993,  1878,  2045,   731,  2046,
    1285,   601,   602,   603,   604,   605,  1427,   863,  1429,   718,
    2050,  2051,   864,   601,   602,   603,   604,   605,   606,   607,
     608,   609,   610,   611,   612,   613,   614,   869,   721,   615,
     606,   607,   608,   609,   610,   611,   612,   613,   614,   726,
     683,   615,   685,   736,  1691,  1873,   727,   325,   326,  1795,
    1795,  1677,  1799,  1272,  1273,   737,  1800,  1801,  1865,  1678,
    1866,  1679,  2117,  1680,   738,  1795,  1681,  1895,  2129,  2130,
    2131,  2132,  2658,  2720,  1278,  1279,  2723,  2724,  2423,   601,
     602,   603,   604,   605,  1369,  1370,   739,  1847,  2725,  1848,
    1610,  1611,   727,  1914,  1692,   748,   606,   607,   608,   609,
     610,   611,   612,   613,   614,   708,   750,   615,  1648,  1649,
    2424,   601,   602,   603,   604,   605,  2731,  2732,   740,   693,
     749,  2733,  2734,   499,   763,  2173,  1849,  1850,   606,   607,
     608,   609,   610,   611,   612,   613,   614,  1938,  1939,   615,
    1693,   752,  1942,   754,  1694,  1854,  1855,  1695,  1696,  1946,
    1902,  1903,  1947,   788,   789,   398,  1783,   753,   357,   755,
     399,   400,   756,  1412,  1414,   500,   501,   759,   402,   762,
     502,  1909,  1910,  1422,  1423,   774,  1762,  1963,  2022,  1965,
     777,   503,  2000,  2001,  1333,   778,  2425,  1969,  2121,  2122,
     504,   601,   602,   603,   604,   605,  2140,  2141,  2145,  2146,
     797,  1334,  2166,  2256,  2257,  2178,  2179,  2180,   606,   607,
     608,   609,   610,   611,   612,   613,   614,  1292,   779,   615,
    1454,  1980,   802,   403,  2426,   798,   404,   801,  2011,  1462,
    2427,  2158,  2159,   803,  2011,   804,  1816,  2420,  2421,   807,
    1335,   808,  1817,  1818,  1819,  1820,  1364,  1821,  1822,  1823,
    2428,   810,  1824,   811,  1825,  1826,  1512,  1019,  1020,  1021,
    1827,  1828,  2688,  2689,  2690,  2691,  2692,   812,   727,   727,
     813,  1551,  1552,  2432,  2433,  1365,   601,   602,   603,   604,
     605,  1022,  1023,  2579,  2580,  1024,   814,  1843,  2056,  1971,
    2597,  2598,   824,   606,   607,   608,   609,   610,   611,   612,
     613,   614,  2261,  1986,   615,  1988,   601,   602,   603,   604,
     605,  1391,  2406,  2407,   815,   708,  2055,   823,   826,   825,
     827,   828,   833,   606,   607,   608,   609,   610,   611,   612,
     613,   614,   830,   831,   615,   832,  1336,  2030,  1164,  1164,
     834,   837,   846,   601,   602,   603,   604,   605,   840,   841,
     843,   847,   848,   852,  2225,   855,   858,   859,   873,  2095,
     606,   607,   608,   609,   610,   611,   612,   613,   614,   871,
     874,   615,   875,   876,   877,   878,   879,   887,   888,  1484,
     862,   880,   881,   505,   889,   900,   398,   890,   398,   902,
     905,   399,   400,   399,   400,  1392,  1646,  1647,   708,   402,
     904,   402,   906,  1485,   908,   910,   911,  2103,   327,  2120,
     328,   329,   330,   331,   332,  2128,   861,   862,   912,   587,
     587,   913,   601,   602,   603,   604,   605,   586,   586,   917,
    1337,   916,   863,   929,  1686,   930,   931,   864,  2539,   606,
     607,   608,   609,   610,   611,   612,   613,   614,   932,   938,
     615,   948,   865,   935,   403,   949,   403,   404,   866,   404,
     615,   959,  1829,   727,   727,   727,   398,  1795,   964,  1027,
     965,   399,   400,  1028,  2182,   536,  1697,  2105,  1029,   402,
    1030,  1035,   601,   602,   603,   604,   605,  1037,  1038,  1486,
    1077,  1080,  1081,  1338,  1724,  1725,  1727,  1084,  1729,   606,
     607,   608,   609,   610,   611,   612,   613,   614,  1085,  1091,
     615,  1393,  1394,  1094,  1096,  1743,  2239,  1746,  1395,  2011,
    1102,  2011,  1753,  1101,  1107,  1103,  1110,  1114,   601,   602,
     603,   604,   605,  1127,   403,  1137,  1138,   404,  1140,  1141,
    1155,  1142,  2154,  1775,  1156,   606,   607,   608,   609,   610,
     611,   612,   613,   614,  2574,  2575,   615,  1147,  2578,  1160,
     727,  2262,  2003,  2004,  2005,  1844,  1148,  1158,  2269,  1166,
    1202,  1339,  1159,  1185,  1162,   506,  1186,  1845,  1188,  2006,
    2007,  2008,  1190,  1192,  2606,  2607,  1203,  1195,  2318,  1206,
    1208,  1222,  2321,  1340,  2325,  2326,  1211,  2329,   601,   602,
     603,   604,   605,  1229,  1230,  1231,  1238,  2235,  2236,  2237,
     892,  2238,  1257,  1261,  1266,   606,   607,   608,   609,   610,
     611,   612,   613,   614,  1270,  1341,   615,  1271,  1274,   601,
     602,   603,   604,   605,  1275,  1288,  1487,  1488,  2402,  1291,
    2403,  2404,  2405,  1846,  2225,  2225,   606,   607,   608,   609,
     610,   611,   612,   613,   614,  1293,  1300,   615,  1298,  1305,
    2009,  2010,   861,   862,  1299,  2697,  2698,  1319,  1320,  1321,
    1324,  2430,  1326,  1762,  2434,  1861,  1353,  1355,   863,  2444,
    2660,  2274,  1892,   864,  2702,  1361,  1714,  2457,  1363,  1366,
    2275,  1374,   861,   862,  1375,  2276,  2470,  2471,   865,  2473,
    1385,  2475,  1383,  2478,   866,  2481,  1510,  1386,   863,  1407,
    1911,  1410,  2485,   864,  1411,  1416,  1715,  1417,  2391,  1419,
    -902,  -902,  -902,  1546,  2487,  2488,  1420,  2395,   869,  2489,
    2490,  1922,  1923,  1924,  1421,  1424,  1928,  -902,  -902,  -902,
    -902,  -902,   611,   612,   613,   614,  1547,  1574,   615,  1432,
    2514,  1581,  1436,  1583,  2514,  2031,  1584,  1437,  2523,  1440,
    1940,  1604,  1577,  1442,  1444,  2537,  1836,  1446,  1602,  1447,
    1448,  1605,  1612,  1615,  2548,   601,   602,   603,   604,   605,
    1449,  1450,  1451,  1452,  2555,  1453,  1457,  1458,  1459,  1960,
    1616,  1461,   606,   607,   608,   609,   610,   611,   612,   613,
     614,  1617,  1463,   615,  1464,  1513,  1396,  1543,  1489,  1544,
    1618,  1619,   727,   727,  1620,  1621,   727,  1626,  2277,  1628,
    1629,  1630,  2585,  1641,  1644,  1642,  1645,  2158,  2159,  1650,
    1656,  1651,  2599,  2600,  2601,  2602,  2603,  1664,  1668,   291,
    2032,   292,   727,   727,   601,   602,   603,   604,   605,  1665,
    1669,  1670,  1712,  1713,  1671,  2611,  2612,  2613,  2161,  1672,
    1673,   606,   607,   608,   609,   610,   611,   612,   613,   614,
    2137,   293,   615,  2620,  2621,  1674,   537,  1684,   294,  2624,
    2278,  2279,  1688,  1689,  2280,  1690,   538,  1716,  2337,  1699,
    2630,  1703,  2565,  1717,  2637,  1718,  1704,  2640,  2641,  1719,
    1705,  1706,  2644,  2645,  1707,  1720,  1708,  2398,  1721,  1709,
    2651,   295,  2061,   296,  1736,  1710,  1711,  1730,  1737,  1748,
    2281,   297,  2652,  2653,  2654,  1754,  1755,  1771,  2282,  2283,
    2284,  1772,   298,  1777,  1781,  2657,  1779,  1791,   727,  2661,
    2361,  2285,  2662,  2663,  1793,  2338,  2339,  2078,  1798,  1803,
    1804,  1807,  1808,  2079,  1814,   601,   602,   603,   604,   605,
    1815,  1853,  1859,  1860,  1862,  1863,  1864,  2673,  2674,  1867,
    1869,  2340,   606,   607,   608,   609,   610,   611,   612,   613,
     614,  1870,  2096,   615,  1887,  1871,  2399,   601,   602,   603,
     604,   605,    82,  1872,    83,    84,  1894,  1896,  1897,   601,
     602,   603,   604,   605,   606,   607,   608,   609,   610,   611,
     612,   613,   614,  1898,  1901,   615,   606,   607,   608,   609,
     610,   611,   612,   613,   614,  1929,  2706,   615,  2707,  2708,
    1906,  1514,  1907,  1908,  1912,  1915,  2710,  1935,  2341,  1962,
    1964,  1941,  1943,  2713,  1531,  1945,  2400,   299,  1970,  1997,
    1948,  2342,  1973,  1949,  2670,  1984,  1974,  1998,  1999,  2002,
    1987,  2024,  2158,  2159,  2016,  2029,  2715,  2716,  2033,   337,
     338,   339,   340,   341,   342,   343,   344,   345,  2037,   346,
     347,   348,   349,   350,   351,  2039,  2158,  2159,  2362,  2044,
    2047,  2049,  2401,  2161,  2181,  2057,  2059,  2062,  2063,  2064,
     398,  2066,  2067,  2075,  2068,   399,   400,  2077,  2076,  1515,
    2081,  2195,  2089,   402,  2093,  2097,  2100,  2161,  2104,    96,
      97,    98,    99,   100,   101,  2449,  2450,  2451,  2452,  2453,
    2454,  2455,  2456,   300,  2112,  2113,  2119,  2136,  2114,  2363,
    2364,  2115,  2124,  2365,  2116,  2118,  1531,  2174,    82,  2138,
      83,    84,  2139,  1495,  2144,  2196,  2148,   301,  1496,  2151,
    2219,  2155,  2558,  2147,  2197,  2218,  2245,  2248,   403,  2249,
    2251,   404,  2254,  1058,  2366,  2259,  2260,  2272,  2258,  2367,
    2270,  2320,  2392,  2327,  2330,  2336,  2368,  2332,  2413,  2369,
     325,   326,  2273,  2672,  2333,  2396,  2415,  2397,   160,  2408,
     161,   162,   163,   164,   165,   166,  2409,   167,  2317,  2410,
    2319,  2286,  2411,  2412,  2416,   168,   169,   952,  2414,  2419,
     170,  2422,  2331,  2370,  2431,  2334,  2335,  2435,  2445,  2441,
    2446,  2447,   171,   172,   173,   174,   469,    82,   175,    83,
      84,  2458,  2448,  2462,  2463,  1059,  2371,  2469,  2464,  2472,
    2476,  2482,  2483,  2495,  2484,  2486,   398,  2502,  2505,  1497,
    2504,   399,   400,  2507,  2343,  2501,  2344,  2345,  2508,   402,
    2506,  2346,  2347,  2510,  2348,    96,    97,    98,    99,   100,
     101,  2349,  2350,  1956,  2512,  2518,  2522,  2542,  2198,  2199,
    2200,   176,   177,   573,   574,   575,   576,   178,   179,  2525,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
    2538,  2540,  2372,  2541,  2465,  2545,  1060,  2546,   190,   191,
     192,  2474,  2549,   193,   403,  2551,  2570,   404,  1061,  1062,
    2550,   194,  2552,  2553,  2557,  2373,  2554,  1354,  2564,   195,
    1063,  2566,  2567,  2568,  2569,   398,  2571,  2581,  2577,  2605,
     399,   400,  2576,  2374,  2596,  2608,  2615,  1498,   402,  2626,
    2629,  2631,  2503,  2632,    96,    97,    98,    99,   100,   101,
    2642,  2639,  2668,  2646,   980,   981,  2669,  2351,  2671,  2676,
    2524,  1312,   982,  2704,  2531,  2675,  2714,  2728,  1792,  2375,
    2376,  2377,  2378,  2730,  2379,  2241,  1313,  1314,  2380,  2678,
    2737,  2681,  1837,  1499,  2682,  2693,  1976,  2718,   983,  1532,
     984,  2721,  2722,   403,  2726,   196,   404,  2727,  2735,  2025,
    1500,  2242,  1501,  1838,  1516,  1517,  1502,  2240,  1518,   470,
    1503,  1504,  2026,  1519,  1839,  2027,  1632,   985,  1840,   940,
    1509,  1985,  1533,  1534,  2560,   577,  2201,  1868,   986,   987,
    2561,   988,   989,   990,   991,   992,  1296,   508,    82,  2604,
      83,    84,   993,   994,   995,   926,   996,   997,  1295,  1297,
    2381,  1482,  1654,  1225,   845,  1607,  1608,  2677,  2696,  1036,
    2614,  1660,  1433,  2052,  2156,   197,  2084,  2617,  2065,  2073,
    2619,  2183,  2638,    82,  2492,    83,    84,  2622,  2623,  2516,
    2648,  2625,  2520,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,  2650,  1966,  2519,  1373,  2098,  1255,  1348,
    1520,     0,  2643,  1092,     0,     0,  2647,     0,  2649,     0,
     208,   209,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  2655,  2656,  2110,
       0,   327,     0,   328,   329,   330,   331,   332,  2659,   210,
    1358,  2111,     0,     0,  1505,  2664,   398,  2665,  2666,  2667,
       0,   399,   400,     0,   211,     0,     0,     0,     0,   402,
       0,     0,     0,     0,     0,    96,    97,    98,    99,   100,
     101,     0,     0,   212,   213,   214,   215,     0,     0,     0,
       0,   398,   471,   472,   473,   474,   399,   400,  1535,     0,
       0,     0,     0,  1536,   402,  1537,     0,     0,     0,     0,
      96,    97,    98,    99,   100,   101,  2699,     0,     0,     0,
     475,     0,     0,  2700,   403,   476,     0,   404,   477,     0,
       0,   478,     0,     0,  2705,     0,     0,     0,     0,     0,
     521,   337,   338,   339,   340,   341,   342,   343,   344,   345,
    2712,   346,   347,   348,   349,   350,   351,     0,     0,   403,
       0,     0,   404,     0,   216,   217,  1538,    82,     0,    83,
      84,     0,   513,     0,     0,     0,     0,     0,     0,     0,
     218,   219,     0,     0,   514,     0,     0,     0,     0,     0,
       0,     0,   522,     0,     0,     0,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,     0,     0,     0,     0,     0,     0,
       0,   160,     0,   161,   162,   163,   164,   165,   166,     0,
     167,     0,     0,   248,   249,     0,   250,     0,   168,   169,
       0,     0,   251,   170,     0,   479,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   171,   172,   173,   174,     0,
      82,   175,    83,    84,     0,   398,     0,     0,     0,     0,
     399,   400,     0,   515,     0,     0,     0,     0,   402,     0,
       0,     4,     5,     6,    96,    97,    98,    99,   100,   101,
       0,    82,     0,    83,    84,     0,     0,     0,   488,   998,
       0,   999,  1000,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   176,   177,   573,   574,   575,   576,
     178,   179,     0,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   403,     0,     0,   404,     0,     0,   523,
       0,   190,   191,   192,     0,     0,   193,     0,     0,     0,
       0,     0,     0,     0,   194,     0,     0,     7,     0,     0,
       0,     0,   195,     0,     0,   489,     0,     0,   398,     0,
       8,     0,     0,   399,   400,     0,     0,  1524,     0,     0,
       0,   402,     0,     0,     0,     0,   490,    96,    97,    98,
      99,   100,   101,     0,     0,     9,    10,     0,     0,   398,
       0,     0,     0,     0,   399,   400,     0,     0,   401,     0,
     524,   525,   402,     0,     0,     0,     0,     0,    96,    97,
      98,    99,   100,   101,     0,     0,  1001,  1002,  1003,  1004,
    1005,  1006,     0,    11,     0,    12,   403,    13,   196,   404,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,     0,     0,     0,   526,   527,   528,     0,     0,
       0,     0,     0,     0,     0,     0,   529,   403,     0,     0,
     404,     0,     0,     0,     0,     0,     0,    15,     0,     0,
       0,    16,     0,     0,     0,     0,    17,    18,     0,     0,
       0,     0,   405,     0,     0,     0,     0,     0,     0,     0,
       0,   516,     0,     0,     0,     0,     0,     0,   197,  1007,
    1008,  1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,  1017,
       0,     0,     0,     0,     0,     0,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,     0,     0,     0,     0,
     160,     0,   161,   162,   163,   164,   165,   166,     0,   167,
       0,     0,     0,   208,   209,     0,     0,   168,   169,  1595,
       0,     0,   170,     0,     0,     0,   491,   492,   493,     0,
       0,     0,     0,     0,   171,   172,   173,   174,     0,     0,
     175,     0,   210,     0,   601,   602,   603,     0,    82,     0,
      83,    84,     0,     0,   379,     0,     0,   211,     0,     0,
       0,   606,   607,   608,   609,   610,   611,   612,   613,   614,
       0,    19,   615,     0,     0,     0,   212,   213,   214,   215,
       0,   380,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   176,   177,     0,     0,     0,     0,   178,
     179,     0,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   381,  1525,     0,     0,     0,     0,     0,     0,
     190,   191,   192,     0,     0,   193,     0,     0,     0,     0,
       0,     0,     0,   194,     0,     0,     0,     0,     0,     0,
       0,   195,     0,     0,     0,   494,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   398,   216,   217,     0,
       0,   399,   400,     0,     0,  1399,  1526,     0,     0,   402,
     382,     0,     0,   218,   219,    96,    97,    98,    99,   100,
     101,   383,     0,     0,     0,     0,     0,     0,     0,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   196,     0,     0,
       0,     0,     0,     0,   403,     0,     0,   404,     0,     0,
       0,    20,    21,     0,     0,     0,   248,   249,     0,   250,
       0,     0,     0,     0,     0,   251,     0,     0,  1527,     0,
     601,   602,   603,   604,   605,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   606,   607,   608,
     609,   610,   611,   612,   613,   614,     0,     0,   615,   406,
       0,     0,     0,     0,     0,     0,     0,   197,     0,     0,
       0,     0,     0,   384,     0,     0,   385,     0,     0,     0,
       0,     0,     0,  1400,     0,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,     0,     0,     0,     0,   160,
       0,   161,   162,   163,   164,   165,   166,     0,   167,     0,
       0,     0,   208,   209,     0,     0,   168,   169,  1599,     0,
       0,   170,  1977,     0,     0,     0,   386,     0,     0,     0,
     387,   388,     0,   171,   172,   173,   174,     0,     0,   175,
       0,   210,     0,     0,     0,     0,   601,   602,   603,   604,
     605,     0,     0,     0,     0,     0,   211,     0,     0,     0,
       0,     0,   389,   606,   607,   608,   609,   610,   611,   612,
     613,   614,     0,     0,   615,   212,   213,   214,   215,     0,
       0,     0,     0,     0,     0,     0,   390,     0,     0,     0,
     391,     0,   176,   177,     0,     0,     0,     0,   178,   179,
       0,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,     0,     0,     0,     0,     0,     0,     0,     0,   190,
     191,   192,   600,     0,   193,     0,     0,   601,   602,   603,
     604,   605,   194,     0,     0,     0,     0,     0,   392,     0,
     195,     0,     0,     0,   606,   607,   608,   609,   610,   611,
     612,   613,   614,     0,     0,   615,   216,   217,     0,     0,
       0,     0,  1465,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   218,   219,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1979,     0,     0,     0,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   196,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   499,     0,     0,     0,   248,   249,     0,   250,     0,
       0,     0,  1466,     0,   251,     0,     0,   337,   338,   339,
     340,   341,   342,   343,   344,   345,  1401,   346,   347,   348,
     349,   350,   351,     0,     0,     0,     0,     0,     0,  1387,
       0,     0,     0,  1467,  1468,     0,     0,     0,   502,     0,
       0,     0,     0,     0,     0,     0,   197,     0,     0,   503,
       0,     0,     0,     0,     0,     0,     0,     0,   504,     0,
       0,     0,     0,     0,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,     0,     0,     0,     0,   160,     0,
     161,   162,   163,   164,   165,   166,     0,   167,     0,     0,
       0,   208,   209,     0,     0,   168,   169,  2035,     0,     0,
     170,     0,     0,  1469,     0,  1470,     0,  1471,     0,     0,
       0,     0,   171,   172,   173,   174,     0,     0,   175,     0,
     210,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   211,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   212,   213,   214,   215,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   176,   177,  1472,     0,     0,     0,   178,   179,     0,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
       0,     0,     0,     0,     0,     0,     0,     0,   190,   191,
     192,     0,     0,   193,     0,     0,  1473,     0,     0,     0,
       0,   194,  1039,     0,     0,  1474,  1475,     0,     0,   195,
       0,     0,  1476,     0,     0,     0,   554,   882,   883,   884,
       0,     0,     0,     0,  1477,   216,   217,     0,     0,     0,
       0,     0,     0,  1478,     0,  1479,     0,     0,  1480,     0,
       0,   218,   219,     0,     0,     0,   171,     0,   173,   174,
       0,     0,     0,     0,     0,     0,     0,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,   196,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1040,  1041,   248,   249,     0,   250,     0,     0,
       0,     0,     0,   251,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1042,   862,     0,     0,     0,
       0,     0,     0,     0,     0,   197,     0,  1043,     0,     0,
       0,  1044,     0,     0,  1045,  1046,     0,     0,     0,  1047,
       0,     0,     0,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,     0,     0,     0,     0,   160,  1048,   161,
     162,   163,   164,   165,   166,     0,   167,     0,     0,     0,
     208,   209,     0,     0,   168,   169,  2040,     0,     0,   170,
       0,  1049,  1050,  1051,     0,     0,  1052,  1053,  1054,     0,
       0,   171,   172,   173,   174,     0,     0,   175,     0,   210,
       0,     0,     0,  1481,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   211,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   212,   213,   214,   215,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     176,   177,     0,     0,     0,     0,   178,   179,     0,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,     0,
       0,     0,     0,     0,     0,     0,     0,   190,   191,   192,
       0,     0,   193,     0,     0,     0,     0,     0,     0,     0,
     194,     0,     0,     0,     0,     0,     0,     0,   195,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   216,   217,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     218,   219,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,   196,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   248,   249,     0,   250,     0,     0,     0,
       0,     0,   251,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   197,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,     0,     0,     0,     0,   160,     0,   161,   162,
     163,   164,   165,   166,     0,   167,     0,     0,     0,   208,
     209,     0,     0,   168,   169,  2042,     0,     0,   170,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     171,   172,   173,   174,     0,     0,   175,     0,   210,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   211,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   212,   213,   214,   215,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   176,
     177,     0,     0,     0,     0,   178,   179,     0,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,     0,     0,
       0,     0,     0,     0,     0,     0,   190,   191,   192,     0,
       0,   193,     0,     0,     0,     0,     0,     0,     0,   194,
       0,     0,     0,     0,     0,     0,     0,   195,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   216,   217,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   218,
     219,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   196,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   248,   249,     0,   250,     0,     0,     0,     0,
       0,   251,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   197,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,     0,     0,     0,     0,   160,     0,   161,   162,   163,
     164,   165,   166,     0,   167,     0,     0,     0,   208,   209,
       0,     0,   168,   169,     0,     0,     0,   170,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   171,
     172,   173,   174,     0,     0,   175,     0,   210,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   211,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   212,   213,   214,   215,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   176,   177,
       0,     0,     0,     0,   178,   179,     0,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,     0,     0,     0,
       0,     0,     0,     0,     0,   190,   191,   192,     0,     0,
     193,     0,     0,     0,     0,     0,     0,     0,   194,     0,
       0,     0,     0,     0,     0,     0,   195,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   216,   217,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   218,   219,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   196,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   248,   249,     0,   250,     0,     0,     0,     0,     0,
     251,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   197,  1553,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
       0,   172,     0,     0,     0,     0,   175,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   208,   209,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  1554,
    1555,  1556,  1557,  1558,  1559,  1560,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   210,     0,     0,     0,
       0,     0,  1561,     0,     0,  1562,  1563,     0,     0,     0,
       0,   211,     0,     0,     0,   178,   179,     0,   180,   181,
     182,   183,   184,   185,   186,   187,   188,     0,     0,     0,
     212,   213,   214,   215,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1564,     0,  1565,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   216,   217,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   218,   219,     0,
       0,     0,     0,   196,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     248,   249,     0,   250,     0,     0,     0,     0,     0,   251,
       0,     0,     0,   197,     0,  1553,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,     0,     0,   172,     0,     0,     0,     0,   175,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   208,   209,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  1554,  1555,  1556,  1557,  1558,  1559,  1560,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   210,   346,   347,
     348,   349,   350,   351,  1561,     0,     0,  1562,  1563,     0,
       0,     0,   211,     0,     0,     0,     0,   178,   179,     0,
     180,   181,   182,   183,   184,   185,   186,   187,   188,     0,
       0,   212,   213,   214,   215,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  1564,     0,  1565,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   216,   217,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   218,   219,
       0,     0,     0,     0,     0,   196,     0,     0,     0,     0,
       0,     0,     0,     0,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   248,   249,     0,     0,     0,     0,  1566,  1567,     0,
       0,     0,     0,     0,     0,   197,  1553,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,     0,   172,     0,     0,     0,     0,   175,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     208,   209,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,  1554,  1555,  1556,  1557,  1558,  1559,  1560,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   210,
       0,     0,     0,     0,     0,  1561,     0,     0,  1562,  1563,
       0,     0,     0,     0,   211,     0,     0,     0,   178,   179,
       0,   180,   181,   182,   183,   184,   185,   186,   187,   188,
       0,     0,     0,   212,   213,   214,   215,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1564,     0,  1565,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   216,   217,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     218,   219,     0,     0,     0,     0,   196,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     244,   245,   246,   247,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  2106,
       0,     0,     0,   248,   249,     0,     0,     0,     0,  1566,
       0,     0,     0,     0,     0,     0,   197,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   172,     0,     0,
       0,     0,   175,     0,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   208,   209,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     210,   178,   179,     0,   180,   181,   182,   183,   184,   185,
     186,   187,   188,     0,     0,   211,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   212,   213,   214,   215,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   216,   217,     0,     0,   196,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   218,   219,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   246,   247,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   197,
       0,     0,     0,     0,   248,   249,     0,     0,     0,     0,
    1566,     0,     0,     0,     0,     0,     0,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,   209,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   210,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   211,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   212,   213,   214,
     215,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  2289,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   415,   416,     0,     0,     0,     0,   216,   217,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   218,   219,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   417,     0,     0,     0,     0,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   418,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   248,   249,     0,
       0,     0,     0,     0,     0,     0,   419,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   398,     0,
       0,  2158,  2159,   399,   400,     0,   420,   421,     0,     0,
       0,   402,   422,     0,     0,     0,     0,   423,   424,   425,
     426,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     427,     0,  2161,     0,   428,     0,   429,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    2290,     0,     0,     0,  2291,  2292,     0,     0,  2293,  2294,
       0,     0,     0,     0,     0,     0,   403,     0,     0,   404,
       0,     0,     0,     0,     0,     0,     0,     0,   430,     0,
       0,     0,   431,     0,     0,  2295,     0,     0,     0,  2296,
       0,   432,     0,     0,  2297,     0,     0,  2298,     0,     0,
       0,     0,     0,  2299,     0,     0,     0,     0,  2300,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   433,   434,   435,   436,     0,   437,     0,     0,     0,
     438,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   439,
     440,     0,   441,   442,     0,     0,   443,   444,     0,     0,
       0,  2301,     0,     0,     0,     0,     0,     0,     0,   196,
       0,     0,   445,   446,     0,     0,     0,   447,   448,   449,
       0,     0,     0,     0,     0,     0,   450,  2302,     0,     0,
     451,     0,     0,   452,   453,     0,   454,     0,   455,     0,
       0,   456,   457,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   458,     0,     0,     0,     0,     0,     0,
       0,   459,   460,     0,  2303,     0,     0,  2304,  2305,  2306,
       0,   461,     0,     0,     0,  2307,     0,     0,     0,   197,
    2308,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    2309,     0,     0,     0,     0,     0,  2310,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   208,   209,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   210,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   211,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   212,   213,   214,
     215,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,  2311,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   462,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   216,   217,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   218,   219,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   248,   249
};

static const yytype_int16 yycheck[] =
{
     102,   165,     7,   251,   436,   437,   251,    88,    81,   165,
    1256,   566,    85,    86,   375,  1256,   139,   160,  1288,  1289,
    1290,  1291,  1256,   281,   471,   133,   149,   150,   743,   137,
     501,   139,  1256,   141,   284,   791,    45,    17,    18,   641,
      20,   149,   150,    39,   541,     7,   543,   544,   156,     7,
     547,   548,   128,     7,   130,     7,   132,     7,   134,     7,
     136,     7,   138,   795,    61,   315,   142,   162,   144,    15,
     146,   301,   148,   641,    20,  2259,     3,    22,   154,   155,
     682,     6,    87,    15,    30,   122,    32,   251,    20,   513,
      15,    16,    17,    18,    25,   251,    11,    12,    30,   249,
      32,   359,   360,   123,    15,    16,    17,    18,    19,    20,
      37,    45,  1358,  1518,     8,     9,    10,     0,    11,    12,
      47,    15,    49,    50,  1358,  1555,    20,    17,    18,    47,
      20,    25,    26,    27,  1358,     8,     9,    10,    11,    12,
    1570,    18,   400,    20,    30,    31,    17,    18,    75,    20,
     137,  2102,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    48,    15,    36,    17,   170,   162,    20,     8,     9,
      10,    11,    12,    15,    39,  2102,    17,    18,    20,    20,
       8,     9,    10,    11,    12,    25,    26,    27,    28,    29,
      30,    31,    32,    33,     4,    48,    36,    25,    26,    27,
      28,    29,    30,    31,    32,    33,   123,    39,    36,   474,
     167,    15,   106,   107,  1804,    31,    20,   641,    15,   146,
      17,   148,   149,    20,   151,    20,   153,    22,   155,    47,
     157,   158,   159,   242,   161,    47,    15,    16,    17,    18,
      19,    20,     3,     3,  1042,   250,   203,   174,   175,   176,
     177,   178,   179,    15,    16,    17,    18,    47,   361,    15,
      16,    17,    18,     8,     9,    10,    11,    12,    82,    15,
     372,   374,   375,     3,    20,   197,   198,    63,    64,   352,
      25,    26,    27,    28,    29,    30,    31,    32,    33,  1087,
      22,    36,   315,   740,   216,   163,    34,   162,   546,   226,
     402,   403,    20,   376,    15,   232,  2220,   194,   242,    20,
     575,   238,   239,    15,    16,    17,    18,    19,    20,   167,
     875,     8,     9,    10,    11,    12,   428,   759,    39,   145,
     162,    15,   435,   888,   239,  2102,    20,   494,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    15,   364,    36,
    2292,   501,    20,   714,   457,     8,     9,    10,    11,    12,
       3,   143,   144,    15,    16,    17,    18,    19,    20,   167,
     272,   474,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    17,    18,    36,    20,    15,     3,    20,   404,  1104,
      20,  1637,    15,  2577,    15,   552,   499,    20,   541,    20,
     543,   544,  1057,  1637,   547,   548,   432,   394,    38,    39,
      40,    41,   438,  1637,   644,   645,   646,   647,   648,   522,
     650,   651,  2364,   653,   654,   270,   656,   657,   925,     8,
       9,    10,    11,    12,    48,  1840,   473,   474,   461,   328,
     477,   478,  2393,   361,   162,   231,    25,    26,    27,    28,
      29,    30,    31,    32,    33,   303,   476,    36,   363,    15,
     892,    20,   894,    22,    20,    15,  2393,    22,   250,   251,
      20,   428,   254,  1041,     8,     9,    10,    11,    12,   574,
    2394,   349,    38,    39,    40,    41,   186,   285,   239,   850,
     851,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      45,   233,    36,     8,     9,    10,    11,    12,    15,   270,
      48,    15,   298,    20,   511,  2105,    20,  1249,  2108,   167,
      25,    26,    27,    28,    29,    30,    31,    32,    33,   162,
     214,    36,   377,  1088,    38,    39,    40,    41,   575,    20,
     545,    22,  1972,   361,   282,   502,   791,     3,   306,   361,
     467,   309,   182,   558,    17,    18,   360,    20,   447,   476,
    1806,   570,   567,   568,   569,   570,   571,   572,    15,  1833,
     643,   361,  1806,    20,  1798,  1839,     3,   391,   574,   584,
     194,  1851,  1806,  1853,    20,   688,  1856,   432,  1858,  1859,
     467,    38,    39,    40,    41,   575,   601,   602,   603,   604,
     605,   606,   607,   608,   609,   610,   611,   612,   613,   614,
     572,   714,   363,   156,  1216,   573,   719,  1041,   572,   573,
     572,   724,   572,   726,   572,   571,  2393,   791,  2490,   574,
     428,   577,  2426,   579,   561,   791,   570,   285,   560,   571,
     571,   308,    20,   570,    22,   577,   571,   579,   315,    47,
     575,     8,     9,    10,    11,    12,   194,   171,   573,   735,
     571,   764,   765,    31,   214,  1320,  1321,   578,    25,    26,
      27,    28,    29,    30,    31,    32,    33,   571,   180,    36,
     573,   137,    15,   239,   574,   690,  1484,    20,   574,   435,
     436,   361,     3,   248,   573,   798,   701,   242,   801,   578,
      20,   574,    22,   708,   575,    38,    39,    40,    41,   574,
     242,   959,   960,   227,   269,    15,   197,   962,   571,   883,
      20,   823,   120,    20,   122,    22,   162,   883,   570,   571,
     575,  1977,   572,   573,   575,   371,  1977,   840,    38,    39,
      40,    41,   574,  1977,   572,  1192,  1502,   850,   851,  1983,
     420,   283,    81,  1977,   549,     9,    10,    11,    12,  1983,
      15,   556,   865,   571,    18,    20,   869,   571,    45,  1266,
     873,    15,  1269,    45,   571,  2637,    20,   145,  2640,    20,
     428,    22,   925,    38,    39,    40,    41,   319,   320,  2583,
    2584,   169,   571,   575,    38,    39,    40,    41,   962,   578,
    2594,  2595,   905,    17,    18,   908,   962,    63,    64,   571,
     186,   434,   199,   916,   373,   571,  1066,  1067,  1068,  1069,
    1070,  1071,   367,   368,  1074,   571,  1076,   572,  1078,  1079,
       8,     9,    10,    11,    12,   146,   147,    20,    20,    22,
      22,    15,   162,  1073,   571,  1075,    20,    25,    26,    27,
      28,    29,    30,    31,    32,    33,   573,   862,    36,   170,
     571,   578,  2126,  2127,    38,    39,    40,    41,   476,   571,
    2134,  2135,  2142,    15,   482,    20,  1040,  2147,    20,  2149,
     362,   279,  2152,   194,  1040,   572,   573,   571,   275,  1336,
      15,   406,   407,  1340,  1341,    20,    38,    39,    40,    41,
     170,  2086,   570,   571,     8,     9,    10,    11,    12,   250,
     357,   244,   245,    38,    39,    40,    41,   399,   381,   571,
     573,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    1485,    82,    36,   574,   123,  1658,     8,     9,    10,    11,
      12,   571,   172,  1666,   949,   869,   951,   429,   571,   873,
     571,    33,   434,    25,    26,    27,    28,    29,    30,    31,
      32,    33,  1394,    33,    36,   242,   448,   575,   475,   199,
     242,    33,   161,  2219,   456,   231,   165,   166,  2219,   162,
     162,   170,   311,    63,    64,  2219,    48,  1089,   319,   320,
     472,  2176,   181,   475,   573,  2219,   478,  1468,   253,  2184,
    1102,   190,   557,   558,   559,    25,   488,   571,   240,   253,
     161,   340,  1114,  1136,  1116,   571,    36,  1098,   574,   164,
     349,   571,   573,  1738,   575,   570,    15,  1135,  1136,   167,
    2215,   285,   286,   312,  1137,  1138,   236,  1140,  1141,   573,
     331,   332,   298,  1766,   526,   275,   528,   163,  1151,    15,
    1773,    20,    21,    17,    20,    20,    20,    22,   174,   175,
     145,   340,   341,   201,   571,  1168,  1169,   571,   573,   188,
     574,   301,    38,    39,    40,    41,   376,  1180,  1181,    48,
     574,   476,  1185,  1186,   316,  1188,   481,  1190,   574,   158,
     367,   368,  1342,  1166,  1344,   367,   368,   571,    20,    21,
    1203,   377,   357,  1535,   573,   305,  1209,   573,  1216,   578,
     576,   507,   508,   357,   573,  1218,   575,   513,   571,  1222,
    1243,  1244,  1245,  1266,   571,   571,  1269,    32,    33,  1387,
     275,    36,   571,   333,  1384,  1243,  1244,  1245,   571,   104,
     416,  2411,  1534,  2413,  1308,  1537,   571,   285,   286,  1252,
     571,   231,  1308,     8,     9,    10,    11,    12,  1261,   144,
      17,    18,  1717,    20,   280,   441,  1721,   236,   237,   238,
      25,    26,    27,    28,    29,    30,    31,    32,    33,   575,
     575,    36,     8,     9,    10,    11,    12,   516,   517,   518,
     364,   365,   573,   312,   575,    17,    18,   162,    20,    25,
      26,    27,    28,    29,    30,    31,    32,    33,   144,   328,
      36,     8,     9,    10,    11,    12,   573,   336,   298,    17,
      18,    19,    20,     8,     9,    10,    11,    12,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    36,   571,    36,
      25,    26,    27,    28,    29,    30,    31,    32,    33,   143,
     144,    36,   377,  2017,    86,     3,  2020,  1502,    22,    15,
     225,     3,  2026,  2027,    20,    20,   748,    22,  1273,     3,
     143,   571,   165,   166,  1279,  1356,  1281,  1282,  1283,  1284,
     234,   235,    38,    39,    40,    41,  1359,     3,  1649,  1294,
      42,    43,    44,   570,   439,  1300,   778,   779,   570,     3,
     782,  1748,   289,   201,   202,   485,   486,   487,   453,   454,
     208,   209,   210,   211,  1417,   797,   571,   162,   191,   192,
     193,   803,   573,   805,   575,   807,   808,   571,   810,   811,
     812,   813,   814,   815,  1695,    20,   818,   819,  1502,   821,
      43,    44,    15,   825,   826,   146,  1502,    20,    15,   222,
    1514,   833,   834,     3,  1518,   837,   507,    17,    18,   841,
      20,    20,    22,    22,    23,    38,    39,    40,    41,     3,
     243,   172,     3,   246,   247,     3,  1479,    17,    18,    19,
      20,    17,    18,  1486,    20,  1487,   575,   383,   384,   385,
     386,   387,   874,   225,   195,   573,   197,   571,   199,   550,
     551,   552,   553,   554,   555,   887,  2676,    47,     3,    49,
      50,   284,   381,     3,   383,   384,   385,   386,   387,    20,
     902,    22,   904,  1526,    18,  2671,    20,     3,  1531,   571,
    2671,     3,    45,  1535,   573,    22,   575,  2671,    51,    52,
      53,    54,     3,    56,    57,    58,   571,  2671,    61,     3,
      63,    64,   184,   185,   186,   187,    69,    70,     3,   381,
    2730,   383,   384,   385,   386,   387,   494,    17,    18,   573,
      20,  1574,   273,   274,   275,   276,    25,   443,   203,   204,
     205,   206,   207,   134,   135,    20,  1918,   138,  1652,   245,
      20,   146,   147,   570,   145,   146,  1652,   570,   574,   572,
     572,     8,     9,    10,    11,    12,  1165,   162,  1167,   570,
    1871,  1872,   167,     8,     9,    10,    11,    12,    25,    26,
      27,    28,    29,    30,    31,    32,    33,   182,   570,    36,
      25,    26,    27,    28,    29,    30,    31,    32,    33,   240,
     318,    36,   320,   492,   143,  1648,  1649,    20,    21,  1554,
    1555,   383,  1557,   572,   573,    20,  1561,  1562,  1631,   391,
    1633,   393,  1994,   395,    48,  1570,   398,  1670,    65,    66,
      67,    68,   245,  2678,   572,   573,  2681,  2682,    48,     8,
       9,    10,    11,    12,    43,    44,   574,  1592,  2693,  1594,
      17,    18,  1695,  1696,   193,   249,    25,    26,    27,    28,
      29,    30,    31,    32,    33,   571,   574,    36,   255,   256,
      80,     8,     9,    10,    11,    12,  2721,  2722,   156,    92,
     570,  2726,  2727,   123,    22,  2057,   572,   573,    25,    26,
      27,    28,    29,    30,    31,    32,    33,  1740,  1741,    36,
     239,   570,  1745,   574,   243,   572,   573,   246,   247,  1752,
     399,   400,  1755,    17,    18,   155,    20,   570,    22,   574,
     160,   161,   574,  1145,  1146,   165,   166,   328,   168,   570,
     170,   244,   245,  1155,  1156,   317,  1840,  1780,  1834,  1782,
     362,   181,    59,    60,     3,   186,   156,  1790,    11,    12,
     190,     8,     9,    10,    11,    12,   572,   573,   572,   573,
     288,    20,   575,   244,   245,  2066,  2067,  2068,    25,    26,
      27,    28,    29,    30,    31,    32,    33,   572,   249,    36,
    1202,  1802,   306,   223,   194,   253,   226,   253,  1831,  1211,
     200,   191,   192,   291,  1837,   315,    45,   203,   204,   331,
      59,   157,    51,    52,    53,    54,   572,    56,    57,    58,
     220,   337,    61,   344,    63,    64,  1238,   171,   172,   173,
      69,    70,   203,   204,   205,   206,   207,   291,  1871,  1872,
     291,  1253,  1254,   295,   296,   572,     8,     9,    10,    11,
      12,   195,   196,   203,   204,   199,   352,   572,  1890,  1794,
     174,   175,   139,    25,    26,    27,    28,    29,    30,    31,
      32,    33,  2163,  1808,    36,  1810,     8,     9,    10,    11,
      12,    80,  2246,  2247,   350,   571,  1889,   372,   382,   288,
     570,   574,   452,    25,    26,    27,    28,    29,    30,    31,
      32,    33,   570,   574,    36,   574,   155,  1842,  2083,  2084,
     186,   155,   570,     8,     9,    10,    11,    12,   156,   368,
     361,   574,   574,    15,  2102,    15,   570,   570,   182,  1962,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    20,
     224,    36,   170,   570,   570,   574,   574,   249,   170,   146,
     147,   574,   574,   383,   570,   574,   155,   570,   155,   186,
     170,   160,   161,   160,   161,   164,  1378,  1379,   571,   168,
     356,   168,   244,   170,   253,   570,   570,  1980,   381,  2012,
     383,   384,   385,   386,   387,  2018,   146,   147,   574,  2083,
    2084,   574,     8,     9,    10,    11,    12,  2083,  2084,   570,
     249,   253,   162,    14,  1416,    15,    15,   167,  2370,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    33,    31,
      36,    15,   182,    30,   223,   573,   223,   226,   188,   226,
      36,     7,   575,  2066,  2067,  2068,   155,  1972,    15,     3,
      15,   160,   161,     3,  2077,   164,   575,  1982,     3,   168,
       3,     3,     8,     9,    10,    11,    12,     3,     3,   256,
       3,     3,     3,   312,  1476,  1477,  1478,     3,  1480,    25,
      26,    27,    28,    29,    30,    31,    32,    33,     3,     3,
      36,   280,   281,     3,   571,  1497,  2119,  1499,   287,  2122,
     573,  2124,  1504,   570,   570,   573,   570,   372,     8,     9,
      10,    11,    12,    20,   223,   253,   253,   226,   253,   253,
     342,   570,  2047,  1525,   227,    25,    26,    27,    28,    29,
      30,    31,    32,    33,  2415,  2416,    36,   570,  2419,   574,
    2163,  2164,     8,     9,    10,   572,   570,   570,  2170,    34,
     369,   390,   570,   253,   570,   575,   253,   572,   253,    25,
      26,    27,   253,   156,  2445,  2446,   242,   570,  2191,   570,
     570,   182,  2195,   412,  2197,  2198,   242,  2200,     8,     9,
      10,    11,    12,    15,    15,    20,   189,  2112,  2113,  2114,
     311,  2116,   570,   297,    15,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    48,   444,    36,    15,   145,     8,
       9,    10,    11,    12,    31,   573,   403,   404,  2241,   573,
    2243,  2244,  2245,   572,  2392,  2393,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    15,   571,    36,   242,     3,
     106,   107,   146,   147,   242,  2597,  2598,    20,   170,   170,
      20,  2274,     3,  2337,  2277,   572,     3,   572,   162,  2282,
    2541,    45,  1664,   167,  2616,    20,   170,  2290,   570,    22,
      54,   570,   146,   147,   570,    59,  2299,  2300,   182,  2302,
     573,  2304,    31,  2306,   188,  2308,   183,   570,   162,   570,
    1692,   570,  2315,   167,   570,   186,   170,   253,  2223,   570,
       8,     9,    10,    47,  2327,  2328,   570,  2232,   182,  2332,
    2333,  1713,  1714,  1715,   570,   570,  1718,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    15,   182,    36,   570,
    2353,    18,   570,    31,  2357,   572,    31,   570,  2361,   570,
    1742,    15,   574,   570,   570,  2368,   575,   570,   573,   570,
     570,    15,    20,     3,  2377,     8,     9,    10,    11,    12,
     570,   570,   570,   570,  2386,   570,   570,   570,   570,  1771,
       3,   570,    25,    26,    27,    28,    29,    30,    31,    32,
      33,     3,   570,    36,   570,   570,   575,   570,   575,   570,
       3,     3,  2415,  2416,     3,    15,  2419,     3,   182,     3,
       3,     3,  2425,   572,   252,   573,   252,   191,   192,   570,
     570,    22,  2435,  2436,  2437,  2438,  2439,   288,   155,   197,
     572,   199,  2445,  2446,     8,     9,    10,    11,    12,   570,
     570,   573,   299,   356,   570,  2458,  2459,  2460,   222,   570,
     570,    25,    26,    27,    28,    29,    30,    31,    32,    33,
     572,   229,    36,  2476,  2477,   570,   565,   570,   236,  2482,
     244,   245,   570,   570,   248,   570,   575,   362,    86,   570,
    2493,   570,  2397,   170,  2497,   186,   570,  2500,  2501,    54,
     570,   570,  2505,  2506,   570,   339,   570,   572,   170,   570,
    2513,   269,  1894,   271,   134,   570,   570,   570,   570,   249,
     284,   279,  2525,  2526,  2527,   570,   445,   172,   292,   293,
     294,   570,   290,   570,   144,  2538,   188,   570,  2541,  2542,
      45,   305,  2545,  2546,    33,   143,   144,  1929,   571,   571,
     571,   570,    25,  1935,    33,     8,     9,    10,    11,    12,
      31,   573,   573,    15,    18,     3,     3,  2570,  2571,   575,
     570,   169,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    15,  1964,    36,   574,   253,   572,     8,     9,    10,
      11,    12,    47,   253,    49,    50,   156,   182,   401,     8,
       9,    10,    11,    12,    25,    26,    27,    28,    29,    30,
      31,    32,    33,   397,   396,    36,    25,    26,    27,    28,
      29,    30,    31,    32,    33,   299,  2629,    36,  2631,  2632,
     570,    86,   570,   570,   245,   570,  2639,   313,   236,   182,
      63,   574,   574,  2646,    45,   574,   572,   405,    15,    15,
     574,   249,   570,   574,  2559,   575,   570,    31,    55,    62,
     570,   570,   191,   192,    35,   570,  2668,  2669,    32,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    32,    93,
      94,    95,    96,    97,    98,     7,   191,   192,   193,   575,
      25,   572,   572,   222,  2076,   283,   574,   570,   392,   273,
     155,   246,   246,   390,   246,   160,   161,   253,   186,   164,
     574,   240,   574,   168,   574,   574,   572,   222,   104,   174,
     175,   176,   177,   178,   179,   420,   421,   422,   423,   424,
     425,   426,   427,   491,   571,   571,     7,    71,   571,   244,
     245,   570,   573,   248,   571,   570,    45,   280,    47,   572,
      49,    50,   572,   164,   572,   284,   572,   515,   169,   572,
     101,   570,   572,   571,   293,   570,    33,    15,   223,    32,
      32,   226,     7,     3,   279,   574,   245,   570,  2160,   284,
     574,   570,     7,   315,   570,   570,   291,   476,   571,   294,
      20,    21,  2174,   572,   476,   572,   246,   573,    13,   572,
      15,    16,    17,    18,    19,    20,   572,    22,  2190,   572,
    2192,   575,   573,   572,   246,    30,    31,    32,   572,   246,
      35,   201,  2204,   328,   299,  2207,  2208,   297,   246,   249,
     246,    54,    47,    48,    49,    50,    46,    47,    53,    49,
      50,   297,   570,   249,   449,    75,   351,   422,   574,   352,
     315,   120,   420,   461,   570,   570,   155,   461,   253,   270,
     466,   160,   161,   299,   462,   468,   464,   465,   283,   168,
     471,   469,   470,   283,   472,   174,   175,   176,   177,   178,
     179,   479,   480,    22,   570,   570,   570,   120,   417,   418,
     419,   106,   107,   108,   109,   110,   111,   112,   113,   297,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     253,   352,   417,   253,  2296,   253,   146,   253,   133,   134,
     135,  2303,   498,   138,   223,   504,    33,   226,   158,   159,
     500,   146,   570,   510,   570,   440,   509,   570,   570,   154,
     170,   570,   570,   570,   570,   155,    33,    22,   570,   304,
     160,   161,   575,   458,   575,   299,   450,   368,   168,   484,
     253,   246,  2344,   246,   174,   175,   176,   177,   178,   179,
     463,   253,   192,   182,    51,    52,   192,   575,   105,   573,
    2362,  1040,    59,   434,  2366,   572,   434,   429,  1546,   494,
     495,   496,   497,   573,   499,  2123,  1040,  1040,   503,   570,
     572,   570,  1586,   414,   570,   570,   570,   570,    85,   308,
      87,   570,   570,   223,   570,   230,   226,   570,   570,  1837,
     431,  2124,   433,  1586,   479,   480,   437,  2122,   483,   239,
     441,   442,  1838,   488,  1586,  1839,  1357,   114,  1586,   565,
    1233,  1806,   341,   342,  2392,   250,   575,  1637,   125,   126,
    2393,   128,   129,   130,   131,   132,   960,   141,    47,  2441,
      49,    50,   139,   140,   141,   546,   143,   144,   959,   962,
     575,  1214,  1385,   845,   482,  1301,  1303,  2577,  2596,   636,
    2462,  1393,  1179,  1886,  2052,   300,  1943,  2469,  1904,  1917,
    2472,  2080,  2498,    47,  2337,    49,    50,  2479,  2480,  2354,
    2509,  2483,  2358,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,  2511,  1786,  2357,  1119,   570,   899,  1082,
     575,    -1,  2504,   685,    -1,    -1,  2508,    -1,  2510,    -1,
     345,   346,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  2529,  2530,   570,
      -1,   381,    -1,   383,   384,   385,   386,   387,  2540,   374,
     574,   570,    -1,    -1,   575,  2547,   155,  2549,  2550,  2551,
      -1,   160,   161,    -1,   389,    -1,    -1,    -1,    -1,   168,
      -1,    -1,    -1,    -1,    -1,   174,   175,   176,   177,   178,
     179,    -1,    -1,   408,   409,   410,   411,    -1,    -1,    -1,
      -1,   155,   412,   413,   414,   415,   160,   161,   507,    -1,
      -1,    -1,    -1,   512,   168,   514,    -1,    -1,    -1,    -1,
     174,   175,   176,   177,   178,   179,  2608,    -1,    -1,    -1,
     440,    -1,    -1,  2615,   223,   445,    -1,   226,   448,    -1,
      -1,   451,    -1,    -1,  2626,    -1,    -1,    -1,    -1,    -1,
     239,    83,    84,    85,    86,    87,    88,    89,    90,    91,
    2642,    93,    94,    95,    96,    97,    98,    -1,    -1,   223,
      -1,    -1,   226,    -1,   489,   490,   575,    47,    -1,    49,
      50,    -1,   236,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     505,   506,    -1,    -1,   248,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   291,    -1,    -1,    -1,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    13,    -1,    15,    16,    17,    18,    19,    20,    -1,
      22,    -1,    -1,   568,   569,    -1,   571,    -1,    30,    31,
      -1,    -1,   577,    35,    -1,   575,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,    -1,
      47,    53,    49,    50,    -1,   155,    -1,    -1,    -1,    -1,
     160,   161,    -1,   347,    -1,    -1,    -1,    -1,   168,    -1,
      -1,    72,    73,    74,   174,   175,   176,   177,   178,   179,
      -1,    47,    -1,    49,    50,    -1,    -1,    -1,   188,    47,
      -1,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   106,   107,   108,   109,   110,   111,
     112,   113,    -1,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   223,    -1,    -1,   226,    -1,    -1,   448,
      -1,   133,   134,   135,    -1,    -1,   138,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   146,    -1,    -1,   148,    -1,    -1,
      -1,    -1,   154,    -1,    -1,   255,    -1,    -1,   155,    -1,
     161,    -1,    -1,   160,   161,    -1,    -1,   164,    -1,    -1,
      -1,   168,    -1,    -1,    -1,    -1,   276,   174,   175,   176,
     177,   178,   179,    -1,    -1,   186,   187,    -1,    -1,   155,
      -1,    -1,    -1,    -1,   160,   161,    -1,    -1,   164,    -1,
     519,   520,   168,    -1,    -1,    -1,    -1,    -1,   174,   175,
     176,   177,   178,   179,    -1,    -1,   174,   175,   176,   177,
     178,   179,    -1,   224,    -1,   226,   223,   228,   230,   226,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   239,    -1,
      -1,    -1,    -1,    -1,    -1,   564,   565,   566,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   575,   223,    -1,    -1,
     226,    -1,    -1,    -1,    -1,    -1,    -1,   268,    -1,    -1,
      -1,   272,    -1,    -1,    -1,    -1,   277,   278,    -1,    -1,
      -1,    -1,   248,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   575,    -1,    -1,    -1,    -1,    -1,    -1,   300,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
      -1,    -1,    -1,    -1,    -1,    -1,   318,   319,   320,   321,
     322,   323,   324,   325,   326,   327,    -1,    -1,    -1,    -1,
      13,    -1,    15,    16,    17,    18,    19,    20,    -1,    22,
      -1,    -1,    -1,   345,   346,    -1,    -1,    30,    31,    32,
      -1,    -1,    35,    -1,    -1,    -1,   456,   457,   458,    -1,
      -1,    -1,    -1,    -1,    47,    48,    49,    50,    -1,    -1,
      53,    -1,   374,    -1,     8,     9,    10,    -1,    47,    -1,
      49,    50,    -1,    -1,   163,    -1,    -1,   389,    -1,    -1,
      -1,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      -1,   402,    36,    -1,    -1,    -1,   408,   409,   410,   411,
      -1,   190,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   106,   107,    -1,    -1,    -1,    -1,   112,
     113,    -1,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   221,   440,    -1,    -1,    -1,    -1,    -1,    -1,
     133,   134,   135,    -1,    -1,   138,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   146,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   154,    -1,    -1,    -1,   575,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   155,   489,   490,    -1,
      -1,   160,   161,    -1,    -1,   164,   493,    -1,    -1,   168,
     279,    -1,    -1,   505,   506,   174,   175,   176,   177,   178,
     179,   290,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   521,
     522,   523,   524,   525,   526,   527,   528,   529,   530,   531,
     532,   533,   534,   535,   536,   537,   538,   539,   540,   541,
     542,   543,   544,   545,   546,   547,   548,   230,    -1,    -1,
      -1,    -1,    -1,    -1,   223,    -1,    -1,   226,    -1,    -1,
      -1,   562,   563,    -1,    -1,    -1,   568,   569,    -1,   571,
      -1,    -1,    -1,    -1,    -1,   577,    -1,    -1,   575,    -1,
       8,     9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    -1,    -1,    36,   575,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   300,    -1,    -1,
      -1,    -1,    -1,   402,    -1,    -1,   405,    -1,    -1,    -1,
      -1,    -1,    -1,   302,    -1,   318,   319,   320,   321,   322,
     323,   324,   325,   326,   327,    -1,    -1,    -1,    -1,    13,
      -1,    15,    16,    17,    18,    19,    20,    -1,    22,    -1,
      -1,    -1,   345,   346,    -1,    -1,    30,    31,    32,    -1,
      -1,    35,   100,    -1,    -1,    -1,   455,    -1,    -1,    -1,
     459,   460,    -1,    47,    48,    49,    50,    -1,    -1,    53,
      -1,   374,    -1,    -1,    -1,    -1,     8,     9,    10,    11,
      12,    -1,    -1,    -1,    -1,    -1,   389,    -1,    -1,    -1,
      -1,    -1,   491,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    -1,    -1,    36,   408,   409,   410,   411,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   515,    -1,    -1,    -1,
     519,    -1,   106,   107,    -1,    -1,    -1,    -1,   112,   113,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   133,
     134,   135,     3,    -1,   138,    -1,    -1,     8,     9,    10,
      11,    12,   146,    -1,    -1,    -1,    -1,    -1,   567,    -1,
     154,    -1,    -1,    -1,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    -1,    -1,    36,   489,   490,    -1,    -1,
      -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   505,   506,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,   521,   522,
     523,   524,   525,   526,   527,   528,   529,   530,   531,   532,
     533,   534,   535,   536,   537,   538,   539,   540,   541,   542,
     543,   544,   545,   546,   547,   548,   230,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   123,    -1,    -1,    -1,   568,   569,    -1,   571,    -1,
      -1,    -1,   134,    -1,   577,    -1,    -1,    83,    84,    85,
      86,    87,    88,    89,    90,    91,   575,    93,    94,    95,
      96,    97,    98,    -1,    -1,    -1,    -1,    -1,    -1,   161,
      -1,    -1,    -1,   165,   166,    -1,    -1,    -1,   170,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   300,    -1,    -1,   181,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   190,    -1,
      -1,    -1,    -1,    -1,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,    -1,    -1,    -1,    -1,    13,    -1,
      15,    16,    17,    18,    19,    20,    -1,    22,    -1,    -1,
      -1,   345,   346,    -1,    -1,    30,    31,    32,    -1,    -1,
      35,    -1,    -1,   235,    -1,   237,    -1,   239,    -1,    -1,
      -1,    -1,    47,    48,    49,    50,    -1,    -1,    53,    -1,
     374,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   389,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   408,   409,   410,   411,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   106,   107,   305,    -1,    -1,    -1,   112,   113,    -1,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   133,   134,
     135,    -1,    -1,   138,    -1,    -1,   338,    -1,    -1,    -1,
      -1,   146,     3,    -1,    -1,   347,   348,    -1,    -1,   154,
      -1,    -1,   354,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    -1,    -1,   366,   489,   490,    -1,    -1,    -1,
      -1,    -1,    -1,   375,    -1,   377,    -1,    -1,   380,    -1,
      -1,   505,   506,    -1,    -1,    -1,    47,    -1,    49,    50,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   521,   522,   523,
     524,   525,   526,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,   540,   541,   542,   543,
     544,   545,   546,   547,   548,   230,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   103,   104,   568,   569,    -1,   571,    -1,    -1,
      -1,    -1,    -1,   577,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   146,   147,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   300,    -1,   158,    -1,    -1,
      -1,   162,    -1,    -1,   165,   166,    -1,    -1,    -1,   170,
      -1,    -1,    -1,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,    -1,    -1,    -1,    -1,    13,   189,    15,
      16,    17,    18,    19,    20,    -1,    22,    -1,    -1,    -1,
     345,   346,    -1,    -1,    30,    31,    32,    -1,    -1,    35,
      -1,   212,   213,   214,    -1,    -1,   217,   218,   219,    -1,
      -1,    47,    48,    49,    50,    -1,    -1,    53,    -1,   374,
      -1,    -1,    -1,   575,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   389,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   408,   409,   410,   411,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     106,   107,    -1,    -1,    -1,    -1,   112,   113,    -1,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   133,   134,   135,
      -1,    -1,   138,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     146,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   154,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   489,   490,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     505,   506,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   230,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   568,   569,    -1,   571,    -1,    -1,    -1,
      -1,    -1,   577,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   300,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   318,   319,   320,   321,   322,   323,   324,   325,
     326,   327,    -1,    -1,    -1,    -1,    13,    -1,    15,    16,
      17,    18,    19,    20,    -1,    22,    -1,    -1,    -1,   345,
     346,    -1,    -1,    30,    31,    32,    -1,    -1,    35,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    49,    50,    -1,    -1,    53,    -1,   374,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   389,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   408,   409,   410,   411,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,
     107,    -1,    -1,    -1,    -1,   112,   113,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   133,   134,   135,    -1,
      -1,   138,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   146,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   154,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   489,   490,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   505,
     506,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   521,   522,   523,   524,   525,
     526,   527,   528,   529,   530,   531,   532,   533,   534,   535,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   548,   230,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   568,   569,    -1,   571,    -1,    -1,    -1,    -1,
      -1,   577,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   300,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,    -1,    -1,    -1,    -1,    13,    -1,    15,    16,    17,
      18,    19,    20,    -1,    22,    -1,    -1,    -1,   345,   346,
      -1,    -1,    30,    31,    -1,    -1,    -1,    35,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,
      48,    49,    50,    -1,    -1,    53,    -1,   374,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   389,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   408,   409,   410,   411,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   106,   107,
      -1,    -1,    -1,    -1,   112,   113,    -1,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   133,   134,   135,    -1,    -1,
     138,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   146,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   154,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   489,   490,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   505,   506,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   521,   522,   523,   524,   525,   526,
     527,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   538,   539,   540,   541,   542,   543,   544,   545,   546,
     547,   548,   230,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   568,   569,    -1,   571,    -1,    -1,    -1,    -1,    -1,
     577,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   300,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
      -1,    48,    -1,    -1,    -1,    -1,    53,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   345,   346,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      77,    78,    79,    80,    81,    82,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   374,    -1,    -1,    -1,
      -1,    -1,    99,    -1,    -1,   102,   103,    -1,    -1,    -1,
      -1,   389,    -1,    -1,    -1,   112,   113,    -1,   115,   116,
     117,   118,   119,   120,   121,   122,   123,    -1,    -1,    -1,
     408,   409,   410,   411,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   150,    -1,   152,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   489,   490,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   505,   506,    -1,
      -1,    -1,    -1,   230,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   521,   522,   523,   524,   525,   526,   527,
     528,   529,   530,   531,   532,   533,   534,   535,   536,   537,
     538,   539,   540,   541,   542,   543,   544,   545,   546,   547,
     548,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     568,   569,    -1,   571,    -1,    -1,    -1,    -1,    -1,   577,
      -1,    -1,    -1,   300,    -1,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   318,   319,   320,   321,   322,   323,   324,   325,   326,
     327,    -1,    -1,    48,    -1,    -1,    -1,    -1,    53,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   345,   346,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,   374,    93,    94,
      95,    96,    97,    98,    99,    -1,    -1,   102,   103,    -1,
      -1,    -1,   389,    -1,    -1,    -1,    -1,   112,   113,    -1,
     115,   116,   117,   118,   119,   120,   121,   122,   123,    -1,
      -1,   408,   409,   410,   411,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   150,    -1,   152,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   489,   490,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   505,   506,
      -1,    -1,    -1,    -1,    -1,   230,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   521,   522,   523,   524,   525,   526,
     527,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   538,   539,   540,   541,   542,   543,   544,   545,   546,
     547,   548,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   568,   569,    -1,    -1,    -1,    -1,   574,   575,    -1,
      -1,    -1,    -1,    -1,    -1,   300,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,    -1,    48,    -1,    -1,    -1,    -1,    53,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     345,   346,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    76,    77,    78,    79,    80,    81,    82,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   374,
      -1,    -1,    -1,    -1,    -1,    99,    -1,    -1,   102,   103,
      -1,    -1,    -1,    -1,   389,    -1,    -1,    -1,   112,   113,
      -1,   115,   116,   117,   118,   119,   120,   121,   122,   123,
      -1,    -1,    -1,   408,   409,   410,   411,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   150,    -1,   152,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   489,   490,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     505,   506,    -1,    -1,    -1,    -1,   230,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
      -1,    -1,    -1,   568,   569,    -1,    -1,    -1,    -1,   574,
      -1,    -1,    -1,    -1,    -1,    -1,   300,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    -1,    53,    -1,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   345,   346,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     374,   112,   113,    -1,   115,   116,   117,   118,   119,   120,
     121,   122,   123,    -1,    -1,   389,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   408,   409,   410,   411,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   489,   490,    -1,    -1,   230,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   505,   506,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   521,   522,   523,
     524,   525,   526,   527,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,   540,   541,   542,   543,
     544,   545,   546,   547,   548,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   300,
      -1,    -1,    -1,    -1,   568,   569,    -1,    -1,    -1,    -1,
     574,    -1,    -1,    -1,    -1,    -1,    -1,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   345,   346,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   374,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   389,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   408,   409,   410,
     411,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    49,    50,    -1,    -1,    -1,    -1,   489,   490,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   505,   506,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    -1,    -1,    -1,
     521,   522,   523,   524,   525,   526,   527,   528,   529,   530,
     531,   532,   533,   534,   535,   536,   537,   538,   539,   540,
     541,   542,   543,   544,   545,   546,   547,   548,   115,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   568,   569,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   143,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   155,    -1,
      -1,   191,   192,   160,   161,    -1,   163,   164,    -1,    -1,
      -1,   168,   169,    -1,    -1,    -1,    -1,   174,   175,   176,
     177,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     187,    -1,   222,    -1,   191,    -1,   193,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     240,    -1,    -1,    -1,   244,   245,    -1,    -1,   248,   249,
      -1,    -1,    -1,    -1,    -1,    -1,   223,    -1,    -1,   226,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   235,    -1,
      -1,    -1,   239,    -1,    -1,   275,    -1,    -1,    -1,   279,
      -1,   248,    -1,    -1,   284,    -1,    -1,   287,    -1,    -1,
      -1,    -1,    -1,   293,    -1,    -1,    -1,    -1,   298,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   278,   279,   280,   281,    -1,   283,    -1,    -1,    -1,
     287,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   306,
     307,    -1,   309,   310,    -1,    -1,   313,   314,    -1,    -1,
      -1,   351,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   230,
      -1,    -1,   329,   330,    -1,    -1,    -1,   334,   335,   336,
      -1,    -1,    -1,    -1,    -1,    -1,   343,   377,    -1,    -1,
     347,    -1,    -1,   350,   351,    -1,   353,    -1,   355,    -1,
      -1,   358,   359,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   370,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   378,   379,    -1,   414,    -1,    -1,   417,   418,   419,
      -1,   388,    -1,    -1,    -1,   425,    -1,    -1,    -1,   300,
     430,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     440,    -1,    -1,    -1,    -1,    -1,   446,   318,   319,   320,
     321,   322,   323,   324,   325,   326,   327,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   345,   346,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   374,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   389,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   408,   409,   410,
     411,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   575,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   575,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   489,   490,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   505,   506,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     521,   522,   523,   524,   525,   526,   527,   528,   529,   530,
     531,   532,   533,   534,   535,   536,   537,   538,   539,   540,
     541,   542,   543,   544,   545,   546,   547,   548,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   568,   569
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     4,   581,   582,    72,    73,    74,   148,   161,   186,
     187,   224,   226,   228,   239,   268,   272,   277,   278,   402,
     562,   563,   638,   639,   640,   641,   763,   764,   765,   766,
     768,   769,   770,   777,   779,   780,   781,   782,   783,   784,
     785,   786,   787,   788,   789,   792,   793,   794,   795,   796,
     797,   798,   800,   801,   823,   834,   835,   836,   837,   861,
     899,   900,   901,   902,   903,   904,   905,   906,   961,   962,
     985,   998,   999,  1000,  1001,  1002,  1003,  1004,  1005,     0,
       3,    37,    47,    49,    50,    75,   146,   148,   149,   151,
     153,   155,   157,   158,   159,   161,   174,   175,   176,   177,
     178,   179,   226,   232,   238,   239,   561,   570,   583,   584,
     642,   644,   645,   646,   647,   657,   702,   704,   705,   706,
     707,   708,   713,   727,   730,   740,   741,   742,   805,   808,
     824,   827,   838,   845,   857,   860,   864,   865,   908,   909,
     911,   913,   914,   919,   950,   953,   963,   964,   976,   977,
     986,   989,   996,   997,  1008,  1011,  1012,     3,     3,     3,
      13,    15,    16,    17,    18,    19,    20,    22,    30,    31,
      35,    47,    48,    49,    50,    53,   106,   107,   112,   113,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     133,   134,   135,   138,   146,   154,   230,   300,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   345,   346,
     374,   389,   408,   409,   410,   411,   489,   490,   505,   506,
     521,   522,   523,   524,   525,   526,   527,   528,   529,   530,
     531,   532,   533,   534,   535,   536,   537,   538,   539,   540,
     541,   542,   543,   544,   545,   546,   547,   548,   568,   569,
     571,   577,   592,   593,   594,   595,   596,   597,   598,   599,
     600,   601,   602,   603,   635,   636,   637,   666,   682,   684,
     685,   687,   688,   689,   690,   804,    20,    22,   162,   776,
     799,   802,    20,   802,   802,   104,   225,   802,   802,     3,
     186,   197,   199,   229,   236,   269,   271,   279,   290,   405,
     491,   515,   146,   172,   195,   197,   199,   273,   274,   275,
     276,    39,   162,   574,   790,   802,   272,    22,   248,   269,
     557,   558,   559,  1006,     3,    20,    21,   381,   383,   384,
     385,   386,   387,   587,   587,   587,   690,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    93,    94,    95,    96,
      97,    98,   648,   571,   571,   156,    20,    22,   612,   197,
     198,   216,   560,   180,   197,   612,   158,   236,   237,   238,
     728,   729,   988,   186,   494,   552,   170,   250,   709,   163,
     190,   221,   279,   290,   402,   405,   455,   459,   460,   491,
     515,   519,   567,   863,   949,    48,   587,   714,   155,   160,
     161,   164,   168,   223,   226,   248,   575,   706,   715,   716,
     722,   744,   714,   714,   714,    49,    50,    82,   115,   143,
     163,   164,   169,   174,   175,   176,   177,   187,   191,   193,
     235,   239,   248,   278,   279,   280,   281,   283,   287,   306,
     307,   309,   310,   313,   314,   329,   330,   334,   335,   336,
     343,   347,   350,   351,   353,   355,   358,   359,   370,   378,
     379,   388,   575,   715,   848,   849,   856,   714,   714,    46,
     239,   412,   413,   414,   415,   440,   445,   448,   451,   575,
     706,   715,   867,   868,   869,   870,   871,   714,   188,   255,
     276,   456,   457,   458,   575,   715,   722,   910,   574,   123,
     165,   166,   170,   181,   190,   383,   575,   715,   717,   714,
     714,   714,   714,   236,   248,   347,   575,   715,   722,   980,
     981,   239,   291,   448,   519,   520,   564,   565,   566,   575,
     715,   722,   987,  1010,   714,   714,   164,   565,   575,   715,
    1013,    15,    20,    30,    32,   571,   577,   579,   681,   683,
     688,    33,   591,    33,    17,    18,   592,   593,   571,   145,
      31,   145,   690,    15,   574,   574,   571,   571,   571,   571,
     571,   571,   571,   108,   109,   110,   111,   671,   690,    15,
      16,    18,    19,    20,   571,   578,   592,   593,   600,   666,
     667,   670,   672,   673,   674,   676,   678,   679,   144,   144,
       3,     8,     9,    10,    11,    12,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    36,    36,    39,   162,   574,
     745,     3,   745,    22,   613,   613,   802,   802,     3,    39,
     162,   574,   748,     3,    20,    22,   162,   803,     3,   270,
       3,   771,     3,   778,   289,   172,   199,   275,   301,    20,
     164,   275,   439,   453,   454,   767,   199,   275,   767,   162,
       3,     3,     3,     3,     3,     3,     3,     3,    82,   161,
     507,   550,   551,   552,   553,   554,   555,   994,   995,    15,
     791,   613,   862,  1006,     3,  1006,     3,     3,   494,  1007,
      25,   658,   643,    92,   587,    20,    20,    15,    17,    20,
      48,   571,   703,   570,   745,   745,    15,    20,   571,   585,
      38,    39,    40,    41,   182,   585,   588,   748,   570,    22,
     233,   570,   549,   556,   612,   992,   240,   585,   586,   586,
     587,   574,   712,   406,   407,   912,   492,    20,    48,   574,
     156,   588,   745,   743,   169,   612,   612,   748,   249,   570,
     574,   720,   570,   570,   574,   574,   574,   240,   316,   328,
     306,   309,   570,    22,   319,   320,    47,   361,    47,   361,
      47,   361,    47,   361,   317,   612,   588,   362,   186,   249,
     236,   305,   333,   239,   588,   846,   570,   585,    17,    18,
      20,   371,   589,   840,   381,   589,   839,   288,   253,   308,
     315,   253,   306,   291,   315,   331,   332,   331,   157,   588,
     337,   344,   291,   291,   352,   350,   242,   283,   319,   320,
     588,   360,   585,   372,   139,   288,   382,   570,   574,   854,
     570,   574,   574,   452,   186,   703,   588,   155,   585,   588,
     156,   368,   588,   361,   866,   720,   570,   574,   574,   588,
     143,   144,    15,    48,   194,    15,   165,   166,   570,   570,
     585,   146,   147,   162,   167,   182,   188,   655,   724,   182,
     724,    20,   723,   182,   224,   170,   570,   570,   574,   574,
     574,   574,    18,    19,    20,   603,   607,   249,   170,   570,
     570,    81,   311,   340,   349,   516,   517,   518,   978,   979,
     574,   984,   186,   585,   356,   170,   244,   588,   253,   588,
     570,   570,   574,   574,  1014,  1018,   253,   570,     7,   683,
     683,   683,   690,    17,    18,   578,   676,   683,   683,    14,
      15,    15,    33,   686,   690,    30,    31,   145,    31,   632,
     632,   723,   690,   690,   690,   690,   690,   690,    15,   573,
     572,   573,    32,   669,   671,   690,    30,    31,   574,     7,
     573,   578,   573,   578,    15,    15,   690,   690,   690,   690,
     690,   690,   690,   690,   690,   690,   690,   690,   690,   690,
      51,    52,    59,    85,    87,   114,   125,   126,   128,   129,
     130,   131,   132,   139,   140,   141,   143,   144,    47,    49,
      50,   174,   175,   176,   177,   178,   179,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   775,   171,
     172,   173,   195,   196,   199,   746,   747,     3,     3,     3,
       3,   171,   227,   749,   750,     3,   803,     3,     3,     3,
     103,   104,   146,   158,   162,   165,   166,   170,   189,   212,
     213,   214,   217,   218,   219,   607,   655,   774,     3,    75,
     146,   158,   159,   170,   587,   767,   767,   767,   767,   767,
     767,   767,   907,   162,   767,   162,   767,     3,   767,   767,
       3,     3,   573,   575,     3,     3,     3,   146,   170,   194,
     655,     3,  1007,   585,     3,   690,   571,   650,   651,   652,
     656,   570,   573,   573,   737,   738,   690,   570,   690,   586,
     570,   585,   234,   235,   372,   990,   585,   991,   162,   574,
     993,   585,    42,    43,    44,   711,   714,    20,   703,   731,
     734,   735,   588,   719,   721,   807,   826,   253,   253,   589,
     253,   253,   570,   585,   585,    82,   391,   570,   570,   239,
     363,   253,   357,   588,   588,   342,   227,   588,   570,   570,
     574,   762,   570,    20,   600,   606,    34,   590,    45,   242,
     367,   368,   570,   383,   384,   385,   386,   387,   844,   590,
      45,   242,   570,   588,   585,   253,   253,   585,   253,   588,
     253,   588,   156,   588,   588,   570,   588,   588,   588,   588,
     588,   588,   369,   242,   588,   588,   570,   588,   570,   373,
     612,   242,   588,   588,   853,   855,   859,   588,   253,   357,
     588,   588,   182,   585,   588,   721,   872,   586,   586,    15,
      15,    20,   653,   654,   690,    39,   585,   726,   189,   726,
     726,   588,   723,   918,   952,   965,   588,   723,    22,   589,
     589,    47,   120,   122,   279,   978,   983,   570,   588,   588,
     585,   297,   585,   735,   736,   585,    15,   572,   683,   578,
      48,    15,   572,   573,   145,    31,   633,   634,   572,   573,
     572,   573,   573,   573,   573,   572,   690,   690,   573,     7,
     573,   573,   572,    15,   571,   667,   674,   678,   242,   242,
     571,   573,   575,   573,   575,     3,    17,    18,    19,    20,
     592,   593,   594,   595,   596,   772,   773,   607,   653,    20,
     170,   170,    20,   162,    20,   776,     3,   613,   613,   613,
     613,   613,   613,     3,    20,    59,   155,   249,   312,   390,
     412,   444,   767,   613,   767,   613,   613,   613,   995,   653,
      22,   723,   612,     3,   570,   572,   649,   659,   574,   648,
     612,    20,   735,   570,   572,   572,    22,    43,    44,    43,
      44,   612,   612,   994,   570,   570,   143,   144,   250,   251,
     254,   575,   710,    31,   733,   573,   570,   161,   575,   717,
     718,    80,   164,   280,   281,   287,   575,   715,   806,   164,
     302,   575,   715,   722,   825,   585,   585,   570,   585,   585,
     570,   570,   588,   574,   588,   841,   186,   253,   585,   570,
     570,   570,   588,   588,   570,   761,   847,   840,   587,   840,
     585,   585,   570,   839,   585,   585,   570,   570,   585,   585,
     570,   585,   570,   585,   570,   703,   570,   570,   570,   570,
     570,   570,   570,   570,   588,   570,   585,   570,   570,   570,
     585,   570,   588,   570,   570,    54,   134,   165,   166,   235,
     237,   239,   305,   338,   347,   348,   354,   366,   375,   377,
     380,   575,   718,   852,   146,   170,   256,   403,   404,   575,
     655,   715,   858,   585,   585,   164,   169,   270,   368,   414,
     431,   433,   437,   441,   442,   575,   873,   880,   883,   652,
     183,   725,   588,   570,    86,   164,   479,   480,   483,   488,
     575,   715,   722,   915,   164,   440,   493,   575,   715,   722,
     951,    45,   308,   341,   342,   507,   512,   514,   575,   715,
     722,   966,   968,   570,   570,   590,    47,    15,   609,   610,
     585,   588,   588,    20,    76,    77,    78,    79,    80,    81,
      82,    99,   102,   103,   150,   152,   574,   575,   687,   689,
     691,   696,   804,   982,   182,  1009,   585,   574,  1017,   683,
     683,    18,   690,    31,    31,   630,   631,   690,   690,   690,
     690,   690,   573,   572,   573,    32,   669,   690,   669,    32,
     669,   669,   573,   690,    15,    15,   690,   747,   750,    48,
      17,    18,    20,   776,   776,     3,     3,     3,     3,     3,
       3,    15,   703,   703,   703,   613,     3,   613,     3,     3,
       3,   648,   656,   648,   660,   663,   664,   665,   694,   696,
     587,   572,   573,   575,   252,   252,   588,   588,   255,   256,
     570,    22,   613,   732,   734,   745,   570,   810,   811,   817,
     811,   818,   589,   822,   288,   570,   828,   833,   155,   570,
     573,   570,   570,   570,   570,    86,   225,   383,   391,   393,
     395,   398,   842,   843,   570,   357,   588,   585,   570,   570,
     570,   143,   193,   239,   243,   246,   247,   575,   760,   570,
     570,   570,   570,   570,   570,   570,   570,   570,   570,   570,
     570,   570,   299,   356,   170,   170,   362,   170,   186,    54,
     339,   170,   163,   349,   588,   588,   376,   588,   585,   588,
     570,   653,    22,   723,   585,   612,   134,   570,   892,   898,
     377,   416,   441,   588,   875,   377,   588,   874,   249,   606,
     876,   877,   443,   588,   570,   445,   893,   184,   185,   186,
     187,    20,   593,   605,   929,   934,   920,   928,   916,   917,
     605,   172,   570,   954,   960,   588,   585,   570,   585,   188,
     967,   144,   967,    20,   589,   612,   969,    61,   511,   970,
     967,   570,   610,    33,    25,   690,   692,   692,   571,   690,
     690,   690,   697,   571,   571,   661,   662,   570,    25,   692,
      25,    36,   585,  1016,    33,    31,    45,    51,    52,    53,
      54,    56,    57,    58,    61,    63,    64,    69,    70,   575,
     616,   617,   618,   619,   620,   628,   575,   617,   618,   619,
     620,   629,   573,   572,   572,   572,   572,   690,   690,   572,
     573,     7,   572,   573,   572,   573,     7,   572,   573,   573,
      15,   572,    18,     3,     3,   587,   587,   575,   664,   570,
      15,   253,   253,   585,   586,    17,    18,    20,   592,   593,
     608,   163,   174,   175,   280,   809,   736,   574,   816,    34,
     282,   819,   588,   736,   156,   585,   182,   401,   397,   137,
     394,   396,   399,   400,   573,   575,   570,   570,   570,   244,
     245,   588,   245,   586,   585,   570,   188,   312,   328,   336,
     244,   245,   588,   588,   588,   239,   363,   723,   588,   299,
     312,   340,   341,   851,   723,   313,   735,   894,   585,   585,
     588,   574,   585,   574,   703,   574,   585,   585,   574,   574,
     933,   736,   574,   589,   935,   940,    22,   574,   941,   946,
     588,   736,   182,   585,    63,   585,   968,   971,   972,   585,
      15,   690,   573,   570,   570,   804,   570,   100,   574,    20,
     648,   695,   700,   700,   575,   660,   690,   570,   690,   134,
     135,   138,   145,   146,   474,   575,  1015,    15,    31,    55,
      59,    60,    62,     8,     9,    10,    25,    26,    27,   106,
     107,   585,   614,   621,   622,   623,    35,   624,   625,   625,
     627,    20,   592,   604,   570,   621,   624,   627,   605,   570,
     690,   572,   572,    32,   669,    32,   669,    32,   669,     7,
      32,   669,    32,   669,   575,   570,   572,    25,   693,   572,
     586,   586,   816,   815,   812,   587,   612,   283,   820,   574,
     832,   588,   570,   392,   273,   843,   246,   246,   246,   364,
     365,   588,   850,   850,   589,   390,   186,   253,   588,   588,
     895,   574,   897,   885,   885,   878,   881,   932,   931,   574,
     927,   936,   942,   574,   959,   585,   588,   574,   570,   692,
     572,   696,   680,   587,   104,   690,    20,   689,   701,   804,
     570,   570,   571,   571,   571,   570,   571,   589,   570,     7,
     585,    11,    12,   615,   573,    12,   573,   615,   585,    65,
      66,    67,    68,   626,   573,   615,    71,   572,   572,   572,
     572,   573,     7,   572,   572,   572,   573,   571,   572,     7,
     572,   572,     7,   572,   690,   570,   812,   143,   191,   192,
     193,   222,   243,   246,   247,   284,   575,   751,   752,   754,
     755,   756,   813,   589,   280,   821,   831,   829,   586,   586,
     586,   588,   585,   893,   896,   575,   606,   886,   887,   575,
     270,   377,   432,   575,   879,   240,   284,   293,   417,   418,
     419,   575,   751,   882,   122,   473,   474,   477,   478,   575,
     930,   926,   925,   939,   945,   958,   955,   973,   570,   101,
       6,    15,    16,   571,   575,   666,   668,   670,   672,   673,
     675,   677,   698,   700,   700,   690,   690,   690,   690,   585,
     623,   614,   622,   625,   625,    33,   625,   625,    15,    32,
     669,    32,   669,   669,     7,   669,   244,   245,   588,   574,
     245,   586,   585,   167,   201,   285,   286,   814,    23,   612,
     574,   759,   570,   588,    45,    54,    59,   182,   244,   245,
     248,   284,   292,   293,   294,   305,   575,   751,   830,    59,
     240,   244,   245,   248,   249,   275,   279,   284,   287,   293,
     298,   351,   377,   414,   417,   418,   419,   425,   430,   440,
     446,   575,   751,   884,   432,   438,   888,   588,   585,   588,
     570,   585,   167,   285,   428,   585,   585,   315,   214,   585,
     570,   588,   476,   476,   588,   588,   570,    86,   143,   144,
     169,   236,   249,   462,   464,   465,   469,   470,   472,   479,
     480,   575,   921,   476,   481,   575,   937,   476,   482,   575,
     943,    45,   193,   244,   245,   248,   279,   284,   291,   294,
     328,   351,   417,   440,   458,   494,   495,   496,   497,   499,
     503,   575,   751,   956,   507,   508,   513,   575,   974,   696,
     661,   690,     7,   573,   576,   690,   572,   573,   572,   572,
     572,   572,   585,   585,   585,   585,   626,   626,   572,   572,
     572,   573,   572,   571,   572,   246,   246,   753,   754,   246,
     203,   204,   201,    48,    80,   156,   194,   200,   220,   757,
     585,   299,   295,   296,   585,   297,    63,    64,   231,   298,
     739,   249,   167,   303,   585,   246,   246,    54,   570,   420,
     421,   422,   423,   424,   425,   426,   427,   585,   297,    63,
     298,   739,   249,   449,   574,   588,   167,   285,   428,   422,
     585,   585,   352,   585,   588,   585,   315,   214,   585,   328,
     447,   585,   120,   420,   570,   585,   570,   585,   585,   585,
     585,   467,   934,   315,   461,   461,   123,   476,   123,   467,
     476,   468,   461,   588,   466,   253,   471,   299,   283,   922,
     283,   923,   570,   475,   585,   947,   935,   938,   570,   947,
     941,   944,   570,   585,   588,   297,    63,   298,   739,   249,
     501,   588,   167,   203,   428,   502,   957,   585,   253,   589,
     352,   253,   120,   361,   420,   253,   253,   434,   585,   498,
     500,   504,   570,   510,   509,   612,   975,   570,   572,   571,
     668,   675,   661,   699,   570,   690,   570,   570,   570,   570,
      33,    33,   669,   669,   586,   586,   575,   570,   586,   203,
     204,    22,   611,    48,   194,   585,   611,   201,   202,   208,
     209,   210,   211,   758,    48,   194,   575,   174,   175,   585,
     585,   585,   585,   585,   588,   304,   586,   586,   299,   435,
     436,   585,   585,   585,   588,   450,   889,   588,   245,   588,
     585,   585,   588,   588,   585,   588,   484,   948,   948,   253,
     585,   246,   246,   485,   486,   487,   924,   585,   924,   253,
     585,   585,   463,   588,   585,   585,   182,   588,   940,   588,
     946,   585,   585,   585,   585,   588,   588,   585,   245,   588,
     586,   585,   585,   585,   588,   588,   588,   588,   192,   192,
     690,   105,   572,   585,   585,   572,   573,   753,   570,   611,
     611,   570,   570,   203,   204,   205,   206,   207,   203,   204,
     205,   206,   207,   570,   611,   611,   759,   589,   589,   588,
     588,   575,   589,   891,   434,   588,   585,   585,   585,   948,
     585,   948,   588,   585,   434,   612,   612,   696,   570,   669,
     757,   570,   570,   757,   757,   757,   570,   570,   429,   890,
     573,   757,   757,   757,   757,   570,   669,   572
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int16 yyr1[] =
{
       0,   580,   581,   581,   582,   582,   583,   583,   584,   585,
     585,   585,   586,   586,   586,   587,   587,   588,   588,   588,
     588,   588,   588,   589,   589,   589,   590,   590,   591,   592,
     593,   594,   595,   596,   597,   598,   599,   600,   600,   601,
     601,   602,   602,   603,   603,   603,   603,   603,   603,   604,
     604,   605,   605,   606,   606,   607,   607,   608,   608,   608,
     608,   608,   609,   610,   610,   611,   612,   612,   613,   613,
     614,   614,   614,   614,   614,   614,   614,   614,   615,   615,
     616,   616,   616,   616,   616,   616,   616,   616,   616,   616,
     617,   618,   619,   620,   620,   621,   621,   621,   621,   622,
     622,   623,   623,   624,   624,   624,   625,   625,   626,   626,
     626,   626,   627,   627,   627,   628,   628,   628,   628,   629,
     629,   629,   629,   629,   630,   630,   631,   631,   632,   633,
     634,   635,   636,   637,   637,   638,   639,   638,   640,   638,
     641,   583,   643,   642,   583,   644,   583,   645,   583,   646,
     583,   647,   648,   648,   648,   648,   648,   648,   648,   648,
     648,   648,   648,   648,   648,   648,   648,   648,   649,   649,
     650,   650,   651,   651,   652,   653,   654,   653,   655,   656,
     583,   658,   659,   657,   660,   660,   662,   661,   663,   663,
     664,   665,   665,   666,   666,   667,   667,   667,   667,   667,
     667,   668,   668,   668,   668,   669,   670,   670,   670,   670,
     670,   670,   670,   670,   670,   670,   671,   671,   671,   671,
     672,   672,   672,   673,   673,   673,   673,   673,   673,   673,
     673,   674,   674,   674,   674,   674,   675,   675,   675,   675,
     675,   676,   676,   677,   677,   678,   678,   678,   678,   679,
     679,   680,   680,   680,   681,   681,   682,   683,   683,   683,
     683,   683,   683,   683,   683,   683,   684,   684,   684,   684,
     684,   684,   684,   684,   684,   684,   684,   685,   685,   685,
     685,   686,   686,   686,   687,   688,   689,   689,   689,   689,
     689,   689,   689,   689,   689,   689,   689,   689,   689,   690,
     690,   690,   690,   690,   690,   690,   690,   690,   690,   690,
     690,   690,   690,   690,   690,   690,   690,   690,   690,   690,
     690,   690,   690,   690,   690,   690,   690,   690,   690,   690,
     690,   690,   690,   690,   690,   690,   690,   690,   690,   690,
     690,   690,   690,   690,   690,   690,   690,   690,   690,   690,
     690,   691,   691,   691,   692,   692,   692,   693,   693,   694,
     695,   695,   696,   696,   696,   697,   698,   699,   696,   696,
     696,   696,   696,   696,   696,   696,   696,   696,   696,   696,
     696,   696,   696,   696,   696,   696,   700,   701,   701,   701,
     583,   702,   702,   703,   703,   703,   703,   583,   704,   583,
     705,   706,   706,   706,   706,   706,   706,   706,   706,   706,
     706,   583,   707,   708,   709,   709,   710,   710,   710,   710,
     710,   710,   711,   711,   712,   712,   583,   713,   713,   714,
     714,   714,   714,   715,   715,   715,   715,   715,   715,   715,
     715,   716,   717,   717,   717,   717,   717,   717,   717,   717,
     717,   718,   718,   719,   719,   720,   720,   721,   722,   723,
     724,   724,   724,   724,   725,   725,   725,   725,   725,   726,
     726,   583,   727,   727,   727,   583,   728,   728,   728,   728,
     729,   729,   729,   729,   729,   729,   729,   729,   730,   731,
     732,   732,   733,   733,   734,   735,   735,   736,   737,   738,
     739,   739,   740,   741,   742,   742,   742,   742,   742,   743,
     744,   745,   745,   745,   746,   746,   747,   747,   747,   747,
     747,   747,   748,   748,   748,   749,   749,   750,   750,   751,
     752,   752,   753,   753,   754,   754,   755,   755,   756,   756,
     757,   757,   757,   757,   757,   757,   757,   757,   757,   758,
     758,   758,   758,   758,   758,   758,   758,   758,   758,   758,
     758,   758,   758,   759,   760,   760,   760,   760,   760,   760,
     760,   761,   761,   762,   638,   763,   638,   764,   638,   765,
     638,   766,   767,   767,   638,   768,   638,   769,   638,   770,
     771,   771,   771,   771,   771,   771,   771,   771,   771,   771,
     771,   771,   771,   771,   771,   771,   771,   771,   772,   772,
     772,   772,   772,   772,   772,   772,   773,   773,   773,   773,
     774,   774,   774,   774,   775,   775,   775,   775,   775,   775,
     775,   775,   775,   775,   775,   775,   775,   775,   775,   775,
     775,   775,   775,   775,   776,   638,   777,   778,   778,   778,
     778,   778,   778,   778,   638,   779,   638,   780,   638,   781,
     638,   782,   638,   783,   638,   784,   638,   785,   638,   786,
     638,   787,   638,   788,   638,   789,   790,   790,   790,   791,
     791,   638,   792,   638,   793,   638,   794,   638,   795,   638,
     796,   638,   797,   638,   798,   799,   799,   638,   800,   638,
     801,   802,   802,   802,   803,   803,   803,   804,   740,   805,
     806,   806,   806,   806,   806,   806,   807,   807,   808,   809,
     809,   809,   809,   810,   810,   811,   812,   813,   813,   813,
     813,   813,   813,   813,   813,   814,   814,   814,   814,   814,
     814,   815,   815,   816,   816,   817,   818,   819,   819,   819,
     820,   820,   821,   821,   822,   638,   823,   740,   824,   825,
     825,   825,   825,   826,   826,   827,   828,   829,   830,   830,
     830,   830,   830,   830,   830,   830,   830,   830,   830,   830,
     830,   830,   830,   830,   830,   830,   830,   830,   831,   831,
     832,   832,   833,   804,   638,   834,   638,   835,   638,   836,
     638,   837,   587,   587,   587,   587,   587,   587,   740,   838,
     839,   839,   839,   840,   840,   840,   840,   840,   841,   841,
     842,   842,   843,   843,   843,   843,   843,   843,   843,   843,
     843,   844,   844,   844,   844,   844,   845,   845,   845,   845,
     845,   845,   845,   845,   845,   845,   845,   845,   845,   845,
     845,   845,   845,   845,   845,   845,   845,   845,   845,   845,
     845,   845,   845,   845,   845,   845,   845,   845,   845,   845,
     845,   845,   845,   845,   845,   845,   845,   845,   845,   845,
     845,   845,   845,   845,   845,   845,   845,   845,   845,   845,
     845,   845,   845,   845,   845,   845,   845,   845,   845,   845,
     845,   846,   847,   845,   845,   845,   845,   848,   848,   848,
     848,   848,   848,   848,   848,   848,   848,   848,   848,   849,
     850,   850,   850,   851,   851,   851,   852,   852,   852,   852,
     852,   852,   852,   852,   852,   852,   852,   852,   852,   852,
     852,   852,   852,   852,   852,   852,   852,   852,   852,   852,
     852,   853,   853,   854,   854,   855,   856,   804,   804,   804,
     804,   804,   804,   804,   804,   804,   804,   804,   804,   804,
     804,   740,   857,   858,   858,   858,   858,   858,   858,   858,
     858,   859,   859,   860,   638,   861,   862,   862,   862,   862,
     862,   862,   740,   863,   863,   863,   864,   865,   865,   866,
     866,   867,   868,   869,   869,   869,   869,   869,   869,   869,
     869,   869,   869,   869,   869,   869,   869,   869,   870,   871,
     872,   872,   873,   873,   873,   873,   873,   873,   873,   873,
     873,   874,   873,   875,   873,   873,   873,   873,   876,   876,
     877,   878,   878,   879,   879,   879,   880,   880,   881,   881,
     882,   882,   882,   882,   882,   882,   882,   882,   882,   882,
     882,   883,   884,   884,   884,   884,   884,   884,   884,   884,
     884,   884,   884,   884,   884,   884,   884,   884,   884,   884,
     884,   884,   884,   884,   884,   884,   884,   884,   884,   884,
     884,   884,   884,   884,   884,   884,   884,   884,   884,   884,
     884,   885,   885,   886,   887,   888,   888,   888,   889,   889,
     890,   890,   891,   892,   893,   893,   895,   894,   896,   896,
     897,   897,   898,   804,   804,   804,   804,   638,   899,   638,
     900,   638,   901,   638,   902,   638,   903,   638,   904,   638,
     905,   638,   906,   907,   907,   907,   907,   907,   907,   907,
     907,   907,   740,   908,   909,   909,   909,   910,   910,   910,
     910,   910,   910,   910,   910,   910,   910,   740,   912,   911,
     913,   913,   913,   913,   740,   914,   915,   915,   915,   915,
     916,   915,   917,   915,   915,   915,   918,   918,   919,   920,
     921,   921,   921,   921,   921,   921,   921,   921,   921,   921,
     921,   921,   921,   921,   921,   921,   921,   922,   921,   923,
     921,   921,   921,   924,   924,   924,   925,   926,   926,   927,
     927,   928,   929,   930,   930,   930,   930,   930,   931,   932,
     932,   933,   933,   934,   935,   936,   937,   937,   937,   938,
     939,   939,   940,   940,   941,   942,   943,   943,   943,   944,
     945,   945,   946,   946,   947,   947,   948,   948,   804,   804,
     740,   949,   949,   950,   951,   951,   951,   951,   951,   951,
     952,   952,   953,   954,   955,   956,   956,   956,   956,   956,
     956,   956,   956,   956,   956,   956,   956,   956,   956,   956,
     956,   956,   956,   956,   956,   956,   956,   956,   956,   956,
     956,   956,   957,   957,   957,   957,   958,   958,   959,   959,
     960,   804,   804,   638,   961,   638,   962,   740,   963,   964,
     965,   965,   966,   966,   966,   966,   966,   966,   966,   966,
     966,   966,   967,   967,   968,   969,   969,   970,   970,   971,
     972,   973,   973,   974,   974,   974,   975,   740,   976,   977,
     977,   977,   977,   977,   977,   978,   978,   978,   978,   978,
     978,   979,   979,   980,   981,   981,   981,   981,   981,   981,
     981,   981,   981,   982,   983,   983,   984,   984,   638,   985,
     986,   987,   987,   987,   804,   804,   804,   804,   804,   804,
     804,   804,   804,   804,   804,   804,   804,   804,   804,   804,
     804,   804,   804,   804,   804,   804,   804,   804,   804,   804,
     804,   804,   583,   988,   989,   990,   990,   991,   991,   992,
     992,   992,   993,   993,   994,   994,   995,   995,   995,   995,
     995,   995,   995,   995,   995,   583,   996,   996,   583,   997,
     997,   997,   997,   638,   998,   638,   999,   638,  1000,   638,
    1001,   638,  1002,   638,  1003,   638,  1004,   638,  1005,  1006,
    1006,  1007,  1007,  1007,   740,  1008,   986,   986,  1009,  1009,
    1010,  1010,  1010,  1010,  1010,  1010,  1010,   740,  1011,  1012,
    1012,  1013,  1013,  1013,  1014,  1015,  1016,  1016,  1017,  1017,
    1018,   804,   804
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     2,     0,     2,     1,     1,     5,     1,
       3,     1,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     1,     0,     2,     2,     2,
       3,     7,     2,     2,     5,     5,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     1,     1,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     2,     2,     1,     1,     1,
       1,     2,     1,     1,     1,     2,     4,     1,     3,     1,
       3,     1,     3,     4,     6,     6,     0,     1,     1,     1,
       1,     1,     2,     4,     4,     2,     2,     2,     2,     2,
       4,     2,     2,     2,     0,     3,     0,     3,     0,     0,
       0,     6,     6,     1,     1,     1,     2,     1,     2,     1,
       2,     1,     0,     4,     1,     2,     1,     4,     1,     6,
       1,     8,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     0,     4,
       2,     5,     0,     4,     1,     1,     0,     2,     2,     4,
       1,     0,     0,     6,     0,     1,     0,     2,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     3,     1,     1,     1,     5,     7,     5,     7,
       9,     7,     5,     7,     5,    11,     1,     1,     2,     1,
       7,     9,     7,     7,     9,     7,     9,     7,     9,     7,
      15,     1,     1,     1,     1,     3,     1,     1,     1,     1,
       3,     1,     3,     1,     3,     1,     2,     2,     6,     1,
       3,     0,     4,     3,     1,     3,     3,     2,     4,     3,
       4,     2,     2,     2,     2,     0,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     3,     1,     5,     7,     7,
       1,     0,     1,     3,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     4,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       6,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     5,     7,     6,     6,     6,     6,     4,     8,     4,
       1,     1,     1,     1,     0,     1,     3,     0,     2,     4,
       2,     1,     3,     4,     6,     0,     0,     0,     9,     4,
       3,     4,     4,     5,     3,     3,     3,     2,     5,     4,
       7,     7,     7,     7,     7,     9,     0,     1,     1,     1,
       1,     4,     5,     1,     3,     1,     1,     1,     3,     1,
       5,     1,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     0,     1,     2,     2,     4,     4,
       3,     3,     0,     3,     0,     3,     1,     1,     1,     0,
       1,     2,     3,     0,     2,     2,     2,     3,     2,     2,
       2,     1,     2,     2,     2,     3,     3,     3,     2,     4,
       3,     1,     2,     0,     3,     0,     3,     0,     3,     1,
       2,     1,     1,     1,     0,     2,     2,     2,     2,     2,
       1,     1,     3,     3,     3,     1,     1,     1,     1,     1,
       2,     4,     3,     4,     4,     3,     4,     4,     3,     0,
       1,     2,     0,     1,     3,     1,     3,     1,     0,     2,
       2,     2,     2,     2,     3,     3,     3,     3,     5,     0,
       3,     1,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     1,     1,
       4,     1,     0,     3,     5,     2,     1,     1,     2,     2,
       0,     5,     5,     5,     5,     4,     4,     4,     4,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     1,
       1,     1,     1,     0,     2,     4,     4,     4,     2,     2,
       1,     0,     3,     3,     1,     3,     1,     3,     1,     4,
       1,     5,     1,     0,     1,     3,     1,     4,     1,     4,
       0,     2,     3,     3,     3,     3,     4,     4,     3,     2,
       2,     2,     2,     3,     3,     3,     2,     2,     1,     1,
       1,     1,     2,     2,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     4,     0,     2,     2,
       2,     2,     2,     2,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     4,     1,     3,     1,     4,     1,     1,     3,     0,
       1,     1,     4,     1,     4,     1,     4,     1,     3,     1,
       4,     1,     4,     1,     3,     2,     2,     1,     4,     1,
       2,     1,     1,     1,     1,     0,     1,     1,     1,     2,
       1,     2,     2,     2,     2,     3,     0,     3,     5,     1,
       1,     1,     1,     0,     2,     0,     0,     2,     4,     4,
       4,     2,     2,     2,     1,     1,     1,     2,     2,     3,
       3,     0,     3,     0,     3,     4,     3,     0,     2,     2,
       0,     2,     0,     2,     4,     1,     5,     1,     2,     1,
       1,     2,     4,     0,     3,     5,     0,     0,     0,     2,
       2,     2,     2,     2,     3,     3,     3,     3,     2,     3,
       3,     4,     4,     4,     2,     2,     3,     1,     0,     3,
       0,     3,     4,     1,     1,     6,     1,     6,     1,     6,
       1,     6,     1,     1,     1,     1,     1,     1,     2,     2,
       0,     3,     3,     0,     3,     3,     2,     2,     1,     3,
       1,     3,     1,     3,     2,     2,     2,     2,     2,     2,
       3,     1,     1,     1,     1,     1,     3,     3,     3,     4,
       6,     4,     6,     6,     4,     6,     5,     5,     4,     5,
       5,     6,     6,     3,     3,     4,     5,     6,     5,     5,
       5,     5,     5,     5,     5,     5,     5,     5,     6,     6,
       6,     5,     6,     6,     8,     6,     6,     6,     5,     6,
       5,     4,     4,     4,     4,     5,     5,     6,     5,     5,
       6,     5,     5,     6,     7,     7,     7,     5,     5,     4,
       4,     0,     0,     6,     6,     5,     5,     1,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     4,     4,
       4,     2,     3,     2,     2,     2,     2,     3,     5,     5,
       4,     3,     3,     3,     3,     3,     2,     2,     2,     3,
       3,     0,     3,     0,     3,     0,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     1,     2,     2,     2,     1,     2,     2,
       4,     0,     3,     5,     1,     4,     0,     3,     3,     3,
       2,     3,     2,     1,     2,     2,     2,     3,     3,     0,
       1,     2,     3,     1,     1,     2,     2,     3,     3,     3,
       3,     4,     2,     4,     3,     2,     3,     1,     2,     4,
       0,     3,     2,     1,     2,     3,     3,     3,     3,     2,
       3,     0,     5,     0,     5,     2,     2,     1,     4,     1,
       1,     0,     3,     2,     2,     2,     5,     2,     0,     3,
       0,     2,     2,     3,     2,     2,     3,     2,     2,     2,
       1,     3,     2,     2,     2,     2,     2,     2,     3,     2,
       2,     2,     2,     2,     2,     2,     2,     3,     3,     3,
       3,     2,     3,     2,     3,     3,     4,     4,     2,     2,
       2,     3,     3,     3,     2,     3,     3,     3,     4,     2,
       1,     0,     2,     3,     1,     0,     1,     2,     0,     2,
       0,     1,     3,     0,     0,     2,     0,     3,     0,     3,
       0,     3,     3,     1,     1,     1,     1,     1,     4,     1,
       6,     1,     6,     1,     6,     1,     7,     1,     6,     1,
       7,     1,     5,     0,     2,     3,     2,     3,     3,     2,
       3,     2,     2,     2,     3,     3,     3,     1,     3,     3,
       2,     3,     3,     2,     2,     2,     2,     2,     0,     4,
       2,     3,     3,     5,     1,     2,     1,     1,     2,     2,
       0,     3,     0,     3,     2,     3,     0,     3,     5,     0,
       4,     4,     3,     4,     2,     3,     3,     3,     3,     4,
       4,     4,     3,     3,     4,     4,     2,     0,     3,     0,
       3,     3,     3,     1,     1,     1,     0,     0,     3,     0,
       3,     4,     1,     2,     2,     2,     4,     4,     0,     0,
       3,     0,     3,     3,     1,     0,     0,     2,     2,     0,
       0,     3,     1,     5,     1,     0,     0,     2,     2,     0,
       0,     3,     1,     5,     1,     2,     0,     2,     1,     1,
       1,     1,     2,     2,     1,     1,     2,     4,     2,     2,
       0,     3,     5,     0,     0,     2,     2,     2,     2,     2,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     3,     4,     3,     2,
       2,     1,     1,     1,     1,     1,     0,     3,     0,     3,
       4,     1,     1,     1,     6,     1,     6,     1,     2,     5,
       0,     3,     1,     1,     2,     3,     1,     2,     3,     3,
       3,     4,     0,     1,     2,     1,     1,     2,     6,     0,
       0,     0,     3,     4,     4,     2,     0,     2,     2,     3,
       3,     3,     5,     5,     4,     3,     2,     3,     3,     3,
       3,     1,     2,     2,     2,     3,     5,     1,     2,     2,
       2,     2,     2,     1,     0,     2,     0,     3,     1,     4,
       3,     3,     2,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     0,     5,     2,     0,     0,     2,     2,
       2,     1,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     3,     1,     3,
       4,     3,     3,     1,     4,     1,     5,     1,     3,     1,
       3,     1,     3,     1,     4,     1,     2,     1,     3,     0,
       1,     0,     1,     2,     2,     2,     3,     3,     0,     2,
       1,     1,     2,     3,     2,     3,     4,     2,     2,     3,
       3,     1,     2,     3,     0,     2,     0,     3,     0,     3,
       3,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        YY_LAC_DISCARD ("YYBACKUP");                              \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif

/* Given a state stack such that *YYBOTTOM is its bottom, such that
   *YYTOP is either its top or is YYTOP_EMPTY to indicate an empty
   stack, and such that *YYCAPACITY is the maximum number of elements it
   can hold without a reallocation, make sure there is enough room to
   store YYADD more elements.  If not, allocate a new stack using
   YYSTACK_ALLOC, copy the existing elements, and adjust *YYBOTTOM,
   *YYTOP, and *YYCAPACITY to reflect the new capacity and memory
   location.  If *YYBOTTOM != YYBOTTOM_NO_FREE, then free the old stack
   using YYSTACK_FREE.  Return 0 if successful or if no reallocation is
   required.  Return 1 if memory is exhausted.  */
static int
yy_lac_stack_realloc (YYPTRDIFF_T *yycapacity, YYPTRDIFF_T yyadd,
#if YYDEBUG
                      char const *yydebug_prefix,
                      char const *yydebug_suffix,
#endif
                      yy_state_t **yybottom,
                      yy_state_t *yybottom_no_free,
                      yy_state_t **yytop, yy_state_t *yytop_empty)
{
  YYPTRDIFF_T yysize_old =
    *yytop == yytop_empty ? 0 : *yytop - *yybottom + 1;
  YYPTRDIFF_T yysize_new = yysize_old + yyadd;
  if (*yycapacity < yysize_new)
    {
      YYPTRDIFF_T yyalloc = 2 * yysize_new;
      yy_state_t *yybottom_new;
      /* Use YYMAXDEPTH for maximum stack size given that the stack
         should never need to grow larger than the main state stack
         needs to grow without LAC.  */
      if (YYMAXDEPTH < yysize_new)
        {
          YYDPRINTF ((stderr, "%smax size exceeded%s", yydebug_prefix,
                      yydebug_suffix));
          return 1;
        }
      if (YYMAXDEPTH < yyalloc)
        yyalloc = YYMAXDEPTH;
      yybottom_new =
        YY_CAST (yy_state_t *,
                 YYSTACK_ALLOC (YY_CAST (YYSIZE_T,
                                         yyalloc * YYSIZEOF (*yybottom_new))));
      if (!yybottom_new)
        {
          YYDPRINTF ((stderr, "%srealloc failed%s", yydebug_prefix,
                      yydebug_suffix));
          return 1;
        }
      if (*yytop != yytop_empty)
        {
          YYCOPY (yybottom_new, *yybottom, yysize_old);
          *yytop = yybottom_new + (yysize_old - 1);
        }
      if (*yybottom != yybottom_no_free)
        YYSTACK_FREE (*yybottom);
      *yybottom = yybottom_new;
      *yycapacity = yyalloc;
    }
  return 0;
}

/* Establish the initial context for the current lookahead if no initial
   context is currently established.

   We define a context as a snapshot of the parser stacks.  We define
   the initial context for a lookahead as the context in which the
   parser initially examines that lookahead in order to select a
   syntactic action.  Thus, if the lookahead eventually proves
   syntactically unacceptable (possibly in a later context reached via a
   series of reductions), the initial context can be used to determine
   the exact set of tokens that would be syntactically acceptable in the
   lookahead's place.  Moreover, it is the context after which any
   further semantic actions would be erroneous because they would be
   determined by a syntactically unacceptable token.

   YY_LAC_ESTABLISH should be invoked when a reduction is about to be
   performed in an inconsistent state (which, for the purposes of LAC,
   includes consistent states that don't know they're consistent because
   their default reductions have been disabled).  Iff there is a
   lookahead token, it should also be invoked before reporting a syntax
   error.  This latter case is for the sake of the debugging output.

   For parse.lac=full, the implementation of YY_LAC_ESTABLISH is as
   follows.  If no initial context is currently established for the
   current lookahead, then check if that lookahead can eventually be
   shifted if syntactic actions continue from the current context.
   Report a syntax error if it cannot.  */
#define YY_LAC_ESTABLISH                                         \
do {                                                             \
  if (!yy_lac_established)                                       \
    {                                                            \
      YYDPRINTF ((stderr,                                        \
                  "LAC: initial context established for %s\n",   \
                  yytname[yytoken]));                            \
      yy_lac_established = 1;                                    \
      {                                                          \
        int yy_lac_status =                                      \
          yy_lac (yyesa, &yyes, &yyes_capacity, yyssp, yytoken); \
        if (yy_lac_status == 2)                                  \
          goto yyexhaustedlab;                                   \
        if (yy_lac_status == 1)                                  \
          goto yyerrlab;                                         \
      }                                                          \
    }                                                            \
} while (0)

/* Discard any previous initial lookahead context because of Event,
   which may be a lookahead change or an invalidation of the currently
   established initial context for the current lookahead.

   The most common example of a lookahead change is a shift.  An example
   of both cases is syntax error recovery.  That is, a syntax error
   occurs when the lookahead is syntactically erroneous for the
   currently established initial context, so error recovery manipulates
   the parser stacks to try to find a new initial context in which the
   current lookahead is syntactically acceptable.  If it fails to find
   such a context, it discards the lookahead.  */
#if YYDEBUG
# define YY_LAC_DISCARD(Event)                                           \
do {                                                                     \
  if (yy_lac_established)                                                \
    {                                                                    \
      if (yydebug)                                                       \
        YYFPRINTF (stderr, "LAC: initial context discarded due to "      \
                   Event "\n");                                          \
      yy_lac_established = 0;                                            \
    }                                                                    \
} while (0)
#else
# define YY_LAC_DISCARD(Event) yy_lac_established = 0
#endif

/* Given the stack whose top is *YYSSP, return 0 iff YYTOKEN can
   eventually (after perhaps some reductions) be shifted, return 1 if
   not, or return 2 if memory is exhausted.  As preconditions and
   postconditions: *YYES_CAPACITY is the allocated size of the array to
   which *YYES points, and either *YYES = YYESA or *YYES points to an
   array allocated with YYSTACK_ALLOC.  yy_lac may overwrite the
   contents of either array, alter *YYES and *YYES_CAPACITY, and free
   any old *YYES other than YYESA.  */
static int
yy_lac (yy_state_t *yyesa, yy_state_t **yyes,
        YYPTRDIFF_T *yyes_capacity, yy_state_t *yyssp, int yytoken)
{
  yy_state_t *yyes_prev = yyssp;
  yy_state_t *yyesp = yyes_prev;
  YYDPRINTF ((stderr, "LAC: checking lookahead %s:", yytname[yytoken]));
  if (yytoken == YYUNDEFTOK)
    {
      YYDPRINTF ((stderr, " Always Err\n"));
      return 1;
    }
  while (1)
    {
      int yyrule = yypact[+*yyesp];
      if (yypact_value_is_default (yyrule)
          || (yyrule += yytoken) < 0 || YYLAST < yyrule
          || yycheck[yyrule] != yytoken)
        {
          yyrule = yydefact[+*yyesp];
          if (yyrule == 0)
            {
              YYDPRINTF ((stderr, " Err\n"));
              return 1;
            }
        }
      else
        {
          yyrule = yytable[yyrule];
          if (yytable_value_is_error (yyrule))
            {
              YYDPRINTF ((stderr, " Err\n"));
              return 1;
            }
          if (0 < yyrule)
            {
              YYDPRINTF ((stderr, " S%d\n", yyrule));
              return 0;
            }
          yyrule = -yyrule;
        }
      {
        YYPTRDIFF_T yylen = yyr2[yyrule];
        YYDPRINTF ((stderr, " R%d", yyrule - 1));
        if (yyesp != yyes_prev)
          {
            YYPTRDIFF_T yysize = yyesp - *yyes + 1;
            if (yylen < yysize)
              {
                yyesp -= yylen;
                yylen = 0;
              }
            else
              {
                yylen -= yysize;
                yyesp = yyes_prev;
              }
          }
        if (yylen)
          yyesp = yyes_prev -= yylen;
      }
      {
        yy_state_fast_t yystate;
        {
          const int yylhs = yyr1[yyrule] - YYNTOKENS;
          const int yyi = yypgoto[yylhs] + *yyesp;
          yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyesp
                     ? yytable[yyi]
                     : yydefgoto[yylhs]);
        }
        if (yyesp == yyes_prev)
          {
            yyesp = *yyes;
            YY_IGNORE_USELESS_CAST_BEGIN
            *yyesp = YY_CAST (yy_state_t, yystate);
            YY_IGNORE_USELESS_CAST_END
          }
        else
          {
            if (yy_lac_stack_realloc (yyes_capacity, 1,
#if YYDEBUG
                                      " (", ")",
#endif
                                      yyes, yyesa, &yyesp, yyes_prev))
              {
                YYDPRINTF ((stderr, "\n"));
                return 2;
              }
            YY_IGNORE_USELESS_CAST_BEGIN
            *++yyesp = YY_CAST (yy_state_t, yystate);
            YY_IGNORE_USELESS_CAST_END
          }
        YYDPRINTF ((stderr, " G%d", yystate));
      }
    }
}


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.  In order to see if a particular token T is a
   valid looakhead, invoke yy_lac (YYESA, YYES, YYES_CAPACITY, YYSSP, T).

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store or if
   yy_lac returned 2.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyesa, yy_state_t **yyes,
                YYPTRDIFF_T *yyes_capacity, yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
       In the first two cases, it might appear that the current syntax
       error should have been detected in the previous state when yy_lac
       was invoked.  However, at that time, there might have been a
       different syntax error that discarded a different initial context
       during error recovery, leaving behind the current lookahead.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      YYDPRINTF ((stderr, "Constructing syntax error message\n"));
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          int yyx;

          for (yyx = 0; yyx < YYNTOKENS; ++yyx)
            if (yyx != YYTERROR && yyx != YYUNDEFTOK)
              {
                {
                  int yy_lac_status = yy_lac (yyesa, yyes, yyes_capacity,
                                              yyssp, yyx);
                  if (yy_lac_status == 2)
                    return 2;
                  if (yy_lac_status == 1)
                    continue;
                }
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
# if YYDEBUG
      else if (yydebug)
        YYFPRINTF (stderr, "No expected tokens.\n");
# endif
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

    yy_state_t yyesa[20];
    yy_state_t *yyes;
    YYPTRDIFF_T yyes_capacity;

  int yy_lac_established = 0;
  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  yyes = yyesa;
  yyes_capacity = 20;
  if (YYMAXDEPTH < yyes_capacity)
    yyes_capacity = YYMAXDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    {
      YY_LAC_ESTABLISH;
      goto yydefault;
    }
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      YY_LAC_ESTABLISH;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  YY_LAC_DISCARD ("shift");
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  {
    int yychar_backup = yychar;
    switch (yyn)
      {
  case 2:
#line 137 "conf/confbase.Y"
                         { return 0; }
#line 6104 "obj/conf/cf-parse.tab.c"
    break;

  case 3:
#line 138 "conf/confbase.Y"
                      { return 0; }
#line 6110 "obj/conf/cf-parse.tab.c"
    break;

  case 8:
#line 154 "conf/confbase.Y"
                              {
     struct f_val *val = cfg_allocz(sizeof(struct f_val));
     if (f_eval(f_linearize((yyvsp[-1].x), 1), cfg_mem, val) > F_RETURN) cf_error("Runtime error");
     cf_define_symbol((yyvsp[-3].s), SYM_CONSTANT | val->type, val, val);
   }
#line 6120 "obj/conf/cf-parse.tab.c"
    break;

  case 10:
#line 163 "conf/confbase.Y"
                { (yyval.i) = f_eval_int(f_linearize((yyvsp[-1].x), 1)); }
#line 6126 "obj/conf/cf-parse.tab.c"
    break;

  case 11:
#line 164 "conf/confbase.Y"
                {
     if ((yyvsp[0].s)->class != (SYM_CONSTANT | T_INT)) cf_error("Number constant expected");
     (yyval.i) = SYM_VAL((yyvsp[0].s)).i; }
#line 6134 "obj/conf/cf-parse.tab.c"
    break;

  case 12:
#line 170 "conf/confbase.Y"
           { (yyval.time) = (yyvsp[-1].i) S_; }
#line 6140 "obj/conf/cf-parse.tab.c"
    break;

  case 13:
#line 171 "conf/confbase.Y"
           { (yyval.time) = (yyvsp[-1].i) MS_; }
#line 6146 "obj/conf/cf-parse.tab.c"
    break;

  case 14:
#line 172 "conf/confbase.Y"
           { (yyval.time) = (yyvsp[-1].i) US_; }
#line 6152 "obj/conf/cf-parse.tab.c"
    break;

  case 17:
#line 180 "conf/confbase.Y"
        { (yyval.i) = !!(yyvsp[0].i); }
#line 6158 "obj/conf/cf-parse.tab.c"
    break;

  case 18:
#line 181 "conf/confbase.Y"
      { (yyval.i) = 1; }
#line 6164 "obj/conf/cf-parse.tab.c"
    break;

  case 19:
#line 182 "conf/confbase.Y"
       { (yyval.i) = 1; }
#line 6170 "obj/conf/cf-parse.tab.c"
    break;

  case 20:
#line 183 "conf/confbase.Y"
       { (yyval.i) = 0; }
#line 6176 "obj/conf/cf-parse.tab.c"
    break;

  case 21:
#line 184 "conf/confbase.Y"
      { (yyval.i) = 0; }
#line 6182 "obj/conf/cf-parse.tab.c"
    break;

  case 22:
#line 185 "conf/confbase.Y"
                                 { (yyval.i) = 1; }
#line 6188 "obj/conf/cf-parse.tab.c"
    break;

  case 23:
#line 192 "conf/confbase.Y"
       { (yyval.a) = ipa_from_ip4((yyvsp[0].ip4)); }
#line 6194 "obj/conf/cf-parse.tab.c"
    break;

  case 24:
#line 193 "conf/confbase.Y"
       { (yyval.a) = ipa_from_ip6((yyvsp[0].ip6)); }
#line 6200 "obj/conf/cf-parse.tab.c"
    break;

  case 25:
#line 194 "conf/confbase.Y"
                {
     if ((yyvsp[0].s)->class != (SYM_CONSTANT | T_IP)) cf_error("IP address constant expected");
     (yyval.a) = SYM_VAL((yyvsp[0].s)).ip;
   }
#line 6209 "obj/conf/cf-parse.tab.c"
    break;

  case 26:
#line 201 "conf/confbase.Y"
               { (yyval.iface) = NULL; }
#line 6215 "obj/conf/cf-parse.tab.c"
    break;

  case 27:
#line 202 "conf/confbase.Y"
              { (yyval.iface) = if_get_by_name((yyvsp[0].s)->name); }
#line 6221 "obj/conf/cf-parse.tab.c"
    break;

  case 28:
#line 209 "conf/confbase.Y"
           {
     if ((yyvsp[0].i) > IP6_MAX_PREFIX_LENGTH) cf_error("Invalid prefix length %u", (yyvsp[0].i)); // FIXME
     (yyval.i) = (yyvsp[0].i);
   }
#line 6230 "obj/conf/cf-parse.tab.c"
    break;

  case 29:
#line 216 "conf/confbase.Y"
{
  net_fill_ip4(&((yyval.net)), (yyvsp[-1].ip4), (yyvsp[0].i));

  net_addr_ip4 *n = (void *) &((yyval.net));
  if (!net_validate_ip4(n))
    cf_error("Invalid IPv4 prefix %I4/%d, maybe you wanted %I4/%d",
	     n->prefix, n->pxlen, ip4_and(n->prefix, ip4_mkmask(n->pxlen)), n->pxlen);
}
#line 6243 "obj/conf/cf-parse.tab.c"
    break;

  case 30:
#line 226 "conf/confbase.Y"
{
  if ((yyvsp[0].i) > IP6_MAX_PREFIX_LENGTH)
    cf_error("Invalid prefix length %u", (yyvsp[0].i));

  net_fill_ip6(&((yyval.net)), (yyvsp[-2].ip6), (yyvsp[0].i));

  net_addr_ip6 *n = (void *) &((yyval.net));
  if (!net_validate_ip6(n))
    cf_error("Invalid IPv6 prefix %I6/%d, maybe you wanted %I6/%d",
	     n->prefix, n->pxlen, ip6_and(n->prefix, ip6_mkmask(n->pxlen)), n->pxlen);
}
#line 6259 "obj/conf/cf-parse.tab.c"
    break;

  case 31:
#line 239 "conf/confbase.Y"
{
  if ((yyvsp[-4].i) > IP6_MAX_PREFIX_LENGTH)
    cf_error("Invalid prefix length %u", (yyvsp[-4].i));

  if ((yyvsp[0].i) > IP6_MAX_PREFIX_LENGTH)
    cf_error("Invalid prefix length %u", (yyvsp[0].i));

  (yyval.net_ptr) = cfg_alloc(sizeof(net_addr_ip6_sadr));
  net_fill_ip6_sadr((yyval.net_ptr), (yyvsp[-6].ip6), (yyvsp[-4].i), (yyvsp[-2].ip6), (yyvsp[0].i));

  net_addr_ip6_sadr *n = (void *) (yyval.net_ptr);
  if (!net_validate_ip6_sadr(n))
    cf_error("Invalid SADR IPv6 prefix %I6/%d from %I6/%d, maybe you wanted %I6/%d from %I6/%d",
	     n->dst_prefix, n->dst_pxlen, n->src_prefix, n->src_pxlen,
	     ip6_and(n->dst_prefix, ip6_mkmask(n->dst_pxlen)), n->dst_pxlen,
	     ip6_and(n->src_prefix, ip6_mkmask(n->src_pxlen)), n->src_pxlen);
}
#line 6281 "obj/conf/cf-parse.tab.c"
    break;

  case 32:
#line 258 "conf/confbase.Y"
{
  (yyval.net_ptr) = cfg_alloc(sizeof(net_addr_vpn4));
  net_fill_vpn4((yyval.net_ptr), net4_prefix(&(yyvsp[0].net)), net4_pxlen(&(yyvsp[0].net)), (yyvsp[-1].i64));
}
#line 6290 "obj/conf/cf-parse.tab.c"
    break;

  case 33:
#line 264 "conf/confbase.Y"
{
  (yyval.net_ptr) = cfg_alloc(sizeof(net_addr_vpn6));
  net_fill_vpn6((yyval.net_ptr), net6_prefix(&(yyvsp[0].net)), net6_pxlen(&(yyvsp[0].net)), (yyvsp[-1].i64));
}
#line 6299 "obj/conf/cf-parse.tab.c"
    break;

  case 34:
#line 270 "conf/confbase.Y"
{
  (yyval.net_ptr) = cfg_alloc(sizeof(net_addr_roa4));
  net_fill_roa4((yyval.net_ptr), net4_prefix(&(yyvsp[-4].net)), net4_pxlen(&(yyvsp[-4].net)), (yyvsp[-2].i), (yyvsp[0].i));
  if ((yyvsp[-2].i) < net4_pxlen(&(yyvsp[-4].net)) || (yyvsp[-2].i) > IP4_MAX_PREFIX_LENGTH)
    cf_error("Invalid max prefix length %u", (yyvsp[-2].i));
}
#line 6310 "obj/conf/cf-parse.tab.c"
    break;

  case 35:
#line 278 "conf/confbase.Y"
{
  (yyval.net_ptr) = cfg_alloc(sizeof(net_addr_roa6));
  net_fill_roa6((yyval.net_ptr), net6_prefix(&(yyvsp[-4].net)), net6_pxlen(&(yyvsp[-4].net)), (yyvsp[-2].i), (yyvsp[0].i));
  if ((yyvsp[-2].i) < net6_pxlen(&(yyvsp[-4].net)) || (yyvsp[-2].i) > IP6_MAX_PREFIX_LENGTH)
    cf_error("Invalid max prefix length %u", (yyvsp[-2].i));
}
#line 6321 "obj/conf/cf-parse.tab.c"
    break;

  case 36:
#line 286 "conf/confbase.Y"
{
  (yyval.net_ptr) = cfg_alloc(sizeof(net_addr_roa6));
  net_fill_mpls((yyval.net_ptr), (yyvsp[0].i));
}
#line 6330 "obj/conf/cf-parse.tab.c"
    break;

  case 43:
#line 296 "conf/confbase.Y"
           { (yyval.net_ptr) = cfg_alloc((yyvsp[0].net).length); net_copy((yyval.net_ptr), &((yyvsp[0].net))); }
#line 6336 "obj/conf/cf-parse.tab.c"
    break;

  case 50:
#line 309 "conf/confbase.Y"
                {
     if (((yyvsp[0].s)->class != (SYM_CONSTANT | T_NET)) || (SYM_VAL((yyvsp[0].s)).net->type != NET_IP4))
       cf_error("IPv4 network constant expected");
     (yyval.net) = * SYM_VAL((yyvsp[0].s)).net;
   }
#line 6346 "obj/conf/cf-parse.tab.c"
    break;

  case 52:
#line 318 "conf/confbase.Y"
                {
     if (((yyvsp[0].s)->class != (SYM_CONSTANT | T_NET)) || (SYM_VAL((yyvsp[0].s)).net->type != NET_IP6))
       cf_error("IPv6 network constant expected");
     (yyval.net) = * SYM_VAL((yyvsp[0].s)).net;
   }
#line 6356 "obj/conf/cf-parse.tab.c"
    break;

  case 54:
#line 327 "conf/confbase.Y"
                {
     if (((yyvsp[0].s)->class != (SYM_CONSTANT | T_NET)) || !net_is_ip(SYM_VAL((yyvsp[0].s)).net))
       cf_error("IP network constant expected");
     (yyval.net) = * SYM_VAL((yyvsp[0].s)).net;
   }
#line 6366 "obj/conf/cf-parse.tab.c"
    break;

  case 56:
#line 336 "conf/confbase.Y"
                {
     if ((yyvsp[0].s)->class != (SYM_CONSTANT | T_NET))
       cf_error("Network constant expected");
     (yyval.net_ptr) = (net_addr *) SYM_VAL((yyvsp[0].s)).net; /* Avoid const warning */
   }
#line 6376 "obj/conf/cf-parse.tab.c"
    break;

  case 59:
#line 346 "conf/confbase.Y"
       { net_fill_ip4(&((yyval.net)), (yyvsp[0].ip4), IP4_MAX_PREFIX_LENGTH); }
#line 6382 "obj/conf/cf-parse.tab.c"
    break;

  case 60:
#line 347 "conf/confbase.Y"
       { net_fill_ip6(&((yyval.net)), (yyvsp[0].ip6), IP6_MAX_PREFIX_LENGTH); }
#line 6388 "obj/conf/cf-parse.tab.c"
    break;

  case 61:
#line 348 "conf/confbase.Y"
                {
     if ((yyvsp[0].s)->class == (SYM_CONSTANT | T_IP))
       net_fill_ip_host(&((yyval.net)), SYM_VAL((yyvsp[0].s)).ip);
     else if (((yyvsp[0].s)->class == (SYM_CONSTANT | T_NET)) && net_is_ip(SYM_VAL((yyvsp[0].s)).net))
       (yyval.net) = * SYM_VAL((yyvsp[0].s)).net;
     else
       cf_error("IP address or network constant expected");
   }
#line 6401 "obj/conf/cf-parse.tab.c"
    break;

  case 62:
#line 359 "conf/confbase.Y"
{
  (yyval.mls) = cfg_allocz(sizeof(mpls_label_stack));
  (yyval.mls)->len = 1;
  (yyval.mls)->stack[0] = (yyvsp[0].i);
}
#line 6411 "obj/conf/cf-parse.tab.c"
    break;

  case 64:
#line 367 "conf/confbase.Y"
                        {
    if ((yyvsp[-2].mls)->len >= MPLS_MAX_LABEL_STACK)
      cf_error("Too many labels in stack");
    (yyvsp[-2].mls)->stack[(yyvsp[-2].mls)->len++] = (yyvsp[0].i);
    (yyval.mls) = (yyvsp[-2].mls);
  }
#line 6422 "obj/conf/cf-parse.tab.c"
    break;

  case 65:
#line 376 "conf/confbase.Y"
        {
     (yyval.time) = tm_parse_time((yyvsp[0].t));
     if (!(yyval.time))
       cf_error("Invalid date/time");
   }
#line 6432 "obj/conf/cf-parse.tab.c"
    break;

  case 67:
#line 385 "conf/confbase.Y"
                {
     if ((yyvsp[0].s)->class != (SYM_CONSTANT | T_STRING)) cf_error("String constant expected");
     (yyval.t) = SYM_VAL((yyvsp[0].s)).s;
   }
#line 6441 "obj/conf/cf-parse.tab.c"
    break;

  case 69:
#line 393 "conf/confbase.Y"
               { (yyval.t) = NULL; }
#line 6447 "obj/conf/cf-parse.tab.c"
    break;

  case 70:
#line 37 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_OP_TRUE; }
#line 6453 "obj/conf/cf-parse.tab.c"
    break;

  case 71:
#line 38 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_OP_EQ;  }
#line 6459 "obj/conf/cf-parse.tab.c"
    break;

  case 72:
#line 39 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_OP_NEQ; }
#line 6465 "obj/conf/cf-parse.tab.c"
    break;

  case 73:
#line 40 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_OP_LT;  }
#line 6471 "obj/conf/cf-parse.tab.c"
    break;

  case 74:
#line 41 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_OP_LEQ; }
#line 6477 "obj/conf/cf-parse.tab.c"
    break;

  case 75:
#line 42 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_OP_GT;  }
#line 6483 "obj/conf/cf-parse.tab.c"
    break;

  case 76:
#line 43 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_OP_GEQ; }
#line 6489 "obj/conf/cf-parse.tab.c"
    break;

  case 77:
#line 44 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_OP_FALSE; }
#line 6495 "obj/conf/cf-parse.tab.c"
    break;

  case 78:
#line 48 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_OP_OR; }
#line 6501 "obj/conf/cf-parse.tab.c"
    break;

  case 79:
#line 49 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_OP_AND; }
#line 6507 "obj/conf/cf-parse.tab.c"
    break;

  case 80:
#line 53 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_IP_PROTOCOL; }
#line 6513 "obj/conf/cf-parse.tab.c"
    break;

  case 81:
#line 54 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_NEXT_HEADER; }
#line 6519 "obj/conf/cf-parse.tab.c"
    break;

  case 82:
#line 55 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_PORT; }
#line 6525 "obj/conf/cf-parse.tab.c"
    break;

  case 83:
#line 56 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_DST_PORT; }
#line 6531 "obj/conf/cf-parse.tab.c"
    break;

  case 84:
#line 57 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_SRC_PORT; }
#line 6537 "obj/conf/cf-parse.tab.c"
    break;

  case 85:
#line 58 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_ICMP_TYPE; }
#line 6543 "obj/conf/cf-parse.tab.c"
    break;

  case 86:
#line 59 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_ICMP_CODE; }
#line 6549 "obj/conf/cf-parse.tab.c"
    break;

  case 87:
#line 60 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_PACKET_LENGTH; }
#line 6555 "obj/conf/cf-parse.tab.c"
    break;

  case 88:
#line 61 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_DSCP; }
#line 6561 "obj/conf/cf-parse.tab.c"
    break;

  case 89:
#line 62 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_LABEL; }
#line 6567 "obj/conf/cf-parse.tab.c"
    break;

  case 90:
#line 65 "conf/flowspec.Y"
                             { flow_builder_set_type(this_flow, (yyvsp[0].i32)); }
#line 6573 "obj/conf/cf-parse.tab.c"
    break;

  case 91:
#line 66 "conf/flowspec.Y"
                             { flow_builder_set_type(this_flow, FLOW_TYPE_TCP_FLAGS); }
#line 6579 "obj/conf/cf-parse.tab.c"
    break;

  case 92:
#line 67 "conf/flowspec.Y"
                             { flow_builder_set_type(this_flow, FLOW_TYPE_FRAGMENT); }
#line 6585 "obj/conf/cf-parse.tab.c"
    break;

  case 93:
#line 70 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_DST_PREFIX; }
#line 6591 "obj/conf/cf-parse.tab.c"
    break;

  case 94:
#line 71 "conf/flowspec.Y"
                { (yyval.i32) = FLOW_TYPE_SRC_PREFIX; }
#line 6597 "obj/conf/cf-parse.tab.c"
    break;

  case 95:
#line 75 "conf/flowspec.Y"
                    {
     flow_check_cf_value_length(this_flow, (yyvsp[0].i));
     flow_builder_add_op_val(this_flow, (yyvsp[-1].i32), (yyvsp[0].i));
   }
#line 6606 "obj/conf/cf-parse.tab.c"
    break;

  case 96:
#line 79 "conf/flowspec.Y"
                                                {
     flow_check_cf_value_length(this_flow, (yyvsp[0].i));
     flow_builder_add_op_val(this_flow, (yyvsp[-2].i32) | (yyvsp[-1].i32), (yyvsp[0].i));
   }
#line 6615 "obj/conf/cf-parse.tab.c"
    break;

  case 99:
#line 88 "conf/flowspec.Y"
        {
     flow_check_cf_value_length(this_flow, (yyvsp[0].i));
     flow_builder_add_op_val(this_flow, FLOW_OP_EQ, (yyvsp[0].i));
   }
#line 6624 "obj/conf/cf-parse.tab.c"
    break;

  case 100:
#line 92 "conf/flowspec.Y"
                  {
     flow_check_cf_value_length(this_flow, (yyvsp[-2].i));
     flow_check_cf_value_length(this_flow, (yyvsp[0].i));
     flow_builder_add_op_val(this_flow, FLOW_OP_GEQ, (yyvsp[-2].i));
     flow_builder_add_op_val(this_flow, FLOW_OP_AND | FLOW_OP_LEQ, (yyvsp[0].i));
   }
#line 6635 "obj/conf/cf-parse.tab.c"
    break;

  case 103:
#line 106 "conf/flowspec.Y"
                          {
     flow_check_cf_bmk_values(this_flow, (yyvsp[-3].i32), (yyvsp[-2].i), (yyvsp[0].i));
     flow_builder_add_val_mask(this_flow, (yyvsp[-3].i32), (yyvsp[-2].i), (yyvsp[0].i));
   }
#line 6644 "obj/conf/cf-parse.tab.c"
    break;

  case 104:
#line 110 "conf/flowspec.Y"
                                                      {
     flow_check_cf_bmk_values(this_flow, (yyvsp[-3].i32), (yyvsp[-2].i), (yyvsp[0].i));
     flow_builder_add_val_mask(this_flow, (yyvsp[-4].i32) | (yyvsp[-3].i32), (yyvsp[-2].i), (yyvsp[0].i));
   }
#line 6653 "obj/conf/cf-parse.tab.c"
    break;

  case 105:
#line 114 "conf/flowspec.Y"
                                            {
     flow_check_cf_bmk_values(this_flow, (yyvsp[-3].i32), (yyvsp[-2].i), (yyvsp[0].i));
     flow_builder_add_val_mask(this_flow, 0x40 | (yyvsp[-3].i32), (yyvsp[-2].i), (yyvsp[0].i)); /* AND */
   }
#line 6662 "obj/conf/cf-parse.tab.c"
    break;

  case 106:
#line 121 "conf/flowspec.Y"
                { (yyval.i32) = 0x00; }
#line 6668 "obj/conf/cf-parse.tab.c"
    break;

  case 107:
#line 122 "conf/flowspec.Y"
                { (yyval.i32) = 0x02; }
#line 6674 "obj/conf/cf-parse.tab.c"
    break;

  case 108:
#line 126 "conf/flowspec.Y"
                  { (yyval.i32) = 1; }
#line 6680 "obj/conf/cf-parse.tab.c"
    break;

  case 109:
#line 127 "conf/flowspec.Y"
                  { (yyval.i32) = 2; }
#line 6686 "obj/conf/cf-parse.tab.c"
    break;

  case 110:
#line 128 "conf/flowspec.Y"
                  { (yyval.i32) = 4; }
#line 6692 "obj/conf/cf-parse.tab.c"
    break;

  case 111:
#line 129 "conf/flowspec.Y"
                  { (yyval.i32) = 8; }
#line 6698 "obj/conf/cf-parse.tab.c"
    break;

  case 112:
#line 133 "conf/flowspec.Y"
                          {
     flow_builder_add_val_mask(this_flow, 0, ((yyvsp[-1].i32) ? 0 : (yyvsp[0].i32)), (yyvsp[0].i32));
   }
#line 6706 "obj/conf/cf-parse.tab.c"
    break;

  case 113:
#line 136 "conf/flowspec.Y"
                                                       {
     flow_builder_add_val_mask(this_flow, (yyvsp[-2].i32), ((yyvsp[-1].i32) ? 0 : (yyvsp[0].i32)), (yyvsp[0].i32));
   }
#line 6714 "obj/conf/cf-parse.tab.c"
    break;

  case 114:
#line 139 "conf/flowspec.Y"
                                             {
     flow_builder_add_val_mask(this_flow, 0x40, ((yyvsp[-1].i32) ? 0 : (yyvsp[0].i32)), (yyvsp[0].i32)); /* AND */
   }
#line 6722 "obj/conf/cf-parse.tab.c"
    break;

  case 115:
#line 145 "conf/flowspec.Y"
                       {
     flow_builder_set_type(this_flow, (yyvsp[-1].i32));
     flow_builder4_add_pfx(this_flow, (net_addr_ip4 *) &((yyvsp[0].net)));
   }
#line 6731 "obj/conf/cf-parse.tab.c"
    break;

  case 119:
#line 155 "conf/flowspec.Y"
                       {
     flow_builder_set_type(this_flow, (yyvsp[-1].i32));
     flow_builder6_add_pfx(this_flow, (net_addr_ip6 *) &((yyvsp[0].net)), 0);
   }
#line 6740 "obj/conf/cf-parse.tab.c"
    break;

  case 120:
#line 159 "conf/flowspec.Y"
                                  {
     if ((yyvsp[0].i) > (yyvsp[-2].net).pxlen)
       cf_error("Prefix offset is higher than prefix length");
     flow_builder_set_type(this_flow, (yyvsp[-3].i32));
     flow_builder6_add_pfx(this_flow, (net_addr_ip6 *) &((yyvsp[-2].net)), (yyvsp[0].i));
   }
#line 6751 "obj/conf/cf-parse.tab.c"
    break;

  case 128:
#line 181 "conf/flowspec.Y"
{
  if (this_flow == NULL)
    this_flow = flow_builder_init(config_pool);	  /* FIXME: This should be allocated from tmp in future */
  else
    flow_builder_clear(this_flow);
}
#line 6762 "obj/conf/cf-parse.tab.c"
    break;

  case 129:
#line 188 "conf/flowspec.Y"
                       { this_flow->ipv6 = 0; }
#line 6768 "obj/conf/cf-parse.tab.c"
    break;

  case 130:
#line 189 "conf/flowspec.Y"
                       { this_flow->ipv6 = 1; }
#line 6774 "obj/conf/cf-parse.tab.c"
    break;

  case 131:
#line 192 "conf/flowspec.Y"
{
  (yyval.net_ptr) = (net_addr *) flow_builder4_finalize(this_flow, cfg_mem);
  flow4_validate_cf((net_addr_flow4 *) (yyval.net_ptr));
}
#line 6783 "obj/conf/cf-parse.tab.c"
    break;

  case 132:
#line 198 "conf/flowspec.Y"
{
  (yyval.net_ptr) = (net_addr *) flow_builder6_finalize(this_flow, cfg_mem);
  flow6_validate_cf((net_addr_flow6 *) (yyval.net_ptr));
}
#line 6792 "obj/conf/cf-parse.tab.c"
    break;

  case 142:
#line 348 "filter/config.Y"
                 { (yyvsp[0].s) = cf_define_symbol((yyvsp[0].s), SYM_FILTER, filter, NULL); cf_push_scope( (yyvsp[0].s) ); }
#line 6798 "obj/conf/cf-parse.tab.c"
    break;

  case 143:
#line 349 "filter/config.Y"
                 {
     struct filter *f = cfg_alloc(sizeof(struct filter));
     *f = (struct filter) { .sym = (yyvsp[-2].s), .root = (yyvsp[0].fl) };
     (yyvsp[-2].s)->filter = f;

     cf_pop_scope();
   }
#line 6810 "obj/conf/cf-parse.tab.c"
    break;

  case 145:
#line 360 "filter/config.Y"
             { f_eval_int(f_linearize((yyvsp[0].x), 1)); }
#line 6816 "obj/conf/cf-parse.tab.c"
    break;

  case 147:
#line 364 "filter/config.Y"
                                       {
  cf_define_symbol((yyvsp[-1].s), SYM_ATTRIBUTE, attribute, ca_lookup(new_config->pool, (yyvsp[-1].s)->name, (yyvsp[-2].i))->fda);
}
#line 6824 "obj/conf/cf-parse.tab.c"
    break;

  case 149:
#line 370 "filter/config.Y"
                                             {
  cf_assert_symbol((yyvsp[-3].s), SYM_FUNCTION);
  struct f_bt_test_suite *t = cfg_allocz(sizeof(struct f_bt_test_suite));
  t->fn = (yyvsp[-3].s)->function;
  t->fn_name = (yyvsp[-3].s)->name;
  t->dsc = (yyvsp[-1].t);

  add_tail(&new_config->tests, &t->n);
 }
#line 6838 "obj/conf/cf-parse.tab.c"
    break;

  case 151:
#line 383 "filter/config.Y"
                                                            {
  cf_assert_symbol((yyvsp[-5].s), SYM_FUNCTION);
  cf_assert_symbol((yyvsp[-3].s), SYM_FUNCTION);
  struct f_bt_test_suite *t = cfg_allocz(sizeof(struct f_bt_test_suite));
  t->fn = (yyvsp[-5].s)->function;
  t->cmp = (yyvsp[-3].s)->function;
  t->result = (yyvsp[-1].i);
  t->fn_name = (yyvsp[-5].s)->name;
  t->dsc = (yyvsp[-3].s)->name;
  add_tail(&new_config->tests, &t->n);
 }
#line 6854 "obj/conf/cf-parse.tab.c"
    break;

  case 152:
#line 397 "filter/config.Y"
       { (yyval.i) = T_INT; }
#line 6860 "obj/conf/cf-parse.tab.c"
    break;

  case 153:
#line 398 "filter/config.Y"
        { (yyval.i) = T_BOOL; }
#line 6866 "obj/conf/cf-parse.tab.c"
    break;

  case 154:
#line 399 "filter/config.Y"
      { (yyval.i) = T_IP; }
#line 6872 "obj/conf/cf-parse.tab.c"
    break;

  case 155:
#line 400 "filter/config.Y"
      { (yyval.i) = T_RD; }
#line 6878 "obj/conf/cf-parse.tab.c"
    break;

  case 156:
#line 401 "filter/config.Y"
          { (yyval.i) = T_NET; }
#line 6884 "obj/conf/cf-parse.tab.c"
    break;

  case 157:
#line 402 "filter/config.Y"
        { (yyval.i) = T_PAIR; }
#line 6890 "obj/conf/cf-parse.tab.c"
    break;

  case 158:
#line 403 "filter/config.Y"
        { (yyval.i) = T_QUAD; }
#line 6896 "obj/conf/cf-parse.tab.c"
    break;

  case 159:
#line 404 "filter/config.Y"
      { (yyval.i) = T_EC; }
#line 6902 "obj/conf/cf-parse.tab.c"
    break;

  case 160:
#line 405 "filter/config.Y"
      { (yyval.i) = T_LC; }
#line 6908 "obj/conf/cf-parse.tab.c"
    break;

  case 161:
#line 406 "filter/config.Y"
          { (yyval.i) = T_STRING; }
#line 6914 "obj/conf/cf-parse.tab.c"
    break;

  case 162:
#line 407 "filter/config.Y"
           { (yyval.i) = T_PATH_MASK; }
#line 6920 "obj/conf/cf-parse.tab.c"
    break;

  case 163:
#line 408 "filter/config.Y"
           { (yyval.i) = T_PATH; }
#line 6926 "obj/conf/cf-parse.tab.c"
    break;

  case 164:
#line 409 "filter/config.Y"
         { (yyval.i) = T_CLIST; }
#line 6932 "obj/conf/cf-parse.tab.c"
    break;

  case 165:
#line 410 "filter/config.Y"
          { (yyval.i) = T_ECLIST; }
#line 6938 "obj/conf/cf-parse.tab.c"
    break;

  case 166:
#line 411 "filter/config.Y"
          { (yyval.i) = T_LCLIST; }
#line 6944 "obj/conf/cf-parse.tab.c"
    break;

  case 167:
#line 412 "filter/config.Y"
            {
	switch ((yyvsp[-1].i)) {
	  case T_INT:
	  case T_PAIR:
	  case T_QUAD:
	  case T_EC:
	  case T_LC:
	  case T_RD:
	  case T_IP:
	       (yyval.i) = T_SET;
	       break;

	  case T_NET:
	       (yyval.i) = T_PREFIX_SET;
	    break;

	  default:
		cf_error( "You can't create sets of this type." );
	}
   }
#line 6969 "obj/conf/cf-parse.tab.c"
    break;

  case 168:
#line 435 "filter/config.Y"
               { (yyval.fa) = NULL; }
#line 6975 "obj/conf/cf-parse.tab.c"
    break;

  case 169:
#line 436 "filter/config.Y"
                                  {
     if ((yyvsp[-1].s)->scope->slots >= 0xfe) cf_error("Too many declarations, at most 255 allowed");
     (yyval.fa) = cfg_alloc(sizeof(struct f_arg));
     (yyval.fa)->arg = cf_define_symbol((yyvsp[-1].s), SYM_VARIABLE | (yyvsp[-2].i), offset, sym_->scope->slots++);
     (yyval.fa)->next = (yyvsp[-3].fa);
   }
#line 6986 "obj/conf/cf-parse.tab.c"
    break;

  case 170:
#line 445 "filter/config.Y"
           { (yyval.fa) = NULL; }
#line 6992 "obj/conf/cf-parse.tab.c"
    break;

  case 171:
#line 446 "filter/config.Y"
                                      {
     (yyval.fa) = cfg_alloc(sizeof(struct f_arg));
     (yyval.fa)->arg = cf_define_symbol((yyvsp[-1].s), SYM_VARIABLE | (yyvsp[-2].i), offset, sym_->scope->slots++);
     (yyval.fa)->next = (yyvsp[-3].fa);
   }
#line 7002 "obj/conf/cf-parse.tab.c"
    break;

  case 172:
#line 454 "filter/config.Y"
               { (yyval.i) = 0; }
#line 7008 "obj/conf/cf-parse.tab.c"
    break;

  case 173:
#line 455 "filter/config.Y"
                                 {
     cf_define_symbol((yyvsp[-1].s), SYM_VARIABLE | (yyvsp[-2].i), offset, f_new_var(sym_->scope));
     (yyval.i) = (yyvsp[-3].i) + 1;
   }
#line 7017 "obj/conf/cf-parse.tab.c"
    break;

  case 175:
#line 464 "filter/config.Y"
                {
     cf_assert_symbol((yyvsp[0].s), SYM_FILTER);
     (yyval.f) = (yyvsp[0].s)->filter;
   }
#line 7026 "obj/conf/cf-parse.tab.c"
    break;

  case 176:
#line 468 "filter/config.Y"
   { cf_push_scope(NULL); }
#line 7032 "obj/conf/cf-parse.tab.c"
    break;

  case 177:
#line 468 "filter/config.Y"
                                        {
     struct filter *f = cfg_alloc(sizeof(struct filter));
     *f = (struct filter) { .root = (yyvsp[0].fl) };
     (yyval.f) = f;

     cf_pop_scope();
   }
#line 7044 "obj/conf/cf-parse.tab.c"
    break;

  case 178:
#line 478 "filter/config.Y"
              {
     /* Construct 'IF term THEN { ACCEPT; } ELSE { REJECT; }' */
     (yyval.f) = f_new_where((yyvsp[0].x));
   }
#line 7053 "obj/conf/cf-parse.tab.c"
    break;

  case 179:
#line 485 "filter/config.Y"
                              {
     (yyval.fl) = f_linearize((yyvsp[-1].x), 0);
     (yyval.fl)->vars = (yyvsp[-3].i);
   }
#line 7062 "obj/conf/cf-parse.tab.c"
    break;

  case 181:
#line 493 "filter/config.Y"
                   {
     DBG( "Beginning of function %s\n", (yyvsp[0].s)->name );
     (yyvsp[0].s) = cf_define_symbol((yyvsp[0].s), SYM_FUNCTION, function, NULL);
     cf_push_scope((yyvsp[0].s));
   }
#line 7072 "obj/conf/cf-parse.tab.c"
    break;

  case 182:
#line 497 "filter/config.Y"
                   {
     /* Make dummy f_line for storing function prototype */
     struct f_line *dummy = cfg_allocz(sizeof(struct f_line));
     (yyvsp[-2].s)->function = dummy;

     /* Revert the args */
     while ((yyvsp[0].fa)) {
       struct f_arg *tmp = (yyvsp[0].fa);
       (yyvsp[0].fa) = (yyvsp[0].fa)->next;

       tmp->next = dummy->arg_list;
       dummy->arg_list = tmp;
       dummy->args++;
     }
   }
#line 7092 "obj/conf/cf-parse.tab.c"
    break;

  case 183:
#line 511 "filter/config.Y"
                   {
     (yyvsp[0].fl)->args = (yyvsp[-4].s)->function->args;
     (yyvsp[0].fl)->arg_list = (yyvsp[-4].s)->function->arg_list;
     (yyvsp[-4].s)->function = (yyvsp[0].fl);
     cf_pop_scope();
   }
#line 7103 "obj/conf/cf-parse.tab.c"
    break;

  case 184:
#line 521 "filter/config.Y"
                  { (yyval.x) = NULL; }
#line 7109 "obj/conf/cf-parse.tab.c"
    break;

  case 185:
#line 522 "filter/config.Y"
            { (yyval.x) = (yyvsp[0].xp).begin; }
#line 7115 "obj/conf/cf-parse.tab.c"
    break;

  case 186:
#line 525 "filter/config.Y"
             { cf_push_soft_scope(); }
#line 7121 "obj/conf/cf-parse.tab.c"
    break;

  case 187:
#line 525 "filter/config.Y"
                                            { cf_pop_soft_scope(); (yyval.x) = (yyvsp[0].x); }
#line 7127 "obj/conf/cf-parse.tab.c"
    break;

  case 190:
#line 529 "filter/config.Y"
                  {
  (yyval.xp).begin = (yyval.xp).end = (yyvsp[0].x);
  if ((yyvsp[0].x))
    while ((yyval.xp).end->next)
      (yyval.xp).end = (yyval.xp).end->next;
}
#line 7138 "obj/conf/cf-parse.tab.c"
    break;

  case 192:
#line 538 "filter/config.Y"
                     {
  if (!(yyvsp[-1].xp).begin)
    (yyval.xp) = (yyvsp[0].xp);
  else if (!(yyvsp[0].xp).begin)
    (yyval.xp) = (yyvsp[-1].xp);
  else {
    (yyval.xp).begin = (yyvsp[-1].xp).begin;
    (yyval.xp).end = (yyvsp[0].xp).end;
    (yyvsp[-1].xp).end->next = (yyvsp[0].xp).begin;
  }
 }
#line 7154 "obj/conf/cf-parse.tab.c"
    break;

  case 193:
#line 555 "filter/config.Y"
                          { (yyval.v).type = T_IP; (yyval.v).val.ip = ipa_from_ip4((yyvsp[0].ip4)); }
#line 7160 "obj/conf/cf-parse.tab.c"
    break;

  case 194:
#line 556 "filter/config.Y"
                          { (yyval.v).type = T_IP; (yyval.v).val.ip = ipa_from_ip6((yyvsp[0].ip6)); }
#line 7166 "obj/conf/cf-parse.tab.c"
    break;

  case 195:
#line 569 "filter/config.Y"
          { (yyval.v).type = T_INT; (yyval.v).val.i = (yyvsp[0].i); }
#line 7172 "obj/conf/cf-parse.tab.c"
    break;

  case 196:
#line 570 "filter/config.Y"
          { (yyval.v) = (yyvsp[0].v); }
#line 7178 "obj/conf/cf-parse.tab.c"
    break;

  case 197:
#line 571 "filter/config.Y"
          { (yyval.v).type = T_RD; (yyval.v).val.ec = (yyvsp[0].i64); }
#line 7184 "obj/conf/cf-parse.tab.c"
    break;

  case 198:
#line 572 "filter/config.Y"
          { (yyval.v).type = pair_a((yyvsp[0].i)); (yyval.v).val.i = pair_b((yyvsp[0].i)); }
#line 7190 "obj/conf/cf-parse.tab.c"
    break;

  case 199:
#line 573 "filter/config.Y"
                {
     if (f_eval(f_linearize((yyvsp[-1].x), 1), cfg_mem, &((yyval.v))) > F_RETURN) cf_error("Runtime error");
     if (!f_valid_set_type((yyval.v).type)) cf_error("Set-incompatible type");
   }
#line 7199 "obj/conf/cf-parse.tab.c"
    break;

  case 200:
#line 577 "filter/config.Y"
                {
     cf_assert_symbol((yyvsp[0].s), SYM_CONSTANT);
     if (!f_valid_set_type(SYM_TYPE((yyvsp[0].s)))) cf_error("%s: set-incompatible type", (yyvsp[0].s)->name);
     (yyval.v) = *(yyvsp[0].s)->val;
   }
#line 7209 "obj/conf/cf-parse.tab.c"
    break;

  case 201:
#line 585 "filter/config.Y"
         { (yyval.v).type = T_INT; (yyval.v).val.i = (yyvsp[0].i); }
#line 7215 "obj/conf/cf-parse.tab.c"
    break;

  case 202:
#line 586 "filter/config.Y"
                { (yyval.v).type = T_INT; (yyval.v).val.i = f_eval_int(f_linearize((yyvsp[-1].x), 1)); }
#line 7221 "obj/conf/cf-parse.tab.c"
    break;

  case 203:
#line 587 "filter/config.Y"
         { (yyval.v) = (yyvsp[0].v); }
#line 7227 "obj/conf/cf-parse.tab.c"
    break;

  case 204:
#line 588 "filter/config.Y"
         { (yyval.v).type = pair_a((yyvsp[0].i)); (yyval.v).val.i = pair_b((yyvsp[0].i)); }
#line 7233 "obj/conf/cf-parse.tab.c"
    break;

  case 205:
#line 592 "filter/config.Y"
        { (yyval.i32) = f_eval_int(f_linearize((yyvsp[0].x), 1)); }
#line 7239 "obj/conf/cf-parse.tab.c"
    break;

  case 206:
#line 595 "filter/config.Y"
                                        { (yyval.e) = f_new_pair_item((yyvsp[-3].i32), (yyvsp[-3].i32), (yyvsp[-1].i32), (yyvsp[-1].i32)); }
#line 7245 "obj/conf/cf-parse.tab.c"
    break;

  case 207:
#line 596 "filter/config.Y"
                                        { (yyval.e) = f_new_pair_item((yyvsp[-5].i32), (yyvsp[-5].i32), (yyvsp[-3].i32), (yyvsp[-1].i32)); }
#line 7251 "obj/conf/cf-parse.tab.c"
    break;

  case 208:
#line 597 "filter/config.Y"
                                        { (yyval.e) = f_new_pair_item((yyvsp[-3].i32), (yyvsp[-3].i32), 0, CC_ALL); }
#line 7257 "obj/conf/cf-parse.tab.c"
    break;

  case 209:
#line 598 "filter/config.Y"
                                        { (yyval.e) = f_new_pair_set((yyvsp[-5].i32), (yyvsp[-3].i32), (yyvsp[-1].i32), (yyvsp[-1].i32)); }
#line 7263 "obj/conf/cf-parse.tab.c"
    break;

  case 210:
#line 599 "filter/config.Y"
                                             { (yyval.e) = f_new_pair_set((yyvsp[-7].i32), (yyvsp[-5].i32), (yyvsp[-3].i32), (yyvsp[-1].i32)); }
#line 7269 "obj/conf/cf-parse.tab.c"
    break;

  case 211:
#line 600 "filter/config.Y"
                                        { (yyval.e) = f_new_pair_item((yyvsp[-5].i32), (yyvsp[-3].i32), 0, CC_ALL); }
#line 7275 "obj/conf/cf-parse.tab.c"
    break;

  case 212:
#line 601 "filter/config.Y"
                                        { (yyval.e) = f_new_pair_set(0, CC_ALL, (yyvsp[-1].i32), (yyvsp[-1].i32)); }
#line 7281 "obj/conf/cf-parse.tab.c"
    break;

  case 213:
#line 602 "filter/config.Y"
                                        { (yyval.e) = f_new_pair_set(0, CC_ALL, (yyvsp[-3].i32), (yyvsp[-1].i32)); }
#line 7287 "obj/conf/cf-parse.tab.c"
    break;

  case 214:
#line 603 "filter/config.Y"
                                        { (yyval.e) = f_new_pair_item(0, CC_ALL, 0, CC_ALL); }
#line 7293 "obj/conf/cf-parse.tab.c"
    break;

  case 215:
#line 605 "filter/config.Y"
   { (yyval.e) = f_new_pair_item((yyvsp[-9].i32), (yyvsp[-3].i32), (yyvsp[-7].i32), (yyvsp[-1].i32)); }
#line 7299 "obj/conf/cf-parse.tab.c"
    break;

  case 216:
#line 609 "filter/config.Y"
      { (yyval.ecs) = EC_RT; }
#line 7305 "obj/conf/cf-parse.tab.c"
    break;

  case 217:
#line 610 "filter/config.Y"
      { (yyval.ecs) = EC_RO; }
#line 7311 "obj/conf/cf-parse.tab.c"
    break;

  case 218:
#line 611 "filter/config.Y"
               { (yyval.ecs) = (yyvsp[0].i); }
#line 7317 "obj/conf/cf-parse.tab.c"
    break;

  case 219:
#line 612 "filter/config.Y"
           { (yyval.ecs) = EC_GENERIC; }
#line 7323 "obj/conf/cf-parse.tab.c"
    break;

  case 220:
#line 616 "filter/config.Y"
                                                { (yyval.e) = f_new_ec_item((yyvsp[-5].ecs), 0, (yyvsp[-3].i32), (yyvsp[-1].i32), (yyvsp[-1].i32)); }
#line 7329 "obj/conf/cf-parse.tab.c"
    break;

  case 221:
#line 617 "filter/config.Y"
                                                { (yyval.e) = f_new_ec_item((yyvsp[-7].ecs), 0, (yyvsp[-5].i32), (yyvsp[-3].i32), (yyvsp[-1].i32)); }
#line 7335 "obj/conf/cf-parse.tab.c"
    break;

  case 222:
#line 618 "filter/config.Y"
                                                { (yyval.e) = f_new_ec_item((yyvsp[-5].ecs), 0, (yyvsp[-3].i32), 0, EC_ALL); }
#line 7341 "obj/conf/cf-parse.tab.c"
    break;

  case 223:
#line 622 "filter/config.Y"
                                            { (yyval.e) = f_new_lc_item((yyvsp[-5].i32), (yyvsp[-5].i32), (yyvsp[-3].i32), (yyvsp[-3].i32), (yyvsp[-1].i32), (yyvsp[-1].i32)); }
#line 7347 "obj/conf/cf-parse.tab.c"
    break;

  case 224:
#line 623 "filter/config.Y"
                                            { (yyval.e) = f_new_lc_item((yyvsp[-7].i32), (yyvsp[-7].i32), (yyvsp[-5].i32), (yyvsp[-5].i32), (yyvsp[-3].i32), (yyvsp[-1].i32)); }
#line 7353 "obj/conf/cf-parse.tab.c"
    break;

  case 225:
#line 624 "filter/config.Y"
                                            { (yyval.e) = f_new_lc_item((yyvsp[-5].i32), (yyvsp[-5].i32), (yyvsp[-3].i32), (yyvsp[-3].i32), 0, LC_ALL); }
#line 7359 "obj/conf/cf-parse.tab.c"
    break;

  case 226:
#line 625 "filter/config.Y"
                                            { (yyval.e) = f_new_lc_item((yyvsp[-7].i32), (yyvsp[-7].i32), (yyvsp[-5].i32), (yyvsp[-3].i32), 0, LC_ALL); }
#line 7365 "obj/conf/cf-parse.tab.c"
    break;

  case 227:
#line 626 "filter/config.Y"
                                            { (yyval.e) = f_new_lc_item((yyvsp[-5].i32), (yyvsp[-5].i32), 0, LC_ALL, 0, LC_ALL); }
#line 7371 "obj/conf/cf-parse.tab.c"
    break;

  case 228:
#line 627 "filter/config.Y"
                                            { (yyval.e) = f_new_lc_item((yyvsp[-7].i32), (yyvsp[-5].i32), 0, LC_ALL, 0, LC_ALL); }
#line 7377 "obj/conf/cf-parse.tab.c"
    break;

  case 229:
#line 628 "filter/config.Y"
                                            { (yyval.e) = f_new_lc_item(0, LC_ALL, 0, LC_ALL, 0, LC_ALL); }
#line 7383 "obj/conf/cf-parse.tab.c"
    break;

  case 230:
#line 630 "filter/config.Y"
   { (yyval.e) = f_new_lc_item((yyvsp[-13].i32), (yyvsp[-5].i32), (yyvsp[-11].i32), (yyvsp[-3].i32), (yyvsp[-9].i32), (yyvsp[-1].i32)); }
#line 7389 "obj/conf/cf-parse.tab.c"
    break;

  case 234:
#line 637 "filter/config.Y"
            { (yyval.e) = f_new_item((yyvsp[0].v), (yyvsp[0].v)); }
#line 7395 "obj/conf/cf-parse.tab.c"
    break;

  case 235:
#line 638 "filter/config.Y"
                          { (yyval.e) = f_new_item((yyvsp[-2].v), (yyvsp[0].v)); }
#line 7401 "obj/conf/cf-parse.tab.c"
    break;

  case 239:
#line 645 "filter/config.Y"
               { (yyval.e) = f_new_item((yyvsp[0].v), (yyvsp[0].v)); }
#line 7407 "obj/conf/cf-parse.tab.c"
    break;

  case 240:
#line 646 "filter/config.Y"
                                { (yyval.e) = f_new_item((yyvsp[-2].v), (yyvsp[0].v)); }
#line 7413 "obj/conf/cf-parse.tab.c"
    break;

  case 242:
#line 651 "filter/config.Y"
                          { (yyval.e) = f_merge_items((yyvsp[-2].e), (yyvsp[0].e)); }
#line 7419 "obj/conf/cf-parse.tab.c"
    break;

  case 244:
#line 656 "filter/config.Y"
                                { (yyval.e) = f_merge_items((yyvsp[-2].e), (yyvsp[0].e)); }
#line 7425 "obj/conf/cf-parse.tab.c"
    break;

  case 245:
#line 660 "filter/config.Y"
                { (yyval.px).net = (yyvsp[0].net); (yyval.px).lo = (yyvsp[0].net).pxlen; (yyval.px).hi = (yyvsp[0].net).pxlen; }
#line 7431 "obj/conf/cf-parse.tab.c"
    break;

  case 246:
#line 661 "filter/config.Y"
                { (yyval.px).net = (yyvsp[-1].net); (yyval.px).lo = (yyvsp[-1].net).pxlen; (yyval.px).hi = net_max_prefix_length[(yyvsp[-1].net).type]; }
#line 7437 "obj/conf/cf-parse.tab.c"
    break;

  case 247:
#line 662 "filter/config.Y"
                { (yyval.px).net = (yyvsp[-1].net); (yyval.px).lo = 0; (yyval.px).hi = (yyvsp[-1].net).pxlen; }
#line 7443 "obj/conf/cf-parse.tab.c"
    break;

  case 248:
#line 663 "filter/config.Y"
                               {
     (yyval.px).net = (yyvsp[-5].net); (yyval.px).lo = (yyvsp[-3].i); (yyval.px).hi = (yyvsp[-1].i);
     if (((yyvsp[-3].i) > (yyvsp[-1].i)) || ((yyvsp[-1].i) > net_max_prefix_length[(yyvsp[-5].net).type]))
       cf_error("Invalid prefix pattern range: {%u, %u}", (yyvsp[-3].i), (yyvsp[-1].i));
   }
#line 7453 "obj/conf/cf-parse.tab.c"
    break;

  case 249:
#line 671 "filter/config.Y"
           { (yyval.trie) = f_new_trie(cfg_mem, 0); trie_add_prefix((yyval.trie), &((yyvsp[0].px).net), (yyvsp[0].px).lo, (yyvsp[0].px).hi); }
#line 7459 "obj/conf/cf-parse.tab.c"
    break;

  case 250:
#line 672 "filter/config.Y"
                           { (yyval.trie) = (yyvsp[-2].trie); if (!trie_add_prefix((yyval.trie), &((yyvsp[0].px).net), (yyvsp[0].px).lo, (yyvsp[0].px).hi)) cf_error("Mixed IPv4/IPv6 prefixes in prefix set"); }
#line 7465 "obj/conf/cf-parse.tab.c"
    break;

  case 251:
#line 675 "filter/config.Y"
                         { (yyval.e) = NULL; }
#line 7471 "obj/conf/cf-parse.tab.c"
    break;

  case 252:
#line 676 "filter/config.Y"
                                             {
     /* Fill data fields */
     struct f_tree *t;
     for (t = (yyvsp[-2].e); t; t = t->left)
       t->data = (yyvsp[0].x);
     (yyval.e) = f_merge_items((yyvsp[-3].e), (yyvsp[-2].e));
   }
#line 7483 "obj/conf/cf-parse.tab.c"
    break;

  case 253:
#line 683 "filter/config.Y"
                                   {
     struct f_tree *t = f_new_tree();
     t->from.type = t->to.type = T_VOID;
     t->right = t;
     t->data = (yyvsp[0].x);
     (yyval.e) = f_merge_items((yyvsp[-2].e), t);
 }
#line 7495 "obj/conf/cf-parse.tab.c"
    break;

  case 254:
#line 693 "filter/config.Y"
                { (yyval.x) = (yyvsp[0].x); }
#line 7501 "obj/conf/cf-parse.tab.c"
    break;

  case 255:
#line 694 "filter/config.Y"
                { (yyval.x) = (yyvsp[-1].x); }
#line 7507 "obj/conf/cf-parse.tab.c"
    break;

  case 256:
#line 698 "filter/config.Y"
                         { (yyval.x) = (yyvsp[-1].x); }
#line 7513 "obj/conf/cf-parse.tab.c"
    break;

  case 257:
#line 702 "filter/config.Y"
                                { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .asn = (yyvsp[-1].i), .kind = PM_ASN, }, }); (yyval.x)->next = (yyvsp[0].x);  }
#line 7519 "obj/conf/cf-parse.tab.c"
    break;

  case 258:
#line 703 "filter/config.Y"
                                { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .from = (yyvsp[-3].i), .to = (yyvsp[-1].i), .kind = PM_ASN_RANGE }, }); (yyval.x)->next = (yyvsp[0].x); }
#line 7525 "obj/conf/cf-parse.tab.c"
    break;

  case 259:
#line 704 "filter/config.Y"
                         { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .set = NULL, .kind = PM_ASN_SET }, }); (yyval.x)->next = (yyvsp[0].x); }
#line 7531 "obj/conf/cf-parse.tab.c"
    break;

  case 260:
#line 705 "filter/config.Y"
                                   {
   if ((yyvsp[-2].e)->from.type != T_INT) cf_error("Only integer sets allowed in path mask");
   (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .set = build_tree((yyvsp[-2].e)), .kind = PM_ASN_SET }, }); (yyval.x)->next = (yyvsp[0].x);
 }
#line 7540 "obj/conf/cf-parse.tab.c"
    break;

  case 261:
#line 709 "filter/config.Y"
                                { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .kind = PM_ASTERISK }, }); (yyval.x)->next = (yyvsp[0].x); }
#line 7546 "obj/conf/cf-parse.tab.c"
    break;

  case 262:
#line 710 "filter/config.Y"
                                { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .kind = PM_QUESTION }, }); (yyval.x)->next = (yyvsp[0].x); }
#line 7552 "obj/conf/cf-parse.tab.c"
    break;

  case 263:
#line 711 "filter/config.Y"
                                { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .kind = PM_LOOP }, }); (yyval.x)->next = (yyvsp[0].x); }
#line 7558 "obj/conf/cf-parse.tab.c"
    break;

  case 264:
#line 712 "filter/config.Y"
                                { (yyval.x) = (yyvsp[-1].x); (yyval.x)->next = (yyvsp[0].x); }
#line 7564 "obj/conf/cf-parse.tab.c"
    break;

  case 265:
#line 713 "filter/config.Y"
                                { (yyval.x) = NULL; }
#line 7570 "obj/conf/cf-parse.tab.c"
    break;

  case 266:
#line 717 "filter/config.Y"
          { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_INT, .val.i = (yyvsp[0].i), }); }
#line 7576 "obj/conf/cf-parse.tab.c"
    break;

  case 267:
#line 718 "filter/config.Y"
          { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_BOOL, .val.i = 1, }); }
#line 7582 "obj/conf/cf-parse.tab.c"
    break;

  case 268:
#line 719 "filter/config.Y"
          { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_BOOL, .val.i = 0, }); }
#line 7588 "obj/conf/cf-parse.tab.c"
    break;

  case 269:
#line 720 "filter/config.Y"
          { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_STRING, .val.s = (yyvsp[0].t), }); }
#line 7594 "obj/conf/cf-parse.tab.c"
    break;

  case 270:
#line 721 "filter/config.Y"
          { (yyval.x) = f_new_inst(FI_CONSTANT, (yyvsp[0].v)); }
#line 7600 "obj/conf/cf-parse.tab.c"
    break;

  case 271:
#line 722 "filter/config.Y"
          { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_RD, .val.ec = (yyvsp[0].i64), }); }
#line 7606 "obj/conf/cf-parse.tab.c"
    break;

  case 272:
#line 723 "filter/config.Y"
          { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_NET, .val.net = (yyvsp[0].net_ptr), }); }
#line 7612 "obj/conf/cf-parse.tab.c"
    break;

  case 273:
#line 724 "filter/config.Y"
           { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_SET, .val.t = NULL, }); }
#line 7618 "obj/conf/cf-parse.tab.c"
    break;

  case 274:
#line 725 "filter/config.Y"
                     {
     DBG( "We've got a set here..." );
     (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_SET, .val.t = build_tree((yyvsp[-1].e)), });
     DBG( "ook\n" );
 }
#line 7628 "obj/conf/cf-parse.tab.c"
    break;

  case 275:
#line 730 "filter/config.Y"
                       { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PREFIX_SET, .val.ti = (yyvsp[-1].trie), }); }
#line 7634 "obj/conf/cf-parse.tab.c"
    break;

  case 276:
#line 731 "filter/config.Y"
          { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { .type = (yyvsp[0].i) >> 16, .val.i = (yyvsp[0].i) & 0xffff, }); }
#line 7640 "obj/conf/cf-parse.tab.c"
    break;

  case 277:
#line 735 "filter/config.Y"
                         { (yyval.x) = f_new_inst(FI_PAIR_CONSTRUCT, (yyvsp[-3].x), (yyvsp[-1].x)); }
#line 7646 "obj/conf/cf-parse.tab.c"
    break;

  case 278:
#line 736 "filter/config.Y"
                                     { (yyval.x) = f_new_inst(FI_EC_CONSTRUCT, (yyvsp[-3].x), (yyvsp[-1].x), (yyvsp[-5].ecs)); }
#line 7652 "obj/conf/cf-parse.tab.c"
    break;

  case 279:
#line 737 "filter/config.Y"
                                  { (yyval.x) = f_new_inst(FI_LC_CONSTRUCT, (yyvsp[-5].x), (yyvsp[-3].x), (yyvsp[-1].x)); }
#line 7658 "obj/conf/cf-parse.tab.c"
    break;

  case 280:
#line 738 "filter/config.Y"
            { (yyval.x) = f_new_inst(FI_PATHMASK_CONSTRUCT, (yyvsp[0].x)); }
#line 7664 "obj/conf/cf-parse.tab.c"
    break;

  case 281:
#line 743 "filter/config.Y"
                      { (yyval.x) = NULL; }
#line 7670 "obj/conf/cf-parse.tab.c"
    break;

  case 282:
#line 744 "filter/config.Y"
        { (yyval.x) = (yyvsp[0].x); }
#line 7676 "obj/conf/cf-parse.tab.c"
    break;

  case 283:
#line 745 "filter/config.Y"
                     { (yyval.x) = (yyvsp[0].x); (yyval.x)->next = (yyvsp[-2].x); }
#line 7682 "obj/conf/cf-parse.tab.c"
    break;

  case 284:
#line 749 "filter/config.Y"
   {
     if ((yyvsp[-3].s)->class != SYM_FUNCTION)
       cf_error("You can't call something which is not a function. Really.");

     /* Revert the var_list */
     struct f_inst *args = NULL;
     while ((yyvsp[-1].x)) {
       struct f_inst *tmp = (yyvsp[-1].x);
       (yyvsp[-1].x) = (yyvsp[-1].x)->next;

       tmp->next = args;
       args = tmp;
     }

     (yyval.x) = f_new_inst(FI_CALL, args, (yyvsp[-3].s));
   }
#line 7703 "obj/conf/cf-parse.tab.c"
    break;

  case 285:
#line 768 "filter/config.Y"
  {
    switch ((yyvsp[0].s)->class) {
      case SYM_CONSTANT_RANGE:
	(yyval.x) = f_new_inst(FI_CONSTANT, *((yyvsp[0].s)->val));
	break;
      case SYM_VARIABLE_RANGE:
	(yyval.x) = f_new_inst(FI_VAR_GET, (yyvsp[0].s));
	break;
      case SYM_ATTRIBUTE:
	(yyval.x) = f_new_inst(FI_EA_GET, *(yyvsp[0].s)->attribute);
	break;
      default:
	cf_error("Can't get value of symbol %s", (yyvsp[0].s)->name);
    }
  }
#line 7723 "obj/conf/cf-parse.tab.c"
    break;

  case 286:
#line 786 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_IP,         SA_FROM,	0); }
#line 7729 "obj/conf/cf-parse.tab.c"
    break;

  case 287:
#line 787 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_IP,         SA_GW,	0); }
#line 7735 "obj/conf/cf-parse.tab.c"
    break;

  case 288:
#line 788 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_NET,	  SA_NET,	1); }
#line 7741 "obj/conf/cf-parse.tab.c"
    break;

  case 289:
#line 789 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_STRING,     SA_PROTO,	1); }
#line 7747 "obj/conf/cf-parse.tab.c"
    break;

  case 290:
#line 790 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_ENUM_RTS,   SA_SOURCE,	1); }
#line 7753 "obj/conf/cf-parse.tab.c"
    break;

  case 291:
#line 791 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_ENUM_SCOPE, SA_SCOPE,	0); }
#line 7759 "obj/conf/cf-parse.tab.c"
    break;

  case 292:
#line 792 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_ENUM_RTD,   SA_DEST,	0); }
#line 7765 "obj/conf/cf-parse.tab.c"
    break;

  case 293:
#line 793 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_STRING,     SA_IFNAME,	0); }
#line 7771 "obj/conf/cf-parse.tab.c"
    break;

  case 294:
#line 794 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_INT,        SA_IFINDEX,	1); }
#line 7777 "obj/conf/cf-parse.tab.c"
    break;

  case 295:
#line 795 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_INT,        SA_WEIGHT,	0); }
#line 7783 "obj/conf/cf-parse.tab.c"
    break;

  case 296:
#line 796 "filter/config.Y"
              { (yyval.fsa) = f_new_static_attr(T_INT,	  SA_PREF,	0); }
#line 7789 "obj/conf/cf-parse.tab.c"
    break;

  case 297:
#line 797 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_INT,        SA_GW_MPLS,	0); }
#line 7795 "obj/conf/cf-parse.tab.c"
    break;

  case 298:
#line 798 "filter/config.Y"
           { (yyval.fsa) = f_new_static_attr(T_BOOL,       SA_ONLINK,	0); }
#line 7801 "obj/conf/cf-parse.tab.c"
    break;

  case 299:
#line 802 "filter/config.Y"
                        { (yyval.x) = (yyvsp[-1].x); }
#line 7807 "obj/conf/cf-parse.tab.c"
    break;

  case 300:
#line 803 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_ADD, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7813 "obj/conf/cf-parse.tab.c"
    break;

  case 301:
#line 804 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_SUBTRACT, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7819 "obj/conf/cf-parse.tab.c"
    break;

  case 302:
#line 805 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_MULTIPLY, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7825 "obj/conf/cf-parse.tab.c"
    break;

  case 303:
#line 806 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_DIVIDE, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7831 "obj/conf/cf-parse.tab.c"
    break;

  case 304:
#line 807 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_AND, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7837 "obj/conf/cf-parse.tab.c"
    break;

  case 305:
#line 808 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_OR, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7843 "obj/conf/cf-parse.tab.c"
    break;

  case 306:
#line 809 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_EQ, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7849 "obj/conf/cf-parse.tab.c"
    break;

  case 307:
#line 810 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_NEQ, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7855 "obj/conf/cf-parse.tab.c"
    break;

  case 308:
#line 811 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_LT, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7861 "obj/conf/cf-parse.tab.c"
    break;

  case 309:
#line 812 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_LTE, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7867 "obj/conf/cf-parse.tab.c"
    break;

  case 310:
#line 813 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_LT, (yyvsp[0].x), (yyvsp[-2].x)); }
#line 7873 "obj/conf/cf-parse.tab.c"
    break;

  case 311:
#line 814 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_LTE, (yyvsp[0].x), (yyvsp[-2].x)); }
#line 7879 "obj/conf/cf-parse.tab.c"
    break;

  case 312:
#line 815 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_MATCH, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7885 "obj/conf/cf-parse.tab.c"
    break;

  case 313:
#line 816 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_NOT_MATCH, (yyvsp[-2].x), (yyvsp[0].x)); }
#line 7891 "obj/conf/cf-parse.tab.c"
    break;

  case 314:
#line 817 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_NOT, (yyvsp[0].x)); }
#line 7897 "obj/conf/cf-parse.tab.c"
    break;

  case 315:
#line 818 "filter/config.Y"
                        { (yyval.x) = f_new_inst(FI_DEFINED, (yyvsp[-1].x)); }
#line 7903 "obj/conf/cf-parse.tab.c"
    break;

  case 316:
#line 820 "filter/config.Y"
                  { (yyval.x) = (yyvsp[0].x); }
#line 7909 "obj/conf/cf-parse.tab.c"
    break;

  case 317:
#line 821 "filter/config.Y"
            { (yyval.x) = (yyvsp[0].x); }
#line 7915 "obj/conf/cf-parse.tab.c"
    break;

  case 318:
#line 822 "filter/config.Y"
               { (yyval.x) = (yyvsp[0].x); }
#line 7921 "obj/conf/cf-parse.tab.c"
    break;

  case 319:
#line 824 "filter/config.Y"
               { (yyval.x) = f_new_inst(FI_RTA_GET, (yyvsp[0].fsa)); }
#line 7927 "obj/conf/cf-parse.tab.c"
    break;

  case 320:
#line 826 "filter/config.Y"
                { (yyval.x) = f_new_inst(FI_EA_GET, (yyvsp[0].fda)); }
#line 7933 "obj/conf/cf-parse.tab.c"
    break;

  case 321:
#line 828 "filter/config.Y"
                  { (yyval.x) = f_new_inst(FI_IS_V4, (yyvsp[-2].x)); }
#line 7939 "obj/conf/cf-parse.tab.c"
    break;

  case 322:
#line 829 "filter/config.Y"
                 { (yyval.x) = f_new_inst(FI_TYPE, (yyvsp[-2].x)); }
#line 7945 "obj/conf/cf-parse.tab.c"
    break;

  case 323:
#line 830 "filter/config.Y"
               { (yyval.x) = f_new_inst(FI_IP, (yyvsp[-2].x)); }
#line 7951 "obj/conf/cf-parse.tab.c"
    break;

  case 324:
#line 831 "filter/config.Y"
               { (yyval.x) = f_new_inst(FI_ROUTE_DISTINGUISHER, (yyvsp[-2].x)); }
#line 7957 "obj/conf/cf-parse.tab.c"
    break;

  case 325:
#line 832 "filter/config.Y"
                { (yyval.x) = f_new_inst(FI_LENGTH, (yyvsp[-2].x)); }
#line 7963 "obj/conf/cf-parse.tab.c"
    break;

  case 326:
#line 833 "filter/config.Y"
                   { (yyval.x) = f_new_inst(FI_ROA_MAXLEN, (yyvsp[-2].x)); }
#line 7969 "obj/conf/cf-parse.tab.c"
    break;

  case 327:
#line 834 "filter/config.Y"
                { (yyval.x) = f_new_inst(FI_ASN, (yyvsp[-2].x)); }
#line 7975 "obj/conf/cf-parse.tab.c"
    break;

  case 328:
#line 835 "filter/config.Y"
                { (yyval.x) = f_new_inst(FI_NET_SRC, (yyvsp[-2].x)); }
#line 7981 "obj/conf/cf-parse.tab.c"
    break;

  case 329:
#line 836 "filter/config.Y"
                { (yyval.x) = f_new_inst(FI_NET_DST, (yyvsp[-2].x)); }
#line 7987 "obj/conf/cf-parse.tab.c"
    break;

  case 330:
#line 837 "filter/config.Y"
                              { (yyval.x) = f_new_inst(FI_IP_MASK, (yyvsp[-5].x), (yyvsp[-1].x)); }
#line 7993 "obj/conf/cf-parse.tab.c"
    break;

  case 331:
#line 838 "filter/config.Y"
                  { (yyval.x) = f_new_inst(FI_AS_PATH_FIRST, (yyvsp[-2].x)); }
#line 7999 "obj/conf/cf-parse.tab.c"
    break;

  case 332:
#line 839 "filter/config.Y"
                  { (yyval.x) = f_new_inst(FI_AS_PATH_LAST, (yyvsp[-2].x)); }
#line 8005 "obj/conf/cf-parse.tab.c"
    break;

  case 333:
#line 840 "filter/config.Y"
                                { (yyval.x) = f_new_inst(FI_AS_PATH_LAST_NAG, (yyvsp[-2].x)); }
#line 8011 "obj/conf/cf-parse.tab.c"
    break;

  case 334:
#line 841 "filter/config.Y"
                 { (yyval.x) = f_new_inst(FI_PAIR_DATA, (yyvsp[-2].x)); }
#line 8017 "obj/conf/cf-parse.tab.c"
    break;

  case 335:
#line 842 "filter/config.Y"
                  { (yyval.x) = f_new_inst(FI_LC_DATA1, (yyvsp[-2].x)); }
#line 8023 "obj/conf/cf-parse.tab.c"
    break;

  case 336:
#line 843 "filter/config.Y"
                  { (yyval.x) = f_new_inst(FI_LC_DATA2, (yyvsp[-2].x)); }
#line 8029 "obj/conf/cf-parse.tab.c"
    break;

  case 337:
#line 844 "filter/config.Y"
                 { (yyval.x) = f_new_inst(FI_MIN, (yyvsp[-2].x)); }
#line 8035 "obj/conf/cf-parse.tab.c"
    break;

  case 338:
#line 845 "filter/config.Y"
                 { (yyval.x) = f_new_inst(FI_MAX, (yyvsp[-2].x)); }
#line 8041 "obj/conf/cf-parse.tab.c"
    break;

  case 339:
#line 855 "filter/config.Y"
                 { (yyval.x) = f_new_inst(FI_CONSTANT, f_const_empty_path); }
#line 8047 "obj/conf/cf-parse.tab.c"
    break;

  case 340:
#line 856 "filter/config.Y"
                 { (yyval.x) = f_new_inst(FI_CONSTANT, f_const_empty_clist); }
#line 8053 "obj/conf/cf-parse.tab.c"
    break;

  case 341:
#line 857 "filter/config.Y"
                         { (yyval.x) = f_new_inst(FI_CONSTANT, f_const_empty_eclist); }
#line 8059 "obj/conf/cf-parse.tab.c"
    break;

  case 342:
#line 858 "filter/config.Y"
                                 { (yyval.x) = f_new_inst(FI_CONSTANT, f_const_empty_lclist); }
#line 8065 "obj/conf/cf-parse.tab.c"
    break;

  case 343:
#line 859 "filter/config.Y"
                                 { (yyval.x) = f_new_inst(FI_PATH_PREPEND, (yyvsp[-3].x), (yyvsp[-1].x)); }
#line 8071 "obj/conf/cf-parse.tab.c"
    break;

  case 344:
#line 860 "filter/config.Y"
                             { (yyval.x) = f_new_inst(FI_CLIST_ADD, (yyvsp[-3].x), (yyvsp[-1].x)); }
#line 8077 "obj/conf/cf-parse.tab.c"
    break;

  case 345:
#line 861 "filter/config.Y"
                                { (yyval.x) = f_new_inst(FI_CLIST_DEL, (yyvsp[-3].x), (yyvsp[-1].x)); }
#line 8083 "obj/conf/cf-parse.tab.c"
    break;

  case 346:
#line 862 "filter/config.Y"
                                { (yyval.x) = f_new_inst(FI_CLIST_FILTER, (yyvsp[-3].x), (yyvsp[-1].x)); }
#line 8089 "obj/conf/cf-parse.tab.c"
    break;

  case 347:
#line 864 "filter/config.Y"
                            { (yyval.x) = f_new_inst(FI_ROA_CHECK_IMPLICIT, (yyvsp[-1].r)); }
#line 8095 "obj/conf/cf-parse.tab.c"
    break;

  case 348:
#line 865 "filter/config.Y"
                                              { (yyval.x) = f_new_inst(FI_ROA_CHECK_EXPLICIT, (yyvsp[-3].x), (yyvsp[-1].x), (yyvsp[-5].r)); }
#line 8101 "obj/conf/cf-parse.tab.c"
    break;

  case 349:
#line 867 "filter/config.Y"
                       {  (yyval.x) = f_new_inst(FI_FORMAT, (yyvsp[-1].x)); }
#line 8107 "obj/conf/cf-parse.tab.c"
    break;

  case 351:
#line 875 "filter/config.Y"
          { (yyval.fret) = F_ACCEPT; }
#line 8113 "obj/conf/cf-parse.tab.c"
    break;

  case 352:
#line 876 "filter/config.Y"
          { (yyval.fret) = F_REJECT; }
#line 8119 "obj/conf/cf-parse.tab.c"
    break;

  case 353:
#line 877 "filter/config.Y"
         { (yyval.fret) = F_ERROR; }
#line 8125 "obj/conf/cf-parse.tab.c"
    break;

  case 354:
#line 880 "filter/config.Y"
                        { (yyval.x) = NULL; }
#line 8131 "obj/conf/cf-parse.tab.c"
    break;

  case 355:
#line 881 "filter/config.Y"
        { (yyval.x) = (yyvsp[0].x); }
#line 8137 "obj/conf/cf-parse.tab.c"
    break;

  case 356:
#line 882 "filter/config.Y"
                       {
     ASSERT((yyvsp[-2].x));
     ASSERT((yyvsp[-2].x)->next == NULL);
     (yyvsp[-2].x)->next = (yyvsp[0].x);
     (yyval.x) = (yyvsp[-2].x);
   }
#line 8148 "obj/conf/cf-parse.tab.c"
    break;

  case 357:
#line 891 "filter/config.Y"
               { (yyval.x) = f_new_inst(FI_CONSTANT, (struct f_val) { }); }
#line 8154 "obj/conf/cf-parse.tab.c"
    break;

  case 358:
#line 892 "filter/config.Y"
            { (yyval.x) = (yyvsp[0].x); }
#line 8160 "obj/conf/cf-parse.tab.c"
    break;

  case 359:
#line 896 "filter/config.Y"
                            {
     struct symbol *sym = cf_define_symbol((yyvsp[-2].s), SYM_VARIABLE | (yyvsp[-3].i), offset, f_new_var(sym_->scope));
     (yyval.x) = f_new_inst(FI_VAR_INIT, (yyvsp[-1].x), sym);
   }
#line 8169 "obj/conf/cf-parse.tab.c"
    break;

  case 360:
#line 902 "filter/config.Y"
               { (yyval.s) = cf_define_symbol((yyvsp[0].s), SYM_VARIABLE | (yyvsp[-1].i), offset, f_new_var(sym_->scope)); }
#line 8175 "obj/conf/cf-parse.tab.c"
    break;

  case 361:
#line 903 "filter/config.Y"
                { (yyval.s) = (yyvsp[0].s); cf_assert_symbol((yyvsp[0].s), SYM_VARIABLE); }
#line 8181 "obj/conf/cf-parse.tab.c"
    break;

  case 362:
#line 907 "filter/config.Y"
                       {
     (yyval.x) = (yyvsp[-1].x);
   }
#line 8189 "obj/conf/cf-parse.tab.c"
    break;

  case 363:
#line 910 "filter/config.Y"
                    {
     (yyval.x) = f_new_inst(FI_CONDITION, (yyvsp[-2].x), (yyvsp[0].x), NULL);
   }
#line 8197 "obj/conf/cf-parse.tab.c"
    break;

  case 364:
#line 913 "filter/config.Y"
                             {
     (yyval.x) = f_new_inst(FI_CONDITION, (yyvsp[-4].x), (yyvsp[-2].x), (yyvsp[0].x));
   }
#line 8205 "obj/conf/cf-parse.tab.c"
    break;

  case 365:
#line 916 "filter/config.Y"
       {
     /* Reserve space for walk data on stack */
     cf_push_block_scope();
     conf_this_scope->slots += 2;
   }
#line 8215 "obj/conf/cf-parse.tab.c"
    break;

  case 366:
#line 922 "filter/config.Y"
   { conf_this_scope->active = 0; }
#line 8221 "obj/conf/cf-parse.tab.c"
    break;

  case 367:
#line 922 "filter/config.Y"
                                         { conf_this_scope->active = 1; }
#line 8227 "obj/conf/cf-parse.tab.c"
    break;

  case 368:
#line 923 "filter/config.Y"
          {
     cf_pop_block_scope();
     (yyval.x) = f_new_inst(FI_FOR_INIT, (yyvsp[-3].x), (yyvsp[-6].s));
     (yyval.x)->next = f_new_inst(FI_FOR_NEXT, (yyvsp[-6].s), (yyvsp[0].x));
   }
#line 8237 "obj/conf/cf-parse.tab.c"
    break;

  case 369:
#line 928 "filter/config.Y"
                             {
     switch ((yyvsp[-3].s)->class) {
       case SYM_VARIABLE_RANGE:
	 (yyval.x) = f_new_inst(FI_VAR_SET, (yyvsp[-1].x), (yyvsp[-3].s));
	 break;
       case SYM_ATTRIBUTE:
	 (yyval.x) = f_new_inst(FI_EA_SET, (yyvsp[-1].x), *(yyvsp[-3].s)->attribute);
	 break;
       default:
	 cf_error("Can't assign to symbol %s", (yyvsp[-3].s)->name);
     }
   }
#line 8254 "obj/conf/cf-parse.tab.c"
    break;

  case 370:
#line 940 "filter/config.Y"
                   {
     DBG( "Ook, we'll return the value\n" );
     (yyval.x) = f_new_inst(FI_RETURN, (yyvsp[-1].x));
   }
#line 8263 "obj/conf/cf-parse.tab.c"
    break;

  case 371:
#line 944 "filter/config.Y"
                             {
     (yyval.x) = f_new_inst(FI_EA_SET, (yyvsp[-1].x), (yyvsp[-3].fda));
   }
#line 8271 "obj/conf/cf-parse.tab.c"
    break;

  case 372:
#line 947 "filter/config.Y"
                            {
     if ((yyvsp[-3].fsa).readonly)
       cf_error( "This static attribute is read-only.");
     (yyval.x) = f_new_inst(FI_RTA_SET, (yyvsp[-1].x), (yyvsp[-3].fsa));
   }
#line 8281 "obj/conf/cf-parse.tab.c"
    break;

  case 373:
#line 952 "filter/config.Y"
                                  {
     (yyval.x) = f_new_inst(FI_EA_UNSET, (yyvsp[-2].fda));
   }
#line 8289 "obj/conf/cf-parse.tab.c"
    break;

  case 374:
#line 955 "filter/config.Y"
                                {
    struct f_inst *breaker = f_new_inst(FI_DIE, (yyvsp[-2].fret));
    if ((yyvsp[-1].x)) {
      struct f_inst *printer = f_new_inst(FI_PRINT, (yyvsp[-1].x));
      struct f_inst *flusher = f_new_inst(FI_FLUSH);
      printer->next = flusher;
      flusher->next = breaker;
      (yyval.x) = printer;
    } else
      (yyval.x) = breaker;
   }
#line 8305 "obj/conf/cf-parse.tab.c"
    break;

  case 375:
#line 966 "filter/config.Y"
                        {
    (yyval.x) = f_new_inst(FI_PRINT, (yyvsp[-1].x));
    (yyval.x)->next = f_new_inst(FI_FLUSH);
   }
#line 8314 "obj/conf/cf-parse.tab.c"
    break;

  case 376:
#line 970 "filter/config.Y"
                         {
    (yyval.x) = f_new_inst(FI_PRINT, (yyvsp[-1].x));
   }
#line 8322 "obj/conf/cf-parse.tab.c"
    break;

  case 377:
#line 973 "filter/config.Y"
                     { (yyval.x) = f_new_inst(FI_DROP_RESULT, (yyvsp[-1].x)); }
#line 8328 "obj/conf/cf-parse.tab.c"
    break;

  case 378:
#line 974 "filter/config.Y"
                                 {
      (yyval.x) = f_new_inst(FI_SWITCH, (yyvsp[-3].x), (yyvsp[-1].e));
   }
#line 8336 "obj/conf/cf-parse.tab.c"
    break;

  case 379:
#line 978 "filter/config.Y"
                              { (yyval.x) = f_generate_empty((yyvsp[-3].fda)); }
#line 8342 "obj/conf/cf-parse.tab.c"
    break;

  case 380:
#line 979 "filter/config.Y"
                                               { (yyval.x) = f_generate_complex( FI_PATH_PREPEND, (yyvsp[-6].fda), (yyvsp[-2].x) ); }
#line 8348 "obj/conf/cf-parse.tab.c"
    break;

  case 381:
#line 980 "filter/config.Y"
                                               { (yyval.x) = f_generate_complex( FI_CLIST_ADD, (yyvsp[-6].fda), (yyvsp[-2].x) ); }
#line 8354 "obj/conf/cf-parse.tab.c"
    break;

  case 382:
#line 981 "filter/config.Y"
                                               { (yyval.x) = f_generate_complex( FI_CLIST_DEL, (yyvsp[-6].fda), (yyvsp[-2].x) ); }
#line 8360 "obj/conf/cf-parse.tab.c"
    break;

  case 383:
#line 982 "filter/config.Y"
                                               { (yyval.x) = f_generate_complex( FI_CLIST_FILTER, (yyvsp[-6].fda), (yyvsp[-2].x) ); }
#line 8366 "obj/conf/cf-parse.tab.c"
    break;

  case 384:
#line 983 "filter/config.Y"
                                                              { (yyval.x) = assert_done((yyvsp[-3].x), (yyvsp[-4].t) + 1, (yyvsp[-2].t) - 1); }
#line 8372 "obj/conf/cf-parse.tab.c"
    break;

  case 385:
#line 984 "filter/config.Y"
                                                                               { (yyval.x) = assert_assign(&(yyvsp[-5].flv), (yyvsp[-2].x), (yyvsp[-6].t) + 1, (yyvsp[-4].t) - 1); }
#line 8378 "obj/conf/cf-parse.tab.c"
    break;

  case 386:
#line 988 "filter/config.Y"
{
  (yyval.t) = cf_text;
}
#line 8386 "obj/conf/cf-parse.tab.c"
    break;

  case 387:
#line 993 "filter/config.Y"
                { cf_assert_symbol((yyvsp[0].s), SYM_VARIABLE); (yyval.flv) = (struct f_lval) { .type = F_LVAL_VARIABLE, .sym = (yyvsp[0].s) }; }
#line 8392 "obj/conf/cf-parse.tab.c"
    break;

  case 388:
#line 994 "filter/config.Y"
               { (yyval.flv) = (struct f_lval) { .type = F_LVAL_SA, .sa = (yyvsp[0].fsa) }; }
#line 8398 "obj/conf/cf-parse.tab.c"
    break;

  case 389:
#line 995 "filter/config.Y"
                { (yyval.flv) = (struct f_lval) { .type = F_LVAL_EA, .da = (yyvsp[0].fda) }; }
#line 8404 "obj/conf/cf-parse.tab.c"
    break;

  case 391:
#line 162 "nest/config.Y"
                       { new_config->router_id = (yyvsp[-1].i32); }
#line 8410 "obj/conf/cf-parse.tab.c"
    break;

  case 392:
#line 163 "nest/config.Y"
                                 { new_config->router_id_from = this_ipatt; }
#line 8416 "obj/conf/cf-parse.tab.c"
    break;

  case 393:
#line 167 "nest/config.Y"
       { (yyval.i32) = (yyvsp[0].i); }
#line 8422 "obj/conf/cf-parse.tab.c"
    break;

  case 394:
#line 168 "nest/config.Y"
                { (yyval.i32) = f_eval_int(f_linearize((yyvsp[-1].x), 1)); }
#line 8428 "obj/conf/cf-parse.tab.c"
    break;

  case 395:
#line 169 "nest/config.Y"
       { (yyval.i32) = ip4_to_u32((yyvsp[0].ip4)); }
#line 8434 "obj/conf/cf-parse.tab.c"
    break;

  case 396:
#line 170 "nest/config.Y"
                {
     if ((yyvsp[0].s)->class == (SYM_CONSTANT | T_INT) || (yyvsp[0].s)->class == (SYM_CONSTANT | T_QUAD))
       (yyval.i32) = SYM_VAL((yyvsp[0].s)).i;
     else if (((yyvsp[0].s)->class == (SYM_CONSTANT | T_IP)) && ipa_is_ip4(SYM_VAL((yyvsp[0].s)).ip))
       (yyval.i32) = ipa_to_u32(SYM_VAL((yyvsp[0].s)).ip);
     else
       cf_error("Number or IPv4 address constant expected");
   }
#line 8447 "obj/conf/cf-parse.tab.c"
    break;

  case 398:
#line 182 "nest/config.Y"
                                     { new_config->hostname = (yyvsp[-1].t); }
#line 8453 "obj/conf/cf-parse.tab.c"
    break;

  case 400:
#line 186 "nest/config.Y"
                                        { new_config->gr_wait = (yyvsp[-1].i); }
#line 8459 "obj/conf/cf-parse.tab.c"
    break;

  case 401:
#line 192 "nest/config.Y"
        { (yyval.i) = NET_IP4; }
#line 8465 "obj/conf/cf-parse.tab.c"
    break;

  case 402:
#line 193 "nest/config.Y"
        { (yyval.i) = NET_IP6; }
#line 8471 "obj/conf/cf-parse.tab.c"
    break;

  case 403:
#line 194 "nest/config.Y"
             { (yyval.i) = NET_IP6_SADR; }
#line 8477 "obj/conf/cf-parse.tab.c"
    break;

  case 404:
#line 195 "nest/config.Y"
        { (yyval.i) = NET_VPN4; }
#line 8483 "obj/conf/cf-parse.tab.c"
    break;

  case 405:
#line 196 "nest/config.Y"
        { (yyval.i) = NET_VPN6; }
#line 8489 "obj/conf/cf-parse.tab.c"
    break;

  case 406:
#line 197 "nest/config.Y"
        { (yyval.i) = NET_ROA4; }
#line 8495 "obj/conf/cf-parse.tab.c"
    break;

  case 407:
#line 198 "nest/config.Y"
        { (yyval.i) = NET_ROA6; }
#line 8501 "obj/conf/cf-parse.tab.c"
    break;

  case 408:
#line 199 "nest/config.Y"
        { (yyval.i) = NET_FLOW4; }
#line 8507 "obj/conf/cf-parse.tab.c"
    break;

  case 409:
#line 200 "nest/config.Y"
        { (yyval.i) = NET_FLOW6; }
#line 8513 "obj/conf/cf-parse.tab.c"
    break;

  case 410:
#line 201 "nest/config.Y"
        { (yyval.i) = NET_MPLS; }
#line 8519 "obj/conf/cf-parse.tab.c"
    break;

  case 413:
#line 213 "nest/config.Y"
                                   {
   this_table = rt_new_table((yyvsp[0].s), (yyvsp[-2].i));
   }
#line 8527 "obj/conf/cf-parse.tab.c"
    break;

  case 415:
#line 220 "nest/config.Y"
          { this_table->sorted = 1; }
#line 8533 "obj/conf/cf-parse.tab.c"
    break;

  case 416:
#line 224 "nest/config.Y"
               { this_table->sorted = (yyvsp[0].i); }
#line 8539 "obj/conf/cf-parse.tab.c"
    break;

  case 417:
#line 225 "nest/config.Y"
             {
     if (!net_val_match(this_table->addr_type, NB_IP | NB_VPN | NB_ROA | NB_IP6_SADR))
       cf_error("Trie option not supported for %s table", net_label[this_table->addr_type]);
     this_table->trie_used = (yyvsp[0].i);
   }
#line 8549 "obj/conf/cf-parse.tab.c"
    break;

  case 418:
#line 230 "nest/config.Y"
                           { this_table->min_settle_time = (yyvsp[0].time); }
#line 8555 "obj/conf/cf-parse.tab.c"
    break;

  case 419:
#line 231 "nest/config.Y"
                           { this_table->max_settle_time = (yyvsp[0].time); }
#line 8561 "obj/conf/cf-parse.tab.c"
    break;

  case 420:
#line 232 "nest/config.Y"
                     { this_table->gc_threshold = (yyvsp[0].i); }
#line 8567 "obj/conf/cf-parse.tab.c"
    break;

  case 421:
#line 233 "nest/config.Y"
                     { this_table->gc_period = (uint) (yyvsp[0].time); if ((yyvsp[0].time) > 3600 S_) cf_error("GC period must be at most 3600 s"); }
#line 8573 "obj/conf/cf-parse.tab.c"
    break;

  case 426:
#line 249 "nest/config.Y"
            { proto_postconfig(); }
#line 8579 "obj/conf/cf-parse.tab.c"
    break;

  case 427:
#line 252 "nest/config.Y"
            { (yyval.i) = SYM_PROTO; }
#line 8585 "obj/conf/cf-parse.tab.c"
    break;

  case 428:
#line 253 "nest/config.Y"
            { (yyval.i) = SYM_TEMPLATE; }
#line 8591 "obj/conf/cf-parse.tab.c"
    break;

  case 429:
#line 257 "nest/config.Y"
               {
     struct symbol *s = cf_default_name(this_proto->protocol->template, &this_proto->protocol->name_counter);
     s->class = this_proto->class;
     s->proto = this_proto;
     this_proto->name = s->name;
     }
#line 8602 "obj/conf/cf-parse.tab.c"
    break;

  case 430:
#line 263 "nest/config.Y"
          {
     cf_define_symbol((yyvsp[0].s), this_proto->class, proto, this_proto);
     this_proto->name = (yyvsp[0].s)->name;
   }
#line 8611 "obj/conf/cf-parse.tab.c"
    break;

  case 431:
#line 267 "nest/config.Y"
                     {
     if (((yyvsp[0].s)->class != SYM_TEMPLATE) && ((yyvsp[0].s)->class != SYM_PROTO)) cf_error("Template or protocol name expected");

     struct symbol *s = cf_default_name(this_proto->protocol->template, &this_proto->protocol->name_counter);
     s->class = this_proto->class;
     s->proto = this_proto;
     this_proto->name = s->name;

     proto_copy_config(this_proto, (yyvsp[0].s)->proto);
   }
#line 8626 "obj/conf/cf-parse.tab.c"
    break;

  case 432:
#line 277 "nest/config.Y"
                            {
     if (((yyvsp[0].s)->class != SYM_TEMPLATE) && ((yyvsp[0].s)->class != SYM_PROTO)) cf_error("Template or protocol name expected");

     cf_define_symbol((yyvsp[-2].s), this_proto->class, proto, this_proto);
     this_proto->name = (yyvsp[-2].s)->name;

     proto_copy_config(this_proto, (yyvsp[0].s)->proto);
   }
#line 8639 "obj/conf/cf-parse.tab.c"
    break;

  case 434:
#line 289 "nest/config.Y"
                 { this_proto->disabled = (yyvsp[0].i); }
#line 8645 "obj/conf/cf-parse.tab.c"
    break;

  case 435:
#line 290 "nest/config.Y"
                    { this_proto->debug = (yyvsp[0].i); }
#line 8651 "obj/conf/cf-parse.tab.c"
    break;

  case 436:
#line 291 "nest/config.Y"
                        { this_proto->mrtdump = (yyvsp[0].i); }
#line 8657 "obj/conf/cf-parse.tab.c"
    break;

  case 437:
#line 292 "nest/config.Y"
                   { this_proto->router_id = (yyvsp[0].i32); }
#line 8663 "obj/conf/cf-parse.tab.c"
    break;

  case 438:
#line 293 "nest/config.Y"
                    { this_proto->dsc = (yyvsp[0].t); }
#line 8669 "obj/conf/cf-parse.tab.c"
    break;

  case 439:
#line 294 "nest/config.Y"
            { this_proto->vrf = if_get_by_name((yyvsp[0].t)); this_proto->vrf_set = 1; }
#line 8675 "obj/conf/cf-parse.tab.c"
    break;

  case 440:
#line 295 "nest/config.Y"
               { this_proto->vrf = NULL; this_proto->vrf_set = 1; }
#line 8681 "obj/conf/cf-parse.tab.c"
    break;

  case 441:
#line 300 "nest/config.Y"
{
  (yyval.cc) = this_channel = channel_config_get(NULL, net_label[(yyvsp[0].i)], (yyvsp[0].i), this_proto);
}
#line 8689 "obj/conf/cf-parse.tab.c"
    break;

  case 442:
#line 305 "nest/config.Y"
                {
     if (this_channel->net_type && ((yyvsp[0].r)->addr_type != this_channel->net_type))
       cf_error("Incompatible table type");
     this_channel->table = (yyvsp[0].r);
   }
#line 8699 "obj/conf/cf-parse.tab.c"
    break;

  case 443:
#line 310 "nest/config.Y"
                   { this_channel->in_filter = (yyvsp[0].f); }
#line 8705 "obj/conf/cf-parse.tab.c"
    break;

  case 444:
#line 311 "nest/config.Y"
                   { this_channel->out_filter = (yyvsp[0].f); }
#line 8711 "obj/conf/cf-parse.tab.c"
    break;

  case 445:
#line 312 "nest/config.Y"
                            { this_channel->rx_limit = (yyvsp[0].cl); }
#line 8717 "obj/conf/cf-parse.tab.c"
    break;

  case 446:
#line 313 "nest/config.Y"
                           { this_channel->in_limit = (yyvsp[0].cl); }
#line 8723 "obj/conf/cf-parse.tab.c"
    break;

  case 447:
#line 314 "nest/config.Y"
                           { this_channel->out_limit = (yyvsp[0].cl); }
#line 8729 "obj/conf/cf-parse.tab.c"
    break;

  case 448:
#line 315 "nest/config.Y"
                   { this_channel->preference = (yyvsp[0].i); check_u16((yyvsp[0].i)); }
#line 8735 "obj/conf/cf-parse.tab.c"
    break;

  case 449:
#line 316 "nest/config.Y"
                             { this_channel->in_keep_filtered = (yyvsp[0].i); }
#line 8741 "obj/conf/cf-parse.tab.c"
    break;

  case 450:
#line 317 "nest/config.Y"
                    { this_channel->rpki_reload = (yyvsp[0].i); }
#line 8747 "obj/conf/cf-parse.tab.c"
    break;

  case 452:
#line 323 "nest/config.Y"
                    { this_channel->debug = (yyvsp[0].i); }
#line 8753 "obj/conf/cf-parse.tab.c"
    break;

  case 457:
#line 337 "nest/config.Y"
{
  if (!this_channel->table)
    cf_error("Routing table not specified");

  this_channel = NULL;
}
#line 8764 "obj/conf/cf-parse.tab.c"
    break;

  case 459:
#line 347 "nest/config.Y"
                     { cf_assert_symbol((yyvsp[0].s), SYM_TABLE); (yyval.r) = (yyvsp[0].s)->table; }
#line 8770 "obj/conf/cf-parse.tab.c"
    break;

  case 460:
#line 350 "nest/config.Y"
                 { (yyval.f) = (yyvsp[0].f); }
#line 8776 "obj/conf/cf-parse.tab.c"
    break;

  case 462:
#line 352 "nest/config.Y"
       { (yyval.f) = FILTER_ACCEPT; }
#line 8782 "obj/conf/cf-parse.tab.c"
    break;

  case 463:
#line 353 "nest/config.Y"
        { (yyval.f) = FILTER_REJECT; }
#line 8788 "obj/conf/cf-parse.tab.c"
    break;

  case 464:
#line 357 "nest/config.Y"
                 { (yyval.i) = PLA_DISABLE; }
#line 8794 "obj/conf/cf-parse.tab.c"
    break;

  case 465:
#line 358 "nest/config.Y"
               { (yyval.i) = PLA_WARN; }
#line 8800 "obj/conf/cf-parse.tab.c"
    break;

  case 466:
#line 359 "nest/config.Y"
                { (yyval.i) = PLA_BLOCK; }
#line 8806 "obj/conf/cf-parse.tab.c"
    break;

  case 467:
#line 360 "nest/config.Y"
                  { (yyval.i) = PLA_RESTART; }
#line 8812 "obj/conf/cf-parse.tab.c"
    break;

  case 468:
#line 361 "nest/config.Y"
                  { (yyval.i) = PLA_DISABLE; }
#line 8818 "obj/conf/cf-parse.tab.c"
    break;

  case 469:
#line 365 "nest/config.Y"
                     { (yyval.cl) = (struct channel_limit){ .limit = (yyvsp[-1].i), (yyval.cl).action = (yyvsp[0].i) }; }
#line 8824 "obj/conf/cf-parse.tab.c"
    break;

  case 470:
#line 366 "nest/config.Y"
       { (yyval.cl) = (struct channel_limit){}; }
#line 8830 "obj/conf/cf-parse.tab.c"
    break;

  case 472:
#line 373 "nest/config.Y"
                              { new_config->proto_default_debug = (yyvsp[0].i); }
#line 8836 "obj/conf/cf-parse.tab.c"
    break;

  case 473:
#line 374 "nest/config.Y"
                             { new_config->channel_default_debug = (yyvsp[0].i); }
#line 8842 "obj/conf/cf-parse.tab.c"
    break;

  case 474:
#line 375 "nest/config.Y"
                       { new_config->cli_debug = (yyvsp[0].i); }
#line 8848 "obj/conf/cf-parse.tab.c"
    break;

  case 476:
#line 383 "nest/config.Y"
         { (yyval.tf) = &new_config->tf_route; }
#line 8854 "obj/conf/cf-parse.tab.c"
    break;

  case 477:
#line 384 "nest/config.Y"
            { (yyval.tf) = &new_config->tf_proto; }
#line 8860 "obj/conf/cf-parse.tab.c"
    break;

  case 478:
#line 385 "nest/config.Y"
        { (yyval.tf) = &new_config->tf_base; }
#line 8866 "obj/conf/cf-parse.tab.c"
    break;

  case 479:
#line 386 "nest/config.Y"
       { (yyval.tf) = &new_config->tf_log; }
#line 8872 "obj/conf/cf-parse.tab.c"
    break;

  case 480:
#line 390 "nest/config.Y"
                         { *(yyvsp[-1].tf) = (struct timeformat){(yyvsp[0].t), NULL, 0}; }
#line 8878 "obj/conf/cf-parse.tab.c"
    break;

  case 481:
#line 391 "nest/config.Y"
                                   { *(yyvsp[-3].tf) = (struct timeformat){(yyvsp[-2].t), (yyvsp[0].t), (s64) (yyvsp[-1].i) S_}; }
#line 8884 "obj/conf/cf-parse.tab.c"
    break;

  case 482:
#line 392 "nest/config.Y"
                                 { *(yyvsp[-2].tf) = TM_ISO_SHORT_S; }
#line 8890 "obj/conf/cf-parse.tab.c"
    break;

  case 483:
#line 393 "nest/config.Y"
                                 { *(yyvsp[-3].tf) = TM_ISO_SHORT_MS; }
#line 8896 "obj/conf/cf-parse.tab.c"
    break;

  case 484:
#line 394 "nest/config.Y"
                                 { *(yyvsp[-3].tf) = TM_ISO_SHORT_US; }
#line 8902 "obj/conf/cf-parse.tab.c"
    break;

  case 485:
#line 395 "nest/config.Y"
                                { *(yyvsp[-2].tf) = TM_ISO_LONG_S; }
#line 8908 "obj/conf/cf-parse.tab.c"
    break;

  case 486:
#line 396 "nest/config.Y"
                                { *(yyvsp[-3].tf) = TM_ISO_LONG_MS; }
#line 8914 "obj/conf/cf-parse.tab.c"
    break;

  case 487:
#line 397 "nest/config.Y"
                                { *(yyvsp[-3].tf) = TM_ISO_LONG_US; }
#line 8920 "obj/conf/cf-parse.tab.c"
    break;

  case 489:
#line 408 "nest/config.Y"
               {
     struct iface_patt_node *ipn = cfg_allocz(sizeof(struct iface_patt_node));
     add_tail(&this_ipatt->ipn_list, NODE ipn);
     this_ipn = ipn;
   }
#line 8930 "obj/conf/cf-parse.tab.c"
    break;

  case 490:
#line 416 "nest/config.Y"
        { this_ipn->pattern = (yyvsp[0].t); /* this_ipn->prefix stays zero */ }
#line 8936 "obj/conf/cf-parse.tab.c"
    break;

  case 491:
#line 417 "nest/config.Y"
                      { this_ipn->pattern = (yyvsp[-1].t); this_ipn->prefix = (yyvsp[0].net); }
#line 8942 "obj/conf/cf-parse.tab.c"
    break;

  case 492:
#line 421 "nest/config.Y"
       { this_ipn->positive = 1; }
#line 8948 "obj/conf/cf-parse.tab.c"
    break;

  case 493:
#line 422 "nest/config.Y"
       { this_ipn->positive = 0; }
#line 8954 "obj/conf/cf-parse.tab.c"
    break;

  case 497:
#line 436 "nest/config.Y"
                                      { iface_patt_check(); }
#line 8960 "obj/conf/cf-parse.tab.c"
    break;

  case 498:
#line 438 "nest/config.Y"
                 {
   /* Generic this_ipatt init */
   this_ipatt = cfg_allocz(sizeof(struct iface_patt));
   init_list(&this_ipatt->ipn_list);
 }
#line 8970 "obj/conf/cf-parse.tab.c"
    break;

  case 500:
#line 450 "nest/config.Y"
              { (yyval.i) = (yyvsp[0].i) & 0xfc;        if ((yyvsp[0].i) > 255) cf_error("TX class must be in range 0-255"); }
#line 8976 "obj/conf/cf-parse.tab.c"
    break;

  case 501:
#line 451 "nest/config.Y"
              { (yyval.i) = ((yyvsp[0].i) & 0x3f) << 2; if ((yyvsp[0].i) > 63)  cf_error("TX DSCP must be in range 0-63"); }
#line 8982 "obj/conf/cf-parse.tab.c"
    break;

  case 503:
#line 458 "nest/config.Y"
                                    {
     this_proto = proto_config_new(&proto_device, (yyvsp[-1].i));
     init_list(&DIRECT_CFG->iface_list);
   }
#line 8991 "obj/conf/cf-parse.tab.c"
    break;

  case 508:
#line 469 "nest/config.Y"
                                 { DIRECT_CFG->check_link = (yyvsp[-1].i); }
#line 8997 "obj/conf/cf-parse.tab.c"
    break;

  case 509:
#line 473 "nest/config.Y"
               {
     this_ipatt = cfg_allocz(sizeof(struct iface_patt));
     add_tail(&DIRECT_CFG->iface_list, NODE this_ipatt);
     init_list(&this_ipatt->ipn_list);
   }
#line 9007 "obj/conf/cf-parse.tab.c"
    break;

  case 511:
#line 487 "nest/config.Y"
       { (yyval.i) = ~0; }
#line 9013 "obj/conf/cf-parse.tab.c"
    break;

  case 512:
#line 488 "nest/config.Y"
       { (yyval.i) = 0; }
#line 9019 "obj/conf/cf-parse.tab.c"
    break;

  case 513:
#line 489 "nest/config.Y"
                      { (yyval.i) = (yyvsp[-1].i); }
#line 9025 "obj/conf/cf-parse.tab.c"
    break;

  case 515:
#line 494 "nest/config.Y"
                             { (yyval.i) = (yyvsp[-2].i) | (yyvsp[0].i); }
#line 9031 "obj/conf/cf-parse.tab.c"
    break;

  case 516:
#line 498 "nest/config.Y"
                { (yyval.i) = D_STATES; }
#line 9037 "obj/conf/cf-parse.tab.c"
    break;

  case 517:
#line 499 "nest/config.Y"
                { (yyval.i) = D_ROUTES; }
#line 9043 "obj/conf/cf-parse.tab.c"
    break;

  case 518:
#line 500 "nest/config.Y"
                { (yyval.i) = D_FILTERS; }
#line 9049 "obj/conf/cf-parse.tab.c"
    break;

  case 519:
#line 501 "nest/config.Y"
                { (yyval.i) = D_IFACES; }
#line 9055 "obj/conf/cf-parse.tab.c"
    break;

  case 520:
#line 502 "nest/config.Y"
                { (yyval.i) = D_EVENTS; }
#line 9061 "obj/conf/cf-parse.tab.c"
    break;

  case 521:
#line 503 "nest/config.Y"
                { (yyval.i) = D_PACKETS; }
#line 9067 "obj/conf/cf-parse.tab.c"
    break;

  case 522:
#line 509 "nest/config.Y"
       { (yyval.i) = ~0; }
#line 9073 "obj/conf/cf-parse.tab.c"
    break;

  case 523:
#line 510 "nest/config.Y"
       { (yyval.i) = 0; }
#line 9079 "obj/conf/cf-parse.tab.c"
    break;

  case 524:
#line 511 "nest/config.Y"
                        { (yyval.i) = (yyvsp[-1].i); }
#line 9085 "obj/conf/cf-parse.tab.c"
    break;

  case 526:
#line 516 "nest/config.Y"
                                 { (yyval.i) = (yyvsp[-2].i) | (yyvsp[0].i); }
#line 9091 "obj/conf/cf-parse.tab.c"
    break;

  case 527:
#line 520 "nest/config.Y"
                { (yyval.i) = MD_STATES; }
#line 9097 "obj/conf/cf-parse.tab.c"
    break;

  case 528:
#line 521 "nest/config.Y"
                { (yyval.i) = MD_MESSAGES; }
#line 9103 "obj/conf/cf-parse.tab.c"
    break;

  case 538:
#line 548 "nest/config.Y"
                  { init_password_list(); init_password((yyvsp[0].t), strlen((yyvsp[0].t)), password_id++); }
#line 9109 "obj/conf/cf-parse.tab.c"
    break;

  case 539:
#line 549 "nest/config.Y"
                        { init_password_list(); init_password((yyvsp[0].bs)->data, (yyvsp[0].bs)->length, password_id++); }
#line 9115 "obj/conf/cf-parse.tab.c"
    break;

  case 540:
#line 553 "nest/config.Y"
               { }
#line 9121 "obj/conf/cf-parse.tab.c"
    break;

  case 541:
#line 554 "nest/config.Y"
                                               { this_p_item->genfrom = (yyvsp[-2].time); }
#line 9127 "obj/conf/cf-parse.tab.c"
    break;

  case 542:
#line 555 "nest/config.Y"
                                             { this_p_item->gento = (yyvsp[-2].time); }
#line 9133 "obj/conf/cf-parse.tab.c"
    break;

  case 543:
#line 556 "nest/config.Y"
                                             { this_p_item->accfrom = (yyvsp[-2].time); }
#line 9139 "obj/conf/cf-parse.tab.c"
    break;

  case 544:
#line 557 "nest/config.Y"
                                           { this_p_item->accto = (yyvsp[-2].time); }
#line 9145 "obj/conf/cf-parse.tab.c"
    break;

  case 545:
#line 558 "nest/config.Y"
                                      { this_p_item->genfrom = this_p_item->accfrom = (yyvsp[-2].time); }
#line 9151 "obj/conf/cf-parse.tab.c"
    break;

  case 546:
#line 559 "nest/config.Y"
                                    { this_p_item->gento = this_p_item->accto = (yyvsp[-2].time); }
#line 9157 "obj/conf/cf-parse.tab.c"
    break;

  case 547:
#line 560 "nest/config.Y"
                                    { this_p_item->id = (yyvsp[-2].i); if ((yyvsp[-2].i) > 255) cf_error("Password ID must be in range 0-255"); }
#line 9163 "obj/conf/cf-parse.tab.c"
    break;

  case 548:
#line 561 "nest/config.Y"
                                                         { this_p_item->alg = (yyvsp[-2].i); }
#line 9169 "obj/conf/cf-parse.tab.c"
    break;

  case 549:
#line 565 "nest/config.Y"
                { (yyval.i) = ALG_MD5; }
#line 9175 "obj/conf/cf-parse.tab.c"
    break;

  case 550:
#line 566 "nest/config.Y"
                { (yyval.i) = ALG_SHA1; }
#line 9181 "obj/conf/cf-parse.tab.c"
    break;

  case 551:
#line 567 "nest/config.Y"
                { (yyval.i) = ALG_SHA256; }
#line 9187 "obj/conf/cf-parse.tab.c"
    break;

  case 552:
#line 568 "nest/config.Y"
                { (yyval.i) = ALG_SHA384; }
#line 9193 "obj/conf/cf-parse.tab.c"
    break;

  case 553:
#line 569 "nest/config.Y"
                { (yyval.i) = ALG_SHA512; }
#line 9199 "obj/conf/cf-parse.tab.c"
    break;

  case 554:
#line 570 "nest/config.Y"
                { (yyval.i) = ALG_HMAC_MD5; }
#line 9205 "obj/conf/cf-parse.tab.c"
    break;

  case 555:
#line 571 "nest/config.Y"
                { (yyval.i) = ALG_HMAC_SHA1; }
#line 9211 "obj/conf/cf-parse.tab.c"
    break;

  case 556:
#line 572 "nest/config.Y"
                { (yyval.i) = ALG_HMAC_SHA256; }
#line 9217 "obj/conf/cf-parse.tab.c"
    break;

  case 557:
#line 573 "nest/config.Y"
                { (yyval.i) = ALG_HMAC_SHA384; }
#line 9223 "obj/conf/cf-parse.tab.c"
    break;

  case 558:
#line 574 "nest/config.Y"
                { (yyval.i) = ALG_HMAC_SHA512; }
#line 9229 "obj/conf/cf-parse.tab.c"
    break;

  case 559:
#line 575 "nest/config.Y"
                { (yyval.i) = ALG_BLAKE2S_128; }
#line 9235 "obj/conf/cf-parse.tab.c"
    break;

  case 560:
#line 576 "nest/config.Y"
                { (yyval.i) = ALG_BLAKE2S_256; }
#line 9241 "obj/conf/cf-parse.tab.c"
    break;

  case 561:
#line 577 "nest/config.Y"
                { (yyval.i) = ALG_BLAKE2B_256; }
#line 9247 "obj/conf/cf-parse.tab.c"
    break;

  case 562:
#line 578 "nest/config.Y"
                { (yyval.i) = ALG_BLAKE2B_512; }
#line 9253 "obj/conf/cf-parse.tab.c"
    break;

  case 563:
#line 582 "nest/config.Y"
{
  password_validate_length(this_p_item);
}
#line 9261 "obj/conf/cf-parse.tab.c"
    break;

  case 564:
#line 590 "nest/config.Y"
                    { this_bfd_opts->min_rx_int = this_bfd_opts->min_tx_int = (yyvsp[0].time); }
#line 9267 "obj/conf/cf-parse.tab.c"
    break;

  case 565:
#line 591 "nest/config.Y"
                           { this_bfd_opts->min_rx_int = (yyvsp[0].time); }
#line 9273 "obj/conf/cf-parse.tab.c"
    break;

  case 566:
#line 592 "nest/config.Y"
                           { this_bfd_opts->min_tx_int = (yyvsp[0].time); }
#line 9279 "obj/conf/cf-parse.tab.c"
    break;

  case 567:
#line 593 "nest/config.Y"
                            { this_bfd_opts->idle_tx_int = (yyvsp[0].time); }
#line 9285 "obj/conf/cf-parse.tab.c"
    break;

  case 568:
#line 594 "nest/config.Y"
                   { this_bfd_opts->multiplier = (yyvsp[0].i); }
#line 9291 "obj/conf/cf-parse.tab.c"
    break;

  case 569:
#line 595 "nest/config.Y"
                { this_bfd_opts->passive = (yyvsp[0].i); this_bfd_opts->passive_set = 1; }
#line 9297 "obj/conf/cf-parse.tab.c"
    break;

  case 570:
#line 596 "nest/config.Y"
            { this_bfd_opts->mode = BGP_BFD_GRACEFUL; }
#line 9303 "obj/conf/cf-parse.tab.c"
    break;

  case 575:
#line 612 "nest/config.Y"
{ cmd_show_status(); }
#line 9309 "obj/conf/cf-parse.tab.c"
    break;

  case 577:
#line 615 "nest/config.Y"
{ cmd_show_memory(); }
#line 9315 "obj/conf/cf-parse.tab.c"
    break;

  case 579:
#line 618 "nest/config.Y"
{ proto_apply_cmd((yyvsp[-1].ps), proto_cmd_show, 0, 0); }
#line 9321 "obj/conf/cf-parse.tab.c"
    break;

  case 581:
#line 621 "nest/config.Y"
{ proto_apply_cmd((yyvsp[-1].ps), proto_cmd_show, 0, 1); }
#line 9327 "obj/conf/cf-parse.tab.c"
    break;

  case 582:
#line 624 "nest/config.Y"
                { cf_assert_symbol((yyvsp[0].s), SYM_PROTO); (yyval.s) = (yyvsp[0].s); }
#line 9333 "obj/conf/cf-parse.tab.c"
    break;

  case 583:
#line 625 "nest/config.Y"
               { (yyval.s) = NULL; }
#line 9339 "obj/conf/cf-parse.tab.c"
    break;

  case 585:
#line 629 "nest/config.Y"
{ if_show(); }
#line 9345 "obj/conf/cf-parse.tab.c"
    break;

  case 587:
#line 632 "nest/config.Y"
{ if_show_summary(); }
#line 9351 "obj/conf/cf-parse.tab.c"
    break;

  case 589:
#line 636 "nest/config.Y"
{ rt_show((yyvsp[-1].ra)); }
#line 9357 "obj/conf/cf-parse.tab.c"
    break;

  case 590:
#line 639 "nest/config.Y"
               {
     (yyval.ra) = cfg_allocz(sizeof(struct rt_show_data));
     init_list(&((yyval.ra)->tables));
     (yyval.ra)->filter = FILTER_ACCEPT;
     (yyval.ra)->running_on_config = new_config->fallback;
   }
#line 9368 "obj/conf/cf-parse.tab.c"
    break;

  case 591:
#line 645 "nest/config.Y"
                  {
     (yyval.ra) = (yyvsp[-1].ra);
     if ((yyval.ra)->addr) cf_error("Only one prefix expected");
     (yyval.ra)->addr = (yyvsp[0].net_ptr);
     (yyval.ra)->addr_mode = RSD_ADDR_EQUAL;
   }
#line 9379 "obj/conf/cf-parse.tab.c"
    break;

  case 592:
#line 651 "nest/config.Y"
                         {
     (yyval.ra) = (yyvsp[-2].ra);
     if ((yyval.ra)->addr) cf_error("Only one prefix expected");
     (yyval.ra)->addr = (yyvsp[0].net_ptr);
     (yyval.ra)->addr_mode = RSD_ADDR_FOR;
   }
#line 9390 "obj/conf/cf-parse.tab.c"
    break;

  case 593:
#line 657 "nest/config.Y"
                     {
     (yyval.ra) = (yyvsp[-2].ra);
     if ((yyval.ra)->addr) cf_error("Only one prefix expected");
     if (!net_type_match((yyvsp[0].net_ptr), NB_IP)) cf_error("Only IP networks accepted for 'in' argument");
     (yyval.ra)->addr = (yyvsp[0].net_ptr);
     (yyval.ra)->addr_mode = RSD_ADDR_IN;
   }
#line 9402 "obj/conf/cf-parse.tab.c"
    break;

  case 594:
#line 664 "nest/config.Y"
                            {
     cf_assert_symbol((yyvsp[0].s), SYM_TABLE);
     (yyval.ra) = (yyvsp[-2].ra);
     rt_show_add_table((yyval.ra), (yyvsp[0].s)->table->table);
     (yyval.ra)->tables_defined_by = RSD_TDB_DIRECT;
   }
#line 9413 "obj/conf/cf-parse.tab.c"
    break;

  case 595:
#line 670 "nest/config.Y"
                    {
     struct rtable_config *t;
     (yyval.ra) = (yyvsp[-2].ra);
     WALK_LIST(t, config->tables)
       rt_show_add_table((yyval.ra), t->table);
     (yyval.ra)->tables_defined_by = RSD_TDB_ALL;
   }
#line 9425 "obj/conf/cf-parse.tab.c"
    break;

  case 596:
#line 677 "nest/config.Y"
                                   {
     if (!(yyvsp[0].c)->in_table) cf_error("No import table in channel %s.%s", (yyvsp[0].c)->proto->name, (yyvsp[0].c)->name);
     rt_show_add_table((yyval.ra), (yyvsp[0].c)->in_table);
     (yyval.ra)->tables_defined_by = RSD_TDB_DIRECT;
   }
#line 9435 "obj/conf/cf-parse.tab.c"
    break;

  case 597:
#line 682 "nest/config.Y"
                                   {
     if (!(yyvsp[0].c)->out_table) cf_error("No export table in channel %s.%s", (yyvsp[0].c)->proto->name, (yyvsp[0].c)->name);
     rt_show_add_table((yyval.ra), (yyvsp[0].c)->out_table);
     (yyval.ra)->tables_defined_by = RSD_TDB_DIRECT;
   }
#line 9445 "obj/conf/cf-parse.tab.c"
    break;

  case 598:
#line 687 "nest/config.Y"
                        {
     (yyval.ra) = (yyvsp[-2].ra);
     if ((yyval.ra)->filter != FILTER_ACCEPT) cf_error("Filter specified twice");
     (yyval.ra)->filter = (yyvsp[0].f);
   }
#line 9455 "obj/conf/cf-parse.tab.c"
    break;

  case 599:
#line 692 "nest/config.Y"
                       {
     (yyval.ra) = (yyvsp[-1].ra);
     if ((yyval.ra)->filter != FILTER_ACCEPT) cf_error("Filter specified twice");
     (yyval.ra)->filter = (yyvsp[0].f);
   }
#line 9465 "obj/conf/cf-parse.tab.c"
    break;

  case 600:
#line 697 "nest/config.Y"
              {
     (yyval.ra) = (yyvsp[-1].ra);
     (yyval.ra)->verbose = 1;
   }
#line 9474 "obj/conf/cf-parse.tab.c"
    break;

  case 601:
#line 701 "nest/config.Y"
                  {
     (yyval.ra) = (yyvsp[-1].ra);
     (yyval.ra)->primary_only = 1;
   }
#line 9483 "obj/conf/cf-parse.tab.c"
    break;

  case 602:
#line 705 "nest/config.Y"
                   {
     (yyval.ra) = (yyvsp[-1].ra);
     (yyval.ra)->filtered = 1;
   }
#line 9492 "obj/conf/cf-parse.tab.c"
    break;

  case 603:
#line 709 "nest/config.Y"
                                   {
     cf_assert_symbol((yyvsp[0].s), SYM_PROTO);
     struct proto_config *c = (struct proto_config *) (yyvsp[0].s)->proto;
     (yyval.ra) = (yyvsp[-2].ra);
     if ((yyval.ra)->export_mode) cf_error("Export specified twice");
     if (!c->proto) cf_error("%s is not a protocol", (yyvsp[0].s)->name);
     (yyval.ra)->export_mode = (yyvsp[-1].i);
     (yyval.ra)->export_protocol = c->proto;
     (yyval.ra)->tables_defined_by = RSD_TDB_INDIRECT;
   }
#line 9507 "obj/conf/cf-parse.tab.c"
    break;

  case 604:
#line 719 "nest/config.Y"
                                  {
     (yyval.ra) = (yyvsp[-2].ra);
     if ((yyval.ra)->export_mode) cf_error("Export specified twice");
     (yyval.ra)->export_mode = (yyvsp[-1].i);
     (yyval.ra)->export_channel = (yyvsp[0].c);
     (yyval.ra)->tables_defined_by = RSD_TDB_INDIRECT;
   }
#line 9519 "obj/conf/cf-parse.tab.c"
    break;

  case 605:
#line 726 "nest/config.Y"
                                {
     cf_assert_symbol((yyvsp[0].s), SYM_PROTO);
     struct proto_config *c = (struct proto_config *) (yyvsp[0].s)->proto;
     (yyval.ra) = (yyvsp[-2].ra);
     if ((yyval.ra)->show_protocol) cf_error("Protocol specified twice");
     if (!c->proto) cf_error("%s is not a protocol", (yyvsp[0].s)->name);
     (yyval.ra)->show_protocol = c->proto;
     (yyval.ra)->tables_defined_by = RSD_TDB_INDIRECT;
   }
#line 9533 "obj/conf/cf-parse.tab.c"
    break;

  case 606:
#line 735 "nest/config.Y"
                {
     (yyval.ra) = (yyvsp[-1].ra);
     (yyval.ra)->stats = 1;
   }
#line 9542 "obj/conf/cf-parse.tab.c"
    break;

  case 607:
#line 739 "nest/config.Y"
                {
     (yyval.ra) = (yyvsp[-1].ra);
     (yyval.ra)->stats = 2;
   }
#line 9551 "obj/conf/cf-parse.tab.c"
    break;

  case 608:
#line 746 "nest/config.Y"
                 {
    (yyval.net_ptr) = cfg_alloc((yyvsp[0].net).length);
    net_copy((yyval.net_ptr), &(yyvsp[0].net));
  }
#line 9560 "obj/conf/cf-parse.tab.c"
    break;

  case 612:
#line 753 "nest/config.Y"
              {
    (yyval.net_ptr) = cfg_alloc(sizeof(net_addr_vpn4));
    net_fill_vpn4((yyval.net_ptr), (yyvsp[0].ip4), IP4_MAX_PREFIX_LENGTH, (yyvsp[-1].i64));
  }
#line 9569 "obj/conf/cf-parse.tab.c"
    break;

  case 613:
#line 757 "nest/config.Y"
              {
    (yyval.net_ptr) = cfg_alloc(sizeof(net_addr_vpn6));
    net_fill_vpn6((yyval.net_ptr), (yyvsp[0].ip6), IP6_MAX_PREFIX_LENGTH, (yyvsp[-1].i64));
  }
#line 9578 "obj/conf/cf-parse.tab.c"
    break;

  case 614:
#line 761 "nest/config.Y"
                {
    (yyval.net_ptr) = cfg_alloc(sizeof(net_addr_ip6_sadr));
    net_fill_ip6_sadr((yyval.net_ptr), (yyvsp[-2].ip6), IP6_MAX_PREFIX_LENGTH, (yyvsp[0].ip6), IP6_MAX_PREFIX_LENGTH);
  }
#line 9587 "obj/conf/cf-parse.tab.c"
    break;

  case 615:
#line 765 "nest/config.Y"
                {
     if ((yyvsp[0].s)->class == (SYM_CONSTANT | T_IP))
     {
       (yyval.net_ptr) = cfg_alloc(ipa_is_ip4(SYM_VAL((yyvsp[0].s)).ip) ? sizeof(net_addr_ip4) : sizeof(net_addr_ip6));
       net_fill_ip_host((yyval.net_ptr), SYM_VAL((yyvsp[0].s)).ip);
     }
     else if (((yyvsp[0].s)->class == (SYM_CONSTANT | T_NET)) && net_type_match(SYM_VAL((yyvsp[0].s)).net, NB_IP | NB_VPN))
       (yyval.net_ptr) = (net_addr *) SYM_VAL((yyvsp[0].s)).net; /* Avoid const warning */
     else
       cf_error("IP address or network constant expected");
   }
#line 9603 "obj/conf/cf-parse.tab.c"
    break;

  case 618:
#line 781 "nest/config.Y"
       { net_fill_ip4(&((yyval.net)), (yyvsp[0].ip4), IP4_MAX_PREFIX_LENGTH); }
#line 9609 "obj/conf/cf-parse.tab.c"
    break;

  case 619:
#line 782 "nest/config.Y"
       { net_fill_ip6(&((yyval.net)), (yyvsp[0].ip6), IP6_MAX_PREFIX_LENGTH); }
#line 9615 "obj/conf/cf-parse.tab.c"
    break;

  case 620:
#line 785 "nest/config.Y"
                { (yyval.i) = RSEM_PREEXPORT; }
#line 9621 "obj/conf/cf-parse.tab.c"
    break;

  case 621:
#line 786 "nest/config.Y"
                { (yyval.i) = RSEM_EXPORT; }
#line 9627 "obj/conf/cf-parse.tab.c"
    break;

  case 622:
#line 787 "nest/config.Y"
                { (yyval.i) = RSEM_NOEXPORT; }
#line 9633 "obj/conf/cf-parse.tab.c"
    break;

  case 623:
#line 788 "nest/config.Y"
                { (yyval.i) = RSEM_EXPORTED; }
#line 9639 "obj/conf/cf-parse.tab.c"
    break;

  case 624:
#line 793 "nest/config.Y"
                { (yyval.t) = "ipv4"; }
#line 9645 "obj/conf/cf-parse.tab.c"
    break;

  case 625:
#line 794 "nest/config.Y"
                { (yyval.t) = "ipv4-mc"; }
#line 9651 "obj/conf/cf-parse.tab.c"
    break;

  case 626:
#line 795 "nest/config.Y"
                { (yyval.t) = "ipv4-mpls"; }
#line 9657 "obj/conf/cf-parse.tab.c"
    break;

  case 627:
#line 796 "nest/config.Y"
                { (yyval.t) = "ipv6"; }
#line 9663 "obj/conf/cf-parse.tab.c"
    break;

  case 628:
#line 797 "nest/config.Y"
                { (yyval.t) = "ipv6-mc"; }
#line 9669 "obj/conf/cf-parse.tab.c"
    break;

  case 629:
#line 798 "nest/config.Y"
                { (yyval.t) = "ipv6-mpls"; }
#line 9675 "obj/conf/cf-parse.tab.c"
    break;

  case 630:
#line 799 "nest/config.Y"
                { (yyval.t) = "ipv6-sadr"; }
#line 9681 "obj/conf/cf-parse.tab.c"
    break;

  case 631:
#line 800 "nest/config.Y"
                { (yyval.t) = "vpn4"; }
#line 9687 "obj/conf/cf-parse.tab.c"
    break;

  case 632:
#line 801 "nest/config.Y"
                { (yyval.t) = "vpn4-mc"; }
#line 9693 "obj/conf/cf-parse.tab.c"
    break;

  case 633:
#line 802 "nest/config.Y"
                { (yyval.t) = "vpn4-mpls"; }
#line 9699 "obj/conf/cf-parse.tab.c"
    break;

  case 634:
#line 803 "nest/config.Y"
                { (yyval.t) = "vpn6"; }
#line 9705 "obj/conf/cf-parse.tab.c"
    break;

  case 635:
#line 804 "nest/config.Y"
                { (yyval.t) = "vpn6-mc"; }
#line 9711 "obj/conf/cf-parse.tab.c"
    break;

  case 636:
#line 805 "nest/config.Y"
                { (yyval.t) = "vpn6-mpls"; }
#line 9717 "obj/conf/cf-parse.tab.c"
    break;

  case 637:
#line 806 "nest/config.Y"
                { (yyval.t) = "roa4"; }
#line 9723 "obj/conf/cf-parse.tab.c"
    break;

  case 638:
#line 807 "nest/config.Y"
                { (yyval.t) = "roa6"; }
#line 9729 "obj/conf/cf-parse.tab.c"
    break;

  case 639:
#line 808 "nest/config.Y"
                { (yyval.t) = "flow4"; }
#line 9735 "obj/conf/cf-parse.tab.c"
    break;

  case 640:
#line 809 "nest/config.Y"
                { (yyval.t) = "flow6"; }
#line 9741 "obj/conf/cf-parse.tab.c"
    break;

  case 641:
#line 810 "nest/config.Y"
                { (yyval.t) = "mpls"; }
#line 9747 "obj/conf/cf-parse.tab.c"
    break;

  case 642:
#line 811 "nest/config.Y"
                { (yyval.t) = "pri"; }
#line 9753 "obj/conf/cf-parse.tab.c"
    break;

  case 643:
#line 812 "nest/config.Y"
                { (yyval.t) = "sec"; }
#line 9759 "obj/conf/cf-parse.tab.c"
    break;

  case 644:
#line 816 "nest/config.Y"
                                {
     cf_assert_symbol((yyvsp[-2].s), SYM_PROTO);
     struct proto *p =  (yyvsp[-2].s)->proto->proto;
     if (!p) cf_error("%s is not a protocol", (yyvsp[-2].s)->name);
     (yyval.c) = proto_find_channel_by_name(p, (yyvsp[0].t));
     if (!(yyval.c)) cf_error("Channel %s.%s not found", (yyvsp[-2].s)->name, (yyvsp[0].t));
   }
#line 9771 "obj/conf/cf-parse.tab.c"
    break;

  case 646:
#line 827 "nest/config.Y"
{ cmd_show_symbols((yyvsp[-1].sd)); }
#line 9777 "obj/conf/cf-parse.tab.c"
    break;

  case 647:
#line 830 "nest/config.Y"
               {
     (yyval.sd) = cfg_allocz(sizeof(struct sym_show_data));
   }
#line 9785 "obj/conf/cf-parse.tab.c"
    break;

  case 648:
#line 833 "nest/config.Y"
                  { (yyval.sd) = (yyvsp[-1].sd); (yyval.sd)->type = SYM_TABLE; }
#line 9791 "obj/conf/cf-parse.tab.c"
    break;

  case 649:
#line 834 "nest/config.Y"
                     { (yyval.sd) = (yyvsp[-1].sd); (yyval.sd)->type = SYM_FUNCTION; }
#line 9797 "obj/conf/cf-parse.tab.c"
    break;

  case 650:
#line 835 "nest/config.Y"
                   { (yyval.sd) = (yyvsp[-1].sd); (yyval.sd)->type = SYM_FILTER; }
#line 9803 "obj/conf/cf-parse.tab.c"
    break;

  case 651:
#line 836 "nest/config.Y"
                     { (yyval.sd) = (yyvsp[-1].sd); (yyval.sd)->type = SYM_PROTO; }
#line 9809 "obj/conf/cf-parse.tab.c"
    break;

  case 652:
#line 837 "nest/config.Y"
                     { (yyval.sd) = (yyvsp[-1].sd); (yyval.sd)->type = SYM_TEMPLATE; }
#line 9815 "obj/conf/cf-parse.tab.c"
    break;

  case 653:
#line 838 "nest/config.Y"
                   { (yyval.sd) = (yyvsp[-1].sd); (yyval.sd)->sym = (yyvsp[0].s); }
#line 9821 "obj/conf/cf-parse.tab.c"
    break;

  case 655:
#line 844 "nest/config.Y"
{ rdump(&root_pool); cli_msg(0, ""); }
#line 9827 "obj/conf/cf-parse.tab.c"
    break;

  case 657:
#line 846 "nest/config.Y"
{ sk_dump_all(); cli_msg(0, ""); }
#line 9833 "obj/conf/cf-parse.tab.c"
    break;

  case 659:
#line 848 "nest/config.Y"
{ io_log_dump(); cli_msg(0, ""); }
#line 9839 "obj/conf/cf-parse.tab.c"
    break;

  case 661:
#line 850 "nest/config.Y"
{ if_dump_all(); cli_msg(0, ""); }
#line 9845 "obj/conf/cf-parse.tab.c"
    break;

  case 663:
#line 852 "nest/config.Y"
{ neigh_dump_all(); cli_msg(0, ""); }
#line 9851 "obj/conf/cf-parse.tab.c"
    break;

  case 665:
#line 854 "nest/config.Y"
{ rta_dump_all(); cli_msg(0, ""); }
#line 9857 "obj/conf/cf-parse.tab.c"
    break;

  case 667:
#line 856 "nest/config.Y"
{ rt_dump_all(); cli_msg(0, ""); }
#line 9863 "obj/conf/cf-parse.tab.c"
    break;

  case 669:
#line 858 "nest/config.Y"
{ protos_dump_all(); cli_msg(0, ""); }
#line 9869 "obj/conf/cf-parse.tab.c"
    break;

  case 671:
#line 860 "nest/config.Y"
{ filters_dump_all(); cli_msg(0, ""); }
#line 9875 "obj/conf/cf-parse.tab.c"
    break;

  case 673:
#line 863 "nest/config.Y"
{ cmd_eval(f_linearize((yyvsp[-1].x), 1)); }
#line 9881 "obj/conf/cf-parse.tab.c"
    break;

  case 675:
#line 866 "nest/config.Y"
                                       {
  cli_set_log_echo(this_cli, (yyvsp[-2].i), (yyvsp[-1].i));
  cli_msg(0, "");
}
#line 9890 "obj/conf/cf-parse.tab.c"
    break;

  case 676:
#line 872 "nest/config.Y"
       { (yyval.i) = ~0; }
#line 9896 "obj/conf/cf-parse.tab.c"
    break;

  case 677:
#line 873 "nest/config.Y"
       { (yyval.i) = 0; }
#line 9902 "obj/conf/cf-parse.tab.c"
    break;

  case 678:
#line 874 "nest/config.Y"
                         { (yyval.i) = (yyvsp[-1].i); }
#line 9908 "obj/conf/cf-parse.tab.c"
    break;

  case 679:
#line 878 "nest/config.Y"
               { (yyval.i) = 4096; }
#line 9914 "obj/conf/cf-parse.tab.c"
    break;

  case 680:
#line 879 "nest/config.Y"
       {
     if ((yyvsp[0].i) < 256 || (yyvsp[0].i) > 65536) cf_error("Invalid log buffer size");
     (yyval.i) = (yyvsp[0].i);
   }
#line 9923 "obj/conf/cf-parse.tab.c"
    break;

  case 682:
#line 886 "nest/config.Y"
{ proto_apply_cmd((yyvsp[-2].ps), proto_cmd_disable, 1, (uintptr_t) (yyvsp[-1].t)); }
#line 9929 "obj/conf/cf-parse.tab.c"
    break;

  case 684:
#line 888 "nest/config.Y"
{ proto_apply_cmd((yyvsp[-2].ps), proto_cmd_enable, 1, (uintptr_t) (yyvsp[-1].t)); }
#line 9935 "obj/conf/cf-parse.tab.c"
    break;

  case 686:
#line 890 "nest/config.Y"
{ proto_apply_cmd((yyvsp[-2].ps), proto_cmd_restart, 1, (uintptr_t) (yyvsp[-1].t)); }
#line 9941 "obj/conf/cf-parse.tab.c"
    break;

  case 688:
#line 892 "nest/config.Y"
{ proto_apply_cmd((yyvsp[-1].ps), proto_cmd_reload, 1, CMD_RELOAD); }
#line 9947 "obj/conf/cf-parse.tab.c"
    break;

  case 690:
#line 894 "nest/config.Y"
{ proto_apply_cmd((yyvsp[-1].ps), proto_cmd_reload, 1, CMD_RELOAD_IN); }
#line 9953 "obj/conf/cf-parse.tab.c"
    break;

  case 692:
#line 896 "nest/config.Y"
{ proto_apply_cmd((yyvsp[-1].ps), proto_cmd_reload, 1, CMD_RELOAD_OUT); }
#line 9959 "obj/conf/cf-parse.tab.c"
    break;

  case 694:
#line 900 "nest/config.Y"
{ /* Done in debug_args */  }
#line 9965 "obj/conf/cf-parse.tab.c"
    break;

  case 695:
#line 903 "nest/config.Y"
                         { proto_apply_cmd((yyvsp[-1].ps), proto_cmd_debug, 1, (yyvsp[0].i));  }
#line 9971 "obj/conf/cf-parse.tab.c"
    break;

  case 696:
#line 904 "nest/config.Y"
                          { channel_cmd_debug((yyvsp[-1].c), (yyvsp[0].i)); }
#line 9977 "obj/conf/cf-parse.tab.c"
    break;

  case 698:
#line 909 "nest/config.Y"
{ proto_apply_cmd((yyvsp[-2].ps), proto_cmd_mrtdump, 1, (yyvsp[-1].i)); }
#line 9983 "obj/conf/cf-parse.tab.c"
    break;

  case 700:
#line 912 "nest/config.Y"
{ this_cli->restricted = 1; cli_msg(16, "Access restricted"); }
#line 9989 "obj/conf/cf-parse.tab.c"
    break;

  case 701:
#line 915 "nest/config.Y"
                { cf_assert_symbol((yyvsp[0].s), SYM_PROTO); (yyval.ps).ptr = (yyvsp[0].s); (yyval.ps).patt = 0; }
#line 9995 "obj/conf/cf-parse.tab.c"
    break;

  case 702:
#line 916 "nest/config.Y"
        { (yyval.ps).ptr = NULL; (yyval.ps).patt = 1; }
#line 10001 "obj/conf/cf-parse.tab.c"
    break;

  case 703:
#line 917 "nest/config.Y"
        { (yyval.ps).ptr = (yyvsp[0].t); (yyval.ps).patt = 1; }
#line 10007 "obj/conf/cf-parse.tab.c"
    break;

  case 704:
#line 921 "nest/config.Y"
                { cf_assert_symbol((yyvsp[0].s), SYM_PROTO); (yyval.ps).ptr = (yyvsp[0].s); (yyval.ps).patt = 0; }
#line 10013 "obj/conf/cf-parse.tab.c"
    break;

  case 705:
#line 922 "nest/config.Y"
        { (yyval.ps).ptr = NULL; (yyval.ps).patt = 1; }
#line 10019 "obj/conf/cf-parse.tab.c"
    break;

  case 706:
#line 923 "nest/config.Y"
        { (yyval.ps).ptr = (yyvsp[0].t); (yyval.ps).patt = 1; }
#line 10025 "obj/conf/cf-parse.tab.c"
    break;

  case 707:
#line 926 "nest/config.Y"
                         { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_GEN_IGP_METRIC); }
#line 10031 "obj/conf/cf-parse.tab.c"
    break;

  case 709:
#line 38 "proto/bfd/config.Y"
{
  this_proto = proto_config_new(&proto_bfd, (yyvsp[-1].i));
  init_list(&BFD_CFG->patt_list);
  init_list(&BFD_CFG->neigh_list);
  BFD_CFG->accept_ipv4 = BFD_CFG->accept_ipv6 = 1;
  BFD_CFG->accept_direct = BFD_CFG->accept_multihop = 1;
}
#line 10043 "obj/conf/cf-parse.tab.c"
    break;

  case 715:
#line 52 "proto/bfd/config.Y"
                    { BFD_CFG->strict_bind = (yyvsp[0].i); }
#line 10049 "obj/conf/cf-parse.tab.c"
    break;

  case 719:
#line 65 "proto/bfd/config.Y"
                        { BFD_CFG->accept_ipv4 = 1;  BFD_CFG->accept_ipv6 = 0; }
#line 10055 "obj/conf/cf-parse.tab.c"
    break;

  case 720:
#line 66 "proto/bfd/config.Y"
                        { BFD_CFG->accept_ipv4 = 0;  BFD_CFG->accept_ipv6 = 1; }
#line 10061 "obj/conf/cf-parse.tab.c"
    break;

  case 721:
#line 67 "proto/bfd/config.Y"
                        { BFD_CFG->accept_direct = 1;  BFD_CFG->accept_multihop = 0; }
#line 10067 "obj/conf/cf-parse.tab.c"
    break;

  case 722:
#line 68 "proto/bfd/config.Y"
                        { BFD_CFG->accept_direct = 0;  BFD_CFG->accept_multihop = 1; }
#line 10073 "obj/conf/cf-parse.tab.c"
    break;

  case 723:
#line 72 "proto/bfd/config.Y"
   {
     BFD_CFG->accept_ipv4 = BFD_CFG->accept_ipv6 = 1;
     BFD_CFG->accept_direct = BFD_CFG->accept_multihop = 1;
   }
#line 10082 "obj/conf/cf-parse.tab.c"
    break;

  case 725:
#line 80 "proto/bfd/config.Y"
{
  this_ipatt = cfg_allocz(sizeof(struct bfd_iface_config));
  add_tail(&BFD_CFG->patt_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);

  BFD_IFACE->min_rx_int = BFD_DEFAULT_MIN_RX_INT;
  BFD_IFACE->min_tx_int = BFD_DEFAULT_MIN_TX_INT;
  BFD_IFACE->idle_tx_int = BFD_DEFAULT_IDLE_TX_INT;
  BFD_IFACE->multiplier = BFD_DEFAULT_MULTIPLIER;

  reset_passwords();
}
#line 10099 "obj/conf/cf-parse.tab.c"
    break;

  case 726:
#line 94 "proto/bfd/config.Y"
{
  BFD_IFACE->passwords = get_passwords();

  if (!BFD_IFACE->auth_type != !BFD_IFACE->passwords)
    log(L_WARN "Authentication and password options should be used together");

  if (BFD_IFACE->passwords)
  {
    struct password_item *pass;
    WALK_LIST(pass, *BFD_IFACE->passwords)
    {
      if (pass->alg)
        cf_error("Password algorithm option not available in BFD protocol");

      pass->alg = bfd_auth_type_to_hash_alg[BFD_IFACE->auth_type];
    }
  }
}
#line 10122 "obj/conf/cf-parse.tab.c"
    break;

  case 727:
#line 114 "proto/bfd/config.Y"
                    { BFD_IFACE->min_rx_int = BFD_IFACE->min_tx_int = (yyvsp[0].time); }
#line 10128 "obj/conf/cf-parse.tab.c"
    break;

  case 728:
#line 115 "proto/bfd/config.Y"
                           { BFD_IFACE->min_rx_int = (yyvsp[0].time); }
#line 10134 "obj/conf/cf-parse.tab.c"
    break;

  case 729:
#line 116 "proto/bfd/config.Y"
                           { BFD_IFACE->min_tx_int = (yyvsp[0].time); }
#line 10140 "obj/conf/cf-parse.tab.c"
    break;

  case 730:
#line 117 "proto/bfd/config.Y"
                            { BFD_IFACE->idle_tx_int = (yyvsp[0].time); }
#line 10146 "obj/conf/cf-parse.tab.c"
    break;

  case 731:
#line 118 "proto/bfd/config.Y"
                   { BFD_IFACE->multiplier = (yyvsp[0].i); }
#line 10152 "obj/conf/cf-parse.tab.c"
    break;

  case 732:
#line 119 "proto/bfd/config.Y"
                { BFD_IFACE->passive = (yyvsp[0].i); }
#line 10158 "obj/conf/cf-parse.tab.c"
    break;

  case 733:
#line 120 "proto/bfd/config.Y"
                                { BFD_IFACE->auth_type = (yyvsp[0].i); }
#line 10164 "obj/conf/cf-parse.tab.c"
    break;

  case 734:
#line 121 "proto/bfd/config.Y"
                 {}
#line 10170 "obj/conf/cf-parse.tab.c"
    break;

  case 735:
#line 125 "proto/bfd/config.Y"
                         { (yyval.i) = BFD_AUTH_NONE; }
#line 10176 "obj/conf/cf-parse.tab.c"
    break;

  case 736:
#line 126 "proto/bfd/config.Y"
                         { (yyval.i) = BFD_AUTH_SIMPLE; }
#line 10182 "obj/conf/cf-parse.tab.c"
    break;

  case 737:
#line 127 "proto/bfd/config.Y"
                         { (yyval.i) = BFD_AUTH_KEYED_MD5; }
#line 10188 "obj/conf/cf-parse.tab.c"
    break;

  case 738:
#line 128 "proto/bfd/config.Y"
                         { (yyval.i) = BFD_AUTH_KEYED_SHA1; }
#line 10194 "obj/conf/cf-parse.tab.c"
    break;

  case 739:
#line 129 "proto/bfd/config.Y"
                         { (yyval.i) = BFD_AUTH_METICULOUS_KEYED_MD5; }
#line 10200 "obj/conf/cf-parse.tab.c"
    break;

  case 740:
#line 130 "proto/bfd/config.Y"
                         { (yyval.i) = BFD_AUTH_METICULOUS_KEYED_SHA1; }
#line 10206 "obj/conf/cf-parse.tab.c"
    break;

  case 746:
#line 148 "proto/bfd/config.Y"
{ BFD_CFG->multihop = BFD_IFACE; }
#line 10212 "obj/conf/cf-parse.tab.c"
    break;

  case 747:
#line 152 "proto/bfd/config.Y"
               { (yyval.iface) = NULL; }
#line 10218 "obj/conf/cf-parse.tab.c"
    break;

  case 748:
#line 153 "proto/bfd/config.Y"
              { (yyval.iface) = if_get_by_name((yyvsp[0].s)->name); }
#line 10224 "obj/conf/cf-parse.tab.c"
    break;

  case 749:
#line 154 "proto/bfd/config.Y"
            { (yyval.iface) = if_get_by_name((yyvsp[0].t)); }
#line 10230 "obj/conf/cf-parse.tab.c"
    break;

  case 750:
#line 158 "proto/bfd/config.Y"
               { (yyval.a) = IPA_NONE; }
#line 10236 "obj/conf/cf-parse.tab.c"
    break;

  case 751:
#line 159 "proto/bfd/config.Y"
             { (yyval.a) = (yyvsp[0].a); }
#line 10242 "obj/conf/cf-parse.tab.c"
    break;

  case 752:
#line 163 "proto/bfd/config.Y"
               { (yyval.i) = 0; }
#line 10248 "obj/conf/cf-parse.tab.c"
    break;

  case 753:
#line 164 "proto/bfd/config.Y"
                 { (yyval.i) = (yyvsp[0].i); }
#line 10254 "obj/conf/cf-parse.tab.c"
    break;

  case 754:
#line 168 "proto/bfd/config.Y"
{
  this_bfd_neighbor = cfg_allocz(sizeof(struct bfd_neighbor));
  add_tail(&BFD_CFG->neigh_list, NODE this_bfd_neighbor);

  BFD_NEIGHBOR->addr = (yyvsp[-3].a);
  BFD_NEIGHBOR->local = (yyvsp[-1].a);
  BFD_NEIGHBOR->iface = (yyvsp[-2].iface);
  BFD_NEIGHBOR->multihop = (yyvsp[0].i);

  if ((yyvsp[0].i) && (yyvsp[-2].iface))
    cf_error("Neighbor cannot set both interface and multihop");

  if ((yyvsp[0].i) && ipa_zero((yyvsp[-1].a)))
    cf_error("Multihop neighbor requires specified local address");
}
#line 10274 "obj/conf/cf-parse.tab.c"
    break;

  case 756:
#line 187 "proto/bfd/config.Y"
{ PROTO_WALK_CMD((yyvsp[-1].s), &proto_bfd, p) bfd_show_sessions(p); }
#line 10280 "obj/conf/cf-parse.tab.c"
    break;

  case 758:
#line 36 "proto/babel/config.Y"
{
  this_proto = proto_config_new(&proto_babel, (yyvsp[-1].i));
  init_list(&BABEL_CFG->iface_list);
  BABEL_CFG->hold_time = 1 S_;
}
#line 10290 "obj/conf/cf-parse.tab.c"
    break;

  case 762:
#line 46 "proto/babel/config.Y"
                            { BABEL_CFG->randomize_router_id = (yyvsp[0].i); }
#line 10296 "obj/conf/cf-parse.tab.c"
    break;

  case 766:
#line 59 "proto/babel/config.Y"
{
  this_ipatt = cfg_allocz(sizeof(struct babel_iface_config));
  add_tail(&BABEL_CFG->iface_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);
  reset_passwords();

  BABEL_IFACE->port = BABEL_PORT;
  BABEL_IFACE->type = BABEL_IFACE_TYPE_WIRED;
  BABEL_IFACE->limit = BABEL_HELLO_LIMIT;
  BABEL_IFACE->tx_tos = IP_PREC_INTERNET_CONTROL;
  BABEL_IFACE->tx_priority = sk_priority_control;
  BABEL_IFACE->check_link = 1;
  BABEL_IFACE->ext_next_hop = 1;
}
#line 10315 "obj/conf/cf-parse.tab.c"
    break;

  case 767:
#line 76 "proto/babel/config.Y"
{
  if (BABEL_IFACE->type == BABEL_IFACE_TYPE_WIRELESS)
  {
    if (!BABEL_IFACE->hello_interval)
      BABEL_IFACE->hello_interval = BABEL_HELLO_INTERVAL_WIRELESS;
    if (!BABEL_IFACE->rxcost)
      BABEL_IFACE->rxcost = BABEL_RXCOST_WIRELESS;
  }
  else
  {
    if (!BABEL_IFACE->hello_interval)
      BABEL_IFACE->hello_interval = BABEL_HELLO_INTERVAL_WIRED;
    if (!BABEL_IFACE->rxcost)
      BABEL_IFACE->rxcost = BABEL_RXCOST_WIRED;
  }

  /* Make sure we do not overflow the 16-bit centisec fields */
  if (!BABEL_IFACE->update_interval)
    BABEL_IFACE->update_interval = MIN_(BABEL_IFACE->hello_interval*BABEL_UPDATE_INTERVAL_FACTOR, BABEL_MAX_INTERVAL);
  BABEL_IFACE->ihu_interval = MIN_(BABEL_IFACE->hello_interval*BABEL_IHU_INTERVAL_FACTOR, BABEL_MAX_INTERVAL);

  BABEL_CFG->hold_time = MAX_(BABEL_CFG->hold_time, BABEL_IFACE->update_interval*BABEL_HOLD_TIME_FACTOR);

  BABEL_IFACE->passwords = get_passwords();

  if (!BABEL_IFACE->auth_type != !BABEL_IFACE->passwords)
    cf_error("Authentication and password options should be used together");

  if (BABEL_IFACE->passwords)
  {
    struct password_item *pass;
    uint len = 0, i = 0;

    WALK_LIST(pass, *BABEL_IFACE->passwords)
    {
      /* Set default crypto algorithm (HMAC-SHA256) */
      if (!pass->alg)
	pass->alg = ALG_HMAC_SHA256;

      if (!((pass->alg & ALG_HMAC) ||
	    (pass->alg == ALG_BLAKE2S_128) ||
	    (pass->alg == ALG_BLAKE2S_256) ||
	    (pass->alg == ALG_BLAKE2B_256) ||
	    (pass->alg == ALG_BLAKE2B_512)))
	cf_error("Only HMAC and Blake2 algorithms are supported");

      len += mac_type_length(pass->alg);
      i++;
    }

    BABEL_IFACE->mac_num_keys = i;
    BABEL_IFACE->mac_total_len = len;
  }

}
#line 10375 "obj/conf/cf-parse.tab.c"
    break;

  case 769:
#line 134 "proto/babel/config.Y"
             { BABEL_IFACE->port = (yyvsp[0].i); if (((yyvsp[0].i)<1) || ((yyvsp[0].i)>65535)) cf_error("Invalid port number"); }
#line 10381 "obj/conf/cf-parse.tab.c"
    break;

  case 770:
#line 135 "proto/babel/config.Y"
               { BABEL_IFACE->rxcost = (yyvsp[0].i); if (((yyvsp[0].i)<1) || ((yyvsp[0].i)>65535)) cf_error("Invalid rxcost"); }
#line 10387 "obj/conf/cf-parse.tab.c"
    break;

  case 771:
#line 136 "proto/babel/config.Y"
              { BABEL_IFACE->limit = (yyvsp[0].i); if (((yyvsp[0].i)<1) || ((yyvsp[0].i)>16)) cf_error("Limit must be in range 1-16"); }
#line 10393 "obj/conf/cf-parse.tab.c"
    break;

  case 772:
#line 137 "proto/babel/config.Y"
              { BABEL_IFACE->type = BABEL_IFACE_TYPE_WIRED; }
#line 10399 "obj/conf/cf-parse.tab.c"
    break;

  case 773:
#line 138 "proto/babel/config.Y"
                 { BABEL_IFACE->type = BABEL_IFACE_TYPE_WIRELESS; }
#line 10405 "obj/conf/cf-parse.tab.c"
    break;

  case 774:
#line 139 "proto/babel/config.Y"
                          { BABEL_IFACE->hello_interval = (yyvsp[0].time); if (((yyvsp[0].time)<BABEL_MIN_INTERVAL) || ((yyvsp[0].time)>BABEL_MAX_INTERVAL)) cf_error("Hello interval must be in range 10 ms - 655 s"); }
#line 10411 "obj/conf/cf-parse.tab.c"
    break;

  case 775:
#line 140 "proto/babel/config.Y"
                           { BABEL_IFACE->update_interval = (yyvsp[0].time); if (((yyvsp[0].time)<BABEL_MIN_INTERVAL) || ((yyvsp[0].time)>BABEL_MAX_INTERVAL)) cf_error("Update interval must be in range 10 ms - 655 s"); }
#line 10417 "obj/conf/cf-parse.tab.c"
    break;

  case 776:
#line 141 "proto/babel/config.Y"
                  { BABEL_IFACE->rx_buffer = (yyvsp[0].i); if (((yyvsp[0].i)<256) || ((yyvsp[0].i)>65535)) cf_error("RX buffer must be in range 256-65535"); }
#line 10423 "obj/conf/cf-parse.tab.c"
    break;

  case 777:
#line 142 "proto/babel/config.Y"
                  { BABEL_IFACE->tx_length = (yyvsp[0].i); if (((yyvsp[0].i)<256) || ((yyvsp[0].i)>65535)) cf_error("TX length must be in range 256-65535"); }
#line 10429 "obj/conf/cf-parse.tab.c"
    break;

  case 778:
#line 143 "proto/babel/config.Y"
          { BABEL_IFACE->tx_tos = (yyvsp[0].i); }
#line 10435 "obj/conf/cf-parse.tab.c"
    break;

  case 779:
#line 144 "proto/babel/config.Y"
                    { BABEL_IFACE->tx_priority = (yyvsp[0].i); }
#line 10441 "obj/conf/cf-parse.tab.c"
    break;

  case 780:
#line 145 "proto/babel/config.Y"
                   { BABEL_IFACE->check_link = (yyvsp[0].i); }
#line 10447 "obj/conf/cf-parse.tab.c"
    break;

  case 781:
#line 146 "proto/babel/config.Y"
                     { BABEL_IFACE->next_hop_ip4 = (yyvsp[0].a); if (!ipa_is_ip4((yyvsp[0].a))) cf_error("Must be an IPv4 address"); }
#line 10453 "obj/conf/cf-parse.tab.c"
    break;

  case 782:
#line 147 "proto/babel/config.Y"
                     { BABEL_IFACE->next_hop_ip6 = (yyvsp[0].a); if (!ipa_is_ip6((yyvsp[0].a))) cf_error("Must be an IPv6 address"); }
#line 10459 "obj/conf/cf-parse.tab.c"
    break;

  case 783:
#line 148 "proto/babel/config.Y"
                          { BABEL_IFACE->ext_next_hop = (yyvsp[0].i); }
#line 10465 "obj/conf/cf-parse.tab.c"
    break;

  case 784:
#line 149 "proto/babel/config.Y"
                       { BABEL_IFACE->auth_type = BABEL_AUTH_NONE; }
#line 10471 "obj/conf/cf-parse.tab.c"
    break;

  case 785:
#line 150 "proto/babel/config.Y"
                      { BABEL_IFACE->auth_type = BABEL_AUTH_MAC; BABEL_IFACE->auth_permissive = 0; }
#line 10477 "obj/conf/cf-parse.tab.c"
    break;

  case 786:
#line 151 "proto/babel/config.Y"
                                 { BABEL_IFACE->auth_type = BABEL_AUTH_MAC; BABEL_IFACE->auth_permissive = 1; }
#line 10483 "obj/conf/cf-parse.tab.c"
    break;

  case 793:
#line 169 "proto/babel/config.Y"
                           { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_BABEL_METRIC); }
#line 10489 "obj/conf/cf-parse.tab.c"
    break;

  case 795:
#line 174 "proto/babel/config.Y"
{ PROTO_WALK_CMD((yyvsp[-2].s), &proto_babel, p) babel_show_interfaces(p, (yyvsp[-1].t)); }
#line 10495 "obj/conf/cf-parse.tab.c"
    break;

  case 797:
#line 177 "proto/babel/config.Y"
{ PROTO_WALK_CMD((yyvsp[-2].s), &proto_babel, p) babel_show_neighbors(p, (yyvsp[-1].t)); }
#line 10501 "obj/conf/cf-parse.tab.c"
    break;

  case 799:
#line 180 "proto/babel/config.Y"
{ PROTO_WALK_CMD((yyvsp[-2].s), &proto_babel, p) babel_show_entries(p); }
#line 10507 "obj/conf/cf-parse.tab.c"
    break;

  case 801:
#line 183 "proto/babel/config.Y"
{ PROTO_WALK_CMD((yyvsp[-2].s), &proto_babel, p)  babel_show_routes(p); }
#line 10513 "obj/conf/cf-parse.tab.c"
    break;

  case 809:
#line 53 "proto/bgp/config.Y"
                                 {
     this_proto = proto_config_new(&proto_bgp, (yyvsp[-1].i));
     BGP_CFG->local_port = BGP_PORT;
     BGP_CFG->remote_port = BGP_PORT;
     BGP_CFG->multihop = -1;	/* undefined */
     BGP_CFG->hold_time = 240;
     BGP_CFG->initial_hold_time = 240;
     BGP_CFG->compare_path_lengths = 1;
     BGP_CFG->igp_metric = 1;
     BGP_CFG->connect_delay_time = 5;
     BGP_CFG->connect_retry_time = 120;
     BGP_CFG->error_amnesia_time = 300;
     BGP_CFG->error_delay_time_min = 60;
     BGP_CFG->error_delay_time_max = 300;
     BGP_CFG->enable_refresh = 1;
     BGP_CFG->enable_as4 = 1;
     BGP_CFG->enable_hostname = 0;
     BGP_CFG->capabilities = 2;
     BGP_CFG->interpret_communities = 1;
     BGP_CFG->allow_as_sets = 1;
     BGP_CFG->default_local_pref = 100;
     BGP_CFG->gr_mode = BGP_GR_AWARE;
     BGP_CFG->gr_time = 120;
     BGP_CFG->llgr_mode = -1;
     BGP_CFG->llgr_time = 3600;
     BGP_CFG->setkey = 1;
     BGP_CFG->local_role = BGP_ROLE_UNDEFINED;
     BGP_CFG->dynamic_name = "dynbgp";
     BGP_CFG->check_link = -1;
   }
#line 10548 "obj/conf/cf-parse.tab.c"
    break;

  case 811:
#line 87 "proto/bgp/config.Y"
                          { BGP_CFG->local_port = (yyvsp[0].i); if (((yyvsp[0].i)<1) || ((yyvsp[0].i)>65535)) cf_error("Invalid port number"); }
#line 10554 "obj/conf/cf-parse.tab.c"
    break;

  case 812:
#line 88 "proto/bgp/config.Y"
                        { BGP_CFG->local_as = (yyvsp[0].i); }
#line 10560 "obj/conf/cf-parse.tab.c"
    break;

  case 814:
#line 93 "proto/bgp/config.Y"
                          { BGP_CFG->remote_port = (yyvsp[0].i); if (((yyvsp[0].i)<1) || ((yyvsp[0].i)>65535)) cf_error("Invalid port number"); }
#line 10566 "obj/conf/cf-parse.tab.c"
    break;

  case 815:
#line 94 "proto/bgp/config.Y"
                        { BGP_CFG->remote_as = (yyvsp[0].i); }
#line 10572 "obj/conf/cf-parse.tab.c"
    break;

  case 816:
#line 95 "proto/bgp/config.Y"
                         { BGP_CFG->peer_type = BGP_PT_INTERNAL; }
#line 10578 "obj/conf/cf-parse.tab.c"
    break;

  case 817:
#line 96 "proto/bgp/config.Y"
                         { BGP_CFG->peer_type = BGP_PT_EXTERNAL; }
#line 10584 "obj/conf/cf-parse.tab.c"
    break;

  case 818:
#line 101 "proto/bgp/config.Y"
        { (yyval.i) = (yyvsp[0].i) ? ~(1 << 7) : 0; }
#line 10590 "obj/conf/cf-parse.tab.c"
    break;

  case 819:
#line 102 "proto/bgp/config.Y"
                          { (yyval.i) = (yyvsp[-1].i); }
#line 10596 "obj/conf/cf-parse.tab.c"
    break;

  case 821:
#line 107 "proto/bgp/config.Y"
                                     { (yyval.i) = (yyvsp[-2].i) | (yyvsp[0].i); }
#line 10602 "obj/conf/cf-parse.tab.c"
    break;

  case 822:
#line 111 "proto/bgp/config.Y"
                                { (yyval.i) = 1 << 0; }
#line 10608 "obj/conf/cf-parse.tab.c"
    break;

  case 823:
#line 112 "proto/bgp/config.Y"
                                { (yyval.i) = 1 << 1; }
#line 10614 "obj/conf/cf-parse.tab.c"
    break;

  case 824:
#line 113 "proto/bgp/config.Y"
                                { (yyval.i) = 1 << 2; }
#line 10620 "obj/conf/cf-parse.tab.c"
    break;

  case 825:
#line 114 "proto/bgp/config.Y"
                                { (yyval.i) = 1 << 3; }
#line 10626 "obj/conf/cf-parse.tab.c"
    break;

  case 826:
#line 115 "proto/bgp/config.Y"
                                { (yyval.i) = 1 << 4; }
#line 10632 "obj/conf/cf-parse.tab.c"
    break;

  case 827:
#line 116 "proto/bgp/config.Y"
                                { (yyval.i) = 1 << 5; }
#line 10638 "obj/conf/cf-parse.tab.c"
    break;

  case 828:
#line 117 "proto/bgp/config.Y"
                                { (yyval.i) = 1 << 6; }
#line 10644 "obj/conf/cf-parse.tab.c"
    break;

  case 829:
#line 118 "proto/bgp/config.Y"
                                { (yyval.i) = 1 << 7; }
#line 10650 "obj/conf/cf-parse.tab.c"
    break;

  case 830:
#line 119 "proto/bgp/config.Y"
                                { (yyval.i) = 1 << 8; }
#line 10656 "obj/conf/cf-parse.tab.c"
    break;

  case 831:
#line 123 "proto/bgp/config.Y"
             { (yyval.i) = BGP_ROLE_PEER; }
#line 10662 "obj/conf/cf-parse.tab.c"
    break;

  case 832:
#line 124 "proto/bgp/config.Y"
             { (yyval.i) = BGP_ROLE_PROVIDER; }
#line 10668 "obj/conf/cf-parse.tab.c"
    break;

  case 833:
#line 125 "proto/bgp/config.Y"
             { (yyval.i) = BGP_ROLE_CUSTOMER; }
#line 10674 "obj/conf/cf-parse.tab.c"
    break;

  case 834:
#line 126 "proto/bgp/config.Y"
             { (yyval.i) = BGP_ROLE_RS_SERVER; }
#line 10680 "obj/conf/cf-parse.tab.c"
    break;

  case 835:
#line 127 "proto/bgp/config.Y"
             { (yyval.i) = BGP_ROLE_RS_CLIENT; }
#line 10686 "obj/conf/cf-parse.tab.c"
    break;

  case 840:
#line 135 "proto/bgp/config.Y"
                                                  {
     BGP_CFG->local_ip = (yyvsp[-3].a);
     if ((yyvsp[-2].iface)) BGP_CFG->iface = (yyvsp[-2].iface);
   }
#line 10695 "obj/conf/cf-parse.tab.c"
    break;

  case 842:
#line 140 "proto/bgp/config.Y"
                                                     {
     if (ipa_nonzero(BGP_CFG->remote_ip) || BGP_CFG->remote_range)
       cf_error("Only one neighbor per BGP instance is allowed");
     BGP_CFG->remote_ip = (yyvsp[-3].a);
     if ((yyvsp[-2].iface)) BGP_CFG->iface = (yyvsp[-2].iface);
   }
#line 10706 "obj/conf/cf-parse.tab.c"
    break;

  case 843:
#line 146 "proto/bgp/config.Y"
                                                    {
     if (ipa_nonzero(BGP_CFG->remote_ip) || BGP_CFG->remote_range)
       cf_error("Only one neighbor per BGP instance is allowed");
     net_addr *n = cfg_alloc((yyvsp[-2].net).length);
     net_copy(n, &((yyvsp[-2].net)));
     BGP_CFG->remote_range = n;
   }
#line 10718 "obj/conf/cf-parse.tab.c"
    break;

  case 844:
#line 153 "proto/bgp/config.Y"
                                { BGP_CFG->iface = if_get_by_name((yyvsp[-1].t)); }
#line 10724 "obj/conf/cf-parse.tab.c"
    break;

  case 845:
#line 154 "proto/bgp/config.Y"
                                     { BGP_CFG->rr_cluster_id = (yyvsp[-1].i32); }
#line 10730 "obj/conf/cf-parse.tab.c"
    break;

  case 846:
#line 155 "proto/bgp/config.Y"
                                { BGP_CFG->rr_client = (yyvsp[-1].i); }
#line 10736 "obj/conf/cf-parse.tab.c"
    break;

  case 847:
#line 156 "proto/bgp/config.Y"
                                { BGP_CFG->rs_client = (yyvsp[-1].i); }
#line 10742 "obj/conf/cf-parse.tab.c"
    break;

  case 848:
#line 157 "proto/bgp/config.Y"
                                    { BGP_CFG->confederation = (yyvsp[-1].i); }
#line 10748 "obj/conf/cf-parse.tab.c"
    break;

  case 849:
#line 158 "proto/bgp/config.Y"
                                           { BGP_CFG->confederation_member = (yyvsp[-1].i); }
#line 10754 "obj/conf/cf-parse.tab.c"
    break;

  case 850:
#line 159 "proto/bgp/config.Y"
                                { BGP_CFG->hold_time = (yyvsp[-1].i); if (((yyvsp[-1].i) && (yyvsp[-1].i)<3) || ((yyvsp[-1].i)>65535)) cf_error("Hold time must be in range 3-65535 or zero"); }
#line 10760 "obj/conf/cf-parse.tab.c"
    break;

  case 851:
#line 160 "proto/bgp/config.Y"
                                    { BGP_CFG->min_hold_time = (yyvsp[-1].i); }
#line 10766 "obj/conf/cf-parse.tab.c"
    break;

  case 852:
#line 161 "proto/bgp/config.Y"
                                        { BGP_CFG->initial_hold_time = (yyvsp[-1].i); }
#line 10772 "obj/conf/cf-parse.tab.c"
    break;

  case 853:
#line 162 "proto/bgp/config.Y"
                        { BGP_CFG->multihop = 0; }
#line 10778 "obj/conf/cf-parse.tab.c"
    break;

  case 854:
#line 163 "proto/bgp/config.Y"
                          { BGP_CFG->multihop = 64; }
#line 10784 "obj/conf/cf-parse.tab.c"
    break;

  case 855:
#line 164 "proto/bgp/config.Y"
                               { BGP_CFG->multihop = (yyvsp[-1].i); if (((yyvsp[-1].i)<1) || ((yyvsp[-1].i)>255)) cf_error("Multihop must be in range 1-255"); }
#line 10790 "obj/conf/cf-parse.tab.c"
    break;

  case 856:
#line 165 "proto/bgp/config.Y"
                                   {
     if (strchr((yyvsp[-1].t), '%')) cf_error("Forbidden character '%%' in dynamic name");
     if (strlen((yyvsp[-1].t)) > (SYM_MAX_LEN - 16)) cf_error("Dynamic name too long");
     BGP_CFG->dynamic_name = (yyvsp[-1].t);
   }
#line 10800 "obj/conf/cf-parse.tab.c"
    break;

  case 857:
#line 170 "proto/bgp/config.Y"
                                          { BGP_CFG->dynamic_name_digits = (yyvsp[-1].i); if ((yyvsp[-1].i)>10) cf_error("Dynamic name digits must be at most 10"); }
#line 10806 "obj/conf/cf-parse.tab.c"
    break;

  case 858:
#line 171 "proto/bgp/config.Y"
                                  { BGP_CFG->strict_bind = (yyvsp[-1].i); }
#line 10812 "obj/conf/cf-parse.tab.c"
    break;

  case 859:
#line 172 "proto/bgp/config.Y"
                                { BGP_CFG->free_bind = (yyvsp[-1].i); }
#line 10818 "obj/conf/cf-parse.tab.c"
    break;

  case 860:
#line 173 "proto/bgp/config.Y"
                                  { BGP_CFG->compare_path_lengths = (yyvsp[-1].i); }
#line 10824 "obj/conf/cf-parse.tab.c"
    break;

  case 861:
#line 174 "proto/bgp/config.Y"
                                 { BGP_CFG->med_metric = (yyvsp[-1].i); }
#line 10830 "obj/conf/cf-parse.tab.c"
    break;

  case 862:
#line 175 "proto/bgp/config.Y"
                                 { BGP_CFG->igp_metric = (yyvsp[-1].i); }
#line 10836 "obj/conf/cf-parse.tab.c"
    break;

  case 863:
#line 176 "proto/bgp/config.Y"
                                   { BGP_CFG->prefer_older = (yyvsp[-1].i); }
#line 10842 "obj/conf/cf-parse.tab.c"
    break;

  case 864:
#line 177 "proto/bgp/config.Y"
                                        { BGP_CFG->deterministic_med = (yyvsp[-1].i); }
#line 10848 "obj/conf/cf-parse.tab.c"
    break;

  case 865:
#line 178 "proto/bgp/config.Y"
                                      { BGP_CFG->default_med = (yyvsp[-1].i); }
#line 10854 "obj/conf/cf-parse.tab.c"
    break;

  case 866:
#line 179 "proto/bgp/config.Y"
                                             { BGP_CFG->default_local_pref = (yyvsp[-1].i); }
#line 10860 "obj/conf/cf-parse.tab.c"
    break;

  case 867:
#line 180 "proto/bgp/config.Y"
                                    { BGP_CFG->local_ip = (yyvsp[-1].a); }
#line 10866 "obj/conf/cf-parse.tab.c"
    break;

  case 868:
#line 181 "proto/bgp/config.Y"
                                       { BGP_CFG->connect_delay_time = (yyvsp[-1].i); log(L_WARN "%s: Start delay time option is deprecated, use connect delay time", this_proto->name); }
#line 10872 "obj/conf/cf-parse.tab.c"
    break;

  case 869:
#line 182 "proto/bgp/config.Y"
                                         { BGP_CFG->connect_delay_time = (yyvsp[-1].i); }
#line 10878 "obj/conf/cf-parse.tab.c"
    break;

  case 870:
#line 183 "proto/bgp/config.Y"
                                         { BGP_CFG->connect_retry_time = (yyvsp[-1].i); }
#line 10884 "obj/conf/cf-parse.tab.c"
    break;

  case 871:
#line 184 "proto/bgp/config.Y"
                                     { BGP_CFG->keepalive_time = (yyvsp[-1].i); if (((yyvsp[-1].i)<1) || ((yyvsp[-1].i)>65535)) cf_error("Keepalive time must be in range 1-65535"); }
#line 10890 "obj/conf/cf-parse.tab.c"
    break;

  case 872:
#line 185 "proto/bgp/config.Y"
                                         { BGP_CFG->min_keepalive_time = (yyvsp[-1].i); }
#line 10896 "obj/conf/cf-parse.tab.c"
    break;

  case 873:
#line 186 "proto/bgp/config.Y"
                                        { BGP_CFG->error_amnesia_time = (yyvsp[-1].i); }
#line 10902 "obj/conf/cf-parse.tab.c"
    break;

  case 874:
#line 187 "proto/bgp/config.Y"
                                               { BGP_CFG->error_delay_time_min = (yyvsp[-3].i); BGP_CFG->error_delay_time_max = (yyvsp[-1].i); }
#line 10908 "obj/conf/cf-parse.tab.c"
    break;

  case 875:
#line 188 "proto/bgp/config.Y"
                                          { BGP_CFG->disable_after_error = (yyvsp[-1].i); }
#line 10914 "obj/conf/cf-parse.tab.c"
    break;

  case 876:
#line 189 "proto/bgp/config.Y"
                                                    { BGP_CFG->disable_after_cease = (yyvsp[-1].i); }
#line 10920 "obj/conf/cf-parse.tab.c"
    break;

  case 877:
#line 190 "proto/bgp/config.Y"
                                           { BGP_CFG->enable_refresh = (yyvsp[-1].i); }
#line 10926 "obj/conf/cf-parse.tab.c"
    break;

  case 878:
#line 191 "proto/bgp/config.Y"
                                 { BGP_CFG->enable_as4 = (yyvsp[-1].i); }
#line 10932 "obj/conf/cf-parse.tab.c"
    break;

  case 879:
#line 192 "proto/bgp/config.Y"
                                               { BGP_CFG->enable_extended_messages = (yyvsp[-1].i); }
#line 10938 "obj/conf/cf-parse.tab.c"
    break;

  case 880:
#line 193 "proto/bgp/config.Y"
                                         { BGP_CFG->enable_hostname = (yyvsp[-1].i); }
#line 10944 "obj/conf/cf-parse.tab.c"
    break;

  case 881:
#line 194 "proto/bgp/config.Y"
                                   { BGP_CFG->capabilities = (yyvsp[-1].i); }
#line 10950 "obj/conf/cf-parse.tab.c"
    break;

  case 882:
#line 195 "proto/bgp/config.Y"
                               { BGP_CFG->password = (yyvsp[-1].t); }
#line 10956 "obj/conf/cf-parse.tab.c"
    break;

  case 883:
#line 196 "proto/bgp/config.Y"
                             { BGP_CFG->setkey = (yyvsp[-1].i); }
#line 10962 "obj/conf/cf-parse.tab.c"
    break;

  case 884:
#line 197 "proto/bgp/config.Y"
                              { BGP_CFG->passive = (yyvsp[-1].i); }
#line 10968 "obj/conf/cf-parse.tab.c"
    break;

  case 885:
#line 198 "proto/bgp/config.Y"
                                            { BGP_CFG->interpret_communities = (yyvsp[-1].i); }
#line 10974 "obj/conf/cf-parse.tab.c"
    break;

  case 886:
#line 199 "proto/bgp/config.Y"
                                { BGP_CFG->allow_local_as = -1; }
#line 10980 "obj/conf/cf-parse.tab.c"
    break;

  case 887:
#line 200 "proto/bgp/config.Y"
                                     { BGP_CFG->allow_local_as = (yyvsp[-1].i); }
#line 10986 "obj/conf/cf-parse.tab.c"
    break;

  case 888:
#line 201 "proto/bgp/config.Y"
                                           { BGP_CFG->allow_local_pref = (yyvsp[-1].i); }
#line 10992 "obj/conf/cf-parse.tab.c"
    break;

  case 889:
#line 202 "proto/bgp/config.Y"
                                    { BGP_CFG->allow_med = (yyvsp[-1].i); }
#line 10998 "obj/conf/cf-parse.tab.c"
    break;

  case 890:
#line 203 "proto/bgp/config.Y"
                                    { BGP_CFG->allow_as_sets = (yyvsp[-1].i); }
#line 11004 "obj/conf/cf-parse.tab.c"
    break;

  case 891:
#line 204 "proto/bgp/config.Y"
                                       { BGP_CFG->gr_mode = (yyvsp[-1].i); }
#line 11010 "obj/conf/cf-parse.tab.c"
    break;

  case 892:
#line 205 "proto/bgp/config.Y"
                                        { BGP_CFG->gr_mode = BGP_GR_AWARE; }
#line 11016 "obj/conf/cf-parse.tab.c"
    break;

  case 893:
#line 206 "proto/bgp/config.Y"
                                            { BGP_CFG->gr_time = (yyvsp[-1].i); }
#line 11022 "obj/conf/cf-parse.tab.c"
    break;

  case 894:
#line 207 "proto/bgp/config.Y"
                                                  { BGP_CFG->llgr_mode = (yyvsp[-1].i); }
#line 11028 "obj/conf/cf-parse.tab.c"
    break;

  case 895:
#line 208 "proto/bgp/config.Y"
                                                   { BGP_CFG->llgr_mode = BGP_LLGR_AWARE; }
#line 11034 "obj/conf/cf-parse.tab.c"
    break;

  case 896:
#line 209 "proto/bgp/config.Y"
                                            { BGP_CFG->llgr_time = (yyvsp[-1].i); }
#line 11040 "obj/conf/cf-parse.tab.c"
    break;

  case 897:
#line 210 "proto/bgp/config.Y"
                                   { BGP_CFG->ttl_security = (yyvsp[-1].i); }
#line 11046 "obj/conf/cf-parse.tab.c"
    break;

  case 898:
#line 211 "proto/bgp/config.Y"
                                 { BGP_CFG->check_link = (yyvsp[-1].i); }
#line 11052 "obj/conf/cf-parse.tab.c"
    break;

  case 899:
#line 212 "proto/bgp/config.Y"
                          { if ((yyvsp[-1].i)) init_bfd_opts(&BGP_CFG->bfd); else BGP_CFG->bfd = NULL; }
#line 11058 "obj/conf/cf-parse.tab.c"
    break;

  case 900:
#line 213 "proto/bgp/config.Y"
                              { init_bfd_opts(&BGP_CFG->bfd); BGP_CFG->bfd->mode = BGP_BFD_GRACEFUL; }
#line 11064 "obj/conf/cf-parse.tab.c"
    break;

  case 901:
#line 214 "proto/bgp/config.Y"
                 { open_bfd_opts(&BGP_CFG->bfd); }
#line 11070 "obj/conf/cf-parse.tab.c"
    break;

  case 902:
#line 214 "proto/bgp/config.Y"
                                                            { close_bfd_opts(); }
#line 11076 "obj/conf/cf-parse.tab.c"
    break;

  case 904:
#line 215 "proto/bgp/config.Y"
                                       { BGP_CFG->enforce_first_as = (yyvsp[-1].i); }
#line 11082 "obj/conf/cf-parse.tab.c"
    break;

  case 905:
#line 216 "proto/bgp/config.Y"
                                          { BGP_CFG->local_role = (yyvsp[-1].i); }
#line 11088 "obj/conf/cf-parse.tab.c"
    break;

  case 906:
#line 217 "proto/bgp/config.Y"
                                    { BGP_CFG->require_roles = (yyvsp[-1].i); }
#line 11094 "obj/conf/cf-parse.tab.c"
    break;

  case 907:
#line 221 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_IPV4; }
#line 11100 "obj/conf/cf-parse.tab.c"
    break;

  case 908:
#line 222 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_IPV6; }
#line 11106 "obj/conf/cf-parse.tab.c"
    break;

  case 909:
#line 223 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_IPV4_MC; }
#line 11112 "obj/conf/cf-parse.tab.c"
    break;

  case 910:
#line 224 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_IPV6_MC; }
#line 11118 "obj/conf/cf-parse.tab.c"
    break;

  case 911:
#line 225 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_IPV4_MPLS; }
#line 11124 "obj/conf/cf-parse.tab.c"
    break;

  case 912:
#line 226 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_IPV6_MPLS; }
#line 11130 "obj/conf/cf-parse.tab.c"
    break;

  case 913:
#line 227 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_VPN4_MPLS; }
#line 11136 "obj/conf/cf-parse.tab.c"
    break;

  case 914:
#line 228 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_VPN6_MPLS; }
#line 11142 "obj/conf/cf-parse.tab.c"
    break;

  case 915:
#line 229 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_VPN4_MC; }
#line 11148 "obj/conf/cf-parse.tab.c"
    break;

  case 916:
#line 230 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_VPN6_MC; }
#line 11154 "obj/conf/cf-parse.tab.c"
    break;

  case 917:
#line 231 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_FLOW4; }
#line 11160 "obj/conf/cf-parse.tab.c"
    break;

  case 918:
#line 232 "proto/bgp/config.Y"
                        { (yyval.i32) = BGP_AF_FLOW6; }
#line 11166 "obj/conf/cf-parse.tab.c"
    break;

  case 919:
#line 236 "proto/bgp/config.Y"
{
  const struct bgp_af_desc *desc = bgp_get_af_desc((yyvsp[0].i32));

  if (!desc)
    cf_error("Unknown AFI/SAFI");

  this_channel = channel_config_get(&channel_bgp, desc->name, desc->net, this_proto);

  /* New channel */
  if (!BGP_CC->desc)
  {
    BGP_CC->c.in_filter = FILTER_UNDEF;
    BGP_CC->c.out_filter = FILTER_UNDEF;
    BGP_CC->c.ra_mode = RA_UNDEF;
    BGP_CC->afi = (yyvsp[0].i32);
    BGP_CC->desc = desc;
    BGP_CC->next_hop_keep = 0xff; /* undefined */
    BGP_CC->gr_able = 0xff;	/* undefined */
    BGP_CC->llgr_able = 0xff;	/* undefined */
    BGP_CC->llgr_time = ~0U;	/* undefined */
    BGP_CC->aigp = 0xff;	/* undefined */
  }
}
#line 11194 "obj/conf/cf-parse.tab.c"
    break;

  case 920:
#line 261 "proto/bgp/config.Y"
        { (yyval.i) = (yyvsp[0].i); }
#line 11200 "obj/conf/cf-parse.tab.c"
    break;

  case 921:
#line 262 "proto/bgp/config.Y"
        { (yyval.i) = NH_IBGP; }
#line 11206 "obj/conf/cf-parse.tab.c"
    break;

  case 922:
#line 263 "proto/bgp/config.Y"
        { (yyval.i) = NH_EBGP; }
#line 11212 "obj/conf/cf-parse.tab.c"
    break;

  case 927:
#line 269 "proto/bgp/config.Y"
                        { BGP_CC->next_hop_addr = (yyvsp[0].a); }
#line 11218 "obj/conf/cf-parse.tab.c"
    break;

  case 928:
#line 270 "proto/bgp/config.Y"
                        { BGP_CC->next_hop_self = (yyvsp[0].i); }
#line 11224 "obj/conf/cf-parse.tab.c"
    break;

  case 929:
#line 271 "proto/bgp/config.Y"
                        { BGP_CC->next_hop_keep = (yyvsp[0].i); }
#line 11230 "obj/conf/cf-parse.tab.c"
    break;

  case 930:
#line 272 "proto/bgp/config.Y"
                          { BGP_CC->next_hop_prefer = NHP_GLOBAL; }
#line 11236 "obj/conf/cf-parse.tab.c"
    break;

  case 931:
#line 273 "proto/bgp/config.Y"
                  { BGP_CC->mandatory = (yyvsp[0].i); }
#line 11242 "obj/conf/cf-parse.tab.c"
    break;

  case 932:
#line 274 "proto/bgp/config.Y"
                             { log(L_WARN "%s.%s: Missing lladdr option is deprecated and ignored, remove it", this_proto->name, this_channel->name); }
#line 11248 "obj/conf/cf-parse.tab.c"
    break;

  case 933:
#line 275 "proto/bgp/config.Y"
                  { BGP_CC->gw_mode = GW_DIRECT; }
#line 11254 "obj/conf/cf-parse.tab.c"
    break;

  case 934:
#line 276 "proto/bgp/config.Y"
                     { BGP_CC->gw_mode = GW_RECURSIVE; }
#line 11260 "obj/conf/cf-parse.tab.c"
    break;

  case 935:
#line 277 "proto/bgp/config.Y"
                  { BGP_CC->secondary = (yyvsp[0].i); }
#line 11266 "obj/conf/cf-parse.tab.c"
    break;

  case 936:
#line 278 "proto/bgp/config.Y"
                 {
    BGP_CC->validate = (yyvsp[0].i);
    if (BGP_SAFI(BGP_CC->afi) != BGP_SAFI_FLOW)
      cf_error("Validate option limited to flowspec channels");
   }
#line 11276 "obj/conf/cf-parse.tab.c"
    break;

  case 937:
#line 283 "proto/bgp/config.Y"
                         { BGP_CC->gr_able = (yyvsp[0].i); }
#line 11282 "obj/conf/cf-parse.tab.c"
    break;

  case 938:
#line 284 "proto/bgp/config.Y"
                                    { BGP_CC->llgr_able = (yyvsp[0].i); }
#line 11288 "obj/conf/cf-parse.tab.c"
    break;

  case 939:
#line 285 "proto/bgp/config.Y"
                              { BGP_CC->llgr_time = (yyvsp[0].i); }
#line 11294 "obj/conf/cf-parse.tab.c"
    break;

  case 940:
#line 286 "proto/bgp/config.Y"
                          { BGP_CC->ext_next_hop = (yyvsp[0].i); }
#line 11300 "obj/conf/cf-parse.tab.c"
    break;

  case 941:
#line 287 "proto/bgp/config.Y"
                { BGP_CC->add_path = BGP_ADD_PATH_RX; }
#line 11306 "obj/conf/cf-parse.tab.c"
    break;

  case 942:
#line 288 "proto/bgp/config.Y"
                { BGP_CC->add_path = BGP_ADD_PATH_TX; }
#line 11312 "obj/conf/cf-parse.tab.c"
    break;

  case 943:
#line 289 "proto/bgp/config.Y"
                  { BGP_CC->add_path = (yyvsp[0].i) ? BGP_ADD_PATH_FULL : 0; }
#line 11318 "obj/conf/cf-parse.tab.c"
    break;

  case 944:
#line 290 "proto/bgp/config.Y"
                     { BGP_CC->import_table = (yyvsp[0].i); }
#line 11324 "obj/conf/cf-parse.tab.c"
    break;

  case 945:
#line 291 "proto/bgp/config.Y"
                     { BGP_CC->export_table = (yyvsp[0].i); }
#line 11330 "obj/conf/cf-parse.tab.c"
    break;

  case 946:
#line 292 "proto/bgp/config.Y"
             { BGP_CC->aigp = (yyvsp[0].i); BGP_CC->aigp_originate = 0; }
#line 11336 "obj/conf/cf-parse.tab.c"
    break;

  case 947:
#line 293 "proto/bgp/config.Y"
                  { BGP_CC->aigp = 1; BGP_CC->aigp_originate = 1; }
#line 11342 "obj/conf/cf-parse.tab.c"
    break;

  case 948:
#line 294 "proto/bgp/config.Y"
             { BGP_CC->cost = (yyvsp[0].i); if ((yyvsp[0].i) < 1) cf_error("Cost must be positive"); }
#line 11348 "obj/conf/cf-parse.tab.c"
    break;

  case 949:
#line 295 "proto/bgp/config.Y"
                    {
    if (BGP_CC->desc->no_igp)
      cf_error("IGP table not allowed here");

    if ((yyvsp[0].r)->addr_type == NET_IP4)
      BGP_CC->igp_table_ip4 = (yyvsp[0].r);
    else if ((yyvsp[0].r)->addr_type == NET_IP6)
      BGP_CC->igp_table_ip6 = (yyvsp[0].r);
    else
      cf_error("Mismatched IGP table type");
   }
#line 11364 "obj/conf/cf-parse.tab.c"
    break;

  case 950:
#line 306 "proto/bgp/config.Y"
                     {
    if (BGP_SAFI(BGP_CC->afi) != BGP_SAFI_FLOW)
      cf_error("Base table option limited to flowspec channels");

    if (((BGP_CC->afi == BGP_AF_FLOW4) && ((yyvsp[0].r)->addr_type == NET_IP4)) ||
        ((BGP_CC->afi == BGP_AF_FLOW6) && ((yyvsp[0].r)->addr_type == NET_IP6)))
      BGP_CC->base_table = (yyvsp[0].r);
    else
      cf_error("Mismatched base table type");
   }
#line 11379 "obj/conf/cf-parse.tab.c"
    break;

  case 955:
#line 329 "proto/bgp/config.Y"
{
  if (!this_channel->table)
    cf_error("Routing table not specified");

  this_channel = NULL;
}
#line 11390 "obj/conf/cf-parse.tab.c"
    break;

  case 957:
#line 340 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_ENUM_BGP_ORIGIN, EA_CODE(PROTOCOL_BGP, BA_ORIGIN)); }
#line 11396 "obj/conf/cf-parse.tab.c"
    break;

  case 958:
#line 342 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_AS_PATH, T_PATH, EA_CODE(PROTOCOL_BGP, BA_AS_PATH)); }
#line 11402 "obj/conf/cf-parse.tab.c"
    break;

  case 959:
#line 344 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_IP_ADDRESS, T_IP, EA_CODE(PROTOCOL_BGP, BA_NEXT_HOP)); }
#line 11408 "obj/conf/cf-parse.tab.c"
    break;

  case 960:
#line 346 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_CODE(PROTOCOL_BGP, BA_MULTI_EXIT_DISC)); }
#line 11414 "obj/conf/cf-parse.tab.c"
    break;

  case 961:
#line 348 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_CODE(PROTOCOL_BGP, BA_LOCAL_PREF)); }
#line 11420 "obj/conf/cf-parse.tab.c"
    break;

  case 962:
#line 350 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_OPAQUE, T_ENUM_EMPTY, EA_CODE(PROTOCOL_BGP, BA_ATOMIC_AGGR)); }
#line 11426 "obj/conf/cf-parse.tab.c"
    break;

  case 963:
#line 352 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_OPAQUE, T_ENUM_EMPTY, EA_CODE(PROTOCOL_BGP, BA_AGGREGATOR)); }
#line 11432 "obj/conf/cf-parse.tab.c"
    break;

  case 964:
#line 354 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT_SET, T_CLIST, EA_CODE(PROTOCOL_BGP, BA_COMMUNITY)); }
#line 11438 "obj/conf/cf-parse.tab.c"
    break;

  case 965:
#line 356 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_ROUTER_ID, T_QUAD, EA_CODE(PROTOCOL_BGP, BA_ORIGINATOR_ID)); }
#line 11444 "obj/conf/cf-parse.tab.c"
    break;

  case 966:
#line 358 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT_SET, T_CLIST, EA_CODE(PROTOCOL_BGP, BA_CLUSTER_LIST)); }
#line 11450 "obj/conf/cf-parse.tab.c"
    break;

  case 967:
#line 360 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_EC_SET, T_ECLIST, EA_CODE(PROTOCOL_BGP, BA_EXT_COMMUNITY)); }
#line 11456 "obj/conf/cf-parse.tab.c"
    break;

  case 968:
#line 362 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_OPAQUE, T_ENUM_EMPTY, EA_CODE(PROTOCOL_BGP, BA_AIGP)); }
#line 11462 "obj/conf/cf-parse.tab.c"
    break;

  case 969:
#line 364 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_LC_SET, T_LCLIST, EA_CODE(PROTOCOL_BGP, BA_LARGE_COMMUNITY)); }
#line 11468 "obj/conf/cf-parse.tab.c"
    break;

  case 970:
#line 366 "proto/bgp/config.Y"
        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_CODE(PROTOCOL_BGP, BA_ONLY_TO_CUSTOMER)); }
#line 11474 "obj/conf/cf-parse.tab.c"
    break;

  case 972:
#line 29 "proto/mrt/config.Y"
{
  this_proto = proto_config_new(&proto_mrt, (yyvsp[-1].i));
}
#line 11482 "obj/conf/cf-parse.tab.c"
    break;

  case 974:
#line 35 "proto/mrt/config.Y"
                        { MRT_CFG->table_cf = (yyvsp[0].r); }
#line 11488 "obj/conf/cf-parse.tab.c"
    break;

  case 975:
#line 36 "proto/mrt/config.Y"
                        { MRT_CFG->table_expr = (yyvsp[0].t); }
#line 11494 "obj/conf/cf-parse.tab.c"
    break;

  case 976:
#line 37 "proto/mrt/config.Y"
                        { MRT_CFG->filter = (yyvsp[0].f); }
#line 11500 "obj/conf/cf-parse.tab.c"
    break;

  case 977:
#line 38 "proto/mrt/config.Y"
                        { MRT_CFG->filter = (yyvsp[0].f); }
#line 11506 "obj/conf/cf-parse.tab.c"
    break;

  case 978:
#line 39 "proto/mrt/config.Y"
                        { MRT_CFG->filename = (yyvsp[0].t); }
#line 11512 "obj/conf/cf-parse.tab.c"
    break;

  case 979:
#line 40 "proto/mrt/config.Y"
                        { MRT_CFG->period = (yyvsp[0].i); }
#line 11518 "obj/conf/cf-parse.tab.c"
    break;

  case 980:
#line 41 "proto/mrt/config.Y"
                        { MRT_CFG->always_add_path = (yyvsp[0].i); }
#line 11524 "obj/conf/cf-parse.tab.c"
    break;

  case 983:
#line 50 "proto/mrt/config.Y"
                                                     { mrt_check_config(this_proto); }
#line 11530 "obj/conf/cf-parse.tab.c"
    break;

  case 985:
#line 54 "proto/mrt/config.Y"
{ mrt_dump_cmd((yyvsp[-1].md)); }
#line 11536 "obj/conf/cf-parse.tab.c"
    break;

  case 986:
#line 57 "proto/mrt/config.Y"
               { (yyval.md) = cfg_allocz(sizeof(struct mrt_dump_data)); }
#line 11542 "obj/conf/cf-parse.tab.c"
    break;

  case 987:
#line 58 "proto/mrt/config.Y"
                                { (yyval.md) = (yyvsp[-2].md); (yyval.md)->table_ptr = (yyvsp[0].r)->table; }
#line 11548 "obj/conf/cf-parse.tab.c"
    break;

  case 988:
#line 59 "proto/mrt/config.Y"
                                { (yyval.md) = (yyvsp[-2].md); (yyval.md)->table_expr = (yyvsp[0].t); }
#line 11554 "obj/conf/cf-parse.tab.c"
    break;

  case 989:
#line 60 "proto/mrt/config.Y"
                                { (yyval.md) = (yyvsp[-2].md); (yyval.md)->filter = (yyvsp[0].f); }
#line 11560 "obj/conf/cf-parse.tab.c"
    break;

  case 990:
#line 61 "proto/mrt/config.Y"
                                { (yyval.md) = (yyvsp[-1].md); (yyval.md)->filter = (yyvsp[0].f); }
#line 11566 "obj/conf/cf-parse.tab.c"
    break;

  case 991:
#line 62 "proto/mrt/config.Y"
                                { (yyval.md) = (yyvsp[-2].md); (yyval.md)->filename = (yyvsp[0].t); }
#line 11572 "obj/conf/cf-parse.tab.c"
    break;

  case 992:
#line 211 "proto/ospf/config.Y"
                      { ospf_proto_finish(); }
#line 11578 "obj/conf/cf-parse.tab.c"
    break;

  case 993:
#line 214 "proto/ospf/config.Y"
           { (yyval.i) = 1; }
#line 11584 "obj/conf/cf-parse.tab.c"
    break;

  case 994:
#line 215 "proto/ospf/config.Y"
           { (yyval.i) = 1; }
#line 11590 "obj/conf/cf-parse.tab.c"
    break;

  case 995:
#line 216 "proto/ospf/config.Y"
           { (yyval.i) = 0; }
#line 11596 "obj/conf/cf-parse.tab.c"
    break;

  case 996:
#line 220 "proto/ospf/config.Y"
{
  this_proto = proto_config_new(&proto_ospf, (yyvsp[-1].i));
  this_proto->net_type = (yyvsp[0].i) ? NET_IP4 : 0;

  init_list(&OSPF_CFG->area_list);
  init_list(&OSPF_CFG->vlink_list);
  OSPF_CFG->ecmp = rt_default_ecmp;
  OSPF_CFG->tick = OSPF_DEFAULT_TICK;
  OSPF_CFG->ospf2 = (yyvsp[0].i);
  OSPF_CFG->af_ext = !(yyvsp[0].i);
  OSPF_CFG->gr_mode = OSPF_GR_AWARE;
  OSPF_CFG->gr_time = OSPF_DEFAULT_GR_TIME;
}
#line 11614 "obj/conf/cf-parse.tab.c"
    break;

  case 999:
#line 240 "proto/ospf/config.Y"
             { (yyval.i) = 0; }
#line 11620 "obj/conf/cf-parse.tab.c"
    break;

  case 1000:
#line 241 "proto/ospf/config.Y"
             { (yyval.i) = 1; }
#line 11626 "obj/conf/cf-parse.tab.c"
    break;

  case 1001:
#line 246 "proto/ospf/config.Y"
{
  /* TODO: change name for multicast channels */
  (yyval.cc) = this_channel = channel_config_get(NULL, net_label[(yyvsp[-1].i)], (yyvsp[-1].i), this_proto);

  /* Save the multicast flag */
  OSPF_CFG->af_mc = (yyvsp[0].i);
}
#line 11638 "obj/conf/cf-parse.tab.c"
    break;

  case 1004:
#line 258 "proto/ospf/config.Y"
                { this_proto->net_type = (yyvsp[0].cc)->net_type; }
#line 11644 "obj/conf/cf-parse.tab.c"
    break;

  case 1005:
#line 259 "proto/ospf/config.Y"
                      { OSPF_CFG->rfc1583 = (yyvsp[0].i); }
#line 11650 "obj/conf/cf-parse.tab.c"
    break;

  case 1006:
#line 260 "proto/ospf/config.Y"
                { OSPF_CFG->af_ext = (yyvsp[0].i); if (!ospf_cfg_is_v3()) cf_error("RFC5838 option requires OSPFv3"); }
#line 11656 "obj/conf/cf-parse.tab.c"
    break;

  case 1007:
#line 261 "proto/ospf/config.Y"
               { OSPF_CFG->vpn_pe = (yyvsp[0].i); }
#line 11662 "obj/conf/cf-parse.tab.c"
    break;

  case 1008:
#line 262 "proto/ospf/config.Y"
                    { OSPF_CFG->stub_router = (yyvsp[0].i); }
#line 11668 "obj/conf/cf-parse.tab.c"
    break;

  case 1009:
#line 263 "proto/ospf/config.Y"
                         { OSPF_CFG->gr_mode = (yyvsp[0].i); }
#line 11674 "obj/conf/cf-parse.tab.c"
    break;

  case 1010:
#line 264 "proto/ospf/config.Y"
                          { OSPF_CFG->gr_mode = OSPF_GR_AWARE; }
#line 11680 "obj/conf/cf-parse.tab.c"
    break;

  case 1011:
#line 265 "proto/ospf/config.Y"
                              { OSPF_CFG->gr_time = (yyvsp[0].i); if (((yyvsp[0].i) < 1) || ((yyvsp[0].i) > 1800)) cf_error("Graceful restart time must be in range 1-1800"); }
#line 11686 "obj/conf/cf-parse.tab.c"
    break;

  case 1012:
#line 266 "proto/ospf/config.Y"
             { OSPF_CFG->ecmp = (yyvsp[0].i) ? OSPF_DEFAULT_ECMP_LIMIT : 0; }
#line 11692 "obj/conf/cf-parse.tab.c"
    break;

  case 1013:
#line 267 "proto/ospf/config.Y"
                        { OSPF_CFG->ecmp = (yyvsp[-2].i) ? (yyvsp[0].i) : 0; }
#line 11698 "obj/conf/cf-parse.tab.c"
    break;

  case 1014:
#line 268 "proto/ospf/config.Y"
                       { OSPF_CFG->merge_external = (yyvsp[0].i); }
#line 11704 "obj/conf/cf-parse.tab.c"
    break;

  case 1015:
#line 269 "proto/ospf/config.Y"
             { OSPF_CFG->tick = (yyvsp[0].i); if((yyvsp[0].i) <= 0) cf_error("Tick must be greater than zero"); }
#line 11710 "obj/conf/cf-parse.tab.c"
    break;

  case 1016:
#line 270 "proto/ospf/config.Y"
                    { OSPF_CFG->instance_id = (yyvsp[0].i); OSPF_CFG->instance_id_set = 1; if ((yyvsp[0].i) > 255) cf_error("Instance ID must be in range 0-255"); }
#line 11716 "obj/conf/cf-parse.tab.c"
    break;

  case 1018:
#line 274 "proto/ospf/config.Y"
                            {
  this_area = cfg_allocz(sizeof(struct ospf_area_config));
  add_tail(&OSPF_CFG->area_list, NODE this_area);
  this_area->areaid = (yyvsp[0].i32);
  this_area->default_cost = OSPF_DEFAULT_STUB_COST;
  this_area->type = OPT_E;
  this_area->transint = OSPF_DEFAULT_TRANSINT;

  init_list(&this_area->patt_list);
  init_list(&this_area->net_list);
  init_list(&this_area->enet_list);
  init_list(&this_area->stubnet_list);
 }
#line 11734 "obj/conf/cf-parse.tab.c"
    break;

  case 1019:
#line 289 "proto/ospf/config.Y"
                                                  { ospf_area_finish(); }
#line 11740 "obj/conf/cf-parse.tab.c"
    break;

  case 1022:
#line 298 "proto/ospf/config.Y"
             { this_area->type = (yyvsp[0].i) ? 0 : OPT_E; /* We should remove the option */ }
#line 11746 "obj/conf/cf-parse.tab.c"
    break;

  case 1023:
#line 299 "proto/ospf/config.Y"
        { this_area->type = OPT_N; }
#line 11752 "obj/conf/cf-parse.tab.c"
    break;

  case 1024:
#line 300 "proto/ospf/config.Y"
                { this_area->summary = (yyvsp[0].i); }
#line 11758 "obj/conf/cf-parse.tab.c"
    break;

  case 1025:
#line 301 "proto/ospf/config.Y"
                     { this_area->default_nssa = (yyvsp[0].i); }
#line 11764 "obj/conf/cf-parse.tab.c"
    break;

  case 1026:
#line 302 "proto/ospf/config.Y"
                     { this_area->default_cost = (yyvsp[0].i); ospf_check_defcost((yyvsp[0].i)); }
#line 11770 "obj/conf/cf-parse.tab.c"
    break;

  case 1027:
#line 303 "proto/ospf/config.Y"
                      { this_area->default_cost = (yyvsp[0].i) | LSA_EXT3_EBIT; ospf_check_defcost((yyvsp[0].i)); }
#line 11776 "obj/conf/cf-parse.tab.c"
    break;

  case 1028:
#line 304 "proto/ospf/config.Y"
                  { this_area->default_cost = (yyvsp[0].i); ospf_check_defcost((yyvsp[0].i)); }
#line 11782 "obj/conf/cf-parse.tab.c"
    break;

  case 1029:
#line 305 "proto/ospf/config.Y"
                   { this_area->translator = (yyvsp[0].i); }
#line 11788 "obj/conf/cf-parse.tab.c"
    break;

  case 1030:
#line 306 "proto/ospf/config.Y"
                             { this_area->transint = (yyvsp[0].i); }
#line 11794 "obj/conf/cf-parse.tab.c"
    break;

  case 1031:
#line 307 "proto/ospf/config.Y"
            { this_nets = &this_area->net_list; }
#line 11800 "obj/conf/cf-parse.tab.c"
    break;

  case 1033:
#line 308 "proto/ospf/config.Y"
            { this_nets = &this_area->enet_list; }
#line 11806 "obj/conf/cf-parse.tab.c"
    break;

  case 1040:
#line 320 "proto/ospf/config.Y"
          {
     this_stubnet = cfg_allocz(sizeof(struct ospf_stubnet_config));
     add_tail(&this_area->stubnet_list, NODE this_stubnet);
     this_stubnet->prefix = (yyvsp[0].net);
     this_stubnet->cost = COST_D;
   }
#line 11817 "obj/conf/cf-parse.tab.c"
    break;

  case 1043:
#line 334 "proto/ospf/config.Y"
               { this_stubnet->hidden = (yyvsp[0].i); }
#line 11823 "obj/conf/cf-parse.tab.c"
    break;

  case 1044:
#line 335 "proto/ospf/config.Y"
                { this_stubnet->summary = (yyvsp[0].i); }
#line 11829 "obj/conf/cf-parse.tab.c"
    break;

  case 1045:
#line 336 "proto/ospf/config.Y"
             { this_stubnet->cost = (yyvsp[0].i); }
#line 11835 "obj/conf/cf-parse.tab.c"
    break;

  case 1046:
#line 340 "proto/ospf/config.Y"
                                                             { ospf_iface_finish(); }
#line 11841 "obj/conf/cf-parse.tab.c"
    break;

  case 1047:
#line 341 "proto/ospf/config.Y"
                                     { ospf_iface_finish(); }
#line 11847 "obj/conf/cf-parse.tab.c"
    break;

  case 1051:
#line 350 "proto/ospf/config.Y"
              { OSPF_PATT->helloint = (yyvsp[0].i) ; if (((yyvsp[0].i)<=0) || ((yyvsp[0].i)>65535)) cf_error("Hello interval must be in range 1-65535"); }
#line 11853 "obj/conf/cf-parse.tab.c"
    break;

  case 1052:
#line 351 "proto/ospf/config.Y"
                   { OSPF_PATT->rxmtint = (yyvsp[0].i) ; if ((yyvsp[0].i)<=1) cf_error("Retransmit int must be greater than one"); }
#line 11859 "obj/conf/cf-parse.tab.c"
    break;

  case 1053:
#line 352 "proto/ospf/config.Y"
                       { OSPF_PATT->inftransdelay = (yyvsp[0].i) ; if (((yyvsp[0].i)<=0) || ((yyvsp[0].i)>65535)) cf_error("Transmit delay must be in range 1-65535"); }
#line 11865 "obj/conf/cf-parse.tab.c"
    break;

  case 1054:
#line 353 "proto/ospf/config.Y"
             { OSPF_PATT->waitint = (yyvsp[0].i) ; if ((yyvsp[0].i)<=1) cf_error("Wait interval must be greater than one"); }
#line 11871 "obj/conf/cf-parse.tab.c"
    break;

  case 1055:
#line 354 "proto/ospf/config.Y"
             { OSPF_PATT->deadint = (yyvsp[0].i) ; if ((yyvsp[0].i)<=1) cf_error("Dead interval must be greater than one"); }
#line 11877 "obj/conf/cf-parse.tab.c"
    break;

  case 1056:
#line 355 "proto/ospf/config.Y"
                   { OSPF_PATT->deadc = (yyvsp[0].i) ; if ((yyvsp[0].i)<=1) cf_error("Dead count must be greater than one"); }
#line 11883 "obj/conf/cf-parse.tab.c"
    break;

  case 1057:
#line 356 "proto/ospf/config.Y"
                       { OSPF_PATT->autype = OSPF_AUTH_NONE;  }
#line 11889 "obj/conf/cf-parse.tab.c"
    break;

  case 1058:
#line 357 "proto/ospf/config.Y"
                         { OSPF_PATT->autype = OSPF_AUTH_SIMPLE; ospf_check_auth(); }
#line 11895 "obj/conf/cf-parse.tab.c"
    break;

  case 1059:
#line 358 "proto/ospf/config.Y"
                                { OSPF_PATT->autype = OSPF_AUTH_CRYPT; }
#line 11901 "obj/conf/cf-parse.tab.c"
    break;

  case 1061:
#line 363 "proto/ospf/config.Y"
 {
  if (this_area->areaid == 0) cf_error("Virtual link cannot be in backbone");
  this_ipatt = cfg_allocz(sizeof(struct ospf_iface_patt));
  add_tail(&OSPF_CFG->vlink_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);
  OSPF_PATT->voa = this_area->areaid;
  OSPF_PATT->vid = (yyvsp[0].i32);
  OSPF_PATT->helloint = HELLOINT_D;
  OSPF_PATT->rxmtint = RXMTINT_D;
  OSPF_PATT->inftransdelay = INFTRANSDELAY_D;
  OSPF_PATT->deadc = DEADC_D;
  OSPF_PATT->type = OSPF_IT_VLINK;
  init_list(&OSPF_PATT->nbma_list);
  reset_passwords();
 }
#line 11921 "obj/conf/cf-parse.tab.c"
    break;

  case 1062:
#line 381 "proto/ospf/config.Y"
             { OSPF_PATT->cost = (yyvsp[0].i) ; if (((yyvsp[0].i)<=0) || ((yyvsp[0].i)>65535)) cf_error("Cost must be in range 1-65535"); }
#line 11927 "obj/conf/cf-parse.tab.c"
    break;

  case 1063:
#line 382 "proto/ospf/config.Y"
              { OSPF_PATT->helloint = (yyvsp[0].i) ; if (((yyvsp[0].i)<=0) || ((yyvsp[0].i)>65535)) cf_error("Hello interval must be in range 1-65535"); }
#line 11933 "obj/conf/cf-parse.tab.c"
    break;

  case 1064:
#line 383 "proto/ospf/config.Y"
             { OSPF_PATT->pollint = (yyvsp[0].i) ; if ((yyvsp[0].i)<=0) cf_error("Poll int must be greater than zero"); }
#line 11939 "obj/conf/cf-parse.tab.c"
    break;

  case 1065:
#line 384 "proto/ospf/config.Y"
                   { OSPF_PATT->rxmtint = (yyvsp[0].i) ; if ((yyvsp[0].i)<=1) cf_error("Retransmit int must be greater than one"); }
#line 11945 "obj/conf/cf-parse.tab.c"
    break;

  case 1066:
#line 385 "proto/ospf/config.Y"
             { OSPF_PATT->waitint = (yyvsp[0].i) ; if ((yyvsp[0].i)<=1) cf_error("Wait interval must be greater than one"); }
#line 11951 "obj/conf/cf-parse.tab.c"
    break;

  case 1067:
#line 386 "proto/ospf/config.Y"
             { OSPF_PATT->deadint = (yyvsp[0].i) ; if ((yyvsp[0].i)<=1) cf_error("Dead interval must be greater than one"); }
#line 11957 "obj/conf/cf-parse.tab.c"
    break;

  case 1068:
#line 387 "proto/ospf/config.Y"
                   { OSPF_PATT->deadc = (yyvsp[0].i) ; if ((yyvsp[0].i)<=1) cf_error("Dead count must be greater than one"); }
#line 11963 "obj/conf/cf-parse.tab.c"
    break;

  case 1069:
#line 388 "proto/ospf/config.Y"
                  { OSPF_PATT->type = OSPF_IT_BCAST ; }
#line 11969 "obj/conf/cf-parse.tab.c"
    break;

  case 1070:
#line 389 "proto/ospf/config.Y"
              { OSPF_PATT->type = OSPF_IT_BCAST ; }
#line 11975 "obj/conf/cf-parse.tab.c"
    break;

  case 1071:
#line 390 "proto/ospf/config.Y"
                     { OSPF_PATT->type = OSPF_IT_NBMA ; }
#line 11981 "obj/conf/cf-parse.tab.c"
    break;

  case 1072:
#line 391 "proto/ospf/config.Y"
             { OSPF_PATT->type = OSPF_IT_NBMA ; }
#line 11987 "obj/conf/cf-parse.tab.c"
    break;

  case 1073:
#line 392 "proto/ospf/config.Y"
                    { OSPF_PATT->type = OSPF_IT_PTP ; }
#line 11993 "obj/conf/cf-parse.tab.c"
    break;

  case 1074:
#line 393 "proto/ospf/config.Y"
            { OSPF_PATT->type = OSPF_IT_PTP ; }
#line 11999 "obj/conf/cf-parse.tab.c"
    break;

  case 1075:
#line 394 "proto/ospf/config.Y"
                         { OSPF_PATT->type = OSPF_IT_PTMP ; }
#line 12005 "obj/conf/cf-parse.tab.c"
    break;

  case 1076:
#line 395 "proto/ospf/config.Y"
             { OSPF_PATT->type = OSPF_IT_PTMP ; }
#line 12011 "obj/conf/cf-parse.tab.c"
    break;

  case 1077:
#line 396 "proto/ospf/config.Y"
                       { OSPF_PATT->real_bcast = (yyvsp[0].i); if (!ospf_cfg_is_v2()) cf_error("Real broadcast option requires OSPFv2"); }
#line 12017 "obj/conf/cf-parse.tab.c"
    break;

  case 1078:
#line 397 "proto/ospf/config.Y"
                    { OSPF_PATT->ptp_netmask = (yyvsp[0].i); if (!ospf_cfg_is_v2()) cf_error("PtP netmask option requires OSPFv2"); }
#line 12023 "obj/conf/cf-parse.tab.c"
    break;

  case 1079:
#line 398 "proto/ospf/config.Y"
                    { OSPF_PATT->ptp_address = (yyvsp[0].i); if (!ospf_cfg_is_v2()) cf_error("PtP address option requires OSPFv2"); }
#line 12029 "obj/conf/cf-parse.tab.c"
    break;

  case 1080:
#line 399 "proto/ospf/config.Y"
                       { OSPF_PATT->inftransdelay = (yyvsp[0].i) ; if (((yyvsp[0].i)<=0) || ((yyvsp[0].i)>65535)) cf_error("Transmit delay must be in range 1-65535"); }
#line 12035 "obj/conf/cf-parse.tab.c"
    break;

  case 1081:
#line 400 "proto/ospf/config.Y"
                 { OSPF_PATT->priority = (yyvsp[0].i) ; if ((yyvsp[0].i)>255) cf_error("Priority must be in range 0-255"); }
#line 12041 "obj/conf/cf-parse.tab.c"
    break;

  case 1082:
#line 401 "proto/ospf/config.Y"
                            { OSPF_PATT->strictnbma = (yyvsp[0].i) ; }
#line 12047 "obj/conf/cf-parse.tab.c"
    break;

  case 1083:
#line 402 "proto/ospf/config.Y"
             { OSPF_PATT->stub = (yyvsp[0].i) ; }
#line 12053 "obj/conf/cf-parse.tab.c"
    break;

  case 1084:
#line 403 "proto/ospf/config.Y"
                   { OSPF_PATT->check_link = (yyvsp[0].i); }
#line 12059 "obj/conf/cf-parse.tab.c"
    break;

  case 1085:
#line 404 "proto/ospf/config.Y"
                    { OSPF_PATT->ecmp_weight = (yyvsp[0].i) - 1; if (((yyvsp[0].i)<1) || ((yyvsp[0].i)>256)) cf_error("ECMP weight must be in range 1-256"); }
#line 12065 "obj/conf/cf-parse.tab.c"
    break;

  case 1086:
#line 405 "proto/ospf/config.Y"
                             { OSPF_PATT->link_lsa_suppression = (yyvsp[0].i); if (!ospf_cfg_is_v3()) cf_error("Link LSA suppression option requires OSPFv3"); }
#line 12071 "obj/conf/cf-parse.tab.c"
    break;

  case 1088:
#line 407 "proto/ospf/config.Y"
                       { OSPF_PATT->autype = OSPF_AUTH_NONE; }
#line 12077 "obj/conf/cf-parse.tab.c"
    break;

  case 1089:
#line 408 "proto/ospf/config.Y"
                         { OSPF_PATT->autype = OSPF_AUTH_SIMPLE; ospf_check_auth(); }
#line 12083 "obj/conf/cf-parse.tab.c"
    break;

  case 1090:
#line 409 "proto/ospf/config.Y"
                                { OSPF_PATT->autype = OSPF_AUTH_CRYPT; }
#line 12089 "obj/conf/cf-parse.tab.c"
    break;

  case 1091:
#line 410 "proto/ospf/config.Y"
                    { OSPF_PATT->rx_buffer = 0; }
#line 12095 "obj/conf/cf-parse.tab.c"
    break;

  case 1092:
#line 411 "proto/ospf/config.Y"
                   { OSPF_PATT->rx_buffer = OSPF_MAX_PKT_SIZE; }
#line 12101 "obj/conf/cf-parse.tab.c"
    break;

  case 1093:
#line 412 "proto/ospf/config.Y"
                  { OSPF_PATT->rx_buffer = (yyvsp[0].i); if (((yyvsp[0].i) < OSPF_MIN_PKT_SIZE) || ((yyvsp[0].i) > OSPF_MAX_PKT_SIZE)) cf_error("Buffer size must be in range 256-65535"); }
#line 12107 "obj/conf/cf-parse.tab.c"
    break;

  case 1094:
#line 413 "proto/ospf/config.Y"
          { OSPF_PATT->tx_tos = (yyvsp[0].i); }
#line 12113 "obj/conf/cf-parse.tab.c"
    break;

  case 1095:
#line 414 "proto/ospf/config.Y"
                    { OSPF_PATT->tx_priority = (yyvsp[0].i); }
#line 12119 "obj/conf/cf-parse.tab.c"
    break;

  case 1096:
#line 415 "proto/ospf/config.Y"
                  { OSPF_PATT->tx_length = (yyvsp[0].i); if (((yyvsp[0].i) < OSPF_MIN_PKT_SIZE) || ((yyvsp[0].i) > OSPF_MAX_PKT_SIZE)) cf_error("TX length must be in range 256-65535"); }
#line 12125 "obj/conf/cf-parse.tab.c"
    break;

  case 1097:
#line 416 "proto/ospf/config.Y"
                     { OSPF_PATT->ttl_security = (yyvsp[0].i); }
#line 12131 "obj/conf/cf-parse.tab.c"
    break;

  case 1098:
#line 417 "proto/ospf/config.Y"
                        { OSPF_PATT->ttl_security = 2; }
#line 12137 "obj/conf/cf-parse.tab.c"
    break;

  case 1099:
#line 418 "proto/ospf/config.Y"
            { OSPF_PATT->bfd = (yyvsp[0].i); cf_check_bfd((yyvsp[0].i)); }
#line 12143 "obj/conf/cf-parse.tab.c"
    break;

  case 1104:
#line 430 "proto/ospf/config.Y"
 {
   this_pref = cfg_allocz(sizeof(struct area_net_config));
   add_tail(this_nets, NODE this_pref);
   this_pref->prefix = (yyvsp[0].net);
 }
#line 12153 "obj/conf/cf-parse.tab.c"
    break;

  case 1106:
#line 439 "proto/ospf/config.Y"
          { this_pref->hidden = 1; }
#line 12159 "obj/conf/cf-parse.tab.c"
    break;

  case 1107:
#line 440 "proto/ospf/config.Y"
            { this_pref->tag = (yyvsp[0].i); }
#line 12165 "obj/conf/cf-parse.tab.c"
    break;

  case 1110:
#line 449 "proto/ospf/config.Y"
             { (yyval.i) = 0; }
#line 12171 "obj/conf/cf-parse.tab.c"
    break;

  case 1111:
#line 450 "proto/ospf/config.Y"
            { (yyval.i) = 1; }
#line 12177 "obj/conf/cf-parse.tab.c"
    break;

  case 1112:
#line 454 "proto/ospf/config.Y"
 {
   this_nbma = cfg_allocz(sizeof(struct nbma_node));
   add_tail(&OSPF_PATT->nbma_list, NODE this_nbma);
   this_nbma->ip=(yyvsp[-2].a);
   this_nbma->eligible=(yyvsp[-1].i);
 }
#line 12188 "obj/conf/cf-parse.tab.c"
    break;

  case 1113:
#line 463 "proto/ospf/config.Y"
 {
  this_ipatt = cfg_allocz(sizeof(struct ospf_iface_patt));
  add_tail(&this_area->patt_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);
  OSPF_PATT->cost = COST_D;
  OSPF_PATT->helloint = HELLOINT_D;
  OSPF_PATT->pollint = POLLINT_D;
  OSPF_PATT->rxmtint = RXMTINT_D;
  OSPF_PATT->inftransdelay = INFTRANSDELAY_D;
  OSPF_PATT->priority = PRIORITY_D;
  OSPF_PATT->deadc = DEADC_D;
  OSPF_PATT->type = OSPF_IT_UNDEF;
  init_list(&OSPF_PATT->nbma_list);
  OSPF_PATT->check_link = 1;
  OSPF_PATT->ptp_netmask = 2; /* not specified */
  OSPF_PATT->ptp_address = 2; /* not specified */
  OSPF_PATT->tx_tos = IP_PREC_INTERNET_CONTROL;
  OSPF_PATT->tx_priority = sk_priority_control;
  reset_passwords();
 }
#line 12213 "obj/conf/cf-parse.tab.c"
    break;

  case 1115:
#line 487 "proto/ospf/config.Y"
                 { OSPF_PATT->instance_id = (yyvsp[0].i); OSPF_PATT->instance_id_set = 1; if ((yyvsp[0].i) > 255) cf_error("Instance ID must be in range 0-255"); }
#line 12219 "obj/conf/cf-parse.tab.c"
    break;

  case 1116:
#line 491 "proto/ospf/config.Y"
                   { if (ospf_cfg_is_v3()) iface_patt_check(); }
#line 12225 "obj/conf/cf-parse.tab.c"
    break;

  case 1122:
#line 505 "proto/ospf/config.Y"
                                                            { ospf_iface_finish(); }
#line 12231 "obj/conf/cf-parse.tab.c"
    break;

  case 1123:
#line 508 "proto/ospf/config.Y"
                           { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_OSPF_METRIC1); }
#line 12237 "obj/conf/cf-parse.tab.c"
    break;

  case 1124:
#line 509 "proto/ospf/config.Y"
                           { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_OSPF_METRIC2); }
#line 12243 "obj/conf/cf-parse.tab.c"
    break;

  case 1125:
#line 510 "proto/ospf/config.Y"
                       { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_OSPF_TAG); }
#line 12249 "obj/conf/cf-parse.tab.c"
    break;

  case 1126:
#line 511 "proto/ospf/config.Y"
                             { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_ROUTER_ID, T_QUAD, EA_OSPF_ROUTER_ID); }
#line 12255 "obj/conf/cf-parse.tab.c"
    break;

  case 1128:
#line 515 "proto/ospf/config.Y"
{ PROTO_WALK_CMD((yyvsp[-1].s), &proto_ospf, p) ospf_sh(p); }
#line 12261 "obj/conf/cf-parse.tab.c"
    break;

  case 1130:
#line 518 "proto/ospf/config.Y"
{ PROTO_WALK_CMD((yyvsp[-2].s), &proto_ospf, p) ospf_sh_neigh(p, (yyvsp[-1].t)); }
#line 12267 "obj/conf/cf-parse.tab.c"
    break;

  case 1132:
#line 521 "proto/ospf/config.Y"
{ PROTO_WALK_CMD((yyvsp[-2].s), &proto_ospf, p) ospf_sh_iface(p, (yyvsp[-1].t)); }
#line 12273 "obj/conf/cf-parse.tab.c"
    break;

  case 1134:
#line 526 "proto/ospf/config.Y"
{ ospf_sh_state(proto_get_named((yyvsp[-2].s), &proto_ospf), 0, 1); }
#line 12279 "obj/conf/cf-parse.tab.c"
    break;

  case 1136:
#line 529 "proto/ospf/config.Y"
{ ospf_sh_state(proto_get_named((yyvsp[-2].s), &proto_ospf), 0, 0); }
#line 12285 "obj/conf/cf-parse.tab.c"
    break;

  case 1138:
#line 534 "proto/ospf/config.Y"
{ ospf_sh_state(proto_get_named((yyvsp[-2].s), &proto_ospf), 1, 1); }
#line 12291 "obj/conf/cf-parse.tab.c"
    break;

  case 1140:
#line 537 "proto/ospf/config.Y"
{ ospf_sh_state(proto_get_named((yyvsp[-2].s), &proto_ospf), 1, 0); }
#line 12297 "obj/conf/cf-parse.tab.c"
    break;

  case 1142:
#line 541 "proto/ospf/config.Y"
{
  if (!(yyvsp[-1].ld)->proto)
    (yyvsp[-1].ld)->proto = (struct ospf_proto *) proto_get_named(NULL, &proto_ospf);

  ospf_sh_lsadb((yyvsp[-1].ld));
}
#line 12308 "obj/conf/cf-parse.tab.c"
    break;

  case 1143:
#line 549 "proto/ospf/config.Y"
               {
     (yyval.ld) = cfg_allocz(sizeof(struct lsadb_show_data));
   }
#line 12316 "obj/conf/cf-parse.tab.c"
    break;

  case 1144:
#line 552 "proto/ospf/config.Y"
                     { (yyval.ld) = (yyvsp[-1].ld); (yyval.ld)->scope = LSA_SCOPE_AS; }
#line 12322 "obj/conf/cf-parse.tab.c"
    break;

  case 1145:
#line 553 "proto/ospf/config.Y"
                         { (yyval.ld) = (yyvsp[-2].ld); (yyval.ld)->scope = LSA_SCOPE_AREA; (yyval.ld)->area = (yyvsp[0].i32); }
#line 12328 "obj/conf/cf-parse.tab.c"
    break;

  case 1146:
#line 554 "proto/ospf/config.Y"
                   { (yyval.ld) = (yyvsp[-1].ld); (yyval.ld)->scope = 1; /* hack, 0 is no filter */ }
#line 12334 "obj/conf/cf-parse.tab.c"
    break;

  case 1147:
#line 555 "proto/ospf/config.Y"
                       { (yyval.ld) = (yyvsp[-2].ld); (yyval.ld)->type = (yyvsp[0].i); }
#line 12340 "obj/conf/cf-parse.tab.c"
    break;

  case 1148:
#line 556 "proto/ospf/config.Y"
                         { (yyval.ld) = (yyvsp[-2].ld); (yyval.ld)->lsid = (yyvsp[0].i32); }
#line 12346 "obj/conf/cf-parse.tab.c"
    break;

  case 1149:
#line 557 "proto/ospf/config.Y"
                   { (yyval.ld) = (yyvsp[-1].ld); (yyval.ld)->router = SH_ROUTER_SELF; }
#line 12352 "obj/conf/cf-parse.tab.c"
    break;

  case 1150:
#line 558 "proto/ospf/config.Y"
                           { (yyval.ld) = (yyvsp[-2].ld); (yyval.ld)->router = (yyvsp[0].i32); }
#line 12358 "obj/conf/cf-parse.tab.c"
    break;

  case 1151:
#line 559 "proto/ospf/config.Y"
                           { cf_assert_symbol((yyvsp[0].s), SYM_PROTO); (yyval.ld) = (yyvsp[-1].ld); (yyval.ld)->proto = (struct ospf_proto *) proto_get_named((yyvsp[0].s), &proto_ospf); }
#line 12364 "obj/conf/cf-parse.tab.c"
    break;

  case 1153:
#line 27 "proto/perf/config.Y"
{
  this_proto = proto_config_new(&proto_perf, (yyvsp[-1].i));
  PERF_CFG->from = 10;
  PERF_CFG->to = 20;
  PERF_CFG->repeat = 4;
  PERF_CFG->threshold_max = 500 MS_;
  PERF_CFG->threshold_min = 1 MS_;
  PERF_CFG->attrs_per_rte = 0;
  PERF_CFG->keep = 0;
  PERF_CFG->mode = PERF_MODE_IMPORT;
}
#line 12380 "obj/conf/cf-parse.tab.c"
    break;

  case 1157:
#line 46 "proto/perf/config.Y"
                 { this_proto->net_type = (yyvsp[0].cc)->net_type; }
#line 12386 "obj/conf/cf-parse.tab.c"
    break;

  case 1158:
#line 47 "proto/perf/config.Y"
                { PERF_CFG->from = (yyvsp[0].i); }
#line 12392 "obj/conf/cf-parse.tab.c"
    break;

  case 1159:
#line 48 "proto/perf/config.Y"
              { PERF_CFG->to = (yyvsp[0].i); }
#line 12398 "obj/conf/cf-parse.tab.c"
    break;

  case 1160:
#line 49 "proto/perf/config.Y"
              { PERF_CFG->repeat = (yyvsp[0].i); }
#line 12404 "obj/conf/cf-parse.tab.c"
    break;

  case 1161:
#line 50 "proto/perf/config.Y"
                         { PERF_CFG->threshold_min = (yyvsp[0].time); }
#line 12410 "obj/conf/cf-parse.tab.c"
    break;

  case 1162:
#line 51 "proto/perf/config.Y"
                         { PERF_CFG->threshold_max = (yyvsp[0].time); }
#line 12416 "obj/conf/cf-parse.tab.c"
    break;

  case 1163:
#line 52 "proto/perf/config.Y"
                  { PERF_CFG->attrs_per_rte = (yyvsp[0].i); }
#line 12422 "obj/conf/cf-parse.tab.c"
    break;

  case 1164:
#line 53 "proto/perf/config.Y"
             { PERF_CFG->keep = (yyvsp[0].i); }
#line 12428 "obj/conf/cf-parse.tab.c"
    break;

  case 1165:
#line 54 "proto/perf/config.Y"
               { PERF_CFG->mode = PERF_MODE_IMPORT; }
#line 12434 "obj/conf/cf-parse.tab.c"
    break;

  case 1166:
#line 55 "proto/perf/config.Y"
               { PERF_CFG->mode = PERF_MODE_EXPORT; }
#line 12440 "obj/conf/cf-parse.tab.c"
    break;

  case 1167:
#line 23 "proto/pipe/config.Y"
                      { this_channel = NULL; }
#line 12446 "obj/conf/cf-parse.tab.c"
    break;

  case 1168:
#line 26 "proto/pipe/config.Y"
{
  this_proto = proto_config_new(&proto_pipe, (yyvsp[-1].i));
}
#line 12454 "obj/conf/cf-parse.tab.c"
    break;

  case 1169:
#line 30 "proto/pipe/config.Y"
{
  this_channel = proto_cf_main_channel(this_proto);
  if (!this_channel) {
    this_channel = channel_config_new(NULL, NULL, 0, this_proto);
    this_channel->in_filter = FILTER_ACCEPT;
    this_channel->out_filter = FILTER_ACCEPT;
  }
}
#line 12467 "obj/conf/cf-parse.tab.c"
    break;

  case 1173:
#line 43 "proto/pipe/config.Y"
                                    { PIPE_CFG->peer = (yyvsp[-1].r); }
#line 12473 "obj/conf/cf-parse.tab.c"
    break;

  case 1175:
#line 47 "proto/radv/config.Y"
{
  this_proto = proto_config_new(&proto_radv, (yyvsp[-1].i));
  this_proto->net_type = NET_IP6;

  init_list(&RADV_CFG->patt_list);
  init_list(&RADV_CFG->pref_list);
  init_list(&RADV_CFG->rdnss_list);
  init_list(&RADV_CFG->dnssl_list);
}
#line 12487 "obj/conf/cf-parse.tab.c"
    break;

  case 1179:
#line 61 "proto/radv/config.Y"
                      { add_tail(&RADV_CFG->pref_list, NODE this_radv_prefix); }
#line 12493 "obj/conf/cf-parse.tab.c"
    break;

  case 1180:
#line 62 "proto/radv/config.Y"
         { init_list(&radv_dns_list); }
#line 12499 "obj/conf/cf-parse.tab.c"
    break;

  case 1181:
#line 62 "proto/radv/config.Y"
                                                   { add_tail_list(&RADV_CFG->rdnss_list, &radv_dns_list); }
#line 12505 "obj/conf/cf-parse.tab.c"
    break;

  case 1182:
#line 63 "proto/radv/config.Y"
         { init_list(&radv_dns_list); }
#line 12511 "obj/conf/cf-parse.tab.c"
    break;

  case 1183:
#line 63 "proto/radv/config.Y"
                                                   { add_tail_list(&RADV_CFG->dnssl_list, &radv_dns_list); }
#line 12517 "obj/conf/cf-parse.tab.c"
    break;

  case 1184:
#line 64 "proto/radv/config.Y"
                   { RADV_CFG->trigger = (yyvsp[0].net); }
#line 12523 "obj/conf/cf-parse.tab.c"
    break;

  case 1185:
#line 65 "proto/radv/config.Y"
                         { RADV_CFG->propagate_routes = (yyvsp[0].i); }
#line 12529 "obj/conf/cf-parse.tab.c"
    break;

  case 1189:
#line 78 "proto/radv/config.Y"
{
  this_ipatt = cfg_allocz(sizeof(struct radv_iface_config));
  add_tail(&RADV_CFG->patt_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);
  init_list(&RADV_IFACE->pref_list);
  init_list(&RADV_IFACE->rdnss_list);
  init_list(&RADV_IFACE->dnssl_list);

  RADV_IFACE->min_ra_int = (u32) -1; /* undefined */
  RADV_IFACE->max_ra_int = DEFAULT_MAX_RA_INT;
  RADV_IFACE->min_delay = DEFAULT_MIN_DELAY;
  RADV_IFACE->prefix_linger_time = (u32) -1;
  RADV_IFACE->route_linger_time = (u32) -1;
  RADV_IFACE->current_hop_limit = DEFAULT_CURRENT_HOP_LIMIT;
  RADV_IFACE->default_lifetime = (u32) -1;
  RADV_IFACE->default_lifetime_sensitive = 1;
  RADV_IFACE->default_preference = RA_PREF_MEDIUM;
  RADV_IFACE->route_lifetime = (u32) -1;
  RADV_IFACE->route_lifetime_sensitive = 0;
  RADV_IFACE->route_preference = RA_PREF_MEDIUM;
}
#line 12555 "obj/conf/cf-parse.tab.c"
    break;

  case 1190:
#line 101 "proto/radv/config.Y"
                        { RADV_IFACE->min_ra_int = (yyvsp[0].i); if ((yyvsp[0].i) < 3) cf_error("Min RA interval must be at least 3"); }
#line 12561 "obj/conf/cf-parse.tab.c"
    break;

  case 1191:
#line 102 "proto/radv/config.Y"
                        { RADV_IFACE->max_ra_int = (yyvsp[0].i); if (((yyvsp[0].i) < 4) || ((yyvsp[0].i) > 1800)) cf_error("Max RA interval must be in range 4-1800"); }
#line 12567 "obj/conf/cf-parse.tab.c"
    break;

  case 1192:
#line 103 "proto/radv/config.Y"
                  { RADV_IFACE->min_delay = (yyvsp[0].i); if ((yyvsp[0].i) <= 0) cf_error("Min delay must be positive"); }
#line 12573 "obj/conf/cf-parse.tab.c"
    break;

  case 1193:
#line 104 "proto/radv/config.Y"
                             { RADV_IFACE->solicited_ra_unicast = (yyvsp[0].i); }
#line 12579 "obj/conf/cf-parse.tab.c"
    break;

  case 1194:
#line 105 "proto/radv/config.Y"
                { RADV_IFACE->managed = (yyvsp[0].i); }
#line 12585 "obj/conf/cf-parse.tab.c"
    break;

  case 1195:
#line 106 "proto/radv/config.Y"
                     { RADV_IFACE->other_config = (yyvsp[0].i); }
#line 12591 "obj/conf/cf-parse.tab.c"
    break;

  case 1196:
#line 107 "proto/radv/config.Y"
                 { RADV_IFACE->link_mtu = (yyvsp[0].i); }
#line 12597 "obj/conf/cf-parse.tab.c"
    break;

  case 1197:
#line 108 "proto/radv/config.Y"
                       { RADV_IFACE->reachable_time = (yyvsp[0].i); if ((yyvsp[0].i) > 3600000) cf_error("Reachable time must be in range 0-3600000"); }
#line 12603 "obj/conf/cf-parse.tab.c"
    break;

  case 1198:
#line 109 "proto/radv/config.Y"
                      { RADV_IFACE->retrans_timer = (yyvsp[0].i); }
#line 12609 "obj/conf/cf-parse.tab.c"
    break;

  case 1199:
#line 110 "proto/radv/config.Y"
                          { RADV_IFACE->current_hop_limit = (yyvsp[0].i); if ((yyvsp[0].i) > 255) cf_error("Current hop limit must be in range 0-255"); }
#line 12615 "obj/conf/cf-parse.tab.c"
    break;

  case 1200:
#line 111 "proto/radv/config.Y"
                                        {
     RADV_IFACE->default_lifetime = (yyvsp[-1].i);
     if ((yyvsp[-1].i) > 9000)  cf_error("Default lifetime must be in range 0-9000");
     if ((yyvsp[0].i) != (uint) -1) RADV_IFACE->default_lifetime_sensitive = (yyvsp[0].i);
   }
#line 12625 "obj/conf/cf-parse.tab.c"
    break;

  case 1201:
#line 116 "proto/radv/config.Y"
                                      {
     RADV_IFACE->route_lifetime = (yyvsp[-1].i);
     if ((yyvsp[0].i) != (uint) -1) RADV_IFACE->route_lifetime_sensitive = (yyvsp[0].i);
   }
#line 12634 "obj/conf/cf-parse.tab.c"
    break;

  case 1202:
#line 120 "proto/radv/config.Y"
                                      { RADV_IFACE->default_preference = (yyvsp[0].i); }
#line 12640 "obj/conf/cf-parse.tab.c"
    break;

  case 1203:
#line 121 "proto/radv/config.Y"
                                    { RADV_IFACE->route_preference = (yyvsp[0].i); }
#line 12646 "obj/conf/cf-parse.tab.c"
    break;

  case 1204:
#line 122 "proto/radv/config.Y"
                           { RADV_IFACE->prefix_linger_time = (yyvsp[0].i); }
#line 12652 "obj/conf/cf-parse.tab.c"
    break;

  case 1205:
#line 123 "proto/radv/config.Y"
                          { RADV_IFACE->route_linger_time = (yyvsp[0].i); }
#line 12658 "obj/conf/cf-parse.tab.c"
    break;

  case 1206:
#line 124 "proto/radv/config.Y"
                      { add_tail(&RADV_IFACE->pref_list, NODE this_radv_prefix); }
#line 12664 "obj/conf/cf-parse.tab.c"
    break;

  case 1207:
#line 125 "proto/radv/config.Y"
         { init_list(&radv_dns_list); }
#line 12670 "obj/conf/cf-parse.tab.c"
    break;

  case 1208:
#line 125 "proto/radv/config.Y"
                                                   { add_tail_list(&RADV_IFACE->rdnss_list, &radv_dns_list); }
#line 12676 "obj/conf/cf-parse.tab.c"
    break;

  case 1209:
#line 126 "proto/radv/config.Y"
         { init_list(&radv_dns_list); }
#line 12682 "obj/conf/cf-parse.tab.c"
    break;

  case 1210:
#line 126 "proto/radv/config.Y"
                                                   { add_tail_list(&RADV_IFACE->dnssl_list, &radv_dns_list); }
#line 12688 "obj/conf/cf-parse.tab.c"
    break;

  case 1211:
#line 127 "proto/radv/config.Y"
                    { RADV_IFACE->rdnss_local = (yyvsp[0].i); }
#line 12694 "obj/conf/cf-parse.tab.c"
    break;

  case 1212:
#line 128 "proto/radv/config.Y"
                    { RADV_IFACE->dnssl_local = (yyvsp[0].i); }
#line 12700 "obj/conf/cf-parse.tab.c"
    break;

  case 1213:
#line 132 "proto/radv/config.Y"
       { (yyval.i) = RA_PREF_LOW; }
#line 12706 "obj/conf/cf-parse.tab.c"
    break;

  case 1214:
#line 133 "proto/radv/config.Y"
          { (yyval.i) = RA_PREF_MEDIUM; }
#line 12712 "obj/conf/cf-parse.tab.c"
    break;

  case 1215:
#line 134 "proto/radv/config.Y"
        { (yyval.i) = RA_PREF_HIGH; }
#line 12718 "obj/conf/cf-parse.tab.c"
    break;

  case 1216:
#line 137 "proto/radv/config.Y"
{
  struct radv_iface_config *ic = RADV_IFACE;

  if (ic->min_ra_int == (u32) -1)
    ic->min_ra_int = MAX_(ic->max_ra_int / 3, 3);

  if (ic->default_lifetime == (u32) -1)
    ic->default_lifetime = 3 * ic->max_ra_int;

  if (ic->route_lifetime == (u32) -1)
    ic->route_lifetime = 3 * ic->max_ra_int;

  if (ic->prefix_linger_time == (u32) -1)
    ic->prefix_linger_time = 3 * ic->max_ra_int;

  if (ic->route_linger_time == (u32) -1)
    ic->route_linger_time = 3 * ic->max_ra_int;

  if ((ic->min_ra_int > 3) &&
      (ic->min_ra_int > (ic->max_ra_int * 3 / 4)))
    cf_error("Min RA interval must be at most 3/4 * Max RA interval");

  if ((ic->default_lifetime > 0) && (ic->default_lifetime < ic->max_ra_int))
    cf_error("Default lifetime must be either 0 or at least Max RA interval");

  if ((ic->route_lifetime > 0) && (ic->route_lifetime < ic->max_ra_int))
    cf_error("Route lifetime must be either 0 or at least Max RA interval");

  if ((ic->prefix_linger_time > 0) && (ic->prefix_linger_time < ic->max_ra_int))
    cf_error("Prefix linger time must be either 0 or at least Max RA interval");

  if ((ic->route_linger_time > 0) && (ic->route_linger_time < ic->max_ra_int))
    cf_error("Route linger time must be either 0 or at least Max RA interval");

  RADV_CFG->max_linger_time = MAX_(RADV_CFG->max_linger_time, ic->route_linger_time);
}
#line 12759 "obj/conf/cf-parse.tab.c"
    break;

  case 1222:
#line 190 "proto/radv/config.Y"
{
  this_radv_prefix = cfg_allocz(sizeof(struct radv_prefix_config));
  RADV_PREFIX->prefix = *(net_addr_ip6 *) &((yyvsp[0].net));

  RADV_PREFIX->onlink = 1;
  RADV_PREFIX->autonomous = 1;
  RADV_PREFIX->valid_lifetime = DEFAULT_VALID_LIFETIME;
  RADV_PREFIX->preferred_lifetime = DEFAULT_PREFERRED_LIFETIME;
}
#line 12773 "obj/conf/cf-parse.tab.c"
    break;

  case 1223:
#line 201 "proto/radv/config.Y"
             { RADV_PREFIX->skip = (yyvsp[0].i); }
#line 12779 "obj/conf/cf-parse.tab.c"
    break;

  case 1224:
#line 202 "proto/radv/config.Y"
               { RADV_PREFIX->onlink = (yyvsp[0].i); }
#line 12785 "obj/conf/cf-parse.tab.c"
    break;

  case 1225:
#line 203 "proto/radv/config.Y"
                   { RADV_PREFIX->autonomous = (yyvsp[0].i); }
#line 12791 "obj/conf/cf-parse.tab.c"
    break;

  case 1226:
#line 204 "proto/radv/config.Y"
                                      {
     RADV_PREFIX->valid_lifetime = (yyvsp[-1].i);
     if ((yyvsp[0].i) != (uint) -1) RADV_PREFIX->valid_lifetime_sensitive = (yyvsp[0].i);
   }
#line 12800 "obj/conf/cf-parse.tab.c"
    break;

  case 1227:
#line 208 "proto/radv/config.Y"
                                          {
     RADV_PREFIX->preferred_lifetime = (yyvsp[-1].i);
     if ((yyvsp[0].i) != (uint) -1) RADV_PREFIX->preferred_lifetime_sensitive = (yyvsp[0].i);
   }
#line 12809 "obj/conf/cf-parse.tab.c"
    break;

  case 1228:
#line 215 "proto/radv/config.Y"
{
  if (RADV_PREFIX->preferred_lifetime > RADV_PREFIX->valid_lifetime)
    cf_error("Preferred lifetime must be at most Valid lifetime");

  if (RADV_PREFIX->valid_lifetime_sensitive > RADV_PREFIX->preferred_lifetime_sensitive)
    cf_error("Valid lifetime sensitive requires that Preferred lifetime is sensitive too");
}
#line 12821 "obj/conf/cf-parse.tab.c"
    break;

  case 1234:
#line 239 "proto/radv/config.Y"
{
  struct radv_rdnss_config *cf = cfg_allocz(sizeof(struct radv_rdnss_config));
  add_tail(&radv_dns_list, NODE cf);

  cf->server = (yyvsp[0].a);
  cf->lifetime_mult = DEFAULT_DNS_LIFETIME_MULT;
}
#line 12833 "obj/conf/cf-parse.tab.c"
    break;

  case 1235:
#line 248 "proto/radv/config.Y"
{
  RADV_RDNSS->lifetime = 0;
  RADV_RDNSS->lifetime_mult = DEFAULT_DNS_LIFETIME_MULT;
}
#line 12842 "obj/conf/cf-parse.tab.c"
    break;

  case 1238:
#line 255 "proto/radv/config.Y"
                      { RADV_RDNSS->lifetime = (yyvsp[0].i); RADV_RDNSS->lifetime_mult = radv_mult_val; }
#line 12848 "obj/conf/cf-parse.tab.c"
    break;

  case 1239:
#line 259 "proto/radv/config.Y"
{
  if (EMPTY_LIST(radv_dns_list))
    cf_error("No nameserver in RDNSS section");

  struct radv_rdnss_config *cf;
  WALK_LIST(cf, radv_dns_list)
  {
    cf->lifetime = RADV_RDNSS->lifetime;
    cf->lifetime_mult = RADV_RDNSS->lifetime_mult;
  }
}
#line 12864 "obj/conf/cf-parse.tab.c"
    break;

  case 1244:
#line 283 "proto/radv/config.Y"
{
  struct radv_dnssl_config *cf = cfg_allocz(sizeof(struct radv_dnssl_config));
  add_tail(&radv_dns_list, NODE cf);

  cf->domain = (yyvsp[0].t);
  cf->lifetime_mult = DEFAULT_DNS_LIFETIME_MULT;

  if (radv_process_domain(cf) < 0)
    cf_error("Invalid domain dame");
}
#line 12879 "obj/conf/cf-parse.tab.c"
    break;

  case 1245:
#line 295 "proto/radv/config.Y"
{
  RADV_DNSSL->lifetime = 0;
  RADV_DNSSL->lifetime_mult = DEFAULT_DNS_LIFETIME_MULT;
}
#line 12888 "obj/conf/cf-parse.tab.c"
    break;

  case 1248:
#line 302 "proto/radv/config.Y"
                      { RADV_DNSSL->lifetime = (yyvsp[0].i); RADV_DNSSL->lifetime_mult = radv_mult_val; }
#line 12894 "obj/conf/cf-parse.tab.c"
    break;

  case 1249:
#line 306 "proto/radv/config.Y"
{
  if (EMPTY_LIST(radv_dns_list))
    cf_error("No domain in DNSSL section");

  struct radv_dnssl_config *cf;
  WALK_LIST(cf, radv_dns_list)
  {
    cf->lifetime = RADV_DNSSL->lifetime;
    cf->lifetime_mult = RADV_DNSSL->lifetime_mult;
  }
}
#line 12910 "obj/conf/cf-parse.tab.c"
    break;

  case 1254:
#line 330 "proto/radv/config.Y"
        { (yyval.i) = (yyvsp[0].i); radv_mult_val = 0; }
#line 12916 "obj/conf/cf-parse.tab.c"
    break;

  case 1255:
#line 331 "proto/radv/config.Y"
             { (yyval.i) = 0; radv_mult_val = (yyvsp[0].i); if (((yyvsp[0].i) < 1) || ((yyvsp[0].i) > 254)) cf_error("Multiplier must be in range 1-254"); }
#line 12922 "obj/conf/cf-parse.tab.c"
    break;

  case 1256:
#line 335 "proto/radv/config.Y"
               { (yyval.i) = (uint) -1; }
#line 12928 "obj/conf/cf-parse.tab.c"
    break;

  case 1257:
#line 336 "proto/radv/config.Y"
                  { (yyval.i) = (yyvsp[0].i); }
#line 12934 "obj/conf/cf-parse.tab.c"
    break;

  case 1258:
#line 339 "proto/radv/config.Y"
                            { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_ENUM_RA_PREFERENCE, EA_RA_PREFERENCE); }
#line 12940 "obj/conf/cf-parse.tab.c"
    break;

  case 1259:
#line 340 "proto/radv/config.Y"
                          { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_RA_LIFETIME); }
#line 12946 "obj/conf/cf-parse.tab.c"
    break;

  case 1261:
#line 49 "proto/rip/config.Y"
          { (yyval.i) = 1; }
#line 12952 "obj/conf/cf-parse.tab.c"
    break;

  case 1262:
#line 50 "proto/rip/config.Y"
          { (yyval.i) = 0; }
#line 12958 "obj/conf/cf-parse.tab.c"
    break;

  case 1263:
#line 54 "proto/rip/config.Y"
{
  this_proto = proto_config_new(&proto_rip, (yyvsp[-1].i));
  this_proto->net_type = (yyvsp[0].i) ? NET_IP4 : NET_IP6;

  init_list(&RIP_CFG->patt_list);
  RIP_CFG->rip2 = (yyvsp[0].i);
  RIP_CFG->ecmp = rt_default_ecmp;
  RIP_CFG->infinity = RIP_DEFAULT_INFINITY;
  RIP_CFG->min_timeout_time = 60 S_;
  RIP_CFG->max_garbage_time = 60 S_;
}
#line 12974 "obj/conf/cf-parse.tab.c"
    break;

  case 1266:
#line 69 "proto/rip/config.Y"
                        { RIP_CFG->ecmp = (yyvsp[0].i) ? RIP_DEFAULT_ECMP_LIMIT : 0; }
#line 12980 "obj/conf/cf-parse.tab.c"
    break;

  case 1267:
#line 70 "proto/rip/config.Y"
                        { RIP_CFG->ecmp = (yyvsp[-2].i) ? (yyvsp[0].i) : 0; }
#line 12986 "obj/conf/cf-parse.tab.c"
    break;

  case 1268:
#line 71 "proto/rip/config.Y"
                        { RIP_CFG->infinity = (yyvsp[0].i); }
#line 12992 "obj/conf/cf-parse.tab.c"
    break;

  case 1273:
#line 85 "proto/rip/config.Y"
{
  this_ipatt = cfg_allocz(sizeof(struct rip_iface_config));
  add_tail(&RIP_CFG->patt_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);
  reset_passwords();

  RIP_IFACE->metric = 1;
  RIP_IFACE->port = rip_cfg_is_v2() ? RIP_PORT : RIP_NG_PORT;
  RIP_IFACE->version = rip_cfg_is_v2() ? RIP_V2 : RIP_V1;
  RIP_IFACE->split_horizon = 1;
  RIP_IFACE->poison_reverse = 1;
  RIP_IFACE->check_zero = 1;
  RIP_IFACE->check_link = 1;
  RIP_IFACE->ttl_security = rip_cfg_is_v2() ? 0 : 1;
  RIP_IFACE->rx_buffer = rip_cfg_is_v2() ? RIP_MAX_PKT_LENGTH : 0;
  RIP_IFACE->tx_length = rip_cfg_is_v2() ? RIP_MAX_PKT_LENGTH : 0;
  RIP_IFACE->tx_tos = IP_PREC_INTERNET_CONTROL;
  RIP_IFACE->tx_priority = sk_priority_control;
  RIP_IFACE->update_time = RIP_DEFAULT_UPDATE_TIME;
  RIP_IFACE->timeout_time = RIP_DEFAULT_TIMEOUT_TIME;
  RIP_IFACE->garbage_time = RIP_DEFAULT_GARBAGE_TIME;
  RIP_IFACE->rxmt_time = RIP_DEFAULT_RXMT_TIME;
}
#line 13020 "obj/conf/cf-parse.tab.c"
    break;

  case 1274:
#line 110 "proto/rip/config.Y"
{
  /* Default mode is broadcast for RIPv1, multicast for RIPv2 and RIPng */
  if (!RIP_IFACE->mode)
    RIP_IFACE->mode = (rip_cfg_is_v2() && (RIP_IFACE->version == RIP_V1)) ?
      RIP_IM_BROADCAST : RIP_IM_MULTICAST;

  RIP_IFACE->passwords = get_passwords();

  if (!RIP_IFACE->auth_type != !RIP_IFACE->passwords)
    log(L_WARN "Authentication and password options should be used together");

  if (RIP_IFACE->passwords)
  {
    struct password_item *pass;
    WALK_LIST(pass, *RIP_IFACE->passwords)
    {
      if (pass->alg && (RIP_IFACE->auth_type != RIP_AUTH_CRYPTO))
	cf_error("Password algorithm option requires cryptographic authentication");

      /* Set default crypto algorithm (MD5) */
      if (!pass->alg && (RIP_IFACE->auth_type == RIP_AUTH_CRYPTO))
	pass->alg = ALG_MD5;
    }
  }

  RIP_CFG->min_timeout_time = MIN_(RIP_CFG->min_timeout_time, RIP_IFACE->timeout_time);
  RIP_CFG->max_garbage_time = MAX_(RIP_CFG->max_garbage_time, RIP_IFACE->garbage_time);
}
#line 13053 "obj/conf/cf-parse.tab.c"
    break;

  case 1275:
#line 140 "proto/rip/config.Y"
                        { RIP_IFACE->metric = (yyvsp[0].i); if (((yyvsp[0].i)<1) || ((yyvsp[0].i)>255)) cf_error("Metric must be in range 1-255"); }
#line 13059 "obj/conf/cf-parse.tab.c"
    break;

  case 1276:
#line 141 "proto/rip/config.Y"
                        { RIP_IFACE->mode = RIP_IM_MULTICAST; }
#line 13065 "obj/conf/cf-parse.tab.c"
    break;

  case 1277:
#line 142 "proto/rip/config.Y"
                        { RIP_IFACE->mode = RIP_IM_BROADCAST; if (rip_cfg_is_ng()) cf_error("Broadcast not supported in RIPng"); }
#line 13071 "obj/conf/cf-parse.tab.c"
    break;

  case 1278:
#line 143 "proto/rip/config.Y"
                        { RIP_IFACE->passive = (yyvsp[0].i); }
#line 13077 "obj/conf/cf-parse.tab.c"
    break;

  case 1279:
#line 144 "proto/rip/config.Y"
                        { RIP_IFACE->address = (yyvsp[0].a); if (ipa_is_ip4((yyvsp[0].a)) != rip_cfg_is_v2()) cf_error("IP address version mismatch"); }
#line 13083 "obj/conf/cf-parse.tab.c"
    break;

  case 1280:
#line 145 "proto/rip/config.Y"
                        { RIP_IFACE->port = (yyvsp[0].i); if (((yyvsp[0].i)<1) || ((yyvsp[0].i)>65535)) cf_error("Invalid port number"); }
#line 13089 "obj/conf/cf-parse.tab.c"
    break;

  case 1281:
#line 146 "proto/rip/config.Y"
                        { RIP_IFACE->version = (yyvsp[0].i);
			  if (rip_cfg_is_ng()) cf_error("Version not supported in RIPng");
			  if (((yyvsp[0].i) != RIP_V1) && ((yyvsp[0].i) != RIP_V2)) cf_error("Unsupported version");
			}
#line 13098 "obj/conf/cf-parse.tab.c"
    break;

  case 1282:
#line 150 "proto/rip/config.Y"
                        { RIP_IFACE->version_only = (yyvsp[0].i); }
#line 13104 "obj/conf/cf-parse.tab.c"
    break;

  case 1283:
#line 151 "proto/rip/config.Y"
                        { RIP_IFACE->split_horizon = (yyvsp[0].i); }
#line 13110 "obj/conf/cf-parse.tab.c"
    break;

  case 1284:
#line 152 "proto/rip/config.Y"
                        { RIP_IFACE->poison_reverse = (yyvsp[0].i); }
#line 13116 "obj/conf/cf-parse.tab.c"
    break;

  case 1285:
#line 153 "proto/rip/config.Y"
                        { RIP_IFACE->check_zero = (yyvsp[0].i); }
#line 13122 "obj/conf/cf-parse.tab.c"
    break;

  case 1286:
#line 154 "proto/rip/config.Y"
                        { RIP_IFACE->demand_circuit = (yyvsp[0].i); }
#line 13128 "obj/conf/cf-parse.tab.c"
    break;

  case 1287:
#line 155 "proto/rip/config.Y"
                        { RIP_IFACE->update_time = (yyvsp[0].i) S_; if ((yyvsp[0].i)<=0) cf_error("Update time must be positive"); }
#line 13134 "obj/conf/cf-parse.tab.c"
    break;

  case 1288:
#line 156 "proto/rip/config.Y"
                        { RIP_IFACE->timeout_time = (yyvsp[0].i) S_; if ((yyvsp[0].i)<=0) cf_error("Timeout time must be positive"); }
#line 13140 "obj/conf/cf-parse.tab.c"
    break;

  case 1289:
#line 157 "proto/rip/config.Y"
                        { RIP_IFACE->garbage_time = (yyvsp[0].i) S_; if ((yyvsp[0].i)<=0) cf_error("Garbage time must be positive"); }
#line 13146 "obj/conf/cf-parse.tab.c"
    break;

  case 1290:
#line 158 "proto/rip/config.Y"
                           { RIP_IFACE->rxmt_time = (yyvsp[0].time); if ((yyvsp[0].time)<=0) cf_error("Retransmit time must be positive"); }
#line 13152 "obj/conf/cf-parse.tab.c"
    break;

  case 1291:
#line 159 "proto/rip/config.Y"
                        { RIP_IFACE->ecmp_weight = (yyvsp[0].i) - 1; if (((yyvsp[0].i)<1) || ((yyvsp[0].i)>256)) cf_error("ECMP weight must be in range 1-256"); }
#line 13158 "obj/conf/cf-parse.tab.c"
    break;

  case 1292:
#line 160 "proto/rip/config.Y"
                        { RIP_IFACE->rx_buffer = (yyvsp[0].i); if (((yyvsp[0].i)<256) || ((yyvsp[0].i)>65535)) cf_error("RX length must be in range 256-65535"); }
#line 13164 "obj/conf/cf-parse.tab.c"
    break;

  case 1293:
#line 161 "proto/rip/config.Y"
                        { RIP_IFACE->tx_length = (yyvsp[0].i); if (((yyvsp[0].i)<256) || ((yyvsp[0].i)>65535)) cf_error("TX length must be in range 256-65535"); }
#line 13170 "obj/conf/cf-parse.tab.c"
    break;

  case 1294:
#line 162 "proto/rip/config.Y"
                        { RIP_IFACE->tx_tos = (yyvsp[0].i); }
#line 13176 "obj/conf/cf-parse.tab.c"
    break;

  case 1295:
#line 163 "proto/rip/config.Y"
                        { RIP_IFACE->tx_priority = (yyvsp[0].i); }
#line 13182 "obj/conf/cf-parse.tab.c"
    break;

  case 1296:
#line 164 "proto/rip/config.Y"
                        { RIP_IFACE->ttl_security = (yyvsp[0].i); }
#line 13188 "obj/conf/cf-parse.tab.c"
    break;

  case 1297:
#line 165 "proto/rip/config.Y"
                        { RIP_IFACE->ttl_security = 2; }
#line 13194 "obj/conf/cf-parse.tab.c"
    break;

  case 1298:
#line 166 "proto/rip/config.Y"
                        { RIP_IFACE->check_link = (yyvsp[0].i); }
#line 13200 "obj/conf/cf-parse.tab.c"
    break;

  case 1299:
#line 167 "proto/rip/config.Y"
                        { RIP_IFACE->bfd = (yyvsp[0].i); cf_check_bfd((yyvsp[0].i)); }
#line 13206 "obj/conf/cf-parse.tab.c"
    break;

  case 1300:
#line 168 "proto/rip/config.Y"
                           { RIP_IFACE->auth_type = (yyvsp[0].i); if ((yyvsp[0].i)) rip_check_auth(); }
#line 13212 "obj/conf/cf-parse.tab.c"
    break;

  case 1301:
#line 169 "proto/rip/config.Y"
                        { rip_check_auth(); }
#line 13218 "obj/conf/cf-parse.tab.c"
    break;

  case 1302:
#line 173 "proto/rip/config.Y"
                        { (yyval.i) = RIP_AUTH_NONE; }
#line 13224 "obj/conf/cf-parse.tab.c"
    break;

  case 1303:
#line 174 "proto/rip/config.Y"
                        { (yyval.i) = RIP_AUTH_PLAIN; }
#line 13230 "obj/conf/cf-parse.tab.c"
    break;

  case 1304:
#line 175 "proto/rip/config.Y"
                        { (yyval.i) = RIP_AUTH_CRYPTO; }
#line 13236 "obj/conf/cf-parse.tab.c"
    break;

  case 1305:
#line 176 "proto/rip/config.Y"
                        { (yyval.i) = RIP_AUTH_CRYPTO; }
#line 13242 "obj/conf/cf-parse.tab.c"
    break;

  case 1311:
#line 193 "proto/rip/config.Y"
                         { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_RIP_METRIC); }
#line 13248 "obj/conf/cf-parse.tab.c"
    break;

  case 1312:
#line 194 "proto/rip/config.Y"
                      { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_RIP_TAG); }
#line 13254 "obj/conf/cf-parse.tab.c"
    break;

  case 1314:
#line 199 "proto/rip/config.Y"
{ PROTO_WALK_CMD((yyvsp[-2].s), &proto_rip, p) rip_show_interfaces(p, (yyvsp[-1].t)); }
#line 13260 "obj/conf/cf-parse.tab.c"
    break;

  case 1316:
#line 202 "proto/rip/config.Y"
{ PROTO_WALK_CMD((yyvsp[-2].s), &proto_rip, p) rip_show_neighbors(p, (yyvsp[-1].t)); }
#line 13266 "obj/conf/cf-parse.tab.c"
    break;

  case 1318:
#line 43 "proto/rpki/config.Y"
                                   {
  this_proto = proto_config_new(&proto_rpki, (yyvsp[-1].i));
  RPKI_CFG->retry_interval = RPKI_RETRY_INTERVAL;
  RPKI_CFG->refresh_interval = RPKI_REFRESH_INTERVAL;
  RPKI_CFG->expire_interval = RPKI_EXPIRE_INTERVAL;
}
#line 13277 "obj/conf/cf-parse.tab.c"
    break;

  case 1319:
#line 50 "proto/rpki/config.Y"
                                                                { rpki_check_config(RPKI_CFG); }
#line 13283 "obj/conf/cf-parse.tab.c"
    break;

  case 1328:
#line 64 "proto/rpki/config.Y"
                                   {
     if (rpki_check_refresh_interval((yyvsp[0].i)))
       cf_error(rpki_check_refresh_interval((yyvsp[0].i)));
     RPKI_CFG->refresh_interval = (yyvsp[0].i);
     RPKI_CFG->keep_refresh_interval = (yyvsp[-1].i);
   }
#line 13294 "obj/conf/cf-parse.tab.c"
    break;

  case 1329:
#line 70 "proto/rpki/config.Y"
                                 {
     if (rpki_check_retry_interval((yyvsp[0].i)))
       cf_error(rpki_check_retry_interval((yyvsp[0].i)));
     RPKI_CFG->retry_interval = (yyvsp[0].i);
     RPKI_CFG->keep_retry_interval = (yyvsp[-1].i);
   }
#line 13305 "obj/conf/cf-parse.tab.c"
    break;

  case 1330:
#line 76 "proto/rpki/config.Y"
                                  {
     if (rpki_check_expire_interval((yyvsp[0].i)))
       cf_error(rpki_check_expire_interval((yyvsp[0].i)));
     RPKI_CFG->expire_interval = (yyvsp[0].i);
     RPKI_CFG->keep_expire_interval = (yyvsp[-1].i);
   }
#line 13316 "obj/conf/cf-parse.tab.c"
    break;

  case 1331:
#line 82 "proto/rpki/config.Y"
                          { RPKI_CFG->ignore_max_length = (yyvsp[0].i); }
#line 13322 "obj/conf/cf-parse.tab.c"
    break;

  case 1332:
#line 86 "proto/rpki/config.Y"
             { (yyval.i) = 0; }
#line 13328 "obj/conf/cf-parse.tab.c"
    break;

  case 1333:
#line 87 "proto/rpki/config.Y"
        { (yyval.i) = 1; }
#line 13334 "obj/conf/cf-parse.tab.c"
    break;

  case 1334:
#line 90 "proto/rpki/config.Y"
                                { check_u16((yyvsp[0].i)); RPKI_CFG->port = (yyvsp[0].i); }
#line 13340 "obj/conf/cf-parse.tab.c"
    break;

  case 1335:
#line 93 "proto/rpki/config.Y"
        {
     rpki_check_unused_hostname();
     RPKI_CFG->hostname = (yyvsp[0].t);
   }
#line 13349 "obj/conf/cf-parse.tab.c"
    break;

  case 1336:
#line 97 "proto/rpki/config.Y"
       {
     rpki_check_unused_hostname();
     RPKI_CFG->ip = (yyvsp[0].a);
     /* Ensure hostname is filled */
     char *hostname = cfg_allocz(INET6_ADDRSTRLEN + 1);
     bsnprintf(hostname, INET6_ADDRSTRLEN+1, "%I", RPKI_CFG->ip);
     RPKI_CFG->hostname = hostname;
   }
#line 13362 "obj/conf/cf-parse.tab.c"
    break;

  case 1339:
#line 113 "proto/rpki/config.Y"
{
  rpki_check_unused_transport();
  RPKI_CFG->tr_config.spec = cfg_allocz(sizeof(struct rpki_tr_tcp_config));
  RPKI_CFG->tr_config.type = RPKI_TR_TCP;
}
#line 13372 "obj/conf/cf-parse.tab.c"
    break;

  case 1340:
#line 120 "proto/rpki/config.Y"
{
#if HAVE_LIBSSH
  rpki_check_unused_transport();
  RPKI_CFG->tr_config.spec = cfg_allocz(sizeof(struct rpki_tr_ssh_config));
  RPKI_CFG->tr_config.type = RPKI_TR_SSH;
#else
  cf_error("This build doesn't support SSH");
#endif
}
#line 13386 "obj/conf/cf-parse.tab.c"
    break;

  case 1343:
#line 136 "proto/rpki/config.Y"
                          { RPKI_TR_SSH_CFG->bird_private_key = (yyvsp[0].t); }
#line 13392 "obj/conf/cf-parse.tab.c"
    break;

  case 1344:
#line 137 "proto/rpki/config.Y"
                          { RPKI_TR_SSH_CFG->cache_public_key = (yyvsp[0].t); }
#line 13398 "obj/conf/cf-parse.tab.c"
    break;

  case 1345:
#line 138 "proto/rpki/config.Y"
                          { RPKI_TR_SSH_CFG->user = (yyvsp[0].t); }
#line 13404 "obj/conf/cf-parse.tab.c"
    break;

  case 1346:
#line 142 "proto/rpki/config.Y"
{
  if (RPKI_TR_SSH_CFG->user == NULL)
    cf_error("User must be set");
}
#line 13413 "obj/conf/cf-parse.tab.c"
    break;

  case 1348:
#line 57 "proto/static/config.Y"
{
  this_proto = proto_config_new(&proto_static, (yyvsp[-1].i));
  init_list(&STATIC_CFG->routes);
}
#line 13422 "obj/conf/cf-parse.tab.c"
    break;

  case 1351:
#line 65 "proto/static/config.Y"
                                  { this_proto->net_type = (yyvsp[-1].cc)->net_type; }
#line 13428 "obj/conf/cf-parse.tab.c"
    break;

  case 1352:
#line 66 "proto/static/config.Y"
                                    { STATIC_CFG->check_link = (yyvsp[-1].i); }
#line 13434 "obj/conf/cf-parse.tab.c"
    break;

  case 1353:
#line 67 "proto/static/config.Y"
                                     {
    if ((yyvsp[-1].r)->addr_type == NET_IP4)
      STATIC_CFG->igp_table_ip4 = (yyvsp[-1].r);
    else if ((yyvsp[-1].r)->addr_type == NET_IP6)
      STATIC_CFG->igp_table_ip6 = (yyvsp[-1].r);
    else
      cf_error("Incompatible IGP table type");
   }
#line 13447 "obj/conf/cf-parse.tab.c"
    break;

  case 1354:
#line 75 "proto/static/config.Y"
                                                   { static_route_finish(); }
#line 13453 "obj/conf/cf-parse.tab.c"
    break;

  case 1355:
#line 79 "proto/static/config.Y"
                      {
      this_snh = static_nexthop_new();
      this_snh->via = (yyvsp[-1].a);
      this_snh->iface = (yyvsp[0].iface);
    }
#line 13463 "obj/conf/cf-parse.tab.c"
    break;

  case 1356:
#line 84 "proto/static/config.Y"
             {
      this_snh = static_nexthop_new();
      this_snh->via = IPA_NONE;
      this_snh->iface = if_get_by_name((yyvsp[0].t));
    }
#line 13473 "obj/conf/cf-parse.tab.c"
    break;

  case 1357:
#line 89 "proto/static/config.Y"
                                  {
    this_snh->mls = (yyvsp[0].mls);
  }
#line 13481 "obj/conf/cf-parse.tab.c"
    break;

  case 1358:
#line 92 "proto/static/config.Y"
                             {
    this_snh->onlink = (yyvsp[0].i);
  }
#line 13489 "obj/conf/cf-parse.tab.c"
    break;

  case 1359:
#line 95 "proto/static/config.Y"
                             {
    this_snh->weight = (yyvsp[0].i) - 1;
    if (((yyvsp[0].i)<1) || ((yyvsp[0].i)>256)) cf_error("Weight must be in range 1-256");
  }
#line 13498 "obj/conf/cf-parse.tab.c"
    break;

  case 1360:
#line 99 "proto/static/config.Y"
                          {
    this_snh->use_bfd = (yyvsp[0].i); cf_check_bfd((yyvsp[0].i));
  }
#line 13506 "obj/conf/cf-parse.tab.c"
    break;

  case 1363:
#line 109 "proto/static/config.Y"
                           {
     this_srt = cfg_allocz(sizeof(struct static_route));
     add_tail(&STATIC_CFG->routes, &this_srt->n);
     this_srt->net = (yyvsp[0].net_ptr);
     this_srt_cmds = NULL;
     this_srt_last_cmd = NULL;
     this_srt->mp_next = NULL;
     this_snh = NULL;
  }
#line 13520 "obj/conf/cf-parse.tab.c"
    break;

  case 1365:
#line 122 "proto/static/config.Y"
                             {
      this_srt->dest = RTDX_RECURSIVE;
      this_srt->via = (yyvsp[0].a);
   }
#line 13529 "obj/conf/cf-parse.tab.c"
    break;

  case 1366:
#line 126 "proto/static/config.Y"
                                              {
      this_srt->dest = RTDX_RECURSIVE;
      this_srt->via = (yyvsp[-2].a);
      this_srt->mls = (yyvsp[0].mls);
   }
#line 13539 "obj/conf/cf-parse.tab.c"
    break;

  case 1367:
#line 131 "proto/static/config.Y"
                                { this_srt->dest = RTD_NONE; }
#line 13545 "obj/conf/cf-parse.tab.c"
    break;

  case 1368:
#line 132 "proto/static/config.Y"
                                { this_srt->dest = RTD_BLACKHOLE; }
#line 13551 "obj/conf/cf-parse.tab.c"
    break;

  case 1369:
#line 133 "proto/static/config.Y"
                                { this_srt->dest = RTD_UNREACHABLE; }
#line 13557 "obj/conf/cf-parse.tab.c"
    break;

  case 1370:
#line 134 "proto/static/config.Y"
                                { this_srt->dest = RTD_BLACKHOLE; }
#line 13563 "obj/conf/cf-parse.tab.c"
    break;

  case 1371:
#line 135 "proto/static/config.Y"
                                { this_srt->dest = RTD_UNREACHABLE; }
#line 13569 "obj/conf/cf-parse.tab.c"
    break;

  case 1372:
#line 136 "proto/static/config.Y"
                                { this_srt->dest = RTD_PROHIBIT; }
#line 13575 "obj/conf/cf-parse.tab.c"
    break;

  case 1373:
#line 140 "proto/static/config.Y"
       {
     if (this_srt_last_cmd)
       this_srt_last_cmd->next = (yyvsp[0].x);
     else
       this_srt_cmds = (yyvsp[0].x);
     this_srt_last_cmd = (yyvsp[0].x);
   }
#line 13587 "obj/conf/cf-parse.tab.c"
    break;

  case 1379:
#line 161 "proto/static/config.Y"
{ PROTO_WALK_CMD((yyvsp[-1].s), &proto_static, p) static_show(p); }
#line 13593 "obj/conf/cf-parse.tab.c"
    break;

  case 1381:
#line 26 "sysdep/linux/netlink.Y"
                     { THIS_KRT->sys.table_id = (yyvsp[0].i); }
#line 13599 "obj/conf/cf-parse.tab.c"
    break;

  case 1382:
#line 27 "sysdep/linux/netlink.Y"
               { THIS_KRT->sys.metric = (yyvsp[0].i); }
#line 13605 "obj/conf/cf-parse.tab.c"
    break;

  case 1383:
#line 28 "sysdep/linux/netlink.Y"
                          { THIS_KRT->sys.netlink_rx_buffer = (yyvsp[0].i); }
#line 13611 "obj/conf/cf-parse.tab.c"
    break;

  case 1384:
#line 31 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_IP_ADDRESS, T_IP, EA_KRT_PREFSRC); }
#line 13617 "obj/conf/cf-parse.tab.c"
    break;

  case 1385:
#line 32 "sysdep/linux/netlink.Y"
                        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_REALM); }
#line 13623 "obj/conf/cf-parse.tab.c"
    break;

  case 1386:
#line 33 "sysdep/linux/netlink.Y"
                        { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_SCOPE); }
#line 13629 "obj/conf/cf-parse.tab.c"
    break;

  case 1387:
#line 35 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_MTU); }
#line 13635 "obj/conf/cf-parse.tab.c"
    break;

  case 1388:
#line 36 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_WINDOW); }
#line 13641 "obj/conf/cf-parse.tab.c"
    break;

  case 1389:
#line 37 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_RTT); }
#line 13647 "obj/conf/cf-parse.tab.c"
    break;

  case 1390:
#line 38 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_RTTVAR); }
#line 13653 "obj/conf/cf-parse.tab.c"
    break;

  case 1391:
#line 39 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_SSTRESH); }
#line 13659 "obj/conf/cf-parse.tab.c"
    break;

  case 1392:
#line 40 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_CWND); }
#line 13665 "obj/conf/cf-parse.tab.c"
    break;

  case 1393:
#line 41 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_ADVMSS); }
#line 13671 "obj/conf/cf-parse.tab.c"
    break;

  case 1394:
#line 42 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_REORDERING); }
#line 13677 "obj/conf/cf-parse.tab.c"
    break;

  case 1395:
#line 43 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_HOPLIMIT); }
#line 13683 "obj/conf/cf-parse.tab.c"
    break;

  case 1396:
#line 44 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_INITCWND); }
#line 13689 "obj/conf/cf-parse.tab.c"
    break;

  case 1397:
#line 45 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_RTO_MIN); }
#line 13695 "obj/conf/cf-parse.tab.c"
    break;

  case 1398:
#line 46 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_INITRWND); }
#line 13701 "obj/conf/cf-parse.tab.c"
    break;

  case 1399:
#line 47 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_QUICKACK); }
#line 13707 "obj/conf/cf-parse.tab.c"
    break;

  case 1400:
#line 51 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr_bit(2, T_BOOL, EA_KRT_LOCK); }
#line 13713 "obj/conf/cf-parse.tab.c"
    break;

  case 1401:
#line 52 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr_bit(3, T_BOOL, EA_KRT_LOCK); }
#line 13719 "obj/conf/cf-parse.tab.c"
    break;

  case 1402:
#line 53 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr_bit(4, T_BOOL, EA_KRT_LOCK); }
#line 13725 "obj/conf/cf-parse.tab.c"
    break;

  case 1403:
#line 54 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr_bit(5, T_BOOL, EA_KRT_LOCK); }
#line 13731 "obj/conf/cf-parse.tab.c"
    break;

  case 1404:
#line 55 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr_bit(6, T_BOOL, EA_KRT_LOCK); }
#line 13737 "obj/conf/cf-parse.tab.c"
    break;

  case 1405:
#line 56 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr_bit(7, T_BOOL, EA_KRT_LOCK); }
#line 13743 "obj/conf/cf-parse.tab.c"
    break;

  case 1406:
#line 57 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr_bit(8, T_BOOL, EA_KRT_LOCK); }
#line 13749 "obj/conf/cf-parse.tab.c"
    break;

  case 1407:
#line 58 "sysdep/linux/netlink.Y"
                                  { (yyval.fda) = f_new_dynamic_attr_bit(9, T_BOOL, EA_KRT_LOCK); }
#line 13755 "obj/conf/cf-parse.tab.c"
    break;

  case 1408:
#line 59 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr_bit(10, T_BOOL, EA_KRT_LOCK); }
#line 13761 "obj/conf/cf-parse.tab.c"
    break;

  case 1409:
#line 60 "sysdep/linux/netlink.Y"
                               { (yyval.fda) = f_new_dynamic_attr_bit(13, T_BOOL, EA_KRT_LOCK); }
#line 13767 "obj/conf/cf-parse.tab.c"
    break;

  case 1410:
#line 62 "sysdep/linux/netlink.Y"
                                { (yyval.fda) = f_new_dynamic_attr_bit(0, T_BOOL, EA_KRT_FEATURES); }
#line 13773 "obj/conf/cf-parse.tab.c"
    break;

  case 1411:
#line 63 "sysdep/linux/netlink.Y"
                                  { (yyval.fda) = f_new_dynamic_attr(3, T_BOOL, EA_KRT_FEATURES); }
#line 13779 "obj/conf/cf-parse.tab.c"
    break;

  case 1413:
#line 33 "sysdep/unix/config.Y"
           { this_log = cfg_allocz(sizeof(struct log_config)); }
#line 13785 "obj/conf/cf-parse.tab.c"
    break;

  case 1414:
#line 35 "sysdep/unix/config.Y"
                                                {
    this_log->mask = (yyvsp[-1].i);
    add_tail(&new_config->logfiles, &this_log->n);
  }
#line 13794 "obj/conf/cf-parse.tab.c"
    break;

  case 1415:
#line 42 "sysdep/unix/config.Y"
             { (yyval.t) = (yyvsp[0].t); }
#line 13800 "obj/conf/cf-parse.tab.c"
    break;

  case 1416:
#line 43 "sysdep/unix/config.Y"
   { (yyval.t) = bird_name; }
#line 13806 "obj/conf/cf-parse.tab.c"
    break;

  case 1418:
#line 48 "sysdep/unix/config.Y"
             { this_log->limit = (yyvsp[-1].i); this_log->backup = (yyvsp[0].t); }
#line 13812 "obj/conf/cf-parse.tab.c"
    break;

  case 1419:
#line 52 "sysdep/unix/config.Y"
                  {
     if (!parse_and_exit)
     {
       this_log->rf = rf_open(new_config->pool, (yyvsp[-1].t), "a");
       if (!this_log->rf) cf_error("Unable to open log file '%s': %m", (yyvsp[-1].t));
       this_log->fh = rf_file(this_log->rf);
     }
     this_log->pos = -1;
     this_log->filename = (yyvsp[-1].t);
   }
#line 13827 "obj/conf/cf-parse.tab.c"
    break;

  case 1420:
#line 62 "sysdep/unix/config.Y"
                      { this_log->fh = NULL; new_config->syslog_name = (yyvsp[0].t); }
#line 13833 "obj/conf/cf-parse.tab.c"
    break;

  case 1421:
#line 63 "sysdep/unix/config.Y"
          { this_log->fh = stderr; }
#line 13839 "obj/conf/cf-parse.tab.c"
    break;

  case 1422:
#line 67 "sysdep/unix/config.Y"
       { (yyval.i) = ~0; }
#line 13845 "obj/conf/cf-parse.tab.c"
    break;

  case 1423:
#line 68 "sysdep/unix/config.Y"
                         { (yyval.i) = (yyvsp[-1].i); }
#line 13851 "obj/conf/cf-parse.tab.c"
    break;

  case 1424:
#line 72 "sysdep/unix/config.Y"
           { (yyval.i) = 1 << (yyvsp[0].i); }
#line 13857 "obj/conf/cf-parse.tab.c"
    break;

  case 1425:
#line 73 "sysdep/unix/config.Y"
                             { (yyval.i) = (yyvsp[-2].i) | (1 << (yyvsp[0].i)); }
#line 13863 "obj/conf/cf-parse.tab.c"
    break;

  case 1426:
#line 77 "sysdep/unix/config.Y"
         { (yyval.i) = L_DEBUG[0]; }
#line 13869 "obj/conf/cf-parse.tab.c"
    break;

  case 1427:
#line 78 "sysdep/unix/config.Y"
         { (yyval.i) = L_TRACE[0]; }
#line 13875 "obj/conf/cf-parse.tab.c"
    break;

  case 1428:
#line 79 "sysdep/unix/config.Y"
        { (yyval.i) = L_INFO[0]; }
#line 13881 "obj/conf/cf-parse.tab.c"
    break;

  case 1429:
#line 80 "sysdep/unix/config.Y"
          { (yyval.i) = L_REMOTE[0]; }
#line 13887 "obj/conf/cf-parse.tab.c"
    break;

  case 1430:
#line 81 "sysdep/unix/config.Y"
           { (yyval.i) = L_WARN[0]; }
#line 13893 "obj/conf/cf-parse.tab.c"
    break;

  case 1431:
#line 82 "sysdep/unix/config.Y"
         { (yyval.i) = L_ERR[0]; }
#line 13899 "obj/conf/cf-parse.tab.c"
    break;

  case 1432:
#line 83 "sysdep/unix/config.Y"
        { (yyval.i) = L_AUTH[0]; }
#line 13905 "obj/conf/cf-parse.tab.c"
    break;

  case 1433:
#line 84 "sysdep/unix/config.Y"
         { (yyval.i) = L_FATAL[0]; }
#line 13911 "obj/conf/cf-parse.tab.c"
    break;

  case 1434:
#line 85 "sysdep/unix/config.Y"
       { (yyval.i) = L_BUG[0]; }
#line 13917 "obj/conf/cf-parse.tab.c"
    break;

  case 1436:
#line 92 "sysdep/unix/config.Y"
                                      { new_config->proto_default_mrtdump = (yyvsp[-1].i); }
#line 13923 "obj/conf/cf-parse.tab.c"
    break;

  case 1437:
#line 93 "sysdep/unix/config.Y"
                    {
     if (!parse_and_exit)
     {
       struct rfile *f = rf_open(new_config->pool, (yyvsp[-1].t), "a");
       if (!f) cf_error("Unable to open MRTDump file '%s': %m", (yyvsp[-1].t));
       new_config->mrtdump_file = rf_fileno(f);
     }
   }
#line 13936 "obj/conf/cf-parse.tab.c"
    break;

  case 1439:
#line 107 "sysdep/unix/config.Y"
                      { new_config->latency_debug = (yyvsp[0].i); }
#line 13942 "obj/conf/cf-parse.tab.c"
    break;

  case 1440:
#line 108 "sysdep/unix/config.Y"
                               { new_config->latency_limit = (yyvsp[0].time); }
#line 13948 "obj/conf/cf-parse.tab.c"
    break;

  case 1441:
#line 109 "sysdep/unix/config.Y"
                            { new_config->watchdog_warning = (yyvsp[0].time); }
#line 13954 "obj/conf/cf-parse.tab.c"
    break;

  case 1442:
#line 110 "sysdep/unix/config.Y"
                            { new_config->watchdog_timeout = ((yyvsp[0].time) + 999999) TO_S; }
#line 13960 "obj/conf/cf-parse.tab.c"
    break;

  case 1444:
#line 119 "sysdep/unix/config.Y"
{ cmd_reconfig((yyvsp[-2].t), RECONFIG_HARD, (yyvsp[-1].i)); }
#line 13966 "obj/conf/cf-parse.tab.c"
    break;

  case 1446:
#line 122 "sysdep/unix/config.Y"
{ cmd_reconfig((yyvsp[-2].t), RECONFIG_SOFT, (yyvsp[-1].i)); }
#line 13972 "obj/conf/cf-parse.tab.c"
    break;

  case 1448:
#line 129 "sysdep/unix/config.Y"
{ cmd_reconfig_confirm(); }
#line 13978 "obj/conf/cf-parse.tab.c"
    break;

  case 1450:
#line 132 "sysdep/unix/config.Y"
{ cmd_reconfig_undo(); }
#line 13984 "obj/conf/cf-parse.tab.c"
    break;

  case 1452:
#line 135 "sysdep/unix/config.Y"
{ cmd_reconfig_status(); }
#line 13990 "obj/conf/cf-parse.tab.c"
    break;

  case 1454:
#line 138 "sysdep/unix/config.Y"
{ cmd_check_config((yyvsp[-1].t)); }
#line 13996 "obj/conf/cf-parse.tab.c"
    break;

  case 1456:
#line 141 "sysdep/unix/config.Y"
{ cmd_shutdown(); }
#line 14002 "obj/conf/cf-parse.tab.c"
    break;

  case 1458:
#line 146 "sysdep/unix/config.Y"
{ cmd_graceful_restart(); }
#line 14008 "obj/conf/cf-parse.tab.c"
    break;

  case 1459:
#line 150 "sysdep/unix/config.Y"
               { (yyval.t) = NULL; }
#line 14014 "obj/conf/cf-parse.tab.c"
    break;

  case 1461:
#line 155 "sysdep/unix/config.Y"
               { (yyval.i) = 0; }
#line 14020 "obj/conf/cf-parse.tab.c"
    break;

  case 1462:
#line 156 "sysdep/unix/config.Y"
           { (yyval.i) = UNIX_DEFAULT_CONFIGURE_TIMEOUT; }
#line 14026 "obj/conf/cf-parse.tab.c"
    break;

  case 1463:
#line 157 "sysdep/unix/config.Y"
                { (yyval.i) = (yyvsp[0].i); }
#line 14032 "obj/conf/cf-parse.tab.c"
    break;

  case 1465:
#line 43 "sysdep/unix/krt.Y"
                                     {
     this_proto = krt_init_config((yyvsp[-1].i));
}
#line 14040 "obj/conf/cf-parse.tab.c"
    break;

  case 1468:
#line 52 "sysdep/unix/krt.Y"
               { (yyval.i) = KRT_DEFAULT_ECMP_LIMIT; }
#line 14046 "obj/conf/cf-parse.tab.c"
    break;

  case 1469:
#line 53 "sysdep/unix/krt.Y"
               { (yyval.i) = (yyvsp[0].i); if (((yyvsp[0].i) <= 0) || ((yyvsp[0].i) > 255)) cf_error("Merge paths limit must be in range 1-255"); }
#line 14052 "obj/conf/cf-parse.tab.c"
    break;

  case 1471:
#line 58 "sysdep/unix/krt.Y"
                 { this_proto->net_type = (yyvsp[0].cc)->net_type; }
#line 14058 "obj/conf/cf-parse.tab.c"
    break;

  case 1472:
#line 59 "sysdep/unix/krt.Y"
                { THIS_KRT->persist = (yyvsp[0].i); }
#line 14064 "obj/conf/cf-parse.tab.c"
    break;

  case 1473:
#line 60 "sysdep/unix/krt.Y"
                  {
      /* Scan time of 0 means scan on startup only */
      THIS_KRT->scan_time = (yyvsp[0].i) S_;
   }
#line 14073 "obj/conf/cf-parse.tab.c"
    break;

  case 1474:
#line 64 "sysdep/unix/krt.Y"
              {
      THIS_KRT->learn = (yyvsp[0].i);
#ifndef KRT_ALLOW_LEARN
      if ((yyvsp[0].i))
	cf_error("Learning of kernel routes not supported on this platform");
#endif
   }
#line 14085 "obj/conf/cf-parse.tab.c"
    break;

  case 1475:
#line 71 "sysdep/unix/krt.Y"
                         { THIS_KRT->graceful_restart = (yyvsp[0].i); }
#line 14091 "obj/conf/cf-parse.tab.c"
    break;

  case 1476:
#line 72 "sysdep/unix/krt.Y"
                                  {
      THIS_KRT->merge_paths = (yyvsp[-1].i) ? (yyvsp[0].i) : 0;
#ifndef KRT_ALLOW_MERGE_PATHS
      if ((yyvsp[-1].i))
	cf_error("Path merging not supported on this platform");
#endif
   }
#line 14103 "obj/conf/cf-parse.tab.c"
    break;

  case 1478:
#line 85 "sysdep/unix/krt.Y"
                                    { this_proto = kif_init_config((yyvsp[-1].i)); }
#line 14109 "obj/conf/cf-parse.tab.c"
    break;

  case 1483:
#line 94 "sysdep/unix/krt.Y"
                  {
      /* Scan time of 0 means scan on startup only */
      THIS_KIF->scan_time = (yyvsp[0].i) S_;
   }
#line 14118 "obj/conf/cf-parse.tab.c"
    break;

  case 1484:
#line 101 "sysdep/unix/krt.Y"
{
  this_ipatt = cfg_allocz(sizeof(struct kif_iface_config));
  add_tail(&THIS_KIF->iface_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);
}
#line 14128 "obj/conf/cf-parse.tab.c"
    break;

  case 1485:
#line 108 "sysdep/unix/krt.Y"
                 { kif_set_preferred((yyvsp[0].a)); }
#line 14134 "obj/conf/cf-parse.tab.c"
    break;

  case 1491:
#line 125 "sysdep/unix/krt.Y"
                         { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_SOURCE); }
#line 14140 "obj/conf/cf-parse.tab.c"
    break;

  case 1492:
#line 126 "sysdep/unix/krt.Y"
                         { (yyval.fda) = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_METRIC); }
#line 14146 "obj/conf/cf-parse.tab.c"
    break;


#line 14150 "obj/conf/cf-parse.tab.c"

        default: break;
      }
    if (yychar_backup != yychar)
      YY_LAC_DISCARD ("yychar change");
  }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyesa, &yyes, &yyes_capacity, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        if (yychar != YYEMPTY)
          YY_LAC_ESTABLISH;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  /* If the stack popping above didn't lose the initial context for the
     current lookahead token, the shift below will for sure.  */
  YY_LAC_DISCARD ("error recovery");

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if 1
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yyes != yyesa)
    YYSTACK_FREE (yyes);
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 50 "conf/gen_parser.m4"

#line 397 "conf/confbase.Y"
/* C Code from conf/confbase.Y */

#line 206 "conf/flowspec.Y"
/* C Code from conf/flowspec.Y */

#line 929 "nest/config.Y"
/* C Code from nest/config.Y */

#line 189 "proto/bfd/config.Y"
/* C Code from proto/bfd/config.Y */

#line 185 "proto/babel/config.Y"
/* C Code from proto/babel/config.Y */

#line 372 "proto/bgp/config.Y"
/* C Code from proto/bgp/config.Y */

#line 66 "proto/mrt/config.Y"
/* C Code from proto/mrt/config.Y */

#line 562 "proto/ospf/config.Y"
/* C Code from proto/ospf/config.Y */

#line 59 "proto/perf/config.Y"
/* C Code from proto/perf/config.Y */

#line 46 "proto/pipe/config.Y"
/* C Code from proto/pipe/config.Y */

#line 342 "proto/radv/config.Y"
/* C Code from proto/radv/config.Y */

#line 205 "proto/rip/config.Y"
/* C Code from proto/rip/config.Y */

#line 147 "proto/rpki/config.Y"
/* C Code from proto/rpki/config.Y */

#line 163 "proto/static/config.Y"
/* C Code from proto/static/config.Y */

#line 66 "sysdep/linux/netlink.Y"
/* C Code from sysdep/linux/netlink.Y */

#line 160 "sysdep/unix/config.Y"
/* C Code from sysdep/unix/config.Y */

#line 128 "sysdep/unix/krt.Y"
/* C Code from sysdep/unix/krt.Y */

