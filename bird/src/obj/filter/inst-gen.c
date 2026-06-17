#line 459 "filter/decl.m4"


#if defined(__GNUC__) && __GNUC__ >= 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#endif

#include "nest/bird.h"
#include "filter/filter.h"
#include "filter/f-inst.h"

/* Instruction codes to string */
static const char * const f_instruction_name_str[] = {
#line 284 "filter/f-inst.c"
 /* Code enum to string for FI_ADD */
#line 284
  [FI_ADD] = "FI_ADD",
#line 289 "filter/f-inst.c"
 /* Code enum to string for FI_SUBTRACT */
#line 289
  [FI_SUBTRACT] = "FI_SUBTRACT",
#line 294 "filter/f-inst.c"
 /* Code enum to string for FI_MULTIPLY */
#line 294
  [FI_MULTIPLY] = "FI_MULTIPLY",
#line 300 "filter/f-inst.c"
 /* Code enum to string for FI_DIVIDE */
#line 300
  [FI_DIVIDE] = "FI_DIVIDE",
#line 310 "filter/f-inst.c"
 /* Code enum to string for FI_AND */
#line 310
  [FI_AND] = "FI_AND",
#line 321 "filter/f-inst.c"
 /* Code enum to string for FI_OR */
#line 321
  [FI_OR] = "FI_OR",
#line 331 "filter/f-inst.c"
 /* Code enum to string for FI_PAIR_CONSTRUCT */
#line 331
  [FI_PAIR_CONSTRUCT] = "FI_PAIR_CONSTRUCT",
#line 371 "filter/f-inst.c"
 /* Code enum to string for FI_EC_CONSTRUCT */
#line 371
  [FI_EC_CONSTRUCT] = "FI_EC_CONSTRUCT",
#line 378 "filter/f-inst.c"
 /* Code enum to string for FI_LC_CONSTRUCT */
#line 378
  [FI_LC_CONSTRUCT] = "FI_LC_CONSTRUCT",
#line 428 "filter/f-inst.c"
 /* Code enum to string for FI_PATHMASK_CONSTRUCT */
#line 428
  [FI_PATHMASK_CONSTRUCT] = "FI_PATHMASK_CONSTRUCT",
#line 435 "filter/f-inst.c"
 /* Code enum to string for FI_NEQ */
#line 435
  [FI_NEQ] = "FI_NEQ",
#line 442 "filter/f-inst.c"
 /* Code enum to string for FI_EQ */
#line 442
  [FI_EQ] = "FI_EQ",
#line 453 "filter/f-inst.c"
 /* Code enum to string for FI_LT */
#line 453
  [FI_LT] = "FI_LT",
#line 464 "filter/f-inst.c"
 /* Code enum to string for FI_LTE */
#line 464
  [FI_LTE] = "FI_LTE",
#line 469 "filter/f-inst.c"
 /* Code enum to string for FI_NOT */
#line 469
  [FI_NOT] = "FI_NOT",
#line 478 "filter/f-inst.c"
 /* Code enum to string for FI_MATCH */
#line 478
  [FI_MATCH] = "FI_MATCH",
#line 487 "filter/f-inst.c"
 /* Code enum to string for FI_NOT_MATCH */
#line 487
  [FI_NOT_MATCH] = "FI_NOT_MATCH",
#line 492 "filter/f-inst.c"
 /* Code enum to string for FI_DEFINED */
#line 492
  [FI_DEFINED] = "FI_DEFINED",
#line 504 "filter/f-inst.c"
 /* Code enum to string for FI_TYPE */
#line 504
  [FI_TYPE] = "FI_TYPE",
#line 509 "filter/f-inst.c"
 /* Code enum to string for FI_IS_V4 */
#line 509
  [FI_IS_V4] = "FI_IS_V4",
#line 522 "filter/f-inst.c"
 /* Code enum to string for FI_VAR_INIT */
#line 522
  [FI_VAR_INIT] = "FI_VAR_INIT",
#line 531 "filter/f-inst.c"
 /* Code enum to string for FI_VAR_SET */
#line 531
  [FI_VAR_SET] = "FI_VAR_SET",
#line 538 "filter/f-inst.c"
 /* Code enum to string for FI_VAR_GET */
#line 538
  [FI_VAR_GET] = "FI_VAR_GET",
#line 551 "filter/f-inst.c"
 /* Code enum to string for FI_CONSTANT */
#line 551
  [FI_CONSTANT] = "FI_CONSTANT",
#line 585 "filter/f-inst.c"
 /* Code enum to string for FI_FOR_INIT */
#line 585
  [FI_FOR_INIT] = "FI_FOR_INIT",
#line 639 "filter/f-inst.c"
 /* Code enum to string for FI_FOR_NEXT */
#line 639
  [FI_FOR_NEXT] = "FI_FOR_NEXT",
#line 647 "filter/f-inst.c"
 /* Code enum to string for FI_CONDITION */
#line 647
  [FI_CONDITION] = "FI_CONDITION",
#line 656 "filter/f-inst.c"
 /* Code enum to string for FI_PRINT */
#line 656
  [FI_PRINT] = "FI_PRINT",
#line 663 "filter/f-inst.c"
 /* Code enum to string for FI_FLUSH */
#line 663
  [FI_FLUSH] = "FI_FLUSH",
#line 677 "filter/f-inst.c"
 /* Code enum to string for FI_DIE */
#line 677
  [FI_DIE] = "FI_DIE",
#line 705 "filter/f-inst.c"
 /* Code enum to string for FI_RTA_GET */
#line 705
  [FI_RTA_GET] = "FI_RTA_GET",
#line 820 "filter/f-inst.c"
 /* Code enum to string for FI_RTA_SET */
#line 820
  [FI_RTA_SET] = "FI_RTA_SET",
#line 892 "filter/f-inst.c"
 /* Code enum to string for FI_EA_GET */
#line 892
  [FI_EA_GET] = "FI_EA_GET",
#line 959 "filter/f-inst.c"
 /* Code enum to string for FI_EA_SET */
#line 959
  [FI_EA_SET] = "FI_EA_SET",
#line 968 "filter/f-inst.c"
 /* Code enum to string for FI_EA_UNSET */
#line 968
  [FI_EA_UNSET] = "FI_EA_UNSET",
#line 980 "filter/f-inst.c"
 /* Code enum to string for FI_LENGTH */
#line 980
  [FI_LENGTH] = "FI_LENGTH",
#line 1015 "filter/f-inst.c"
 /* Code enum to string for FI_NET_SRC */
#line 1015
  [FI_NET_SRC] = "FI_NET_SRC",
#line 1050 "filter/f-inst.c"
 /* Code enum to string for FI_NET_DST */
#line 1050
  [FI_NET_DST] = "FI_NET_DST",
#line 1060 "filter/f-inst.c"
 /* Code enum to string for FI_ROA_MAXLEN */
#line 1060
  [FI_ROA_MAXLEN] = "FI_ROA_MAXLEN",
#line 1087 "filter/f-inst.c"
 /* Code enum to string for FI_ASN */
#line 1087
  [FI_ASN] = "FI_ASN",
#line 1092 "filter/f-inst.c"
 /* Code enum to string for FI_IP */
#line 1092
  [FI_IP] = "FI_IP",
#line 1099 "filter/f-inst.c"
 /* Code enum to string for FI_ROUTE_DISTINGUISHER */
#line 1099
  [FI_ROUTE_DISTINGUISHER] = "FI_ROUTE_DISTINGUISHER",
#line 1106 "filter/f-inst.c"
 /* Code enum to string for FI_AS_PATH_FIRST */
#line 1106
  [FI_AS_PATH_FIRST] = "FI_AS_PATH_FIRST",
#line 1113 "filter/f-inst.c"
 /* Code enum to string for FI_AS_PATH_LAST */
#line 1113
  [FI_AS_PATH_LAST] = "FI_AS_PATH_LAST",
#line 1118 "filter/f-inst.c"
 /* Code enum to string for FI_AS_PATH_LAST_NAG */
#line 1118
  [FI_AS_PATH_LAST_NAG] = "FI_AS_PATH_LAST_NAG",
#line 1123 "filter/f-inst.c"
 /* Code enum to string for FI_PAIR_DATA */
#line 1123
  [FI_PAIR_DATA] = "FI_PAIR_DATA",
#line 1128 "filter/f-inst.c"
 /* Code enum to string for FI_LC_DATA1 */
#line 1128
  [FI_LC_DATA1] = "FI_LC_DATA1",
#line 1133 "filter/f-inst.c"
 /* Code enum to string for FI_LC_DATA2 */
#line 1133
  [FI_LC_DATA2] = "FI_LC_DATA2",
#line 1167 "filter/f-inst.c"
 /* Code enum to string for FI_MIN */
#line 1167
  [FI_MIN] = "FI_MIN",
#line 1201 "filter/f-inst.c"
 /* Code enum to string for FI_MAX */
#line 1201
  [FI_MAX] = "FI_MAX",
#line 1227 "filter/f-inst.c"
 /* Code enum to string for FI_RETURN */
#line 1227
  [FI_RETURN] = "FI_RETURN",
#line 1288 "filter/f-inst.c"
 /* Code enum to string for FI_CALL */
#line 1288
  [FI_CALL] = "FI_CALL",
#line 1293 "filter/f-inst.c"
 /* Code enum to string for FI_DROP_RESULT */
#line 1293
  [FI_DROP_RESULT] = "FI_DROP_RESULT",
#line 1334 "filter/f-inst.c"
 /* Code enum to string for FI_SWITCH */
#line 1334
  [FI_SWITCH] = "FI_SWITCH",
#line 1342 "filter/f-inst.c"
 /* Code enum to string for FI_IP_MASK */
#line 1342
  [FI_IP_MASK] = "FI_IP_MASK",
#line 1348 "filter/f-inst.c"
 /* Code enum to string for FI_PATH_PREPEND */
#line 1348
  [FI_PATH_PREPEND] = "FI_PATH_PREPEND",
#line 1405 "filter/f-inst.c"
 /* Code enum to string for FI_CLIST_ADD */
#line 1405
  [FI_CLIST_ADD] = "FI_CLIST_ADD",
#line 1460 "filter/f-inst.c"
 /* Code enum to string for FI_CLIST_DEL */
#line 1460
  [FI_CLIST_DEL] = "FI_CLIST_DEL",
#line 1506 "filter/f-inst.c"
 /* Code enum to string for FI_CLIST_FILTER */
#line 1506
  [FI_CLIST_FILTER] = "FI_CLIST_FILTER",
#line 1536 "filter/f-inst.c"
 /* Code enum to string for FI_ROA_CHECK_IMPLICIT */
#line 1536
  [FI_ROA_CHECK_IMPLICIT] = "FI_ROA_CHECK_IMPLICIT",
#line 1558 "filter/f-inst.c"
 /* Code enum to string for FI_ROA_CHECK_EXPLICIT */
#line 1558
  [FI_ROA_CHECK_EXPLICIT] = "FI_ROA_CHECK_EXPLICIT",
#line 1563 "filter/f-inst.c"
 /* Code enum to string for FI_FORMAT */
#line 1563
  [FI_FORMAT] = "FI_FORMAT",
#line 696 "filter/decl.m4"
 /* Code enum to string for FI_ASSERT */
#line 696
  [FI_ASSERT] = "FI_ASSERT",
#line 472 "filter/decl.m4"

};

const char *
f_instruction_name_(enum f_instruction_code fi)
{
  if (fi < (sizeof(f_instruction_name_str) / sizeof(f_instruction_name_str[0])))
    return f_instruction_name_str[fi];
  else
    bug("Got unknown instruction code: %d", fi);
}

static inline struct f_inst *
fi_new(enum f_instruction_code fi_code)
{
  struct f_inst *what = tmp_allocz(sizeof(struct f_inst));
  what->lineno = ifs->lino;
  what->size = 1;
  what->fi_code = fi_code;
  return what;
}

static inline struct f_inst *
fi_constant(struct f_inst *what, struct f_val val)
{
  what->fi_code = FI_CONSTANT;
  what->i_FI_CONSTANT.val = val;
  return what;
}

static int
f_const_promotion(struct f_inst *arg, enum f_type want)
{
  if (arg->fi_code != FI_CONSTANT)
    return 0;

  struct f_val *c = &arg->i_FI_CONSTANT.val;

  if ((c->type == T_IP) && ipa_is_ip4(c->val.ip) && (want == T_QUAD)) {
    *c = (struct f_val) {
      .type = T_QUAD,
      .val.i = ipa_to_u32(c->val.ip),
    };
    return 1;
  }

  else if ((c->type == T_SET) && (!c->val.t) && (want == T_PREFIX_SET)) {
    *c = f_const_empty_prefix_set;
    return 1;
  }

  return 0;
}

#define v1 whati->f1->i_FI_CONSTANT.val
#define v2 whati->f2->i_FI_CONSTANT.val
#define v3 whati->f3->i_FI_CONSTANT.val
#define vv(i) items[i]->i_FI_CONSTANT.val
#define runtime(fmt, ...) cf_error("filter preevaluation, line %d: " fmt, ifs->lino, ##__VA_ARGS__)
#define fpool cfg_mem
#define falloc(size) cfg_alloc(size)
/* Instruction constructors */
#line 284 "filter/f-inst.c"
 /* Constructor for FI_ADD */
#line 284
struct f_inst *
#line 280 "filter/f-inst.c"
NONNULL(2)
#line 281 "filter/f-inst.c"
NONNULL(3)
#line 284 "filter/f-inst.c"
f_new_inst_FI_ADD(enum f_instruction_code fi_code
#line 280 "filter/f-inst.c"
  , struct f_inst * f1
#line 281 "filter/f-inst.c"
  , struct f_inst * f2
#line 284 "filter/f-inst.c"
)
#line 284
  {
#line 284
    /* Allocate the structure */
#line 284
    struct f_inst *what = fi_new(fi_code);
#line 284
    uint constargs = 1;
#line 284

#line 284
    /* Initialize all the members */
#line 284
  #define whati (&(what->i_FI_ADD))
#line 284
  #line 280 "filter/f-inst.c"
whati->f1 = f1;
#line 280
const struct f_inst *child1 = f1;
#line 280
do {
#line 280
  what->size += child1->size;
#line 280

#line 280
  if (child1->fi_code != FI_CONSTANT)
#line 280
    constargs = 0;
#line 280

#line 280
} while (child1 = child1->next);
#line 280 "filter/f-inst.c"
if (f1->type && (f1->type != (T_INT)) && !f_const_promotion(f1, (T_INT)))
#line 280
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 280
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f1->type));
#line 281 "filter/f-inst.c"
whati->f2 = f2;
#line 281
const struct f_inst *child2 = f2;
#line 281
do {
#line 281
  what->size += child2->size;
#line 281

#line 281
  if (child2->fi_code != FI_CONSTANT)
#line 281
    constargs = 0;
#line 281

#line 281
} while (child2 = child2->next);
#line 281 "filter/f-inst.c"
if (f2->type && (f2->type != (T_INT)) && !f_const_promotion(f2, (T_INT)))
#line 281
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 281
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f2->type));
#line 282 "filter/f-inst.c"
what->type = T_INT;

#line 284 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 284
    if (!constargs)
#line 284
      return what;
#line 284

#line 284
    /* Try to pre-calculate the result */
#line 284
    #line 279 "filter/f-inst.c"
 {
    
#line 280 "filter/f-inst.c"
 
#line 280 "filter/f-inst.c"
 
#line 280 "filter/f-inst.c"
;
    
#line 281 "filter/f-inst.c"
 
#line 281 "filter/f-inst.c"
 
#line 281 "filter/f-inst.c"
;
     
#line 282 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = v1.val.i + v2.val.i } );
  }
  
  #undef whati
#line 284 "filter/f-inst.c"
  }
#line 284

#line 284

#line 289 "filter/f-inst.c"
 /* Constructor for FI_SUBTRACT */
#line 289
struct f_inst *
#line 285 "filter/f-inst.c"
NONNULL(2)
#line 286 "filter/f-inst.c"
NONNULL(3)
#line 289 "filter/f-inst.c"
f_new_inst_FI_SUBTRACT(enum f_instruction_code fi_code
#line 285 "filter/f-inst.c"
  , struct f_inst * f1
#line 286 "filter/f-inst.c"
  , struct f_inst * f2
#line 289 "filter/f-inst.c"
)
#line 289
  {
#line 289
    /* Allocate the structure */
#line 289
    struct f_inst *what = fi_new(fi_code);
#line 289
    uint constargs = 1;
#line 289

#line 289
    /* Initialize all the members */
#line 289
  #define whati (&(what->i_FI_SUBTRACT))
#line 289
  #line 285 "filter/f-inst.c"
whati->f1 = f1;
#line 285
const struct f_inst *child1 = f1;
#line 285
do {
#line 285
  what->size += child1->size;
#line 285

#line 285
  if (child1->fi_code != FI_CONSTANT)
#line 285
    constargs = 0;
#line 285

#line 285
} while (child1 = child1->next);
#line 285 "filter/f-inst.c"
if (f1->type && (f1->type != (T_INT)) && !f_const_promotion(f1, (T_INT)))
#line 285
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 285
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f1->type));
#line 286 "filter/f-inst.c"
whati->f2 = f2;
#line 286
const struct f_inst *child2 = f2;
#line 286
do {
#line 286
  what->size += child2->size;
#line 286

#line 286
  if (child2->fi_code != FI_CONSTANT)
#line 286
    constargs = 0;
#line 286

#line 286
} while (child2 = child2->next);
#line 286 "filter/f-inst.c"
if (f2->type && (f2->type != (T_INT)) && !f_const_promotion(f2, (T_INT)))
#line 286
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 286
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f2->type));
#line 287 "filter/f-inst.c"
what->type = T_INT;

#line 289 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 289
    if (!constargs)
#line 289
      return what;
#line 289

#line 289
    /* Try to pre-calculate the result */
#line 289
    #line 284 "filter/f-inst.c"
 {
    
#line 285 "filter/f-inst.c"
 
#line 285 "filter/f-inst.c"
 
#line 285 "filter/f-inst.c"
;
    
#line 286 "filter/f-inst.c"
 
#line 286 "filter/f-inst.c"
 
#line 286 "filter/f-inst.c"
;
     
#line 287 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = v1.val.i - v2.val.i } );
  }
  
  #undef whati
#line 289 "filter/f-inst.c"
  }
#line 289

#line 289

#line 294 "filter/f-inst.c"
 /* Constructor for FI_MULTIPLY */
#line 294
struct f_inst *
#line 290 "filter/f-inst.c"
NONNULL(2)
#line 291 "filter/f-inst.c"
NONNULL(3)
#line 294 "filter/f-inst.c"
f_new_inst_FI_MULTIPLY(enum f_instruction_code fi_code
#line 290 "filter/f-inst.c"
  , struct f_inst * f1
#line 291 "filter/f-inst.c"
  , struct f_inst * f2
#line 294 "filter/f-inst.c"
)
#line 294
  {
#line 294
    /* Allocate the structure */
#line 294
    struct f_inst *what = fi_new(fi_code);
#line 294
    uint constargs = 1;
#line 294

#line 294
    /* Initialize all the members */
#line 294
  #define whati (&(what->i_FI_MULTIPLY))
#line 294
  #line 290 "filter/f-inst.c"
whati->f1 = f1;
#line 290
const struct f_inst *child1 = f1;
#line 290
do {
#line 290
  what->size += child1->size;
#line 290

#line 290
  if (child1->fi_code != FI_CONSTANT)
#line 290
    constargs = 0;
#line 290

#line 290
} while (child1 = child1->next);
#line 290 "filter/f-inst.c"
if (f1->type && (f1->type != (T_INT)) && !f_const_promotion(f1, (T_INT)))
#line 290
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 290
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f1->type));
#line 291 "filter/f-inst.c"
whati->f2 = f2;
#line 291
const struct f_inst *child2 = f2;
#line 291
do {
#line 291
  what->size += child2->size;
#line 291

#line 291
  if (child2->fi_code != FI_CONSTANT)
#line 291
    constargs = 0;
#line 291

#line 291
} while (child2 = child2->next);
#line 291 "filter/f-inst.c"
if (f2->type && (f2->type != (T_INT)) && !f_const_promotion(f2, (T_INT)))
#line 291
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 291
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f2->type));
#line 292 "filter/f-inst.c"
what->type = T_INT;

#line 294 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 294
    if (!constargs)
#line 294
      return what;
#line 294

#line 294
    /* Try to pre-calculate the result */
#line 294
    #line 289 "filter/f-inst.c"
 {
    
#line 290 "filter/f-inst.c"
 
#line 290 "filter/f-inst.c"
 
#line 290 "filter/f-inst.c"
;
    
#line 291 "filter/f-inst.c"
 
#line 291 "filter/f-inst.c"
 
#line 291 "filter/f-inst.c"
;
     
#line 292 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = v1.val.i * v2.val.i } );
  }
  
  #undef whati
#line 294 "filter/f-inst.c"
  }
#line 294

#line 294

#line 300 "filter/f-inst.c"
 /* Constructor for FI_DIVIDE */
#line 300
struct f_inst *
#line 295 "filter/f-inst.c"
NONNULL(2)
#line 296 "filter/f-inst.c"
NONNULL(3)
#line 300 "filter/f-inst.c"
f_new_inst_FI_DIVIDE(enum f_instruction_code fi_code
#line 295 "filter/f-inst.c"
  , struct f_inst * f1
#line 296 "filter/f-inst.c"
  , struct f_inst * f2
#line 300 "filter/f-inst.c"
)
#line 300
  {
#line 300
    /* Allocate the structure */
#line 300
    struct f_inst *what = fi_new(fi_code);
#line 300
    uint constargs = 1;
#line 300

#line 300
    /* Initialize all the members */
#line 300
  #define whati (&(what->i_FI_DIVIDE))
#line 300
  #line 295 "filter/f-inst.c"
whati->f1 = f1;
#line 295
const struct f_inst *child1 = f1;
#line 295
do {
#line 295
  what->size += child1->size;
#line 295

#line 295
  if (child1->fi_code != FI_CONSTANT)
#line 295
    constargs = 0;
#line 295

#line 295
} while (child1 = child1->next);
#line 295 "filter/f-inst.c"
if (f1->type && (f1->type != (T_INT)) && !f_const_promotion(f1, (T_INT)))
#line 295
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 295
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f1->type));
#line 296 "filter/f-inst.c"
whati->f2 = f2;
#line 296
const struct f_inst *child2 = f2;
#line 296
do {
#line 296
  what->size += child2->size;
#line 296

#line 296
  if (child2->fi_code != FI_CONSTANT)
#line 296
    constargs = 0;
#line 296

#line 296
} while (child2 = child2->next);
#line 296 "filter/f-inst.c"
if (f2->type && (f2->type != (T_INT)) && !f_const_promotion(f2, (T_INT)))
#line 296
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 296
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f2->type));
#line 298 "filter/f-inst.c"
what->type = T_INT;

#line 300 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 300
    if (!constargs)
#line 300
      return what;
#line 300

#line 300
    /* Try to pre-calculate the result */
#line 300
    #line 294 "filter/f-inst.c"
 {
    
#line 295 "filter/f-inst.c"
 
#line 295 "filter/f-inst.c"
 
#line 295 "filter/f-inst.c"
;
    
#line 296 "filter/f-inst.c"
 
#line 296 "filter/f-inst.c"
 
#line 296 "filter/f-inst.c"
;
    if (v2.val.i == 0) runtime( "Mother told me not to divide by 0" );
     
#line 298 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = v1.val.i / v2.val.i } );
  }
  
  #undef whati
#line 300 "filter/f-inst.c"
  }
#line 300

#line 300

#line 310 "filter/f-inst.c"
 /* Constructor for FI_AND */
#line 310
struct f_inst *
#line 301 "filter/f-inst.c"
NONNULL(2)
#line 310 "filter/f-inst.c"
f_new_inst_FI_AND(enum f_instruction_code fi_code
#line 301 "filter/f-inst.c"
  , struct f_inst * f1
#line 306 "filter/f-inst.c"
  , struct f_inst * f2
#line 310 "filter/f-inst.c"
)
#line 310
  {
#line 310
    /* Allocate the structure */
#line 310
    struct f_inst *what = fi_new(fi_code);
#line 310
    uint constargs = 1;
#line 310

#line 310
    /* Initialize all the members */
#line 310
  #define whati (&(what->i_FI_AND))
#line 310
  #line 301 "filter/f-inst.c"
whati->f1 = f1;
#line 301
const struct f_inst *child1 = f1;
#line 301
do {
#line 301
  what->size += child1->size;
#line 301

#line 301
  if (child1->fi_code != FI_CONSTANT)
#line 301
    constargs = 0;
#line 301

#line 301
} while (child1 = child1->next);
#line 301 "filter/f-inst.c"
if (f1->type && (f1->type != (T_BOOL)) && !f_const_promotion(f1, (T_BOOL)))
#line 301
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 301
	   f_instruction_name(what->fi_code), f_type_name(T_BOOL), f_type_name(f1->type));
#line 302 "filter/f-inst.c"
if (f2->type && (f2->type != (T_BOOL)) && !f_const_promotion(f2, (T_BOOL)))
#line 302
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 302
	   f_instruction_name(what->fi_code), f_type_name(T_BOOL), f_type_name(f2->type));
#line 303 "filter/f-inst.c"
what->type = T_BOOL;
#line 306 "filter/f-inst.c"
whati->f2 = f2;

#line 310 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 310
    if (!constargs)
#line 310
      return what;
#line 310

#line 310
    /* Try to pre-calculate the result */
#line 310
    #line 300 "filter/f-inst.c"
 {
    
#line 301 "filter/f-inst.c"
 
#line 301 "filter/f-inst.c"
 
#line 301 "filter/f-inst.c"
;
    
#line 302 "filter/f-inst.c"
;
     
#line 303 "filter/f-inst.c"
;

    if (v1.val.i)
      
#line 306 "filter/f-inst.c"
return whati->f2
#line 306
;
    else
      return fi_constant(what, v1);
  }
  
  #undef whati
#line 310 "filter/f-inst.c"
  }
#line 310

#line 310

#line 321 "filter/f-inst.c"
 /* Constructor for FI_OR */
#line 321
struct f_inst *
#line 311 "filter/f-inst.c"
NONNULL(2)
#line 321 "filter/f-inst.c"
f_new_inst_FI_OR(enum f_instruction_code fi_code
#line 311 "filter/f-inst.c"
  , struct f_inst * f1
#line 316 "filter/f-inst.c"
  , struct f_inst * f2
#line 321 "filter/f-inst.c"
)
#line 321
  {
#line 321
    /* Allocate the structure */
#line 321
    struct f_inst *what = fi_new(fi_code);
#line 321
    uint constargs = 1;
#line 321

#line 321
    /* Initialize all the members */
#line 321
  #define whati (&(what->i_FI_OR))
#line 321
  #line 311 "filter/f-inst.c"
whati->f1 = f1;
#line 311
const struct f_inst *child1 = f1;
#line 311
do {
#line 311
  what->size += child1->size;
#line 311

#line 311
  if (child1->fi_code != FI_CONSTANT)
#line 311
    constargs = 0;
#line 311

#line 311
} while (child1 = child1->next);
#line 311 "filter/f-inst.c"
if (f1->type && (f1->type != (T_BOOL)) && !f_const_promotion(f1, (T_BOOL)))
#line 311
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 311
	   f_instruction_name(what->fi_code), f_type_name(T_BOOL), f_type_name(f1->type));
#line 312 "filter/f-inst.c"
if (f2->type && (f2->type != (T_BOOL)) && !f_const_promotion(f2, (T_BOOL)))
#line 312
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 312
	   f_instruction_name(what->fi_code), f_type_name(T_BOOL), f_type_name(f2->type));
#line 313 "filter/f-inst.c"
what->type = T_BOOL;
#line 316 "filter/f-inst.c"
whati->f2 = f2;

#line 321 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 321
    if (!constargs)
#line 321
      return what;
#line 321

#line 321
    /* Try to pre-calculate the result */
#line 321
    #line 310 "filter/f-inst.c"
 {
    
#line 311 "filter/f-inst.c"
 
#line 311 "filter/f-inst.c"
 
#line 311 "filter/f-inst.c"
;
    
#line 312 "filter/f-inst.c"
;
     
#line 313 "filter/f-inst.c"
;

    if (!v1.val.i)
      
#line 316 "filter/f-inst.c"
return whati->f2
#line 316
;
    else
      return fi_constant(what, v1);
  }

  
  #undef whati
#line 321 "filter/f-inst.c"
  }
#line 321

#line 321

#line 331 "filter/f-inst.c"
 /* Constructor for FI_PAIR_CONSTRUCT */
#line 331
struct f_inst *
#line 322 "filter/f-inst.c"
NONNULL(2)
#line 323 "filter/f-inst.c"
NONNULL(3)
#line 331 "filter/f-inst.c"
f_new_inst_FI_PAIR_CONSTRUCT(enum f_instruction_code fi_code
#line 322 "filter/f-inst.c"
  , struct f_inst * f1
#line 323 "filter/f-inst.c"
  , struct f_inst * f2
#line 331 "filter/f-inst.c"
)
#line 331
  {
#line 331
    /* Allocate the structure */
#line 331
    struct f_inst *what = fi_new(fi_code);
#line 331
    uint constargs = 1;
#line 331

#line 331
    /* Initialize all the members */
#line 331
  #define whati (&(what->i_FI_PAIR_CONSTRUCT))
#line 331
  #line 322 "filter/f-inst.c"
whati->f1 = f1;
#line 322
const struct f_inst *child1 = f1;
#line 322
do {
#line 322
  what->size += child1->size;
#line 322

#line 322
  if (child1->fi_code != FI_CONSTANT)
#line 322
    constargs = 0;
#line 322

#line 322
} while (child1 = child1->next);
#line 322 "filter/f-inst.c"
if (f1->type && (f1->type != (T_INT)) && !f_const_promotion(f1, (T_INT)))
#line 322
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 322
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f1->type));
#line 323 "filter/f-inst.c"
whati->f2 = f2;
#line 323
const struct f_inst *child2 = f2;
#line 323
do {
#line 323
  what->size += child2->size;
#line 323

#line 323
  if (child2->fi_code != FI_CONSTANT)
#line 323
    constargs = 0;
#line 323

#line 323
} while (child2 = child2->next);
#line 323 "filter/f-inst.c"
if (f2->type && (f2->type != (T_INT)) && !f_const_promotion(f2, (T_INT)))
#line 323
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 323
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f2->type));
#line 328 "filter/f-inst.c"
what->type = T_PAIR;

#line 331 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 331
    if (!constargs)
#line 331
      return what;
#line 331

#line 331
    /* Try to pre-calculate the result */
#line 331
    #line 321 "filter/f-inst.c"
 {
    
#line 322 "filter/f-inst.c"
 
#line 322 "filter/f-inst.c"
 
#line 322 "filter/f-inst.c"
;
    
#line 323 "filter/f-inst.c"
 
#line 323 "filter/f-inst.c"
 
#line 323 "filter/f-inst.c"
;
    uint u1 = v1.val.i;
    uint u2 = v2.val.i;
    if ((u1 > 0xFFFF) || (u2 > 0xFFFF))
      runtime( "Can't operate with value out of bounds in pair constructor" );
     
#line 328 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_PAIR, .val.i = (u1 << 16) | u2 } );
  }

  
  #undef whati
#line 331 "filter/f-inst.c"
  }
#line 331

#line 331

#line 371 "filter/f-inst.c"
 /* Constructor for FI_EC_CONSTRUCT */
#line 371
struct f_inst *
#line 332 "filter/f-inst.c"
NONNULL(2)
#line 333 "filter/f-inst.c"
NONNULL(3)
#line 371 "filter/f-inst.c"
f_new_inst_FI_EC_CONSTRUCT(enum f_instruction_code fi_code
#line 332 "filter/f-inst.c"
  , struct f_inst * f1
#line 333 "filter/f-inst.c"
  , struct f_inst * f2
#line 335 "filter/f-inst.c"
  , enum ec_subtype ecs
#line 371 "filter/f-inst.c"
)
#line 371
  {
#line 371
    /* Allocate the structure */
#line 371
    struct f_inst *what = fi_new(fi_code);
#line 371
    uint constargs = 1;
#line 371

#line 371
    /* Initialize all the members */
#line 371
  #define whati (&(what->i_FI_EC_CONSTRUCT))
#line 371
  #line 332 "filter/f-inst.c"
whati->f1 = f1;
#line 332
const struct f_inst *child1 = f1;
#line 332
do {
#line 332
  what->size += child1->size;
#line 332

#line 332
  if (child1->fi_code != FI_CONSTANT)
#line 332
    constargs = 0;
#line 332

#line 332
} while (child1 = child1->next);
#line 333 "filter/f-inst.c"
whati->f2 = f2;
#line 333
const struct f_inst *child2 = f2;
#line 333
do {
#line 333
  what->size += child2->size;
#line 333

#line 333
  if (child2->fi_code != FI_CONSTANT)
#line 333
    constargs = 0;
#line 333

#line 333
} while (child2 = child2->next);
#line 333 "filter/f-inst.c"
if (f2->type && (f2->type != (T_INT)) && !f_const_promotion(f2, (T_INT)))
#line 333
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 333
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f2->type));
#line 335 "filter/f-inst.c"
whati->ecs = ecs;
#line 356 "filter/f-inst.c"
what->type = T_EC;

#line 371 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 371
    if (!constargs)
#line 371
      return what;
#line 371

#line 371
    /* Try to pre-calculate the result */
#line 371
    #line 331 "filter/f-inst.c"
 {
    
#line 332 "filter/f-inst.c"
;
    
#line 333 "filter/f-inst.c"
 
#line 333 "filter/f-inst.c"
 
#line 333 "filter/f-inst.c"
;

    #line 335 "filter/f-inst.c"
;

    int ipv4_used;
    u32 key, val;

    if (v1.type == T_INT) {
      ipv4_used = 0; key = v1.val.i;
    }
    else if (v1.type == T_QUAD) {
      ipv4_used = 1; key = v1.val.i;
    }
    /* IP->Quad implicit conversion */
    else if (val_is_ip4(&v1)) {
      ipv4_used = 1; key = ipa_to_u32(v1.val.ip);
    }
    else
      runtime("Argument 1 of EC constructor must be integer or IPv4 address, got 0x%02x", v1.type);

    val = v2.val.i;

    if (ecs == EC_GENERIC)
       
#line 356 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_EC, .val.ec = ec_generic(key, val) } );
    else if (ipv4_used)
      if (val <= 0xFFFF)
	 return fi_constant(what,  (struct f_val) { .type = T_EC, .val.ec = ec_ip4(ecs, key, val) } );
      else
	runtime("4-byte value %u can't be used with IP-address key in extended community", val);
    else if (key < 0x10000)
       return fi_constant(what,  (struct f_val) { .type = T_EC, .val.ec = ec_as2(ecs, key, val) } );
    else
      if (val <= 0xFFFF)
	 return fi_constant(what,  (struct f_val) { .type = T_EC, .val.ec = ec_as4(ecs, key, val) } );
      else
	runtime("4-byte value %u can't be used with 4-byte ASN in extended community", val);
  }

  
  #undef whati
#line 371 "filter/f-inst.c"
  }
#line 371

#line 371

#line 378 "filter/f-inst.c"
 /* Constructor for FI_LC_CONSTRUCT */
#line 378
struct f_inst *
#line 372 "filter/f-inst.c"
NONNULL(2)
#line 373 "filter/f-inst.c"
NONNULL(3)
#line 374 "filter/f-inst.c"
NONNULL(4)
#line 378 "filter/f-inst.c"
f_new_inst_FI_LC_CONSTRUCT(enum f_instruction_code fi_code
#line 372 "filter/f-inst.c"
  , struct f_inst * f1
#line 373 "filter/f-inst.c"
  , struct f_inst * f2
#line 374 "filter/f-inst.c"
  , struct f_inst * f3
#line 378 "filter/f-inst.c"
)
#line 378
  {
#line 378
    /* Allocate the structure */
#line 378
    struct f_inst *what = fi_new(fi_code);
#line 378
    uint constargs = 1;
#line 378

#line 378
    /* Initialize all the members */
#line 378
  #define whati (&(what->i_FI_LC_CONSTRUCT))
#line 378
  #line 372 "filter/f-inst.c"
whati->f1 = f1;
#line 372
const struct f_inst *child1 = f1;
#line 372
do {
#line 372
  what->size += child1->size;
#line 372

#line 372
  if (child1->fi_code != FI_CONSTANT)
#line 372
    constargs = 0;
#line 372

#line 372
} while (child1 = child1->next);
#line 372 "filter/f-inst.c"
if (f1->type && (f1->type != (T_INT)) && !f_const_promotion(f1, (T_INT)))
#line 372
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 372
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f1->type));
#line 373 "filter/f-inst.c"
whati->f2 = f2;
#line 373
const struct f_inst *child2 = f2;
#line 373
do {
#line 373
  what->size += child2->size;
#line 373

#line 373
  if (child2->fi_code != FI_CONSTANT)
#line 373
    constargs = 0;
#line 373

#line 373
} while (child2 = child2->next);
#line 373 "filter/f-inst.c"
if (f2->type && (f2->type != (T_INT)) && !f_const_promotion(f2, (T_INT)))
#line 373
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 373
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f2->type));
#line 374 "filter/f-inst.c"
whati->f3 = f3;
#line 374
const struct f_inst *child3 = f3;
#line 374
do {
#line 374
  what->size += child3->size;
#line 374

#line 374
  if (child3->fi_code != FI_CONSTANT)
#line 374
    constargs = 0;
#line 374

#line 374
} while (child3 = child3->next);
#line 374 "filter/f-inst.c"
if (f3->type && (f3->type != (T_INT)) && !f_const_promotion(f3, (T_INT)))
#line 374
  cf_error("Argument 3 of %s must be of type %s, got type %s",
#line 374
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f3->type));
#line 375 "filter/f-inst.c"
what->type = T_LC;

#line 378 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 378
    if (!constargs)
#line 378
      return what;
#line 378

#line 378
    /* Try to pre-calculate the result */
#line 378
    #line 371 "filter/f-inst.c"
 {
    
#line 372 "filter/f-inst.c"
 
#line 372 "filter/f-inst.c"
 
#line 372 "filter/f-inst.c"
;
    
#line 373 "filter/f-inst.c"
 
#line 373 "filter/f-inst.c"
 
#line 373 "filter/f-inst.c"
;
    
#line 374 "filter/f-inst.c"
 
#line 374 "filter/f-inst.c"
 
#line 374 "filter/f-inst.c"
;
     
#line 375 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_LC, .val.lc = (lcomm) { v1.val.i, v2.val.i, v3.val.i } } );
  }

  
  #undef whati
#line 378 "filter/f-inst.c"
  }
#line 378

#line 378

#line 428 "filter/f-inst.c"
 /* Constructor for FI_PATHMASK_CONSTRUCT */
#line 428
struct f_inst *
#line 428
f_new_inst_FI_PATHMASK_CONSTRUCT(enum f_instruction_code fi_code
#line 379 "filter/f-inst.c"
  , struct f_inst * fvar
#line 428 "filter/f-inst.c"
)
#line 428
  {
#line 428
    /* Allocate the structure */
#line 428
    struct f_inst *what = fi_new(fi_code);
#line 428
    uint constargs = 1;
#line 428

#line 428
    /* Initialize all the members */
#line 428
  #define whati (&(what->i_FI_PATHMASK_CONSTRUCT))
#line 428
  #line 379 "filter/f-inst.c"
whati->varcount = 0;
#line 379
whati->fvar = fvar;
#line 379
for (const struct f_inst *child = fvar; child; child = child->next, whati->varcount++) {
#line 379
  what->size += child->size;
#line 379

#line 379
  if (child->fi_code != FI_CONSTANT)
#line 379
    constargs = 0;
#line 379

#line 379
}
#line 379

#line 379
  const struct f_inst **items = NULL;
#line 379
  if (constargs && whati->varcount) {
#line 379
    items = alloca(whati->varcount * sizeof(struct f_inst *));
#line 379
    const struct f_inst *child = fvar;
#line 379
    for (uint i=0; child; i++)
#line 379
      child = (items[i] = child)->next;
#line 379
  }
#line 379

#line 423 "filter/f-inst.c"
what->type = T_PATH_MASK;

#line 428 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 428
    if (!constargs)
#line 428
      return what;
#line 428

#line 428
    /* Try to pre-calculate the result */
#line 428
    #line 378 "filter/f-inst.c"
 {
    
#line 379 "filter/f-inst.c"

#line 379

#line 379
;

    struct f_path_mask *pm = falloc(sizeof(struct f_path_mask) + whati->varcount * sizeof(struct f_path_mask_item));
    pm->len = whati->varcount;

    for (uint i=0; i<whati->varcount; i++) {
      switch (vv(i).type) {
	case T_PATH_MASK_ITEM:
	  if (vv(i).val.pmi.kind == PM_LOOP)
	  {
	    if (i == 0)
	      runtime("Path mask iterator '+' cannot be first");

	    /* We want PM_LOOP as prefix operator */
	    pm->item[i] = pm->item[i - 1];
	    pm->item[i - 1] = vv(i).val.pmi;
	    break;
	  }

	  pm->item[i] = vv(i).val.pmi;
	  break;

	case T_INT:
	  pm->item[i] = (struct f_path_mask_item) {
	    .asn = vv(i).val.i,
	    .kind = PM_ASN,
	  };
	  break;

	case T_SET:
	  if (!path_set_type(vv(i).val.t))
	    runtime("Only integer sets allowed in path mask");

	  pm->item[i] = (struct f_path_mask_item) {
	    .set = vv(i).val.t,
	    .kind = PM_ASN_SET,
	  };
	  break;

	default:
	  runtime( "Error resolving path mask template: value not an integer" );
      }
    }

     
#line 423 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_PATH_MASK, .val.path_mask = pm } );
  }

/* Relational operators */

  
  #undef whati
#line 428 "filter/f-inst.c"
  }
#line 428

#line 428

#line 435 "filter/f-inst.c"
 /* Constructor for FI_NEQ */
#line 435
struct f_inst *
#line 429 "filter/f-inst.c"
NONNULL(2)
#line 430 "filter/f-inst.c"
NONNULL(3)
#line 435 "filter/f-inst.c"
f_new_inst_FI_NEQ(enum f_instruction_code fi_code
#line 429 "filter/f-inst.c"
  , struct f_inst * f1
#line 430 "filter/f-inst.c"
  , struct f_inst * f2
#line 435 "filter/f-inst.c"
)
#line 435
  {
#line 435
    /* Allocate the structure */
#line 435
    struct f_inst *what = fi_new(fi_code);
#line 435
    uint constargs = 1;
#line 435

#line 435
    /* Initialize all the members */
#line 435
  #define whati (&(what->i_FI_NEQ))
#line 435
  #line 429 "filter/f-inst.c"
whati->f1 = f1;
#line 429
const struct f_inst *child1 = f1;
#line 429
do {
#line 429
  what->size += child1->size;
#line 429

#line 429
  if (child1->fi_code != FI_CONSTANT)
#line 429
    constargs = 0;
#line 429

#line 429
} while (child1 = child1->next);
#line 430 "filter/f-inst.c"
whati->f2 = f2;
#line 430
const struct f_inst *child2 = f2;
#line 430
do {
#line 430
  what->size += child2->size;
#line 430

#line 430
  if (child2->fi_code != FI_CONSTANT)
#line 430
    constargs = 0;
#line 430

#line 430
} while (child2 = child2->next);
#line 431 "filter/f-inst.c"
if (f1->type && f2->type && (f1->type != f2->type))
#line 431
   (void) (f_const_promotion(f2, f1->type) || f_const_promotion(f1, f2->type));
#line 432 "filter/f-inst.c"
what->type = T_BOOL;

#line 435 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 435
    if (!constargs)
#line 435
      return what;
#line 435

#line 435
    /* Try to pre-calculate the result */
#line 435
    #line 428 "filter/f-inst.c"
 {
    
#line 429 "filter/f-inst.c"
;
    
#line 430 "filter/f-inst.c"
;
    
#line 431 "filter/f-inst.c"
;
     
#line 432 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_BOOL, .val.i = !val_same(&v1, &v2) } );
  }

  
  #undef whati
#line 435 "filter/f-inst.c"
  }
#line 435

#line 435

#line 442 "filter/f-inst.c"
 /* Constructor for FI_EQ */
#line 442
struct f_inst *
#line 436 "filter/f-inst.c"
NONNULL(2)
#line 437 "filter/f-inst.c"
NONNULL(3)
#line 442 "filter/f-inst.c"
f_new_inst_FI_EQ(enum f_instruction_code fi_code
#line 436 "filter/f-inst.c"
  , struct f_inst * f1
#line 437 "filter/f-inst.c"
  , struct f_inst * f2
#line 442 "filter/f-inst.c"
)
#line 442
  {
#line 442
    /* Allocate the structure */
#line 442
    struct f_inst *what = fi_new(fi_code);
#line 442
    uint constargs = 1;
#line 442

#line 442
    /* Initialize all the members */
#line 442
  #define whati (&(what->i_FI_EQ))
#line 442
  #line 436 "filter/f-inst.c"
whati->f1 = f1;
#line 436
const struct f_inst *child1 = f1;
#line 436
do {
#line 436
  what->size += child1->size;
#line 436

#line 436
  if (child1->fi_code != FI_CONSTANT)
#line 436
    constargs = 0;
#line 436

#line 436
} while (child1 = child1->next);
#line 437 "filter/f-inst.c"
whati->f2 = f2;
#line 437
const struct f_inst *child2 = f2;
#line 437
do {
#line 437
  what->size += child2->size;
#line 437

#line 437
  if (child2->fi_code != FI_CONSTANT)
#line 437
    constargs = 0;
#line 437

#line 437
} while (child2 = child2->next);
#line 438 "filter/f-inst.c"
if (f1->type && f2->type && (f1->type != f2->type))
#line 438
   (void) (f_const_promotion(f2, f1->type) || f_const_promotion(f1, f2->type));
#line 439 "filter/f-inst.c"
what->type = T_BOOL;

#line 442 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 442
    if (!constargs)
#line 442
      return what;
#line 442

#line 442
    /* Try to pre-calculate the result */
#line 442
    #line 435 "filter/f-inst.c"
 {
    
#line 436 "filter/f-inst.c"
;
    
#line 437 "filter/f-inst.c"
;
    
#line 438 "filter/f-inst.c"
;
     
#line 439 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_BOOL, .val.i = val_same(&v1, &v2) } );
  }

  
  #undef whati
#line 442 "filter/f-inst.c"
  }
#line 442

#line 442

#line 453 "filter/f-inst.c"
 /* Constructor for FI_LT */
#line 453
struct f_inst *
#line 443 "filter/f-inst.c"
NONNULL(2)
#line 444 "filter/f-inst.c"
NONNULL(3)
#line 453 "filter/f-inst.c"
f_new_inst_FI_LT(enum f_instruction_code fi_code
#line 443 "filter/f-inst.c"
  , struct f_inst * f1
#line 444 "filter/f-inst.c"
  , struct f_inst * f2
#line 453 "filter/f-inst.c"
)
#line 453
  {
#line 453
    /* Allocate the structure */
#line 453
    struct f_inst *what = fi_new(fi_code);
#line 453
    uint constargs = 1;
#line 453

#line 453
    /* Initialize all the members */
#line 453
  #define whati (&(what->i_FI_LT))
#line 453
  #line 443 "filter/f-inst.c"
whati->f1 = f1;
#line 443
const struct f_inst *child1 = f1;
#line 443
do {
#line 443
  what->size += child1->size;
#line 443

#line 443
  if (child1->fi_code != FI_CONSTANT)
#line 443
    constargs = 0;
#line 443

#line 443
} while (child1 = child1->next);
#line 444 "filter/f-inst.c"
whati->f2 = f2;
#line 444
const struct f_inst *child2 = f2;
#line 444
do {
#line 444
  what->size += child2->size;
#line 444

#line 444
  if (child2->fi_code != FI_CONSTANT)
#line 444
    constargs = 0;
#line 444

#line 444
} while (child2 = child2->next);
#line 445 "filter/f-inst.c"
if (f1->type && f2->type && (f1->type != f2->type) &&
#line 445
   !f_const_promotion(f2, f1->type) && !f_const_promotion(f1, f2->type))
#line 445
  cf_error("Arguments 1 and 2 of %s must be of the same type", f_instruction_name(what->fi_code));
#line 450 "filter/f-inst.c"
what->type = T_BOOL;

#line 453 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 453
    if (!constargs)
#line 453
      return what;
#line 453

#line 453
    /* Try to pre-calculate the result */
#line 453
    #line 442 "filter/f-inst.c"
 {
    
#line 443 "filter/f-inst.c"
;
    
#line 444 "filter/f-inst.c"
;
    
#line 445 "filter/f-inst.c"
;

    int i = val_compare(&v1, &v2);
    if (i == F_CMP_ERROR)
      runtime( "Can't compare values of incompatible types" );
     
#line 450 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_BOOL, .val.i = (i == -1) } );
  }

  
  #undef whati
#line 453 "filter/f-inst.c"
  }
#line 453

#line 453

#line 464 "filter/f-inst.c"
 /* Constructor for FI_LTE */
#line 464
struct f_inst *
#line 454 "filter/f-inst.c"
NONNULL(2)
#line 455 "filter/f-inst.c"
NONNULL(3)
#line 464 "filter/f-inst.c"
f_new_inst_FI_LTE(enum f_instruction_code fi_code
#line 454 "filter/f-inst.c"
  , struct f_inst * f1
#line 455 "filter/f-inst.c"
  , struct f_inst * f2
#line 464 "filter/f-inst.c"
)
#line 464
  {
#line 464
    /* Allocate the structure */
#line 464
    struct f_inst *what = fi_new(fi_code);
#line 464
    uint constargs = 1;
#line 464

#line 464
    /* Initialize all the members */
#line 464
  #define whati (&(what->i_FI_LTE))
#line 464
  #line 454 "filter/f-inst.c"
whati->f1 = f1;
#line 454
const struct f_inst *child1 = f1;
#line 454
do {
#line 454
  what->size += child1->size;
#line 454

#line 454
  if (child1->fi_code != FI_CONSTANT)
#line 454
    constargs = 0;
#line 454

#line 454
} while (child1 = child1->next);
#line 455 "filter/f-inst.c"
whati->f2 = f2;
#line 455
const struct f_inst *child2 = f2;
#line 455
do {
#line 455
  what->size += child2->size;
#line 455

#line 455
  if (child2->fi_code != FI_CONSTANT)
#line 455
    constargs = 0;
#line 455

#line 455
} while (child2 = child2->next);
#line 456 "filter/f-inst.c"
if (f1->type && f2->type && (f1->type != f2->type) &&
#line 456
   !f_const_promotion(f2, f1->type) && !f_const_promotion(f1, f2->type))
#line 456
  cf_error("Arguments 1 and 2 of %s must be of the same type", f_instruction_name(what->fi_code));
#line 461 "filter/f-inst.c"
what->type = T_BOOL;

#line 464 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 464
    if (!constargs)
#line 464
      return what;
#line 464

#line 464
    /* Try to pre-calculate the result */
#line 464
    #line 453 "filter/f-inst.c"
 {
    
#line 454 "filter/f-inst.c"
;
    
#line 455 "filter/f-inst.c"
;
    
#line 456 "filter/f-inst.c"
;

    int i = val_compare(&v1, &v2);
    if (i == F_CMP_ERROR)
      runtime( "Can't compare values of incompatible types" );
     
#line 461 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_BOOL, .val.i = (i != 1) } );
  }

  
  #undef whati
#line 464 "filter/f-inst.c"
  }
#line 464

#line 464

#line 469 "filter/f-inst.c"
 /* Constructor for FI_NOT */
#line 469
struct f_inst *
#line 465 "filter/f-inst.c"
NONNULL(2)
#line 469 "filter/f-inst.c"
f_new_inst_FI_NOT(enum f_instruction_code fi_code
#line 465 "filter/f-inst.c"
  , struct f_inst * f1
#line 469 "filter/f-inst.c"
)
#line 469
  {
#line 469
    /* Allocate the structure */
#line 469
    struct f_inst *what = fi_new(fi_code);
#line 469
    uint constargs = 1;
#line 469

#line 469
    /* Initialize all the members */
#line 469
  #define whati (&(what->i_FI_NOT))
#line 469
  #line 465 "filter/f-inst.c"
whati->f1 = f1;
#line 465
const struct f_inst *child1 = f1;
#line 465
do {
#line 465
  what->size += child1->size;
#line 465

#line 465
  if (child1->fi_code != FI_CONSTANT)
#line 465
    constargs = 0;
#line 465

#line 465
} while (child1 = child1->next);
#line 465 "filter/f-inst.c"
if (f1->type && (f1->type != (T_BOOL)) && !f_const_promotion(f1, (T_BOOL)))
#line 465
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 465
	   f_instruction_name(what->fi_code), f_type_name(T_BOOL), f_type_name(f1->type));
#line 466 "filter/f-inst.c"
what->type = T_BOOL;

#line 469 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 469
    if (!constargs)
#line 469
      return what;
#line 469

#line 469
    /* Try to pre-calculate the result */
#line 469
    #line 464 "filter/f-inst.c"
 {
    
#line 465 "filter/f-inst.c"
 
#line 465 "filter/f-inst.c"
 
#line 465 "filter/f-inst.c"
;
     
#line 466 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_BOOL, .val.i = !v1.val.i } );
  }

  
  #undef whati
#line 469 "filter/f-inst.c"
  }
#line 469

#line 469

#line 478 "filter/f-inst.c"
 /* Constructor for FI_MATCH */
#line 478
struct f_inst *
#line 470 "filter/f-inst.c"
NONNULL(2)
#line 471 "filter/f-inst.c"
NONNULL(3)
#line 478 "filter/f-inst.c"
f_new_inst_FI_MATCH(enum f_instruction_code fi_code
#line 470 "filter/f-inst.c"
  , struct f_inst * f1
#line 471 "filter/f-inst.c"
  , struct f_inst * f2
#line 478 "filter/f-inst.c"
)
#line 478
  {
#line 478
    /* Allocate the structure */
#line 478
    struct f_inst *what = fi_new(fi_code);
#line 478
    uint constargs = 1;
#line 478

#line 478
    /* Initialize all the members */
#line 478
  #define whati (&(what->i_FI_MATCH))
#line 478
  #line 470 "filter/f-inst.c"
whati->f1 = f1;
#line 470
const struct f_inst *child1 = f1;
#line 470
do {
#line 470
  what->size += child1->size;
#line 470

#line 470
  if (child1->fi_code != FI_CONSTANT)
#line 470
    constargs = 0;
#line 470

#line 470
} while (child1 = child1->next);
#line 471 "filter/f-inst.c"
whati->f2 = f2;
#line 471
const struct f_inst *child2 = f2;
#line 471
do {
#line 471
  what->size += child2->size;
#line 471

#line 471
  if (child2->fi_code != FI_CONSTANT)
#line 471
    constargs = 0;
#line 471

#line 471
} while (child2 = child2->next);
#line 475 "filter/f-inst.c"
what->type = T_BOOL;

#line 478 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 478
    if (!constargs)
#line 478
      return what;
#line 478

#line 478
    /* Try to pre-calculate the result */
#line 478
    #line 469 "filter/f-inst.c"
 {
    
#line 470 "filter/f-inst.c"
;
    
#line 471 "filter/f-inst.c"
;
    int i = val_in_range(&v1, &v2);
    if (i == F_CMP_ERROR)
      runtime( "~ applied on unknown type pair" );
     
#line 475 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_BOOL, .val.i = !!i } );
  }

  
  #undef whati
#line 478 "filter/f-inst.c"
  }
#line 478

#line 478

#line 487 "filter/f-inst.c"
 /* Constructor for FI_NOT_MATCH */
#line 487
struct f_inst *
#line 479 "filter/f-inst.c"
NONNULL(2)
#line 480 "filter/f-inst.c"
NONNULL(3)
#line 487 "filter/f-inst.c"
f_new_inst_FI_NOT_MATCH(enum f_instruction_code fi_code
#line 479 "filter/f-inst.c"
  , struct f_inst * f1
#line 480 "filter/f-inst.c"
  , struct f_inst * f2
#line 487 "filter/f-inst.c"
)
#line 487
  {
#line 487
    /* Allocate the structure */
#line 487
    struct f_inst *what = fi_new(fi_code);
#line 487
    uint constargs = 1;
#line 487

#line 487
    /* Initialize all the members */
#line 487
  #define whati (&(what->i_FI_NOT_MATCH))
#line 487
  #line 479 "filter/f-inst.c"
whati->f1 = f1;
#line 479
const struct f_inst *child1 = f1;
#line 479
do {
#line 479
  what->size += child1->size;
#line 479

#line 479
  if (child1->fi_code != FI_CONSTANT)
#line 479
    constargs = 0;
#line 479

#line 479
} while (child1 = child1->next);
#line 480 "filter/f-inst.c"
whati->f2 = f2;
#line 480
const struct f_inst *child2 = f2;
#line 480
do {
#line 480
  what->size += child2->size;
#line 480

#line 480
  if (child2->fi_code != FI_CONSTANT)
#line 480
    constargs = 0;
#line 480

#line 480
} while (child2 = child2->next);
#line 484 "filter/f-inst.c"
what->type = T_BOOL;

#line 487 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 487
    if (!constargs)
#line 487
      return what;
#line 487

#line 487
    /* Try to pre-calculate the result */
#line 487
    #line 478 "filter/f-inst.c"
 {
    
#line 479 "filter/f-inst.c"
;
    
#line 480 "filter/f-inst.c"
;
    int i = val_in_range(&v1, &v2);
    if (i == F_CMP_ERROR)
      runtime( "!~ applied on unknown type pair" );
     
#line 484 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_BOOL, .val.i = !i } );
  }

  
  #undef whati
#line 487 "filter/f-inst.c"
  }
#line 487

#line 487

#line 492 "filter/f-inst.c"
 /* Constructor for FI_DEFINED */
#line 492
struct f_inst *
#line 488 "filter/f-inst.c"
NONNULL(2)
#line 492 "filter/f-inst.c"
f_new_inst_FI_DEFINED(enum f_instruction_code fi_code
#line 488 "filter/f-inst.c"
  , struct f_inst * f1
#line 492 "filter/f-inst.c"
)
#line 492
  {
#line 492
    /* Allocate the structure */
#line 492
    struct f_inst *what = fi_new(fi_code);
#line 492
    uint constargs = 1;
#line 492

#line 492
    /* Initialize all the members */
#line 492
  #define whati (&(what->i_FI_DEFINED))
#line 492
  #line 488 "filter/f-inst.c"
whati->f1 = f1;
#line 488
const struct f_inst *child1 = f1;
#line 488
do {
#line 488
  what->size += child1->size;
#line 488

#line 488
  if (child1->fi_code != FI_CONSTANT)
#line 488
    constargs = 0;
#line 488

#line 488
} while (child1 = child1->next);
#line 489 "filter/f-inst.c"
what->type = T_BOOL;

#line 492 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 492
    if (!constargs)
#line 492
      return what;
#line 492

#line 492
    /* Try to pre-calculate the result */
#line 492
    #line 487 "filter/f-inst.c"
 {
    
#line 488 "filter/f-inst.c"
;
     
#line 489 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_BOOL, .val.i = (v1.type != T_VOID) && !undef_value(v1) } );
  }

  
  #undef whati
#line 492 "filter/f-inst.c"
  }
#line 492

#line 492

#line 504 "filter/f-inst.c"
 /* Constructor for FI_TYPE */
#line 504
struct f_inst *
#line 493 "filter/f-inst.c"
NONNULL(2)
#line 504 "filter/f-inst.c"
f_new_inst_FI_TYPE(enum f_instruction_code fi_code
#line 493 "filter/f-inst.c"
  , struct f_inst * f1
#line 504 "filter/f-inst.c"
)
#line 504
  {
#line 504
    /* Allocate the structure */
#line 504
    struct f_inst *what = fi_new(fi_code);
#line 504
    uint constargs = 1;
#line 504

#line 504
    /* Initialize all the members */
#line 504
  #define whati (&(what->i_FI_TYPE))
#line 504
  #line 493 "filter/f-inst.c"
whati->f1 = f1;
#line 493
const struct f_inst *child1 = f1;
#line 493
do {
#line 493
  what->size += child1->size;
#line 493

#line 493
  if (child1->fi_code != FI_CONSTANT)
#line 493
    constargs = 0;
#line 493

#line 493
} while (child1 = child1->next);
#line 497 "filter/f-inst.c"
what->type = T_ENUM_NETTYPE;

#line 504 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 504
    if (!constargs)
#line 504
      return what;
#line 504

#line 504
    /* Try to pre-calculate the result */
#line 504
    #line 492 "filter/f-inst.c"
 {
    
#line 493 "filter/f-inst.c"
; /* There may be more types supporting this operation */
    switch (v1.type)
    {
      case T_NET:
	 
#line 497 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_ENUM_NETTYPE, .val.i = v1.val.net->type } );
	break;
      default:
	runtime( "Can't determine type of this item" );
    }
  }

  
  #undef whati
#line 504 "filter/f-inst.c"
  }
#line 504

#line 504

#line 509 "filter/f-inst.c"
 /* Constructor for FI_IS_V4 */
#line 509
struct f_inst *
#line 505 "filter/f-inst.c"
NONNULL(2)
#line 509 "filter/f-inst.c"
f_new_inst_FI_IS_V4(enum f_instruction_code fi_code
#line 505 "filter/f-inst.c"
  , struct f_inst * f1
#line 509 "filter/f-inst.c"
)
#line 509
  {
#line 509
    /* Allocate the structure */
#line 509
    struct f_inst *what = fi_new(fi_code);
#line 509
    uint constargs = 1;
#line 509

#line 509
    /* Initialize all the members */
#line 509
  #define whati (&(what->i_FI_IS_V4))
#line 509
  #line 505 "filter/f-inst.c"
whati->f1 = f1;
#line 505
const struct f_inst *child1 = f1;
#line 505
do {
#line 505
  what->size += child1->size;
#line 505

#line 505
  if (child1->fi_code != FI_CONSTANT)
#line 505
    constargs = 0;
#line 505

#line 505
} while (child1 = child1->next);
#line 505 "filter/f-inst.c"
if (f1->type && (f1->type != (T_IP)) && !f_const_promotion(f1, (T_IP)))
#line 505
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 505
	   f_instruction_name(what->fi_code), f_type_name(T_IP), f_type_name(f1->type));
#line 506 "filter/f-inst.c"
what->type = T_BOOL;

#line 509 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 509
    if (!constargs)
#line 509
      return what;
#line 509

#line 509
    /* Try to pre-calculate the result */
#line 509
    #line 504 "filter/f-inst.c"
 {
    
#line 505 "filter/f-inst.c"
 
#line 505 "filter/f-inst.c"
 
#line 505 "filter/f-inst.c"
;
     
#line 506 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_BOOL, .val.i = ipa_is_ip4(v1.val.ip) } );
  }

  
  #undef whati
#line 509 "filter/f-inst.c"
  }
#line 509

#line 509

#line 522 "filter/f-inst.c"
 /* Constructor for FI_VAR_INIT */
#line 522
struct f_inst *
#line 511 "filter/f-inst.c"
NONNULL(2)
#line 522 "filter/f-inst.c"
f_new_inst_FI_VAR_INIT(enum f_instruction_code fi_code
#line 511 "filter/f-inst.c"
  , struct f_inst * f1
#line 512 "filter/f-inst.c"
  , struct symbol * sym
#line 522 "filter/f-inst.c"
)
#line 522
  {
#line 522
    /* Allocate the structure */
#line 522
    struct f_inst *what = fi_new(fi_code);
#line 522
    
#line 522

#line 522
    /* Initialize all the members */
#line 522
  #define whati (&(what->i_FI_VAR_INIT))
#line 522
  #line 511 "filter/f-inst.c"
whati->f1 = f1;
#line 511
const struct f_inst *child1 = f1;
#line 511
do {
#line 511
  what->size += child1->size;
#line 511

#line 511
} while (child1 = child1->next);
#line 512 "filter/f-inst.c"
whati->sym = sym;
#line 513 "filter/f-inst.c"
if (f1->type && (f1->type != (sym->class & 0xff)) && !f_const_promotion(f1, (sym->class & 0xff)))
#line 513
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 513
	   f_instruction_name(what->fi_code), f_type_name(sym->class & 0xff), f_type_name(f1->type));

#line 522 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 522
    
#line 522
      return what;
#line 522

#line 522
    /* Try to pre-calculate the result */
#line 522
      #undef whati
#line 522
  }
#line 522

#line 522

#line 531 "filter/f-inst.c"
 /* Constructor for FI_VAR_SET */
#line 531
struct f_inst *
#line 524 "filter/f-inst.c"
NONNULL(2)
#line 531 "filter/f-inst.c"
f_new_inst_FI_VAR_SET(enum f_instruction_code fi_code
#line 524 "filter/f-inst.c"
  , struct f_inst * f1
#line 525 "filter/f-inst.c"
  , struct symbol * sym
#line 531 "filter/f-inst.c"
)
#line 531
  {
#line 531
    /* Allocate the structure */
#line 531
    struct f_inst *what = fi_new(fi_code);
#line 531
    
#line 531

#line 531
    /* Initialize all the members */
#line 531
  #define whati (&(what->i_FI_VAR_SET))
#line 531
  #line 524 "filter/f-inst.c"
whati->f1 = f1;
#line 524
const struct f_inst *child1 = f1;
#line 524
do {
#line 524
  what->size += child1->size;
#line 524

#line 524
} while (child1 = child1->next);
#line 525 "filter/f-inst.c"
whati->sym = sym;
#line 526 "filter/f-inst.c"
if (f1->type && (f1->type != (sym->class & 0xff)) && !f_const_promotion(f1, (sym->class & 0xff)))
#line 526
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 526
	   f_instruction_name(what->fi_code), f_type_name(sym->class & 0xff), f_type_name(f1->type));

#line 531 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 531
    
#line 531
      return what;
#line 531

#line 531
    /* Try to pre-calculate the result */
#line 531
      #undef whati
#line 531
  }
#line 531

#line 531

#line 538 "filter/f-inst.c"
 /* Constructor for FI_VAR_GET */
#line 538
struct f_inst *
#line 538
f_new_inst_FI_VAR_GET(enum f_instruction_code fi_code
#line 532 "filter/f-inst.c"
  , struct symbol * sym
#line 538 "filter/f-inst.c"
)
#line 538
  {
#line 538
    /* Allocate the structure */
#line 538
    struct f_inst *what = fi_new(fi_code);
#line 538
    
#line 538

#line 538
    /* Initialize all the members */
#line 538
  #define whati (&(what->i_FI_VAR_GET))
#line 538
  #line 532 "filter/f-inst.c"
whati->sym = sym;
#line 534 "filter/f-inst.c"
what->type = sym->class & 0xff;

#line 538 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 538
    
#line 538
      return what;
#line 538

#line 538
    /* Try to pre-calculate the result */
#line 538
      #undef whati
#line 538
  }
#line 538

#line 538

#line 551 "filter/f-inst.c"
 /* Constructor for FI_CONSTANT */
#line 551
struct f_inst *
#line 551
f_new_inst_FI_CONSTANT(enum f_instruction_code fi_code
#line 539 "filter/f-inst.c"
  , struct f_val val
#line 551 "filter/f-inst.c"
)
#line 551
  {
#line 551
    /* Allocate the structure */
#line 551
    struct f_inst *what = fi_new(fi_code);
#line 551
    uint constargs = 1;
#line 551

#line 551
    /* Initialize all the members */
#line 551
  #define whati (&(what->i_FI_CONSTANT))
#line 551
  #line 539 "filter/f-inst.c"
whati->val = val;
#line 547 "filter/f-inst.c"
what->type = val.type;

#line 551 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 551
    if (!constargs)
#line 551
      return what;
#line 551

#line 551
    /* Try to pre-calculate the result */
#line 551
    #line 538 "filter/f-inst.c"
 {
    #line 545 "filter/f-inst.c"
;

     
#line 547 "filter/f-inst.c"
;
    return fi_constant(what, val);
  }

  
  #undef whati
#line 551 "filter/f-inst.c"
  }
#line 551

#line 551

#line 585 "filter/f-inst.c"
 /* Constructor for FI_FOR_INIT */
#line 585
struct f_inst *
#line 553 "filter/f-inst.c"
NONNULL(2)
#line 585 "filter/f-inst.c"
f_new_inst_FI_FOR_INIT(enum f_instruction_code fi_code
#line 553 "filter/f-inst.c"
  , struct f_inst * f1
#line 554 "filter/f-inst.c"
  , struct symbol * sym
#line 585 "filter/f-inst.c"
)
#line 585
  {
#line 585
    /* Allocate the structure */
#line 585
    struct f_inst *what = fi_new(fi_code);
#line 585
    
#line 585

#line 585
    /* Initialize all the members */
#line 585
  #define whati (&(what->i_FI_FOR_INIT))
#line 585
  #line 553 "filter/f-inst.c"
whati->f1 = f1;
#line 553
const struct f_inst *child1 = f1;
#line 553
do {
#line 553
  what->size += child1->size;
#line 553

#line 553
} while (child1 = child1->next);
#line 554 "filter/f-inst.c"
whati->sym = sym;

#line 557 "filter/f-inst.c"
    ASSERT((sym->class & ~0xff) == SYM_VARIABLE);

    /* Static type check */
    if (f1->type)
    {
      enum f_type t_var = (sym->class & 0xff);
      enum f_type t_arg = f_type_element_type(f1->type);
      if (!t_arg)
        cf_error("Value of expression in FOR must be iterable, got %s",
		 f_type_name(f1->type));
      if (t_var != t_arg)
	cf_error("Loop variable '%s' in FOR must be %s, is %s",
		 sym->name, f_type_name(t_arg), f_type_name(t_var));
    }

    
#line 585 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 585
    
#line 585
      return what;
#line 585

#line 585
    /* Try to pre-calculate the result */
#line 585
      #undef whati
#line 585
  }
#line 585

#line 585

#line 639 "filter/f-inst.c"
 /* Constructor for FI_FOR_NEXT */
#line 639
struct f_inst *
#line 639
f_new_inst_FI_FOR_NEXT(enum f_instruction_code fi_code
#line 587 "filter/f-inst.c"
  , struct symbol * sym
#line 630 "filter/f-inst.c"
  , struct f_inst * f1
#line 639 "filter/f-inst.c"
)
#line 639
  {
#line 639
    /* Allocate the structure */
#line 639
    struct f_inst *what = fi_new(fi_code);
#line 639
    
#line 639

#line 639
    /* Initialize all the members */
#line 639
  #define whati (&(what->i_FI_FOR_NEXT))
#line 639
  #line 587 "filter/f-inst.c"
whati->sym = sym;
#line 630 "filter/f-inst.c"
whati->f1 = f1;

#line 639 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 639
    
#line 639
      return what;
#line 639

#line 639
    /* Try to pre-calculate the result */
#line 639
      #undef whati
#line 639
  }
#line 639

#line 639

#line 647 "filter/f-inst.c"
 /* Constructor for FI_CONDITION */
#line 647
struct f_inst *
#line 640 "filter/f-inst.c"
NONNULL(2)
#line 647 "filter/f-inst.c"
f_new_inst_FI_CONDITION(enum f_instruction_code fi_code
#line 640 "filter/f-inst.c"
  , struct f_inst * f1
#line 642 "filter/f-inst.c"
  , struct f_inst * f2
#line 644 "filter/f-inst.c"
  , struct f_inst * f3
#line 647 "filter/f-inst.c"
)
#line 647
  {
#line 647
    /* Allocate the structure */
#line 647
    struct f_inst *what = fi_new(fi_code);
#line 647
    uint constargs = 1;
#line 647

#line 647
    /* Initialize all the members */
#line 647
  #define whati (&(what->i_FI_CONDITION))
#line 647
  #line 640 "filter/f-inst.c"
whati->f1 = f1;
#line 640
const struct f_inst *child1 = f1;
#line 640
do {
#line 640
  what->size += child1->size;
#line 640

#line 640
  if (child1->fi_code != FI_CONSTANT)
#line 640
    constargs = 0;
#line 640

#line 640
} while (child1 = child1->next);
#line 640 "filter/f-inst.c"
if (f1->type && (f1->type != (T_BOOL)) && !f_const_promotion(f1, (T_BOOL)))
#line 640
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 640
	   f_instruction_name(what->fi_code), f_type_name(T_BOOL), f_type_name(f1->type));
#line 642 "filter/f-inst.c"
whati->f2 = f2;
#line 644 "filter/f-inst.c"
whati->f3 = f3;

#line 647 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 647
    if (!constargs)
#line 647
      return what;
#line 647

#line 647
    /* Try to pre-calculate the result */
#line 647
    #line 639 "filter/f-inst.c"
 {
    
#line 640 "filter/f-inst.c"
 
#line 640 "filter/f-inst.c"
 
#line 640 "filter/f-inst.c"
;
    if (v1.val.i)
      
#line 642 "filter/f-inst.c"
return whati->f2
#line 642
;
    else
      
#line 644 "filter/f-inst.c"
return whati->f3
#line 644
;
  }

  
  #undef whati
#line 647 "filter/f-inst.c"
  }
#line 647

#line 647

#line 656 "filter/f-inst.c"
 /* Constructor for FI_PRINT */
#line 656
struct f_inst *
#line 656
f_new_inst_FI_PRINT(enum f_instruction_code fi_code
#line 649 "filter/f-inst.c"
  , struct f_inst * fvar
#line 656 "filter/f-inst.c"
)
#line 656
  {
#line 656
    /* Allocate the structure */
#line 656
    struct f_inst *what = fi_new(fi_code);
#line 656
    
#line 656

#line 656
    /* Initialize all the members */
#line 656
  #define whati (&(what->i_FI_PRINT))
#line 656
  #line 649 "filter/f-inst.c"
whati->varcount = 0;
#line 649
whati->fvar = fvar;
#line 649
for (const struct f_inst *child = fvar; child; child = child->next, whati->varcount++) {
#line 649
  what->size += child->size;
#line 649

#line 649
}
#line 649


#line 656 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 656
    
#line 656
      return what;
#line 656

#line 656
    /* Try to pre-calculate the result */
#line 656
      #undef whati
#line 656
  }
#line 656

#line 656

#line 663 "filter/f-inst.c"
 /* Constructor for FI_FLUSH */
#line 663
struct f_inst *
#line 663
f_new_inst_FI_FLUSH(enum f_instruction_code fi_code
#line 663
)
#line 663
  {
#line 663
    /* Allocate the structure */
#line 663
    struct f_inst *what = fi_new(fi_code);
#line 663
    
#line 663

#line 663
    /* Initialize all the members */
#line 663
  #define whati (&(what->i_FI_FLUSH))
#line 663
  
#line 663
    /* If not constant, return the instruction itself */
#line 663
    
#line 663
      return what;
#line 663

#line 663
    /* Try to pre-calculate the result */
#line 663
      #undef whati
#line 663
  }
#line 663

#line 663

#line 677 "filter/f-inst.c"
 /* Constructor for FI_DIE */
#line 677
struct f_inst *
#line 677
f_new_inst_FI_DIE(enum f_instruction_code fi_code
#line 665 "filter/f-inst.c"
  , enum filter_return fret
#line 677 "filter/f-inst.c"
)
#line 677
  {
#line 677
    /* Allocate the structure */
#line 677
    struct f_inst *what = fi_new(fi_code);
#line 677
    
#line 677

#line 677
    /* Initialize all the members */
#line 677
  #define whati (&(what->i_FI_DIE))
#line 677
  #line 665 "filter/f-inst.c"
whati->fret = fret;

#line 677 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 677
    
#line 677
      return what;
#line 677

#line 677
    /* Try to pre-calculate the result */
#line 677
      #undef whati
#line 677
  }
#line 677

#line 677

#line 705 "filter/f-inst.c"
 /* Constructor for FI_RTA_GET */
#line 705
struct f_inst *
#line 705
f_new_inst_FI_RTA_GET(enum f_instruction_code fi_code
#line 679 "filter/f-inst.c"
  , struct f_static_attr sa
#line 705 "filter/f-inst.c"
)
#line 705
  {
#line 705
    /* Allocate the structure */
#line 705
    struct f_inst *what = fi_new(fi_code);
#line 705
    
#line 705

#line 705
    /* Initialize all the members */
#line 705
  #define whati (&(what->i_FI_RTA_GET))
#line 705
  #line 679 "filter/f-inst.c"
whati->sa = sa;
#line 685 "filter/f-inst.c"
what->type = sa.f_type;

#line 705 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 705
    
#line 705
      return what;
#line 705

#line 705
    /* Try to pre-calculate the result */
#line 705
      #undef whati
#line 705
  }
#line 705

#line 705

#line 820 "filter/f-inst.c"
 /* Constructor for FI_RTA_SET */
#line 820
struct f_inst *
#line 707 "filter/f-inst.c"
NONNULL(2)
#line 820 "filter/f-inst.c"
f_new_inst_FI_RTA_SET(enum f_instruction_code fi_code
#line 707 "filter/f-inst.c"
  , struct f_inst * f1
#line 708 "filter/f-inst.c"
  , struct f_static_attr sa
#line 820 "filter/f-inst.c"
)
#line 820
  {
#line 820
    /* Allocate the structure */
#line 820
    struct f_inst *what = fi_new(fi_code);
#line 820
    
#line 820

#line 820
    /* Initialize all the members */
#line 820
  #define whati (&(what->i_FI_RTA_SET))
#line 820
  #line 707 "filter/f-inst.c"
whati->f1 = f1;
#line 707
const struct f_inst *child1 = f1;
#line 707
do {
#line 707
  what->size += child1->size;
#line 707

#line 707
} while (child1 = child1->next);
#line 708 "filter/f-inst.c"
whati->sa = sa;
#line 709 "filter/f-inst.c"
if (f1->type && (f1->type != (sa.f_type)) && !f_const_promotion(f1, (sa.f_type)))
#line 709
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 709
	   f_instruction_name(what->fi_code), f_type_name(sa.f_type), f_type_name(f1->type));

#line 820 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 820
    
#line 820
      return what;
#line 820

#line 820
    /* Try to pre-calculate the result */
#line 820
      #undef whati
#line 820
  }
#line 820

#line 820

#line 892 "filter/f-inst.c"
 /* Constructor for FI_EA_GET */
#line 892
struct f_inst *
#line 892
f_new_inst_FI_EA_GET(enum f_instruction_code fi_code
#line 821 "filter/f-inst.c"
  , struct f_dynamic_attr da
#line 892 "filter/f-inst.c"
)
#line 892
  {
#line 892
    /* Allocate the structure */
#line 892
    struct f_inst *what = fi_new(fi_code);
#line 892
    
#line 892

#line 892
    /* Initialize all the members */
#line 892
  #define whati (&(what->i_FI_EA_GET))
#line 892
  #line 821 "filter/f-inst.c"
whati->da = da;
#line 824 "filter/f-inst.c"
what->type = da.f_type;

#line 892 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 892
    
#line 892
      return what;
#line 892

#line 892
    /* Try to pre-calculate the result */
#line 892
      #undef whati
#line 892
  }
#line 892

#line 892

#line 959 "filter/f-inst.c"
 /* Constructor for FI_EA_SET */
#line 959
struct f_inst *
#line 895 "filter/f-inst.c"
NONNULL(2)
#line 959 "filter/f-inst.c"
f_new_inst_FI_EA_SET(enum f_instruction_code fi_code
#line 895 "filter/f-inst.c"
  , struct f_inst * f1
#line 896 "filter/f-inst.c"
  , struct f_dynamic_attr da
#line 959 "filter/f-inst.c"
)
#line 959
  {
#line 959
    /* Allocate the structure */
#line 959
    struct f_inst *what = fi_new(fi_code);
#line 959
    
#line 959

#line 959
    /* Initialize all the members */
#line 959
  #define whati (&(what->i_FI_EA_SET))
#line 959
  #line 895 "filter/f-inst.c"
whati->f1 = f1;
#line 895
const struct f_inst *child1 = f1;
#line 895
do {
#line 895
  what->size += child1->size;
#line 895

#line 895
} while (child1 = child1->next);
#line 896 "filter/f-inst.c"
whati->da = da;
#line 897 "filter/f-inst.c"
if (f1->type && (f1->type != (da.f_type)) && !f_const_promotion(f1, (da.f_type)))
#line 897
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 897
	   f_instruction_name(what->fi_code), f_type_name(da.f_type), f_type_name(f1->type));

#line 959 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 959
    
#line 959
      return what;
#line 959

#line 959
    /* Try to pre-calculate the result */
#line 959
      #undef whati
#line 959
  }
#line 959

#line 959

#line 968 "filter/f-inst.c"
 /* Constructor for FI_EA_UNSET */
#line 968
struct f_inst *
#line 968
f_new_inst_FI_EA_UNSET(enum f_instruction_code fi_code
#line 960 "filter/f-inst.c"
  , struct f_dynamic_attr da
#line 968 "filter/f-inst.c"
)
#line 968
  {
#line 968
    /* Allocate the structure */
#line 968
    struct f_inst *what = fi_new(fi_code);
#line 968
    
#line 968

#line 968
    /* Initialize all the members */
#line 968
  #define whati (&(what->i_FI_EA_UNSET))
#line 968
  #line 960 "filter/f-inst.c"
whati->da = da;

#line 968 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 968
    
#line 968
      return what;
#line 968

#line 968
    /* Try to pre-calculate the result */
#line 968
      #undef whati
#line 968
  }
#line 968

#line 968

#line 980 "filter/f-inst.c"
 /* Constructor for FI_LENGTH */
#line 980
struct f_inst *
#line 969 "filter/f-inst.c"
NONNULL(2)
#line 980 "filter/f-inst.c"
f_new_inst_FI_LENGTH(enum f_instruction_code fi_code
#line 969 "filter/f-inst.c"
  , struct f_inst * f1
#line 980 "filter/f-inst.c"
)
#line 980
  {
#line 980
    /* Allocate the structure */
#line 980
    struct f_inst *what = fi_new(fi_code);
#line 980
    uint constargs = 1;
#line 980

#line 980
    /* Initialize all the members */
#line 980
  #define whati (&(what->i_FI_LENGTH))
#line 980
  #line 969 "filter/f-inst.c"
whati->f1 = f1;
#line 969
const struct f_inst *child1 = f1;
#line 969
do {
#line 969
  what->size += child1->size;
#line 969

#line 969
  if (child1->fi_code != FI_CONSTANT)
#line 969
    constargs = 0;
#line 969

#line 969
} while (child1 = child1->next);
#line 971 "filter/f-inst.c"
what->type = T_INT;

#line 980 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 980
    if (!constargs)
#line 980
      return what;
#line 980

#line 980
    /* Try to pre-calculate the result */
#line 980
    #line 968 "filter/f-inst.c"
 {	/* Get length of */
    
#line 969 "filter/f-inst.c"
;
    switch(v1.type) {
    case T_NET:     
#line 971 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = net_pxlen(v1.val.net) } ); break;
    case T_PATH:    return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = as_path_getlen(v1.val.ad) } ); break;
    case T_CLIST:   return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = int_set_get_size(v1.val.ad) } ); break;
    case T_ECLIST:  return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = ec_set_get_size(v1.val.ad) } ); break;
    case T_LCLIST:  return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = lc_set_get_size(v1.val.ad) } ); break;
    default: runtime( "Prefix, path, clist or eclist expected" );
    }
  }

  
  #undef whati
#line 980 "filter/f-inst.c"
  }
#line 980

#line 980

#line 1015 "filter/f-inst.c"
 /* Constructor for FI_NET_SRC */
#line 1015
struct f_inst *
#line 981 "filter/f-inst.c"
NONNULL(2)
#line 1015 "filter/f-inst.c"
f_new_inst_FI_NET_SRC(enum f_instruction_code fi_code
#line 981 "filter/f-inst.c"
  , struct f_inst * f1
#line 1015 "filter/f-inst.c"
)
#line 1015
  {
#line 1015
    /* Allocate the structure */
#line 1015
    struct f_inst *what = fi_new(fi_code);
#line 1015
    uint constargs = 1;
#line 1015

#line 1015
    /* Initialize all the members */
#line 1015
  #define whati (&(what->i_FI_NET_SRC))
#line 1015
  #line 981 "filter/f-inst.c"
whati->f1 = f1;
#line 981
const struct f_inst *child1 = f1;
#line 981
do {
#line 981
  what->size += child1->size;
#line 981

#line 981
  if (child1->fi_code != FI_CONSTANT)
#line 981
    constargs = 0;
#line 981

#line 981
} while (child1 = child1->next);
#line 981 "filter/f-inst.c"
if (f1->type && (f1->type != (T_NET)) && !f_const_promotion(f1, (T_NET)))
#line 981
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 981
	   f_instruction_name(what->fi_code), f_type_name(T_NET), f_type_name(f1->type));
#line 1012 "filter/f-inst.c"
what->type = T_NET;

#line 1015 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1015
    if (!constargs)
#line 1015
      return what;
#line 1015

#line 1015
    /* Try to pre-calculate the result */
#line 1015
    #line 980 "filter/f-inst.c"
 { 	/* Get src prefix */
    
#line 981 "filter/f-inst.c"
 
#line 981 "filter/f-inst.c"
 
#line 981 "filter/f-inst.c"
;

    net_addr_union *net = (void *) v1.val.net;
    net_addr *src = falloc(sizeof(net_addr_ip6));
    const byte *part;

    switch(v1.val.net->type) {
    case NET_FLOW4:
      part = flow4_get_part(&net->flow4, FLOW_TYPE_SRC_PREFIX);
      if (part)
	net_fill_ip4(src, flow_read_ip4_part(part), flow_read_pxlen(part));
      else
	net_fill_ip4(src, IP4_NONE, 0);
      break;

    case NET_FLOW6:
      part = flow6_get_part(&net->flow6, FLOW_TYPE_SRC_PREFIX);
      if (part)
	net_fill_ip6(src, flow_read_ip6_part(part), flow_read_pxlen(part));
      else
	net_fill_ip6(src, IP6_NONE, 0);
      break;

    case NET_IP6_SADR:
      net_fill_ip6(src, net->ip6_sadr.src_prefix, net->ip6_sadr.src_pxlen);
      break;

    default:
      runtime( "Flow or SADR expected" );
    }

     
#line 1012 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_NET, .val.net = src } );
  }

  
  #undef whati
#line 1015 "filter/f-inst.c"
  }
#line 1015

#line 1015

#line 1050 "filter/f-inst.c"
 /* Constructor for FI_NET_DST */
#line 1050
struct f_inst *
#line 1016 "filter/f-inst.c"
NONNULL(2)
#line 1050 "filter/f-inst.c"
f_new_inst_FI_NET_DST(enum f_instruction_code fi_code
#line 1016 "filter/f-inst.c"
  , struct f_inst * f1
#line 1050 "filter/f-inst.c"
)
#line 1050
  {
#line 1050
    /* Allocate the structure */
#line 1050
    struct f_inst *what = fi_new(fi_code);
#line 1050
    uint constargs = 1;
#line 1050

#line 1050
    /* Initialize all the members */
#line 1050
  #define whati (&(what->i_FI_NET_DST))
#line 1050
  #line 1016 "filter/f-inst.c"
whati->f1 = f1;
#line 1016
const struct f_inst *child1 = f1;
#line 1016
do {
#line 1016
  what->size += child1->size;
#line 1016

#line 1016
  if (child1->fi_code != FI_CONSTANT)
#line 1016
    constargs = 0;
#line 1016

#line 1016
} while (child1 = child1->next);
#line 1016 "filter/f-inst.c"
if (f1->type && (f1->type != (T_NET)) && !f_const_promotion(f1, (T_NET)))
#line 1016
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1016
	   f_instruction_name(what->fi_code), f_type_name(T_NET), f_type_name(f1->type));
#line 1047 "filter/f-inst.c"
what->type = T_NET;

#line 1050 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1050
    if (!constargs)
#line 1050
      return what;
#line 1050

#line 1050
    /* Try to pre-calculate the result */
#line 1050
    #line 1015 "filter/f-inst.c"
 { 	/* Get dst prefix */
    
#line 1016 "filter/f-inst.c"
 
#line 1016 "filter/f-inst.c"
 
#line 1016 "filter/f-inst.c"
;

    net_addr_union *net = (void *) v1.val.net;
    net_addr *dst = falloc(sizeof(net_addr_ip6));
    const byte *part;

    switch(v1.val.net->type) {
    case NET_FLOW4:
      part = flow4_get_part(&net->flow4, FLOW_TYPE_DST_PREFIX);
      if (part)
	net_fill_ip4(dst, flow_read_ip4_part(part), flow_read_pxlen(part));
      else
	net_fill_ip4(dst, IP4_NONE, 0);
      break;

    case NET_FLOW6:
      part = flow6_get_part(&net->flow6, FLOW_TYPE_DST_PREFIX);
      if (part)
	net_fill_ip6(dst, flow_read_ip6_part(part), flow_read_pxlen(part));
      else
	net_fill_ip6(dst, IP6_NONE, 0);
      break;

    case NET_IP6_SADR:
      net_fill_ip6(dst, net->ip6_sadr.dst_prefix, net->ip6_sadr.dst_pxlen);
      break;

    default:
      runtime( "Flow or SADR expected" );
    }

     
#line 1047 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_NET, .val.net = dst } );
  }

  
  #undef whati
#line 1050 "filter/f-inst.c"
  }
#line 1050

#line 1050

#line 1060 "filter/f-inst.c"
 /* Constructor for FI_ROA_MAXLEN */
#line 1060
struct f_inst *
#line 1051 "filter/f-inst.c"
NONNULL(2)
#line 1060 "filter/f-inst.c"
f_new_inst_FI_ROA_MAXLEN(enum f_instruction_code fi_code
#line 1051 "filter/f-inst.c"
  , struct f_inst * f1
#line 1060 "filter/f-inst.c"
)
#line 1060
  {
#line 1060
    /* Allocate the structure */
#line 1060
    struct f_inst *what = fi_new(fi_code);
#line 1060
    uint constargs = 1;
#line 1060

#line 1060
    /* Initialize all the members */
#line 1060
  #define whati (&(what->i_FI_ROA_MAXLEN))
#line 1060
  #line 1051 "filter/f-inst.c"
whati->f1 = f1;
#line 1051
const struct f_inst *child1 = f1;
#line 1051
do {
#line 1051
  what->size += child1->size;
#line 1051

#line 1051
  if (child1->fi_code != FI_CONSTANT)
#line 1051
    constargs = 0;
#line 1051

#line 1051
} while (child1 = child1->next);
#line 1051 "filter/f-inst.c"
if (f1->type && (f1->type != (T_NET)) && !f_const_promotion(f1, (T_NET)))
#line 1051
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1051
	   f_instruction_name(what->fi_code), f_type_name(T_NET), f_type_name(f1->type));
#line 1055 "filter/f-inst.c"
what->type = T_INT;

#line 1060 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1060
    if (!constargs)
#line 1060
      return what;
#line 1060

#line 1060
    /* Try to pre-calculate the result */
#line 1060
    #line 1050 "filter/f-inst.c"
 { 	/* Get ROA max prefix length */
    
#line 1051 "filter/f-inst.c"
 
#line 1051 "filter/f-inst.c"
 
#line 1051 "filter/f-inst.c"
;
    if (!net_is_roa(v1.val.net))
      runtime( "ROA expected" );

     
#line 1055 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = (v1.val.net->type == NET_ROA4) ?
#line 1055
      ((net_addr_roa4 *) v1.val.net)->max_pxlen :
#line 1055
      ((net_addr_roa6 *) v1.val.net)->max_pxlen } );
#line 1058
  }

  
  #undef whati
#line 1060 "filter/f-inst.c"
  }
#line 1060

#line 1060

#line 1087 "filter/f-inst.c"
 /* Constructor for FI_ASN */
#line 1087
struct f_inst *
#line 1061 "filter/f-inst.c"
NONNULL(2)
#line 1087 "filter/f-inst.c"
f_new_inst_FI_ASN(enum f_instruction_code fi_code
#line 1061 "filter/f-inst.c"
  , struct f_inst * f1
#line 1087 "filter/f-inst.c"
)
#line 1087
  {
#line 1087
    /* Allocate the structure */
#line 1087
    struct f_inst *what = fi_new(fi_code);
#line 1087
    uint constargs = 1;
#line 1087

#line 1087
    /* Initialize all the members */
#line 1087
  #define whati (&(what->i_FI_ASN))
#line 1087
  #line 1061 "filter/f-inst.c"
whati->f1 = f1;
#line 1061
const struct f_inst *child1 = f1;
#line 1061
do {
#line 1061
  what->size += child1->size;
#line 1061

#line 1061
  if (child1->fi_code != FI_CONSTANT)
#line 1061
    constargs = 0;
#line 1061

#line 1061
} while (child1 = child1->next);
#line 1062 "filter/f-inst.c"
what->type = T_INT;

#line 1087 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1087
    if (!constargs)
#line 1087
      return what;
#line 1087

#line 1087
    /* Try to pre-calculate the result */
#line 1087
    #line 1060 "filter/f-inst.c"
 { 	/* Get ROA ASN or community ASN part */
    
#line 1061 "filter/f-inst.c"
;
     
#line 1062 "filter/f-inst.c"
;
    switch(v1.type)
    {
      case T_NET:
        if (!net_is_roa(v1.val.net))
          runtime( "ROA expected" );

        return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = (v1.val.net->type == NET_ROA4) ?
#line 1069
          ((net_addr_roa4 *) v1.val.net)->asn :
#line 1069
          ((net_addr_roa6 *) v1.val.net)->asn } );
#line 1072
        break;

      case T_PAIR:
        return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = v1.val.i >> 16 } );
        break;

      case T_LC:
        return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = v1.val.lc.asn } );
        break;

      default:
        runtime( "Net, pair or lc expected" );
    }
  }

  
  #undef whati
#line 1087 "filter/f-inst.c"
  }
#line 1087

#line 1087

#line 1092 "filter/f-inst.c"
 /* Constructor for FI_IP */
#line 1092
struct f_inst *
#line 1088 "filter/f-inst.c"
NONNULL(2)
#line 1092 "filter/f-inst.c"
f_new_inst_FI_IP(enum f_instruction_code fi_code
#line 1088 "filter/f-inst.c"
  , struct f_inst * f1
#line 1092 "filter/f-inst.c"
)
#line 1092
  {
#line 1092
    /* Allocate the structure */
#line 1092
    struct f_inst *what = fi_new(fi_code);
#line 1092
    uint constargs = 1;
#line 1092

#line 1092
    /* Initialize all the members */
#line 1092
  #define whati (&(what->i_FI_IP))
#line 1092
  #line 1088 "filter/f-inst.c"
whati->f1 = f1;
#line 1088
const struct f_inst *child1 = f1;
#line 1088
do {
#line 1088
  what->size += child1->size;
#line 1088

#line 1088
  if (child1->fi_code != FI_CONSTANT)
#line 1088
    constargs = 0;
#line 1088

#line 1088
} while (child1 = child1->next);
#line 1088 "filter/f-inst.c"
if (f1->type && (f1->type != (T_NET)) && !f_const_promotion(f1, (T_NET)))
#line 1088
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1088
	   f_instruction_name(what->fi_code), f_type_name(T_NET), f_type_name(f1->type));
#line 1089 "filter/f-inst.c"
what->type = T_IP;

#line 1092 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1092
    if (!constargs)
#line 1092
      return what;
#line 1092

#line 1092
    /* Try to pre-calculate the result */
#line 1092
    #line 1087 "filter/f-inst.c"
 {	/* Convert prefix to ... */
    
#line 1088 "filter/f-inst.c"
 
#line 1088 "filter/f-inst.c"
 
#line 1088 "filter/f-inst.c"
;
     
#line 1089 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_IP, .val.ip = net_prefix(v1.val.net) } );
  }

  
  #undef whati
#line 1092 "filter/f-inst.c"
  }
#line 1092

#line 1092

#line 1099 "filter/f-inst.c"
 /* Constructor for FI_ROUTE_DISTINGUISHER */
#line 1099
struct f_inst *
#line 1093 "filter/f-inst.c"
NONNULL(2)
#line 1099 "filter/f-inst.c"
f_new_inst_FI_ROUTE_DISTINGUISHER(enum f_instruction_code fi_code
#line 1093 "filter/f-inst.c"
  , struct f_inst * f1
#line 1099 "filter/f-inst.c"
)
#line 1099
  {
#line 1099
    /* Allocate the structure */
#line 1099
    struct f_inst *what = fi_new(fi_code);
#line 1099
    uint constargs = 1;
#line 1099

#line 1099
    /* Initialize all the members */
#line 1099
  #define whati (&(what->i_FI_ROUTE_DISTINGUISHER))
#line 1099
  #line 1093 "filter/f-inst.c"
whati->f1 = f1;
#line 1093
const struct f_inst *child1 = f1;
#line 1093
do {
#line 1093
  what->size += child1->size;
#line 1093

#line 1093
  if (child1->fi_code != FI_CONSTANT)
#line 1093
    constargs = 0;
#line 1093

#line 1093
} while (child1 = child1->next);
#line 1093 "filter/f-inst.c"
if (f1->type && (f1->type != (T_NET)) && !f_const_promotion(f1, (T_NET)))
#line 1093
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1093
	   f_instruction_name(what->fi_code), f_type_name(T_NET), f_type_name(f1->type));
#line 1096 "filter/f-inst.c"
what->type = T_RD;

#line 1099 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1099
    if (!constargs)
#line 1099
      return what;
#line 1099

#line 1099
    /* Try to pre-calculate the result */
#line 1099
    #line 1092 "filter/f-inst.c"
 {
    
#line 1093 "filter/f-inst.c"
 
#line 1093 "filter/f-inst.c"
 
#line 1093 "filter/f-inst.c"
;
    if (!net_is_vpn(v1.val.net))
      runtime( "VPN address expected" );
     
#line 1096 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_RD, .val.ec = net_rd(v1.val.net) } );
  }

  
  #undef whati
#line 1099 "filter/f-inst.c"
  }
#line 1099

#line 1099

#line 1106 "filter/f-inst.c"
 /* Constructor for FI_AS_PATH_FIRST */
#line 1106
struct f_inst *
#line 1100 "filter/f-inst.c"
NONNULL(2)
#line 1106 "filter/f-inst.c"
f_new_inst_FI_AS_PATH_FIRST(enum f_instruction_code fi_code
#line 1100 "filter/f-inst.c"
  , struct f_inst * f1
#line 1106 "filter/f-inst.c"
)
#line 1106
  {
#line 1106
    /* Allocate the structure */
#line 1106
    struct f_inst *what = fi_new(fi_code);
#line 1106
    uint constargs = 1;
#line 1106

#line 1106
    /* Initialize all the members */
#line 1106
  #define whati (&(what->i_FI_AS_PATH_FIRST))
#line 1106
  #line 1100 "filter/f-inst.c"
whati->f1 = f1;
#line 1100
const struct f_inst *child1 = f1;
#line 1100
do {
#line 1100
  what->size += child1->size;
#line 1100

#line 1100
  if (child1->fi_code != FI_CONSTANT)
#line 1100
    constargs = 0;
#line 1100

#line 1100
} while (child1 = child1->next);
#line 1100 "filter/f-inst.c"
if (f1->type && (f1->type != (T_PATH)) && !f_const_promotion(f1, (T_PATH)))
#line 1100
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1100
	   f_instruction_name(what->fi_code), f_type_name(T_PATH), f_type_name(f1->type));
#line 1103 "filter/f-inst.c"
what->type = T_INT;

#line 1106 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1106
    if (!constargs)
#line 1106
      return what;
#line 1106

#line 1106
    /* Try to pre-calculate the result */
#line 1106
    #line 1099 "filter/f-inst.c"
 {	/* Get first ASN from AS PATH */
    
#line 1100 "filter/f-inst.c"
 
#line 1100 "filter/f-inst.c"
 
#line 1100 "filter/f-inst.c"
;
    u32 as = 0;
    as_path_get_first(v1.val.ad, &as);
     
#line 1103 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = as } );
  }

  
  #undef whati
#line 1106 "filter/f-inst.c"
  }
#line 1106

#line 1106

#line 1113 "filter/f-inst.c"
 /* Constructor for FI_AS_PATH_LAST */
#line 1113
struct f_inst *
#line 1107 "filter/f-inst.c"
NONNULL(2)
#line 1113 "filter/f-inst.c"
f_new_inst_FI_AS_PATH_LAST(enum f_instruction_code fi_code
#line 1107 "filter/f-inst.c"
  , struct f_inst * f1
#line 1113 "filter/f-inst.c"
)
#line 1113
  {
#line 1113
    /* Allocate the structure */
#line 1113
    struct f_inst *what = fi_new(fi_code);
#line 1113
    uint constargs = 1;
#line 1113

#line 1113
    /* Initialize all the members */
#line 1113
  #define whati (&(what->i_FI_AS_PATH_LAST))
#line 1113
  #line 1107 "filter/f-inst.c"
whati->f1 = f1;
#line 1107
const struct f_inst *child1 = f1;
#line 1107
do {
#line 1107
  what->size += child1->size;
#line 1107

#line 1107
  if (child1->fi_code != FI_CONSTANT)
#line 1107
    constargs = 0;
#line 1107

#line 1107
} while (child1 = child1->next);
#line 1107 "filter/f-inst.c"
if (f1->type && (f1->type != (T_PATH)) && !f_const_promotion(f1, (T_PATH)))
#line 1107
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1107
	   f_instruction_name(what->fi_code), f_type_name(T_PATH), f_type_name(f1->type));
#line 1110 "filter/f-inst.c"
what->type = T_INT;

#line 1113 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1113
    if (!constargs)
#line 1113
      return what;
#line 1113

#line 1113
    /* Try to pre-calculate the result */
#line 1113
    #line 1106 "filter/f-inst.c"
 {		/* Get last ASN from AS PATH */
    
#line 1107 "filter/f-inst.c"
 
#line 1107 "filter/f-inst.c"
 
#line 1107 "filter/f-inst.c"
;
    u32 as = 0;
    as_path_get_last(v1.val.ad, &as);
     
#line 1110 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = as } );
  }

  
  #undef whati
#line 1113 "filter/f-inst.c"
  }
#line 1113

#line 1113

#line 1118 "filter/f-inst.c"
 /* Constructor for FI_AS_PATH_LAST_NAG */
#line 1118
struct f_inst *
#line 1114 "filter/f-inst.c"
NONNULL(2)
#line 1118 "filter/f-inst.c"
f_new_inst_FI_AS_PATH_LAST_NAG(enum f_instruction_code fi_code
#line 1114 "filter/f-inst.c"
  , struct f_inst * f1
#line 1118 "filter/f-inst.c"
)
#line 1118
  {
#line 1118
    /* Allocate the structure */
#line 1118
    struct f_inst *what = fi_new(fi_code);
#line 1118
    uint constargs = 1;
#line 1118

#line 1118
    /* Initialize all the members */
#line 1118
  #define whati (&(what->i_FI_AS_PATH_LAST_NAG))
#line 1118
  #line 1114 "filter/f-inst.c"
whati->f1 = f1;
#line 1114
const struct f_inst *child1 = f1;
#line 1114
do {
#line 1114
  what->size += child1->size;
#line 1114

#line 1114
  if (child1->fi_code != FI_CONSTANT)
#line 1114
    constargs = 0;
#line 1114

#line 1114
} while (child1 = child1->next);
#line 1114 "filter/f-inst.c"
if (f1->type && (f1->type != (T_PATH)) && !f_const_promotion(f1, (T_PATH)))
#line 1114
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1114
	   f_instruction_name(what->fi_code), f_type_name(T_PATH), f_type_name(f1->type));
#line 1115 "filter/f-inst.c"
what->type = T_INT;

#line 1118 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1118
    if (!constargs)
#line 1118
      return what;
#line 1118

#line 1118
    /* Try to pre-calculate the result */
#line 1118
    #line 1113 "filter/f-inst.c"
 {	/* Get last ASN from non-aggregated part of AS PATH */
    
#line 1114 "filter/f-inst.c"
 
#line 1114 "filter/f-inst.c"
 
#line 1114 "filter/f-inst.c"
;
     
#line 1115 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = as_path_get_last_nonaggregated(v1.val.ad) } );
  }

  
  #undef whati
#line 1118 "filter/f-inst.c"
  }
#line 1118

#line 1118

#line 1123 "filter/f-inst.c"
 /* Constructor for FI_PAIR_DATA */
#line 1123
struct f_inst *
#line 1119 "filter/f-inst.c"
NONNULL(2)
#line 1123 "filter/f-inst.c"
f_new_inst_FI_PAIR_DATA(enum f_instruction_code fi_code
#line 1119 "filter/f-inst.c"
  , struct f_inst * f1
#line 1123 "filter/f-inst.c"
)
#line 1123
  {
#line 1123
    /* Allocate the structure */
#line 1123
    struct f_inst *what = fi_new(fi_code);
#line 1123
    uint constargs = 1;
#line 1123

#line 1123
    /* Initialize all the members */
#line 1123
  #define whati (&(what->i_FI_PAIR_DATA))
#line 1123
  #line 1119 "filter/f-inst.c"
whati->f1 = f1;
#line 1119
const struct f_inst *child1 = f1;
#line 1119
do {
#line 1119
  what->size += child1->size;
#line 1119

#line 1119
  if (child1->fi_code != FI_CONSTANT)
#line 1119
    constargs = 0;
#line 1119

#line 1119
} while (child1 = child1->next);
#line 1119 "filter/f-inst.c"
if (f1->type && (f1->type != (T_PAIR)) && !f_const_promotion(f1, (T_PAIR)))
#line 1119
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1119
	   f_instruction_name(what->fi_code), f_type_name(T_PAIR), f_type_name(f1->type));
#line 1120 "filter/f-inst.c"
what->type = T_INT;

#line 1123 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1123
    if (!constargs)
#line 1123
      return what;
#line 1123

#line 1123
    /* Try to pre-calculate the result */
#line 1123
    #line 1118 "filter/f-inst.c"
 {	/* Get data part from the standard community */
    
#line 1119 "filter/f-inst.c"
 
#line 1119 "filter/f-inst.c"
 
#line 1119 "filter/f-inst.c"
;
     
#line 1120 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = v1.val.i & 0xFFFF } );
  }

  
  #undef whati
#line 1123 "filter/f-inst.c"
  }
#line 1123

#line 1123

#line 1128 "filter/f-inst.c"
 /* Constructor for FI_LC_DATA1 */
#line 1128
struct f_inst *
#line 1124 "filter/f-inst.c"
NONNULL(2)
#line 1128 "filter/f-inst.c"
f_new_inst_FI_LC_DATA1(enum f_instruction_code fi_code
#line 1124 "filter/f-inst.c"
  , struct f_inst * f1
#line 1128 "filter/f-inst.c"
)
#line 1128
  {
#line 1128
    /* Allocate the structure */
#line 1128
    struct f_inst *what = fi_new(fi_code);
#line 1128
    uint constargs = 1;
#line 1128

#line 1128
    /* Initialize all the members */
#line 1128
  #define whati (&(what->i_FI_LC_DATA1))
#line 1128
  #line 1124 "filter/f-inst.c"
whati->f1 = f1;
#line 1124
const struct f_inst *child1 = f1;
#line 1124
do {
#line 1124
  what->size += child1->size;
#line 1124

#line 1124
  if (child1->fi_code != FI_CONSTANT)
#line 1124
    constargs = 0;
#line 1124

#line 1124
} while (child1 = child1->next);
#line 1124 "filter/f-inst.c"
if (f1->type && (f1->type != (T_LC)) && !f_const_promotion(f1, (T_LC)))
#line 1124
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1124
	   f_instruction_name(what->fi_code), f_type_name(T_LC), f_type_name(f1->type));
#line 1125 "filter/f-inst.c"
what->type = T_INT;

#line 1128 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1128
    if (!constargs)
#line 1128
      return what;
#line 1128

#line 1128
    /* Try to pre-calculate the result */
#line 1128
    #line 1123 "filter/f-inst.c"
 {	/* Get data1 part from the large community */
    
#line 1124 "filter/f-inst.c"
 
#line 1124 "filter/f-inst.c"
 
#line 1124 "filter/f-inst.c"
;
     
#line 1125 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = v1.val.lc.ldp1 } );
  }

  
  #undef whati
#line 1128 "filter/f-inst.c"
  }
#line 1128

#line 1128

#line 1133 "filter/f-inst.c"
 /* Constructor for FI_LC_DATA2 */
#line 1133
struct f_inst *
#line 1129 "filter/f-inst.c"
NONNULL(2)
#line 1133 "filter/f-inst.c"
f_new_inst_FI_LC_DATA2(enum f_instruction_code fi_code
#line 1129 "filter/f-inst.c"
  , struct f_inst * f1
#line 1133 "filter/f-inst.c"
)
#line 1133
  {
#line 1133
    /* Allocate the structure */
#line 1133
    struct f_inst *what = fi_new(fi_code);
#line 1133
    uint constargs = 1;
#line 1133

#line 1133
    /* Initialize all the members */
#line 1133
  #define whati (&(what->i_FI_LC_DATA2))
#line 1133
  #line 1129 "filter/f-inst.c"
whati->f1 = f1;
#line 1129
const struct f_inst *child1 = f1;
#line 1129
do {
#line 1129
  what->size += child1->size;
#line 1129

#line 1129
  if (child1->fi_code != FI_CONSTANT)
#line 1129
    constargs = 0;
#line 1129

#line 1129
} while (child1 = child1->next);
#line 1129 "filter/f-inst.c"
if (f1->type && (f1->type != (T_LC)) && !f_const_promotion(f1, (T_LC)))
#line 1129
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1129
	   f_instruction_name(what->fi_code), f_type_name(T_LC), f_type_name(f1->type));
#line 1130 "filter/f-inst.c"
what->type = T_INT;

#line 1133 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1133
    if (!constargs)
#line 1133
      return what;
#line 1133

#line 1133
    /* Try to pre-calculate the result */
#line 1133
    #line 1128 "filter/f-inst.c"
 {	/* Get data2 part from the large community */
    
#line 1129 "filter/f-inst.c"
 
#line 1129 "filter/f-inst.c"
 
#line 1129 "filter/f-inst.c"
;
     
#line 1130 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_INT, .val.i = v1.val.lc.ldp2 } );
  }

  
  #undef whati
#line 1133 "filter/f-inst.c"
  }
#line 1133

#line 1133

#line 1167 "filter/f-inst.c"
 /* Constructor for FI_MIN */
#line 1167
struct f_inst *
#line 1134 "filter/f-inst.c"
NONNULL(2)
#line 1167 "filter/f-inst.c"
f_new_inst_FI_MIN(enum f_instruction_code fi_code
#line 1134 "filter/f-inst.c"
  , struct f_inst * f1
#line 1167 "filter/f-inst.c"
)
#line 1167
  {
#line 1167
    /* Allocate the structure */
#line 1167
    struct f_inst *what = fi_new(fi_code);
#line 1167
    uint constargs = 1;
#line 1167

#line 1167
    /* Initialize all the members */
#line 1167
  #define whati (&(what->i_FI_MIN))
#line 1167
  #line 1134 "filter/f-inst.c"
whati->f1 = f1;
#line 1134
const struct f_inst *child1 = f1;
#line 1134
do {
#line 1134
  what->size += child1->size;
#line 1134

#line 1134
  if (child1->fi_code != FI_CONSTANT)
#line 1134
    constargs = 0;
#line 1134

#line 1134
} while (child1 = child1->next);
#line 1135 "filter/f-inst.c"
what->type = f_type_element_type(v1.type);

#line 1167 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1167
    if (!constargs)
#line 1167
      return what;
#line 1167

#line 1167
    /* Try to pre-calculate the result */
#line 1167
    #line 1133 "filter/f-inst.c"
 {	/* Get minimum element from list */
    
#line 1134 "filter/f-inst.c"
;
     
#line 1135 "filter/f-inst.c"
;
    switch(v1.type)
    {
      case T_CLIST:
        {
          u32 val = 0;
          int_set_min(v1.val.ad, &val);
          return fi_constant(what,  (struct f_val) { .type = T_PAIR, .val.i = val } );
        }
        break;

      case T_ECLIST:
        {
          u64 val = 0;
          ec_set_min(v1.val.ad, &val);
          return fi_constant(what,  (struct f_val) { .type = T_EC, .val.ec = val } );
        }
        break;

      case T_LCLIST:
        {
          lcomm val = { 0, 0, 0 };
          lc_set_min(v1.val.ad, &val);
          return fi_constant(what,  (struct f_val) { .type = T_LC, .val.lc = val } );
        }
        break;

      default:
        runtime( "Clist or lclist expected" );
    }
  }

  
  #undef whati
#line 1167 "filter/f-inst.c"
  }
#line 1167

#line 1167

#line 1201 "filter/f-inst.c"
 /* Constructor for FI_MAX */
#line 1201
struct f_inst *
#line 1168 "filter/f-inst.c"
NONNULL(2)
#line 1201 "filter/f-inst.c"
f_new_inst_FI_MAX(enum f_instruction_code fi_code
#line 1168 "filter/f-inst.c"
  , struct f_inst * f1
#line 1201 "filter/f-inst.c"
)
#line 1201
  {
#line 1201
    /* Allocate the structure */
#line 1201
    struct f_inst *what = fi_new(fi_code);
#line 1201
    uint constargs = 1;
#line 1201

#line 1201
    /* Initialize all the members */
#line 1201
  #define whati (&(what->i_FI_MAX))
#line 1201
  #line 1168 "filter/f-inst.c"
whati->f1 = f1;
#line 1168
const struct f_inst *child1 = f1;
#line 1168
do {
#line 1168
  what->size += child1->size;
#line 1168

#line 1168
  if (child1->fi_code != FI_CONSTANT)
#line 1168
    constargs = 0;
#line 1168

#line 1168
} while (child1 = child1->next);
#line 1169 "filter/f-inst.c"
what->type = f_type_element_type(v1.type);

#line 1201 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1201
    if (!constargs)
#line 1201
      return what;
#line 1201

#line 1201
    /* Try to pre-calculate the result */
#line 1201
    #line 1167 "filter/f-inst.c"
 {	/* Get maximum element from list */
    
#line 1168 "filter/f-inst.c"
;
     
#line 1169 "filter/f-inst.c"
;
    switch(v1.type)
    {
      case T_CLIST:
        {
          u32 val = 0;
          int_set_max(v1.val.ad, &val);
          return fi_constant(what,  (struct f_val) { .type = T_PAIR, .val.i = val } );
        }
        break;

      case T_ECLIST:
        {
          u64 val = 0;
          ec_set_max(v1.val.ad, &val);
          return fi_constant(what,  (struct f_val) { .type = T_EC, .val.ec = val } );
        }
        break;

      case T_LCLIST:
        {
          lcomm val = { 0, 0, 0 };
          lc_set_max(v1.val.ad, &val);
          return fi_constant(what,  (struct f_val) { .type = T_LC, .val.lc = val } );
        }
        break;

      default:
        runtime( "Clist or lclist expected" );
    }
  }

  
  #undef whati
#line 1201 "filter/f-inst.c"
  }
#line 1201

#line 1201

#line 1227 "filter/f-inst.c"
 /* Constructor for FI_RETURN */
#line 1227
struct f_inst *
#line 1204 "filter/f-inst.c"
NONNULL(2)
#line 1227 "filter/f-inst.c"
f_new_inst_FI_RETURN(enum f_instruction_code fi_code
#line 1204 "filter/f-inst.c"
  , struct f_inst * f1
#line 1227 "filter/f-inst.c"
)
#line 1227
  {
#line 1227
    /* Allocate the structure */
#line 1227
    struct f_inst *what = fi_new(fi_code);
#line 1227
    
#line 1227

#line 1227
    /* Initialize all the members */
#line 1227
  #define whati (&(what->i_FI_RETURN))
#line 1227
  #line 1204 "filter/f-inst.c"
whati->f1 = f1;
#line 1204
const struct f_inst *child1 = f1;
#line 1204
do {
#line 1204
  what->size += child1->size;
#line 1204

#line 1204
} while (child1 = child1->next);

#line 1227 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1227
    
#line 1227
      return what;
#line 1227

#line 1227
    /* Try to pre-calculate the result */
#line 1227
      #undef whati
#line 1227
  }
#line 1227

#line 1227

#line 1288 "filter/f-inst.c"
 /* Constructor for FI_CALL */
#line 1288
struct f_inst *
#line 1288
f_new_inst_FI_CALL(enum f_instruction_code fi_code
#line 1229 "filter/f-inst.c"
  , struct f_inst * fvar
#line 1230 "filter/f-inst.c"
  , struct symbol * sym
#line 1288 "filter/f-inst.c"
)
#line 1288
  {
#line 1288
    /* Allocate the structure */
#line 1288
    struct f_inst *what = fi_new(fi_code);
#line 1288
    
#line 1288

#line 1288
    /* Initialize all the members */
#line 1288
  #define whati (&(what->i_FI_CALL))
#line 1288
  #line 1229 "filter/f-inst.c"
whati->varcount = 0;
#line 1229
whati->fvar = fvar;
#line 1229
for (const struct f_inst *child = fvar; child; child = child->next, whati->varcount++) {
#line 1229
  what->size += child->size;
#line 1229

#line 1229
}
#line 1229

#line 1230 "filter/f-inst.c"
whati->sym = sym;
#line 1233 "filter/f-inst.c"
what->type = T_VOID;

#line 1236 "filter/f-inst.c"
    ASSERT(sym->class == SYM_FUNCTION);

    if (whati->varcount != sym->function->args)
      cf_error("Function '%s' expects %u arguments, got %u arguments",
	       sym->name, sym->function->args, whati->varcount);

    /* Typecheck individual arguments */
    struct f_inst *a = fvar;
    struct f_arg *b = sym->function->arg_list;
    for (uint i = 1; a && b; a = a->next, b = b->next, i++)
    {
      enum f_type b_type = b->arg->class & 0xff;

      if (a->type && (a->type != b_type) && !f_const_promotion(a, b_type))
	cf_error("Argument %u of '%s' must be %s, got %s",
		 i, sym->name, f_type_name(b_type), f_type_name(a->type));
    }
    ASSERT(!a && !b);

    /* Add implicit void slot for the return value */
    struct f_inst *tmp = f_new_inst(FI_CONSTANT, (struct f_val) { .type = T_VOID });
    tmp->next = whati->fvar;
    whati->fvar = tmp;
    what->size += tmp->size;

    /* Mark recursive calls, they have dummy f_line */
    if (!sym->function->len)
      what->flags |= FIF_RECURSIVE;

    
#line 1288 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1288
    
#line 1288
      return what;
#line 1288

#line 1288
    /* Try to pre-calculate the result */
#line 1288
      #undef whati
#line 1288
  }
#line 1288

#line 1288

#line 1293 "filter/f-inst.c"
 /* Constructor for FI_DROP_RESULT */
#line 1293
struct f_inst *
#line 1290 "filter/f-inst.c"
NONNULL(2)
#line 1293 "filter/f-inst.c"
f_new_inst_FI_DROP_RESULT(enum f_instruction_code fi_code
#line 1290 "filter/f-inst.c"
  , struct f_inst * f1
#line 1293 "filter/f-inst.c"
)
#line 1293
  {
#line 1293
    /* Allocate the structure */
#line 1293
    struct f_inst *what = fi_new(fi_code);
#line 1293
    
#line 1293

#line 1293
    /* Initialize all the members */
#line 1293
  #define whati (&(what->i_FI_DROP_RESULT))
#line 1293
  #line 1290 "filter/f-inst.c"
whati->f1 = f1;
#line 1290
const struct f_inst *child1 = f1;
#line 1290
do {
#line 1290
  what->size += child1->size;
#line 1290

#line 1290
} while (child1 = child1->next);

#line 1293 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1293
    
#line 1293
      return what;
#line 1293

#line 1293
    /* Try to pre-calculate the result */
#line 1293
      #undef whati
#line 1293
  }
#line 1293

#line 1293

#line 1334 "filter/f-inst.c"
 /* Constructor for FI_SWITCH */
#line 1334
struct f_inst *
#line 1294 "filter/f-inst.c"
NONNULL(2)
#line 1334 "filter/f-inst.c"
f_new_inst_FI_SWITCH(enum f_instruction_code fi_code
#line 1294 "filter/f-inst.c"
  , struct f_inst * f1
#line 1296 "filter/f-inst.c"
  , struct f_tree * tree
#line 1334 "filter/f-inst.c"
)
#line 1334
  {
#line 1334
    /* Allocate the structure */
#line 1334
    struct f_inst *what = fi_new(fi_code);
#line 1334
    uint constargs = 1;
#line 1334

#line 1334
    /* Initialize all the members */
#line 1334
  #define whati (&(what->i_FI_SWITCH))
#line 1334
  #line 1294 "filter/f-inst.c"
whati->f1 = f1;
#line 1294
const struct f_inst *child1 = f1;
#line 1294
do {
#line 1294
  what->size += child1->size;
#line 1294

#line 1294
  if (child1->fi_code != FI_CONSTANT)
#line 1294
    constargs = 0;
#line 1294

#line 1294
} while (child1 = child1->next);
#line 1296 "filter/f-inst.c"
whati->tree = tree;

#line 1334 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1334
    if (!constargs)
#line 1334
      return what;
#line 1334

#line 1334
    /* Try to pre-calculate the result */
#line 1334
    #line 1293 "filter/f-inst.c"
 {
    
#line 1294 "filter/f-inst.c"
;

    #line 1296 "filter/f-inst.c"
;

    
#line 1320 "filter/f-inst.c"
    /* In parse-time use find_tree_linear(), in runtime use find_tree() */
    const struct f_tree *t = find_tree_linear(tree, &v1);
    if (!t) {
      v1.type = T_VOID;
      t = find_tree_linear(tree, &v1);
      if (!t) {
	debug( "No else statement?\n");
	return NULL;
      }
    }

    return t->data ;
#line 1332 "filter/f-inst.c"
  }

  
  #undef whati
#line 1334 "filter/f-inst.c"
  }
#line 1334

#line 1334

#line 1342 "filter/f-inst.c"
 /* Constructor for FI_IP_MASK */
#line 1342
struct f_inst *
#line 1335 "filter/f-inst.c"
NONNULL(2)
#line 1336 "filter/f-inst.c"
NONNULL(3)
#line 1342 "filter/f-inst.c"
f_new_inst_FI_IP_MASK(enum f_instruction_code fi_code
#line 1335 "filter/f-inst.c"
  , struct f_inst * f1
#line 1336 "filter/f-inst.c"
  , struct f_inst * f2
#line 1342 "filter/f-inst.c"
)
#line 1342
  {
#line 1342
    /* Allocate the structure */
#line 1342
    struct f_inst *what = fi_new(fi_code);
#line 1342
    uint constargs = 1;
#line 1342

#line 1342
    /* Initialize all the members */
#line 1342
  #define whati (&(what->i_FI_IP_MASK))
#line 1342
  #line 1335 "filter/f-inst.c"
whati->f1 = f1;
#line 1335
const struct f_inst *child1 = f1;
#line 1335
do {
#line 1335
  what->size += child1->size;
#line 1335

#line 1335
  if (child1->fi_code != FI_CONSTANT)
#line 1335
    constargs = 0;
#line 1335

#line 1335
} while (child1 = child1->next);
#line 1335 "filter/f-inst.c"
if (f1->type && (f1->type != (T_IP)) && !f_const_promotion(f1, (T_IP)))
#line 1335
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1335
	   f_instruction_name(what->fi_code), f_type_name(T_IP), f_type_name(f1->type));
#line 1336 "filter/f-inst.c"
whati->f2 = f2;
#line 1336
const struct f_inst *child2 = f2;
#line 1336
do {
#line 1336
  what->size += child2->size;
#line 1336

#line 1336
  if (child2->fi_code != FI_CONSTANT)
#line 1336
    constargs = 0;
#line 1336

#line 1336
} while (child2 = child2->next);
#line 1336 "filter/f-inst.c"
if (f2->type && (f2->type != (T_INT)) && !f_const_promotion(f2, (T_INT)))
#line 1336
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 1336
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f2->type));
#line 1337 "filter/f-inst.c"
what->type = T_IP;

#line 1342 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1342
    if (!constargs)
#line 1342
      return what;
#line 1342

#line 1342
    /* Try to pre-calculate the result */
#line 1342
    #line 1334 "filter/f-inst.c"
 { /* IP.MASK(val) */
    
#line 1335 "filter/f-inst.c"
 
#line 1335 "filter/f-inst.c"
 
#line 1335 "filter/f-inst.c"
;
    
#line 1336 "filter/f-inst.c"
 
#line 1336 "filter/f-inst.c"
 
#line 1336 "filter/f-inst.c"
;
     
#line 1337 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_IP, .val.ip =  ipa_is_ip4(v1.val.ip) ?
#line 1337
      ipa_from_ip4(ip4_and(ipa_to_ip4(v1.val.ip), ip4_mkmask(v2.val.i))) :
#line 1337
      ipa_from_ip6(ip6_and(ipa_to_ip6(v1.val.ip), ip6_mkmask(v2.val.i)))  } );
#line 1340
  }

  
  #undef whati
#line 1342 "filter/f-inst.c"
  }
#line 1342

#line 1342

#line 1348 "filter/f-inst.c"
 /* Constructor for FI_PATH_PREPEND */
#line 1348
struct f_inst *
#line 1343 "filter/f-inst.c"
NONNULL(2)
#line 1344 "filter/f-inst.c"
NONNULL(3)
#line 1348 "filter/f-inst.c"
f_new_inst_FI_PATH_PREPEND(enum f_instruction_code fi_code
#line 1343 "filter/f-inst.c"
  , struct f_inst * f1
#line 1344 "filter/f-inst.c"
  , struct f_inst * f2
#line 1348 "filter/f-inst.c"
)
#line 1348
  {
#line 1348
    /* Allocate the structure */
#line 1348
    struct f_inst *what = fi_new(fi_code);
#line 1348
    uint constargs = 1;
#line 1348

#line 1348
    /* Initialize all the members */
#line 1348
  #define whati (&(what->i_FI_PATH_PREPEND))
#line 1348
  #line 1343 "filter/f-inst.c"
whati->f1 = f1;
#line 1343
const struct f_inst *child1 = f1;
#line 1343
do {
#line 1343
  what->size += child1->size;
#line 1343

#line 1343
  if (child1->fi_code != FI_CONSTANT)
#line 1343
    constargs = 0;
#line 1343

#line 1343
} while (child1 = child1->next);
#line 1343 "filter/f-inst.c"
if (f1->type && (f1->type != (T_PATH)) && !f_const_promotion(f1, (T_PATH)))
#line 1343
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1343
	   f_instruction_name(what->fi_code), f_type_name(T_PATH), f_type_name(f1->type));
#line 1344 "filter/f-inst.c"
whati->f2 = f2;
#line 1344
const struct f_inst *child2 = f2;
#line 1344
do {
#line 1344
  what->size += child2->size;
#line 1344

#line 1344
  if (child2->fi_code != FI_CONSTANT)
#line 1344
    constargs = 0;
#line 1344

#line 1344
} while (child2 = child2->next);
#line 1344 "filter/f-inst.c"
if (f2->type && (f2->type != (T_INT)) && !f_const_promotion(f2, (T_INT)))
#line 1344
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 1344
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f2->type));
#line 1345 "filter/f-inst.c"
what->type = T_PATH;

#line 1348 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1348
    if (!constargs)
#line 1348
      return what;
#line 1348

#line 1348
    /* Try to pre-calculate the result */
#line 1348
    #line 1342 "filter/f-inst.c"
 {	/* Path prepend */
    
#line 1343 "filter/f-inst.c"
 
#line 1343 "filter/f-inst.c"
 
#line 1343 "filter/f-inst.c"
;
    
#line 1344 "filter/f-inst.c"
 
#line 1344 "filter/f-inst.c"
 
#line 1344 "filter/f-inst.c"
;
     
#line 1345 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_PATH, .val.ad =  as_path_prepend(fpool, v1.val.ad, v2.val.i)  } );
  }

  
  #undef whati
#line 1348 "filter/f-inst.c"
  }
#line 1348

#line 1348

#line 1405 "filter/f-inst.c"
 /* Constructor for FI_CLIST_ADD */
#line 1405
struct f_inst *
#line 1349 "filter/f-inst.c"
NONNULL(2)
#line 1350 "filter/f-inst.c"
NONNULL(3)
#line 1405 "filter/f-inst.c"
f_new_inst_FI_CLIST_ADD(enum f_instruction_code fi_code
#line 1349 "filter/f-inst.c"
  , struct f_inst * f1
#line 1350 "filter/f-inst.c"
  , struct f_inst * f2
#line 1405 "filter/f-inst.c"
)
#line 1405
  {
#line 1405
    /* Allocate the structure */
#line 1405
    struct f_inst *what = fi_new(fi_code);
#line 1405
    uint constargs = 1;
#line 1405

#line 1405
    /* Initialize all the members */
#line 1405
  #define whati (&(what->i_FI_CLIST_ADD))
#line 1405
  #line 1349 "filter/f-inst.c"
whati->f1 = f1;
#line 1349
const struct f_inst *child1 = f1;
#line 1349
do {
#line 1349
  what->size += child1->size;
#line 1349

#line 1349
  if (child1->fi_code != FI_CONSTANT)
#line 1349
    constargs = 0;
#line 1349

#line 1349
} while (child1 = child1->next);
#line 1350 "filter/f-inst.c"
whati->f2 = f2;
#line 1350
const struct f_inst *child2 = f2;
#line 1350
do {
#line 1350
  what->size += child2->size;
#line 1350

#line 1350
  if (child2->fi_code != FI_CONSTANT)
#line 1350
    constargs = 0;
#line 1350

#line 1350
} while (child2 = child2->next);
#line 1351 "filter/f-inst.c"
what->type = f1->type;

#line 1405 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1405
    if (!constargs)
#line 1405
      return what;
#line 1405

#line 1405
    /* Try to pre-calculate the result */
#line 1405
    #line 1348 "filter/f-inst.c"
 {	/* (Extended) Community list add */
    
#line 1349 "filter/f-inst.c"
;
    
#line 1350 "filter/f-inst.c"
;
     
#line 1351 "filter/f-inst.c"
;

    if (v1.type == T_PATH)
      runtime("Can't add to path");

    else if (v1.type == T_CLIST)
    {
      /* Community (or cluster) list */
      struct f_val dummy;

      if ((v2.type == T_PAIR) || (v2.type == T_QUAD))
	return fi_constant(what,  (struct f_val) { .type = T_CLIST, .val.ad =  int_set_add(fpool, v1.val.ad, v2.val.i)  } );
      /* IP->Quad implicit conversion */
      else if (val_is_ip4(&v2))
	return fi_constant(what,  (struct f_val) { .type = T_CLIST, .val.ad =  int_set_add(fpool, v1.val.ad, ipa_to_u32(v2.val.ip))  } );
      else if ((v2.type == T_SET) && clist_set_type(v2.val.t, &dummy))
	runtime("Can't add set");
      else if (v2.type == T_CLIST)
	return fi_constant(what,  (struct f_val) { .type = T_CLIST, .val.ad =  int_set_union(fpool, v1.val.ad, v2.val.ad)  } );
      else
	runtime("Can't add non-pair");
    }

    else if (v1.type == T_ECLIST)
    {
      /* v2.val is either EC or EC-set */
      if ((v2.type == T_SET) && eclist_set_type(v2.val.t))
	runtime("Can't add set");
      else if (v2.type == T_ECLIST)
	return fi_constant(what,  (struct f_val) { .type = T_ECLIST, .val.ad =  ec_set_union(fpool, v1.val.ad, v2.val.ad)  } );
      else if (v2.type != T_EC)
	runtime("Can't add non-ec");
      else
	return fi_constant(what,  (struct f_val) { .type = T_ECLIST, .val.ad =  ec_set_add(fpool, v1.val.ad, v2.val.ec)  } );
    }

    else if (v1.type == T_LCLIST)
    {
      /* v2.val is either LC or LC-set */
      if ((v2.type == T_SET) && lclist_set_type(v2.val.t))
	runtime("Can't add set");
      else if (v2.type == T_LCLIST)
	return fi_constant(what,  (struct f_val) { .type = T_LCLIST, .val.ad =  lc_set_union(fpool, v1.val.ad, v2.val.ad)  } );
      else if (v2.type != T_LC)
	runtime("Can't add non-lc");
      else
	return fi_constant(what,  (struct f_val) { .type = T_LCLIST, .val.ad =  lc_set_add(fpool, v1.val.ad, v2.val.lc)  } );

    }

    else
      runtime("Can't add to non-[e|l]clist");
  }

  
  #undef whati
#line 1405 "filter/f-inst.c"
  }
#line 1405

#line 1405

#line 1460 "filter/f-inst.c"
 /* Constructor for FI_CLIST_DEL */
#line 1460
struct f_inst *
#line 1406 "filter/f-inst.c"
NONNULL(2)
#line 1407 "filter/f-inst.c"
NONNULL(3)
#line 1460 "filter/f-inst.c"
f_new_inst_FI_CLIST_DEL(enum f_instruction_code fi_code
#line 1406 "filter/f-inst.c"
  , struct f_inst * f1
#line 1407 "filter/f-inst.c"
  , struct f_inst * f2
#line 1460 "filter/f-inst.c"
)
#line 1460
  {
#line 1460
    /* Allocate the structure */
#line 1460
    struct f_inst *what = fi_new(fi_code);
#line 1460
    uint constargs = 1;
#line 1460

#line 1460
    /* Initialize all the members */
#line 1460
  #define whati (&(what->i_FI_CLIST_DEL))
#line 1460
  #line 1406 "filter/f-inst.c"
whati->f1 = f1;
#line 1406
const struct f_inst *child1 = f1;
#line 1406
do {
#line 1406
  what->size += child1->size;
#line 1406

#line 1406
  if (child1->fi_code != FI_CONSTANT)
#line 1406
    constargs = 0;
#line 1406

#line 1406
} while (child1 = child1->next);
#line 1407 "filter/f-inst.c"
whati->f2 = f2;
#line 1407
const struct f_inst *child2 = f2;
#line 1407
do {
#line 1407
  what->size += child2->size;
#line 1407

#line 1407
  if (child2->fi_code != FI_CONSTANT)
#line 1407
    constargs = 0;
#line 1407

#line 1407
} while (child2 = child2->next);
#line 1408 "filter/f-inst.c"
what->type = f1->type;

#line 1460 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1460
    if (!constargs)
#line 1460
      return what;
#line 1460

#line 1460
    /* Try to pre-calculate the result */
#line 1460
    #line 1405 "filter/f-inst.c"
 {	/* (Extended) Community list add or delete */
    
#line 1406 "filter/f-inst.c"
;
    
#line 1407 "filter/f-inst.c"
;
     
#line 1408 "filter/f-inst.c"
;

    if (v1.type == T_PATH)
    {
      if ((v2.type == T_SET) && path_set_type(v2.val.t) || (v2.type == T_INT))
	return fi_constant(what,  (struct f_val) { .type = T_PATH, .val.ad =  as_path_filter(fpool, v1.val.ad, &v2, 0)  } );
      else
	runtime("Can't delete non-integer (set)");
    }

    else if (v1.type == T_CLIST)
    {
      /* Community (or cluster) list */
      struct f_val dummy;

      if ((v2.type == T_PAIR) || (v2.type == T_QUAD))
	return fi_constant(what,  (struct f_val) { .type = T_CLIST, .val.ad =  int_set_del(fpool, v1.val.ad, v2.val.i)  } );
      /* IP->Quad implicit conversion */
      else if (val_is_ip4(&v2))
	return fi_constant(what,  (struct f_val) { .type = T_CLIST, .val.ad =  int_set_del(fpool, v1.val.ad, ipa_to_u32(v2.val.ip))  } );
      else if ((v2.type == T_SET) && clist_set_type(v2.val.t, &dummy) || (v2.type == T_CLIST))
	return fi_constant(what,  (struct f_val) { .type = T_CLIST, .val.ad =  clist_filter(fpool, v1.val.ad, &v2, 0)  } );
      else
	runtime("Can't delete non-pair");
    }

    else if (v1.type == T_ECLIST)
    {
      /* v2.val is either EC or EC-set */
      if ((v2.type == T_SET) && eclist_set_type(v2.val.t) || (v2.type == T_ECLIST))
	return fi_constant(what,  (struct f_val) { .type = T_ECLIST, .val.ad =  eclist_filter(fpool, v1.val.ad, &v2, 0)  } );
      else if (v2.type != T_EC)
	runtime("Can't delete non-ec");
      else
	return fi_constant(what,  (struct f_val) { .type = T_ECLIST, .val.ad =  ec_set_del(fpool, v1.val.ad, v2.val.ec)  } );
    }

    else if (v1.type == T_LCLIST)
    {
      /* v2.val is either LC or LC-set */
      if ((v2.type == T_SET) && lclist_set_type(v2.val.t) || (v2.type == T_LCLIST))
	return fi_constant(what,  (struct f_val) { .type = T_LCLIST, .val.ad =  lclist_filter(fpool, v1.val.ad, &v2, 0)  } );
      else if (v2.type != T_LC)
	runtime("Can't delete non-lc");
      else
	return fi_constant(what,  (struct f_val) { .type = T_LCLIST, .val.ad =  lc_set_del(fpool, v1.val.ad, v2.val.lc)  } );
    }

    else
      runtime("Can't delete in non-[e|l]clist");
  }

  
  #undef whati
#line 1460 "filter/f-inst.c"
  }
#line 1460

#line 1460

#line 1506 "filter/f-inst.c"
 /* Constructor for FI_CLIST_FILTER */
#line 1506
struct f_inst *
#line 1461 "filter/f-inst.c"
NONNULL(2)
#line 1462 "filter/f-inst.c"
NONNULL(3)
#line 1506 "filter/f-inst.c"
f_new_inst_FI_CLIST_FILTER(enum f_instruction_code fi_code
#line 1461 "filter/f-inst.c"
  , struct f_inst * f1
#line 1462 "filter/f-inst.c"
  , struct f_inst * f2
#line 1506 "filter/f-inst.c"
)
#line 1506
  {
#line 1506
    /* Allocate the structure */
#line 1506
    struct f_inst *what = fi_new(fi_code);
#line 1506
    uint constargs = 1;
#line 1506

#line 1506
    /* Initialize all the members */
#line 1506
  #define whati (&(what->i_FI_CLIST_FILTER))
#line 1506
  #line 1461 "filter/f-inst.c"
whati->f1 = f1;
#line 1461
const struct f_inst *child1 = f1;
#line 1461
do {
#line 1461
  what->size += child1->size;
#line 1461

#line 1461
  if (child1->fi_code != FI_CONSTANT)
#line 1461
    constargs = 0;
#line 1461

#line 1461
} while (child1 = child1->next);
#line 1462 "filter/f-inst.c"
whati->f2 = f2;
#line 1462
const struct f_inst *child2 = f2;
#line 1462
do {
#line 1462
  what->size += child2->size;
#line 1462

#line 1462
  if (child2->fi_code != FI_CONSTANT)
#line 1462
    constargs = 0;
#line 1462

#line 1462
} while (child2 = child2->next);
#line 1463 "filter/f-inst.c"
what->type = f1->type;

#line 1506 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1506
    if (!constargs)
#line 1506
      return what;
#line 1506

#line 1506
    /* Try to pre-calculate the result */
#line 1506
    #line 1460 "filter/f-inst.c"
 {	/* (Extended) Community list add or delete */
    
#line 1461 "filter/f-inst.c"
;
    
#line 1462 "filter/f-inst.c"
;
     
#line 1463 "filter/f-inst.c"
;

    if (v1.type == T_PATH)
    {
      if ((v2.type == T_SET) && path_set_type(v2.val.t))
	return fi_constant(what,  (struct f_val) { .type = T_PATH, .val.ad =  as_path_filter(fpool, v1.val.ad, &v2, 1)  } );
      else
	runtime("Can't filter integer");
    }

    else if (v1.type == T_CLIST)
    {
      /* Community (or cluster) list */
      struct f_val dummy;

      if ((v2.type == T_SET) && clist_set_type(v2.val.t, &dummy) || (v2.type == T_CLIST))
	return fi_constant(what,  (struct f_val) { .type = T_CLIST, .val.ad =  clist_filter(fpool, v1.val.ad, &v2, 1)  } );
      else
	runtime("Can't filter pair");
    }

    else if (v1.type == T_ECLIST)
    {
      /* v2.val is either EC or EC-set */
      if ((v2.type == T_SET) && eclist_set_type(v2.val.t) || (v2.type == T_ECLIST))
	return fi_constant(what,  (struct f_val) { .type = T_ECLIST, .val.ad =  eclist_filter(fpool, v1.val.ad, &v2, 1)  } );
      else
	runtime("Can't filter ec");
    }

    else if (v1.type == T_LCLIST)
    {
      /* v2.val is either LC or LC-set */
      if ((v2.type == T_SET) && lclist_set_type(v2.val.t) || (v2.type == T_LCLIST))
	return fi_constant(what,  (struct f_val) { .type = T_LCLIST, .val.ad =  lclist_filter(fpool, v1.val.ad, &v2, 1)  } );
      else
	runtime("Can't filter lc");
    }

    else
      runtime("Can't filter non-[e|l]clist");
  }

  
  #undef whati
#line 1506 "filter/f-inst.c"
  }
#line 1506

#line 1506

#line 1536 "filter/f-inst.c"
 /* Constructor for FI_ROA_CHECK_IMPLICIT */
#line 1536
struct f_inst *
#line 1536
f_new_inst_FI_ROA_CHECK_IMPLICIT(enum f_instruction_code fi_code
#line 1508 "filter/f-inst.c"
  , struct rtable_config * rtc
#line 1536 "filter/f-inst.c"
)
#line 1536
  {
#line 1536
    /* Allocate the structure */
#line 1536
    struct f_inst *what = fi_new(fi_code);
#line 1536
    
#line 1536

#line 1536
    /* Initialize all the members */
#line 1536
  #define whati (&(what->i_FI_ROA_CHECK_IMPLICIT))
#line 1536
  #line 1508 "filter/f-inst.c"
whati->rtc = rtc;
#line 1531 "filter/f-inst.c"
what->type = T_ENUM_ROA;

#line 1536 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1536
    
#line 1536
      return what;
#line 1536

#line 1536
    /* Try to pre-calculate the result */
#line 1536
      #undef whati
#line 1536
  }
#line 1536

#line 1536

#line 1558 "filter/f-inst.c"
 /* Constructor for FI_ROA_CHECK_EXPLICIT */
#line 1558
struct f_inst *
#line 1538 "filter/f-inst.c"
NONNULL(2)
#line 1539 "filter/f-inst.c"
NONNULL(3)
#line 1558 "filter/f-inst.c"
f_new_inst_FI_ROA_CHECK_EXPLICIT(enum f_instruction_code fi_code
#line 1538 "filter/f-inst.c"
  , struct f_inst * f1
#line 1539 "filter/f-inst.c"
  , struct f_inst * f2
#line 1540 "filter/f-inst.c"
  , struct rtable_config * rtc
#line 1558 "filter/f-inst.c"
)
#line 1558
  {
#line 1558
    /* Allocate the structure */
#line 1558
    struct f_inst *what = fi_new(fi_code);
#line 1558
    
#line 1558

#line 1558
    /* Initialize all the members */
#line 1558
  #define whati (&(what->i_FI_ROA_CHECK_EXPLICIT))
#line 1558
  #line 1538 "filter/f-inst.c"
whati->f1 = f1;
#line 1538
const struct f_inst *child1 = f1;
#line 1538
do {
#line 1538
  what->size += child1->size;
#line 1538

#line 1538
} while (child1 = child1->next);
#line 1538 "filter/f-inst.c"
if (f1->type && (f1->type != (T_NET)) && !f_const_promotion(f1, (T_NET)))
#line 1538
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1538
	   f_instruction_name(what->fi_code), f_type_name(T_NET), f_type_name(f1->type));
#line 1539 "filter/f-inst.c"
whati->f2 = f2;
#line 1539
const struct f_inst *child2 = f2;
#line 1539
do {
#line 1539
  what->size += child2->size;
#line 1539

#line 1539
} while (child2 = child2->next);
#line 1539 "filter/f-inst.c"
if (f2->type && (f2->type != (T_INT)) && !f_const_promotion(f2, (T_INT)))
#line 1539
  cf_error("Argument 2 of %s must be of type %s, got type %s",
#line 1539
	   f_instruction_name(what->fi_code), f_type_name(T_INT), f_type_name(f2->type));
#line 1540 "filter/f-inst.c"
whati->rtc = rtc;
#line 1552 "filter/f-inst.c"
what->type = T_ENUM_ROA;

#line 1558 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1558
    
#line 1558
      return what;
#line 1558

#line 1558
    /* Try to pre-calculate the result */
#line 1558
      #undef whati
#line 1558
  }
#line 1558

#line 1558

#line 1563 "filter/f-inst.c"
 /* Constructor for FI_FORMAT */
#line 1563
struct f_inst *
#line 1559 "filter/f-inst.c"
NONNULL(2)
#line 1563 "filter/f-inst.c"
f_new_inst_FI_FORMAT(enum f_instruction_code fi_code
#line 1559 "filter/f-inst.c"
  , struct f_inst * f1
#line 1563 "filter/f-inst.c"
)
#line 1563
  {
#line 1563
    /* Allocate the structure */
#line 1563
    struct f_inst *what = fi_new(fi_code);
#line 1563
    uint constargs = 1;
#line 1563

#line 1563
    /* Initialize all the members */
#line 1563
  #define whati (&(what->i_FI_FORMAT))
#line 1563
  #line 1559 "filter/f-inst.c"
whati->f1 = f1;
#line 1559
const struct f_inst *child1 = f1;
#line 1559
do {
#line 1559
  what->size += child1->size;
#line 1559

#line 1559
  if (child1->fi_code != FI_CONSTANT)
#line 1559
    constargs = 0;
#line 1559

#line 1559
} while (child1 = child1->next);
#line 1560 "filter/f-inst.c"
what->type = T_STRING;

#line 1563 "filter/f-inst.c"
    /* If not constant, return the instruction itself */
#line 1563
    if (!constargs)
#line 1563
      return what;
#line 1563

#line 1563
    /* Try to pre-calculate the result */
#line 1563
    #line 1558 "filter/f-inst.c"
 {	/* Format */
    
#line 1559 "filter/f-inst.c"
;
     
#line 1560 "filter/f-inst.c"
 return fi_constant(what,  (struct f_val) { .type = T_STRING, .val.s = val_format_str(fpool, &v1) } );
  }

  
  #undef whati
#line 1563 "filter/f-inst.c"
  }
#line 1563

#line 1563

#line 696 "filter/decl.m4"
 /* Constructor for FI_ASSERT */
#line 696
struct f_inst *
#line 1565 "filter/f-inst.c"
NONNULL(2)
#line 696 "filter/decl.m4"
f_new_inst_FI_ASSERT(enum f_instruction_code fi_code
#line 1565 "filter/f-inst.c"
  , struct f_inst * f1
#line 1567 "filter/f-inst.c"
  , char * s
#line 696 "filter/decl.m4"
)
#line 696
  {
#line 696
    /* Allocate the structure */
#line 696
    struct f_inst *what = fi_new(fi_code);
#line 696
    
#line 696

#line 696
    /* Initialize all the members */
#line 696
  #define whati (&(what->i_FI_ASSERT))
#line 696
  #line 1565 "filter/f-inst.c"
whati->f1 = f1;
#line 1565
const struct f_inst *child1 = f1;
#line 1565
do {
#line 1565
  what->size += child1->size;
#line 1565

#line 1565
} while (child1 = child1->next);
#line 1565 "filter/f-inst.c"
if (f1->type && (f1->type != (T_BOOL)) && !f_const_promotion(f1, (T_BOOL)))
#line 1565
  cf_error("Argument 1 of %s must be of type %s, got type %s",
#line 1565
	   f_instruction_name(what->fi_code), f_type_name(T_BOOL), f_type_name(f1->type));
#line 1567 "filter/f-inst.c"
whati->s = s;

#line 696 "filter/decl.m4"
    /* If not constant, return the instruction itself */
#line 696
    
#line 696
      return what;
#line 696

#line 696
    /* Try to pre-calculate the result */
#line 696
      #undef whati
#line 696
  }
#line 696

#line 696

#line 534 "filter/decl.m4"

#undef v1
#undef v2
#undef v3
#undef vv

/* Line dumpers */
#define INDENT (((const char *) f_dump_line_indent_str) + sizeof(f_dump_line_indent_str) - (indent) - 1)
static const char f_dump_line_indent_str[] = "                                ";

#line 284 "filter/f-inst.c"
 /* Dump line for FI_ADD */
#line 284
static inline void f_dump_line_item_FI_ADD(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 284

#line 284
{
#line 284
#define item (&(item_->i_FI_ADD))
#line 284
#undef item
#line 284
}
#line 284

#line 289 "filter/f-inst.c"
 /* Dump line for FI_SUBTRACT */
#line 289
static inline void f_dump_line_item_FI_SUBTRACT(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 289

#line 289
{
#line 289
#define item (&(item_->i_FI_SUBTRACT))
#line 289
#undef item
#line 289
}
#line 289

#line 294 "filter/f-inst.c"
 /* Dump line for FI_MULTIPLY */
#line 294
static inline void f_dump_line_item_FI_MULTIPLY(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 294

#line 294
{
#line 294
#define item (&(item_->i_FI_MULTIPLY))
#line 294
#undef item
#line 294
}
#line 294

#line 300 "filter/f-inst.c"
 /* Dump line for FI_DIVIDE */
#line 300
static inline void f_dump_line_item_FI_DIVIDE(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 300

#line 300
{
#line 300
#define item (&(item_->i_FI_DIVIDE))
#line 300
#undef item
#line 300
}
#line 300

#line 310 "filter/f-inst.c"
 /* Dump line for FI_AND */
#line 310
static inline void f_dump_line_item_FI_AND(const struct f_line_item *item_, const int indent)
#line 310

#line 310
{
#line 310
#define item (&(item_->i_FI_AND))
#line 306 "filter/f-inst.c"
f_dump_line(item->fl2, indent + 1);
#line 310 "filter/f-inst.c"
#undef item
#line 310
}
#line 310

#line 321 "filter/f-inst.c"
 /* Dump line for FI_OR */
#line 321
static inline void f_dump_line_item_FI_OR(const struct f_line_item *item_, const int indent)
#line 321

#line 321
{
#line 321
#define item (&(item_->i_FI_OR))
#line 316 "filter/f-inst.c"
f_dump_line(item->fl2, indent + 1);
#line 321 "filter/f-inst.c"
#undef item
#line 321
}
#line 321

#line 331 "filter/f-inst.c"
 /* Dump line for FI_PAIR_CONSTRUCT */
#line 331
static inline void f_dump_line_item_FI_PAIR_CONSTRUCT(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 331

#line 331
{
#line 331
#define item (&(item_->i_FI_PAIR_CONSTRUCT))
#line 331
#undef item
#line 331
}
#line 331

#line 371 "filter/f-inst.c"
 /* Dump line for FI_EC_CONSTRUCT */
#line 371
static inline void f_dump_line_item_FI_EC_CONSTRUCT(const struct f_line_item *item_, const int indent)
#line 371

#line 371
{
#line 371
#define item (&(item_->i_FI_EC_CONSTRUCT))
#line 335 "filter/f-inst.c"
debug("%s" "ec subtype %s" "\n", INDENT, ec_subtype_str(item->ecs));
#line 335

#line 371 "filter/f-inst.c"
#undef item
#line 371
}
#line 371

#line 378 "filter/f-inst.c"
 /* Dump line for FI_LC_CONSTRUCT */
#line 378
static inline void f_dump_line_item_FI_LC_CONSTRUCT(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 378

#line 378
{
#line 378
#define item (&(item_->i_FI_LC_CONSTRUCT))
#line 378
#undef item
#line 378
}
#line 378

#line 428 "filter/f-inst.c"
 /* Dump line for FI_PATHMASK_CONSTRUCT */
#line 428
static inline void f_dump_line_item_FI_PATHMASK_CONSTRUCT(const struct f_line_item *item_, const int indent)
#line 428

#line 428
{
#line 428
#define item (&(item_->i_FI_PATHMASK_CONSTRUCT))
#line 379 "filter/f-inst.c"
  debug("%snumber of varargs %u\n", INDENT, item->varcount);
#line 428 "filter/f-inst.c"
#undef item
#line 428
}
#line 428

#line 435 "filter/f-inst.c"
 /* Dump line for FI_NEQ */
#line 435
static inline void f_dump_line_item_FI_NEQ(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 435

#line 435
{
#line 435
#define item (&(item_->i_FI_NEQ))
#line 435
#undef item
#line 435
}
#line 435

#line 442 "filter/f-inst.c"
 /* Dump line for FI_EQ */
#line 442
static inline void f_dump_line_item_FI_EQ(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 442

#line 442
{
#line 442
#define item (&(item_->i_FI_EQ))
#line 442
#undef item
#line 442
}
#line 442

#line 453 "filter/f-inst.c"
 /* Dump line for FI_LT */
#line 453
static inline void f_dump_line_item_FI_LT(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 453

#line 453
{
#line 453
#define item (&(item_->i_FI_LT))
#line 453
#undef item
#line 453
}
#line 453

#line 464 "filter/f-inst.c"
 /* Dump line for FI_LTE */
#line 464
static inline void f_dump_line_item_FI_LTE(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 464

#line 464
{
#line 464
#define item (&(item_->i_FI_LTE))
#line 464
#undef item
#line 464
}
#line 464

#line 469 "filter/f-inst.c"
 /* Dump line for FI_NOT */
#line 469
static inline void f_dump_line_item_FI_NOT(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 469

#line 469
{
#line 469
#define item (&(item_->i_FI_NOT))
#line 469
#undef item
#line 469
}
#line 469

#line 478 "filter/f-inst.c"
 /* Dump line for FI_MATCH */
#line 478
static inline void f_dump_line_item_FI_MATCH(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 478

#line 478
{
#line 478
#define item (&(item_->i_FI_MATCH))
#line 478
#undef item
#line 478
}
#line 478

#line 487 "filter/f-inst.c"
 /* Dump line for FI_NOT_MATCH */
#line 487
static inline void f_dump_line_item_FI_NOT_MATCH(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 487

#line 487
{
#line 487
#define item (&(item_->i_FI_NOT_MATCH))
#line 487
#undef item
#line 487
}
#line 487

#line 492 "filter/f-inst.c"
 /* Dump line for FI_DEFINED */
#line 492
static inline void f_dump_line_item_FI_DEFINED(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 492

#line 492
{
#line 492
#define item (&(item_->i_FI_DEFINED))
#line 492
#undef item
#line 492
}
#line 492

#line 504 "filter/f-inst.c"
 /* Dump line for FI_TYPE */
#line 504
static inline void f_dump_line_item_FI_TYPE(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 504

#line 504
{
#line 504
#define item (&(item_->i_FI_TYPE))
#line 504
#undef item
#line 504
}
#line 504

#line 509 "filter/f-inst.c"
 /* Dump line for FI_IS_V4 */
#line 509
static inline void f_dump_line_item_FI_IS_V4(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 509

#line 509
{
#line 509
#define item (&(item_->i_FI_IS_V4))
#line 509
#undef item
#line 509
}
#line 509

#line 522 "filter/f-inst.c"
 /* Dump line for FI_VAR_INIT */
#line 522
static inline void f_dump_line_item_FI_VAR_INIT(const struct f_line_item *item_, const int indent)
#line 522

#line 522
{
#line 522
#define item (&(item_->i_FI_VAR_INIT))
#line 512 "filter/f-inst.c"
debug("%s" "symbol %s" "\n", INDENT, item->sym->name);
#line 512

#line 522 "filter/f-inst.c"
#undef item
#line 522
}
#line 522

#line 531 "filter/f-inst.c"
 /* Dump line for FI_VAR_SET */
#line 531
static inline void f_dump_line_item_FI_VAR_SET(const struct f_line_item *item_, const int indent)
#line 531

#line 531
{
#line 531
#define item (&(item_->i_FI_VAR_SET))
#line 525 "filter/f-inst.c"
debug("%s" "symbol %s" "\n", INDENT, item->sym->name);
#line 525

#line 531 "filter/f-inst.c"
#undef item
#line 531
}
#line 531

#line 538 "filter/f-inst.c"
 /* Dump line for FI_VAR_GET */
#line 538
static inline void f_dump_line_item_FI_VAR_GET(const struct f_line_item *item_, const int indent)
#line 538

#line 538
{
#line 538
#define item (&(item_->i_FI_VAR_GET))
#line 532 "filter/f-inst.c"
debug("%s" "symbol %s" "\n", INDENT, item->sym->name);
#line 532

#line 538 "filter/f-inst.c"
#undef item
#line 538
}
#line 538

#line 551 "filter/f-inst.c"
 /* Dump line for FI_CONSTANT */
#line 551
static inline void f_dump_line_item_FI_CONSTANT(const struct f_line_item *item_, const int indent)
#line 551

#line 551
{
#line 551
#define item (&(item_->i_FI_CONSTANT))
#line 539 "filter/f-inst.c"
debug("%s" "value %s" "\n", INDENT, val_dump(&(item->val))
#line 539
    );
#line 539

#line 551 "filter/f-inst.c"
#undef item
#line 551
}
#line 551

#line 585 "filter/f-inst.c"
 /* Dump line for FI_FOR_INIT */
#line 585
static inline void f_dump_line_item_FI_FOR_INIT(const struct f_line_item *item_, const int indent)
#line 585

#line 585
{
#line 585
#define item (&(item_->i_FI_FOR_INIT))
#line 554 "filter/f-inst.c"
debug("%s" "symbol %s" "\n", INDENT, item->sym->name);
#line 554

#line 585 "filter/f-inst.c"
#undef item
#line 585
}
#line 585

#line 639 "filter/f-inst.c"
 /* Dump line for FI_FOR_NEXT */
#line 639
static inline void f_dump_line_item_FI_FOR_NEXT(const struct f_line_item *item_, const int indent)
#line 639

#line 639
{
#line 639
#define item (&(item_->i_FI_FOR_NEXT))
#line 587 "filter/f-inst.c"
debug("%s" "symbol %s" "\n", INDENT, item->sym->name);
#line 587

#line 630 "filter/f-inst.c"
f_dump_line(item->fl1, indent + 1);
#line 639 "filter/f-inst.c"
#undef item
#line 639
}
#line 639

#line 647 "filter/f-inst.c"
 /* Dump line for FI_CONDITION */
#line 647
static inline void f_dump_line_item_FI_CONDITION(const struct f_line_item *item_, const int indent)
#line 647

#line 647
{
#line 647
#define item (&(item_->i_FI_CONDITION))
#line 642 "filter/f-inst.c"
f_dump_line(item->fl2, indent + 1);
#line 644 "filter/f-inst.c"
f_dump_line(item->fl3, indent + 1);
#line 647 "filter/f-inst.c"
#undef item
#line 647
}
#line 647

#line 656 "filter/f-inst.c"
 /* Dump line for FI_PRINT */
#line 656
static inline void f_dump_line_item_FI_PRINT(const struct f_line_item *item_, const int indent)
#line 656

#line 656
{
#line 656
#define item (&(item_->i_FI_PRINT))
#line 649 "filter/f-inst.c"
  debug("%snumber of varargs %u\n", INDENT, item->varcount);
#line 656 "filter/f-inst.c"
#undef item
#line 656
}
#line 656

#line 663 "filter/f-inst.c"
 /* Dump line for FI_FLUSH */
#line 663
static inline void f_dump_line_item_FI_FLUSH(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 663

#line 663
{
#line 663
#define item (&(item_->i_FI_FLUSH))
#line 663
#undef item
#line 663
}
#line 663

#line 677 "filter/f-inst.c"
 /* Dump line for FI_DIE */
#line 677
static inline void f_dump_line_item_FI_DIE(const struct f_line_item *item_, const int indent)
#line 677

#line 677
{
#line 677
#define item (&(item_->i_FI_DIE))
#line 665 "filter/f-inst.c"
debug("%s" "%s" "\n", INDENT, filter_return_str(item->fret));
#line 665

#line 677 "filter/f-inst.c"
#undef item
#line 677
}
#line 677

#line 705 "filter/f-inst.c"
 /* Dump line for FI_RTA_GET */
#line 705
static inline void f_dump_line_item_FI_RTA_GET(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 705

#line 705
{
#line 705
#define item (&(item_->i_FI_RTA_GET))
#line 705
#undef item
#line 705
}
#line 705

#line 820 "filter/f-inst.c"
 /* Dump line for FI_RTA_SET */
#line 820
static inline void f_dump_line_item_FI_RTA_SET(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 820

#line 820
{
#line 820
#define item (&(item_->i_FI_RTA_SET))
#line 820
#undef item
#line 820
}
#line 820

#line 892 "filter/f-inst.c"
 /* Dump line for FI_EA_GET */
#line 892
static inline void f_dump_line_item_FI_EA_GET(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 892

#line 892
{
#line 892
#define item (&(item_->i_FI_EA_GET))
#line 892
#undef item
#line 892
}
#line 892

#line 959 "filter/f-inst.c"
 /* Dump line for FI_EA_SET */
#line 959
static inline void f_dump_line_item_FI_EA_SET(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 959

#line 959
{
#line 959
#define item (&(item_->i_FI_EA_SET))
#line 959
#undef item
#line 959
}
#line 959

#line 968 "filter/f-inst.c"
 /* Dump line for FI_EA_UNSET */
#line 968
static inline void f_dump_line_item_FI_EA_UNSET(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 968

#line 968
{
#line 968
#define item (&(item_->i_FI_EA_UNSET))
#line 968
#undef item
#line 968
}
#line 968

#line 980 "filter/f-inst.c"
 /* Dump line for FI_LENGTH */
#line 980
static inline void f_dump_line_item_FI_LENGTH(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 980

#line 980
{
#line 980
#define item (&(item_->i_FI_LENGTH))
#line 980
#undef item
#line 980
}
#line 980

#line 1015 "filter/f-inst.c"
 /* Dump line for FI_NET_SRC */
#line 1015
static inline void f_dump_line_item_FI_NET_SRC(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1015

#line 1015
{
#line 1015
#define item (&(item_->i_FI_NET_SRC))
#line 1015
#undef item
#line 1015
}
#line 1015

#line 1050 "filter/f-inst.c"
 /* Dump line for FI_NET_DST */
#line 1050
static inline void f_dump_line_item_FI_NET_DST(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1050

#line 1050
{
#line 1050
#define item (&(item_->i_FI_NET_DST))
#line 1050
#undef item
#line 1050
}
#line 1050

#line 1060 "filter/f-inst.c"
 /* Dump line for FI_ROA_MAXLEN */
#line 1060
static inline void f_dump_line_item_FI_ROA_MAXLEN(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1060

#line 1060
{
#line 1060
#define item (&(item_->i_FI_ROA_MAXLEN))
#line 1060
#undef item
#line 1060
}
#line 1060

#line 1087 "filter/f-inst.c"
 /* Dump line for FI_ASN */
#line 1087
static inline void f_dump_line_item_FI_ASN(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1087

#line 1087
{
#line 1087
#define item (&(item_->i_FI_ASN))
#line 1087
#undef item
#line 1087
}
#line 1087

#line 1092 "filter/f-inst.c"
 /* Dump line for FI_IP */
#line 1092
static inline void f_dump_line_item_FI_IP(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1092

#line 1092
{
#line 1092
#define item (&(item_->i_FI_IP))
#line 1092
#undef item
#line 1092
}
#line 1092

#line 1099 "filter/f-inst.c"
 /* Dump line for FI_ROUTE_DISTINGUISHER */
#line 1099
static inline void f_dump_line_item_FI_ROUTE_DISTINGUISHER(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1099

#line 1099
{
#line 1099
#define item (&(item_->i_FI_ROUTE_DISTINGUISHER))
#line 1099
#undef item
#line 1099
}
#line 1099

#line 1106 "filter/f-inst.c"
 /* Dump line for FI_AS_PATH_FIRST */
#line 1106
static inline void f_dump_line_item_FI_AS_PATH_FIRST(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1106

#line 1106
{
#line 1106
#define item (&(item_->i_FI_AS_PATH_FIRST))
#line 1106
#undef item
#line 1106
}
#line 1106

#line 1113 "filter/f-inst.c"
 /* Dump line for FI_AS_PATH_LAST */
#line 1113
static inline void f_dump_line_item_FI_AS_PATH_LAST(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1113

#line 1113
{
#line 1113
#define item (&(item_->i_FI_AS_PATH_LAST))
#line 1113
#undef item
#line 1113
}
#line 1113

#line 1118 "filter/f-inst.c"
 /* Dump line for FI_AS_PATH_LAST_NAG */
#line 1118
static inline void f_dump_line_item_FI_AS_PATH_LAST_NAG(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1118

#line 1118
{
#line 1118
#define item (&(item_->i_FI_AS_PATH_LAST_NAG))
#line 1118
#undef item
#line 1118
}
#line 1118

#line 1123 "filter/f-inst.c"
 /* Dump line for FI_PAIR_DATA */
#line 1123
static inline void f_dump_line_item_FI_PAIR_DATA(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1123

#line 1123
{
#line 1123
#define item (&(item_->i_FI_PAIR_DATA))
#line 1123
#undef item
#line 1123
}
#line 1123

#line 1128 "filter/f-inst.c"
 /* Dump line for FI_LC_DATA1 */
#line 1128
static inline void f_dump_line_item_FI_LC_DATA1(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1128

#line 1128
{
#line 1128
#define item (&(item_->i_FI_LC_DATA1))
#line 1128
#undef item
#line 1128
}
#line 1128

#line 1133 "filter/f-inst.c"
 /* Dump line for FI_LC_DATA2 */
#line 1133
static inline void f_dump_line_item_FI_LC_DATA2(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1133

#line 1133
{
#line 1133
#define item (&(item_->i_FI_LC_DATA2))
#line 1133
#undef item
#line 1133
}
#line 1133

#line 1167 "filter/f-inst.c"
 /* Dump line for FI_MIN */
#line 1167
static inline void f_dump_line_item_FI_MIN(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1167

#line 1167
{
#line 1167
#define item (&(item_->i_FI_MIN))
#line 1167
#undef item
#line 1167
}
#line 1167

#line 1201 "filter/f-inst.c"
 /* Dump line for FI_MAX */
#line 1201
static inline void f_dump_line_item_FI_MAX(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1201

#line 1201
{
#line 1201
#define item (&(item_->i_FI_MAX))
#line 1201
#undef item
#line 1201
}
#line 1201

#line 1227 "filter/f-inst.c"
 /* Dump line for FI_RETURN */
#line 1227
static inline void f_dump_line_item_FI_RETURN(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1227

#line 1227
{
#line 1227
#define item (&(item_->i_FI_RETURN))
#line 1227
#undef item
#line 1227
}
#line 1227

#line 1288 "filter/f-inst.c"
 /* Dump line for FI_CALL */
#line 1288
static inline void f_dump_line_item_FI_CALL(const struct f_line_item *item_, const int indent)
#line 1288

#line 1288
{
#line 1288
#define item (&(item_->i_FI_CALL))
#line 1229 "filter/f-inst.c"
  debug("%snumber of varargs %u\n", INDENT, item->varcount);
#line 1230 "filter/f-inst.c"
debug("%s" "symbol %s" "\n", INDENT, item->sym->name);
#line 1230

#line 1288 "filter/f-inst.c"
#undef item
#line 1288
}
#line 1288

#line 1293 "filter/f-inst.c"
 /* Dump line for FI_DROP_RESULT */
#line 1293
static inline void f_dump_line_item_FI_DROP_RESULT(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1293

#line 1293
{
#line 1293
#define item (&(item_->i_FI_DROP_RESULT))
#line 1293
#undef item
#line 1293
}
#line 1293

#line 1334 "filter/f-inst.c"
 /* Dump line for FI_SWITCH */
#line 1334
static inline void f_dump_line_item_FI_SWITCH(const struct f_line_item *item_, const int indent)
#line 1334

#line 1334
{
#line 1334
#define item (&(item_->i_FI_SWITCH))
#line 1296 "filter/f-inst.c"
debug("%s" "tree %p" "\n", INDENT, item->tree);
#line 1296

#line 1334 "filter/f-inst.c"
#undef item
#line 1334
}
#line 1334

#line 1342 "filter/f-inst.c"
 /* Dump line for FI_IP_MASK */
#line 1342
static inline void f_dump_line_item_FI_IP_MASK(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1342

#line 1342
{
#line 1342
#define item (&(item_->i_FI_IP_MASK))
#line 1342
#undef item
#line 1342
}
#line 1342

#line 1348 "filter/f-inst.c"
 /* Dump line for FI_PATH_PREPEND */
#line 1348
static inline void f_dump_line_item_FI_PATH_PREPEND(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1348

#line 1348
{
#line 1348
#define item (&(item_->i_FI_PATH_PREPEND))
#line 1348
#undef item
#line 1348
}
#line 1348

#line 1405 "filter/f-inst.c"
 /* Dump line for FI_CLIST_ADD */
#line 1405
static inline void f_dump_line_item_FI_CLIST_ADD(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1405

#line 1405
{
#line 1405
#define item (&(item_->i_FI_CLIST_ADD))
#line 1405
#undef item
#line 1405
}
#line 1405

#line 1460 "filter/f-inst.c"
 /* Dump line for FI_CLIST_DEL */
#line 1460
static inline void f_dump_line_item_FI_CLIST_DEL(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1460

#line 1460
{
#line 1460
#define item (&(item_->i_FI_CLIST_DEL))
#line 1460
#undef item
#line 1460
}
#line 1460

#line 1506 "filter/f-inst.c"
 /* Dump line for FI_CLIST_FILTER */
#line 1506
static inline void f_dump_line_item_FI_CLIST_FILTER(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1506

#line 1506
{
#line 1506
#define item (&(item_->i_FI_CLIST_FILTER))
#line 1506
#undef item
#line 1506
}
#line 1506

#line 1536 "filter/f-inst.c"
 /* Dump line for FI_ROA_CHECK_IMPLICIT */
#line 1536
static inline void f_dump_line_item_FI_ROA_CHECK_IMPLICIT(const struct f_line_item *item_, const int indent)
#line 1536

#line 1536
{
#line 1536
#define item (&(item_->i_FI_ROA_CHECK_IMPLICIT))
#line 1508 "filter/f-inst.c"
debug("%s" "route table %s" "\n", INDENT, item->rtc->name);
#line 1508

#line 1536 "filter/f-inst.c"
#undef item
#line 1536
}
#line 1536

#line 1558 "filter/f-inst.c"
 /* Dump line for FI_ROA_CHECK_EXPLICIT */
#line 1558
static inline void f_dump_line_item_FI_ROA_CHECK_EXPLICIT(const struct f_line_item *item_, const int indent)
#line 1558

#line 1558
{
#line 1558
#define item (&(item_->i_FI_ROA_CHECK_EXPLICIT))
#line 1540 "filter/f-inst.c"
debug("%s" "route table %s" "\n", INDENT, item->rtc->name);
#line 1540

#line 1558 "filter/f-inst.c"
#undef item
#line 1558
}
#line 1558

#line 1563 "filter/f-inst.c"
 /* Dump line for FI_FORMAT */
#line 1563
static inline void f_dump_line_item_FI_FORMAT(const struct f_line_item *item UNUSED, const int indent UNUSED)
#line 1563

#line 1563
{
#line 1563
#define item (&(item_->i_FI_FORMAT))
#line 1563
#undef item
#line 1563
}
#line 1563

#line 696 "filter/decl.m4"
 /* Dump line for FI_ASSERT */
#line 696
static inline void f_dump_line_item_FI_ASSERT(const struct f_line_item *item_, const int indent)
#line 696

#line 696
{
#line 696
#define item (&(item_->i_FI_ASSERT))
#line 1567 "filter/f-inst.c"
debug("%s" "string %s" "\n", INDENT, item->s);
#line 1567

#line 696 "filter/decl.m4"
#undef item
#line 696
}
#line 696

#line 544 "filter/decl.m4"


void f_dump_line(const struct f_line *dest, uint indent)
{
  if (!dest) {
    debug("%sNo filter line (NULL)\n", INDENT);
    return;
  }
  debug("%sFilter line %p (len=%u)\n", INDENT, dest, dest->len);
  for (uint i=0; i<dest->len; i++) {
    const struct f_line_item *item = &dest->items[i];
    debug("%sInstruction %s at line %u\n", INDENT, f_instruction_name_(item->fi_code), item->lineno);
    switch (item->fi_code) {
#line 284 "filter/f-inst.c"
 /* Dump line caller for FI_ADD */
#line 284
case FI_ADD: f_dump_line_item_FI_ADD(item, indent + 1); break;
#line 284

#line 289 "filter/f-inst.c"
 /* Dump line caller for FI_SUBTRACT */
#line 289
case FI_SUBTRACT: f_dump_line_item_FI_SUBTRACT(item, indent + 1); break;
#line 289

#line 294 "filter/f-inst.c"
 /* Dump line caller for FI_MULTIPLY */
#line 294
case FI_MULTIPLY: f_dump_line_item_FI_MULTIPLY(item, indent + 1); break;
#line 294

#line 300 "filter/f-inst.c"
 /* Dump line caller for FI_DIVIDE */
#line 300
case FI_DIVIDE: f_dump_line_item_FI_DIVIDE(item, indent + 1); break;
#line 300

#line 310 "filter/f-inst.c"
 /* Dump line caller for FI_AND */
#line 310
case FI_AND: f_dump_line_item_FI_AND(item, indent + 1); break;
#line 310

#line 321 "filter/f-inst.c"
 /* Dump line caller for FI_OR */
#line 321
case FI_OR: f_dump_line_item_FI_OR(item, indent + 1); break;
#line 321

#line 331 "filter/f-inst.c"
 /* Dump line caller for FI_PAIR_CONSTRUCT */
#line 331
case FI_PAIR_CONSTRUCT: f_dump_line_item_FI_PAIR_CONSTRUCT(item, indent + 1); break;
#line 331

#line 371 "filter/f-inst.c"
 /* Dump line caller for FI_EC_CONSTRUCT */
#line 371
case FI_EC_CONSTRUCT: f_dump_line_item_FI_EC_CONSTRUCT(item, indent + 1); break;
#line 371

#line 378 "filter/f-inst.c"
 /* Dump line caller for FI_LC_CONSTRUCT */
#line 378
case FI_LC_CONSTRUCT: f_dump_line_item_FI_LC_CONSTRUCT(item, indent + 1); break;
#line 378

#line 428 "filter/f-inst.c"
 /* Dump line caller for FI_PATHMASK_CONSTRUCT */
#line 428
case FI_PATHMASK_CONSTRUCT: f_dump_line_item_FI_PATHMASK_CONSTRUCT(item, indent + 1); break;
#line 428

#line 435 "filter/f-inst.c"
 /* Dump line caller for FI_NEQ */
#line 435
case FI_NEQ: f_dump_line_item_FI_NEQ(item, indent + 1); break;
#line 435

#line 442 "filter/f-inst.c"
 /* Dump line caller for FI_EQ */
#line 442
case FI_EQ: f_dump_line_item_FI_EQ(item, indent + 1); break;
#line 442

#line 453 "filter/f-inst.c"
 /* Dump line caller for FI_LT */
#line 453
case FI_LT: f_dump_line_item_FI_LT(item, indent + 1); break;
#line 453

#line 464 "filter/f-inst.c"
 /* Dump line caller for FI_LTE */
#line 464
case FI_LTE: f_dump_line_item_FI_LTE(item, indent + 1); break;
#line 464

#line 469 "filter/f-inst.c"
 /* Dump line caller for FI_NOT */
#line 469
case FI_NOT: f_dump_line_item_FI_NOT(item, indent + 1); break;
#line 469

#line 478 "filter/f-inst.c"
 /* Dump line caller for FI_MATCH */
#line 478
case FI_MATCH: f_dump_line_item_FI_MATCH(item, indent + 1); break;
#line 478

#line 487 "filter/f-inst.c"
 /* Dump line caller for FI_NOT_MATCH */
#line 487
case FI_NOT_MATCH: f_dump_line_item_FI_NOT_MATCH(item, indent + 1); break;
#line 487

#line 492 "filter/f-inst.c"
 /* Dump line caller for FI_DEFINED */
#line 492
case FI_DEFINED: f_dump_line_item_FI_DEFINED(item, indent + 1); break;
#line 492

#line 504 "filter/f-inst.c"
 /* Dump line caller for FI_TYPE */
#line 504
case FI_TYPE: f_dump_line_item_FI_TYPE(item, indent + 1); break;
#line 504

#line 509 "filter/f-inst.c"
 /* Dump line caller for FI_IS_V4 */
#line 509
case FI_IS_V4: f_dump_line_item_FI_IS_V4(item, indent + 1); break;
#line 509

#line 522 "filter/f-inst.c"
 /* Dump line caller for FI_VAR_INIT */
#line 522
case FI_VAR_INIT: f_dump_line_item_FI_VAR_INIT(item, indent + 1); break;
#line 522

#line 531 "filter/f-inst.c"
 /* Dump line caller for FI_VAR_SET */
#line 531
case FI_VAR_SET: f_dump_line_item_FI_VAR_SET(item, indent + 1); break;
#line 531

#line 538 "filter/f-inst.c"
 /* Dump line caller for FI_VAR_GET */
#line 538
case FI_VAR_GET: f_dump_line_item_FI_VAR_GET(item, indent + 1); break;
#line 538

#line 551 "filter/f-inst.c"
 /* Dump line caller for FI_CONSTANT */
#line 551
case FI_CONSTANT: f_dump_line_item_FI_CONSTANT(item, indent + 1); break;
#line 551

#line 585 "filter/f-inst.c"
 /* Dump line caller for FI_FOR_INIT */
#line 585
case FI_FOR_INIT: f_dump_line_item_FI_FOR_INIT(item, indent + 1); break;
#line 585

#line 639 "filter/f-inst.c"
 /* Dump line caller for FI_FOR_NEXT */
#line 639
case FI_FOR_NEXT: f_dump_line_item_FI_FOR_NEXT(item, indent + 1); break;
#line 639

#line 647 "filter/f-inst.c"
 /* Dump line caller for FI_CONDITION */
#line 647
case FI_CONDITION: f_dump_line_item_FI_CONDITION(item, indent + 1); break;
#line 647

#line 656 "filter/f-inst.c"
 /* Dump line caller for FI_PRINT */
#line 656
case FI_PRINT: f_dump_line_item_FI_PRINT(item, indent + 1); break;
#line 656

#line 663 "filter/f-inst.c"
 /* Dump line caller for FI_FLUSH */
#line 663
case FI_FLUSH: f_dump_line_item_FI_FLUSH(item, indent + 1); break;
#line 663

#line 677 "filter/f-inst.c"
 /* Dump line caller for FI_DIE */
#line 677
case FI_DIE: f_dump_line_item_FI_DIE(item, indent + 1); break;
#line 677

#line 705 "filter/f-inst.c"
 /* Dump line caller for FI_RTA_GET */
#line 705
case FI_RTA_GET: f_dump_line_item_FI_RTA_GET(item, indent + 1); break;
#line 705

#line 820 "filter/f-inst.c"
 /* Dump line caller for FI_RTA_SET */
#line 820
case FI_RTA_SET: f_dump_line_item_FI_RTA_SET(item, indent + 1); break;
#line 820

#line 892 "filter/f-inst.c"
 /* Dump line caller for FI_EA_GET */
#line 892
case FI_EA_GET: f_dump_line_item_FI_EA_GET(item, indent + 1); break;
#line 892

#line 959 "filter/f-inst.c"
 /* Dump line caller for FI_EA_SET */
#line 959
case FI_EA_SET: f_dump_line_item_FI_EA_SET(item, indent + 1); break;
#line 959

#line 968 "filter/f-inst.c"
 /* Dump line caller for FI_EA_UNSET */
#line 968
case FI_EA_UNSET: f_dump_line_item_FI_EA_UNSET(item, indent + 1); break;
#line 968

#line 980 "filter/f-inst.c"
 /* Dump line caller for FI_LENGTH */
#line 980
case FI_LENGTH: f_dump_line_item_FI_LENGTH(item, indent + 1); break;
#line 980

#line 1015 "filter/f-inst.c"
 /* Dump line caller for FI_NET_SRC */
#line 1015
case FI_NET_SRC: f_dump_line_item_FI_NET_SRC(item, indent + 1); break;
#line 1015

#line 1050 "filter/f-inst.c"
 /* Dump line caller for FI_NET_DST */
#line 1050
case FI_NET_DST: f_dump_line_item_FI_NET_DST(item, indent + 1); break;
#line 1050

#line 1060 "filter/f-inst.c"
 /* Dump line caller for FI_ROA_MAXLEN */
#line 1060
case FI_ROA_MAXLEN: f_dump_line_item_FI_ROA_MAXLEN(item, indent + 1); break;
#line 1060

#line 1087 "filter/f-inst.c"
 /* Dump line caller for FI_ASN */
#line 1087
case FI_ASN: f_dump_line_item_FI_ASN(item, indent + 1); break;
#line 1087

#line 1092 "filter/f-inst.c"
 /* Dump line caller for FI_IP */
#line 1092
case FI_IP: f_dump_line_item_FI_IP(item, indent + 1); break;
#line 1092

#line 1099 "filter/f-inst.c"
 /* Dump line caller for FI_ROUTE_DISTINGUISHER */
#line 1099
case FI_ROUTE_DISTINGUISHER: f_dump_line_item_FI_ROUTE_DISTINGUISHER(item, indent + 1); break;
#line 1099

#line 1106 "filter/f-inst.c"
 /* Dump line caller for FI_AS_PATH_FIRST */
#line 1106
case FI_AS_PATH_FIRST: f_dump_line_item_FI_AS_PATH_FIRST(item, indent + 1); break;
#line 1106

#line 1113 "filter/f-inst.c"
 /* Dump line caller for FI_AS_PATH_LAST */
#line 1113
case FI_AS_PATH_LAST: f_dump_line_item_FI_AS_PATH_LAST(item, indent + 1); break;
#line 1113

#line 1118 "filter/f-inst.c"
 /* Dump line caller for FI_AS_PATH_LAST_NAG */
#line 1118
case FI_AS_PATH_LAST_NAG: f_dump_line_item_FI_AS_PATH_LAST_NAG(item, indent + 1); break;
#line 1118

#line 1123 "filter/f-inst.c"
 /* Dump line caller for FI_PAIR_DATA */
#line 1123
case FI_PAIR_DATA: f_dump_line_item_FI_PAIR_DATA(item, indent + 1); break;
#line 1123

#line 1128 "filter/f-inst.c"
 /* Dump line caller for FI_LC_DATA1 */
#line 1128
case FI_LC_DATA1: f_dump_line_item_FI_LC_DATA1(item, indent + 1); break;
#line 1128

#line 1133 "filter/f-inst.c"
 /* Dump line caller for FI_LC_DATA2 */
#line 1133
case FI_LC_DATA2: f_dump_line_item_FI_LC_DATA2(item, indent + 1); break;
#line 1133

#line 1167 "filter/f-inst.c"
 /* Dump line caller for FI_MIN */
#line 1167
case FI_MIN: f_dump_line_item_FI_MIN(item, indent + 1); break;
#line 1167

#line 1201 "filter/f-inst.c"
 /* Dump line caller for FI_MAX */
#line 1201
case FI_MAX: f_dump_line_item_FI_MAX(item, indent + 1); break;
#line 1201

#line 1227 "filter/f-inst.c"
 /* Dump line caller for FI_RETURN */
#line 1227
case FI_RETURN: f_dump_line_item_FI_RETURN(item, indent + 1); break;
#line 1227

#line 1288 "filter/f-inst.c"
 /* Dump line caller for FI_CALL */
#line 1288
case FI_CALL: f_dump_line_item_FI_CALL(item, indent + 1); break;
#line 1288

#line 1293 "filter/f-inst.c"
 /* Dump line caller for FI_DROP_RESULT */
#line 1293
case FI_DROP_RESULT: f_dump_line_item_FI_DROP_RESULT(item, indent + 1); break;
#line 1293

#line 1334 "filter/f-inst.c"
 /* Dump line caller for FI_SWITCH */
#line 1334
case FI_SWITCH: f_dump_line_item_FI_SWITCH(item, indent + 1); break;
#line 1334

#line 1342 "filter/f-inst.c"
 /* Dump line caller for FI_IP_MASK */
#line 1342
case FI_IP_MASK: f_dump_line_item_FI_IP_MASK(item, indent + 1); break;
#line 1342

#line 1348 "filter/f-inst.c"
 /* Dump line caller for FI_PATH_PREPEND */
#line 1348
case FI_PATH_PREPEND: f_dump_line_item_FI_PATH_PREPEND(item, indent + 1); break;
#line 1348

#line 1405 "filter/f-inst.c"
 /* Dump line caller for FI_CLIST_ADD */
#line 1405
case FI_CLIST_ADD: f_dump_line_item_FI_CLIST_ADD(item, indent + 1); break;
#line 1405

#line 1460 "filter/f-inst.c"
 /* Dump line caller for FI_CLIST_DEL */
#line 1460
case FI_CLIST_DEL: f_dump_line_item_FI_CLIST_DEL(item, indent + 1); break;
#line 1460

#line 1506 "filter/f-inst.c"
 /* Dump line caller for FI_CLIST_FILTER */
#line 1506
case FI_CLIST_FILTER: f_dump_line_item_FI_CLIST_FILTER(item, indent + 1); break;
#line 1506

#line 1536 "filter/f-inst.c"
 /* Dump line caller for FI_ROA_CHECK_IMPLICIT */
#line 1536
case FI_ROA_CHECK_IMPLICIT: f_dump_line_item_FI_ROA_CHECK_IMPLICIT(item, indent + 1); break;
#line 1536

#line 1558 "filter/f-inst.c"
 /* Dump line caller for FI_ROA_CHECK_EXPLICIT */
#line 1558
case FI_ROA_CHECK_EXPLICIT: f_dump_line_item_FI_ROA_CHECK_EXPLICIT(item, indent + 1); break;
#line 1558

#line 1563 "filter/f-inst.c"
 /* Dump line caller for FI_FORMAT */
#line 1563
case FI_FORMAT: f_dump_line_item_FI_FORMAT(item, indent + 1); break;
#line 1563

#line 696 "filter/decl.m4"
 /* Dump line caller for FI_ASSERT */
#line 696
case FI_ASSERT: f_dump_line_item_FI_ASSERT(item, indent + 1); break;
#line 696

#line 557 "filter/decl.m4"

      default: bug("Unknown instruction %x in f_dump_line", item->fi_code);
    }
  }
  debug("%sFilter line %p dump done\n", INDENT, dest);
}

/* Linearize */
static uint
linearize(struct f_line *dest, const struct f_inst *what, uint pos)
{
  for ( ; what; what = what->next) {
    switch (what->fi_code) {
#line 284 "filter/f-inst.c"
 /* Linearize for FI_ADD */
#line 284
case FI_ADD: {
#line 284
#define whati (&(what->i_FI_ADD))
#line 284
#define item (&(dest->items[pos].i_FI_ADD))
#line 284
  #line 280 "filter/f-inst.c"

#line 280
pos = linearize(dest, whati->f1, pos);
#line 281 "filter/f-inst.c"

#line 281
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 284 "filter/f-inst.c"
#undef item
#line 284
  dest->items[pos].fi_code = what->fi_code;
#line 284
  dest->items[pos].flags = what->flags;
#line 284
  dest->items[pos].lineno = what->lineno;
#line 284
  break;
#line 284
}
#line 284

#line 289 "filter/f-inst.c"
 /* Linearize for FI_SUBTRACT */
#line 289
case FI_SUBTRACT: {
#line 289
#define whati (&(what->i_FI_SUBTRACT))
#line 289
#define item (&(dest->items[pos].i_FI_SUBTRACT))
#line 289
  #line 285 "filter/f-inst.c"

#line 285
pos = linearize(dest, whati->f1, pos);
#line 286 "filter/f-inst.c"

#line 286
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 289 "filter/f-inst.c"
#undef item
#line 289
  dest->items[pos].fi_code = what->fi_code;
#line 289
  dest->items[pos].flags = what->flags;
#line 289
  dest->items[pos].lineno = what->lineno;
#line 289
  break;
#line 289
}
#line 289

#line 294 "filter/f-inst.c"
 /* Linearize for FI_MULTIPLY */
#line 294
case FI_MULTIPLY: {
#line 294
#define whati (&(what->i_FI_MULTIPLY))
#line 294
#define item (&(dest->items[pos].i_FI_MULTIPLY))
#line 294
  #line 290 "filter/f-inst.c"

#line 290
pos = linearize(dest, whati->f1, pos);
#line 291 "filter/f-inst.c"

#line 291
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 294 "filter/f-inst.c"
#undef item
#line 294
  dest->items[pos].fi_code = what->fi_code;
#line 294
  dest->items[pos].flags = what->flags;
#line 294
  dest->items[pos].lineno = what->lineno;
#line 294
  break;
#line 294
}
#line 294

#line 300 "filter/f-inst.c"
 /* Linearize for FI_DIVIDE */
#line 300
case FI_DIVIDE: {
#line 300
#define whati (&(what->i_FI_DIVIDE))
#line 300
#define item (&(dest->items[pos].i_FI_DIVIDE))
#line 300
  #line 295 "filter/f-inst.c"

#line 295
pos = linearize(dest, whati->f1, pos);
#line 296 "filter/f-inst.c"

#line 296
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 300 "filter/f-inst.c"
#undef item
#line 300
  dest->items[pos].fi_code = what->fi_code;
#line 300
  dest->items[pos].flags = what->flags;
#line 300
  dest->items[pos].lineno = what->lineno;
#line 300
  break;
#line 300
}
#line 300

#line 310 "filter/f-inst.c"
 /* Linearize for FI_AND */
#line 310
case FI_AND: {
#line 310
#define whati (&(what->i_FI_AND))
#line 310
#define item (&(dest->items[pos].i_FI_AND))
#line 310
  #line 301 "filter/f-inst.c"

#line 301
pos = linearize(dest, whati->f1, pos);
#line 306 "filter/f-inst.c"
item->fl2 = f_linearize(whati->f2, 1);
#undef whati
#line 310 "filter/f-inst.c"
#undef item
#line 310
  dest->items[pos].fi_code = what->fi_code;
#line 310
  dest->items[pos].flags = what->flags;
#line 310
  dest->items[pos].lineno = what->lineno;
#line 310
  break;
#line 310
}
#line 310

#line 321 "filter/f-inst.c"
 /* Linearize for FI_OR */
#line 321
case FI_OR: {
#line 321
#define whati (&(what->i_FI_OR))
#line 321
#define item (&(dest->items[pos].i_FI_OR))
#line 321
  #line 311 "filter/f-inst.c"

#line 311
pos = linearize(dest, whati->f1, pos);
#line 316 "filter/f-inst.c"
item->fl2 = f_linearize(whati->f2, 1);
#undef whati
#line 321 "filter/f-inst.c"
#undef item
#line 321
  dest->items[pos].fi_code = what->fi_code;
#line 321
  dest->items[pos].flags = what->flags;
#line 321
  dest->items[pos].lineno = what->lineno;
#line 321
  break;
#line 321
}
#line 321

#line 331 "filter/f-inst.c"
 /* Linearize for FI_PAIR_CONSTRUCT */
#line 331
case FI_PAIR_CONSTRUCT: {
#line 331
#define whati (&(what->i_FI_PAIR_CONSTRUCT))
#line 331
#define item (&(dest->items[pos].i_FI_PAIR_CONSTRUCT))
#line 331
  #line 322 "filter/f-inst.c"

#line 322
pos = linearize(dest, whati->f1, pos);
#line 323 "filter/f-inst.c"

#line 323
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 331 "filter/f-inst.c"
#undef item
#line 331
  dest->items[pos].fi_code = what->fi_code;
#line 331
  dest->items[pos].flags = what->flags;
#line 331
  dest->items[pos].lineno = what->lineno;
#line 331
  break;
#line 331
}
#line 331

#line 371 "filter/f-inst.c"
 /* Linearize for FI_EC_CONSTRUCT */
#line 371
case FI_EC_CONSTRUCT: {
#line 371
#define whati (&(what->i_FI_EC_CONSTRUCT))
#line 371
#define item (&(dest->items[pos].i_FI_EC_CONSTRUCT))
#line 371
  #line 332 "filter/f-inst.c"

#line 332
pos = linearize(dest, whati->f1, pos);
#line 333 "filter/f-inst.c"

#line 333
pos = linearize(dest, whati->f2, pos);
#line 335 "filter/f-inst.c"
item->ecs = whati->ecs;
#line 335

#undef whati
#line 371 "filter/f-inst.c"
#undef item
#line 371
  dest->items[pos].fi_code = what->fi_code;
#line 371
  dest->items[pos].flags = what->flags;
#line 371
  dest->items[pos].lineno = what->lineno;
#line 371
  break;
#line 371
}
#line 371

#line 378 "filter/f-inst.c"
 /* Linearize for FI_LC_CONSTRUCT */
#line 378
case FI_LC_CONSTRUCT: {
#line 378
#define whati (&(what->i_FI_LC_CONSTRUCT))
#line 378
#define item (&(dest->items[pos].i_FI_LC_CONSTRUCT))
#line 378
  #line 372 "filter/f-inst.c"

#line 372
pos = linearize(dest, whati->f1, pos);
#line 373 "filter/f-inst.c"

#line 373
pos = linearize(dest, whati->f2, pos);
#line 374 "filter/f-inst.c"

#line 374
pos = linearize(dest, whati->f3, pos);
#undef whati
#line 378 "filter/f-inst.c"
#undef item
#line 378
  dest->items[pos].fi_code = what->fi_code;
#line 378
  dest->items[pos].flags = what->flags;
#line 378
  dest->items[pos].lineno = what->lineno;
#line 378
  break;
#line 378
}
#line 378

#line 428 "filter/f-inst.c"
 /* Linearize for FI_PATHMASK_CONSTRUCT */
#line 428
case FI_PATHMASK_CONSTRUCT: {
#line 428
#define whati (&(what->i_FI_PATHMASK_CONSTRUCT))
#line 428
#define item (&(dest->items[pos].i_FI_PATHMASK_CONSTRUCT))
#line 428
  #line 379 "filter/f-inst.c"
  pos = linearize(dest, whati->fvar, pos);
#line 379
  item->varcount = whati->varcount;
#undef whati
#line 428 "filter/f-inst.c"
#undef item
#line 428
  dest->items[pos].fi_code = what->fi_code;
#line 428
  dest->items[pos].flags = what->flags;
#line 428
  dest->items[pos].lineno = what->lineno;
#line 428
  break;
#line 428
}
#line 428

#line 435 "filter/f-inst.c"
 /* Linearize for FI_NEQ */
#line 435
case FI_NEQ: {
#line 435
#define whati (&(what->i_FI_NEQ))
#line 435
#define item (&(dest->items[pos].i_FI_NEQ))
#line 435
  #line 429 "filter/f-inst.c"

#line 429
pos = linearize(dest, whati->f1, pos);
#line 430 "filter/f-inst.c"

#line 430
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 435 "filter/f-inst.c"
#undef item
#line 435
  dest->items[pos].fi_code = what->fi_code;
#line 435
  dest->items[pos].flags = what->flags;
#line 435
  dest->items[pos].lineno = what->lineno;
#line 435
  break;
#line 435
}
#line 435

#line 442 "filter/f-inst.c"
 /* Linearize for FI_EQ */
#line 442
case FI_EQ: {
#line 442
#define whati (&(what->i_FI_EQ))
#line 442
#define item (&(dest->items[pos].i_FI_EQ))
#line 442
  #line 436 "filter/f-inst.c"

#line 436
pos = linearize(dest, whati->f1, pos);
#line 437 "filter/f-inst.c"

#line 437
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 442 "filter/f-inst.c"
#undef item
#line 442
  dest->items[pos].fi_code = what->fi_code;
#line 442
  dest->items[pos].flags = what->flags;
#line 442
  dest->items[pos].lineno = what->lineno;
#line 442
  break;
#line 442
}
#line 442

#line 453 "filter/f-inst.c"
 /* Linearize for FI_LT */
#line 453
case FI_LT: {
#line 453
#define whati (&(what->i_FI_LT))
#line 453
#define item (&(dest->items[pos].i_FI_LT))
#line 453
  #line 443 "filter/f-inst.c"

#line 443
pos = linearize(dest, whati->f1, pos);
#line 444 "filter/f-inst.c"

#line 444
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 453 "filter/f-inst.c"
#undef item
#line 453
  dest->items[pos].fi_code = what->fi_code;
#line 453
  dest->items[pos].flags = what->flags;
#line 453
  dest->items[pos].lineno = what->lineno;
#line 453
  break;
#line 453
}
#line 453

#line 464 "filter/f-inst.c"
 /* Linearize for FI_LTE */
#line 464
case FI_LTE: {
#line 464
#define whati (&(what->i_FI_LTE))
#line 464
#define item (&(dest->items[pos].i_FI_LTE))
#line 464
  #line 454 "filter/f-inst.c"

#line 454
pos = linearize(dest, whati->f1, pos);
#line 455 "filter/f-inst.c"

#line 455
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 464 "filter/f-inst.c"
#undef item
#line 464
  dest->items[pos].fi_code = what->fi_code;
#line 464
  dest->items[pos].flags = what->flags;
#line 464
  dest->items[pos].lineno = what->lineno;
#line 464
  break;
#line 464
}
#line 464

#line 469 "filter/f-inst.c"
 /* Linearize for FI_NOT */
#line 469
case FI_NOT: {
#line 469
#define whati (&(what->i_FI_NOT))
#line 469
#define item (&(dest->items[pos].i_FI_NOT))
#line 469
  #line 465 "filter/f-inst.c"

#line 465
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 469 "filter/f-inst.c"
#undef item
#line 469
  dest->items[pos].fi_code = what->fi_code;
#line 469
  dest->items[pos].flags = what->flags;
#line 469
  dest->items[pos].lineno = what->lineno;
#line 469
  break;
#line 469
}
#line 469

#line 478 "filter/f-inst.c"
 /* Linearize for FI_MATCH */
#line 478
case FI_MATCH: {
#line 478
#define whati (&(what->i_FI_MATCH))
#line 478
#define item (&(dest->items[pos].i_FI_MATCH))
#line 478
  #line 470 "filter/f-inst.c"

#line 470
pos = linearize(dest, whati->f1, pos);
#line 471 "filter/f-inst.c"

#line 471
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 478 "filter/f-inst.c"
#undef item
#line 478
  dest->items[pos].fi_code = what->fi_code;
#line 478
  dest->items[pos].flags = what->flags;
#line 478
  dest->items[pos].lineno = what->lineno;
#line 478
  break;
#line 478
}
#line 478

#line 487 "filter/f-inst.c"
 /* Linearize for FI_NOT_MATCH */
#line 487
case FI_NOT_MATCH: {
#line 487
#define whati (&(what->i_FI_NOT_MATCH))
#line 487
#define item (&(dest->items[pos].i_FI_NOT_MATCH))
#line 487
  #line 479 "filter/f-inst.c"

#line 479
pos = linearize(dest, whati->f1, pos);
#line 480 "filter/f-inst.c"

#line 480
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 487 "filter/f-inst.c"
#undef item
#line 487
  dest->items[pos].fi_code = what->fi_code;
#line 487
  dest->items[pos].flags = what->flags;
#line 487
  dest->items[pos].lineno = what->lineno;
#line 487
  break;
#line 487
}
#line 487

#line 492 "filter/f-inst.c"
 /* Linearize for FI_DEFINED */
#line 492
case FI_DEFINED: {
#line 492
#define whati (&(what->i_FI_DEFINED))
#line 492
#define item (&(dest->items[pos].i_FI_DEFINED))
#line 492
  #line 488 "filter/f-inst.c"

#line 488
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 492 "filter/f-inst.c"
#undef item
#line 492
  dest->items[pos].fi_code = what->fi_code;
#line 492
  dest->items[pos].flags = what->flags;
#line 492
  dest->items[pos].lineno = what->lineno;
#line 492
  break;
#line 492
}
#line 492

#line 504 "filter/f-inst.c"
 /* Linearize for FI_TYPE */
#line 504
case FI_TYPE: {
#line 504
#define whati (&(what->i_FI_TYPE))
#line 504
#define item (&(dest->items[pos].i_FI_TYPE))
#line 504
  #line 493 "filter/f-inst.c"

#line 493
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 504 "filter/f-inst.c"
#undef item
#line 504
  dest->items[pos].fi_code = what->fi_code;
#line 504
  dest->items[pos].flags = what->flags;
#line 504
  dest->items[pos].lineno = what->lineno;
#line 504
  break;
#line 504
}
#line 504

#line 509 "filter/f-inst.c"
 /* Linearize for FI_IS_V4 */
#line 509
case FI_IS_V4: {
#line 509
#define whati (&(what->i_FI_IS_V4))
#line 509
#define item (&(dest->items[pos].i_FI_IS_V4))
#line 509
  #line 505 "filter/f-inst.c"

#line 505
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 509 "filter/f-inst.c"
#undef item
#line 509
  dest->items[pos].fi_code = what->fi_code;
#line 509
  dest->items[pos].flags = what->flags;
#line 509
  dest->items[pos].lineno = what->lineno;
#line 509
  break;
#line 509
}
#line 509

#line 522 "filter/f-inst.c"
 /* Linearize for FI_VAR_INIT */
#line 522
case FI_VAR_INIT: {
#line 522
#define whati (&(what->i_FI_VAR_INIT))
#line 522
#define item (&(dest->items[pos].i_FI_VAR_INIT))
#line 522
  #line 511 "filter/f-inst.c"

#line 511
pos = linearize(dest, whati->f1, pos);
#line 512 "filter/f-inst.c"
item->sym = whati->sym;
#line 512

#undef whati
#line 522 "filter/f-inst.c"
#undef item
#line 522
  dest->items[pos].fi_code = what->fi_code;
#line 522
  dest->items[pos].flags = what->flags;
#line 522
  dest->items[pos].lineno = what->lineno;
#line 522
  break;
#line 522
}
#line 522

#line 531 "filter/f-inst.c"
 /* Linearize for FI_VAR_SET */
#line 531
case FI_VAR_SET: {
#line 531
#define whati (&(what->i_FI_VAR_SET))
#line 531
#define item (&(dest->items[pos].i_FI_VAR_SET))
#line 531
  #line 524 "filter/f-inst.c"

#line 524
pos = linearize(dest, whati->f1, pos);
#line 525 "filter/f-inst.c"
item->sym = whati->sym;
#line 525

#undef whati
#line 531 "filter/f-inst.c"
#undef item
#line 531
  dest->items[pos].fi_code = what->fi_code;
#line 531
  dest->items[pos].flags = what->flags;
#line 531
  dest->items[pos].lineno = what->lineno;
#line 531
  break;
#line 531
}
#line 531

#line 538 "filter/f-inst.c"
 /* Linearize for FI_VAR_GET */
#line 538
case FI_VAR_GET: {
#line 538
#define whati (&(what->i_FI_VAR_GET))
#line 538
#define item (&(dest->items[pos].i_FI_VAR_GET))
#line 538
  #line 532 "filter/f-inst.c"
item->sym = whati->sym;
#line 532

#undef whati
#line 538 "filter/f-inst.c"
#undef item
#line 538
  dest->items[pos].fi_code = what->fi_code;
#line 538
  dest->items[pos].flags = what->flags;
#line 538
  dest->items[pos].lineno = what->lineno;
#line 538
  break;
#line 538
}
#line 538

#line 551 "filter/f-inst.c"
 /* Linearize for FI_CONSTANT */
#line 551
case FI_CONSTANT: {
#line 551
#define whati (&(what->i_FI_CONSTANT))
#line 551
#define item (&(dest->items[pos].i_FI_CONSTANT))
#line 551
  #line 539 "filter/f-inst.c"
item->val = whati->val;
#line 539

#undef whati
#line 551 "filter/f-inst.c"
#undef item
#line 551
  dest->items[pos].fi_code = what->fi_code;
#line 551
  dest->items[pos].flags = what->flags;
#line 551
  dest->items[pos].lineno = what->lineno;
#line 551
  break;
#line 551
}
#line 551

#line 585 "filter/f-inst.c"
 /* Linearize for FI_FOR_INIT */
#line 585
case FI_FOR_INIT: {
#line 585
#define whati (&(what->i_FI_FOR_INIT))
#line 585
#define item (&(dest->items[pos].i_FI_FOR_INIT))
#line 585
  #line 553 "filter/f-inst.c"

#line 553
pos = linearize(dest, whati->f1, pos);
#line 554 "filter/f-inst.c"
item->sym = whati->sym;
#line 554

#undef whati
#line 585 "filter/f-inst.c"
#undef item
#line 585
  dest->items[pos].fi_code = what->fi_code;
#line 585
  dest->items[pos].flags = what->flags;
#line 585
  dest->items[pos].lineno = what->lineno;
#line 585
  break;
#line 585
}
#line 585

#line 639 "filter/f-inst.c"
 /* Linearize for FI_FOR_NEXT */
#line 639
case FI_FOR_NEXT: {
#line 639
#define whati (&(what->i_FI_FOR_NEXT))
#line 639
#define item (&(dest->items[pos].i_FI_FOR_NEXT))
#line 639
  #line 587 "filter/f-inst.c"
item->sym = whati->sym;
#line 587

#line 630 "filter/f-inst.c"
item->fl1 = f_linearize(whati->f1, 0);
#undef whati
#line 639 "filter/f-inst.c"
#undef item
#line 639
  dest->items[pos].fi_code = what->fi_code;
#line 639
  dest->items[pos].flags = what->flags;
#line 639
  dest->items[pos].lineno = what->lineno;
#line 639
  break;
#line 639
}
#line 639

#line 647 "filter/f-inst.c"
 /* Linearize for FI_CONDITION */
#line 647
case FI_CONDITION: {
#line 647
#define whati (&(what->i_FI_CONDITION))
#line 647
#define item (&(dest->items[pos].i_FI_CONDITION))
#line 647
  #line 640 "filter/f-inst.c"

#line 640
pos = linearize(dest, whati->f1, pos);
#line 642 "filter/f-inst.c"
item->fl2 = f_linearize(whati->f2, 0);
#line 644 "filter/f-inst.c"
item->fl3 = f_linearize(whati->f3, 0);
#undef whati
#line 647 "filter/f-inst.c"
#undef item
#line 647
  dest->items[pos].fi_code = what->fi_code;
#line 647
  dest->items[pos].flags = what->flags;
#line 647
  dest->items[pos].lineno = what->lineno;
#line 647
  break;
#line 647
}
#line 647

#line 656 "filter/f-inst.c"
 /* Linearize for FI_PRINT */
#line 656
case FI_PRINT: {
#line 656
#define whati (&(what->i_FI_PRINT))
#line 656
#define item (&(dest->items[pos].i_FI_PRINT))
#line 656
  #line 649 "filter/f-inst.c"
  pos = linearize(dest, whati->fvar, pos);
#line 649
  item->varcount = whati->varcount;
#undef whati
#line 656 "filter/f-inst.c"
#undef item
#line 656
  dest->items[pos].fi_code = what->fi_code;
#line 656
  dest->items[pos].flags = what->flags;
#line 656
  dest->items[pos].lineno = what->lineno;
#line 656
  break;
#line 656
}
#line 656

#line 663 "filter/f-inst.c"
 /* Linearize for FI_FLUSH */
#line 663
case FI_FLUSH: {
#line 663
#define whati (&(what->i_FI_FLUSH))
#line 663
#define item (&(dest->items[pos].i_FI_FLUSH))
#line 663
  #undef whati
#line 663
#undef item
#line 663
  dest->items[pos].fi_code = what->fi_code;
#line 663
  dest->items[pos].flags = what->flags;
#line 663
  dest->items[pos].lineno = what->lineno;
#line 663
  break;
#line 663
}
#line 663

#line 677 "filter/f-inst.c"
 /* Linearize for FI_DIE */
#line 677
case FI_DIE: {
#line 677
#define whati (&(what->i_FI_DIE))
#line 677
#define item (&(dest->items[pos].i_FI_DIE))
#line 677
  #line 665 "filter/f-inst.c"
item->fret = whati->fret;
#line 665

#undef whati
#line 677 "filter/f-inst.c"
#undef item
#line 677
  dest->items[pos].fi_code = what->fi_code;
#line 677
  dest->items[pos].flags = what->flags;
#line 677
  dest->items[pos].lineno = what->lineno;
#line 677
  break;
#line 677
}
#line 677

#line 705 "filter/f-inst.c"
 /* Linearize for FI_RTA_GET */
#line 705
case FI_RTA_GET: {
#line 705
#define whati (&(what->i_FI_RTA_GET))
#line 705
#define item (&(dest->items[pos].i_FI_RTA_GET))
#line 705
  #line 679 "filter/f-inst.c"
item->sa = whati->sa;
#line 679

#undef whati
#line 705 "filter/f-inst.c"
#undef item
#line 705
  dest->items[pos].fi_code = what->fi_code;
#line 705
  dest->items[pos].flags = what->flags;
#line 705
  dest->items[pos].lineno = what->lineno;
#line 705
  break;
#line 705
}
#line 705

#line 820 "filter/f-inst.c"
 /* Linearize for FI_RTA_SET */
#line 820
case FI_RTA_SET: {
#line 820
#define whati (&(what->i_FI_RTA_SET))
#line 820
#define item (&(dest->items[pos].i_FI_RTA_SET))
#line 820
  #line 707 "filter/f-inst.c"

#line 707
pos = linearize(dest, whati->f1, pos);
#line 708 "filter/f-inst.c"
item->sa = whati->sa;
#line 708

#undef whati
#line 820 "filter/f-inst.c"
#undef item
#line 820
  dest->items[pos].fi_code = what->fi_code;
#line 820
  dest->items[pos].flags = what->flags;
#line 820
  dest->items[pos].lineno = what->lineno;
#line 820
  break;
#line 820
}
#line 820

#line 892 "filter/f-inst.c"
 /* Linearize for FI_EA_GET */
#line 892
case FI_EA_GET: {
#line 892
#define whati (&(what->i_FI_EA_GET))
#line 892
#define item (&(dest->items[pos].i_FI_EA_GET))
#line 892
  #line 821 "filter/f-inst.c"
item->da = whati->da;
#line 821

#undef whati
#line 892 "filter/f-inst.c"
#undef item
#line 892
  dest->items[pos].fi_code = what->fi_code;
#line 892
  dest->items[pos].flags = what->flags;
#line 892
  dest->items[pos].lineno = what->lineno;
#line 892
  break;
#line 892
}
#line 892

#line 959 "filter/f-inst.c"
 /* Linearize for FI_EA_SET */
#line 959
case FI_EA_SET: {
#line 959
#define whati (&(what->i_FI_EA_SET))
#line 959
#define item (&(dest->items[pos].i_FI_EA_SET))
#line 959
  #line 895 "filter/f-inst.c"

#line 895
pos = linearize(dest, whati->f1, pos);
#line 896 "filter/f-inst.c"
item->da = whati->da;
#line 896

#undef whati
#line 959 "filter/f-inst.c"
#undef item
#line 959
  dest->items[pos].fi_code = what->fi_code;
#line 959
  dest->items[pos].flags = what->flags;
#line 959
  dest->items[pos].lineno = what->lineno;
#line 959
  break;
#line 959
}
#line 959

#line 968 "filter/f-inst.c"
 /* Linearize for FI_EA_UNSET */
#line 968
case FI_EA_UNSET: {
#line 968
#define whati (&(what->i_FI_EA_UNSET))
#line 968
#define item (&(dest->items[pos].i_FI_EA_UNSET))
#line 968
  #line 960 "filter/f-inst.c"
item->da = whati->da;
#line 960

#undef whati
#line 968 "filter/f-inst.c"
#undef item
#line 968
  dest->items[pos].fi_code = what->fi_code;
#line 968
  dest->items[pos].flags = what->flags;
#line 968
  dest->items[pos].lineno = what->lineno;
#line 968
  break;
#line 968
}
#line 968

#line 980 "filter/f-inst.c"
 /* Linearize for FI_LENGTH */
#line 980
case FI_LENGTH: {
#line 980
#define whati (&(what->i_FI_LENGTH))
#line 980
#define item (&(dest->items[pos].i_FI_LENGTH))
#line 980
  #line 969 "filter/f-inst.c"

#line 969
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 980 "filter/f-inst.c"
#undef item
#line 980
  dest->items[pos].fi_code = what->fi_code;
#line 980
  dest->items[pos].flags = what->flags;
#line 980
  dest->items[pos].lineno = what->lineno;
#line 980
  break;
#line 980
}
#line 980

#line 1015 "filter/f-inst.c"
 /* Linearize for FI_NET_SRC */
#line 1015
case FI_NET_SRC: {
#line 1015
#define whati (&(what->i_FI_NET_SRC))
#line 1015
#define item (&(dest->items[pos].i_FI_NET_SRC))
#line 1015
  #line 981 "filter/f-inst.c"

#line 981
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1015 "filter/f-inst.c"
#undef item
#line 1015
  dest->items[pos].fi_code = what->fi_code;
#line 1015
  dest->items[pos].flags = what->flags;
#line 1015
  dest->items[pos].lineno = what->lineno;
#line 1015
  break;
#line 1015
}
#line 1015

#line 1050 "filter/f-inst.c"
 /* Linearize for FI_NET_DST */
#line 1050
case FI_NET_DST: {
#line 1050
#define whati (&(what->i_FI_NET_DST))
#line 1050
#define item (&(dest->items[pos].i_FI_NET_DST))
#line 1050
  #line 1016 "filter/f-inst.c"

#line 1016
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1050 "filter/f-inst.c"
#undef item
#line 1050
  dest->items[pos].fi_code = what->fi_code;
#line 1050
  dest->items[pos].flags = what->flags;
#line 1050
  dest->items[pos].lineno = what->lineno;
#line 1050
  break;
#line 1050
}
#line 1050

#line 1060 "filter/f-inst.c"
 /* Linearize for FI_ROA_MAXLEN */
#line 1060
case FI_ROA_MAXLEN: {
#line 1060
#define whati (&(what->i_FI_ROA_MAXLEN))
#line 1060
#define item (&(dest->items[pos].i_FI_ROA_MAXLEN))
#line 1060
  #line 1051 "filter/f-inst.c"

#line 1051
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1060 "filter/f-inst.c"
#undef item
#line 1060
  dest->items[pos].fi_code = what->fi_code;
#line 1060
  dest->items[pos].flags = what->flags;
#line 1060
  dest->items[pos].lineno = what->lineno;
#line 1060
  break;
#line 1060
}
#line 1060

#line 1087 "filter/f-inst.c"
 /* Linearize for FI_ASN */
#line 1087
case FI_ASN: {
#line 1087
#define whati (&(what->i_FI_ASN))
#line 1087
#define item (&(dest->items[pos].i_FI_ASN))
#line 1087
  #line 1061 "filter/f-inst.c"

#line 1061
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1087 "filter/f-inst.c"
#undef item
#line 1087
  dest->items[pos].fi_code = what->fi_code;
#line 1087
  dest->items[pos].flags = what->flags;
#line 1087
  dest->items[pos].lineno = what->lineno;
#line 1087
  break;
#line 1087
}
#line 1087

#line 1092 "filter/f-inst.c"
 /* Linearize for FI_IP */
#line 1092
case FI_IP: {
#line 1092
#define whati (&(what->i_FI_IP))
#line 1092
#define item (&(dest->items[pos].i_FI_IP))
#line 1092
  #line 1088 "filter/f-inst.c"

#line 1088
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1092 "filter/f-inst.c"
#undef item
#line 1092
  dest->items[pos].fi_code = what->fi_code;
#line 1092
  dest->items[pos].flags = what->flags;
#line 1092
  dest->items[pos].lineno = what->lineno;
#line 1092
  break;
#line 1092
}
#line 1092

#line 1099 "filter/f-inst.c"
 /* Linearize for FI_ROUTE_DISTINGUISHER */
#line 1099
case FI_ROUTE_DISTINGUISHER: {
#line 1099
#define whati (&(what->i_FI_ROUTE_DISTINGUISHER))
#line 1099
#define item (&(dest->items[pos].i_FI_ROUTE_DISTINGUISHER))
#line 1099
  #line 1093 "filter/f-inst.c"

#line 1093
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1099 "filter/f-inst.c"
#undef item
#line 1099
  dest->items[pos].fi_code = what->fi_code;
#line 1099
  dest->items[pos].flags = what->flags;
#line 1099
  dest->items[pos].lineno = what->lineno;
#line 1099
  break;
#line 1099
}
#line 1099

#line 1106 "filter/f-inst.c"
 /* Linearize for FI_AS_PATH_FIRST */
#line 1106
case FI_AS_PATH_FIRST: {
#line 1106
#define whati (&(what->i_FI_AS_PATH_FIRST))
#line 1106
#define item (&(dest->items[pos].i_FI_AS_PATH_FIRST))
#line 1106
  #line 1100 "filter/f-inst.c"

#line 1100
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1106 "filter/f-inst.c"
#undef item
#line 1106
  dest->items[pos].fi_code = what->fi_code;
#line 1106
  dest->items[pos].flags = what->flags;
#line 1106
  dest->items[pos].lineno = what->lineno;
#line 1106
  break;
#line 1106
}
#line 1106

#line 1113 "filter/f-inst.c"
 /* Linearize for FI_AS_PATH_LAST */
#line 1113
case FI_AS_PATH_LAST: {
#line 1113
#define whati (&(what->i_FI_AS_PATH_LAST))
#line 1113
#define item (&(dest->items[pos].i_FI_AS_PATH_LAST))
#line 1113
  #line 1107 "filter/f-inst.c"

#line 1107
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1113 "filter/f-inst.c"
#undef item
#line 1113
  dest->items[pos].fi_code = what->fi_code;
#line 1113
  dest->items[pos].flags = what->flags;
#line 1113
  dest->items[pos].lineno = what->lineno;
#line 1113
  break;
#line 1113
}
#line 1113

#line 1118 "filter/f-inst.c"
 /* Linearize for FI_AS_PATH_LAST_NAG */
#line 1118
case FI_AS_PATH_LAST_NAG: {
#line 1118
#define whati (&(what->i_FI_AS_PATH_LAST_NAG))
#line 1118
#define item (&(dest->items[pos].i_FI_AS_PATH_LAST_NAG))
#line 1118
  #line 1114 "filter/f-inst.c"

#line 1114
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1118 "filter/f-inst.c"
#undef item
#line 1118
  dest->items[pos].fi_code = what->fi_code;
#line 1118
  dest->items[pos].flags = what->flags;
#line 1118
  dest->items[pos].lineno = what->lineno;
#line 1118
  break;
#line 1118
}
#line 1118

#line 1123 "filter/f-inst.c"
 /* Linearize for FI_PAIR_DATA */
#line 1123
case FI_PAIR_DATA: {
#line 1123
#define whati (&(what->i_FI_PAIR_DATA))
#line 1123
#define item (&(dest->items[pos].i_FI_PAIR_DATA))
#line 1123
  #line 1119 "filter/f-inst.c"

#line 1119
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1123 "filter/f-inst.c"
#undef item
#line 1123
  dest->items[pos].fi_code = what->fi_code;
#line 1123
  dest->items[pos].flags = what->flags;
#line 1123
  dest->items[pos].lineno = what->lineno;
#line 1123
  break;
#line 1123
}
#line 1123

#line 1128 "filter/f-inst.c"
 /* Linearize for FI_LC_DATA1 */
#line 1128
case FI_LC_DATA1: {
#line 1128
#define whati (&(what->i_FI_LC_DATA1))
#line 1128
#define item (&(dest->items[pos].i_FI_LC_DATA1))
#line 1128
  #line 1124 "filter/f-inst.c"

#line 1124
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1128 "filter/f-inst.c"
#undef item
#line 1128
  dest->items[pos].fi_code = what->fi_code;
#line 1128
  dest->items[pos].flags = what->flags;
#line 1128
  dest->items[pos].lineno = what->lineno;
#line 1128
  break;
#line 1128
}
#line 1128

#line 1133 "filter/f-inst.c"
 /* Linearize for FI_LC_DATA2 */
#line 1133
case FI_LC_DATA2: {
#line 1133
#define whati (&(what->i_FI_LC_DATA2))
#line 1133
#define item (&(dest->items[pos].i_FI_LC_DATA2))
#line 1133
  #line 1129 "filter/f-inst.c"

#line 1129
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1133 "filter/f-inst.c"
#undef item
#line 1133
  dest->items[pos].fi_code = what->fi_code;
#line 1133
  dest->items[pos].flags = what->flags;
#line 1133
  dest->items[pos].lineno = what->lineno;
#line 1133
  break;
#line 1133
}
#line 1133

#line 1167 "filter/f-inst.c"
 /* Linearize for FI_MIN */
#line 1167
case FI_MIN: {
#line 1167
#define whati (&(what->i_FI_MIN))
#line 1167
#define item (&(dest->items[pos].i_FI_MIN))
#line 1167
  #line 1134 "filter/f-inst.c"

#line 1134
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1167 "filter/f-inst.c"
#undef item
#line 1167
  dest->items[pos].fi_code = what->fi_code;
#line 1167
  dest->items[pos].flags = what->flags;
#line 1167
  dest->items[pos].lineno = what->lineno;
#line 1167
  break;
#line 1167
}
#line 1167

#line 1201 "filter/f-inst.c"
 /* Linearize for FI_MAX */
#line 1201
case FI_MAX: {
#line 1201
#define whati (&(what->i_FI_MAX))
#line 1201
#define item (&(dest->items[pos].i_FI_MAX))
#line 1201
  #line 1168 "filter/f-inst.c"

#line 1168
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1201 "filter/f-inst.c"
#undef item
#line 1201
  dest->items[pos].fi_code = what->fi_code;
#line 1201
  dest->items[pos].flags = what->flags;
#line 1201
  dest->items[pos].lineno = what->lineno;
#line 1201
  break;
#line 1201
}
#line 1201

#line 1227 "filter/f-inst.c"
 /* Linearize for FI_RETURN */
#line 1227
case FI_RETURN: {
#line 1227
#define whati (&(what->i_FI_RETURN))
#line 1227
#define item (&(dest->items[pos].i_FI_RETURN))
#line 1227
  #line 1204 "filter/f-inst.c"

#line 1204
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1227 "filter/f-inst.c"
#undef item
#line 1227
  dest->items[pos].fi_code = what->fi_code;
#line 1227
  dest->items[pos].flags = what->flags;
#line 1227
  dest->items[pos].lineno = what->lineno;
#line 1227
  break;
#line 1227
}
#line 1227

#line 1288 "filter/f-inst.c"
 /* Linearize for FI_CALL */
#line 1288
case FI_CALL: {
#line 1288
#define whati (&(what->i_FI_CALL))
#line 1288
#define item (&(dest->items[pos].i_FI_CALL))
#line 1288
  #line 1229 "filter/f-inst.c"
  pos = linearize(dest, whati->fvar, pos);
#line 1229
  item->varcount = whati->varcount;
#line 1230 "filter/f-inst.c"
item->sym = whati->sym;
#line 1230

#undef whati
#line 1288 "filter/f-inst.c"
#undef item
#line 1288
  dest->items[pos].fi_code = what->fi_code;
#line 1288
  dest->items[pos].flags = what->flags;
#line 1288
  dest->items[pos].lineno = what->lineno;
#line 1288
  break;
#line 1288
}
#line 1288

#line 1293 "filter/f-inst.c"
 /* Linearize for FI_DROP_RESULT */
#line 1293
case FI_DROP_RESULT: {
#line 1293
#define whati (&(what->i_FI_DROP_RESULT))
#line 1293
#define item (&(dest->items[pos].i_FI_DROP_RESULT))
#line 1293
  #line 1290 "filter/f-inst.c"

#line 1290
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1293 "filter/f-inst.c"
#undef item
#line 1293
  dest->items[pos].fi_code = what->fi_code;
#line 1293
  dest->items[pos].flags = what->flags;
#line 1293
  dest->items[pos].lineno = what->lineno;
#line 1293
  break;
#line 1293
}
#line 1293

#line 1334 "filter/f-inst.c"
 /* Linearize for FI_SWITCH */
#line 1334
case FI_SWITCH: {
#line 1334
#define whati (&(what->i_FI_SWITCH))
#line 1334
#define item (&(dest->items[pos].i_FI_SWITCH))
#line 1334
  #line 1294 "filter/f-inst.c"

#line 1294
pos = linearize(dest, whati->f1, pos);
#line 1296 "filter/f-inst.c"
item->tree = whati->tree;
#line 1296


#line 1299 "filter/f-inst.c"
    /* Linearize all branches in switch */
    struct f_inst *last_inst = NULL;
    struct f_line *last_line = NULL;
    for (struct f_tree *t = whati->tree; t; t = t->left)
    {
      if (t->data != last_inst)
      {
	last_inst = t->data;
	last_line = f_linearize(t->data, 0);
      }

      t->data = last_line;
    }

    /* Balance the tree */
    item->tree = build_tree(whati->tree);

    #undef whati
#line 1334 "filter/f-inst.c"
#undef item
#line 1334
  dest->items[pos].fi_code = what->fi_code;
#line 1334
  dest->items[pos].flags = what->flags;
#line 1334
  dest->items[pos].lineno = what->lineno;
#line 1334
  break;
#line 1334
}
#line 1334

#line 1342 "filter/f-inst.c"
 /* Linearize for FI_IP_MASK */
#line 1342
case FI_IP_MASK: {
#line 1342
#define whati (&(what->i_FI_IP_MASK))
#line 1342
#define item (&(dest->items[pos].i_FI_IP_MASK))
#line 1342
  #line 1335 "filter/f-inst.c"

#line 1335
pos = linearize(dest, whati->f1, pos);
#line 1336 "filter/f-inst.c"

#line 1336
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 1342 "filter/f-inst.c"
#undef item
#line 1342
  dest->items[pos].fi_code = what->fi_code;
#line 1342
  dest->items[pos].flags = what->flags;
#line 1342
  dest->items[pos].lineno = what->lineno;
#line 1342
  break;
#line 1342
}
#line 1342

#line 1348 "filter/f-inst.c"
 /* Linearize for FI_PATH_PREPEND */
#line 1348
case FI_PATH_PREPEND: {
#line 1348
#define whati (&(what->i_FI_PATH_PREPEND))
#line 1348
#define item (&(dest->items[pos].i_FI_PATH_PREPEND))
#line 1348
  #line 1343 "filter/f-inst.c"

#line 1343
pos = linearize(dest, whati->f1, pos);
#line 1344 "filter/f-inst.c"

#line 1344
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 1348 "filter/f-inst.c"
#undef item
#line 1348
  dest->items[pos].fi_code = what->fi_code;
#line 1348
  dest->items[pos].flags = what->flags;
#line 1348
  dest->items[pos].lineno = what->lineno;
#line 1348
  break;
#line 1348
}
#line 1348

#line 1405 "filter/f-inst.c"
 /* Linearize for FI_CLIST_ADD */
#line 1405
case FI_CLIST_ADD: {
#line 1405
#define whati (&(what->i_FI_CLIST_ADD))
#line 1405
#define item (&(dest->items[pos].i_FI_CLIST_ADD))
#line 1405
  #line 1349 "filter/f-inst.c"

#line 1349
pos = linearize(dest, whati->f1, pos);
#line 1350 "filter/f-inst.c"

#line 1350
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 1405 "filter/f-inst.c"
#undef item
#line 1405
  dest->items[pos].fi_code = what->fi_code;
#line 1405
  dest->items[pos].flags = what->flags;
#line 1405
  dest->items[pos].lineno = what->lineno;
#line 1405
  break;
#line 1405
}
#line 1405

#line 1460 "filter/f-inst.c"
 /* Linearize for FI_CLIST_DEL */
#line 1460
case FI_CLIST_DEL: {
#line 1460
#define whati (&(what->i_FI_CLIST_DEL))
#line 1460
#define item (&(dest->items[pos].i_FI_CLIST_DEL))
#line 1460
  #line 1406 "filter/f-inst.c"

#line 1406
pos = linearize(dest, whati->f1, pos);
#line 1407 "filter/f-inst.c"

#line 1407
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 1460 "filter/f-inst.c"
#undef item
#line 1460
  dest->items[pos].fi_code = what->fi_code;
#line 1460
  dest->items[pos].flags = what->flags;
#line 1460
  dest->items[pos].lineno = what->lineno;
#line 1460
  break;
#line 1460
}
#line 1460

#line 1506 "filter/f-inst.c"
 /* Linearize for FI_CLIST_FILTER */
#line 1506
case FI_CLIST_FILTER: {
#line 1506
#define whati (&(what->i_FI_CLIST_FILTER))
#line 1506
#define item (&(dest->items[pos].i_FI_CLIST_FILTER))
#line 1506
  #line 1461 "filter/f-inst.c"

#line 1461
pos = linearize(dest, whati->f1, pos);
#line 1462 "filter/f-inst.c"

#line 1462
pos = linearize(dest, whati->f2, pos);
#undef whati
#line 1506 "filter/f-inst.c"
#undef item
#line 1506
  dest->items[pos].fi_code = what->fi_code;
#line 1506
  dest->items[pos].flags = what->flags;
#line 1506
  dest->items[pos].lineno = what->lineno;
#line 1506
  break;
#line 1506
}
#line 1506

#line 1536 "filter/f-inst.c"
 /* Linearize for FI_ROA_CHECK_IMPLICIT */
#line 1536
case FI_ROA_CHECK_IMPLICIT: {
#line 1536
#define whati (&(what->i_FI_ROA_CHECK_IMPLICIT))
#line 1536
#define item (&(dest->items[pos].i_FI_ROA_CHECK_IMPLICIT))
#line 1536
  #line 1508 "filter/f-inst.c"
item->rtc = whati->rtc;
#line 1508

#undef whati
#line 1536 "filter/f-inst.c"
#undef item
#line 1536
  dest->items[pos].fi_code = what->fi_code;
#line 1536
  dest->items[pos].flags = what->flags;
#line 1536
  dest->items[pos].lineno = what->lineno;
#line 1536
  break;
#line 1536
}
#line 1536

#line 1558 "filter/f-inst.c"
 /* Linearize for FI_ROA_CHECK_EXPLICIT */
#line 1558
case FI_ROA_CHECK_EXPLICIT: {
#line 1558
#define whati (&(what->i_FI_ROA_CHECK_EXPLICIT))
#line 1558
#define item (&(dest->items[pos].i_FI_ROA_CHECK_EXPLICIT))
#line 1558
  #line 1538 "filter/f-inst.c"

#line 1538
pos = linearize(dest, whati->f1, pos);
#line 1539 "filter/f-inst.c"

#line 1539
pos = linearize(dest, whati->f2, pos);
#line 1540 "filter/f-inst.c"
item->rtc = whati->rtc;
#line 1540

#undef whati
#line 1558 "filter/f-inst.c"
#undef item
#line 1558
  dest->items[pos].fi_code = what->fi_code;
#line 1558
  dest->items[pos].flags = what->flags;
#line 1558
  dest->items[pos].lineno = what->lineno;
#line 1558
  break;
#line 1558
}
#line 1558

#line 1563 "filter/f-inst.c"
 /* Linearize for FI_FORMAT */
#line 1563
case FI_FORMAT: {
#line 1563
#define whati (&(what->i_FI_FORMAT))
#line 1563
#define item (&(dest->items[pos].i_FI_FORMAT))
#line 1563
  #line 1559 "filter/f-inst.c"

#line 1559
pos = linearize(dest, whati->f1, pos);
#undef whati
#line 1563 "filter/f-inst.c"
#undef item
#line 1563
  dest->items[pos].fi_code = what->fi_code;
#line 1563
  dest->items[pos].flags = what->flags;
#line 1563
  dest->items[pos].lineno = what->lineno;
#line 1563
  break;
#line 1563
}
#line 1563

#line 696 "filter/decl.m4"
 /* Linearize for FI_ASSERT */
#line 696
case FI_ASSERT: {
#line 696
#define whati (&(what->i_FI_ASSERT))
#line 696
#define item (&(dest->items[pos].i_FI_ASSERT))
#line 696
  #line 1565 "filter/f-inst.c"

#line 1565
pos = linearize(dest, whati->f1, pos);
#line 1567 "filter/f-inst.c"
item->s = whati->s;
#line 1567

#undef whati
#line 696 "filter/decl.m4"
#undef item
#line 696
  dest->items[pos].fi_code = what->fi_code;
#line 696
  dest->items[pos].flags = what->flags;
#line 696
  dest->items[pos].lineno = what->lineno;
#line 696
  break;
#line 696
}
#line 696

#line 570 "filter/decl.m4"

    }
    pos++;
  }
  return pos;
}

struct f_line *
f_linearize_concat(const struct f_inst * const inst[], uint count, uint results)
{
  uint len = 0;
  for (uint i=0; i<count; i++)
    for (const struct f_inst *what = inst[i]; what; what = what->next)
      len += what->size;

  struct f_line *out = cfg_allocz(sizeof(struct f_line) + sizeof(struct f_line_item)*len);

  for (uint i=0; i<count; i++)
    out->len = linearize(out, inst[i], out->len);

  out->results = results;

#ifdef LOCAL_DEBUG
  f_dump_line(out, 0);
#endif
  return out;
}

/* Filter line comparison */
int
f_same(const struct f_line *fl1, const struct f_line *fl2)
{
  if ((!fl1) && (!fl2))
    return 1;
  if ((!fl1) || (!fl2))
    return 0;
  if (fl1->len != fl2->len)
    return 0;
  for (uint i=0; i<fl1->len; i++) {
#define f1_ (&(fl1->items[i]))
#define f2_ (&(fl2->items[i]))
    if (f1_->fi_code != f2_->fi_code)
      return 0;
    if (f1_->flags != f2_->flags)
      return 0;

    switch(f1_->fi_code) {
#line 284 "filter/f-inst.c"
 /* Comparison for FI_ADD */
#line 284
case FI_ADD:
#line 284
#define f1 (&(f1_->i_FI_ADD))
#line 284
#define f2 (&(f2_->i_FI_ADD))
#line 284
#undef f1
#line 284
#undef f2
#line 284
break;
#line 284

#line 289 "filter/f-inst.c"
 /* Comparison for FI_SUBTRACT */
#line 289
case FI_SUBTRACT:
#line 289
#define f1 (&(f1_->i_FI_SUBTRACT))
#line 289
#define f2 (&(f2_->i_FI_SUBTRACT))
#line 289
#undef f1
#line 289
#undef f2
#line 289
break;
#line 289

#line 294 "filter/f-inst.c"
 /* Comparison for FI_MULTIPLY */
#line 294
case FI_MULTIPLY:
#line 294
#define f1 (&(f1_->i_FI_MULTIPLY))
#line 294
#define f2 (&(f2_->i_FI_MULTIPLY))
#line 294
#undef f1
#line 294
#undef f2
#line 294
break;
#line 294

#line 300 "filter/f-inst.c"
 /* Comparison for FI_DIVIDE */
#line 300
case FI_DIVIDE:
#line 300
#define f1 (&(f1_->i_FI_DIVIDE))
#line 300
#define f2 (&(f2_->i_FI_DIVIDE))
#line 300
#undef f1
#line 300
#undef f2
#line 300
break;
#line 300

#line 310 "filter/f-inst.c"
 /* Comparison for FI_AND */
#line 310
case FI_AND:
#line 310
#define f1 (&(f1_->i_FI_AND))
#line 310
#define f2 (&(f2_->i_FI_AND))
#line 306 "filter/f-inst.c"
if (!f_same(f1->fl2, f2->fl2)) return 0;
#line 310 "filter/f-inst.c"
#undef f1
#line 310
#undef f2
#line 310
break;
#line 310

#line 321 "filter/f-inst.c"
 /* Comparison for FI_OR */
#line 321
case FI_OR:
#line 321
#define f1 (&(f1_->i_FI_OR))
#line 321
#define f2 (&(f2_->i_FI_OR))
#line 316 "filter/f-inst.c"
if (!f_same(f1->fl2, f2->fl2)) return 0;
#line 321 "filter/f-inst.c"
#undef f1
#line 321
#undef f2
#line 321
break;
#line 321

#line 331 "filter/f-inst.c"
 /* Comparison for FI_PAIR_CONSTRUCT */
#line 331
case FI_PAIR_CONSTRUCT:
#line 331
#define f1 (&(f1_->i_FI_PAIR_CONSTRUCT))
#line 331
#define f2 (&(f2_->i_FI_PAIR_CONSTRUCT))
#line 331
#undef f1
#line 331
#undef f2
#line 331
break;
#line 331

#line 371 "filter/f-inst.c"
 /* Comparison for FI_EC_CONSTRUCT */
#line 371
case FI_EC_CONSTRUCT:
#line 371
#define f1 (&(f1_->i_FI_EC_CONSTRUCT))
#line 371
#define f2 (&(f2_->i_FI_EC_CONSTRUCT))
#line 335 "filter/f-inst.c"
if (f1->ecs != f2->ecs) return 0;
#line 335

#line 335

#line 371 "filter/f-inst.c"
#undef f1
#line 371
#undef f2
#line 371
break;
#line 371

#line 378 "filter/f-inst.c"
 /* Comparison for FI_LC_CONSTRUCT */
#line 378
case FI_LC_CONSTRUCT:
#line 378
#define f1 (&(f1_->i_FI_LC_CONSTRUCT))
#line 378
#define f2 (&(f2_->i_FI_LC_CONSTRUCT))
#line 378
#undef f1
#line 378
#undef f2
#line 378
break;
#line 378

#line 428 "filter/f-inst.c"
 /* Comparison for FI_PATHMASK_CONSTRUCT */
#line 428
case FI_PATHMASK_CONSTRUCT:
#line 428
#define f1 (&(f1_->i_FI_PATHMASK_CONSTRUCT))
#line 428
#define f2 (&(f2_->i_FI_PATHMASK_CONSTRUCT))
#line 379 "filter/f-inst.c"
  if (f1->varcount != f2->varcount) return 0;
#line 428 "filter/f-inst.c"
#undef f1
#line 428
#undef f2
#line 428
break;
#line 428

#line 435 "filter/f-inst.c"
 /* Comparison for FI_NEQ */
#line 435
case FI_NEQ:
#line 435
#define f1 (&(f1_->i_FI_NEQ))
#line 435
#define f2 (&(f2_->i_FI_NEQ))
#line 435
#undef f1
#line 435
#undef f2
#line 435
break;
#line 435

#line 442 "filter/f-inst.c"
 /* Comparison for FI_EQ */
#line 442
case FI_EQ:
#line 442
#define f1 (&(f1_->i_FI_EQ))
#line 442
#define f2 (&(f2_->i_FI_EQ))
#line 442
#undef f1
#line 442
#undef f2
#line 442
break;
#line 442

#line 453 "filter/f-inst.c"
 /* Comparison for FI_LT */
#line 453
case FI_LT:
#line 453
#define f1 (&(f1_->i_FI_LT))
#line 453
#define f2 (&(f2_->i_FI_LT))
#line 453
#undef f1
#line 453
#undef f2
#line 453
break;
#line 453

#line 464 "filter/f-inst.c"
 /* Comparison for FI_LTE */
#line 464
case FI_LTE:
#line 464
#define f1 (&(f1_->i_FI_LTE))
#line 464
#define f2 (&(f2_->i_FI_LTE))
#line 464
#undef f1
#line 464
#undef f2
#line 464
break;
#line 464

#line 469 "filter/f-inst.c"
 /* Comparison for FI_NOT */
#line 469
case FI_NOT:
#line 469
#define f1 (&(f1_->i_FI_NOT))
#line 469
#define f2 (&(f2_->i_FI_NOT))
#line 469
#undef f1
#line 469
#undef f2
#line 469
break;
#line 469

#line 478 "filter/f-inst.c"
 /* Comparison for FI_MATCH */
#line 478
case FI_MATCH:
#line 478
#define f1 (&(f1_->i_FI_MATCH))
#line 478
#define f2 (&(f2_->i_FI_MATCH))
#line 478
#undef f1
#line 478
#undef f2
#line 478
break;
#line 478

#line 487 "filter/f-inst.c"
 /* Comparison for FI_NOT_MATCH */
#line 487
case FI_NOT_MATCH:
#line 487
#define f1 (&(f1_->i_FI_NOT_MATCH))
#line 487
#define f2 (&(f2_->i_FI_NOT_MATCH))
#line 487
#undef f1
#line 487
#undef f2
#line 487
break;
#line 487

#line 492 "filter/f-inst.c"
 /* Comparison for FI_DEFINED */
#line 492
case FI_DEFINED:
#line 492
#define f1 (&(f1_->i_FI_DEFINED))
#line 492
#define f2 (&(f2_->i_FI_DEFINED))
#line 492
#undef f1
#line 492
#undef f2
#line 492
break;
#line 492

#line 504 "filter/f-inst.c"
 /* Comparison for FI_TYPE */
#line 504
case FI_TYPE:
#line 504
#define f1 (&(f1_->i_FI_TYPE))
#line 504
#define f2 (&(f2_->i_FI_TYPE))
#line 504
#undef f1
#line 504
#undef f2
#line 504
break;
#line 504

#line 509 "filter/f-inst.c"
 /* Comparison for FI_IS_V4 */
#line 509
case FI_IS_V4:
#line 509
#define f1 (&(f1_->i_FI_IS_V4))
#line 509
#define f2 (&(f2_->i_FI_IS_V4))
#line 509
#undef f1
#line 509
#undef f2
#line 509
break;
#line 509

#line 522 "filter/f-inst.c"
 /* Comparison for FI_VAR_INIT */
#line 522
case FI_VAR_INIT:
#line 522
#define f1 (&(f1_->i_FI_VAR_INIT))
#line 522
#define f2 (&(f2_->i_FI_VAR_INIT))
#line 512 "filter/f-inst.c"
if (strcmp(f1->sym->name, f2->sym->name) || (f1->sym->class != f2->sym->class)) return 0;
#line 512

#line 512

#line 522 "filter/f-inst.c"
#undef f1
#line 522
#undef f2
#line 522
break;
#line 522

#line 531 "filter/f-inst.c"
 /* Comparison for FI_VAR_SET */
#line 531
case FI_VAR_SET:
#line 531
#define f1 (&(f1_->i_FI_VAR_SET))
#line 531
#define f2 (&(f2_->i_FI_VAR_SET))
#line 525 "filter/f-inst.c"
if (strcmp(f1->sym->name, f2->sym->name) || (f1->sym->class != f2->sym->class)) return 0;
#line 525

#line 525

#line 531 "filter/f-inst.c"
#undef f1
#line 531
#undef f2
#line 531
break;
#line 531

#line 538 "filter/f-inst.c"
 /* Comparison for FI_VAR_GET */
#line 538
case FI_VAR_GET:
#line 538
#define f1 (&(f1_->i_FI_VAR_GET))
#line 538
#define f2 (&(f2_->i_FI_VAR_GET))
#line 532 "filter/f-inst.c"
if (strcmp(f1->sym->name, f2->sym->name) || (f1->sym->class != f2->sym->class)) return 0;
#line 532

#line 532

#line 538 "filter/f-inst.c"
#undef f1
#line 538
#undef f2
#line 538
break;
#line 538

#line 551 "filter/f-inst.c"
 /* Comparison for FI_CONSTANT */
#line 551
case FI_CONSTANT:
#line 551
#define f1 (&(f1_->i_FI_CONSTANT))
#line 551
#define f2 (&(f2_->i_FI_CONSTANT))
#line 539 "filter/f-inst.c"
if ( !val_same(&(f1->val), &(f2->val)) ) return 0;
#line 539

#line 539

#line 551 "filter/f-inst.c"
#undef f1
#line 551
#undef f2
#line 551
break;
#line 551

#line 585 "filter/f-inst.c"
 /* Comparison for FI_FOR_INIT */
#line 585
case FI_FOR_INIT:
#line 585
#define f1 (&(f1_->i_FI_FOR_INIT))
#line 585
#define f2 (&(f2_->i_FI_FOR_INIT))
#line 554 "filter/f-inst.c"
if (strcmp(f1->sym->name, f2->sym->name) || (f1->sym->class != f2->sym->class)) return 0;
#line 554

#line 554

#line 585 "filter/f-inst.c"
#undef f1
#line 585
#undef f2
#line 585
break;
#line 585

#line 639 "filter/f-inst.c"
 /* Comparison for FI_FOR_NEXT */
#line 639
case FI_FOR_NEXT:
#line 639
#define f1 (&(f1_->i_FI_FOR_NEXT))
#line 639
#define f2 (&(f2_->i_FI_FOR_NEXT))
#line 587 "filter/f-inst.c"
if (strcmp(f1->sym->name, f2->sym->name) || (f1->sym->class != f2->sym->class)) return 0;
#line 587

#line 587

#line 630 "filter/f-inst.c"
if (!f_same(f1->fl1, f2->fl1)) return 0;
#line 639 "filter/f-inst.c"
#undef f1
#line 639
#undef f2
#line 639
break;
#line 639

#line 647 "filter/f-inst.c"
 /* Comparison for FI_CONDITION */
#line 647
case FI_CONDITION:
#line 647
#define f1 (&(f1_->i_FI_CONDITION))
#line 647
#define f2 (&(f2_->i_FI_CONDITION))
#line 642 "filter/f-inst.c"
if (!f_same(f1->fl2, f2->fl2)) return 0;
#line 644 "filter/f-inst.c"
if (!f_same(f1->fl3, f2->fl3)) return 0;
#line 647 "filter/f-inst.c"
#undef f1
#line 647
#undef f2
#line 647
break;
#line 647

#line 656 "filter/f-inst.c"
 /* Comparison for FI_PRINT */
#line 656
case FI_PRINT:
#line 656
#define f1 (&(f1_->i_FI_PRINT))
#line 656
#define f2 (&(f2_->i_FI_PRINT))
#line 649 "filter/f-inst.c"
  if (f1->varcount != f2->varcount) return 0;
#line 656 "filter/f-inst.c"
#undef f1
#line 656
#undef f2
#line 656
break;
#line 656

#line 663 "filter/f-inst.c"
 /* Comparison for FI_FLUSH */
#line 663
case FI_FLUSH:
#line 663
#define f1 (&(f1_->i_FI_FLUSH))
#line 663
#define f2 (&(f2_->i_FI_FLUSH))
#line 663
#undef f1
#line 663
#undef f2
#line 663
break;
#line 663

#line 677 "filter/f-inst.c"
 /* Comparison for FI_DIE */
#line 677
case FI_DIE:
#line 677
#define f1 (&(f1_->i_FI_DIE))
#line 677
#define f2 (&(f2_->i_FI_DIE))
#line 665 "filter/f-inst.c"
if (f1->fret != f2->fret) return 0;
#line 665

#line 665

#line 677 "filter/f-inst.c"
#undef f1
#line 677
#undef f2
#line 677
break;
#line 677

#line 705 "filter/f-inst.c"
 /* Comparison for FI_RTA_GET */
#line 705
case FI_RTA_GET:
#line 705
#define f1 (&(f1_->i_FI_RTA_GET))
#line 705
#define f2 (&(f2_->i_FI_RTA_GET))
#line 679 "filter/f-inst.c"
if (f1->sa.sa_code != f2->sa.sa_code) return 0;
#line 679

#line 679

#line 705 "filter/f-inst.c"
#undef f1
#line 705
#undef f2
#line 705
break;
#line 705

#line 820 "filter/f-inst.c"
 /* Comparison for FI_RTA_SET */
#line 820
case FI_RTA_SET:
#line 820
#define f1 (&(f1_->i_FI_RTA_SET))
#line 820
#define f2 (&(f2_->i_FI_RTA_SET))
#line 708 "filter/f-inst.c"
if (f1->sa.sa_code != f2->sa.sa_code) return 0;
#line 708

#line 708

#line 820 "filter/f-inst.c"
#undef f1
#line 820
#undef f2
#line 820
break;
#line 820

#line 892 "filter/f-inst.c"
 /* Comparison for FI_EA_GET */
#line 892
case FI_EA_GET:
#line 892
#define f1 (&(f1_->i_FI_EA_GET))
#line 892
#define f2 (&(f2_->i_FI_EA_GET))
#line 821 "filter/f-inst.c"
if (f1->da.ea_code != f2->da.ea_code) return 0;
#line 821

#line 821

#line 892 "filter/f-inst.c"
#undef f1
#line 892
#undef f2
#line 892
break;
#line 892

#line 959 "filter/f-inst.c"
 /* Comparison for FI_EA_SET */
#line 959
case FI_EA_SET:
#line 959
#define f1 (&(f1_->i_FI_EA_SET))
#line 959
#define f2 (&(f2_->i_FI_EA_SET))
#line 896 "filter/f-inst.c"
if (f1->da.ea_code != f2->da.ea_code) return 0;
#line 896

#line 896

#line 959 "filter/f-inst.c"
#undef f1
#line 959
#undef f2
#line 959
break;
#line 959

#line 968 "filter/f-inst.c"
 /* Comparison for FI_EA_UNSET */
#line 968
case FI_EA_UNSET:
#line 968
#define f1 (&(f1_->i_FI_EA_UNSET))
#line 968
#define f2 (&(f2_->i_FI_EA_UNSET))
#line 960 "filter/f-inst.c"
if (f1->da.ea_code != f2->da.ea_code) return 0;
#line 960

#line 960

#line 968 "filter/f-inst.c"
#undef f1
#line 968
#undef f2
#line 968
break;
#line 968

#line 980 "filter/f-inst.c"
 /* Comparison for FI_LENGTH */
#line 980
case FI_LENGTH:
#line 980
#define f1 (&(f1_->i_FI_LENGTH))
#line 980
#define f2 (&(f2_->i_FI_LENGTH))
#line 980
#undef f1
#line 980
#undef f2
#line 980
break;
#line 980

#line 1015 "filter/f-inst.c"
 /* Comparison for FI_NET_SRC */
#line 1015
case FI_NET_SRC:
#line 1015
#define f1 (&(f1_->i_FI_NET_SRC))
#line 1015
#define f2 (&(f2_->i_FI_NET_SRC))
#line 1015
#undef f1
#line 1015
#undef f2
#line 1015
break;
#line 1015

#line 1050 "filter/f-inst.c"
 /* Comparison for FI_NET_DST */
#line 1050
case FI_NET_DST:
#line 1050
#define f1 (&(f1_->i_FI_NET_DST))
#line 1050
#define f2 (&(f2_->i_FI_NET_DST))
#line 1050
#undef f1
#line 1050
#undef f2
#line 1050
break;
#line 1050

#line 1060 "filter/f-inst.c"
 /* Comparison for FI_ROA_MAXLEN */
#line 1060
case FI_ROA_MAXLEN:
#line 1060
#define f1 (&(f1_->i_FI_ROA_MAXLEN))
#line 1060
#define f2 (&(f2_->i_FI_ROA_MAXLEN))
#line 1060
#undef f1
#line 1060
#undef f2
#line 1060
break;
#line 1060

#line 1087 "filter/f-inst.c"
 /* Comparison for FI_ASN */
#line 1087
case FI_ASN:
#line 1087
#define f1 (&(f1_->i_FI_ASN))
#line 1087
#define f2 (&(f2_->i_FI_ASN))
#line 1087
#undef f1
#line 1087
#undef f2
#line 1087
break;
#line 1087

#line 1092 "filter/f-inst.c"
 /* Comparison for FI_IP */
#line 1092
case FI_IP:
#line 1092
#define f1 (&(f1_->i_FI_IP))
#line 1092
#define f2 (&(f2_->i_FI_IP))
#line 1092
#undef f1
#line 1092
#undef f2
#line 1092
break;
#line 1092

#line 1099 "filter/f-inst.c"
 /* Comparison for FI_ROUTE_DISTINGUISHER */
#line 1099
case FI_ROUTE_DISTINGUISHER:
#line 1099
#define f1 (&(f1_->i_FI_ROUTE_DISTINGUISHER))
#line 1099
#define f2 (&(f2_->i_FI_ROUTE_DISTINGUISHER))
#line 1099
#undef f1
#line 1099
#undef f2
#line 1099
break;
#line 1099

#line 1106 "filter/f-inst.c"
 /* Comparison for FI_AS_PATH_FIRST */
#line 1106
case FI_AS_PATH_FIRST:
#line 1106
#define f1 (&(f1_->i_FI_AS_PATH_FIRST))
#line 1106
#define f2 (&(f2_->i_FI_AS_PATH_FIRST))
#line 1106
#undef f1
#line 1106
#undef f2
#line 1106
break;
#line 1106

#line 1113 "filter/f-inst.c"
 /* Comparison for FI_AS_PATH_LAST */
#line 1113
case FI_AS_PATH_LAST:
#line 1113
#define f1 (&(f1_->i_FI_AS_PATH_LAST))
#line 1113
#define f2 (&(f2_->i_FI_AS_PATH_LAST))
#line 1113
#undef f1
#line 1113
#undef f2
#line 1113
break;
#line 1113

#line 1118 "filter/f-inst.c"
 /* Comparison for FI_AS_PATH_LAST_NAG */
#line 1118
case FI_AS_PATH_LAST_NAG:
#line 1118
#define f1 (&(f1_->i_FI_AS_PATH_LAST_NAG))
#line 1118
#define f2 (&(f2_->i_FI_AS_PATH_LAST_NAG))
#line 1118
#undef f1
#line 1118
#undef f2
#line 1118
break;
#line 1118

#line 1123 "filter/f-inst.c"
 /* Comparison for FI_PAIR_DATA */
#line 1123
case FI_PAIR_DATA:
#line 1123
#define f1 (&(f1_->i_FI_PAIR_DATA))
#line 1123
#define f2 (&(f2_->i_FI_PAIR_DATA))
#line 1123
#undef f1
#line 1123
#undef f2
#line 1123
break;
#line 1123

#line 1128 "filter/f-inst.c"
 /* Comparison for FI_LC_DATA1 */
#line 1128
case FI_LC_DATA1:
#line 1128
#define f1 (&(f1_->i_FI_LC_DATA1))
#line 1128
#define f2 (&(f2_->i_FI_LC_DATA1))
#line 1128
#undef f1
#line 1128
#undef f2
#line 1128
break;
#line 1128

#line 1133 "filter/f-inst.c"
 /* Comparison for FI_LC_DATA2 */
#line 1133
case FI_LC_DATA2:
#line 1133
#define f1 (&(f1_->i_FI_LC_DATA2))
#line 1133
#define f2 (&(f2_->i_FI_LC_DATA2))
#line 1133
#undef f1
#line 1133
#undef f2
#line 1133
break;
#line 1133

#line 1167 "filter/f-inst.c"
 /* Comparison for FI_MIN */
#line 1167
case FI_MIN:
#line 1167
#define f1 (&(f1_->i_FI_MIN))
#line 1167
#define f2 (&(f2_->i_FI_MIN))
#line 1167
#undef f1
#line 1167
#undef f2
#line 1167
break;
#line 1167

#line 1201 "filter/f-inst.c"
 /* Comparison for FI_MAX */
#line 1201
case FI_MAX:
#line 1201
#define f1 (&(f1_->i_FI_MAX))
#line 1201
#define f2 (&(f2_->i_FI_MAX))
#line 1201
#undef f1
#line 1201
#undef f2
#line 1201
break;
#line 1201

#line 1227 "filter/f-inst.c"
 /* Comparison for FI_RETURN */
#line 1227
case FI_RETURN:
#line 1227
#define f1 (&(f1_->i_FI_RETURN))
#line 1227
#define f2 (&(f2_->i_FI_RETURN))
#line 1227
#undef f1
#line 1227
#undef f2
#line 1227
break;
#line 1227

#line 1288 "filter/f-inst.c"
 /* Comparison for FI_CALL */
#line 1288
case FI_CALL:
#line 1288
#define f1 (&(f1_->i_FI_CALL))
#line 1288
#define f2 (&(f2_->i_FI_CALL))
#line 1229 "filter/f-inst.c"
  if (f1->varcount != f2->varcount) return 0;
#line 1230 "filter/f-inst.c"
if (strcmp(f1->sym->name, f2->sym->name) || (f1->sym->class != f2->sym->class)) return 0;
#line 1230

#line 1230


#line 1266 "filter/f-inst.c"
    if (!(f1->sym->flags & SYM_FLAG_SAME) && !(f1_->flags & FIF_RECURSIVE))
      return 0;

    #line 1288 "filter/f-inst.c"
#undef f1
#line 1288
#undef f2
#line 1288
break;
#line 1288

#line 1293 "filter/f-inst.c"
 /* Comparison for FI_DROP_RESULT */
#line 1293
case FI_DROP_RESULT:
#line 1293
#define f1 (&(f1_->i_FI_DROP_RESULT))
#line 1293
#define f2 (&(f2_->i_FI_DROP_RESULT))
#line 1293
#undef f1
#line 1293
#undef f2
#line 1293
break;
#line 1293

#line 1334 "filter/f-inst.c"
 /* Comparison for FI_SWITCH */
#line 1334
case FI_SWITCH:
#line 1334
#define f1 (&(f1_->i_FI_SWITCH))
#line 1334
#define f2 (&(f2_->i_FI_SWITCH))
#line 1296 "filter/f-inst.c"
if (!same_tree(f1->tree, f2->tree)) return 0;
#line 1296

#line 1296

#line 1334 "filter/f-inst.c"
#undef f1
#line 1334
#undef f2
#line 1334
break;
#line 1334

#line 1342 "filter/f-inst.c"
 /* Comparison for FI_IP_MASK */
#line 1342
case FI_IP_MASK:
#line 1342
#define f1 (&(f1_->i_FI_IP_MASK))
#line 1342
#define f2 (&(f2_->i_FI_IP_MASK))
#line 1342
#undef f1
#line 1342
#undef f2
#line 1342
break;
#line 1342

#line 1348 "filter/f-inst.c"
 /* Comparison for FI_PATH_PREPEND */
#line 1348
case FI_PATH_PREPEND:
#line 1348
#define f1 (&(f1_->i_FI_PATH_PREPEND))
#line 1348
#define f2 (&(f2_->i_FI_PATH_PREPEND))
#line 1348
#undef f1
#line 1348
#undef f2
#line 1348
break;
#line 1348

#line 1405 "filter/f-inst.c"
 /* Comparison for FI_CLIST_ADD */
#line 1405
case FI_CLIST_ADD:
#line 1405
#define f1 (&(f1_->i_FI_CLIST_ADD))
#line 1405
#define f2 (&(f2_->i_FI_CLIST_ADD))
#line 1405
#undef f1
#line 1405
#undef f2
#line 1405
break;
#line 1405

#line 1460 "filter/f-inst.c"
 /* Comparison for FI_CLIST_DEL */
#line 1460
case FI_CLIST_DEL:
#line 1460
#define f1 (&(f1_->i_FI_CLIST_DEL))
#line 1460
#define f2 (&(f2_->i_FI_CLIST_DEL))
#line 1460
#undef f1
#line 1460
#undef f2
#line 1460
break;
#line 1460

#line 1506 "filter/f-inst.c"
 /* Comparison for FI_CLIST_FILTER */
#line 1506
case FI_CLIST_FILTER:
#line 1506
#define f1 (&(f1_->i_FI_CLIST_FILTER))
#line 1506
#define f2 (&(f2_->i_FI_CLIST_FILTER))
#line 1506
#undef f1
#line 1506
#undef f2
#line 1506
break;
#line 1506

#line 1536 "filter/f-inst.c"
 /* Comparison for FI_ROA_CHECK_IMPLICIT */
#line 1536
case FI_ROA_CHECK_IMPLICIT:
#line 1536
#define f1 (&(f1_->i_FI_ROA_CHECK_IMPLICIT))
#line 1536
#define f2 (&(f2_->i_FI_ROA_CHECK_IMPLICIT))
#line 1508 "filter/f-inst.c"
if (strcmp(f1->rtc->name, f2->rtc->name)) return 0;
#line 1508

#line 1508

#line 1536 "filter/f-inst.c"
#undef f1
#line 1536
#undef f2
#line 1536
break;
#line 1536

#line 1558 "filter/f-inst.c"
 /* Comparison for FI_ROA_CHECK_EXPLICIT */
#line 1558
case FI_ROA_CHECK_EXPLICIT:
#line 1558
#define f1 (&(f1_->i_FI_ROA_CHECK_EXPLICIT))
#line 1558
#define f2 (&(f2_->i_FI_ROA_CHECK_EXPLICIT))
#line 1540 "filter/f-inst.c"
if (strcmp(f1->rtc->name, f2->rtc->name)) return 0;
#line 1540

#line 1540

#line 1558 "filter/f-inst.c"
#undef f1
#line 1558
#undef f2
#line 1558
break;
#line 1558

#line 1563 "filter/f-inst.c"
 /* Comparison for FI_FORMAT */
#line 1563
case FI_FORMAT:
#line 1563
#define f1 (&(f1_->i_FI_FORMAT))
#line 1563
#define f2 (&(f2_->i_FI_FORMAT))
#line 1563
#undef f1
#line 1563
#undef f2
#line 1563
break;
#line 1563

#line 696 "filter/decl.m4"
 /* Comparison for FI_ASSERT */
#line 696
case FI_ASSERT:
#line 696
#define f1 (&(f1_->i_FI_ASSERT))
#line 696
#define f2 (&(f2_->i_FI_ASSERT))
#line 1567 "filter/f-inst.c"
if (strcmp(f1->s, f2->s)) return 0;
#line 1567

#line 1567

#line 696 "filter/decl.m4"
#undef f1
#line 696
#undef f2
#line 696
break;
#line 696

#line 617 "filter/decl.m4"

    }
  }
#undef f1_
#undef f2_
  return 1;
}


/* Part of FI_SWITCH filter iterator */
static void
f_add_tree_lines(const struct f_tree *t, void *fit_)
{
  struct filter_iterator * fit = fit_;

  if (t->data)
    BUFFER_PUSH(fit->lines) = t->data;
}

/* Filter line iterator */
void
f_add_lines(const struct f_line_item *what, struct filter_iterator *fit)
{
  switch(what->fi_code) {
#line 284 "filter/f-inst.c"
 /* Iteration for FI_ADD */
#line 284
case FI_ADD:
#line 284
#define whati (&(what->i_FI_ADD))
#line 284
#undef whati
#line 284
break;
#line 284

#line 289 "filter/f-inst.c"
 /* Iteration for FI_SUBTRACT */
#line 289
case FI_SUBTRACT:
#line 289
#define whati (&(what->i_FI_SUBTRACT))
#line 289
#undef whati
#line 289
break;
#line 289

#line 294 "filter/f-inst.c"
 /* Iteration for FI_MULTIPLY */
#line 294
case FI_MULTIPLY:
#line 294
#define whati (&(what->i_FI_MULTIPLY))
#line 294
#undef whati
#line 294
break;
#line 294

#line 300 "filter/f-inst.c"
 /* Iteration for FI_DIVIDE */
#line 300
case FI_DIVIDE:
#line 300
#define whati (&(what->i_FI_DIVIDE))
#line 300
#undef whati
#line 300
break;
#line 300

#line 310 "filter/f-inst.c"
 /* Iteration for FI_AND */
#line 310
case FI_AND:
#line 310
#define whati (&(what->i_FI_AND))
#line 306 "filter/f-inst.c"
if (whati->fl2) BUFFER_PUSH(fit->lines) = whati->fl2;
#line 310 "filter/f-inst.c"
#undef whati
#line 310
break;
#line 310

#line 321 "filter/f-inst.c"
 /* Iteration for FI_OR */
#line 321
case FI_OR:
#line 321
#define whati (&(what->i_FI_OR))
#line 316 "filter/f-inst.c"
if (whati->fl2) BUFFER_PUSH(fit->lines) = whati->fl2;
#line 321 "filter/f-inst.c"
#undef whati
#line 321
break;
#line 321

#line 331 "filter/f-inst.c"
 /* Iteration for FI_PAIR_CONSTRUCT */
#line 331
case FI_PAIR_CONSTRUCT:
#line 331
#define whati (&(what->i_FI_PAIR_CONSTRUCT))
#line 331
#undef whati
#line 331
break;
#line 331

#line 371 "filter/f-inst.c"
 /* Iteration for FI_EC_CONSTRUCT */
#line 371
case FI_EC_CONSTRUCT:
#line 371
#define whati (&(what->i_FI_EC_CONSTRUCT))
#line 371
#undef whati
#line 371
break;
#line 371

#line 378 "filter/f-inst.c"
 /* Iteration for FI_LC_CONSTRUCT */
#line 378
case FI_LC_CONSTRUCT:
#line 378
#define whati (&(what->i_FI_LC_CONSTRUCT))
#line 378
#undef whati
#line 378
break;
#line 378

#line 428 "filter/f-inst.c"
 /* Iteration for FI_PATHMASK_CONSTRUCT */
#line 428
case FI_PATHMASK_CONSTRUCT:
#line 428
#define whati (&(what->i_FI_PATHMASK_CONSTRUCT))
#line 428
#undef whati
#line 428
break;
#line 428

#line 435 "filter/f-inst.c"
 /* Iteration for FI_NEQ */
#line 435
case FI_NEQ:
#line 435
#define whati (&(what->i_FI_NEQ))
#line 435
#undef whati
#line 435
break;
#line 435

#line 442 "filter/f-inst.c"
 /* Iteration for FI_EQ */
#line 442
case FI_EQ:
#line 442
#define whati (&(what->i_FI_EQ))
#line 442
#undef whati
#line 442
break;
#line 442

#line 453 "filter/f-inst.c"
 /* Iteration for FI_LT */
#line 453
case FI_LT:
#line 453
#define whati (&(what->i_FI_LT))
#line 453
#undef whati
#line 453
break;
#line 453

#line 464 "filter/f-inst.c"
 /* Iteration for FI_LTE */
#line 464
case FI_LTE:
#line 464
#define whati (&(what->i_FI_LTE))
#line 464
#undef whati
#line 464
break;
#line 464

#line 469 "filter/f-inst.c"
 /* Iteration for FI_NOT */
#line 469
case FI_NOT:
#line 469
#define whati (&(what->i_FI_NOT))
#line 469
#undef whati
#line 469
break;
#line 469

#line 478 "filter/f-inst.c"
 /* Iteration for FI_MATCH */
#line 478
case FI_MATCH:
#line 478
#define whati (&(what->i_FI_MATCH))
#line 478
#undef whati
#line 478
break;
#line 478

#line 487 "filter/f-inst.c"
 /* Iteration for FI_NOT_MATCH */
#line 487
case FI_NOT_MATCH:
#line 487
#define whati (&(what->i_FI_NOT_MATCH))
#line 487
#undef whati
#line 487
break;
#line 487

#line 492 "filter/f-inst.c"
 /* Iteration for FI_DEFINED */
#line 492
case FI_DEFINED:
#line 492
#define whati (&(what->i_FI_DEFINED))
#line 492
#undef whati
#line 492
break;
#line 492

#line 504 "filter/f-inst.c"
 /* Iteration for FI_TYPE */
#line 504
case FI_TYPE:
#line 504
#define whati (&(what->i_FI_TYPE))
#line 504
#undef whati
#line 504
break;
#line 504

#line 509 "filter/f-inst.c"
 /* Iteration for FI_IS_V4 */
#line 509
case FI_IS_V4:
#line 509
#define whati (&(what->i_FI_IS_V4))
#line 509
#undef whati
#line 509
break;
#line 509

#line 522 "filter/f-inst.c"
 /* Iteration for FI_VAR_INIT */
#line 522
case FI_VAR_INIT:
#line 522
#define whati (&(what->i_FI_VAR_INIT))
#line 522
#undef whati
#line 522
break;
#line 522

#line 531 "filter/f-inst.c"
 /* Iteration for FI_VAR_SET */
#line 531
case FI_VAR_SET:
#line 531
#define whati (&(what->i_FI_VAR_SET))
#line 531
#undef whati
#line 531
break;
#line 531

#line 538 "filter/f-inst.c"
 /* Iteration for FI_VAR_GET */
#line 538
case FI_VAR_GET:
#line 538
#define whati (&(what->i_FI_VAR_GET))
#line 538
#undef whati
#line 538
break;
#line 538

#line 551 "filter/f-inst.c"
 /* Iteration for FI_CONSTANT */
#line 551
case FI_CONSTANT:
#line 551
#define whati (&(what->i_FI_CONSTANT))
#line 551
#undef whati
#line 551
break;
#line 551

#line 585 "filter/f-inst.c"
 /* Iteration for FI_FOR_INIT */
#line 585
case FI_FOR_INIT:
#line 585
#define whati (&(what->i_FI_FOR_INIT))
#line 585
#undef whati
#line 585
break;
#line 585

#line 639 "filter/f-inst.c"
 /* Iteration for FI_FOR_NEXT */
#line 639
case FI_FOR_NEXT:
#line 639
#define whati (&(what->i_FI_FOR_NEXT))
#line 630 "filter/f-inst.c"
if (whati->fl1) BUFFER_PUSH(fit->lines) = whati->fl1;
#line 639 "filter/f-inst.c"
#undef whati
#line 639
break;
#line 639

#line 647 "filter/f-inst.c"
 /* Iteration for FI_CONDITION */
#line 647
case FI_CONDITION:
#line 647
#define whati (&(what->i_FI_CONDITION))
#line 642 "filter/f-inst.c"
if (whati->fl2) BUFFER_PUSH(fit->lines) = whati->fl2;
#line 644 "filter/f-inst.c"
if (whati->fl3) BUFFER_PUSH(fit->lines) = whati->fl3;
#line 647 "filter/f-inst.c"
#undef whati
#line 647
break;
#line 647

#line 656 "filter/f-inst.c"
 /* Iteration for FI_PRINT */
#line 656
case FI_PRINT:
#line 656
#define whati (&(what->i_FI_PRINT))
#line 656
#undef whati
#line 656
break;
#line 656

#line 663 "filter/f-inst.c"
 /* Iteration for FI_FLUSH */
#line 663
case FI_FLUSH:
#line 663
#define whati (&(what->i_FI_FLUSH))
#line 663
#undef whati
#line 663
break;
#line 663

#line 677 "filter/f-inst.c"
 /* Iteration for FI_DIE */
#line 677
case FI_DIE:
#line 677
#define whati (&(what->i_FI_DIE))
#line 677
#undef whati
#line 677
break;
#line 677

#line 705 "filter/f-inst.c"
 /* Iteration for FI_RTA_GET */
#line 705
case FI_RTA_GET:
#line 705
#define whati (&(what->i_FI_RTA_GET))
#line 705
#undef whati
#line 705
break;
#line 705

#line 820 "filter/f-inst.c"
 /* Iteration for FI_RTA_SET */
#line 820
case FI_RTA_SET:
#line 820
#define whati (&(what->i_FI_RTA_SET))
#line 820
#undef whati
#line 820
break;
#line 820

#line 892 "filter/f-inst.c"
 /* Iteration for FI_EA_GET */
#line 892
case FI_EA_GET:
#line 892
#define whati (&(what->i_FI_EA_GET))
#line 892
#undef whati
#line 892
break;
#line 892

#line 959 "filter/f-inst.c"
 /* Iteration for FI_EA_SET */
#line 959
case FI_EA_SET:
#line 959
#define whati (&(what->i_FI_EA_SET))
#line 959
#undef whati
#line 959
break;
#line 959

#line 968 "filter/f-inst.c"
 /* Iteration for FI_EA_UNSET */
#line 968
case FI_EA_UNSET:
#line 968
#define whati (&(what->i_FI_EA_UNSET))
#line 968
#undef whati
#line 968
break;
#line 968

#line 980 "filter/f-inst.c"
 /* Iteration for FI_LENGTH */
#line 980
case FI_LENGTH:
#line 980
#define whati (&(what->i_FI_LENGTH))
#line 980
#undef whati
#line 980
break;
#line 980

#line 1015 "filter/f-inst.c"
 /* Iteration for FI_NET_SRC */
#line 1015
case FI_NET_SRC:
#line 1015
#define whati (&(what->i_FI_NET_SRC))
#line 1015
#undef whati
#line 1015
break;
#line 1015

#line 1050 "filter/f-inst.c"
 /* Iteration for FI_NET_DST */
#line 1050
case FI_NET_DST:
#line 1050
#define whati (&(what->i_FI_NET_DST))
#line 1050
#undef whati
#line 1050
break;
#line 1050

#line 1060 "filter/f-inst.c"
 /* Iteration for FI_ROA_MAXLEN */
#line 1060
case FI_ROA_MAXLEN:
#line 1060
#define whati (&(what->i_FI_ROA_MAXLEN))
#line 1060
#undef whati
#line 1060
break;
#line 1060

#line 1087 "filter/f-inst.c"
 /* Iteration for FI_ASN */
#line 1087
case FI_ASN:
#line 1087
#define whati (&(what->i_FI_ASN))
#line 1087
#undef whati
#line 1087
break;
#line 1087

#line 1092 "filter/f-inst.c"
 /* Iteration for FI_IP */
#line 1092
case FI_IP:
#line 1092
#define whati (&(what->i_FI_IP))
#line 1092
#undef whati
#line 1092
break;
#line 1092

#line 1099 "filter/f-inst.c"
 /* Iteration for FI_ROUTE_DISTINGUISHER */
#line 1099
case FI_ROUTE_DISTINGUISHER:
#line 1099
#define whati (&(what->i_FI_ROUTE_DISTINGUISHER))
#line 1099
#undef whati
#line 1099
break;
#line 1099

#line 1106 "filter/f-inst.c"
 /* Iteration for FI_AS_PATH_FIRST */
#line 1106
case FI_AS_PATH_FIRST:
#line 1106
#define whati (&(what->i_FI_AS_PATH_FIRST))
#line 1106
#undef whati
#line 1106
break;
#line 1106

#line 1113 "filter/f-inst.c"
 /* Iteration for FI_AS_PATH_LAST */
#line 1113
case FI_AS_PATH_LAST:
#line 1113
#define whati (&(what->i_FI_AS_PATH_LAST))
#line 1113
#undef whati
#line 1113
break;
#line 1113

#line 1118 "filter/f-inst.c"
 /* Iteration for FI_AS_PATH_LAST_NAG */
#line 1118
case FI_AS_PATH_LAST_NAG:
#line 1118
#define whati (&(what->i_FI_AS_PATH_LAST_NAG))
#line 1118
#undef whati
#line 1118
break;
#line 1118

#line 1123 "filter/f-inst.c"
 /* Iteration for FI_PAIR_DATA */
#line 1123
case FI_PAIR_DATA:
#line 1123
#define whati (&(what->i_FI_PAIR_DATA))
#line 1123
#undef whati
#line 1123
break;
#line 1123

#line 1128 "filter/f-inst.c"
 /* Iteration for FI_LC_DATA1 */
#line 1128
case FI_LC_DATA1:
#line 1128
#define whati (&(what->i_FI_LC_DATA1))
#line 1128
#undef whati
#line 1128
break;
#line 1128

#line 1133 "filter/f-inst.c"
 /* Iteration for FI_LC_DATA2 */
#line 1133
case FI_LC_DATA2:
#line 1133
#define whati (&(what->i_FI_LC_DATA2))
#line 1133
#undef whati
#line 1133
break;
#line 1133

#line 1167 "filter/f-inst.c"
 /* Iteration for FI_MIN */
#line 1167
case FI_MIN:
#line 1167
#define whati (&(what->i_FI_MIN))
#line 1167
#undef whati
#line 1167
break;
#line 1167

#line 1201 "filter/f-inst.c"
 /* Iteration for FI_MAX */
#line 1201
case FI_MAX:
#line 1201
#define whati (&(what->i_FI_MAX))
#line 1201
#undef whati
#line 1201
break;
#line 1201

#line 1227 "filter/f-inst.c"
 /* Iteration for FI_RETURN */
#line 1227
case FI_RETURN:
#line 1227
#define whati (&(what->i_FI_RETURN))
#line 1227
#undef whati
#line 1227
break;
#line 1227

#line 1288 "filter/f-inst.c"
 /* Iteration for FI_CALL */
#line 1288
case FI_CALL:
#line 1288
#define whati (&(what->i_FI_CALL))

#line 1270 "filter/f-inst.c"
    if (!(what->flags & FIF_RECURSIVE))
      BUFFER_PUSH(fit->lines) = whati->sym->function;

    #line 1288 "filter/f-inst.c"
#undef whati
#line 1288
break;
#line 1288

#line 1293 "filter/f-inst.c"
 /* Iteration for FI_DROP_RESULT */
#line 1293
case FI_DROP_RESULT:
#line 1293
#define whati (&(what->i_FI_DROP_RESULT))
#line 1293
#undef whati
#line 1293
break;
#line 1293

#line 1334 "filter/f-inst.c"
 /* Iteration for FI_SWITCH */
#line 1334
case FI_SWITCH:
#line 1334
#define whati (&(what->i_FI_SWITCH))

#line 1317 "filter/f-inst.c"
    tree_walk(whati->tree, f_add_tree_lines, fit);

    #line 1334 "filter/f-inst.c"
#undef whati
#line 1334
break;
#line 1334

#line 1342 "filter/f-inst.c"
 /* Iteration for FI_IP_MASK */
#line 1342
case FI_IP_MASK:
#line 1342
#define whati (&(what->i_FI_IP_MASK))
#line 1342
#undef whati
#line 1342
break;
#line 1342

#line 1348 "filter/f-inst.c"
 /* Iteration for FI_PATH_PREPEND */
#line 1348
case FI_PATH_PREPEND:
#line 1348
#define whati (&(what->i_FI_PATH_PREPEND))
#line 1348
#undef whati
#line 1348
break;
#line 1348

#line 1405 "filter/f-inst.c"
 /* Iteration for FI_CLIST_ADD */
#line 1405
case FI_CLIST_ADD:
#line 1405
#define whati (&(what->i_FI_CLIST_ADD))
#line 1405
#undef whati
#line 1405
break;
#line 1405

#line 1460 "filter/f-inst.c"
 /* Iteration for FI_CLIST_DEL */
#line 1460
case FI_CLIST_DEL:
#line 1460
#define whati (&(what->i_FI_CLIST_DEL))
#line 1460
#undef whati
#line 1460
break;
#line 1460

#line 1506 "filter/f-inst.c"
 /* Iteration for FI_CLIST_FILTER */
#line 1506
case FI_CLIST_FILTER:
#line 1506
#define whati (&(what->i_FI_CLIST_FILTER))
#line 1506
#undef whati
#line 1506
break;
#line 1506

#line 1536 "filter/f-inst.c"
 /* Iteration for FI_ROA_CHECK_IMPLICIT */
#line 1536
case FI_ROA_CHECK_IMPLICIT:
#line 1536
#define whati (&(what->i_FI_ROA_CHECK_IMPLICIT))
#line 1536
#undef whati
#line 1536
break;
#line 1536

#line 1558 "filter/f-inst.c"
 /* Iteration for FI_ROA_CHECK_EXPLICIT */
#line 1558
case FI_ROA_CHECK_EXPLICIT:
#line 1558
#define whati (&(what->i_FI_ROA_CHECK_EXPLICIT))
#line 1558
#undef whati
#line 1558
break;
#line 1558

#line 1563 "filter/f-inst.c"
 /* Iteration for FI_FORMAT */
#line 1563
case FI_FORMAT:
#line 1563
#define whati (&(what->i_FI_FORMAT))
#line 1563
#undef whati
#line 1563
break;
#line 1563

#line 696 "filter/decl.m4"
 /* Iteration for FI_ASSERT */
#line 696
case FI_ASSERT:
#line 696
#define whati (&(what->i_FI_ASSERT))
#line 696
#undef whati
#line 696
break;
#line 696

#line 641 "filter/decl.m4"

  }
}


#if defined(__GNUC__) && __GNUC__ >= 6
#pragma GCC diagnostic pop
#endif

