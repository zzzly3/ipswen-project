#line 650 "filter/decl.m4"

/* Filter instruction codes */
enum f_instruction_code {
#line 284 "filter/f-inst.c"
 /* Code enum for FI_ADD */
#line 284
  FI_ADD,
#line 289 "filter/f-inst.c"
 /* Code enum for FI_SUBTRACT */
#line 289
  FI_SUBTRACT,
#line 294 "filter/f-inst.c"
 /* Code enum for FI_MULTIPLY */
#line 294
  FI_MULTIPLY,
#line 300 "filter/f-inst.c"
 /* Code enum for FI_DIVIDE */
#line 300
  FI_DIVIDE,
#line 310 "filter/f-inst.c"
 /* Code enum for FI_AND */
#line 310
  FI_AND,
#line 321 "filter/f-inst.c"
 /* Code enum for FI_OR */
#line 321
  FI_OR,
#line 331 "filter/f-inst.c"
 /* Code enum for FI_PAIR_CONSTRUCT */
#line 331
  FI_PAIR_CONSTRUCT,
#line 371 "filter/f-inst.c"
 /* Code enum for FI_EC_CONSTRUCT */
#line 371
  FI_EC_CONSTRUCT,
#line 378 "filter/f-inst.c"
 /* Code enum for FI_LC_CONSTRUCT */
#line 378
  FI_LC_CONSTRUCT,
#line 428 "filter/f-inst.c"
 /* Code enum for FI_PATHMASK_CONSTRUCT */
#line 428
  FI_PATHMASK_CONSTRUCT,
#line 435 "filter/f-inst.c"
 /* Code enum for FI_NEQ */
#line 435
  FI_NEQ,
#line 442 "filter/f-inst.c"
 /* Code enum for FI_EQ */
#line 442
  FI_EQ,
#line 453 "filter/f-inst.c"
 /* Code enum for FI_LT */
#line 453
  FI_LT,
#line 464 "filter/f-inst.c"
 /* Code enum for FI_LTE */
#line 464
  FI_LTE,
#line 469 "filter/f-inst.c"
 /* Code enum for FI_NOT */
#line 469
  FI_NOT,
#line 478 "filter/f-inst.c"
 /* Code enum for FI_MATCH */
#line 478
  FI_MATCH,
#line 487 "filter/f-inst.c"
 /* Code enum for FI_NOT_MATCH */
#line 487
  FI_NOT_MATCH,
#line 492 "filter/f-inst.c"
 /* Code enum for FI_DEFINED */
#line 492
  FI_DEFINED,
#line 504 "filter/f-inst.c"
 /* Code enum for FI_TYPE */
#line 504
  FI_TYPE,
#line 509 "filter/f-inst.c"
 /* Code enum for FI_IS_V4 */
#line 509
  FI_IS_V4,
#line 522 "filter/f-inst.c"
 /* Code enum for FI_VAR_INIT */
#line 522
  FI_VAR_INIT,
#line 531 "filter/f-inst.c"
 /* Code enum for FI_VAR_SET */
#line 531
  FI_VAR_SET,
#line 538 "filter/f-inst.c"
 /* Code enum for FI_VAR_GET */
#line 538
  FI_VAR_GET,
#line 551 "filter/f-inst.c"
 /* Code enum for FI_CONSTANT */
#line 551
  FI_CONSTANT,
#line 585 "filter/f-inst.c"
 /* Code enum for FI_FOR_INIT */
#line 585
  FI_FOR_INIT,
#line 639 "filter/f-inst.c"
 /* Code enum for FI_FOR_NEXT */
#line 639
  FI_FOR_NEXT,
#line 647 "filter/f-inst.c"
 /* Code enum for FI_CONDITION */
#line 647
  FI_CONDITION,
#line 656 "filter/f-inst.c"
 /* Code enum for FI_PRINT */
#line 656
  FI_PRINT,
#line 663 "filter/f-inst.c"
 /* Code enum for FI_FLUSH */
#line 663
  FI_FLUSH,
#line 677 "filter/f-inst.c"
 /* Code enum for FI_DIE */
#line 677
  FI_DIE,
#line 705 "filter/f-inst.c"
 /* Code enum for FI_RTA_GET */
#line 705
  FI_RTA_GET,
#line 820 "filter/f-inst.c"
 /* Code enum for FI_RTA_SET */
#line 820
  FI_RTA_SET,
#line 892 "filter/f-inst.c"
 /* Code enum for FI_EA_GET */
#line 892
  FI_EA_GET,
#line 959 "filter/f-inst.c"
 /* Code enum for FI_EA_SET */
#line 959
  FI_EA_SET,
#line 968 "filter/f-inst.c"
 /* Code enum for FI_EA_UNSET */
#line 968
  FI_EA_UNSET,
#line 980 "filter/f-inst.c"
 /* Code enum for FI_LENGTH */
#line 980
  FI_LENGTH,
#line 1015 "filter/f-inst.c"
 /* Code enum for FI_NET_SRC */
#line 1015
  FI_NET_SRC,
#line 1050 "filter/f-inst.c"
 /* Code enum for FI_NET_DST */
#line 1050
  FI_NET_DST,
#line 1060 "filter/f-inst.c"
 /* Code enum for FI_ROA_MAXLEN */
#line 1060
  FI_ROA_MAXLEN,
#line 1087 "filter/f-inst.c"
 /* Code enum for FI_ASN */
#line 1087
  FI_ASN,
#line 1092 "filter/f-inst.c"
 /* Code enum for FI_IP */
#line 1092
  FI_IP,
#line 1099 "filter/f-inst.c"
 /* Code enum for FI_ROUTE_DISTINGUISHER */
#line 1099
  FI_ROUTE_DISTINGUISHER,
#line 1106 "filter/f-inst.c"
 /* Code enum for FI_AS_PATH_FIRST */
#line 1106
  FI_AS_PATH_FIRST,
#line 1113 "filter/f-inst.c"
 /* Code enum for FI_AS_PATH_LAST */
#line 1113
  FI_AS_PATH_LAST,
#line 1118 "filter/f-inst.c"
 /* Code enum for FI_AS_PATH_LAST_NAG */
#line 1118
  FI_AS_PATH_LAST_NAG,
#line 1123 "filter/f-inst.c"
 /* Code enum for FI_PAIR_DATA */
#line 1123
  FI_PAIR_DATA,
#line 1128 "filter/f-inst.c"
 /* Code enum for FI_LC_DATA1 */
#line 1128
  FI_LC_DATA1,
#line 1133 "filter/f-inst.c"
 /* Code enum for FI_LC_DATA2 */
#line 1133
  FI_LC_DATA2,
#line 1167 "filter/f-inst.c"
 /* Code enum for FI_MIN */
#line 1167
  FI_MIN,
#line 1201 "filter/f-inst.c"
 /* Code enum for FI_MAX */
#line 1201
  FI_MAX,
#line 1227 "filter/f-inst.c"
 /* Code enum for FI_RETURN */
#line 1227
  FI_RETURN,
#line 1288 "filter/f-inst.c"
 /* Code enum for FI_CALL */
#line 1288
  FI_CALL,
#line 1293 "filter/f-inst.c"
 /* Code enum for FI_DROP_RESULT */
#line 1293
  FI_DROP_RESULT,
#line 1334 "filter/f-inst.c"
 /* Code enum for FI_SWITCH */
#line 1334
  FI_SWITCH,
#line 1342 "filter/f-inst.c"
 /* Code enum for FI_IP_MASK */
#line 1342
  FI_IP_MASK,
#line 1348 "filter/f-inst.c"
 /* Code enum for FI_PATH_PREPEND */
#line 1348
  FI_PATH_PREPEND,
#line 1405 "filter/f-inst.c"
 /* Code enum for FI_CLIST_ADD */
#line 1405
  FI_CLIST_ADD,
#line 1460 "filter/f-inst.c"
 /* Code enum for FI_CLIST_DEL */
#line 1460
  FI_CLIST_DEL,
#line 1506 "filter/f-inst.c"
 /* Code enum for FI_CLIST_FILTER */
#line 1506
  FI_CLIST_FILTER,
#line 1536 "filter/f-inst.c"
 /* Code enum for FI_ROA_CHECK_IMPLICIT */
#line 1536
  FI_ROA_CHECK_IMPLICIT,
#line 1558 "filter/f-inst.c"
 /* Code enum for FI_ROA_CHECK_EXPLICIT */
#line 1558
  FI_ROA_CHECK_EXPLICIT,
#line 1563 "filter/f-inst.c"
 /* Code enum for FI_FORMAT */
#line 1563
  FI_FORMAT,
#line 696 "filter/decl.m4"
 /* Code enum for FI_ASSERT */
#line 696
  FI_ASSERT,
#line 654 "filter/decl.m4"
} PACKED;

/* Filter instruction structure for config */
struct f_inst {
  struct f_inst *next;			/* Next instruction */
  enum f_instruction_code fi_code;	/* Instruction code */
  enum f_instruction_flags flags;	/* Flags, instruction-specific */
  enum f_type type;			/* Type of returned value, if known */
  int size;				/* How many instructions are underneath */
  int lineno;				/* Line number */
  union {
#line 284 "filter/f-inst.c"
 /* Instruction structure for config for FI_ADD */
#line 284
    struct {
#line 280 "filter/f-inst.c"
      struct f_inst * f1;
#line 281 "filter/f-inst.c"
      struct f_inst * f2;
#line 284 "filter/f-inst.c"
    } i_FI_ADD;
#line 289 "filter/f-inst.c"
 /* Instruction structure for config for FI_SUBTRACT */
#line 289
    struct {
#line 285 "filter/f-inst.c"
      struct f_inst * f1;
#line 286 "filter/f-inst.c"
      struct f_inst * f2;
#line 289 "filter/f-inst.c"
    } i_FI_SUBTRACT;
#line 294 "filter/f-inst.c"
 /* Instruction structure for config for FI_MULTIPLY */
#line 294
    struct {
#line 290 "filter/f-inst.c"
      struct f_inst * f1;
#line 291 "filter/f-inst.c"
      struct f_inst * f2;
#line 294 "filter/f-inst.c"
    } i_FI_MULTIPLY;
#line 300 "filter/f-inst.c"
 /* Instruction structure for config for FI_DIVIDE */
#line 300
    struct {
#line 295 "filter/f-inst.c"
      struct f_inst * f1;
#line 296 "filter/f-inst.c"
      struct f_inst * f2;
#line 300 "filter/f-inst.c"
    } i_FI_DIVIDE;
#line 310 "filter/f-inst.c"
 /* Instruction structure for config for FI_AND */
#line 310
    struct {
#line 301 "filter/f-inst.c"
      struct f_inst * f1;
#line 306 "filter/f-inst.c"
      struct f_inst * f2;
#line 310 "filter/f-inst.c"
    } i_FI_AND;
#line 321 "filter/f-inst.c"
 /* Instruction structure for config for FI_OR */
#line 321
    struct {
#line 311 "filter/f-inst.c"
      struct f_inst * f1;
#line 316 "filter/f-inst.c"
      struct f_inst * f2;
#line 321 "filter/f-inst.c"
    } i_FI_OR;
#line 331 "filter/f-inst.c"
 /* Instruction structure for config for FI_PAIR_CONSTRUCT */
#line 331
    struct {
#line 322 "filter/f-inst.c"
      struct f_inst * f1;
#line 323 "filter/f-inst.c"
      struct f_inst * f2;
#line 331 "filter/f-inst.c"
    } i_FI_PAIR_CONSTRUCT;
#line 371 "filter/f-inst.c"
 /* Instruction structure for config for FI_EC_CONSTRUCT */
#line 371
    struct {
#line 332 "filter/f-inst.c"
      struct f_inst * f1;
#line 333 "filter/f-inst.c"
      struct f_inst * f2;
#line 335 "filter/f-inst.c"
      enum ec_subtype ecs;
#line 371 "filter/f-inst.c"
    } i_FI_EC_CONSTRUCT;
#line 378 "filter/f-inst.c"
 /* Instruction structure for config for FI_LC_CONSTRUCT */
#line 378
    struct {
#line 372 "filter/f-inst.c"
      struct f_inst * f1;
#line 373 "filter/f-inst.c"
      struct f_inst * f2;
#line 374 "filter/f-inst.c"
      struct f_inst * f3;
#line 378 "filter/f-inst.c"
    } i_FI_LC_CONSTRUCT;
#line 428 "filter/f-inst.c"
 /* Instruction structure for config for FI_PATHMASK_CONSTRUCT */
#line 428
    struct {
#line 379 "filter/f-inst.c"
      struct f_inst * fvar;
#line 379
      uint varcount;
#line 428 "filter/f-inst.c"
    } i_FI_PATHMASK_CONSTRUCT;
#line 435 "filter/f-inst.c"
 /* Instruction structure for config for FI_NEQ */
#line 435
    struct {
#line 429 "filter/f-inst.c"
      struct f_inst * f1;
#line 430 "filter/f-inst.c"
      struct f_inst * f2;
#line 435 "filter/f-inst.c"
    } i_FI_NEQ;
#line 442 "filter/f-inst.c"
 /* Instruction structure for config for FI_EQ */
#line 442
    struct {
#line 436 "filter/f-inst.c"
      struct f_inst * f1;
#line 437 "filter/f-inst.c"
      struct f_inst * f2;
#line 442 "filter/f-inst.c"
    } i_FI_EQ;
#line 453 "filter/f-inst.c"
 /* Instruction structure for config for FI_LT */
#line 453
    struct {
#line 443 "filter/f-inst.c"
      struct f_inst * f1;
#line 444 "filter/f-inst.c"
      struct f_inst * f2;
#line 453 "filter/f-inst.c"
    } i_FI_LT;
#line 464 "filter/f-inst.c"
 /* Instruction structure for config for FI_LTE */
#line 464
    struct {
#line 454 "filter/f-inst.c"
      struct f_inst * f1;
#line 455 "filter/f-inst.c"
      struct f_inst * f2;
#line 464 "filter/f-inst.c"
    } i_FI_LTE;
#line 469 "filter/f-inst.c"
 /* Instruction structure for config for FI_NOT */
#line 469
    struct {
#line 465 "filter/f-inst.c"
      struct f_inst * f1;
#line 469 "filter/f-inst.c"
    } i_FI_NOT;
#line 478 "filter/f-inst.c"
 /* Instruction structure for config for FI_MATCH */
#line 478
    struct {
#line 470 "filter/f-inst.c"
      struct f_inst * f1;
#line 471 "filter/f-inst.c"
      struct f_inst * f2;
#line 478 "filter/f-inst.c"
    } i_FI_MATCH;
#line 487 "filter/f-inst.c"
 /* Instruction structure for config for FI_NOT_MATCH */
#line 487
    struct {
#line 479 "filter/f-inst.c"
      struct f_inst * f1;
#line 480 "filter/f-inst.c"
      struct f_inst * f2;
#line 487 "filter/f-inst.c"
    } i_FI_NOT_MATCH;
#line 492 "filter/f-inst.c"
 /* Instruction structure for config for FI_DEFINED */
#line 492
    struct {
#line 488 "filter/f-inst.c"
      struct f_inst * f1;
#line 492 "filter/f-inst.c"
    } i_FI_DEFINED;
#line 504 "filter/f-inst.c"
 /* Instruction structure for config for FI_TYPE */
#line 504
    struct {
#line 493 "filter/f-inst.c"
      struct f_inst * f1;
#line 504 "filter/f-inst.c"
    } i_FI_TYPE;
#line 509 "filter/f-inst.c"
 /* Instruction structure for config for FI_IS_V4 */
#line 509
    struct {
#line 505 "filter/f-inst.c"
      struct f_inst * f1;
#line 509 "filter/f-inst.c"
    } i_FI_IS_V4;
#line 522 "filter/f-inst.c"
 /* Instruction structure for config for FI_VAR_INIT */
#line 522
    struct {
#line 511 "filter/f-inst.c"
      struct f_inst * f1;
#line 512 "filter/f-inst.c"
      struct symbol * sym;
#line 522 "filter/f-inst.c"
    } i_FI_VAR_INIT;
#line 531 "filter/f-inst.c"
 /* Instruction structure for config for FI_VAR_SET */
#line 531
    struct {
#line 524 "filter/f-inst.c"
      struct f_inst * f1;
#line 525 "filter/f-inst.c"
      struct symbol * sym;
#line 531 "filter/f-inst.c"
    } i_FI_VAR_SET;
#line 538 "filter/f-inst.c"
 /* Instruction structure for config for FI_VAR_GET */
#line 538
    struct {
#line 532 "filter/f-inst.c"
      struct symbol * sym;
#line 538 "filter/f-inst.c"
    } i_FI_VAR_GET;
#line 551 "filter/f-inst.c"
 /* Instruction structure for config for FI_CONSTANT */
#line 551
    struct {
#line 539 "filter/f-inst.c"
      struct f_val val;
#line 551 "filter/f-inst.c"
    } i_FI_CONSTANT;
#line 585 "filter/f-inst.c"
 /* Instruction structure for config for FI_FOR_INIT */
#line 585
    struct {
#line 553 "filter/f-inst.c"
      struct f_inst * f1;
#line 554 "filter/f-inst.c"
      struct symbol * sym;
#line 585 "filter/f-inst.c"
    } i_FI_FOR_INIT;
#line 639 "filter/f-inst.c"
 /* Instruction structure for config for FI_FOR_NEXT */
#line 639
    struct {
#line 587 "filter/f-inst.c"
      struct symbol * sym;
#line 630 "filter/f-inst.c"
      struct f_inst * f1;
#line 639 "filter/f-inst.c"
    } i_FI_FOR_NEXT;
#line 647 "filter/f-inst.c"
 /* Instruction structure for config for FI_CONDITION */
#line 647
    struct {
#line 640 "filter/f-inst.c"
      struct f_inst * f1;
#line 642 "filter/f-inst.c"
      struct f_inst * f2;
#line 644 "filter/f-inst.c"
      struct f_inst * f3;
#line 647 "filter/f-inst.c"
    } i_FI_CONDITION;
#line 656 "filter/f-inst.c"
 /* Instruction structure for config for FI_PRINT */
#line 656
    struct {
#line 649 "filter/f-inst.c"
      struct f_inst * fvar;
#line 649
      uint varcount;
#line 656 "filter/f-inst.c"
    } i_FI_PRINT;
#line 663 "filter/f-inst.c"
 /* Instruction structure for config for FI_FLUSH */
#line 663
    struct {
#line 663
    } i_FI_FLUSH;
#line 677 "filter/f-inst.c"
 /* Instruction structure for config for FI_DIE */
#line 677
    struct {
#line 665 "filter/f-inst.c"
      enum filter_return fret;
#line 677 "filter/f-inst.c"
    } i_FI_DIE;
#line 705 "filter/f-inst.c"
 /* Instruction structure for config for FI_RTA_GET */
#line 705
    struct {
#line 679 "filter/f-inst.c"
      struct f_static_attr sa;
#line 705 "filter/f-inst.c"
    } i_FI_RTA_GET;
#line 820 "filter/f-inst.c"
 /* Instruction structure for config for FI_RTA_SET */
#line 820
    struct {
#line 707 "filter/f-inst.c"
      struct f_inst * f1;
#line 708 "filter/f-inst.c"
      struct f_static_attr sa;
#line 820 "filter/f-inst.c"
    } i_FI_RTA_SET;
#line 892 "filter/f-inst.c"
 /* Instruction structure for config for FI_EA_GET */
#line 892
    struct {
#line 821 "filter/f-inst.c"
      struct f_dynamic_attr da;
#line 892 "filter/f-inst.c"
    } i_FI_EA_GET;
#line 959 "filter/f-inst.c"
 /* Instruction structure for config for FI_EA_SET */
#line 959
    struct {
#line 895 "filter/f-inst.c"
      struct f_inst * f1;
#line 896 "filter/f-inst.c"
      struct f_dynamic_attr da;
#line 959 "filter/f-inst.c"
    } i_FI_EA_SET;
#line 968 "filter/f-inst.c"
 /* Instruction structure for config for FI_EA_UNSET */
#line 968
    struct {
#line 960 "filter/f-inst.c"
      struct f_dynamic_attr da;
#line 968 "filter/f-inst.c"
    } i_FI_EA_UNSET;
#line 980 "filter/f-inst.c"
 /* Instruction structure for config for FI_LENGTH */
#line 980
    struct {
#line 969 "filter/f-inst.c"
      struct f_inst * f1;
#line 980 "filter/f-inst.c"
    } i_FI_LENGTH;
#line 1015 "filter/f-inst.c"
 /* Instruction structure for config for FI_NET_SRC */
#line 1015
    struct {
#line 981 "filter/f-inst.c"
      struct f_inst * f1;
#line 1015 "filter/f-inst.c"
    } i_FI_NET_SRC;
#line 1050 "filter/f-inst.c"
 /* Instruction structure for config for FI_NET_DST */
#line 1050
    struct {
#line 1016 "filter/f-inst.c"
      struct f_inst * f1;
#line 1050 "filter/f-inst.c"
    } i_FI_NET_DST;
#line 1060 "filter/f-inst.c"
 /* Instruction structure for config for FI_ROA_MAXLEN */
#line 1060
    struct {
#line 1051 "filter/f-inst.c"
      struct f_inst * f1;
#line 1060 "filter/f-inst.c"
    } i_FI_ROA_MAXLEN;
#line 1087 "filter/f-inst.c"
 /* Instruction structure for config for FI_ASN */
#line 1087
    struct {
#line 1061 "filter/f-inst.c"
      struct f_inst * f1;
#line 1087 "filter/f-inst.c"
    } i_FI_ASN;
#line 1092 "filter/f-inst.c"
 /* Instruction structure for config for FI_IP */
#line 1092
    struct {
#line 1088 "filter/f-inst.c"
      struct f_inst * f1;
#line 1092 "filter/f-inst.c"
    } i_FI_IP;
#line 1099 "filter/f-inst.c"
 /* Instruction structure for config for FI_ROUTE_DISTINGUISHER */
#line 1099
    struct {
#line 1093 "filter/f-inst.c"
      struct f_inst * f1;
#line 1099 "filter/f-inst.c"
    } i_FI_ROUTE_DISTINGUISHER;
#line 1106 "filter/f-inst.c"
 /* Instruction structure for config for FI_AS_PATH_FIRST */
#line 1106
    struct {
#line 1100 "filter/f-inst.c"
      struct f_inst * f1;
#line 1106 "filter/f-inst.c"
    } i_FI_AS_PATH_FIRST;
#line 1113 "filter/f-inst.c"
 /* Instruction structure for config for FI_AS_PATH_LAST */
#line 1113
    struct {
#line 1107 "filter/f-inst.c"
      struct f_inst * f1;
#line 1113 "filter/f-inst.c"
    } i_FI_AS_PATH_LAST;
#line 1118 "filter/f-inst.c"
 /* Instruction structure for config for FI_AS_PATH_LAST_NAG */
#line 1118
    struct {
#line 1114 "filter/f-inst.c"
      struct f_inst * f1;
#line 1118 "filter/f-inst.c"
    } i_FI_AS_PATH_LAST_NAG;
#line 1123 "filter/f-inst.c"
 /* Instruction structure for config for FI_PAIR_DATA */
#line 1123
    struct {
#line 1119 "filter/f-inst.c"
      struct f_inst * f1;
#line 1123 "filter/f-inst.c"
    } i_FI_PAIR_DATA;
#line 1128 "filter/f-inst.c"
 /* Instruction structure for config for FI_LC_DATA1 */
#line 1128
    struct {
#line 1124 "filter/f-inst.c"
      struct f_inst * f1;
#line 1128 "filter/f-inst.c"
    } i_FI_LC_DATA1;
#line 1133 "filter/f-inst.c"
 /* Instruction structure for config for FI_LC_DATA2 */
#line 1133
    struct {
#line 1129 "filter/f-inst.c"
      struct f_inst * f1;
#line 1133 "filter/f-inst.c"
    } i_FI_LC_DATA2;
#line 1167 "filter/f-inst.c"
 /* Instruction structure for config for FI_MIN */
#line 1167
    struct {
#line 1134 "filter/f-inst.c"
      struct f_inst * f1;
#line 1167 "filter/f-inst.c"
    } i_FI_MIN;
#line 1201 "filter/f-inst.c"
 /* Instruction structure for config for FI_MAX */
#line 1201
    struct {
#line 1168 "filter/f-inst.c"
      struct f_inst * f1;
#line 1201 "filter/f-inst.c"
    } i_FI_MAX;
#line 1227 "filter/f-inst.c"
 /* Instruction structure for config for FI_RETURN */
#line 1227
    struct {
#line 1204 "filter/f-inst.c"
      struct f_inst * f1;
#line 1227 "filter/f-inst.c"
    } i_FI_RETURN;
#line 1288 "filter/f-inst.c"
 /* Instruction structure for config for FI_CALL */
#line 1288
    struct {
#line 1229 "filter/f-inst.c"
      struct f_inst * fvar;
#line 1229
      uint varcount;
#line 1230 "filter/f-inst.c"
      struct symbol * sym;
#line 1288 "filter/f-inst.c"
    } i_FI_CALL;
#line 1293 "filter/f-inst.c"
 /* Instruction structure for config for FI_DROP_RESULT */
#line 1293
    struct {
#line 1290 "filter/f-inst.c"
      struct f_inst * f1;
#line 1293 "filter/f-inst.c"
    } i_FI_DROP_RESULT;
#line 1334 "filter/f-inst.c"
 /* Instruction structure for config for FI_SWITCH */
#line 1334
    struct {
#line 1294 "filter/f-inst.c"
      struct f_inst * f1;
#line 1296 "filter/f-inst.c"
      struct f_tree * tree;
#line 1334 "filter/f-inst.c"
    } i_FI_SWITCH;
#line 1342 "filter/f-inst.c"
 /* Instruction structure for config for FI_IP_MASK */
#line 1342
    struct {
#line 1335 "filter/f-inst.c"
      struct f_inst * f1;
#line 1336 "filter/f-inst.c"
      struct f_inst * f2;
#line 1342 "filter/f-inst.c"
    } i_FI_IP_MASK;
#line 1348 "filter/f-inst.c"
 /* Instruction structure for config for FI_PATH_PREPEND */
#line 1348
    struct {
#line 1343 "filter/f-inst.c"
      struct f_inst * f1;
#line 1344 "filter/f-inst.c"
      struct f_inst * f2;
#line 1348 "filter/f-inst.c"
    } i_FI_PATH_PREPEND;
#line 1405 "filter/f-inst.c"
 /* Instruction structure for config for FI_CLIST_ADD */
#line 1405
    struct {
#line 1349 "filter/f-inst.c"
      struct f_inst * f1;
#line 1350 "filter/f-inst.c"
      struct f_inst * f2;
#line 1405 "filter/f-inst.c"
    } i_FI_CLIST_ADD;
#line 1460 "filter/f-inst.c"
 /* Instruction structure for config for FI_CLIST_DEL */
#line 1460
    struct {
#line 1406 "filter/f-inst.c"
      struct f_inst * f1;
#line 1407 "filter/f-inst.c"
      struct f_inst * f2;
#line 1460 "filter/f-inst.c"
    } i_FI_CLIST_DEL;
#line 1506 "filter/f-inst.c"
 /* Instruction structure for config for FI_CLIST_FILTER */
#line 1506
    struct {
#line 1461 "filter/f-inst.c"
      struct f_inst * f1;
#line 1462 "filter/f-inst.c"
      struct f_inst * f2;
#line 1506 "filter/f-inst.c"
    } i_FI_CLIST_FILTER;
#line 1536 "filter/f-inst.c"
 /* Instruction structure for config for FI_ROA_CHECK_IMPLICIT */
#line 1536
    struct {
#line 1508 "filter/f-inst.c"
      struct rtable_config * rtc;
#line 1536 "filter/f-inst.c"
    } i_FI_ROA_CHECK_IMPLICIT;
#line 1558 "filter/f-inst.c"
 /* Instruction structure for config for FI_ROA_CHECK_EXPLICIT */
#line 1558
    struct {
#line 1538 "filter/f-inst.c"
      struct f_inst * f1;
#line 1539 "filter/f-inst.c"
      struct f_inst * f2;
#line 1540 "filter/f-inst.c"
      struct rtable_config * rtc;
#line 1558 "filter/f-inst.c"
    } i_FI_ROA_CHECK_EXPLICIT;
#line 1563 "filter/f-inst.c"
 /* Instruction structure for config for FI_FORMAT */
#line 1563
    struct {
#line 1559 "filter/f-inst.c"
      struct f_inst * f1;
#line 1563 "filter/f-inst.c"
    } i_FI_FORMAT;
#line 696 "filter/decl.m4"
 /* Instruction structure for config for FI_ASSERT */
#line 696
    struct {
#line 1565 "filter/f-inst.c"
      struct f_inst * f1;
#line 1567 "filter/f-inst.c"
      char * s;
#line 696 "filter/decl.m4"
    } i_FI_ASSERT;
#line 666 "filter/decl.m4"
  };
};

/* Filter line item */
struct f_line_item {
  enum f_instruction_code fi_code;	/* What to do */
  enum f_instruction_flags flags;	/* Flags, instruction-specific */
  uint lineno;				/* Where */
  union {
#line 284 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_ADD */
#line 284
    struct {
#line 284
    } i_FI_ADD;
#line 289 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_SUBTRACT */
#line 289
    struct {
#line 289
    } i_FI_SUBTRACT;
#line 294 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_MULTIPLY */
#line 294
    struct {
#line 294
    } i_FI_MULTIPLY;
#line 300 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_DIVIDE */
#line 300
    struct {
#line 300
    } i_FI_DIVIDE;
#line 310 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_AND */
#line 310
    struct {
#line 306 "filter/f-inst.c"
      const struct f_line * fl2;
#line 310 "filter/f-inst.c"
    } i_FI_AND;
#line 321 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_OR */
#line 321
    struct {
#line 316 "filter/f-inst.c"
      const struct f_line * fl2;
#line 321 "filter/f-inst.c"
    } i_FI_OR;
#line 331 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_PAIR_CONSTRUCT */
#line 331
    struct {
#line 331
    } i_FI_PAIR_CONSTRUCT;
#line 371 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_EC_CONSTRUCT */
#line 371
    struct {
      enum ec_subtype ecs;
#line 371 "filter/f-inst.c"
    } i_FI_EC_CONSTRUCT;
#line 378 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_LC_CONSTRUCT */
#line 378
    struct {
#line 378
    } i_FI_LC_CONSTRUCT;
#line 428 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_PATHMASK_CONSTRUCT */
#line 428
    struct {
#line 379 "filter/f-inst.c"
      uint varcount;
#line 428 "filter/f-inst.c"
    } i_FI_PATHMASK_CONSTRUCT;
#line 435 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_NEQ */
#line 435
    struct {
#line 435
    } i_FI_NEQ;
#line 442 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_EQ */
#line 442
    struct {
#line 442
    } i_FI_EQ;
#line 453 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_LT */
#line 453
    struct {
#line 453
    } i_FI_LT;
#line 464 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_LTE */
#line 464
    struct {
#line 464
    } i_FI_LTE;
#line 469 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_NOT */
#line 469
    struct {
#line 469
    } i_FI_NOT;
#line 478 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_MATCH */
#line 478
    struct {
#line 478
    } i_FI_MATCH;
#line 487 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_NOT_MATCH */
#line 487
    struct {
#line 487
    } i_FI_NOT_MATCH;
#line 492 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_DEFINED */
#line 492
    struct {
#line 492
    } i_FI_DEFINED;
#line 504 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_TYPE */
#line 504
    struct {
#line 504
    } i_FI_TYPE;
#line 509 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_IS_V4 */
#line 509
    struct {
#line 509
    } i_FI_IS_V4;
#line 522 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_VAR_INIT */
#line 522
    struct {
      struct symbol * sym;
#line 522 "filter/f-inst.c"
    } i_FI_VAR_INIT;
#line 531 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_VAR_SET */
#line 531
    struct {
      struct symbol * sym;
#line 531 "filter/f-inst.c"
    } i_FI_VAR_SET;
#line 538 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_VAR_GET */
#line 538
    struct {
      struct symbol * sym;
#line 538 "filter/f-inst.c"
    } i_FI_VAR_GET;
#line 551 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_CONSTANT */
#line 551
    struct {
      struct f_val val;
#line 551 "filter/f-inst.c"
    } i_FI_CONSTANT;
#line 585 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_FOR_INIT */
#line 585
    struct {
      struct symbol * sym;
#line 585 "filter/f-inst.c"
    } i_FI_FOR_INIT;
#line 639 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_FOR_NEXT */
#line 639
    struct {
      struct symbol * sym;
#line 630 "filter/f-inst.c"
      const struct f_line * fl1;
#line 639 "filter/f-inst.c"
    } i_FI_FOR_NEXT;
#line 647 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_CONDITION */
#line 647
    struct {
#line 642 "filter/f-inst.c"
      const struct f_line * fl2;
#line 644 "filter/f-inst.c"
      const struct f_line * fl3;
#line 647 "filter/f-inst.c"
    } i_FI_CONDITION;
#line 656 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_PRINT */
#line 656
    struct {
#line 649 "filter/f-inst.c"
      uint varcount;
#line 656 "filter/f-inst.c"
    } i_FI_PRINT;
#line 663 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_FLUSH */
#line 663
    struct {
#line 663
    } i_FI_FLUSH;
#line 677 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_DIE */
#line 677
    struct {
      enum filter_return fret;
#line 677 "filter/f-inst.c"
    } i_FI_DIE;
#line 705 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_RTA_GET */
#line 705
    struct {
      struct f_static_attr sa;
#line 705 "filter/f-inst.c"
    } i_FI_RTA_GET;
#line 820 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_RTA_SET */
#line 820
    struct {
      struct f_static_attr sa;
#line 820 "filter/f-inst.c"
    } i_FI_RTA_SET;
#line 892 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_EA_GET */
#line 892
    struct {
      struct f_dynamic_attr da;
#line 892 "filter/f-inst.c"
    } i_FI_EA_GET;
#line 959 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_EA_SET */
#line 959
    struct {
      struct f_dynamic_attr da;
#line 959 "filter/f-inst.c"
    } i_FI_EA_SET;
#line 968 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_EA_UNSET */
#line 968
    struct {
      struct f_dynamic_attr da;
#line 968 "filter/f-inst.c"
    } i_FI_EA_UNSET;
#line 980 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_LENGTH */
#line 980
    struct {
#line 980
    } i_FI_LENGTH;
#line 1015 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_NET_SRC */
#line 1015
    struct {
#line 1015
    } i_FI_NET_SRC;
#line 1050 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_NET_DST */
#line 1050
    struct {
#line 1050
    } i_FI_NET_DST;
#line 1060 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_ROA_MAXLEN */
#line 1060
    struct {
#line 1060
    } i_FI_ROA_MAXLEN;
#line 1087 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_ASN */
#line 1087
    struct {
#line 1087
    } i_FI_ASN;
#line 1092 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_IP */
#line 1092
    struct {
#line 1092
    } i_FI_IP;
#line 1099 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_ROUTE_DISTINGUISHER */
#line 1099
    struct {
#line 1099
    } i_FI_ROUTE_DISTINGUISHER;
#line 1106 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_AS_PATH_FIRST */
#line 1106
    struct {
#line 1106
    } i_FI_AS_PATH_FIRST;
#line 1113 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_AS_PATH_LAST */
#line 1113
    struct {
#line 1113
    } i_FI_AS_PATH_LAST;
#line 1118 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_AS_PATH_LAST_NAG */
#line 1118
    struct {
#line 1118
    } i_FI_AS_PATH_LAST_NAG;
#line 1123 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_PAIR_DATA */
#line 1123
    struct {
#line 1123
    } i_FI_PAIR_DATA;
#line 1128 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_LC_DATA1 */
#line 1128
    struct {
#line 1128
    } i_FI_LC_DATA1;
#line 1133 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_LC_DATA2 */
#line 1133
    struct {
#line 1133
    } i_FI_LC_DATA2;
#line 1167 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_MIN */
#line 1167
    struct {
#line 1167
    } i_FI_MIN;
#line 1201 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_MAX */
#line 1201
    struct {
#line 1201
    } i_FI_MAX;
#line 1227 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_RETURN */
#line 1227
    struct {
#line 1227
    } i_FI_RETURN;
#line 1288 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_CALL */
#line 1288
    struct {
#line 1229 "filter/f-inst.c"
      uint varcount;
      struct symbol * sym;
#line 1288 "filter/f-inst.c"
    } i_FI_CALL;
#line 1293 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_DROP_RESULT */
#line 1293
    struct {
#line 1293
    } i_FI_DROP_RESULT;
#line 1334 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_SWITCH */
#line 1334
    struct {
      struct f_tree * tree;
#line 1334 "filter/f-inst.c"
    } i_FI_SWITCH;
#line 1342 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_IP_MASK */
#line 1342
    struct {
#line 1342
    } i_FI_IP_MASK;
#line 1348 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_PATH_PREPEND */
#line 1348
    struct {
#line 1348
    } i_FI_PATH_PREPEND;
#line 1405 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_CLIST_ADD */
#line 1405
    struct {
#line 1405
    } i_FI_CLIST_ADD;
#line 1460 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_CLIST_DEL */
#line 1460
    struct {
#line 1460
    } i_FI_CLIST_DEL;
#line 1506 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_CLIST_FILTER */
#line 1506
    struct {
#line 1506
    } i_FI_CLIST_FILTER;
#line 1536 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_ROA_CHECK_IMPLICIT */
#line 1536
    struct {
      struct rtable_config * rtc;
#line 1536 "filter/f-inst.c"
    } i_FI_ROA_CHECK_IMPLICIT;
#line 1558 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_ROA_CHECK_EXPLICIT */
#line 1558
    struct {
      struct rtable_config * rtc;
#line 1558 "filter/f-inst.c"
    } i_FI_ROA_CHECK_EXPLICIT;
#line 1563 "filter/f-inst.c"
 /* Instruction structure for interpreter for FI_FORMAT */
#line 1563
    struct {
#line 1563
    } i_FI_FORMAT;
#line 696 "filter/decl.m4"
 /* Instruction structure for interpreter for FI_ASSERT */
#line 696
    struct {
      char * s;
#line 696 "filter/decl.m4"
    } i_FI_ASSERT;
#line 676 "filter/decl.m4"
  };
};

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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
#line 656

#line 663 "filter/f-inst.c"
 /* Constructor for FI_FLUSH */
#line 663
struct f_inst *
#line 663
f_new_inst_FI_FLUSH(enum f_instruction_code fi_code
#line 663
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
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
);
#line 696

#line 680 "filter/decl.m4"

