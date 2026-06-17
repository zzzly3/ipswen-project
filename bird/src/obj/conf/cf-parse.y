#line 50 "conf/gen_parser.m4"
%{
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
%}
#line 50

#line 52 "conf/confbase.Y"
/* Declarations from conf/confbase.Y */

%union {
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
}

%token END CLI_MARKER INVALID_TOKEN ELSECOL DDOT
%token GEQ LEQ NEQ AND OR
%token PO PC
%token <i> NUM ENUM
%token <ip4> IP4
%token <ip6> IP6
%token <i64> VPN_RD
%token <s> CF_SYM_KNOWN CF_SYM_UNDEFINED
%token <t> TEXT
%token <bs> BYTESTRING
%type <iface> ipa_scope

%type <i> expr bool pxlen4
%type <time> expr_us time
%type <a> ipa
%type <net> net_ip4_ net_ip4 net_ip6_ net_ip6 net_ip_ net_ip net_or_ipa
%type <net_ptr> net_ net_any net_vpn4_ net_vpn6_ net_vpn_ net_roa4_ net_roa6_ net_roa_ net_ip6_sadr_ net_mpls_
%type <mls> label_stack_start label_stack

%type <t> text opttext
%type <s> symbol

%nonassoc PREFIX_DUMMY
%left AND OR
%nonassoc '=' '<' '>' '~' GEQ LEQ NEQ NMA PO PC
%left '+' '-'
%left '*' '/' '%'
%left '!'
%nonassoc '.'

%start config

%token<s> DEFINE ON OFF YES NO S MS US PORT VPN MPLS FROM

#line 22 "conf/flowspec.Y"
/* Declarations from conf/flowspec.Y */

%type <i32> flow_num_op flow_srcdst flow_logic_op flow_num_type_ flow_frag_val flow_neg
%type <net_ptr> net_flow4_ net_flow6_ net_flow_

%token<s> FLOW4 FLOW6 DST SRC PROTO NEXT HEADER DPORT SPORT ICMP TYPE CODE TCP FLAGS LENGTH DSCP DONT_FRAGMENT IS_FRAGMENT FIRST_FRAGMENT LAST_FRAGMENT FRAGMENT LABEL OFFSET
#line 30


#line 9 "client/cmds.Y"
%token<s> QUIT
#line 10 "client/cmds.Y"
%token<s> EXIT
#line 11 "client/cmds.Y"
%token<s> HELP
#line 299 "filter/config.Y"
/* Declarations from filter/config.Y */

%token<s> FUNCTION PRINT PRINTN UNSET RETURN ACCEPT REJECT ERROR INT BOOL IP PREFIX RD PAIR QUAD EC LC SET STRING BGPMASK BGPPATH CLIST ECLIST LCLIST IF THEN ELSE CASE FOR IN DO TRUE FALSE RT RO UNKNOWN GENERIC GW NET MASK SOURCE SCOPE DEST IFNAME IFINDEX WEIGHT GW_MPLS ONLINK PREFERENCE ROA_CHECK ASN IS_V4 IS_V6 LEN MAXLEN DATA DATA1 DATA2 DEFINED ADD DELETE CONTAINS RESET PREPEND FIRST LAST LAST_NONAGGREGATED MATCH MIN MAX EMPTY FILTER WHERE EVAL ATTRIBUTE BT_ASSERT BT_TEST_SUITE BT_CHECK_ASSIGN BT_TEST_SAME FORMAT
#line 321

%nonassoc THEN
%nonassoc ELSE

%type <xp> cmds_int cmd_prep
%type <x> term cmd cmd_var cmds cmds_scoped constant constructor print_list var var_init var_list function_call symbol_value bgp_path_expr bgp_path bgp_path_tail
%type <fda> dynamic_attr
%type <fsa> static_attr
%type <f> filter where_filter
%type <fl> filter_body function_body
%type <flv> lvalue
%type <i> type function_vars
%type <fa> function_argsn function_args
%type <ecs> ec_kind
%type <fret> break_command
%type <i32> cnum
%type <e> pair_item ec_item lc_item set_item switch_item set_items switch_items switch_body
%type <trie> fprefix_set
%type <v> set_atom switch_atom fipa
%type <px> fprefix
%type <t> get_cf_position
%type <s> for_var

#line 113 "nest/config.Y"
/* Declarations from nest/config.Y */

%token<s> ROUTER ID HOSTNAME PROTOCOL TEMPLATE DISABLED DEBUG ALL DIRECT
%token<s> INTERFACE IMPORT EXPORT NONE VRF DEFAULT TABLE STATES ROUTES FILTERS
%token<s> IPV4 IPV6 VPN4 VPN6 ROA4 ROA6 SADR
%token<s> RECEIVE LIMIT ACTION WARN BLOCK RESTART DISABLE KEEP FILTERED RPKI
%token<s> PASSWORD KEY PASSIVE TO EVENTS PACKETS PROTOCOLS CHANNELS INTERFACES
%token<s> ALGORITHM KEYED HMAC MD5 SHA1 SHA256 SHA384 SHA512 BLAKE2S128 BLAKE2S256 BLAKE2B256 BLAKE2B512
%token<s> PRIMARY STATS COUNT BY COMMANDS PREEXPORT NOEXPORT EXPORTED GENERATE
%token<s> BGP PASSWORDS DESCRIPTION
%token<s> RELOAD OUT MRTDUMP MESSAGES RESTRICT MEMORY IGP_METRIC CLASS
%token<s> TIMEFORMAT ISO SHORT LONG ROUTE BASE LOG
%token<s> GRACEFUL WAIT FLUSH AS
%token<s> IDLE RX TX INTERVAL MULTIPLIER
%token<s> CHECK LINK
%token<s> SORTED TRIE SETTLE TIME GC THRESHOLD PERIOD

/* For r_args_channel */
%token<s> IPV4_MC IPV4_MPLS IPV6_MC IPV6_MPLS IPV6_SADR VPN4_MC VPN4_MPLS VPN6_MC VPN6_MPLS PRI SEC

#line 134






%type <i32> idval
%type <f> imexport
%type <r> rtable
%type <s> optproto
%type <ra> r_args
%type <sd> sym_args
%type <i> proto_start echo_mask echo_size debug_mask debug_list debug_flag mrtdump_mask mrtdump_list mrtdump_flag export_mode limit_action net_type tos password_algorithm
%type <ps> proto_patt proto_patt2
%type <cc> channel_start proto_channel
%type <cl> limit_spec
%type <net> r_args_for_val
%type <net_ptr> r_args_for
%type <t> channel_sym
%type <c> channel_arg

#line 611 "nest/config.Y"
%token<s> SHOW STATUS
#line 614 "nest/config.Y"
#line 617 "nest/config.Y"
#line 620 "nest/config.Y"
#line 628 "nest/config.Y"
#line 631 "nest/config.Y"
%token<s> SUMMARY
#line 635 "nest/config.Y"
#line 826 "nest/config.Y"
%token<s> SYMBOLS
#line 843 "nest/config.Y"
%token<s> DUMP RESOURCES
#line 845 "nest/config.Y"
%token<s> SOCKETS
#line 847 "nest/config.Y"
#line 849 "nest/config.Y"
#line 851 "nest/config.Y"
%token<s> NEIGHBORS
#line 853 "nest/config.Y"
%token<s> ATTRIBUTES
#line 855 "nest/config.Y"
#line 857 "nest/config.Y"
#line 859 "nest/config.Y"
#line 866 "nest/config.Y"
%token<s> ECHO
#line 887 "nest/config.Y"
%token<s> ENABLE
#line 893 "nest/config.Y"
#line 895 "nest/config.Y"
#line 22 "proto/bfd/config.Y"
/* Declarations from proto/bfd/config.Y */

%token<s> BFD MULTIHOP NEIGHBOR DEV LOCAL AUTHENTICATION SIMPLE METICULOUS STRICT BIND
#line 28

%type <iface> bfd_neigh_iface
%type <a> bfd_neigh_local
%type <i> bfd_neigh_multihop bfd_auth_type

#line 186 "proto/bfd/config.Y"
%token<s> SESSIONS
#line 23 "proto/babel/config.Y"
/* Declarations from proto/babel/config.Y */

%token<s> BABEL METRIC RXCOST HELLO UPDATE WIRED WIRELESS BUFFER PRIORITY HOP BABEL_METRIC ENTRIES RANDOMIZE MAC PERMISSIVE EXTENDED
#line 30

#line 173 "proto/babel/config.Y"
#line 176 "proto/babel/config.Y"
#line 179 "proto/babel/config.Y"
#line 182 "proto/babel/config.Y"
#line 18 "proto/bgp/config.Y"
/* Declarations from proto/bgp/config.Y */

%token<s> HOLD CONNECT RETRY KEEPALIVE STARTUP VIA SELF PATH START DELAY FORGET AFTER BGP_PATH BGP_LOCAL_PREF BGP_MED BGP_ORIGIN BGP_NEXT_HOP BGP_ATOMIC_AGGR BGP_AGGREGATOR BGP_COMMUNITY BGP_EXT_COMMUNITY BGP_LARGE_COMMUNITY ADDRESS RR RS CLIENT CLUSTER AS4 ADVERTISE CAPABILITIES PREFER OLDER MISSING LLADDR DROP IGNORE REFRESH INTERPRET COMMUNITIES BGP_ORIGINATOR_ID BGP_CLUSTER_LIST IGP GATEWAY RECURSIVE MED TTL SECURITY DETERMINISTIC SECONDARY ALLOW PATHS AWARE SETKEY CONFEDERATION MEMBER MULTICAST LIVED STALE IBGP EBGP MANDATORY INTERNAL EXTERNAL SETS DYNAMIC RANGE NAME DIGITS BGP_AIGP AIGP ORIGINATE COST ENFORCE FREE VALIDATE ROLE ROLES PEER PROVIDER CUSTOMER RS_SERVER RS_CLIENT REQUIRE BGP_OTC GLOBAL
#line 36

%type <i> bgp_nh
%type <i32> bgp_afi

%token<s> CEASE HIT ADMINISTRATIVE SHUTDOWN CONFIGURATION CHANGE DECONFIGURED CONNECTION REJECTED COLLISION OF
#line 43

%type<i> bgp_cease_mask bgp_cease_list bgp_cease_flag bgp_role_name

#line 18 "proto/mrt/config.Y"
/* Declarations from proto/mrt/config.Y */

%token<s> MRT FILENAME ALWAYS

%type <md> mrt_dump_args

#line 53 "proto/mrt/config.Y"
#line 191 "proto/ospf/config.Y"
/* Declarations from proto/ospf/config.Y */

%token<s> OSPF V2 V3 OSPF_METRIC1 OSPF_METRIC2 OSPF_TAG OSPF_ROUTER_ID
%token<s> AREA RFC1583COMPAT STUB TICK COST2 RETRANSMIT
%token<s> TRANSMIT DEAD BROADCAST BCAST
%token<s> NONBROADCAST NBMA POINTOPOINT PTP POINTOMULTIPOINT PTMP
%token<s> CRYPTOGRAPHIC
%token<s> ELIGIBLE POLL NETWORKS HIDDEN VIRTUAL ONLY
%token<s> LARGE NORMAL STUBNET TAG
%token<s> LSADB ECMP NSSA TRANSLATOR STABILITY
%token<s> LSID INSTANCE REAL NETMASK
%token<s> MERGE LSA SUPPRESSION RFC5838 PE

#line 205
%type <ld> lsadb_args
%type <i> ospf_variant ospf_af_mc nbma_eligible
%type <cc> ospf_channel_start ospf_channel

#line 514 "proto/ospf/config.Y"
#line 517 "proto/ospf/config.Y"
#line 520 "proto/ospf/config.Y"
#line 525 "proto/ospf/config.Y"
%token<s> TOPOLOGY
#line 528 "proto/ospf/config.Y"
#line 533 "proto/ospf/config.Y"
%token<s> STATE
#line 536 "proto/ospf/config.Y"
#line 540 "proto/ospf/config.Y"
#line 18 "proto/perf/config.Y"
/* Declarations from proto/perf/config.Y */

%token<s> PERF EXP REPEAT MODE

#line 17 "proto/pipe/config.Y"
/* Declarations from proto/pipe/config.Y */

%token<s> PIPE

#line 29 "proto/radv/config.Y"
/* Declarations from proto/radv/config.Y */

%token<s> RADV RA SOLICITED UNICAST MANAGED OTHER CONFIG LINGER MTU REACHABLE RETRANS TIMER CURRENT VALID PREFERRED MULT LIFETIME SKIP AUTONOMOUS RDNSS DNSSL NS DOMAIN TRIGGER SENSITIVE LOW MEDIUM HIGH PROPAGATE RA_PREFERENCE RA_LIFETIME
#line 37



%type<i> radv_mult radv_sensitive radv_preference

#line 33 "proto/rip/config.Y"
/* Declarations from proto/rip/config.Y */

%token<s> RIP NG INFINITY TIMEOUT GARBAGE VERSION SPLIT HORIZON POISON REVERSE ZERO PLAINTEXT DEMAND CIRCUIT RIP_METRIC RIP_TAG
#line 41

%type <i> rip_variant rip_auth

#line 198 "proto/rip/config.Y"
#line 201 "proto/rip/config.Y"
#line 32 "proto/rpki/config.Y"
/* Declarations from proto/rpki/config.Y */

%token<s> REMOTE BIRD PRIVATE PUBLIC SSH TRANSPORT USER EXPIRE
#line 36

%type <i> rpki_keep_interval

#line 46 "proto/static/config.Y"
/* Declarations from proto/static/config.Y */

%token<s> STATIC PROHIBIT
%token<s> BLACKHOLE UNREACHABLE


#line 160 "proto/static/config.Y"
#line 11 "sysdep/linux/netlink.Y"
/* Declarations from sysdep/linux/netlink.Y */

%token<s> KERNEL NETLINK KRT_PREFSRC KRT_REALM KRT_SCOPE KRT_MTU KRT_WINDOW KRT_RTT KRT_RTTVAR KRT_SSTRESH KRT_CWND KRT_ADVMSS KRT_REORDERING KRT_HOPLIMIT KRT_INITCWND KRT_RTO_MIN KRT_INITRWND KRT_QUICKACK KRT_LOCK_MTU KRT_LOCK_WINDOW KRT_LOCK_RTT KRT_LOCK_RTTVAR KRT_LOCK_SSTRESH KRT_LOCK_CWND KRT_LOCK_ADVMSS KRT_LOCK_REORDERING KRT_LOCK_HOPLIMIT KRT_LOCK_RTO_MIN KRT_FEATURE_ECN KRT_FEATURE_ALLFRAG
#line 20

#line 18 "sysdep/unix/config.Y"
/* Declarations from sysdep/unix/config.Y */

%token<s> SYSLOG TRACE INFO WARNING AUTH FATAL BUG STDERR SOFT
%token<s> CONFIRM UNDO LATENCY WATCHDOG

#line 24
%type <i> log_mask log_mask_list log_cat cfg_timeout
%type <t> cfg_name
%type <tf> timeformat_which
%type <t> syslog_name

#line 118 "sysdep/unix/config.Y"
%token<s> CONFIGURE
#line 121 "sysdep/unix/config.Y"
#line 128 "sysdep/unix/config.Y"
#line 131 "sysdep/unix/config.Y"
#line 134 "sysdep/unix/config.Y"
#line 137 "sysdep/unix/config.Y"
#line 140 "sysdep/unix/config.Y"
%token<s> DOWN
#line 145 "sysdep/unix/config.Y"
#line 30 "sysdep/unix/krt.Y"
/* Declarations from sysdep/unix/krt.Y */

%token<s> PERSIST SCAN LEARN DEVICE KRT_SOURCE KRT_METRIC

#line 35
%type <i> kern_mp_limit

#line 50 "conf/gen_parser.m4"

#line 50
%%
#line 133 "conf/confbase.Y"
/* Grammar from conf/confbase.Y */

/* Basic config file structure */

config: conf_entries END { return 0; }
 | CLI_MARKER cli_cmd { return 0; }
 ;

conf_entries:
   /* EMPTY */
 | conf_entries conf
 ;

conf: ';' ;


/* Constant expressions */

conf: definition ;

definition:
   DEFINE symbol '=' term ';' {
     struct f_val *val = cfg_allocz(sizeof(struct f_val));
     if (f_eval(f_linearize($4, 1), cfg_mem, val) > F_RETURN) cf_error("Runtime error");
     cf_define_symbol($2, SYM_CONSTANT | val->type, val, val);
   }
 ;

expr:
   NUM
 | '(' term ')' { $$ = f_eval_int(f_linearize($2, 1)); }
 | CF_SYM_KNOWN {
     if ($1->class != (SYM_CONSTANT | T_INT)) cf_error("Number constant expected");
     $$ = SYM_VAL($1).i; }
 ;

expr_us:
   expr S  { $$ = $1 S_; }
 | expr MS { $$ = $1 MS_; }
 | expr US { $$ = $1 US_; }
 ;

symbol: CF_SYM_UNDEFINED | CF_SYM_KNOWN ;

/* Switches */

bool:
   expr { $$ = !!$1; }
 | ON { $$ = 1; }
 | YES { $$ = 1; }
 | OFF { $$ = 0; }
 | NO { $$ = 0; }
 | /* Silence means agreement */ { $$ = 1; }
 ;


/* Addresses */

ipa:
   IP4 { $$ = ipa_from_ip4($1); }
 | IP6 { $$ = ipa_from_ip6($1); }
 | CF_SYM_KNOWN {
     if ($1->class != (SYM_CONSTANT | T_IP)) cf_error("IP address constant expected");
     $$ = SYM_VAL($1).ip;
   }
 ;

ipa_scope:
   /* empty */ { $$ = NULL; }
 | '%' symbol { $$ = if_get_by_name($2->name); }
 ;


/* Networks - internal */

pxlen4:
   '/' NUM {
     if ($2 > IP6_MAX_PREFIX_LENGTH) cf_error("Invalid prefix length %u", $2); // FIXME
     $$ = $2;
   }
 ;

net_ip4_: IP4 pxlen4
{
  net_fill_ip4(&($$), $1, $2);

  net_addr_ip4 *n = (void *) &($$);
  if (!net_validate_ip4(n))
    cf_error("Invalid IPv4 prefix %I4/%d, maybe you wanted %I4/%d",
	     n->prefix, n->pxlen, ip4_and(n->prefix, ip4_mkmask(n->pxlen)), n->pxlen);
};

net_ip6_: IP6 '/' NUM
{
  if ($3 > IP6_MAX_PREFIX_LENGTH)
    cf_error("Invalid prefix length %u", $3);

  net_fill_ip6(&($$), $1, $3);

  net_addr_ip6 *n = (void *) &($$);
  if (!net_validate_ip6(n))
    cf_error("Invalid IPv6 prefix %I6/%d, maybe you wanted %I6/%d",
	     n->prefix, n->pxlen, ip6_and(n->prefix, ip6_mkmask(n->pxlen)), n->pxlen);
};

net_ip6_sadr_: IP6 '/' NUM FROM IP6 '/' NUM
{
  if ($3 > IP6_MAX_PREFIX_LENGTH)
    cf_error("Invalid prefix length %u", $3);

  if ($7 > IP6_MAX_PREFIX_LENGTH)
    cf_error("Invalid prefix length %u", $7);

  $$ = cfg_alloc(sizeof(net_addr_ip6_sadr));
  net_fill_ip6_sadr($$, $1, $3, $5, $7);

  net_addr_ip6_sadr *n = (void *) $$;
  if (!net_validate_ip6_sadr(n))
    cf_error("Invalid SADR IPv6 prefix %I6/%d from %I6/%d, maybe you wanted %I6/%d from %I6/%d",
	     n->dst_prefix, n->dst_pxlen, n->src_prefix, n->src_pxlen,
	     ip6_and(n->dst_prefix, ip6_mkmask(n->dst_pxlen)), n->dst_pxlen,
	     ip6_and(n->src_prefix, ip6_mkmask(n->src_pxlen)), n->src_pxlen);
};

net_vpn4_: VPN_RD net_ip4_
{
  $$ = cfg_alloc(sizeof(net_addr_vpn4));
  net_fill_vpn4($$, net4_prefix(&$2), net4_pxlen(&$2), $1);
}

net_vpn6_: VPN_RD net_ip6_
{
  $$ = cfg_alloc(sizeof(net_addr_vpn6));
  net_fill_vpn6($$, net6_prefix(&$2), net6_pxlen(&$2), $1);
}

net_roa4_: net_ip4_ MAX NUM AS NUM
{
  $$ = cfg_alloc(sizeof(net_addr_roa4));
  net_fill_roa4($$, net4_prefix(&$1), net4_pxlen(&$1), $3, $5);
  if ($3 < net4_pxlen(&$1) || $3 > IP4_MAX_PREFIX_LENGTH)
    cf_error("Invalid max prefix length %u", $3);
};

net_roa6_: net_ip6_ MAX NUM AS NUM
{
  $$ = cfg_alloc(sizeof(net_addr_roa6));
  net_fill_roa6($$, net6_prefix(&$1), net6_pxlen(&$1), $3, $5);
  if ($3 < net6_pxlen(&$1) || $3 > IP6_MAX_PREFIX_LENGTH)
    cf_error("Invalid max prefix length %u", $3);
};

net_mpls_: MPLS NUM
{
  $$ = cfg_alloc(sizeof(net_addr_roa6));
  net_fill_mpls($$, $2);
}

net_ip_: net_ip4_ | net_ip6_ ;
net_vpn_: net_vpn4_ | net_vpn6_ ;
net_roa_: net_roa4_ | net_roa6_ ;

net_:
   net_ip_ { $$ = cfg_alloc($1.length); net_copy($$, &($1)); }
 | net_vpn_
 | net_roa_
 | net_flow_
 | net_ip6_sadr_
 | net_mpls_
 ;


/* Networks - regular */

net_ip4:
   net_ip4_
 | CF_SYM_KNOWN {
     if (($1->class != (SYM_CONSTANT | T_NET)) || (SYM_VAL($1).net->type != NET_IP4))
       cf_error("IPv4 network constant expected");
     $$ = * SYM_VAL($1).net;
   }
 ;

net_ip6:
   net_ip6_
 | CF_SYM_KNOWN {
     if (($1->class != (SYM_CONSTANT | T_NET)) || (SYM_VAL($1).net->type != NET_IP6))
       cf_error("IPv6 network constant expected");
     $$ = * SYM_VAL($1).net;
   }
 ;

net_ip:
   net_ip_
 | CF_SYM_KNOWN {
     if (($1->class != (SYM_CONSTANT | T_NET)) || !net_is_ip(SYM_VAL($1).net))
       cf_error("IP network constant expected");
     $$ = * SYM_VAL($1).net;
   }
 ;

net_any:
   net_
 | CF_SYM_KNOWN {
     if ($1->class != (SYM_CONSTANT | T_NET))
       cf_error("Network constant expected");
     $$ = (net_addr *) SYM_VAL($1).net; /* Avoid const warning */
   }
 ;

net_or_ipa:
   net_ip4_
 | net_ip6_
 | IP4 { net_fill_ip4(&($$), $1, IP4_MAX_PREFIX_LENGTH); }
 | IP6 { net_fill_ip6(&($$), $1, IP6_MAX_PREFIX_LENGTH); }
 | CF_SYM_KNOWN {
     if ($1->class == (SYM_CONSTANT | T_IP))
       net_fill_ip_host(&($$), SYM_VAL($1).ip);
     else if (($1->class == (SYM_CONSTANT | T_NET)) && net_is_ip(SYM_VAL($1).net))
       $$ = * SYM_VAL($1).net;
     else
       cf_error("IP address or network constant expected");
   }
 ;

label_stack_start: NUM
{
  $$ = cfg_allocz(sizeof(mpls_label_stack));
  $$->len = 1;
  $$->stack[0] = $1;
};

label_stack:
    label_stack_start
  | label_stack '/' NUM {
    if ($1->len >= MPLS_MAX_LABEL_STACK)
      cf_error("Too many labels in stack");
    $1->stack[$1->len++] = $3;
    $$ = $1;
  }
;

time:
   TEXT {
     $$ = tm_parse_time($1);
     if (!$$)
       cf_error("Invalid date/time");
   }
 ;

text:
   TEXT
 | CF_SYM_KNOWN {
     if ($1->class != (SYM_CONSTANT | T_STRING)) cf_error("String constant expected");
     $$ = SYM_VAL($1).s;
   }
 ;

opttext:
    TEXT
 | /* empty */ { $$ = NULL; }
 ;


#line 32 "conf/flowspec.Y"
/* Grammar from conf/flowspec.Y */

/* Network Flow Specification */

flow_num_op:
   TRUE		{ $$ = FLOW_OP_TRUE; }
 | '='		{ $$ = FLOW_OP_EQ;  }
 | NEQ		{ $$ = FLOW_OP_NEQ; }
 | '<'		{ $$ = FLOW_OP_LT;  }
 | LEQ		{ $$ = FLOW_OP_LEQ; }
 | '>'		{ $$ = FLOW_OP_GT;  }
 | GEQ		{ $$ = FLOW_OP_GEQ; }
 | FALSE	{ $$ = FLOW_OP_FALSE; }
 ;

flow_logic_op:
   OR		{ $$ = FLOW_OP_OR; }
 | AND		{ $$ = FLOW_OP_AND; }
 ;

flow_num_type_:
   PROTO	{ $$ = FLOW_TYPE_IP_PROTOCOL; }
 | NEXT HEADER  { $$ = FLOW_TYPE_NEXT_HEADER; }
 | PORT		{ $$ = FLOW_TYPE_PORT; }
 | DPORT	{ $$ = FLOW_TYPE_DST_PORT; }
 | SPORT	{ $$ = FLOW_TYPE_SRC_PORT; }
 | ICMP TYPE	{ $$ = FLOW_TYPE_ICMP_TYPE; }
 | ICMP CODE	{ $$ = FLOW_TYPE_ICMP_CODE; }
 | LENGTH	{ $$ = FLOW_TYPE_PACKET_LENGTH; }
 | DSCP		{ $$ = FLOW_TYPE_DSCP; }
 | LABEL	{ $$ = FLOW_TYPE_LABEL; }
 ;

flow_num_type: flow_num_type_{ flow_builder_set_type(this_flow, $1); };
flow_flag_type: TCP FLAGS    { flow_builder_set_type(this_flow, FLOW_TYPE_TCP_FLAGS); };
flow_frag_type: FRAGMENT     { flow_builder_set_type(this_flow, FLOW_TYPE_FRAGMENT); };

flow_srcdst:
   DST		{ $$ = FLOW_TYPE_DST_PREFIX; }
 | SRC		{ $$ = FLOW_TYPE_SRC_PREFIX; }
 ;

flow_num_opts:
   flow_num_op expr {
     flow_check_cf_value_length(this_flow, $2);
     flow_builder_add_op_val(this_flow, $1, $2);
   }
 | flow_num_opts flow_logic_op flow_num_op expr {
     flow_check_cf_value_length(this_flow, $4);
     flow_builder_add_op_val(this_flow, $2 | $3, $4);
   }
 | flow_num_opt_ext
 | flow_num_opts OR flow_num_opt_ext
 ;

flow_num_opt_ext_expr:
   expr {
     flow_check_cf_value_length(this_flow, $1);
     flow_builder_add_op_val(this_flow, FLOW_OP_EQ, $1);
   }
 | expr DDOT expr {
     flow_check_cf_value_length(this_flow, $1);
     flow_check_cf_value_length(this_flow, $3);
     flow_builder_add_op_val(this_flow, FLOW_OP_GEQ, $1);
     flow_builder_add_op_val(this_flow, FLOW_OP_AND | FLOW_OP_LEQ, $3);
   }
 ;

flow_num_opt_ext:
   flow_num_opt_ext_expr
 | flow_num_opt_ext ',' flow_num_opt_ext_expr
 ;

flow_bmk_opts:
   flow_neg expr '/' expr {
     flow_check_cf_bmk_values(this_flow, $1, $2, $4);
     flow_builder_add_val_mask(this_flow, $1, $2, $4);
   }
 | flow_bmk_opts flow_logic_op flow_neg expr '/' expr {
     flow_check_cf_bmk_values(this_flow, $3, $4, $6);
     flow_builder_add_val_mask(this_flow, $2 | $3, $4, $6);
   }
 | flow_bmk_opts ',' flow_neg expr '/' expr {
     flow_check_cf_bmk_values(this_flow, $3, $4, $6);
     flow_builder_add_val_mask(this_flow, 0x40 | $3, $4, $6); /* AND */
   }
 ;

flow_neg:
   /* empty */ 	{ $$ = 0x00; }
 | '!'		{ $$ = 0x02; }
 ;

flow_frag_val:
   DONT_FRAGMENT  { $$ = 1; }
 | IS_FRAGMENT	  { $$ = 2; }
 | FIRST_FRAGMENT { $$ = 4; }
 | LAST_FRAGMENT  { $$ = 8; }
 ;

flow_frag_opts:
   flow_neg flow_frag_val {
     flow_builder_add_val_mask(this_flow, 0, ($1 ? 0 : $2), $2);
   }
 | flow_frag_opts flow_logic_op flow_neg flow_frag_val {
     flow_builder_add_val_mask(this_flow, $2, ($3 ? 0 : $4), $4);
   }
 | flow_frag_opts ',' flow_neg flow_frag_val {
     flow_builder_add_val_mask(this_flow, 0x40, ($3 ? 0 : $4), $4); /* AND */
   }
 ;

flow4_item:
   flow_srcdst net_ip4 {
     flow_builder_set_type(this_flow, $1);
     flow_builder4_add_pfx(this_flow, (net_addr_ip4 *) &($2));
   }
 | flow_num_type flow_num_opts
 | flow_flag_type flow_bmk_opts
 | flow_frag_type flow_frag_opts
 ;

flow6_item:
   flow_srcdst net_ip6 {
     flow_builder_set_type(this_flow, $1);
     flow_builder6_add_pfx(this_flow, (net_addr_ip6 *) &($2), 0);
   }
 | flow_srcdst net_ip6 OFFSET NUM {
     if ($4 > $2.pxlen)
       cf_error("Prefix offset is higher than prefix length");
     flow_builder_set_type(this_flow, $1);
     flow_builder6_add_pfx(this_flow, (net_addr_ip6 *) &($2), $4);
   }
 | flow_num_type flow_num_opts
 | flow_flag_type flow_bmk_opts
 | flow_frag_type flow_frag_opts
 ;

flow4_opts:
   /* empty */
 | flow4_opts flow4_item ';'
 ;

flow6_opts:
 /* empty */
 | flow6_opts flow6_item ';'
 ;

flow_builder_init:
{
  if (this_flow == NULL)
    this_flow = flow_builder_init(config_pool);	  /* FIXME: This should be allocated from tmp in future */
  else
    flow_builder_clear(this_flow);
};

flow_builder_set_ipv4: { this_flow->ipv6 = 0; };
flow_builder_set_ipv6: { this_flow->ipv6 = 1; };

net_flow4_: FLOW4 '{' flow_builder_init flow_builder_set_ipv4 flow4_opts '}'
{
  $$ = (net_addr *) flow_builder4_finalize(this_flow, cfg_mem);
  flow4_validate_cf((net_addr_flow4 *) $$);
};

net_flow6_: FLOW6 '{' flow_builder_init flow_builder_set_ipv6 flow6_opts '}'
{
  $$ = (net_addr *) flow_builder6_finalize(this_flow, cfg_mem);
  flow6_validate_cf((net_addr_flow6 *) $$);
};

net_flow_: net_flow4_ | net_flow6_ ;


#line 9 "client/cmds.Y"
cli_cmd: cmd_QUIT
#line 9
cmd_QUIT: QUIT  END
#line 10 "client/cmds.Y"
cli_cmd: cmd_EXIT
#line 10
cmd_EXIT: EXIT  END
#line 11 "client/cmds.Y"
cli_cmd: cmd_HELP
#line 11
cmd_HELP: HELP  END
#line 1 "filter/config.Y"
/*
 *	BIRD - filters
 *
 *	Copyright 1998--2000 Pavel Machek
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 *
	FIXME: priority of ! should be lower
 */

#line 344 "filter/config.Y"
/* Grammar from filter/config.Y */

conf: filter_def ;
filter_def:
   FILTER symbol { $2 = cf_define_symbol($2, SYM_FILTER, filter, NULL); cf_push_scope( $2 ); }
     filter_body {
     struct filter *f = cfg_alloc(sizeof(struct filter));
     *f = (struct filter) { .sym = $2, .root = $4 };
     $2->filter = f;

     cf_pop_scope();
   }
 ;

conf: filter_eval ;
filter_eval:
   EVAL term { f_eval_int(f_linearize($2, 1)); }
 ;

conf: custom_attr ;
custom_attr: ATTRIBUTE type symbol ';' {
  cf_define_symbol($3, SYM_ATTRIBUTE, attribute, ca_lookup(new_config->pool, $3->name, $2)->fda);
};

conf: bt_test_suite ;
bt_test_suite:
 BT_TEST_SUITE '(' CF_SYM_KNOWN ',' text ')' {
  cf_assert_symbol($3, SYM_FUNCTION);
  struct f_bt_test_suite *t = cfg_allocz(sizeof(struct f_bt_test_suite));
  t->fn = $3->function;
  t->fn_name = $3->name;
  t->dsc = $5;

  add_tail(&new_config->tests, &t->n);
 }
 ;

conf: bt_test_same ;
bt_test_same:
 BT_TEST_SAME '(' CF_SYM_KNOWN ',' CF_SYM_KNOWN ',' NUM ')' {
  cf_assert_symbol($3, SYM_FUNCTION);
  cf_assert_symbol($5, SYM_FUNCTION);
  struct f_bt_test_suite *t = cfg_allocz(sizeof(struct f_bt_test_suite));
  t->fn = $3->function;
  t->cmp = $5->function;
  t->result = $7;
  t->fn_name = $3->name;
  t->dsc = $5->name;
  add_tail(&new_config->tests, &t->n);
 }
 ;

type:
   INT { $$ = T_INT; }
 | BOOL { $$ = T_BOOL; }
 | IP { $$ = T_IP; }
 | RD { $$ = T_RD; }
 | PREFIX { $$ = T_NET; }
 | PAIR { $$ = T_PAIR; }
 | QUAD { $$ = T_QUAD; }
 | EC { $$ = T_EC; }
 | LC { $$ = T_LC; }
 | STRING { $$ = T_STRING; }
 | BGPMASK { $$ = T_PATH_MASK; }
 | BGPPATH { $$ = T_PATH; }
 | CLIST { $$ = T_CLIST; }
 | ECLIST { $$ = T_ECLIST; }
 | LCLIST { $$ = T_LCLIST; }
 | type SET {
	switch ($1) {
	  case T_INT:
	  case T_PAIR:
	  case T_QUAD:
	  case T_EC:
	  case T_LC:
	  case T_RD:
	  case T_IP:
	       $$ = T_SET;
	       break;

	  case T_NET:
	       $$ = T_PREFIX_SET;
	    break;

	  default:
		cf_error( "You can't create sets of this type." );
	}
   }
 ;

function_argsn:
   /* EMPTY */ { $$ = NULL; }
 | function_argsn type symbol ';' {
     if ($3->scope->slots >= 0xfe) cf_error("Too many declarations, at most 255 allowed");
     $$ = cfg_alloc(sizeof(struct f_arg));
     $$->arg = cf_define_symbol($3, SYM_VARIABLE | $2, offset, sym_->scope->slots++);
     $$->next = $1;
   }
 ;

function_args:
   '(' ')' { $$ = NULL; }
 | '(' function_argsn type symbol ')' {
     $$ = cfg_alloc(sizeof(struct f_arg));
     $$->arg = cf_define_symbol($4, SYM_VARIABLE | $3, offset, sym_->scope->slots++);
     $$->next = $2;
   }
 ;

function_vars:
   /* EMPTY */ { $$ = 0; }
 | function_vars type symbol ';' {
     cf_define_symbol($3, SYM_VARIABLE | $2, offset, f_new_var(sym_->scope));
     $$ = $1 + 1;
   }
 ;

filter_body: function_body ;

filter:
   CF_SYM_KNOWN {
     cf_assert_symbol($1, SYM_FILTER);
     $$ = $1->filter;
   }
 | { cf_push_scope(NULL); } filter_body {
     struct filter *f = cfg_alloc(sizeof(struct filter));
     *f = (struct filter) { .root = $2 };
     $$ = f;

     cf_pop_scope();
   }
 ;

where_filter:
   WHERE term {
     /* Construct 'IF term THEN { ACCEPT; } ELSE { REJECT; }' */
     $$ = f_new_where($2);
   }
 ;

function_body:
   function_vars '{' cmds '}' {
     $$ = f_linearize($3, 0);
     $$->vars = $1;
   }
 ;

conf: function_def ;
function_def:
   FUNCTION symbol {
     DBG( "Beginning of function %s\n", $2->name );
     $2 = cf_define_symbol($2, SYM_FUNCTION, function, NULL);
     cf_push_scope($2);
   } function_args {
     /* Make dummy f_line for storing function prototype */
     struct f_line *dummy = cfg_allocz(sizeof(struct f_line));
     $2->function = dummy;

     /* Revert the args */
     while ($4) {
       struct f_arg *tmp = $4;
       $4 = $4->next;

       tmp->next = dummy->arg_list;
       dummy->arg_list = tmp;
       dummy->args++;
     }
   } function_body {
     $6->args = $2->function->args;
     $6->arg_list = $2->function->arg_list;
     $2->function = $6;
     cf_pop_scope();
   }
 ;

/* Programs */

cmds: /* EMPTY */ { $$ = NULL; }
 | cmds_int { $$ = $1.begin; }
 ;

cmds_scoped: { cf_push_soft_scope(); } cmds { cf_pop_soft_scope(); $$ = $2; } ;

cmd_var: var | cmd ;

cmd_prep: cmd_var {
  $$.begin = $$.end = $1;
  if ($1)
    while ($$.end->next)
      $$.end = $$.end->next;
}
 ;

cmds_int: cmd_prep
 | cmds_int cmd_prep {
  if (!$1.begin)
    $$ = $2;
  else if (!$2.begin)
    $$ = $1;
  else {
    $$.begin = $1.begin;
    $$.end = $2.end;
    $1.end->next = $2.begin;
  }
 }
 ;

/*
 * Complex types, their bison value is struct f_val
 */
fipa:
   IP4 %prec PREFIX_DUMMY { $$.type = T_IP; $$.val.ip = ipa_from_ip4($1); }
 | IP6 %prec PREFIX_DUMMY { $$.type = T_IP; $$.val.ip = ipa_from_ip6($1); }
 ;



/*
 * Set constants. They are also used in switch cases. We use separate
 * nonterminals for switch (set_atom/switch_atom, set_item/switch_item ...)
 * to elude a collision between symbol (in expr) in set_atom and symbol
 * as a function call in switch case cmds.
 */

set_atom:
   NUM    { $$.type = T_INT; $$.val.i = $1; }
 | fipa   { $$ = $1; }
 | VPN_RD { $$.type = T_RD; $$.val.ec = $1; }
 | ENUM   { $$.type = pair_a($1); $$.val.i = pair_b($1); }
 | '(' term ')' {
     if (f_eval(f_linearize($2, 1), cfg_mem, &($$)) > F_RETURN) cf_error("Runtime error");
     if (!f_valid_set_type($$.type)) cf_error("Set-incompatible type");
   }
 | CF_SYM_KNOWN {
     cf_assert_symbol($1, SYM_CONSTANT);
     if (!f_valid_set_type(SYM_TYPE($1))) cf_error("%s: set-incompatible type", $1->name);
     $$ = *$1->val;
   }
 ;

switch_atom:
   NUM   { $$.type = T_INT; $$.val.i = $1; }
 | '(' term ')' { $$.type = T_INT; $$.val.i = f_eval_int(f_linearize($2, 1)); }
 | fipa  { $$ = $1; }
 | ENUM  { $$.type = pair_a($1); $$.val.i = pair_b($1); }
 ;

cnum:
   term { $$ = f_eval_int(f_linearize($1, 1)); }

pair_item:
   '(' cnum ',' cnum ')'		{ $$ = f_new_pair_item($2, $2, $4, $4); }
 | '(' cnum ',' cnum DDOT cnum ')'	{ $$ = f_new_pair_item($2, $2, $4, $6); }
 | '(' cnum ',' '*' ')'			{ $$ = f_new_pair_item($2, $2, 0, CC_ALL); }
 | '(' cnum DDOT cnum ',' cnum ')'	{ $$ = f_new_pair_set($2, $4, $6, $6); }
 | '(' cnum DDOT cnum ',' cnum DDOT cnum ')' { $$ = f_new_pair_set($2, $4, $6, $8); }
 | '(' cnum DDOT cnum ',' '*' ')'	{ $$ = f_new_pair_item($2, $4, 0, CC_ALL); }
 | '(' '*' ',' cnum ')'			{ $$ = f_new_pair_set(0, CC_ALL, $4, $4); }
 | '(' '*' ',' cnum DDOT cnum ')'	{ $$ = f_new_pair_set(0, CC_ALL, $4, $6); }
 | '(' '*' ',' '*' ')'			{ $$ = f_new_pair_item(0, CC_ALL, 0, CC_ALL); }
 | '(' cnum ',' cnum ')' DDOT '(' cnum ',' cnum ')'
   { $$ = f_new_pair_item($2, $8, $4, $10); }
 ;

ec_kind:
   RT { $$ = EC_RT; }
 | RO { $$ = EC_RO; }
 | UNKNOWN NUM { $$ = $2; }
 | GENERIC { $$ = EC_GENERIC; }
 ;

ec_item:
   '(' ec_kind ',' cnum ',' cnum ')'		{ $$ = f_new_ec_item($2, 0, $4, $6, $6); }
 | '(' ec_kind ',' cnum ',' cnum DDOT cnum ')'	{ $$ = f_new_ec_item($2, 0, $4, $6, $8); }
 | '(' ec_kind ',' cnum ',' '*' ')'		{ $$ = f_new_ec_item($2, 0, $4, 0, EC_ALL); }
 ;

lc_item:
   '(' cnum ',' cnum ',' cnum ')'	    { $$ = f_new_lc_item($2, $2, $4, $4, $6, $6); }
 | '(' cnum ',' cnum ',' cnum DDOT cnum ')' { $$ = f_new_lc_item($2, $2, $4, $4, $6, $8); }
 | '(' cnum ',' cnum ',' '*' ')'	    { $$ = f_new_lc_item($2, $2, $4, $4, 0, LC_ALL); }
 | '(' cnum ',' cnum DDOT cnum ',' '*' ')'  { $$ = f_new_lc_item($2, $2, $4, $6, 0, LC_ALL); }
 | '(' cnum ',' '*' ',' '*' ')'		    { $$ = f_new_lc_item($2, $2, 0, LC_ALL, 0, LC_ALL); }
 | '(' cnum DDOT cnum ',' '*' ',' '*' ')'   { $$ = f_new_lc_item($2, $4, 0, LC_ALL, 0, LC_ALL); }
 | '(' '*' ',' '*' ',' '*' ')'		    { $$ = f_new_lc_item(0, LC_ALL, 0, LC_ALL, 0, LC_ALL); }
 | '(' cnum ',' cnum ',' cnum ')' DDOT '(' cnum ',' cnum ',' cnum ')'
   { $$ = f_new_lc_item($2, $10, $4, $12, $6, $14); }
;

set_item:
   pair_item
 | ec_item
 | lc_item
 | set_atom { $$ = f_new_item($1, $1); }
 | set_atom DDOT set_atom { $$ = f_new_item($1, $3); }
 ;

switch_item:
   pair_item
 | ec_item
 | lc_item
 | switch_atom { $$ = f_new_item($1, $1); }
 | switch_atom DDOT switch_atom { $$ = f_new_item($1, $3); }
 ;

set_items:
   set_item
 | set_items ',' set_item { $$ = f_merge_items($1, $3); }
 ;

switch_items:
   switch_item
 | switch_items ',' switch_item { $$ = f_merge_items($1, $3); }
 ;

fprefix:
   net_ip_	{ $$.net = $1; $$.lo = $1.pxlen; $$.hi = $1.pxlen; }
 | net_ip_ '+'	{ $$.net = $1; $$.lo = $1.pxlen; $$.hi = net_max_prefix_length[$1.type]; }
 | net_ip_ '-'	{ $$.net = $1; $$.lo = 0; $$.hi = $1.pxlen; }
 | net_ip_ '{' NUM ',' NUM '}' {
     $$.net = $1; $$.lo = $3; $$.hi = $5;
     if (($3 > $5) || ($5 > net_max_prefix_length[$1.type]))
       cf_error("Invalid prefix pattern range: {%u, %u}", $3, $5);
   }
 ;

fprefix_set:
   fprefix { $$ = f_new_trie(cfg_mem, 0); trie_add_prefix($$, &($1.net), $1.lo, $1.hi); }
 | fprefix_set ',' fprefix { $$ = $1; if (!trie_add_prefix($$, &($3.net), $3.lo, $3.hi)) cf_error("Mixed IPv4/IPv6 prefixes in prefix set"); }
 ;

switch_body: /* EMPTY */ { $$ = NULL; }
 | switch_body switch_items ':' cmds_scoped  {
     /* Fill data fields */
     struct f_tree *t;
     for (t = $2; t; t = t->left)
       t->data = $4;
     $$ = f_merge_items($1, $2);
   }
 | switch_body ELSECOL cmds_scoped {
     struct f_tree *t = f_new_tree();
     t->from.type = t->to.type = T_VOID;
     t->right = t;
     t->data = $3;
     $$ = f_merge_items($1, t);
 }
 ;

bgp_path_expr:
   symbol_value { $$ = $1; }
 | '(' term ')' { $$ = $2; }
 ;

bgp_path:
   PO  bgp_path_tail PC  { $$ = $2; }
 ;

bgp_path_tail:
   NUM bgp_path_tail		{ $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .asn = $1, .kind = PM_ASN, }, }); $$->next = $2;  }
 | NUM DDOT NUM bgp_path_tail	{ $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .from = $1, .to = $3, .kind = PM_ASN_RANGE }, }); $$->next = $4; }
 | '[' ']' bgp_path_tail { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .set = NULL, .kind = PM_ASN_SET }, }); $$->next = $3; }
 | '[' set_items ']' bgp_path_tail {
   if ($2->from.type != T_INT) cf_error("Only integer sets allowed in path mask");
   $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .set = build_tree($2), .kind = PM_ASN_SET }, }); $$->next = $4;
 }
 | '*' bgp_path_tail		{ $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .kind = PM_ASTERISK }, }); $$->next = $2; }
 | '?' bgp_path_tail		{ $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .kind = PM_QUESTION }, }); $$->next = $2; }
 | '+' bgp_path_tail 		{ $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PATH_MASK_ITEM, .val.pmi = { .kind = PM_LOOP }, }); $$->next = $2; }
 | bgp_path_expr bgp_path_tail	{ $$ = $1; $$->next = $2; }
 | 				{ $$ = NULL; }
 ;

constant:
   NUM    { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_INT, .val.i = $1, }); }
 | TRUE   { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_BOOL, .val.i = 1, }); }
 | FALSE  { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_BOOL, .val.i = 0, }); }
 | TEXT   { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_STRING, .val.s = $1, }); }
 | fipa	  { $$ = f_new_inst(FI_CONSTANT, $1); }
 | VPN_RD { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_RD, .val.ec = $1, }); }
 | net_   { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_NET, .val.net = $1, }); }
 | '[' ']' { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_SET, .val.t = NULL, }); }
 | '[' set_items ']' {
     DBG( "We've got a set here..." );
     $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_SET, .val.t = build_tree($2), });
     DBG( "ook\n" );
 }
 | '[' fprefix_set ']' { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_PREFIX_SET, .val.ti = $2, }); }
 | ENUM	  { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { .type = $1 >> 16, .val.i = $1 & 0xffff, }); }
 ;

constructor:
   '(' term ',' term ')' { $$ = f_new_inst(FI_PAIR_CONSTRUCT, $2, $4); }
 | '(' ec_kind ',' term ',' term ')' { $$ = f_new_inst(FI_EC_CONSTRUCT, $4, $6, $2); }
 | '(' term ',' term ',' term ')' { $$ = f_new_inst(FI_LC_CONSTRUCT, $2, $4, $6); }
 | bgp_path { $$ = f_new_inst(FI_PATHMASK_CONSTRUCT, $1); }
 ;


/* This generates the function_call variable list backwards. */
var_list: /* EMPTY */ { $$ = NULL; }
 | term { $$ = $1; }
 | var_list ',' term { $$ = $3; $$->next = $1; }

function_call:
   CF_SYM_KNOWN '(' var_list ')'
   {
     if ($1->class != SYM_FUNCTION)
       cf_error("You can't call something which is not a function. Really.");

     /* Revert the var_list */
     struct f_inst *args = NULL;
     while ($3) {
       struct f_inst *tmp = $3;
       $3 = $3->next;

       tmp->next = args;
       args = tmp;
     }

     $$ = f_new_inst(FI_CALL, args, $1);
   }
 ;

symbol_value: CF_SYM_KNOWN
  {
    switch ($1->class) {
      case SYM_CONSTANT_RANGE:
	$$ = f_new_inst(FI_CONSTANT, *($1->val));
	break;
      case SYM_VARIABLE_RANGE:
	$$ = f_new_inst(FI_VAR_GET, $1);
	break;
      case SYM_ATTRIBUTE:
	$$ = f_new_inst(FI_EA_GET, *$1->attribute);
	break;
      default:
	cf_error("Can't get value of symbol %s", $1->name);
    }
  }
 ;

static_attr:
   FROM    { $$ = f_new_static_attr(T_IP,         SA_FROM,	0); }
 | GW      { $$ = f_new_static_attr(T_IP,         SA_GW,	0); }
 | NET     { $$ = f_new_static_attr(T_NET,	  SA_NET,	1); }
 | PROTO   { $$ = f_new_static_attr(T_STRING,     SA_PROTO,	1); }
 | SOURCE  { $$ = f_new_static_attr(T_ENUM_RTS,   SA_SOURCE,	1); }
 | SCOPE   { $$ = f_new_static_attr(T_ENUM_SCOPE, SA_SCOPE,	0); }
 | DEST    { $$ = f_new_static_attr(T_ENUM_RTD,   SA_DEST,	0); }
 | IFNAME  { $$ = f_new_static_attr(T_STRING,     SA_IFNAME,	0); }
 | IFINDEX { $$ = f_new_static_attr(T_INT,        SA_IFINDEX,	1); }
 | WEIGHT  { $$ = f_new_static_attr(T_INT,        SA_WEIGHT,	0); }
 | PREFERENCE { $$ = f_new_static_attr(T_INT,	  SA_PREF,	0); }
 | GW_MPLS { $$ = f_new_static_attr(T_INT,        SA_GW_MPLS,	0); }
 | ONLINK  { $$ = f_new_static_attr(T_BOOL,       SA_ONLINK,	0); }
 ;

term:
   '(' term ')'		{ $$ = $2; }
 | term '+' term	{ $$ = f_new_inst(FI_ADD, $1, $3); }
 | term '-' term	{ $$ = f_new_inst(FI_SUBTRACT, $1, $3); }
 | term '*' term	{ $$ = f_new_inst(FI_MULTIPLY, $1, $3); }
 | term '/' term	{ $$ = f_new_inst(FI_DIVIDE, $1, $3); }
 | term AND term	{ $$ = f_new_inst(FI_AND, $1, $3); }
 | term OR  term	{ $$ = f_new_inst(FI_OR, $1, $3); }
 | term '=' term	{ $$ = f_new_inst(FI_EQ, $1, $3); }
 | term NEQ term	{ $$ = f_new_inst(FI_NEQ, $1, $3); }
 | term '<' term	{ $$ = f_new_inst(FI_LT, $1, $3); }
 | term LEQ term	{ $$ = f_new_inst(FI_LTE, $1, $3); }
 | term '>' term	{ $$ = f_new_inst(FI_LT, $3, $1); }
 | term GEQ term	{ $$ = f_new_inst(FI_LTE, $3, $1); }
 | term '~' term	{ $$ = f_new_inst(FI_MATCH, $1, $3); }
 | term NMA term	{ $$ = f_new_inst(FI_NOT_MATCH, $1, $3); }
 | '!' term		{ $$ = f_new_inst(FI_NOT, $2); }
 | DEFINED '(' term ')' { $$ = f_new_inst(FI_DEFINED, $3); }

 | symbol_value   { $$ = $1; }
 | constant { $$ = $1; }
 | constructor { $$ = $1; }

 | static_attr { $$ = f_new_inst(FI_RTA_GET, $1); }

 | dynamic_attr { $$ = f_new_inst(FI_EA_GET, $1); }

 | term '.' IS_V4 { $$ = f_new_inst(FI_IS_V4, $1); }
 | term '.' TYPE { $$ = f_new_inst(FI_TYPE, $1); }
 | term '.' IP { $$ = f_new_inst(FI_IP, $1); }
 | term '.' RD { $$ = f_new_inst(FI_ROUTE_DISTINGUISHER, $1); }
 | term '.' LEN { $$ = f_new_inst(FI_LENGTH, $1); }
 | term '.' MAXLEN { $$ = f_new_inst(FI_ROA_MAXLEN, $1); }
 | term '.' ASN { $$ = f_new_inst(FI_ASN, $1); }
 | term '.' SRC { $$ = f_new_inst(FI_NET_SRC, $1); }
 | term '.' DST { $$ = f_new_inst(FI_NET_DST, $1); }
 | term '.' MASK '(' term ')' { $$ = f_new_inst(FI_IP_MASK, $1, $5); }
 | term '.' FIRST { $$ = f_new_inst(FI_AS_PATH_FIRST, $1); }
 | term '.' LAST  { $$ = f_new_inst(FI_AS_PATH_LAST, $1); }
 | term '.' LAST_NONAGGREGATED  { $$ = f_new_inst(FI_AS_PATH_LAST_NAG, $1); }
 | term '.' DATA { $$ = f_new_inst(FI_PAIR_DATA, $1); }
 | term '.' DATA1 { $$ = f_new_inst(FI_LC_DATA1, $1); }
 | term '.' DATA2 { $$ = f_new_inst(FI_LC_DATA2, $1); }
 | term '.' MIN  { $$ = f_new_inst(FI_MIN, $1); }
 | term '.' MAX  { $$ = f_new_inst(FI_MAX, $1); }

/* Communities */
/* This causes one shift/reduce conflict
 | dynamic_attr '.' ADD '(' term ')' { }
 | dynamic_attr '.' DELETE '(' term ')' { }
 | dynamic_attr '.' CONTAINS '(' term ')' { }
 | dynamic_attr '.' RESET{ }
*/

 | '+' EMPTY '+' { $$ = f_new_inst(FI_CONSTANT, f_const_empty_path); }
 | '-' EMPTY '-' { $$ = f_new_inst(FI_CONSTANT, f_const_empty_clist); }
 | '-' '-' EMPTY '-' '-' { $$ = f_new_inst(FI_CONSTANT, f_const_empty_eclist); }
 | '-' '-' '-' EMPTY '-' '-' '-' { $$ = f_new_inst(FI_CONSTANT, f_const_empty_lclist); }
 | PREPEND '(' term ',' term ')' { $$ = f_new_inst(FI_PATH_PREPEND, $3, $5); }
 | ADD '(' term ',' term ')' { $$ = f_new_inst(FI_CLIST_ADD, $3, $5); }
 | DELETE '(' term ',' term ')' { $$ = f_new_inst(FI_CLIST_DEL, $3, $5); }
 | FILTER '(' term ',' term ')' { $$ = f_new_inst(FI_CLIST_FILTER, $3, $5); }

 | ROA_CHECK '(' rtable ')' { $$ = f_new_inst(FI_ROA_CHECK_IMPLICIT, $3); }
 | ROA_CHECK '(' rtable ',' term ',' term ')' { $$ = f_new_inst(FI_ROA_CHECK_EXPLICIT, $5, $7, $3); }

 | FORMAT '(' term ')' {  $$ = f_new_inst(FI_FORMAT, $3); }

/* | term '.' LEN { $$->code = P('P','l'); } */

 | function_call
 ;

break_command:
   ACCEPT { $$ = F_ACCEPT; }
 | REJECT { $$ = F_REJECT; }
 | ERROR { $$ = F_ERROR; }
 ;

print_list: /* EMPTY */ { $$ = NULL; }
 | term { $$ = $1; }
 | term ',' print_list {
     ASSERT($1);
     ASSERT($1->next == NULL);
     $1->next = $3;
     $$ = $1;
   }
 ;

var_init:
   /* empty */ { $$ = f_new_inst(FI_CONSTANT, (struct f_val) { }); }
 | '=' term { $$ = $2; }
 ;

var:
   type symbol var_init ';' {
     struct symbol *sym = cf_define_symbol($2, SYM_VARIABLE | $1, offset, f_new_var(sym_->scope));
     $$ = f_new_inst(FI_VAR_INIT, $3, sym);
   }

for_var:
   type symbol { $$ = cf_define_symbol($2, SYM_VARIABLE | $1, offset, f_new_var(sym_->scope)); }
 | CF_SYM_KNOWN { $$ = $1; cf_assert_symbol($1, SYM_VARIABLE); }
 ;

cmd:
   '{' cmds_scoped '}' {
     $$ = $2;
   }
 | IF term THEN cmd {
     $$ = f_new_inst(FI_CONDITION, $2, $4, NULL);
   }
 | IF term THEN cmd ELSE cmd {
     $$ = f_new_inst(FI_CONDITION, $2, $4, $6);
   }
 | FOR {
     /* Reserve space for walk data on stack */
     cf_push_block_scope();
     conf_this_scope->slots += 2;
   } for_var IN
   /* Parse term in the parent scope */
   { conf_this_scope->active = 0; } term { conf_this_scope->active = 1; }
   DO cmd {
     cf_pop_block_scope();
     $$ = f_new_inst(FI_FOR_INIT, $6, $3);
     $$->next = f_new_inst(FI_FOR_NEXT, $3, $9);
   }
 | CF_SYM_KNOWN '=' term ';' {
     switch ($1->class) {
       case SYM_VARIABLE_RANGE:
	 $$ = f_new_inst(FI_VAR_SET, $3, $1);
	 break;
       case SYM_ATTRIBUTE:
	 $$ = f_new_inst(FI_EA_SET, $3, *$1->attribute);
	 break;
       default:
	 cf_error("Can't assign to symbol %s", $1->name);
     }
   }
 | RETURN term ';' {
     DBG( "Ook, we'll return the value\n" );
     $$ = f_new_inst(FI_RETURN, $2);
   }
 | dynamic_attr '=' term ';' {
     $$ = f_new_inst(FI_EA_SET, $3, $1);
   }
 | static_attr '=' term ';' {
     if ($1.readonly)
       cf_error( "This static attribute is read-only.");
     $$ = f_new_inst(FI_RTA_SET, $3, $1);
   }
 | UNSET '(' dynamic_attr ')' ';' {
     $$ = f_new_inst(FI_EA_UNSET, $3);
   }
 | break_command print_list ';' {
    struct f_inst *breaker = f_new_inst(FI_DIE, $1);
    if ($2) {
      struct f_inst *printer = f_new_inst(FI_PRINT, $2);
      struct f_inst *flusher = f_new_inst(FI_FLUSH);
      printer->next = flusher;
      flusher->next = breaker;
      $$ = printer;
    } else
      $$ = breaker;
   }
 | PRINT print_list ';' {
    $$ = f_new_inst(FI_PRINT, $2);
    $$->next = f_new_inst(FI_FLUSH);
   }
 | PRINTN print_list ';' {
    $$ = f_new_inst(FI_PRINT, $2);
   }
 | function_call ';' { $$ = f_new_inst(FI_DROP_RESULT, $1); }
 | CASE term '{' switch_body '}' {
      $$ = f_new_inst(FI_SWITCH, $2, $4);
   }

 | dynamic_attr '.' EMPTY ';' { $$ = f_generate_empty($1); }
 | dynamic_attr '.' PREPEND '(' term ')' ';'   { $$ = f_generate_complex( FI_PATH_PREPEND, $1, $5 ); }
 | dynamic_attr '.' ADD '(' term ')' ';'       { $$ = f_generate_complex( FI_CLIST_ADD, $1, $5 ); }
 | dynamic_attr '.' DELETE '(' term ')' ';'    { $$ = f_generate_complex( FI_CLIST_DEL, $1, $5 ); }
 | dynamic_attr '.' FILTER '(' term ')' ';'    { $$ = f_generate_complex( FI_CLIST_FILTER, $1, $5 ); }
 | BT_ASSERT '(' get_cf_position term get_cf_position ')' ';' { $$ = assert_done($4, $3 + 1, $5 - 1); }
 | BT_CHECK_ASSIGN '(' get_cf_position lvalue get_cf_position ',' term ')' ';' { $$ = assert_assign(&$4, $7, $3 + 1, $5 - 1); }
 ;

get_cf_position:
{
  $$ = cf_text;
};

lvalue:
   CF_SYM_KNOWN { cf_assert_symbol($1, SYM_VARIABLE); $$ = (struct f_lval) { .type = F_LVAL_VARIABLE, .sym = $1 }; }
 | static_attr { $$ = (struct f_lval) { .type = F_LVAL_SA, .sa = $1 }; }
 | dynamic_attr { $$ = (struct f_lval) { .type = F_LVAL_EA, .da = $1 }; };

#line 155 "nest/config.Y"
/* Grammar from nest/config.Y */

/* Setting of router ID */

conf: rtrid ;

rtrid:
   ROUTER ID idval ';' { new_config->router_id = $3; }
 | ROUTER ID FROM iface_patt ';' { new_config->router_id_from = this_ipatt; }
 ;

idval:
   NUM { $$ = $1; }
 | '(' term ')' { $$ = f_eval_int(f_linearize($2, 1)); }
 | IP4 { $$ = ip4_to_u32($1); }
 | CF_SYM_KNOWN {
     if ($1->class == (SYM_CONSTANT | T_INT) || $1->class == (SYM_CONSTANT | T_QUAD))
       $$ = SYM_VAL($1).i;
     else if (($1->class == (SYM_CONSTANT | T_IP)) && ipa_is_ip4(SYM_VAL($1).ip))
       $$ = ipa_to_u32(SYM_VAL($1).ip);
     else
       cf_error("Number or IPv4 address constant expected");
   }
 ;

conf: hostname_override ;

hostname_override: HOSTNAME text ';' { new_config->hostname = $2; } ;

conf: gr_opts ;

gr_opts: GRACEFUL RESTART WAIT expr ';' { new_config->gr_wait = $4; } ;


/* Network types (for tables, channels) */

net_type:
   IPV4 { $$ = NET_IP4; }
 | IPV6 { $$ = NET_IP6; }
 | IPV6 SADR { $$ = NET_IP6_SADR; }
 | VPN4 { $$ = NET_VPN4; }
 | VPN6 { $$ = NET_VPN6; }
 | ROA4 { $$ = NET_ROA4; }
 | ROA6 { $$ = NET_ROA6; }
 | FLOW4{ $$ = NET_FLOW4; }
 | FLOW6{ $$ = NET_FLOW6; }
 | MPLS { $$ = NET_MPLS; }
 ;




/* Creation of routing tables */

conf: table ;

table: table_start table_sorted table_opt_list ;

table_start: net_type TABLE symbol {
   this_table = rt_new_table($3, $1);
   }
 ;

table_sorted:
   /* empty */
 | SORTED { this_table->sorted = 1; }
 ;

table_opt:
   SORTED bool { this_table->sorted = $2; }
 | TRIE bool {
     if (!net_val_match(this_table->addr_type, NB_IP | NB_VPN | NB_ROA | NB_IP6_SADR))
       cf_error("Trie option not supported for %s table", net_label[this_table->addr_type]);
     this_table->trie_used = $2;
   }
 | MIN SETTLE TIME expr_us { this_table->min_settle_time = $4; }
 | MAX SETTLE TIME expr_us { this_table->max_settle_time = $4; }
 | GC THRESHOLD expr { this_table->gc_threshold = $3; }
 | GC PERIOD expr_us { this_table->gc_period = (uint) $3; if ($3 > 3600 S_) cf_error("GC period must be at most 3600 s"); }
 ;

table_opts:
   /* empty */
 | table_opts table_opt ';'
 ;

table_opt_list:
   /* empty */
 | '{' table_opts '}'
 ;


/* Definition of protocols */

conf: proto { proto_postconfig(); } ;

proto_start:
   PROTOCOL { $$ = SYM_PROTO; }
 | TEMPLATE { $$ = SYM_TEMPLATE; }
 ;

proto_name:
   /* EMPTY */ {
     struct symbol *s = cf_default_name(this_proto->protocol->template, &this_proto->protocol->name_counter);
     s->class = this_proto->class;
     s->proto = this_proto;
     this_proto->name = s->name;
     }
 | symbol {
     cf_define_symbol($1, this_proto->class, proto, this_proto);
     this_proto->name = $1->name;
   }
 | FROM CF_SYM_KNOWN {
     if (($2->class != SYM_TEMPLATE) && ($2->class != SYM_PROTO)) cf_error("Template or protocol name expected");

     struct symbol *s = cf_default_name(this_proto->protocol->template, &this_proto->protocol->name_counter);
     s->class = this_proto->class;
     s->proto = this_proto;
     this_proto->name = s->name;

     proto_copy_config(this_proto, $2->proto);
   }
 | symbol FROM CF_SYM_KNOWN {
     if (($3->class != SYM_TEMPLATE) && ($3->class != SYM_PROTO)) cf_error("Template or protocol name expected");

     cf_define_symbol($1, this_proto->class, proto, this_proto);
     this_proto->name = $1->name;

     proto_copy_config(this_proto, $3->proto);
   }
 ;

proto_item:
   /* EMPTY */
 | DISABLED bool { this_proto->disabled = $2; }
 | DEBUG debug_mask { this_proto->debug = $2; }
 | MRTDUMP mrtdump_mask { this_proto->mrtdump = $2; }
 | ROUTER ID idval { this_proto->router_id = $3; }
 | DESCRIPTION text { this_proto->dsc = $2; }
 | VRF text { this_proto->vrf = if_get_by_name($2); this_proto->vrf_set = 1; }
 | VRF DEFAULT { this_proto->vrf = NULL; this_proto->vrf_set = 1; }
 ;


channel_start: net_type
{
  $$ = this_channel = channel_config_get(NULL, net_label[$1], $1, this_proto);
};

channel_item_:
   TABLE rtable {
     if (this_channel->net_type && ($2->addr_type != this_channel->net_type))
       cf_error("Incompatible table type");
     this_channel->table = $2;
   }
 | IMPORT imexport { this_channel->in_filter = $2; }
 | EXPORT imexport { this_channel->out_filter = $2; }
 | RECEIVE LIMIT limit_spec { this_channel->rx_limit = $3; }
 | IMPORT LIMIT limit_spec { this_channel->in_limit = $3; }
 | EXPORT LIMIT limit_spec { this_channel->out_limit = $3; }
 | PREFERENCE expr { this_channel->preference = $2; check_u16($2); }
 | IMPORT KEEP FILTERED bool { this_channel->in_keep_filtered = $4; }
 | RPKI RELOAD bool { this_channel->rpki_reload = $3; }
 ;

/* To avoid grammar collision in Pipe protocol */
channel_item:
   channel_item_
 | DEBUG debug_mask { this_channel->debug = $2; }
 ;

channel_opts:
   /* empty */
 | channel_opts channel_item ';'
 ;

channel_opt_list:
   /* empty */
 | '{' channel_opts '}'
 ;

channel_end:
{
  if (!this_channel->table)
    cf_error("Routing table not specified");

  this_channel = NULL;
};

proto_channel: channel_start channel_opt_list channel_end;


rtable: CF_SYM_KNOWN { cf_assert_symbol($1, SYM_TABLE); $$ = $1->table; } ;

imexport:
   FILTER filter { $$ = $2; }
 | where_filter
 | ALL { $$ = FILTER_ACCEPT; }
 | NONE { $$ = FILTER_REJECT; }
 ;

limit_action:
   /* default */ { $$ = PLA_DISABLE; }
 | ACTION WARN { $$ = PLA_WARN; }
 | ACTION BLOCK { $$ = PLA_BLOCK; }
 | ACTION RESTART { $$ = PLA_RESTART; }
 | ACTION DISABLE { $$ = PLA_DISABLE; }
 ;

limit_spec:
   expr limit_action { $$ = (struct channel_limit){ .limit = $1, $$.action = $2 }; }
 | OFF { $$ = (struct channel_limit){}; }
 ;


conf: debug_default ;

debug_default:
   DEBUG PROTOCOLS debug_mask { new_config->proto_default_debug = $3; }
 | DEBUG CHANNELS debug_mask { new_config->channel_default_debug = $3; }
 | DEBUG COMMANDS expr { new_config->cli_debug = $3; }
 ;

/* MRTDUMP PROTOCOLS is in systep/unix/config.Y */

conf: timeformat_base ;

timeformat_which:
   ROUTE { $$ = &new_config->tf_route; }
 | PROTOCOL { $$ = &new_config->tf_proto; }
 | BASE { $$ = &new_config->tf_base; }
 | LOG { $$ = &new_config->tf_log; }
 ;

timeformat_spec:
   timeformat_which TEXT { *$1 = (struct timeformat){$2, NULL, 0}; }
 | timeformat_which TEXT expr TEXT { *$1 = (struct timeformat){$2, $4, (s64) $3 S_}; }
 | timeformat_which ISO SHORT    { *$1 = TM_ISO_SHORT_S; }
 | timeformat_which ISO SHORT MS { *$1 = TM_ISO_SHORT_MS; }
 | timeformat_which ISO SHORT US { *$1 = TM_ISO_SHORT_US; }
 | timeformat_which ISO LONG    { *$1 = TM_ISO_LONG_S; }
 | timeformat_which ISO LONG MS { *$1 = TM_ISO_LONG_MS; }
 | timeformat_which ISO LONG US { *$1 = TM_ISO_LONG_US; }
 ;

timeformat_base:
   TIMEFORMAT timeformat_spec ';'
 ;


/* Interface patterns */

iface_patt_node_init:
   /* EMPTY */ {
     struct iface_patt_node *ipn = cfg_allocz(sizeof(struct iface_patt_node));
     add_tail(&this_ipatt->ipn_list, NODE ipn);
     this_ipn = ipn;
   }
 ;

iface_patt_node_body:
   TEXT { this_ipn->pattern = $1; /* this_ipn->prefix stays zero */ }
 | opttext net_or_ipa { this_ipn->pattern = $1; this_ipn->prefix = $2; }
 ;

iface_negate:
       { this_ipn->positive = 1; }
 | '-' { this_ipn->positive = 0; }
 ;

iface_patt_node:
   iface_patt_node_init iface_negate iface_patt_node_body
 ;


iface_patt_list:
   iface_patt_node
 | iface_patt_list ',' iface_patt_node
 ;

/* For name/mask-only iface patterns */
iface_patt_list_nopx: iface_patt_list { iface_patt_check(); }

iface_patt_init: {
   /* Generic this_ipatt init */
   this_ipatt = cfg_allocz(sizeof(struct iface_patt));
   init_list(&this_ipatt->ipn_list);
 }
 ;

iface_patt:
   iface_patt_init iface_patt_list
 ;

tos:
   CLASS expr { $$ = $2 & 0xfc;        if ($2 > 255) cf_error("TX class must be in range 0-255"); }
 | DSCP expr  { $$ = ($2 & 0x3f) << 2; if ($2 > 63)  cf_error("TX DSCP must be in range 0-63"); }
 ;

/* Direct device route protocol */

proto: dev_proto '}' ;

dev_proto_start: proto_start DIRECT {
     this_proto = proto_config_new(&proto_device, $1);
     init_list(&DIRECT_CFG->iface_list);
   }
 ;

dev_proto:
   dev_proto_start proto_name '{'
 | dev_proto proto_item ';'
 | dev_proto proto_channel ';'
 | dev_proto dev_iface_patt ';'
 | dev_proto CHECK LINK bool ';' { DIRECT_CFG->check_link = $4; }
 ;

dev_iface_init:
   /* EMPTY */ {
     this_ipatt = cfg_allocz(sizeof(struct iface_patt));
     add_tail(&DIRECT_CFG->iface_list, NODE this_ipatt);
     init_list(&this_ipatt->ipn_list);
   }
 ;

dev_iface_patt:
   INTERFACE dev_iface_init iface_patt_list
 ;

/* Debug flags */

debug_mask:
   ALL { $$ = ~0; }
 | OFF { $$ = 0; }
 | '{' debug_list '}' { $$ = $2; }
 ;

debug_list:
   debug_flag
 | debug_list ',' debug_flag { $$ = $1 | $3; }
 ;

debug_flag:
   STATES	{ $$ = D_STATES; }
 | ROUTES	{ $$ = D_ROUTES; }
 | FILTERS	{ $$ = D_FILTERS; }
 | INTERFACES	{ $$ = D_IFACES; }
 | EVENTS	{ $$ = D_EVENTS; }
 | PACKETS	{ $$ = D_PACKETS; }
 ;

/* MRTDump flags */

mrtdump_mask:
   ALL { $$ = ~0; }
 | OFF { $$ = 0; }
 | '{' mrtdump_list '}' { $$ = $2; }
 ;

mrtdump_list:
   mrtdump_flag
 | mrtdump_list ',' mrtdump_flag { $$ = $1 | $3; }
 ;

mrtdump_flag:
   STATES	{ $$ = MD_STATES; }
 | MESSAGES	{ $$ = MD_MESSAGES; }
 ;

/* Password lists */

password_list:
   password_list_body
;

password_list_body:
   PASSWORDS '{' password_items '}'
 | password_item
;

password_items:
    /* empty */
  | password_item ';' password_items
;

password_item:
    password_item_begin '{' password_item_params '}' password_item_end
  | password_item_begin password_item_end
;

pass_key: PASSWORD | KEY;

password_item_begin:
    pass_key text { init_password_list(); init_password($2, strlen($2), password_id++); }
  | pass_key BYTESTRING { init_password_list(); init_password($2->data, $2->length, password_id++); }
;

password_item_params:
   /* empty */ { }
 | GENERATE FROM time ';' password_item_params { this_p_item->genfrom = $3; }
 | GENERATE TO time ';' password_item_params { this_p_item->gento = $3; }
 | ACCEPT FROM time ';' password_item_params { this_p_item->accfrom = $3; }
 | ACCEPT TO time ';' password_item_params { this_p_item->accto = $3; }
 | FROM time ';' password_item_params { this_p_item->genfrom = this_p_item->accfrom = $2; }
 | TO time ';' password_item_params { this_p_item->gento = this_p_item->accto = $2; }
 | ID expr ';' password_item_params { this_p_item->id = $2; if ($2 > 255) cf_error("Password ID must be in range 0-255"); }
 | ALGORITHM password_algorithm ';' password_item_params { this_p_item->alg = $2; }
 ;

password_algorithm:
   KEYED MD5	{ $$ = ALG_MD5; }
 | KEYED SHA1	{ $$ = ALG_SHA1; }
 | KEYED SHA256	{ $$ = ALG_SHA256; }
 | KEYED SHA384	{ $$ = ALG_SHA384; }
 | KEYED SHA512	{ $$ = ALG_SHA512; }
 | HMAC MD5	{ $$ = ALG_HMAC_MD5; }
 | HMAC SHA1	{ $$ = ALG_HMAC_SHA1; }
 | HMAC SHA256	{ $$ = ALG_HMAC_SHA256; }
 | HMAC SHA384	{ $$ = ALG_HMAC_SHA384; }
 | HMAC SHA512	{ $$ = ALG_HMAC_SHA512; }
 | BLAKE2S128	{ $$ = ALG_BLAKE2S_128; }
 | BLAKE2S256	{ $$ = ALG_BLAKE2S_256; }
 | BLAKE2B256	{ $$ = ALG_BLAKE2B_256; }
 | BLAKE2B512	{ $$ = ALG_BLAKE2B_512; }
 ;

password_item_end:
{
  password_validate_length(this_p_item);
};


/* BFD options */

bfd_item:
   INTERVAL expr_us { this_bfd_opts->min_rx_int = this_bfd_opts->min_tx_int = $2; }
 | MIN RX INTERVAL expr_us { this_bfd_opts->min_rx_int = $4; }
 | MIN TX INTERVAL expr_us { this_bfd_opts->min_tx_int = $4; }
 | IDLE TX INTERVAL expr_us { this_bfd_opts->idle_tx_int = $4; }
 | MULTIPLIER expr { this_bfd_opts->multiplier = $2; }
 | PASSIVE bool { this_bfd_opts->passive = $2; this_bfd_opts->passive_set = 1; }
 | GRACEFUL { this_bfd_opts->mode = BGP_BFD_GRACEFUL; }
 ;

bfd_items:
   /* empty */
 | bfd_items bfd_item ';'
 ;

bfd_opts:
   '{' bfd_items '}'
 ;

/* Core commands */


#line 611 "nest/config.Y"
cli_cmd: cmd_SHOW_STATUS
#line 611
cmd_SHOW_STATUS: SHOW STATUS  END
{ cmd_show_status(); } ;

cli_cmd: cmd_SHOW_MEMORY
#line 614 "nest/config.Y"
cmd_SHOW_MEMORY: SHOW MEMORY  END
{ cmd_show_memory(); } ;

cli_cmd: cmd_SHOW_PROTOCOLS
#line 617 "nest/config.Y"
cmd_SHOW_PROTOCOLS: SHOW PROTOCOLS proto_patt2 END
{ proto_apply_cmd($3, proto_cmd_show, 0, 0); } ;

cli_cmd: cmd_SHOW_PROTOCOLS_ALL
#line 620 "nest/config.Y"
cmd_SHOW_PROTOCOLS_ALL: SHOW PROTOCOLS ALL proto_patt2 END
{ proto_apply_cmd($4, proto_cmd_show, 0, 1); } ;

optproto:
   CF_SYM_KNOWN { cf_assert_symbol($1, SYM_PROTO); $$ = $1; }
 | /* empty */ { $$ = NULL; }
 ;

cli_cmd: cmd_SHOW_INTERFACES
#line 628 "nest/config.Y"
cmd_SHOW_INTERFACES: SHOW INTERFACES  END
{ if_show(); } ;

#line 631 "nest/config.Y"
cli_cmd: cmd_SHOW_INTERFACES_SUMMARY
#line 631
cmd_SHOW_INTERFACES_SUMMARY: SHOW INTERFACES SUMMARY  END
{ if_show_summary(); } ;


cli_cmd: cmd_SHOW_ROUTE
#line 635 "nest/config.Y"
cmd_SHOW_ROUTE: SHOW ROUTE r_args END
{ rt_show($3); } ;

r_args:
   /* empty */ {
     $$ = cfg_allocz(sizeof(struct rt_show_data));
     init_list(&($$->tables));
     $$->filter = FILTER_ACCEPT;
     $$->running_on_config = new_config->fallback;
   }
 | r_args net_any {
     $$ = $1;
     if ($$->addr) cf_error("Only one prefix expected");
     $$->addr = $2;
     $$->addr_mode = RSD_ADDR_EQUAL;
   }
 | r_args FOR r_args_for {
     $$ = $1;
     if ($$->addr) cf_error("Only one prefix expected");
     $$->addr = $3;
     $$->addr_mode = RSD_ADDR_FOR;
   }
 | r_args IN net_any {
     $$ = $1;
     if ($$->addr) cf_error("Only one prefix expected");
     if (!net_type_match($3, NB_IP)) cf_error("Only IP networks accepted for 'in' argument");
     $$->addr = $3;
     $$->addr_mode = RSD_ADDR_IN;
   }
| r_args TABLE CF_SYM_KNOWN {
     cf_assert_symbol($3, SYM_TABLE);
     $$ = $1;
     rt_show_add_table($$, $3->table->table);
     $$->tables_defined_by = RSD_TDB_DIRECT;
   }
 | r_args TABLE ALL {
     struct rtable_config *t;
     $$ = $1;
     WALK_LIST(t, config->tables)
       rt_show_add_table($$, t->table);
     $$->tables_defined_by = RSD_TDB_ALL;
   }
 | r_args IMPORT TABLE channel_arg {
     if (!$4->in_table) cf_error("No import table in channel %s.%s", $4->proto->name, $4->name);
     rt_show_add_table($$, $4->in_table);
     $$->tables_defined_by = RSD_TDB_DIRECT;
   }
 | r_args EXPORT TABLE channel_arg {
     if (!$4->out_table) cf_error("No export table in channel %s.%s", $4->proto->name, $4->name);
     rt_show_add_table($$, $4->out_table);
     $$->tables_defined_by = RSD_TDB_DIRECT;
   }
 | r_args FILTER filter {
     $$ = $1;
     if ($$->filter != FILTER_ACCEPT) cf_error("Filter specified twice");
     $$->filter = $3;
   }
 | r_args where_filter {
     $$ = $1;
     if ($$->filter != FILTER_ACCEPT) cf_error("Filter specified twice");
     $$->filter = $2;
   }
 | r_args ALL {
     $$ = $1;
     $$->verbose = 1;
   }
 | r_args PRIMARY {
     $$ = $1;
     $$->primary_only = 1;
   }
 | r_args FILTERED {
     $$ = $1;
     $$->filtered = 1;
   }
 | r_args export_mode CF_SYM_KNOWN {
     cf_assert_symbol($3, SYM_PROTO);
     struct proto_config *c = (struct proto_config *) $3->proto;
     $$ = $1;
     if ($$->export_mode) cf_error("Export specified twice");
     if (!c->proto) cf_error("%s is not a protocol", $3->name);
     $$->export_mode = $2;
     $$->export_protocol = c->proto;
     $$->tables_defined_by = RSD_TDB_INDIRECT;
   }
 | r_args export_mode channel_arg {
     $$ = $1;
     if ($$->export_mode) cf_error("Export specified twice");
     $$->export_mode = $2;
     $$->export_channel = $3;
     $$->tables_defined_by = RSD_TDB_INDIRECT;
   }
 | r_args PROTOCOL CF_SYM_KNOWN {
     cf_assert_symbol($3, SYM_PROTO);
     struct proto_config *c = (struct proto_config *) $3->proto;
     $$ = $1;
     if ($$->show_protocol) cf_error("Protocol specified twice");
     if (!c->proto) cf_error("%s is not a protocol", $3->name);
     $$->show_protocol = c->proto;
     $$->tables_defined_by = RSD_TDB_INDIRECT;
   }
 | r_args STATS {
     $$ = $1;
     $$->stats = 1;
   }
 | r_args COUNT {
     $$ = $1;
     $$->stats = 2;
   }
 ;

r_args_for:
  r_args_for_val {
    $$ = cfg_alloc($1.length);
    net_copy($$, &$1);
  }
 | net_vpn4_
 | net_vpn6_
 | net_ip6_sadr_
 | VPN_RD IP4 {
    $$ = cfg_alloc(sizeof(net_addr_vpn4));
    net_fill_vpn4($$, $2, IP4_MAX_PREFIX_LENGTH, $1);
  }
 | VPN_RD IP6 {
    $$ = cfg_alloc(sizeof(net_addr_vpn6));
    net_fill_vpn6($$, $2, IP6_MAX_PREFIX_LENGTH, $1);
  }
 | IP6 FROM IP6 {
    $$ = cfg_alloc(sizeof(net_addr_ip6_sadr));
    net_fill_ip6_sadr($$, $1, IP6_MAX_PREFIX_LENGTH, $3, IP6_MAX_PREFIX_LENGTH);
  }
 | CF_SYM_KNOWN {
     if ($1->class == (SYM_CONSTANT | T_IP))
     {
       $$ = cfg_alloc(ipa_is_ip4(SYM_VAL($1).ip) ? sizeof(net_addr_ip4) : sizeof(net_addr_ip6));
       net_fill_ip_host($$, SYM_VAL($1).ip);
     }
     else if (($1->class == (SYM_CONSTANT | T_NET)) && net_type_match(SYM_VAL($1).net, NB_IP | NB_VPN))
       $$ = (net_addr *) SYM_VAL($1).net; /* Avoid const warning */
     else
       cf_error("IP address or network constant expected");
   }
 ;

r_args_for_val:
   net_ip4_
 | net_ip6_
 | IP4 { net_fill_ip4(&($$), $1, IP4_MAX_PREFIX_LENGTH); }
 | IP6 { net_fill_ip6(&($$), $1, IP6_MAX_PREFIX_LENGTH); }

export_mode:
   PREEXPORT	{ $$ = RSEM_PREEXPORT; }
 | EXPORT	{ $$ = RSEM_EXPORT; }
 | NOEXPORT	{ $$ = RSEM_NOEXPORT; }
 | EXPORTED	{ $$ = RSEM_EXPORTED; }
 ;

/* This is ugly hack */
channel_sym:
   IPV4		{ $$ = "ipv4"; }
 | IPV4_MC	{ $$ = "ipv4-mc"; }
 | IPV4_MPLS	{ $$ = "ipv4-mpls"; }
 | IPV6		{ $$ = "ipv6"; }
 | IPV6_MC	{ $$ = "ipv6-mc"; }
 | IPV6_MPLS	{ $$ = "ipv6-mpls"; }
 | IPV6_SADR	{ $$ = "ipv6-sadr"; }
 | VPN4		{ $$ = "vpn4"; }
 | VPN4_MC	{ $$ = "vpn4-mc"; }
 | VPN4_MPLS	{ $$ = "vpn4-mpls"; }
 | VPN6		{ $$ = "vpn6"; }
 | VPN6_MC	{ $$ = "vpn6-mc"; }
 | VPN6_MPLS	{ $$ = "vpn6-mpls"; }
 | ROA4		{ $$ = "roa4"; }
 | ROA6		{ $$ = "roa6"; }
 | FLOW4	{ $$ = "flow4"; }
 | FLOW6	{ $$ = "flow6"; }
 | MPLS		{ $$ = "mpls"; }
 | PRI		{ $$ = "pri"; }
 | SEC		{ $$ = "sec"; }
 ;

channel_arg:
   CF_SYM_KNOWN '.' channel_sym {
     cf_assert_symbol($1, SYM_PROTO);
     struct proto *p =  $1->proto->proto;
     if (!p) cf_error("%s is not a protocol", $1->name);
     $$ = proto_find_channel_by_name(p, $3);
     if (!$$) cf_error("Channel %s.%s not found", $1->name, $3);
   }
 ;


#line 826 "nest/config.Y"
cli_cmd: cmd_SHOW_SYMBOLS
#line 826
cmd_SHOW_SYMBOLS: SHOW SYMBOLS sym_args END
{ cmd_show_symbols($3); } ;

sym_args:
   /* empty */ {
     $$ = cfg_allocz(sizeof(struct sym_show_data));
   }
 | sym_args TABLE { $$ = $1; $$->type = SYM_TABLE; }
 | sym_args FUNCTION { $$ = $1; $$->type = SYM_FUNCTION; }
 | sym_args FILTER { $$ = $1; $$->type = SYM_FILTER; }
 | sym_args PROTOCOL { $$ = $1; $$->type = SYM_PROTO; }
 | sym_args TEMPLATE { $$ = $1; $$->type = SYM_TEMPLATE; }
 | sym_args symbol { $$ = $1; $$->sym = $2; }
 ;



#line 843 "nest/config.Y"
cli_cmd: cmd_DUMP_RESOURCES
#line 843
cmd_DUMP_RESOURCES: DUMP RESOURCES  END
{ rdump(&root_pool); cli_msg(0, ""); } ;
#line 845 "nest/config.Y"
cli_cmd: cmd_DUMP_SOCKETS
#line 845
cmd_DUMP_SOCKETS: DUMP SOCKETS  END
{ sk_dump_all(); cli_msg(0, ""); } ;
cli_cmd: cmd_DUMP_EVENTS
#line 847 "nest/config.Y"
cmd_DUMP_EVENTS: DUMP EVENTS  END
{ io_log_dump(); cli_msg(0, ""); } ;
cli_cmd: cmd_DUMP_INTERFACES
#line 849 "nest/config.Y"
cmd_DUMP_INTERFACES: DUMP INTERFACES  END
{ if_dump_all(); cli_msg(0, ""); } ;
#line 851 "nest/config.Y"
cli_cmd: cmd_DUMP_NEIGHBORS
#line 851
cmd_DUMP_NEIGHBORS: DUMP NEIGHBORS  END
{ neigh_dump_all(); cli_msg(0, ""); } ;
#line 853 "nest/config.Y"
cli_cmd: cmd_DUMP_ATTRIBUTES
#line 853
cmd_DUMP_ATTRIBUTES: DUMP ATTRIBUTES  END
{ rta_dump_all(); cli_msg(0, ""); } ;
cli_cmd: cmd_DUMP_ROUTES
#line 855 "nest/config.Y"
cmd_DUMP_ROUTES: DUMP ROUTES  END
{ rt_dump_all(); cli_msg(0, ""); } ;
cli_cmd: cmd_DUMP_PROTOCOLS
#line 857 "nest/config.Y"
cmd_DUMP_PROTOCOLS: DUMP PROTOCOLS  END
{ protos_dump_all(); cli_msg(0, ""); } ;
cli_cmd: cmd_DUMP_FILTER_ALL
#line 859 "nest/config.Y"
cmd_DUMP_FILTER_ALL: DUMP FILTER ALL  END
{ filters_dump_all(); cli_msg(0, ""); } ;

#line 862 "nest/config.Y"
cli_cmd: cmd_EVAL
#line 862
cmd_EVAL: EVAL term END
{ cmd_eval(f_linearize($2, 1)); } ;


#line 866 "nest/config.Y"
cli_cmd: cmd_ECHO
#line 866
cmd_ECHO: ECHO echo_mask echo_size END {
  cli_set_log_echo(this_cli, $2, $3);
  cli_msg(0, "");
} ;

echo_mask:
   ALL { $$ = ~0; }
 | OFF { $$ = 0; }
 | '{' log_mask_list '}' { $$ = $2; }
 ;

echo_size:
   /* empty */ { $$ = 4096; }
 | NUM {
     if ($1 < 256 || $1 > 65536) cf_error("Invalid log buffer size");
     $$ = $1;
   }
 ;

#line 885 "nest/config.Y"
cli_cmd: cmd_DISABLE
#line 885
cmd_DISABLE: DISABLE proto_patt opttext END
{ proto_apply_cmd($2, proto_cmd_disable, 1, (uintptr_t) $3); } ;
#line 887 "nest/config.Y"
cli_cmd: cmd_ENABLE
#line 887
cmd_ENABLE: ENABLE proto_patt opttext END
{ proto_apply_cmd($2, proto_cmd_enable, 1, (uintptr_t) $3); } ;
#line 889 "nest/config.Y"
cli_cmd: cmd_RESTART
#line 889
cmd_RESTART: RESTART proto_patt opttext END
{ proto_apply_cmd($2, proto_cmd_restart, 1, (uintptr_t) $3); } ;
#line 891 "nest/config.Y"
cli_cmd: cmd_RELOAD
#line 891
cmd_RELOAD: RELOAD proto_patt END
{ proto_apply_cmd($2, proto_cmd_reload, 1, CMD_RELOAD); } ;
cli_cmd: cmd_RELOAD_IN
#line 893 "nest/config.Y"
cmd_RELOAD_IN: RELOAD IN proto_patt END
{ proto_apply_cmd($3, proto_cmd_reload, 1, CMD_RELOAD_IN); } ;
cli_cmd: cmd_RELOAD_OUT
#line 895 "nest/config.Y"
cmd_RELOAD_OUT: RELOAD OUT proto_patt END
{ proto_apply_cmd($3, proto_cmd_reload, 1, CMD_RELOAD_OUT); } ;


#line 899 "nest/config.Y"
cli_cmd: cmd_DEBUG
#line 899
cmd_DEBUG: DEBUG debug_args END
{ /* Done in debug_args */  };

debug_args:
   proto_patt debug_mask { proto_apply_cmd($1, proto_cmd_debug, 1, $2);  }
 | channel_arg debug_mask { channel_cmd_debug($1, $2); }
 ;


#line 908 "nest/config.Y"
cli_cmd: cmd_MRTDUMP
#line 908
cmd_MRTDUMP: MRTDUMP proto_patt mrtdump_mask END
{ proto_apply_cmd($2, proto_cmd_mrtdump, 1, $3); } ;

#line 911 "nest/config.Y"
cli_cmd: cmd_RESTRICT
#line 911
cmd_RESTRICT: RESTRICT  END
{ this_cli->restricted = 1; cli_msg(16, "Access restricted"); } ;

proto_patt:
   CF_SYM_KNOWN { cf_assert_symbol($1, SYM_PROTO); $$.ptr = $1; $$.patt = 0; }
 | ALL  { $$.ptr = NULL; $$.patt = 1; }
 | TEXT { $$.ptr = $1; $$.patt = 1; }
 ;

proto_patt2:
   CF_SYM_KNOWN { cf_assert_symbol($1, SYM_PROTO); $$.ptr = $1; $$.patt = 0; }
 |      { $$.ptr = NULL; $$.patt = 1; }
 | TEXT { $$.ptr = $1; $$.patt = 1; }
 ;

dynamic_attr: IGP_METRIC { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_GEN_IGP_METRIC); } ;


#line 33 "proto/bfd/config.Y"
/* Grammar from proto/bfd/config.Y */

proto: bfd_proto ;

bfd_proto_start: proto_start BFD
{
  this_proto = proto_config_new(&proto_bfd, $1);
  init_list(&BFD_CFG->patt_list);
  init_list(&BFD_CFG->neigh_list);
  BFD_CFG->accept_ipv4 = BFD_CFG->accept_ipv6 = 1;
  BFD_CFG->accept_direct = BFD_CFG->accept_multihop = 1;
};

bfd_proto_item:
   proto_item
 | ACCEPT bfd_accept
 | INTERFACE bfd_iface
 | MULTIHOP bfd_multihop
 | NEIGHBOR bfd_neighbor
 | STRICT BIND bool { BFD_CFG->strict_bind = $3; }
 ;

bfd_proto_opts:
   /* empty */
 | bfd_proto_opts bfd_proto_item ';'
 ;

bfd_proto:
   bfd_proto_start proto_name '{' bfd_proto_opts '}';


bfd_accept_item:
   IPV4			{ BFD_CFG->accept_ipv4 = 1;  BFD_CFG->accept_ipv6 = 0; }
 | IPV6 		{ BFD_CFG->accept_ipv4 = 0;  BFD_CFG->accept_ipv6 = 1; }
 | DIRECT		{ BFD_CFG->accept_direct = 1;  BFD_CFG->accept_multihop = 0; }
 | MULTIHOP		{ BFD_CFG->accept_direct = 0;  BFD_CFG->accept_multihop = 1; }
 ;

bfd_accept:
   {
     BFD_CFG->accept_ipv4 = BFD_CFG->accept_ipv6 = 1;
     BFD_CFG->accept_direct = BFD_CFG->accept_multihop = 1;
   }
 | bfd_accept bfd_accept_item


bfd_iface_start:
{
  this_ipatt = cfg_allocz(sizeof(struct bfd_iface_config));
  add_tail(&BFD_CFG->patt_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);

  BFD_IFACE->min_rx_int = BFD_DEFAULT_MIN_RX_INT;
  BFD_IFACE->min_tx_int = BFD_DEFAULT_MIN_TX_INT;
  BFD_IFACE->idle_tx_int = BFD_DEFAULT_IDLE_TX_INT;
  BFD_IFACE->multiplier = BFD_DEFAULT_MULTIPLIER;

  reset_passwords();
};

bfd_iface_finish:
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
};

bfd_iface_item:
   INTERVAL expr_us { BFD_IFACE->min_rx_int = BFD_IFACE->min_tx_int = $2; }
 | MIN RX INTERVAL expr_us { BFD_IFACE->min_rx_int = $4; }
 | MIN TX INTERVAL expr_us { BFD_IFACE->min_tx_int = $4; }
 | IDLE TX INTERVAL expr_us { BFD_IFACE->idle_tx_int = $4; }
 | MULTIPLIER expr { BFD_IFACE->multiplier = $2; }
 | PASSIVE bool { BFD_IFACE->passive = $2; }
 | AUTHENTICATION bfd_auth_type { BFD_IFACE->auth_type = $2; }
 | password_list {}
 ;

bfd_auth_type:
   NONE			 { $$ = BFD_AUTH_NONE; }
 | SIMPLE 		 { $$ = BFD_AUTH_SIMPLE; }
 | KEYED MD5		 { $$ = BFD_AUTH_KEYED_MD5; }
 | KEYED SHA1   	 { $$ = BFD_AUTH_KEYED_SHA1; }
 | METICULOUS KEYED MD5	 { $$ = BFD_AUTH_METICULOUS_KEYED_MD5; }
 | METICULOUS KEYED SHA1 { $$ = BFD_AUTH_METICULOUS_KEYED_SHA1; }
 ;

bfd_iface_opts:
   /* empty */
 | bfd_iface_opts bfd_iface_item ';'
 ;

bfd_iface_opt_list:
   /* empty */
 | '{' bfd_iface_opts '}'
 ;

bfd_iface:
  bfd_iface_start iface_patt_list_nopx bfd_iface_opt_list bfd_iface_finish;

bfd_multihop:
  bfd_iface_start bfd_iface_opt_list bfd_iface_finish
{ BFD_CFG->multihop = BFD_IFACE; };


bfd_neigh_iface:
   /* empty */ { $$ = NULL; }
 | '%' symbol { $$ = if_get_by_name($2->name); }
 | DEV text { $$ = if_get_by_name($2); }
 ;

bfd_neigh_local:
   /* empty */ { $$ = IPA_NONE; }
 | LOCAL ipa { $$ = $2; }
 ;

bfd_neigh_multihop:
   /* empty */ { $$ = 0; }
 | MULTIHOP bool { $$ = $2; }
 ;

bfd_neighbor: ipa bfd_neigh_iface bfd_neigh_local bfd_neigh_multihop
{
  this_bfd_neighbor = cfg_allocz(sizeof(struct bfd_neighbor));
  add_tail(&BFD_CFG->neigh_list, NODE this_bfd_neighbor);

  BFD_NEIGHBOR->addr = $1;
  BFD_NEIGHBOR->local = $3;
  BFD_NEIGHBOR->iface = $2;
  BFD_NEIGHBOR->multihop = $4;

  if ($4 && $2)
    cf_error("Neighbor cannot set both interface and multihop");

  if ($4 && ipa_zero($3))
    cf_error("Multihop neighbor requires specified local address");
};


;
#line 186 "proto/bfd/config.Y"
cli_cmd: cmd_SHOW_BFD_SESSIONS
#line 186
cmd_SHOW_BFD_SESSIONS: SHOW BFD SESSIONS optproto END
{ PROTO_WALK_CMD($4, &proto_bfd, p) bfd_show_sessions(p); };

#line 31 "proto/babel/config.Y"
/* Grammar from proto/babel/config.Y */

proto: babel_proto ;

babel_proto_start: proto_start BABEL
{
  this_proto = proto_config_new(&proto_babel, $1);
  init_list(&BABEL_CFG->iface_list);
  BABEL_CFG->hold_time = 1 S_;
};

babel_proto_item:
   proto_item
 | proto_channel
 | INTERFACE babel_iface
 | RANDOMIZE ROUTER ID bool { BABEL_CFG->randomize_router_id = $4; }
 ;

babel_proto_opts:
   /* empty */
 | babel_proto_opts babel_proto_item ';'
 ;

babel_proto:
   babel_proto_start proto_name '{' babel_proto_opts '}';


babel_iface_start:
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
};


babel_iface_finish:
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

};


babel_iface_item:
 | PORT expr { BABEL_IFACE->port = $2; if (($2<1) || ($2>65535)) cf_error("Invalid port number"); }
 | RXCOST expr { BABEL_IFACE->rxcost = $2; if (($2<1) || ($2>65535)) cf_error("Invalid rxcost"); }
 | LIMIT expr { BABEL_IFACE->limit = $2; if (($2<1) || ($2>16)) cf_error("Limit must be in range 1-16"); }
 | TYPE WIRED { BABEL_IFACE->type = BABEL_IFACE_TYPE_WIRED; }
 | TYPE WIRELESS { BABEL_IFACE->type = BABEL_IFACE_TYPE_WIRELESS; }
 | HELLO INTERVAL expr_us { BABEL_IFACE->hello_interval = $3; if (($3<BABEL_MIN_INTERVAL) || ($3>BABEL_MAX_INTERVAL)) cf_error("Hello interval must be in range 10 ms - 655 s"); }
 | UPDATE INTERVAL expr_us { BABEL_IFACE->update_interval = $3; if (($3<BABEL_MIN_INTERVAL) || ($3>BABEL_MAX_INTERVAL)) cf_error("Update interval must be in range 10 ms - 655 s"); }
 | RX BUFFER expr { BABEL_IFACE->rx_buffer = $3; if (($3<256) || ($3>65535)) cf_error("RX buffer must be in range 256-65535"); }
 | TX LENGTH expr { BABEL_IFACE->tx_length = $3; if (($3<256) || ($3>65535)) cf_error("TX length must be in range 256-65535"); }
 | TX tos { BABEL_IFACE->tx_tos = $2; }
 | TX PRIORITY expr { BABEL_IFACE->tx_priority = $3; }
 | CHECK LINK bool { BABEL_IFACE->check_link = $3; }
 | NEXT HOP IPV4 ipa { BABEL_IFACE->next_hop_ip4 = $4; if (!ipa_is_ip4($4)) cf_error("Must be an IPv4 address"); }
 | NEXT HOP IPV6 ipa { BABEL_IFACE->next_hop_ip6 = $4; if (!ipa_is_ip6($4)) cf_error("Must be an IPv6 address"); }
 | EXTENDED NEXT HOP bool { BABEL_IFACE->ext_next_hop = $4; }
 | AUTHENTICATION NONE { BABEL_IFACE->auth_type = BABEL_AUTH_NONE; }
 | AUTHENTICATION MAC { BABEL_IFACE->auth_type = BABEL_AUTH_MAC; BABEL_IFACE->auth_permissive = 0; }
 | AUTHENTICATION MAC PERMISSIVE { BABEL_IFACE->auth_type = BABEL_AUTH_MAC; BABEL_IFACE->auth_permissive = 1; }
 | password_list
 ;

babel_iface_opts:
   /* empty */
 | babel_iface_opts babel_iface_item ';'
 ;

babel_iface_opt_list:
   /* empty */
 | '{' babel_iface_opts '}'
 ;


babel_iface:
  babel_iface_start iface_patt_list_nopx babel_iface_opt_list babel_iface_finish;

dynamic_attr: BABEL_METRIC { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_BABEL_METRIC); } ;

;

cli_cmd: cmd_SHOW_BABEL_INTERFACES
#line 173 "proto/babel/config.Y"
cmd_SHOW_BABEL_INTERFACES: SHOW BABEL INTERFACES optproto opttext END
{ PROTO_WALK_CMD($4, &proto_babel, p) babel_show_interfaces(p, $5); };

cli_cmd: cmd_SHOW_BABEL_NEIGHBORS
#line 176 "proto/babel/config.Y"
cmd_SHOW_BABEL_NEIGHBORS: SHOW BABEL NEIGHBORS optproto opttext END
{ PROTO_WALK_CMD($4, &proto_babel, p) babel_show_neighbors(p, $5); };

cli_cmd: cmd_SHOW_BABEL_ENTRIES
#line 179 "proto/babel/config.Y"
cmd_SHOW_BABEL_ENTRIES: SHOW BABEL ENTRIES optproto opttext END
{ PROTO_WALK_CMD($4, &proto_babel, p) babel_show_entries(p); };

cli_cmd: cmd_SHOW_BABEL_ROUTES
#line 182 "proto/babel/config.Y"
cmd_SHOW_BABEL_ROUTES: SHOW BABEL ROUTES optproto opttext END
{ PROTO_WALK_CMD($4, &proto_babel, p)  babel_show_routes(p); };

#line 46 "proto/bgp/config.Y"
/* Grammar from proto/bgp/config.Y */

/* Workaround for collisions between keywords and symbols */
symbol: ROLE | PEER | PROVIDER | CUSTOMER | RS_SERVER | RS_CLIENT ;

proto: bgp_proto '}'  ;

bgp_proto_start: proto_start BGP {
     this_proto = proto_config_new(&proto_bgp, $1);
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
 ;

bgp_loc_opts:
   /* empty */
 | bgp_loc_opts PORT expr { BGP_CFG->local_port = $3; if (($3<1) || ($3>65535)) cf_error("Invalid port number"); }
 | bgp_loc_opts AS expr { BGP_CFG->local_as = $3; }
 ;

bgp_nbr_opts:
   /* empty */
 | bgp_nbr_opts PORT expr { BGP_CFG->remote_port = $3; if (($3<1) || ($3>65535)) cf_error("Invalid port number"); }
 | bgp_nbr_opts AS expr { BGP_CFG->remote_as = $3; }
 | bgp_nbr_opts INTERNAL { BGP_CFG->peer_type = BGP_PT_INTERNAL; }
 | bgp_nbr_opts EXTERNAL { BGP_CFG->peer_type = BGP_PT_EXTERNAL; }
 ;

bgp_cease_mask:
   /* true -> all except connection collision */
   bool { $$ = $1 ? ~(1 << 7) : 0; }
 | '{' bgp_cease_list '}' { $$ = $2; }
 ;

bgp_cease_list:
   bgp_cease_flag
 | bgp_cease_list ',' bgp_cease_flag { $$ = $1 | $3; }
 ;

bgp_cease_flag:
   CEASE			{ $$ = 1 << 0; }
 | PREFIX LIMIT HIT		{ $$ = 1 << 1; }
 | ADMINISTRATIVE SHUTDOWN	{ $$ = 1 << 2; }
 | PEER DECONFIGURED		{ $$ = 1 << 3; }
 | ADMINISTRATIVE RESET		{ $$ = 1 << 4; }
 | CONNECTION REJECTED		{ $$ = 1 << 5; }
 | CONFIGURATION CHANGE		{ $$ = 1 << 6; }
 | CONNECTION COLLISION		{ $$ = 1 << 7; }
 | OUT OF RESOURCES		{ $$ = 1 << 8; }
 ;

bgp_role_name:
   PEER      { $$ = BGP_ROLE_PEER; }
 | PROVIDER  { $$ = BGP_ROLE_PROVIDER; }
 | CUSTOMER  { $$ = BGP_ROLE_CUSTOMER; }
 | RS_SERVER { $$ = BGP_ROLE_RS_SERVER; }
 | RS_CLIENT { $$ = BGP_ROLE_RS_CLIENT; }
 ;

bgp_proto:
   bgp_proto_start proto_name '{'
 | bgp_proto proto_item ';'
 | bgp_proto bgp_proto_channel ';'
 | bgp_proto LOCAL bgp_loc_opts ';'
 | bgp_proto LOCAL ipa ipa_scope bgp_loc_opts ';' {
     BGP_CFG->local_ip = $3;
     if ($4) BGP_CFG->iface = $4;
   }
 | bgp_proto NEIGHBOR bgp_nbr_opts ';'
 | bgp_proto NEIGHBOR ipa ipa_scope bgp_nbr_opts ';' {
     if (ipa_nonzero(BGP_CFG->remote_ip) || BGP_CFG->remote_range)
       cf_error("Only one neighbor per BGP instance is allowed");
     BGP_CFG->remote_ip = $3;
     if ($4) BGP_CFG->iface = $4;
   }
 | bgp_proto NEIGHBOR RANGE net_ip bgp_nbr_opts ';' {
     if (ipa_nonzero(BGP_CFG->remote_ip) || BGP_CFG->remote_range)
       cf_error("Only one neighbor per BGP instance is allowed");
     net_addr *n = cfg_alloc($4.length);
     net_copy(n, &($4));
     BGP_CFG->remote_range = n;
   }
 | bgp_proto INTERFACE TEXT ';' { BGP_CFG->iface = if_get_by_name($3); }
 | bgp_proto RR CLUSTER ID idval ';' { BGP_CFG->rr_cluster_id = $5; }
 | bgp_proto RR CLIENT bool ';' { BGP_CFG->rr_client = $4; }
 | bgp_proto RS CLIENT bool ';' { BGP_CFG->rs_client = $4; }
 | bgp_proto CONFEDERATION expr ';' { BGP_CFG->confederation = $3; }
 | bgp_proto CONFEDERATION MEMBER bool ';' { BGP_CFG->confederation_member = $4; }
 | bgp_proto HOLD TIME expr ';' { BGP_CFG->hold_time = $4; if (($4 && $4<3) || ($4>65535)) cf_error("Hold time must be in range 3-65535 or zero"); }
 | bgp_proto MIN HOLD TIME expr ';' { BGP_CFG->min_hold_time = $5; }
 | bgp_proto STARTUP HOLD TIME expr ';' { BGP_CFG->initial_hold_time = $5; }
 | bgp_proto DIRECT ';' { BGP_CFG->multihop = 0; }
 | bgp_proto MULTIHOP ';' { BGP_CFG->multihop = 64; }
 | bgp_proto MULTIHOP expr ';' { BGP_CFG->multihop = $3; if (($3<1) || ($3>255)) cf_error("Multihop must be in range 1-255"); }
 | bgp_proto DYNAMIC NAME text ';' {
     if (strchr($4, '%')) cf_error("Forbidden character '%%' in dynamic name");
     if (strlen($4) > (SYM_MAX_LEN - 16)) cf_error("Dynamic name too long");
     BGP_CFG->dynamic_name = $4;
   }
 | bgp_proto DYNAMIC NAME DIGITS expr ';' { BGP_CFG->dynamic_name_digits = $5; if ($5>10) cf_error("Dynamic name digits must be at most 10"); }
 | bgp_proto STRICT BIND bool ';' { BGP_CFG->strict_bind = $4; }
 | bgp_proto FREE BIND bool ';' { BGP_CFG->free_bind = $4; }
 | bgp_proto PATH METRIC bool ';' { BGP_CFG->compare_path_lengths = $4; }
 | bgp_proto MED METRIC bool ';' { BGP_CFG->med_metric = $4; }
 | bgp_proto IGP METRIC bool ';' { BGP_CFG->igp_metric = $4; }
 | bgp_proto PREFER OLDER bool ';' { BGP_CFG->prefer_older = $4; }
 | bgp_proto DETERMINISTIC MED bool ';' { BGP_CFG->deterministic_med = $4; }
 | bgp_proto DEFAULT BGP_MED expr ';' { BGP_CFG->default_med = $4; }
 | bgp_proto DEFAULT BGP_LOCAL_PREF expr ';' { BGP_CFG->default_local_pref = $4; }
 | bgp_proto SOURCE ADDRESS ipa ';' { BGP_CFG->local_ip = $4; }
 | bgp_proto START DELAY TIME expr ';' { BGP_CFG->connect_delay_time = $5; log(L_WARN "%s: Start delay time option is deprecated, use connect delay time", this_proto->name); }
 | bgp_proto CONNECT DELAY TIME expr ';' { BGP_CFG->connect_delay_time = $5; }
 | bgp_proto CONNECT RETRY TIME expr ';' { BGP_CFG->connect_retry_time = $5; }
 | bgp_proto KEEPALIVE TIME expr ';' { BGP_CFG->keepalive_time = $4; if (($4<1) || ($4>65535)) cf_error("Keepalive time must be in range 1-65535"); }
 | bgp_proto MIN KEEPALIVE TIME expr ';' { BGP_CFG->min_keepalive_time = $5; }
 | bgp_proto ERROR FORGET TIME expr ';' { BGP_CFG->error_amnesia_time = $5; }
 | bgp_proto ERROR WAIT TIME expr ',' expr ';' { BGP_CFG->error_delay_time_min = $5; BGP_CFG->error_delay_time_max = $7; }
 | bgp_proto DISABLE AFTER ERROR bool ';' { BGP_CFG->disable_after_error = $5; }
 | bgp_proto DISABLE AFTER CEASE bgp_cease_mask ';' { BGP_CFG->disable_after_cease = $5; }
 | bgp_proto ENABLE ROUTE REFRESH bool ';' { BGP_CFG->enable_refresh = $5; }
 | bgp_proto ENABLE AS4 bool ';' { BGP_CFG->enable_as4 = $4; }
 | bgp_proto ENABLE EXTENDED MESSAGES bool ';' { BGP_CFG->enable_extended_messages = $5; }
 | bgp_proto ADVERTISE HOSTNAME bool ';' { BGP_CFG->enable_hostname = $4; }
 | bgp_proto CAPABILITIES bool ';' { BGP_CFG->capabilities = $3; }
 | bgp_proto PASSWORD text ';' { BGP_CFG->password = $3; }
 | bgp_proto SETKEY bool ';' { BGP_CFG->setkey = $3; }
 | bgp_proto PASSIVE bool ';' { BGP_CFG->passive = $3; }
 | bgp_proto INTERPRET COMMUNITIES bool ';' { BGP_CFG->interpret_communities = $4; }
 | bgp_proto ALLOW LOCAL AS ';' { BGP_CFG->allow_local_as = -1; }
 | bgp_proto ALLOW LOCAL AS expr ';' { BGP_CFG->allow_local_as = $5; }
 | bgp_proto ALLOW BGP_LOCAL_PREF bool ';' { BGP_CFG->allow_local_pref = $4; }
 | bgp_proto ALLOW BGP_MED bool ';' { BGP_CFG->allow_med = $4; }
 | bgp_proto ALLOW AS SETS bool ';' { BGP_CFG->allow_as_sets = $5; }
 | bgp_proto GRACEFUL RESTART bool ';' { BGP_CFG->gr_mode = $4; }
 | bgp_proto GRACEFUL RESTART AWARE ';' { BGP_CFG->gr_mode = BGP_GR_AWARE; }
 | bgp_proto GRACEFUL RESTART TIME expr ';' { BGP_CFG->gr_time = $5; }
 | bgp_proto LONG LIVED GRACEFUL RESTART bool ';' { BGP_CFG->llgr_mode = $6; }
 | bgp_proto LONG LIVED GRACEFUL RESTART AWARE ';' { BGP_CFG->llgr_mode = BGP_LLGR_AWARE; }
 | bgp_proto LONG LIVED STALE TIME expr ';' { BGP_CFG->llgr_time = $6; }
 | bgp_proto TTL SECURITY bool ';' { BGP_CFG->ttl_security = $4; }
 | bgp_proto CHECK LINK bool ';' { BGP_CFG->check_link = $4; }
 | bgp_proto BFD bool ';' { if ($3) init_bfd_opts(&BGP_CFG->bfd); else BGP_CFG->bfd = NULL; }
 | bgp_proto BFD GRACEFUL ';' { init_bfd_opts(&BGP_CFG->bfd); BGP_CFG->bfd->mode = BGP_BFD_GRACEFUL; }
 | bgp_proto BFD { open_bfd_opts(&BGP_CFG->bfd); } bfd_opts { close_bfd_opts(); } ';'
 | bgp_proto ENFORCE FIRST AS bool ';' { BGP_CFG->enforce_first_as = $5; }
 | bgp_proto LOCAL ROLE bgp_role_name ';' { BGP_CFG->local_role = $4; }
 | bgp_proto REQUIRE ROLES bool ';' { BGP_CFG->require_roles = $4; }
 ;

bgp_afi:
   IPV4			{ $$ = BGP_AF_IPV4; }
 | IPV6			{ $$ = BGP_AF_IPV6; }
 | IPV4 MULTICAST	{ $$ = BGP_AF_IPV4_MC; }
 | IPV6 MULTICAST	{ $$ = BGP_AF_IPV6_MC; }
 | IPV4 MPLS		{ $$ = BGP_AF_IPV4_MPLS; }
 | IPV6 MPLS		{ $$ = BGP_AF_IPV6_MPLS; }
 | VPN4 MPLS		{ $$ = BGP_AF_VPN4_MPLS; }
 | VPN6 MPLS		{ $$ = BGP_AF_VPN6_MPLS; }
 | VPN4 MULTICAST	{ $$ = BGP_AF_VPN4_MC; }
 | VPN6 MULTICAST	{ $$ = BGP_AF_VPN6_MC; }
 | FLOW4		{ $$ = BGP_AF_FLOW4; }
 | FLOW6		{ $$ = BGP_AF_FLOW6; }
 ;

bgp_channel_start: bgp_afi
{
  const struct bgp_af_desc *desc = bgp_get_af_desc($1);

  if (!desc)
    cf_error("Unknown AFI/SAFI");

  this_channel = channel_config_get(&channel_bgp, desc->name, desc->net, this_proto);

  /* New channel */
  if (!BGP_CC->desc)
  {
    BGP_CC->c.in_filter = FILTER_UNDEF;
    BGP_CC->c.out_filter = FILTER_UNDEF;
    BGP_CC->c.ra_mode = RA_UNDEF;
    BGP_CC->afi = $1;
    BGP_CC->desc = desc;
    BGP_CC->next_hop_keep = 0xff; /* undefined */
    BGP_CC->gr_able = 0xff;	/* undefined */
    BGP_CC->llgr_able = 0xff;	/* undefined */
    BGP_CC->llgr_time = ~0U;	/* undefined */
    BGP_CC->aigp = 0xff;	/* undefined */
  }
};

bgp_nh:
   bool { $$ = $1; }
 | IBGP { $$ = NH_IBGP; }
 | EBGP { $$ = NH_EBGP; }

bgp_lladdr: SELF | DROP | IGNORE;

bgp_channel_item:
   channel_item
 | NEXT HOP ADDRESS ipa { BGP_CC->next_hop_addr = $4; }
 | NEXT HOP SELF bgp_nh { BGP_CC->next_hop_self = $4; }
 | NEXT HOP KEEP bgp_nh { BGP_CC->next_hop_keep = $4; }
 | NEXT HOP PREFER GLOBAL { BGP_CC->next_hop_prefer = NHP_GLOBAL; }
 | MANDATORY bool { BGP_CC->mandatory = $2; }
 | MISSING LLADDR bgp_lladdr { log(L_WARN "%s.%s: Missing lladdr option is deprecated and ignored, remove it", this_proto->name, this_channel->name); }
 | GATEWAY DIRECT { BGP_CC->gw_mode = GW_DIRECT; }
 | GATEWAY RECURSIVE { BGP_CC->gw_mode = GW_RECURSIVE; }
 | SECONDARY bool { BGP_CC->secondary = $2; }
 | VALIDATE bool {
    BGP_CC->validate = $2;
    if (BGP_SAFI(BGP_CC->afi) != BGP_SAFI_FLOW)
      cf_error("Validate option limited to flowspec channels");
   }
 | GRACEFUL RESTART bool { BGP_CC->gr_able = $3; }
 | LONG LIVED GRACEFUL RESTART bool { BGP_CC->llgr_able = $5; }
 | LONG LIVED STALE TIME expr { BGP_CC->llgr_time = $5; }
 | EXTENDED NEXT HOP bool { BGP_CC->ext_next_hop = $4; }
 | ADD PATHS RX { BGP_CC->add_path = BGP_ADD_PATH_RX; }
 | ADD PATHS TX { BGP_CC->add_path = BGP_ADD_PATH_TX; }
 | ADD PATHS bool { BGP_CC->add_path = $3 ? BGP_ADD_PATH_FULL : 0; }
 | IMPORT TABLE bool { BGP_CC->import_table = $3; }
 | EXPORT TABLE bool { BGP_CC->export_table = $3; }
 | AIGP bool { BGP_CC->aigp = $2; BGP_CC->aigp_originate = 0; }
 | AIGP ORIGINATE { BGP_CC->aigp = 1; BGP_CC->aigp_originate = 1; }
 | COST expr { BGP_CC->cost = $2; if ($2 < 1) cf_error("Cost must be positive"); }
 | IGP TABLE rtable {
    if (BGP_CC->desc->no_igp)
      cf_error("IGP table not allowed here");

    if ($3->addr_type == NET_IP4)
      BGP_CC->igp_table_ip4 = $3;
    else if ($3->addr_type == NET_IP6)
      BGP_CC->igp_table_ip6 = $3;
    else
      cf_error("Mismatched IGP table type");
   }
 | BASE TABLE rtable {
    if (BGP_SAFI(BGP_CC->afi) != BGP_SAFI_FLOW)
      cf_error("Base table option limited to flowspec channels");

    if (((BGP_CC->afi == BGP_AF_FLOW4) && ($3->addr_type == NET_IP4)) ||
        ((BGP_CC->afi == BGP_AF_FLOW6) && ($3->addr_type == NET_IP6)))
      BGP_CC->base_table = $3;
    else
      cf_error("Mismatched base table type");
   }
 ;

bgp_channel_opts:
   /* empty */
 | bgp_channel_opts bgp_channel_item ';'
 ;

bgp_channel_opt_list:
   /* empty */
 | '{' bgp_channel_opts '}'
 ;

bgp_channel_end:
{
  if (!this_channel->table)
    cf_error("Routing table not specified");

  this_channel = NULL;
};

bgp_proto_channel: bgp_channel_start bgp_channel_opt_list bgp_channel_end;


dynamic_attr: BGP_ORIGIN
	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_ENUM_BGP_ORIGIN, EA_CODE(PROTOCOL_BGP, BA_ORIGIN)); } ;
dynamic_attr: BGP_PATH
	{ $$ = f_new_dynamic_attr(EAF_TYPE_AS_PATH, T_PATH, EA_CODE(PROTOCOL_BGP, BA_AS_PATH)); } ;
dynamic_attr: BGP_NEXT_HOP
	{ $$ = f_new_dynamic_attr(EAF_TYPE_IP_ADDRESS, T_IP, EA_CODE(PROTOCOL_BGP, BA_NEXT_HOP)); } ;
dynamic_attr: BGP_MED
	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_CODE(PROTOCOL_BGP, BA_MULTI_EXIT_DISC)); } ;
dynamic_attr: BGP_LOCAL_PREF
	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_CODE(PROTOCOL_BGP, BA_LOCAL_PREF)); } ;
dynamic_attr: BGP_ATOMIC_AGGR
	{ $$ = f_new_dynamic_attr(EAF_TYPE_OPAQUE, T_ENUM_EMPTY, EA_CODE(PROTOCOL_BGP, BA_ATOMIC_AGGR)); } ;
dynamic_attr: BGP_AGGREGATOR
	{ $$ = f_new_dynamic_attr(EAF_TYPE_OPAQUE, T_ENUM_EMPTY, EA_CODE(PROTOCOL_BGP, BA_AGGREGATOR)); } ;
dynamic_attr: BGP_COMMUNITY
	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT_SET, T_CLIST, EA_CODE(PROTOCOL_BGP, BA_COMMUNITY)); } ;
dynamic_attr: BGP_ORIGINATOR_ID
	{ $$ = f_new_dynamic_attr(EAF_TYPE_ROUTER_ID, T_QUAD, EA_CODE(PROTOCOL_BGP, BA_ORIGINATOR_ID)); } ;
dynamic_attr: BGP_CLUSTER_LIST
	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT_SET, T_CLIST, EA_CODE(PROTOCOL_BGP, BA_CLUSTER_LIST)); } ;
dynamic_attr: BGP_EXT_COMMUNITY
	{ $$ = f_new_dynamic_attr(EAF_TYPE_EC_SET, T_ECLIST, EA_CODE(PROTOCOL_BGP, BA_EXT_COMMUNITY)); } ;
dynamic_attr: BGP_AIGP
	{ $$ = f_new_dynamic_attr(EAF_TYPE_OPAQUE, T_ENUM_EMPTY, EA_CODE(PROTOCOL_BGP, BA_AIGP)); } ;
dynamic_attr: BGP_LARGE_COMMUNITY
	{ $$ = f_new_dynamic_attr(EAF_TYPE_LC_SET, T_LCLIST, EA_CODE(PROTOCOL_BGP, BA_LARGE_COMMUNITY)); } ;
dynamic_attr: BGP_OTC
	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_CODE(PROTOCOL_BGP, BA_ONLY_TO_CUSTOMER)); } ;





#line 24 "proto/mrt/config.Y"
/* Grammar from proto/mrt/config.Y */

proto: mrt_proto ;

mrt_proto_start: proto_start MRT
{
  this_proto = proto_config_new(&proto_mrt, $1);
};

mrt_proto_item:
   proto_item
 | TABLE rtable		{ MRT_CFG->table_cf = $2; }
 | TABLE TEXT		{ MRT_CFG->table_expr = $2; }
 | FILTER filter	{ MRT_CFG->filter = $2; }
 | where_filter		{ MRT_CFG->filter = $1; }
 | FILENAME text	{ MRT_CFG->filename = $2; }
 | PERIOD expr		{ MRT_CFG->period = $2; }
 | ALWAYS ADD PATH bool	{ MRT_CFG->always_add_path = $4; }
 ;

mrt_proto_opts:
   /* empty */
 | mrt_proto_opts mrt_proto_item ';'
 ;

mrt_proto:
   mrt_proto_start proto_name '{' mrt_proto_opts '}' { mrt_check_config(this_proto); };


cli_cmd: cmd_MRT_DUMP
#line 53 "proto/mrt/config.Y"
cmd_MRT_DUMP: MRT DUMP mrt_dump_args END
{ mrt_dump_cmd($3); } ;

mrt_dump_args:
   /* empty */ { $$ = cfg_allocz(sizeof(struct mrt_dump_data)); }
 | mrt_dump_args TABLE rtable	{ $$ = $1; $$->table_ptr = $3->table; }
 | mrt_dump_args TABLE TEXT	{ $$ = $1; $$->table_expr = $3; }
 | mrt_dump_args FILTER filter	{ $$ = $1; $$->filter = $3; }
 | mrt_dump_args where_filter	{ $$ = $1; $$->filter = $2; }
 | mrt_dump_args TO text 	{ $$ = $1; $$->filename = $3; }
 ;


#line 209 "proto/ospf/config.Y"
/* Grammar from proto/ospf/config.Y */

proto: ospf_proto '}' { ospf_proto_finish(); }  ;

ospf_variant:
   OSPF    { $$ = 1; }
 | OSPF V2 { $$ = 1; }
 | OSPF V3 { $$ = 0; }
 ;

ospf_proto_start: proto_start ospf_variant
{
  this_proto = proto_config_new(&proto_ospf, $1);
  this_proto->net_type = $2 ? NET_IP4 : 0;

  init_list(&OSPF_CFG->area_list);
  init_list(&OSPF_CFG->vlink_list);
  OSPF_CFG->ecmp = rt_default_ecmp;
  OSPF_CFG->tick = OSPF_DEFAULT_TICK;
  OSPF_CFG->ospf2 = $2;
  OSPF_CFG->af_ext = !$2;
  OSPF_CFG->gr_mode = OSPF_GR_AWARE;
  OSPF_CFG->gr_time = OSPF_DEFAULT_GR_TIME;
};

ospf_proto:
   ospf_proto_start proto_name '{'
 | ospf_proto ospf_proto_item ';'
 ;

ospf_af_mc:
             { $$ = 0; }
 | MULTICAST { $$ = 1; }
 ;

/* We redefine proto_channel to add multicast flag */
ospf_channel_start: net_type ospf_af_mc
{
  /* TODO: change name for multicast channels */
  $$ = this_channel = channel_config_get(NULL, net_label[$1], $1, this_proto);

  /* Save the multicast flag */
  OSPF_CFG->af_mc = $2;
};

ospf_channel: ospf_channel_start channel_opt_list channel_end;

ospf_proto_item:
   proto_item
 | ospf_channel { this_proto->net_type = $1->net_type; }
 | RFC1583COMPAT bool { OSPF_CFG->rfc1583 = $2; }
 | RFC5838 bool { OSPF_CFG->af_ext = $2; if (!ospf_cfg_is_v3()) cf_error("RFC5838 option requires OSPFv3"); }
 | VPN PE bool { OSPF_CFG->vpn_pe = $3; }
 | STUB ROUTER bool { OSPF_CFG->stub_router = $3; }
 | GRACEFUL RESTART bool { OSPF_CFG->gr_mode = $3; }
 | GRACEFUL RESTART AWARE { OSPF_CFG->gr_mode = OSPF_GR_AWARE; }
 | GRACEFUL RESTART TIME expr { OSPF_CFG->gr_time = $4; if (($4 < 1) || ($4 > 1800)) cf_error("Graceful restart time must be in range 1-1800"); }
 | ECMP bool { OSPF_CFG->ecmp = $2 ? OSPF_DEFAULT_ECMP_LIMIT : 0; }
 | ECMP bool LIMIT expr { OSPF_CFG->ecmp = $2 ? $4 : 0; }
 | MERGE EXTERNAL bool { OSPF_CFG->merge_external = $3; }
 | TICK expr { OSPF_CFG->tick = $2; if($2 <= 0) cf_error("Tick must be greater than zero"); }
 | INSTANCE ID expr { OSPF_CFG->instance_id = $3; OSPF_CFG->instance_id_set = 1; if ($3 > 255) cf_error("Instance ID must be in range 0-255"); }
 | ospf_area
 ;

ospf_area_start: AREA idval {
  this_area = cfg_allocz(sizeof(struct ospf_area_config));
  add_tail(&OSPF_CFG->area_list, NODE this_area);
  this_area->areaid = $2;
  this_area->default_cost = OSPF_DEFAULT_STUB_COST;
  this_area->type = OPT_E;
  this_area->transint = OSPF_DEFAULT_TRANSINT;

  init_list(&this_area->patt_list);
  init_list(&this_area->net_list);
  init_list(&this_area->enet_list);
  init_list(&this_area->stubnet_list);
 }
 ;

ospf_area: ospf_area_start '{' ospf_area_opts '}' { ospf_area_finish(); }
 ;

ospf_area_opts:
   /* empty */
 | ospf_area_opts ospf_area_item ';'
 ;

ospf_area_item:
   STUB bool { this_area->type = $2 ? 0 : OPT_E; /* We should remove the option */ }
 | NSSA { this_area->type = OPT_N; }
 | SUMMARY bool { this_area->summary = $2; }
 | DEFAULT NSSA bool { this_area->default_nssa = $3; }
 | DEFAULT COST expr { this_area->default_cost = $3; ospf_check_defcost($3); }
 | DEFAULT COST2 expr { this_area->default_cost = $3 | LSA_EXT3_EBIT; ospf_check_defcost($3); }
 | STUB COST expr { this_area->default_cost = $3; ospf_check_defcost($3); }
 | TRANSLATOR bool { this_area->translator = $2; }
 | TRANSLATOR STABILITY expr { this_area->transint = $3; }
 | NETWORKS { this_nets = &this_area->net_list; } '{' pref_list '}'
 | EXTERNAL { this_nets = &this_area->enet_list; } '{' pref_list '}'
 | STUBNET ospf_stubnet
 | INTERFACE ospf_iface
 | ospf_vlink
 ;

ospf_stubnet:
   ospf_stubnet_start '{' ospf_stubnet_opts '}'
 | ospf_stubnet_start
 ;

ospf_stubnet_start:
   net_ip {
     this_stubnet = cfg_allocz(sizeof(struct ospf_stubnet_config));
     add_tail(&this_area->stubnet_list, NODE this_stubnet);
     this_stubnet->prefix = $1;
     this_stubnet->cost = COST_D;
   }
 ;

ospf_stubnet_opts:
   /* empty */
 | ospf_stubnet_opts ospf_stubnet_item ';'
 ;

ospf_stubnet_item:
   HIDDEN bool { this_stubnet->hidden = $2; }
 | SUMMARY bool { this_stubnet->summary = $2; }
 | COST expr { this_stubnet->cost = $2; }
 ;

ospf_vlink:
   ospf_vlink_start ospf_instance_id '{' ospf_vlink_opts '}' { ospf_iface_finish(); }
 | ospf_vlink_start ospf_instance_id { ospf_iface_finish(); }
 ;

ospf_vlink_opts:
   /* empty */
 | ospf_vlink_opts ospf_vlink_item ';'
 ;

ospf_vlink_item:
 | HELLO expr { OSPF_PATT->helloint = $2 ; if (($2<=0) || ($2>65535)) cf_error("Hello interval must be in range 1-65535"); }
 | RETRANSMIT expr { OSPF_PATT->rxmtint = $2 ; if ($2<=1) cf_error("Retransmit int must be greater than one"); }
 | TRANSMIT DELAY expr { OSPF_PATT->inftransdelay = $3 ; if (($3<=0) || ($3>65535)) cf_error("Transmit delay must be in range 1-65535"); }
 | WAIT expr { OSPF_PATT->waitint = $2 ; if ($2<=1) cf_error("Wait interval must be greater than one"); }
 | DEAD expr { OSPF_PATT->deadint = $2 ; if ($2<=1) cf_error("Dead interval must be greater than one"); }
 | DEAD COUNT expr { OSPF_PATT->deadc = $3 ; if ($3<=1) cf_error("Dead count must be greater than one"); }
 | AUTHENTICATION NONE { OSPF_PATT->autype = OSPF_AUTH_NONE;  }
 | AUTHENTICATION SIMPLE { OSPF_PATT->autype = OSPF_AUTH_SIMPLE; ospf_check_auth(); }
 | AUTHENTICATION CRYPTOGRAPHIC { OSPF_PATT->autype = OSPF_AUTH_CRYPT; }
 | password_list
 ;

ospf_vlink_start: VIRTUAL LINK idval
 {
  if (this_area->areaid == 0) cf_error("Virtual link cannot be in backbone");
  this_ipatt = cfg_allocz(sizeof(struct ospf_iface_patt));
  add_tail(&OSPF_CFG->vlink_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);
  OSPF_PATT->voa = this_area->areaid;
  OSPF_PATT->vid = $3;
  OSPF_PATT->helloint = HELLOINT_D;
  OSPF_PATT->rxmtint = RXMTINT_D;
  OSPF_PATT->inftransdelay = INFTRANSDELAY_D;
  OSPF_PATT->deadc = DEADC_D;
  OSPF_PATT->type = OSPF_IT_VLINK;
  init_list(&OSPF_PATT->nbma_list);
  reset_passwords();
 }
;

ospf_iface_item:
   COST expr { OSPF_PATT->cost = $2 ; if (($2<=0) || ($2>65535)) cf_error("Cost must be in range 1-65535"); }
 | HELLO expr { OSPF_PATT->helloint = $2 ; if (($2<=0) || ($2>65535)) cf_error("Hello interval must be in range 1-65535"); }
 | POLL expr { OSPF_PATT->pollint = $2 ; if ($2<=0) cf_error("Poll int must be greater than zero"); }
 | RETRANSMIT expr { OSPF_PATT->rxmtint = $2 ; if ($2<=1) cf_error("Retransmit int must be greater than one"); }
 | WAIT expr { OSPF_PATT->waitint = $2 ; if ($2<=1) cf_error("Wait interval must be greater than one"); }
 | DEAD expr { OSPF_PATT->deadint = $2 ; if ($2<=1) cf_error("Dead interval must be greater than one"); }
 | DEAD COUNT expr { OSPF_PATT->deadc = $3 ; if ($3<=1) cf_error("Dead count must be greater than one"); }
 | TYPE BROADCAST { OSPF_PATT->type = OSPF_IT_BCAST ; }
 | TYPE BCAST { OSPF_PATT->type = OSPF_IT_BCAST ; }
 | TYPE NONBROADCAST { OSPF_PATT->type = OSPF_IT_NBMA ; }
 | TYPE NBMA { OSPF_PATT->type = OSPF_IT_NBMA ; }
 | TYPE POINTOPOINT { OSPF_PATT->type = OSPF_IT_PTP ; }
 | TYPE PTP { OSPF_PATT->type = OSPF_IT_PTP ; }
 | TYPE POINTOMULTIPOINT { OSPF_PATT->type = OSPF_IT_PTMP ; }
 | TYPE PTMP { OSPF_PATT->type = OSPF_IT_PTMP ; }
 | REAL BROADCAST bool { OSPF_PATT->real_bcast = $3; if (!ospf_cfg_is_v2()) cf_error("Real broadcast option requires OSPFv2"); }
 | PTP NETMASK bool { OSPF_PATT->ptp_netmask = $3; if (!ospf_cfg_is_v2()) cf_error("PtP netmask option requires OSPFv2"); }
 | PTP ADDRESS bool { OSPF_PATT->ptp_address = $3; if (!ospf_cfg_is_v2()) cf_error("PtP address option requires OSPFv2"); }
 | TRANSMIT DELAY expr { OSPF_PATT->inftransdelay = $3 ; if (($3<=0) || ($3>65535)) cf_error("Transmit delay must be in range 1-65535"); }
 | PRIORITY expr { OSPF_PATT->priority = $2 ; if ($2>255) cf_error("Priority must be in range 0-255"); }
 | STRICT NONBROADCAST bool { OSPF_PATT->strictnbma = $3 ; }
 | STUB bool { OSPF_PATT->stub = $2 ; }
 | CHECK LINK bool { OSPF_PATT->check_link = $3; }
 | ECMP WEIGHT expr { OSPF_PATT->ecmp_weight = $3 - 1; if (($3<1) || ($3>256)) cf_error("ECMP weight must be in range 1-256"); }
 | LINK LSA SUPPRESSION bool { OSPF_PATT->link_lsa_suppression = $4; if (!ospf_cfg_is_v3()) cf_error("Link LSA suppression option requires OSPFv3"); }
 | NEIGHBORS '{' nbma_list '}'
 | AUTHENTICATION NONE { OSPF_PATT->autype = OSPF_AUTH_NONE; }
 | AUTHENTICATION SIMPLE { OSPF_PATT->autype = OSPF_AUTH_SIMPLE; ospf_check_auth(); }
 | AUTHENTICATION CRYPTOGRAPHIC { OSPF_PATT->autype = OSPF_AUTH_CRYPT; }
 | RX BUFFER NORMAL { OSPF_PATT->rx_buffer = 0; }
 | RX BUFFER LARGE { OSPF_PATT->rx_buffer = OSPF_MAX_PKT_SIZE; }
 | RX BUFFER expr { OSPF_PATT->rx_buffer = $3; if (($3 < OSPF_MIN_PKT_SIZE) || ($3 > OSPF_MAX_PKT_SIZE)) cf_error("Buffer size must be in range 256-65535"); }
 | TX tos { OSPF_PATT->tx_tos = $2; }
 | TX PRIORITY expr { OSPF_PATT->tx_priority = $3; }
 | TX LENGTH expr { OSPF_PATT->tx_length = $3; if (($3 < OSPF_MIN_PKT_SIZE) || ($3 > OSPF_MAX_PKT_SIZE)) cf_error("TX length must be in range 256-65535"); }
 | TTL SECURITY bool { OSPF_PATT->ttl_security = $3; }
 | TTL SECURITY TX ONLY { OSPF_PATT->ttl_security = 2; }
 | BFD bool { OSPF_PATT->bfd = $2; cf_check_bfd($2); }
 | password_list
 ;

pref_list:
 /* empty */
 | pref_list pref_item
 ;

pref_item: pref_base pref_opt ';' ;

pref_base: net_ip
 {
   this_pref = cfg_allocz(sizeof(struct area_net_config));
   add_tail(this_nets, NODE this_pref);
   this_pref->prefix = $1;
 }
;

pref_opt:
 /* empty */
 | HIDDEN { this_pref->hidden = 1; }
 | TAG expr { this_pref->tag = $2; }
 ;

nbma_list:
 /* empty */
 | nbma_list nbma_item
 ;

nbma_eligible:
 /* empty */ { $$ = 0; }
 | ELIGIBLE { $$ = 1; }
 ;

nbma_item: ipa nbma_eligible ';'
 {
   this_nbma = cfg_allocz(sizeof(struct nbma_node));
   add_tail(&OSPF_PATT->nbma_list, NODE this_nbma);
   this_nbma->ip=$1;
   this_nbma->eligible=$2;
 }
;

ospf_iface_start:
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
;

ospf_instance_id:
   /* empty */
 | INSTANCE expr { OSPF_PATT->instance_id = $2; OSPF_PATT->instance_id_set = 1; if ($2 > 255) cf_error("Instance ID must be in range 0-255"); }
 ;

ospf_iface_patt_list:
   iface_patt_list { if (ospf_cfg_is_v3()) iface_patt_check(); } ospf_instance_id
 ;

ospf_iface_opts:
   /* empty */
 | ospf_iface_opts ospf_iface_item ';'
 ;

ospf_iface_opt_list:
   /* empty */
 | '{' ospf_iface_opts '}'
 ;

ospf_iface:
  ospf_iface_start ospf_iface_patt_list ospf_iface_opt_list { ospf_iface_finish(); }
 ;

dynamic_attr: OSPF_METRIC1 { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_OSPF_METRIC1); } ;
dynamic_attr: OSPF_METRIC2 { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_OSPF_METRIC2); } ;
dynamic_attr: OSPF_TAG { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_OSPF_TAG); } ;
dynamic_attr: OSPF_ROUTER_ID { $$ = f_new_dynamic_attr(EAF_TYPE_ROUTER_ID, T_QUAD, EA_OSPF_ROUTER_ID); } ;

;
cli_cmd: cmd_SHOW_OSPF
#line 514 "proto/ospf/config.Y"
cmd_SHOW_OSPF: SHOW OSPF optproto END
{ PROTO_WALK_CMD($3, &proto_ospf, p) ospf_sh(p); };

cli_cmd: cmd_SHOW_OSPF_NEIGHBORS
#line 517 "proto/ospf/config.Y"
cmd_SHOW_OSPF_NEIGHBORS: SHOW OSPF NEIGHBORS optproto opttext END
{ PROTO_WALK_CMD($4, &proto_ospf, p) ospf_sh_neigh(p, $5); };

cli_cmd: cmd_SHOW_OSPF_INTERFACE
#line 520 "proto/ospf/config.Y"
cmd_SHOW_OSPF_INTERFACE: SHOW OSPF INTERFACE optproto opttext END
{ PROTO_WALK_CMD($4, &proto_ospf, p) ospf_sh_iface(p, $5); };



#line 525 "proto/ospf/config.Y"
cli_cmd: cmd_SHOW_OSPF_TOPOLOGY
#line 525
cmd_SHOW_OSPF_TOPOLOGY: SHOW OSPF TOPOLOGY optproto opttext END
{ ospf_sh_state(proto_get_named($4, &proto_ospf), 0, 1); };

cli_cmd: cmd_SHOW_OSPF_TOPOLOGY_ALL
#line 528 "proto/ospf/config.Y"
cmd_SHOW_OSPF_TOPOLOGY_ALL: SHOW OSPF TOPOLOGY ALL optproto opttext END
{ ospf_sh_state(proto_get_named($5, &proto_ospf), 0, 0); };



#line 533 "proto/ospf/config.Y"
cli_cmd: cmd_SHOW_OSPF_STATE
#line 533
cmd_SHOW_OSPF_STATE: SHOW OSPF STATE optproto opttext END
{ ospf_sh_state(proto_get_named($4, &proto_ospf), 1, 1); };

cli_cmd: cmd_SHOW_OSPF_STATE_ALL
#line 536 "proto/ospf/config.Y"
cmd_SHOW_OSPF_STATE_ALL: SHOW OSPF STATE ALL optproto opttext END
{ ospf_sh_state(proto_get_named($5, &proto_ospf), 1, 0); };

;
cli_cmd: cmd_SHOW_OSPF_LSADB
#line 540 "proto/ospf/config.Y"
cmd_SHOW_OSPF_LSADB: SHOW OSPF LSADB lsadb_args END
{
  if (!$4->proto)
    $4->proto = (struct ospf_proto *) proto_get_named(NULL, &proto_ospf);

  ospf_sh_lsadb($4);
};

lsadb_args:
   /* empty */ {
     $$ = cfg_allocz(sizeof(struct lsadb_show_data));
   }
 | lsadb_args GLOBAL { $$ = $1; $$->scope = LSA_SCOPE_AS; }
 | lsadb_args AREA idval { $$ = $1; $$->scope = LSA_SCOPE_AREA; $$->area = $3; }
 | lsadb_args LINK { $$ = $1; $$->scope = 1; /* hack, 0 is no filter */ }
 | lsadb_args TYPE NUM { $$ = $1; $$->type = $3; }
 | lsadb_args LSID idval { $$ = $1; $$->lsid = $3; }
 | lsadb_args SELF { $$ = $1; $$->router = SH_ROUTER_SELF; }
 | lsadb_args ROUTER idval { $$ = $1; $$->router = $3; }
 | lsadb_args CF_SYM_KNOWN { cf_assert_symbol($2, SYM_PROTO); $$ = $1; $$->proto = (struct ospf_proto *) proto_get_named($2, &proto_ospf); }
 ;

#line 22 "proto/perf/config.Y"
/* Grammar from proto/perf/config.Y */

proto: perf_proto '}' ;

perf_proto_start: proto_start PERF
{
  this_proto = proto_config_new(&proto_perf, $1);
  PERF_CFG->from = 10;
  PERF_CFG->to = 20;
  PERF_CFG->repeat = 4;
  PERF_CFG->threshold_max = 500 MS_;
  PERF_CFG->threshold_min = 1 MS_;
  PERF_CFG->attrs_per_rte = 0;
  PERF_CFG->keep = 0;
  PERF_CFG->mode = PERF_MODE_IMPORT;
};

perf_proto:
   perf_proto_start proto_name '{'
 | perf_proto perf_proto_item ';'
 | perf_proto proto_item ';'
 ;

perf_proto_item:
   proto_channel { this_proto->net_type = $1->net_type; }
 | EXP FROM NUM { PERF_CFG->from = $3; }
 | EXP TO NUM { PERF_CFG->to = $3; }
 | REPEAT NUM { PERF_CFG->repeat = $2; }
 | THRESHOLD MIN expr_us { PERF_CFG->threshold_min = $3; }
 | THRESHOLD MAX expr_us { PERF_CFG->threshold_max = $3; }
 | ATTRIBUTES NUM { PERF_CFG->attrs_per_rte = $2; }
 | KEEP bool { PERF_CFG->keep = $2; }
 | MODE IMPORT { PERF_CFG->mode = PERF_MODE_IMPORT; }
 | MODE EXPORT { PERF_CFG->mode = PERF_MODE_EXPORT; }
;


#line 21 "proto/pipe/config.Y"
/* Grammar from proto/pipe/config.Y */

proto: pipe_proto '}' { this_channel = NULL; }  ;

pipe_proto_start: proto_start PIPE
{
  this_proto = proto_config_new(&proto_pipe, $1);
}
proto_name
{
  this_channel = proto_cf_main_channel(this_proto);
  if (!this_channel) {
    this_channel = channel_config_new(NULL, NULL, 0, this_proto);
    this_channel->in_filter = FILTER_ACCEPT;
    this_channel->out_filter = FILTER_ACCEPT;
  }
};

pipe_proto:
   pipe_proto_start '{'
 | pipe_proto proto_item ';'
 | pipe_proto channel_item_ ';'
 | pipe_proto PEER TABLE rtable ';' { PIPE_CFG->peer = $4; }
 ;

#line 42 "proto/radv/config.Y"
/* Grammar from proto/radv/config.Y */

proto: radv_proto ;

radv_proto_start: proto_start RADV
{
  this_proto = proto_config_new(&proto_radv, $1);
  this_proto->net_type = NET_IP6;

  init_list(&RADV_CFG->patt_list);
  init_list(&RADV_CFG->pref_list);
  init_list(&RADV_CFG->rdnss_list);
  init_list(&RADV_CFG->dnssl_list);
};

radv_proto_item:
   proto_item
 | proto_channel
 | INTERFACE radv_iface
 | PREFIX radv_prefix { add_tail(&RADV_CFG->pref_list, NODE this_radv_prefix); }
 | RDNSS { init_list(&radv_dns_list); } radv_rdnss { add_tail_list(&RADV_CFG->rdnss_list, &radv_dns_list); }
 | DNSSL { init_list(&radv_dns_list); } radv_dnssl { add_tail_list(&RADV_CFG->dnssl_list, &radv_dns_list); }
 | TRIGGER net_ip6 { RADV_CFG->trigger = $2; }
 | PROPAGATE ROUTES bool { RADV_CFG->propagate_routes = $3; }
 ;

radv_proto_opts:
   /* empty */
 | radv_proto_opts radv_proto_item ';'
 ;

radv_proto:
   radv_proto_start proto_name '{' radv_proto_opts '}';


radv_iface_start:
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
};

radv_iface_item:
   MIN RA INTERVAL expr { RADV_IFACE->min_ra_int = $4; if ($4 < 3) cf_error("Min RA interval must be at least 3"); }
 | MAX RA INTERVAL expr { RADV_IFACE->max_ra_int = $4; if (($4 < 4) || ($4 > 1800)) cf_error("Max RA interval must be in range 4-1800"); }
 | MIN DELAY expr { RADV_IFACE->min_delay = $3; if ($3 <= 0) cf_error("Min delay must be positive"); }
 | SOLICITED RA UNICAST bool { RADV_IFACE->solicited_ra_unicast = $4; }
 | MANAGED bool { RADV_IFACE->managed = $2; }
 | OTHER CONFIG bool { RADV_IFACE->other_config = $3; }
 | LINK MTU expr { RADV_IFACE->link_mtu = $3; }
 | REACHABLE TIME expr { RADV_IFACE->reachable_time = $3; if ($3 > 3600000) cf_error("Reachable time must be in range 0-3600000"); }
 | RETRANS TIMER expr { RADV_IFACE->retrans_timer = $3; }
 | CURRENT HOP LIMIT expr { RADV_IFACE->current_hop_limit = $4; if ($4 > 255) cf_error("Current hop limit must be in range 0-255"); }
 | DEFAULT LIFETIME expr radv_sensitive {
     RADV_IFACE->default_lifetime = $3;
     if ($3 > 9000)  cf_error("Default lifetime must be in range 0-9000");
     if ($4 != (uint) -1) RADV_IFACE->default_lifetime_sensitive = $4;
   }
 | ROUTE LIFETIME expr radv_sensitive {
     RADV_IFACE->route_lifetime = $3;
     if ($4 != (uint) -1) RADV_IFACE->route_lifetime_sensitive = $4;
   }
 | DEFAULT PREFERENCE radv_preference { RADV_IFACE->default_preference = $3; }
 | ROUTE PREFERENCE radv_preference { RADV_IFACE->route_preference = $3; }
 | PREFIX LINGER TIME expr { RADV_IFACE->prefix_linger_time = $4; }
 | ROUTE LINGER TIME expr { RADV_IFACE->route_linger_time = $4; }
 | PREFIX radv_prefix { add_tail(&RADV_IFACE->pref_list, NODE this_radv_prefix); }
 | RDNSS { init_list(&radv_dns_list); } radv_rdnss { add_tail_list(&RADV_IFACE->rdnss_list, &radv_dns_list); }
 | DNSSL { init_list(&radv_dns_list); } radv_dnssl { add_tail_list(&RADV_IFACE->dnssl_list, &radv_dns_list); }
 | RDNSS LOCAL bool { RADV_IFACE->rdnss_local = $3; }
 | DNSSL LOCAL bool { RADV_IFACE->dnssl_local = $3; }
 ;

radv_preference:
   LOW { $$ = RA_PREF_LOW; }
 | MEDIUM { $$ = RA_PREF_MEDIUM; }
 | HIGH { $$ = RA_PREF_HIGH; }

radv_iface_finish:
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
};


radv_iface_opts:
   /* empty */
 | radv_iface_opts radv_iface_item ';'
 ;

radv_iface_opt_list:
   /* empty */
 | '{' radv_iface_opts '}'
 ;

radv_iface:
  radv_iface_start iface_patt_list_nopx radv_iface_opt_list radv_iface_finish;


radv_prefix_start: net_ip6
{
  this_radv_prefix = cfg_allocz(sizeof(struct radv_prefix_config));
  RADV_PREFIX->prefix = *(net_addr_ip6 *) &($1);

  RADV_PREFIX->onlink = 1;
  RADV_PREFIX->autonomous = 1;
  RADV_PREFIX->valid_lifetime = DEFAULT_VALID_LIFETIME;
  RADV_PREFIX->preferred_lifetime = DEFAULT_PREFERRED_LIFETIME;
};

radv_prefix_item:
   SKIP bool { RADV_PREFIX->skip = $2; }
 | ONLINK bool { RADV_PREFIX->onlink = $2; }
 | AUTONOMOUS bool { RADV_PREFIX->autonomous = $2; }
 | VALID LIFETIME expr radv_sensitive {
     RADV_PREFIX->valid_lifetime = $3;
     if ($4 != (uint) -1) RADV_PREFIX->valid_lifetime_sensitive = $4;
   }
 | PREFERRED LIFETIME expr radv_sensitive {
     RADV_PREFIX->preferred_lifetime = $3;
     if ($4 != (uint) -1) RADV_PREFIX->preferred_lifetime_sensitive = $4;
   }
 ;

radv_prefix_finish:
{
  if (RADV_PREFIX->preferred_lifetime > RADV_PREFIX->valid_lifetime)
    cf_error("Preferred lifetime must be at most Valid lifetime");

  if (RADV_PREFIX->valid_lifetime_sensitive > RADV_PREFIX->preferred_lifetime_sensitive)
    cf_error("Valid lifetime sensitive requires that Preferred lifetime is sensitive too");
};

radv_prefix_opts:
   /* empty */
 | radv_prefix_opts radv_prefix_item ';'
 ;

radv_prefix_opt_list:
   /* empty */
 | '{' radv_prefix_opts '}'
 ;

radv_prefix:
  radv_prefix_start radv_prefix_opt_list radv_prefix_finish;



radv_rdnss_node: ipa
{
  struct radv_rdnss_config *cf = cfg_allocz(sizeof(struct radv_rdnss_config));
  add_tail(&radv_dns_list, NODE cf);

  cf->server = $1;
  cf->lifetime_mult = DEFAULT_DNS_LIFETIME_MULT;
};

radv_rdnss_start:
{
  RADV_RDNSS->lifetime = 0;
  RADV_RDNSS->lifetime_mult = DEFAULT_DNS_LIFETIME_MULT;
};

radv_rdnss_item:
 | NS radv_rdnss_node
 | LIFETIME radv_mult { RADV_RDNSS->lifetime = $2; RADV_RDNSS->lifetime_mult = radv_mult_val; }
 ;

radv_rdnss_finish:
{
  if (EMPTY_LIST(radv_dns_list))
    cf_error("No nameserver in RDNSS section");

  struct radv_rdnss_config *cf;
  WALK_LIST(cf, radv_dns_list)
  {
    cf->lifetime = RADV_RDNSS->lifetime;
    cf->lifetime_mult = RADV_RDNSS->lifetime_mult;
  }
};

radv_rdnss_opts:
   /* empty */
 | radv_rdnss_opts radv_rdnss_item ';'
 ;

radv_rdnss:
   radv_rdnss_node
 | '{' radv_rdnss_start radv_rdnss_opts '}' radv_rdnss_finish
 ;


radv_dnssl_node: TEXT
{
  struct radv_dnssl_config *cf = cfg_allocz(sizeof(struct radv_dnssl_config));
  add_tail(&radv_dns_list, NODE cf);

  cf->domain = $1;
  cf->lifetime_mult = DEFAULT_DNS_LIFETIME_MULT;

  if (radv_process_domain(cf) < 0)
    cf_error("Invalid domain dame");
};

radv_dnssl_start:
{
  RADV_DNSSL->lifetime = 0;
  RADV_DNSSL->lifetime_mult = DEFAULT_DNS_LIFETIME_MULT;
};

radv_dnssl_item:
 | DOMAIN radv_dnssl_node
 | LIFETIME radv_mult { RADV_DNSSL->lifetime = $2; RADV_DNSSL->lifetime_mult = radv_mult_val; }
 ;

radv_dnssl_finish:
{
  if (EMPTY_LIST(radv_dns_list))
    cf_error("No domain in DNSSL section");

  struct radv_dnssl_config *cf;
  WALK_LIST(cf, radv_dns_list)
  {
    cf->lifetime = RADV_DNSSL->lifetime;
    cf->lifetime_mult = RADV_DNSSL->lifetime_mult;
  }
};

radv_dnssl_opts:
   /* empty */
 | radv_dnssl_opts radv_dnssl_item ';'
 ;

radv_dnssl:
   radv_dnssl_node
 | '{' radv_dnssl_start radv_dnssl_opts '}' radv_dnssl_finish
 ;


radv_mult:
   expr { $$ = $1; radv_mult_val = 0; }
 | MULT expr { $$ = 0; radv_mult_val = $2; if (($2 < 1) || ($2 > 254)) cf_error("Multiplier must be in range 1-254"); }
 ;

radv_sensitive:
   /* empty */ { $$ = (uint) -1; }
 | SENSITIVE bool { $$ = $2; }
 ;

dynamic_attr: RA_PREFERENCE { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_ENUM_RA_PREFERENCE, EA_RA_PREFERENCE); } ;
dynamic_attr: RA_LIFETIME { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_RA_LIFETIME); } ;

#line 44 "proto/rip/config.Y"
/* Grammar from proto/rip/config.Y */

proto: rip_proto ;

rip_variant:
   RIP    { $$ = 1; }
 | RIP NG { $$ = 0; }
 ;

rip_proto_start: proto_start rip_variant
{
  this_proto = proto_config_new(&proto_rip, $1);
  this_proto->net_type = $2 ? NET_IP4 : NET_IP6;

  init_list(&RIP_CFG->patt_list);
  RIP_CFG->rip2 = $2;
  RIP_CFG->ecmp = rt_default_ecmp;
  RIP_CFG->infinity = RIP_DEFAULT_INFINITY;
  RIP_CFG->min_timeout_time = 60 S_;
  RIP_CFG->max_garbage_time = 60 S_;
};

rip_proto_item:
   proto_item
 | proto_channel
 | ECMP bool		{ RIP_CFG->ecmp = $2 ? RIP_DEFAULT_ECMP_LIMIT : 0; }
 | ECMP bool LIMIT expr	{ RIP_CFG->ecmp = $2 ? $4 : 0; }
 | INFINITY expr	{ RIP_CFG->infinity = $2; }
 | INTERFACE rip_iface
 ;

rip_proto_opts:
   /* empty */
 | rip_proto_opts rip_proto_item ';'
 ;

rip_proto:
   rip_proto_start proto_name '{' rip_proto_opts '}';


rip_iface_start:
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
};

rip_iface_finish:
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
};

rip_iface_item:
   METRIC expr		{ RIP_IFACE->metric = $2; if (($2<1) || ($2>255)) cf_error("Metric must be in range 1-255"); }
 | MODE MULTICAST	{ RIP_IFACE->mode = RIP_IM_MULTICAST; }
 | MODE BROADCAST	{ RIP_IFACE->mode = RIP_IM_BROADCAST; if (rip_cfg_is_ng()) cf_error("Broadcast not supported in RIPng"); }
 | PASSIVE bool		{ RIP_IFACE->passive = $2; }
 | ADDRESS ipa		{ RIP_IFACE->address = $2; if (ipa_is_ip4($2) != rip_cfg_is_v2()) cf_error("IP address version mismatch"); }
 | PORT expr		{ RIP_IFACE->port = $2; if (($2<1) || ($2>65535)) cf_error("Invalid port number"); }
 | VERSION expr		{ RIP_IFACE->version = $2;
			  if (rip_cfg_is_ng()) cf_error("Version not supported in RIPng");
			  if (($2 != RIP_V1) && ($2 != RIP_V2)) cf_error("Unsupported version");
			}
 | VERSION ONLY bool	{ RIP_IFACE->version_only = $3; }
 | SPLIT HORIZON bool	{ RIP_IFACE->split_horizon = $3; }
 | POISON REVERSE bool	{ RIP_IFACE->poison_reverse = $3; }
 | CHECK ZERO bool	{ RIP_IFACE->check_zero = $3; }
 | DEMAND CIRCUIT bool	{ RIP_IFACE->demand_circuit = $3; }
 | UPDATE TIME expr	{ RIP_IFACE->update_time = $3 S_; if ($3<=0) cf_error("Update time must be positive"); }
 | TIMEOUT TIME expr	{ RIP_IFACE->timeout_time = $3 S_; if ($3<=0) cf_error("Timeout time must be positive"); }
 | GARBAGE TIME expr	{ RIP_IFACE->garbage_time = $3 S_; if ($3<=0) cf_error("Garbage time must be positive"); }
 | RETRANSMIT TIME expr_us { RIP_IFACE->rxmt_time = $3; if ($3<=0) cf_error("Retransmit time must be positive"); }
 | ECMP WEIGHT expr	{ RIP_IFACE->ecmp_weight = $3 - 1; if (($3<1) || ($3>256)) cf_error("ECMP weight must be in range 1-256"); }
 | RX BUFFER expr	{ RIP_IFACE->rx_buffer = $3; if (($3<256) || ($3>65535)) cf_error("RX length must be in range 256-65535"); }
 | TX LENGTH expr	{ RIP_IFACE->tx_length = $3; if (($3<256) || ($3>65535)) cf_error("TX length must be in range 256-65535"); }
 | TX tos		{ RIP_IFACE->tx_tos = $2; }
 | TX PRIORITY expr	{ RIP_IFACE->tx_priority = $3; }
 | TTL SECURITY bool	{ RIP_IFACE->ttl_security = $3; }
 | TTL SECURITY TX ONLY	{ RIP_IFACE->ttl_security = 2; }
 | CHECK LINK bool	{ RIP_IFACE->check_link = $3; }
 | BFD bool		{ RIP_IFACE->bfd = $2; cf_check_bfd($2); }
 | AUTHENTICATION rip_auth { RIP_IFACE->auth_type = $2; if ($2) rip_check_auth(); }
 | password_list	{ rip_check_auth(); }
;

rip_auth:
   NONE			{ $$ = RIP_AUTH_NONE; }
 | PLAINTEXT		{ $$ = RIP_AUTH_PLAIN; }
 | CRYPTOGRAPHIC	{ $$ = RIP_AUTH_CRYPTO; }
 | MD5			{ $$ = RIP_AUTH_CRYPTO; }	/* For backward compatibility */
 ;

rip_iface_opts:
   /* empty */
 | rip_iface_opts rip_iface_item ';'
 ;

rip_iface_opt_list:
   /* empty */
 | '{' rip_iface_opts '}'
 ;

rip_iface:
  rip_iface_start iface_patt_list_nopx rip_iface_opt_list rip_iface_finish;


dynamic_attr: RIP_METRIC { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_RIP_METRIC); } ;
dynamic_attr: RIP_TAG { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_RIP_TAG); } ;

;

cli_cmd: cmd_SHOW_RIP_INTERFACES
#line 198 "proto/rip/config.Y"
cmd_SHOW_RIP_INTERFACES: SHOW RIP INTERFACES optproto opttext END
{ PROTO_WALK_CMD($4, &proto_rip, p) rip_show_interfaces(p, $5); };

cli_cmd: cmd_SHOW_RIP_NEIGHBORS
#line 201 "proto/rip/config.Y"
cmd_SHOW_RIP_NEIGHBORS: SHOW RIP NEIGHBORS optproto opttext END
{ PROTO_WALK_CMD($4, &proto_rip, p) rip_show_neighbors(p, $5); };


#line 39 "proto/rpki/config.Y"
/* Grammar from proto/rpki/config.Y */

proto: rpki_proto ;

rpki_proto_start: proto_start RPKI {
  this_proto = proto_config_new(&proto_rpki, $1);
  RPKI_CFG->retry_interval = RPKI_RETRY_INTERVAL;
  RPKI_CFG->refresh_interval = RPKI_REFRESH_INTERVAL;
  RPKI_CFG->expire_interval = RPKI_EXPIRE_INTERVAL;
};

rpki_proto: rpki_proto_start proto_name '{' rpki_proto_opts '}' { rpki_check_config(RPKI_CFG); };

rpki_proto_opts:
   /* empty */
 | rpki_proto_opts rpki_proto_item ';'
 ;

rpki_proto_item:
   proto_item
 | proto_channel
 | REMOTE rpki_cache_addr
 | REMOTE rpki_cache_addr rpki_proto_item_port
 | rpki_proto_item_port
 | TRANSPORT rpki_transport
 | REFRESH rpki_keep_interval expr {
     if (rpki_check_refresh_interval($3))
       cf_error(rpki_check_refresh_interval($3));
     RPKI_CFG->refresh_interval = $3;
     RPKI_CFG->keep_refresh_interval = $2;
   }
 | RETRY rpki_keep_interval expr {
     if (rpki_check_retry_interval($3))
       cf_error(rpki_check_retry_interval($3));
     RPKI_CFG->retry_interval = $3;
     RPKI_CFG->keep_retry_interval = $2;
   }
 | EXPIRE rpki_keep_interval expr {
     if (rpki_check_expire_interval($3))
       cf_error(rpki_check_expire_interval($3));
     RPKI_CFG->expire_interval = $3;
     RPKI_CFG->keep_expire_interval = $2;
   }
 | IGNORE MAX LENGTH bool { RPKI_CFG->ignore_max_length = $4; }
 ;

rpki_keep_interval:
 /* empty */ { $$ = 0; }
 | KEEP { $$ = 1; }
 ;

rpki_proto_item_port: PORT expr { check_u16($2); RPKI_CFG->port = $2; };

rpki_cache_addr:
   text {
     rpki_check_unused_hostname();
     RPKI_CFG->hostname = $1;
   }
 | ipa {
     rpki_check_unused_hostname();
     RPKI_CFG->ip = $1;
     /* Ensure hostname is filled */
     char *hostname = cfg_allocz(INET6_ADDRSTRLEN + 1);
     bsnprintf(hostname, INET6_ADDRSTRLEN+1, "%I", RPKI_CFG->ip);
     RPKI_CFG->hostname = hostname;
   }
 ;

rpki_transport:
   TCP rpki_transport_tcp_init
 | SSH rpki_transport_ssh_init '{' rpki_transport_ssh_opts '}' rpki_transport_ssh_check
 ;

rpki_transport_tcp_init:
{
  rpki_check_unused_transport();
  RPKI_CFG->tr_config.spec = cfg_allocz(sizeof(struct rpki_tr_tcp_config));
  RPKI_CFG->tr_config.type = RPKI_TR_TCP;
};

rpki_transport_ssh_init:
{
#if HAVE_LIBSSH
  rpki_check_unused_transport();
  RPKI_CFG->tr_config.spec = cfg_allocz(sizeof(struct rpki_tr_ssh_config));
  RPKI_CFG->tr_config.type = RPKI_TR_SSH;
#else
  cf_error("This build doesn't support SSH");
#endif
};

rpki_transport_ssh_opts:
   /* empty */
 | rpki_transport_ssh_opts rpki_transport_ssh_item ';'
 ;

rpki_transport_ssh_item:
   BIRD PRIVATE KEY text  { RPKI_TR_SSH_CFG->bird_private_key = $4; }
 | REMOTE PUBLIC KEY text { RPKI_TR_SSH_CFG->cache_public_key = $4; }
 | USER text              { RPKI_TR_SSH_CFG->user = $2; }
 ;

rpki_transport_ssh_check:
{
  if (RPKI_TR_SSH_CFG->user == NULL)
    cf_error("User must be set");
};

#line 52 "proto/static/config.Y"
/* Grammar from proto/static/config.Y */

proto: static_proto '}' ;

static_proto_start: proto_start STATIC
{
  this_proto = proto_config_new(&proto_static, $1);
  init_list(&STATIC_CFG->routes);
};

static_proto:
   static_proto_start proto_name '{'
 | static_proto proto_item ';'
 | static_proto proto_channel ';' { this_proto->net_type = $2->net_type; }
 | static_proto CHECK LINK bool ';' { STATIC_CFG->check_link = $4; }
 | static_proto IGP TABLE rtable ';' {
    if ($4->addr_type == NET_IP4)
      STATIC_CFG->igp_table_ip4 = $4;
    else if ($4->addr_type == NET_IP6)
      STATIC_CFG->igp_table_ip6 = $4;
    else
      cf_error("Incompatible IGP table type");
   }
 | static_proto stat_route stat_route_opt_list ';' { static_route_finish(); }
 ;

stat_nexthop:
    VIA ipa ipa_scope {
      this_snh = static_nexthop_new();
      this_snh->via = $2;
      this_snh->iface = $3;
    }
  | VIA TEXT {
      this_snh = static_nexthop_new();
      this_snh->via = IPA_NONE;
      this_snh->iface = if_get_by_name($2);
    }
  | stat_nexthop MPLS label_stack {
    this_snh->mls = $3;
  }
  | stat_nexthop ONLINK bool {
    this_snh->onlink = $3;
  }
  | stat_nexthop WEIGHT expr {
    this_snh->weight = $3 - 1;
    if (($3<1) || ($3>256)) cf_error("Weight must be in range 1-256");
  }
  | stat_nexthop BFD bool {
    this_snh->use_bfd = $3; cf_check_bfd($3);
  }
;

stat_nexthops:
    stat_nexthop
  | stat_nexthops stat_nexthop
;

stat_route0: ROUTE net_any {
     this_srt = cfg_allocz(sizeof(struct static_route));
     add_tail(&STATIC_CFG->routes, &this_srt->n);
     this_srt->net = $2;
     this_srt_cmds = NULL;
     this_srt_last_cmd = NULL;
     this_srt->mp_next = NULL;
     this_snh = NULL;
  }
 ;

stat_route:
   stat_route0 stat_nexthops
 | stat_route0 RECURSIVE ipa {
      this_srt->dest = RTDX_RECURSIVE;
      this_srt->via = $3;
   }
 | stat_route0 RECURSIVE ipa MPLS label_stack {
      this_srt->dest = RTDX_RECURSIVE;
      this_srt->via = $3;
      this_srt->mls = $5;
   }
 | stat_route0			{ this_srt->dest = RTD_NONE; }
 | stat_route0 DROP		{ this_srt->dest = RTD_BLACKHOLE; }
 | stat_route0 REJECT		{ this_srt->dest = RTD_UNREACHABLE; }
 | stat_route0 BLACKHOLE	{ this_srt->dest = RTD_BLACKHOLE; }
 | stat_route0 UNREACHABLE	{ this_srt->dest = RTD_UNREACHABLE; }
 | stat_route0 PROHIBIT		{ this_srt->dest = RTD_PROHIBIT; }
 ;

stat_route_item:
   cmd {
     if (this_srt_last_cmd)
       this_srt_last_cmd->next = $1;
     else
       this_srt_cmds = $1;
     this_srt_last_cmd = $1;
   }
 ;

stat_route_opts:
   /* empty */
 | stat_route_opts stat_route_item
 ;

stat_route_opt_list:
   /* empty */
 | '{' stat_route_opts '}'
 ;


cli_cmd: cmd_SHOW_STATIC
#line 160 "proto/static/config.Y"
cmd_SHOW_STATIC: SHOW STATIC optproto END
{ PROTO_WALK_CMD($3, &proto_static, p) static_show(p); } ;

#line 21 "sysdep/linux/netlink.Y"
/* Grammar from sysdep/linux/netlink.Y */

kern_proto: kern_proto kern_sys_item ';' ;

kern_sys_item:
   KERNEL TABLE expr { THIS_KRT->sys.table_id = $3; }
 | METRIC expr { THIS_KRT->sys.metric = $2; }
 | NETLINK RX BUFFER expr { THIS_KRT->sys.netlink_rx_buffer = $4; }
 ;

dynamic_attr: KRT_PREFSRC	{ $$ = f_new_dynamic_attr(EAF_TYPE_IP_ADDRESS, T_IP, EA_KRT_PREFSRC); } ;
dynamic_attr: KRT_REALM	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_REALM); } ;
dynamic_attr: KRT_SCOPE	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_SCOPE); } ;

dynamic_attr: KRT_MTU		{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_MTU); } ;
dynamic_attr: KRT_WINDOW	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_WINDOW); } ;
dynamic_attr: KRT_RTT		{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_RTT); } ;
dynamic_attr: KRT_RTTVAR	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_RTTVAR); } ;
dynamic_attr: KRT_SSTRESH	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_SSTRESH); } ;
dynamic_attr: KRT_CWND		{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_CWND); } ;
dynamic_attr: KRT_ADVMSS	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_ADVMSS); } ;
dynamic_attr: KRT_REORDERING	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_REORDERING); } ;
dynamic_attr: KRT_HOPLIMIT	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_HOPLIMIT); } ;
dynamic_attr: KRT_INITCWND	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_INITCWND); } ;
dynamic_attr: KRT_RTO_MIN	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_RTO_MIN); } ;
dynamic_attr: KRT_INITRWND	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_INITRWND); } ;
dynamic_attr: KRT_QUICKACK	{ $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_QUICKACK); } ;

/* Bits of EA_KRT_LOCK, based on RTAX_* constants */

dynamic_attr: KRT_LOCK_MTU	{ $$ = f_new_dynamic_attr_bit(2, T_BOOL, EA_KRT_LOCK); } ;
dynamic_attr: KRT_LOCK_WINDOW	{ $$ = f_new_dynamic_attr_bit(3, T_BOOL, EA_KRT_LOCK); } ;
dynamic_attr: KRT_LOCK_RTT	{ $$ = f_new_dynamic_attr_bit(4, T_BOOL, EA_KRT_LOCK); } ;
dynamic_attr: KRT_LOCK_RTTVAR	{ $$ = f_new_dynamic_attr_bit(5, T_BOOL, EA_KRT_LOCK); } ;
dynamic_attr: KRT_LOCK_SSTRESH	{ $$ = f_new_dynamic_attr_bit(6, T_BOOL, EA_KRT_LOCK); } ;
dynamic_attr: KRT_LOCK_CWND	{ $$ = f_new_dynamic_attr_bit(7, T_BOOL, EA_KRT_LOCK); } ;
dynamic_attr: KRT_LOCK_ADVMSS	{ $$ = f_new_dynamic_attr_bit(8, T_BOOL, EA_KRT_LOCK); } ;
dynamic_attr: KRT_LOCK_REORDERING { $$ = f_new_dynamic_attr_bit(9, T_BOOL, EA_KRT_LOCK); } ;
dynamic_attr: KRT_LOCK_HOPLIMIT { $$ = f_new_dynamic_attr_bit(10, T_BOOL, EA_KRT_LOCK); } ;
dynamic_attr: KRT_LOCK_RTO_MIN { $$ = f_new_dynamic_attr_bit(13, T_BOOL, EA_KRT_LOCK); } ;

dynamic_attr: KRT_FEATURE_ECN	{ $$ = f_new_dynamic_attr_bit(0, T_BOOL, EA_KRT_FEATURES); } ;
dynamic_attr: KRT_FEATURE_ALLFRAG { $$ = f_new_dynamic_attr(3, T_BOOL, EA_KRT_FEATURES); } ;


#line 29 "sysdep/unix/config.Y"
/* Grammar from sysdep/unix/config.Y */

conf: log_config ;

log_begin: { this_log = cfg_allocz(sizeof(struct log_config)); };

log_config: LOG log_begin log_file log_mask ';' {
    this_log->mask = $4;
    add_tail(&new_config->logfiles, &this_log->n);
  }
 ;

syslog_name:
   NAME text { $$ = $2; }
 | { $$ = bird_name; }
 ;

log_limit:
   /* empty */
 | expr text { this_log->limit = $1; this_log->backup = $2; }
 ;

log_file:
   text log_limit {
     if (!parse_and_exit)
     {
       this_log->rf = rf_open(new_config->pool, $1, "a");
       if (!this_log->rf) cf_error("Unable to open log file '%s': %m", $1);
       this_log->fh = rf_file(this_log->rf);
     }
     this_log->pos = -1;
     this_log->filename = $1;
   }
 | SYSLOG syslog_name { this_log->fh = NULL; new_config->syslog_name = $2; }
 | STDERR { this_log->fh = stderr; }
 ;

log_mask:
   ALL { $$ = ~0; }
 | '{' log_mask_list '}' { $$ = $2; }
 ;

log_mask_list:
   log_cat { $$ = 1 << $1; }
 | log_mask_list ',' log_cat { $$ = $1 | (1 << $3); }
 ;

log_cat:
   DEBUG { $$ = L_DEBUG[0]; }
 | TRACE { $$ = L_TRACE[0]; }
 | INFO { $$ = L_INFO[0]; }
 | REMOTE { $$ = L_REMOTE[0]; }
 | WARNING { $$ = L_WARN[0]; }
 | ERROR { $$ = L_ERR[0]; }
 | AUTH { $$ = L_AUTH[0]; }
 | FATAL { $$ = L_FATAL[0]; }
 | BUG { $$ = L_BUG[0]; }
 ;


conf: mrtdump_base ;

mrtdump_base:
   MRTDUMP PROTOCOLS mrtdump_mask ';' { new_config->proto_default_mrtdump = $3; }
 | MRTDUMP text ';' {
     if (!parse_and_exit)
     {
       struct rfile *f = rf_open(new_config->pool, $2, "a");
       if (!f) cf_error("Unable to open MRTDump file '%s': %m", $2);
       new_config->mrtdump_file = rf_fileno(f);
     }
   }
 ;


conf: debug_unix ;

debug_unix:
   DEBUG LATENCY bool { new_config->latency_debug = $3; }
 | DEBUG LATENCY LIMIT expr_us { new_config->latency_limit = $4; }
 | WATCHDOG WARNING expr_us { new_config->watchdog_warning = $3; }
 | WATCHDOG TIMEOUT expr_us { new_config->watchdog_timeout = ($3 + 999999) TO_S; }
 ;


/* Unix specific commands */



#line 118 "sysdep/unix/config.Y"
cli_cmd: cmd_CONFIGURE
#line 118
cmd_CONFIGURE: CONFIGURE cfg_name cfg_timeout END
{ cmd_reconfig($2, RECONFIG_HARD, $3); } ;

cli_cmd: cmd_CONFIGURE_SOFT
#line 121 "sysdep/unix/config.Y"
cmd_CONFIGURE_SOFT: CONFIGURE SOFT cfg_name cfg_timeout END
{ cmd_reconfig($3, RECONFIG_SOFT, $4); } ;

/* Hack to get input completion for 'timeout' */



cli_cmd: cmd_CONFIGURE_CONFIRM
#line 128 "sysdep/unix/config.Y"
cmd_CONFIGURE_CONFIRM: CONFIGURE CONFIRM  END
{ cmd_reconfig_confirm(); } ;

cli_cmd: cmd_CONFIGURE_UNDO
#line 131 "sysdep/unix/config.Y"
cmd_CONFIGURE_UNDO: CONFIGURE UNDO  END
{ cmd_reconfig_undo(); } ;

cli_cmd: cmd_CONFIGURE_STATUS
#line 134 "sysdep/unix/config.Y"
cmd_CONFIGURE_STATUS: CONFIGURE STATUS  END
{ cmd_reconfig_status(); } ;

cli_cmd: cmd_CONFIGURE_CHECK
#line 137 "sysdep/unix/config.Y"
cmd_CONFIGURE_CHECK: CONFIGURE CHECK cfg_name END
{ cmd_check_config($3); } ;

#line 140 "sysdep/unix/config.Y"
cli_cmd: cmd_DOWN
#line 140
cmd_DOWN: DOWN  END
{ cmd_shutdown(); } ;



cli_cmd: cmd_GRACEFUL_RESTART
#line 145 "sysdep/unix/config.Y"
cmd_GRACEFUL_RESTART: GRACEFUL RESTART  END
{ cmd_graceful_restart(); } ;


cfg_name:
   /* empty */ { $$ = NULL; }
 | TEXT
 ;

cfg_timeout:
   /* empty */ { $$ = 0; }
 | TIMEOUT { $$ = UNIX_DEFAULT_CONFIGURE_TIMEOUT; }
 | TIMEOUT expr { $$ = $2; }
 ;

#line 37 "sysdep/unix/krt.Y"
/* Grammar from sysdep/unix/krt.Y */

/* Kernel syncer protocol */

proto: kern_proto '}' ;

kern_proto_start: proto_start KERNEL {
     this_proto = krt_init_config($1);
}
 ;

kern_proto: kern_proto_start proto_name '{' ;
kern_proto: kern_proto kern_item ';' ;

kern_mp_limit:
   /* empty */ { $$ = KRT_DEFAULT_ECMP_LIMIT; }
 | LIMIT expr  { $$ = $2; if (($2 <= 0) || ($2 > 255)) cf_error("Merge paths limit must be in range 1-255"); }
 ;

kern_item:
   proto_item
 | proto_channel { this_proto->net_type = $1->net_type; }
 | PERSIST bool { THIS_KRT->persist = $2; }
 | SCAN TIME expr {
      /* Scan time of 0 means scan on startup only */
      THIS_KRT->scan_time = $3 S_;
   }
 | LEARN bool {
      THIS_KRT->learn = $2;
#ifndef KRT_ALLOW_LEARN
      if ($2)
	cf_error("Learning of kernel routes not supported on this platform");
#endif
   }
 | GRACEFUL RESTART bool { THIS_KRT->graceful_restart = $3; }
 | MERGE PATHS bool kern_mp_limit {
      THIS_KRT->merge_paths = $3 ? $4 : 0;
#ifndef KRT_ALLOW_MERGE_PATHS
      if ($3)
	cf_error("Path merging not supported on this platform");
#endif
   }
 ;

/* Kernel interface protocol */

proto: kif_proto '}' ;

kif_proto_start: proto_start DEVICE { this_proto = kif_init_config($1); }
 ;

kif_proto: kif_proto_start proto_name '{' ;
kif_proto: kif_proto kif_item ';' ;

kif_item:
   proto_item
 | INTERFACE kif_iface
 | SCAN TIME expr {
      /* Scan time of 0 means scan on startup only */
      THIS_KIF->scan_time = $3 S_;
   }
 ;

kif_iface_start:
{
  this_ipatt = cfg_allocz(sizeof(struct kif_iface_config));
  add_tail(&THIS_KIF->iface_list, NODE this_ipatt);
  init_list(&this_ipatt->ipn_list);
}

kif_iface_item:
   PREFERRED ipa { kif_set_preferred($2); }
 ;

kif_iface_opts:
   /* empty */
 | kif_iface_opts kif_iface_item ';'
 ;

kif_iface_opt_list:
   /* empty */
 | '{' kif_iface_opts '}'
 ;

kif_iface:
  kif_iface_start iface_patt_list_nopx kif_iface_opt_list;


dynamic_attr: KRT_SOURCE { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_SOURCE); } ;
dynamic_attr: KRT_METRIC { $$ = f_new_dynamic_attr(EAF_TYPE_INT, T_INT, EA_KRT_METRIC); } ;

#line 50 "conf/gen_parser.m4"

#line 50
%%
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

