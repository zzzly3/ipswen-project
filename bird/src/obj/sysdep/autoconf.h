/* obj/sysdep/autoconf.h.  Generated from autoconf.h.in by configure.  */
/* sysdep/autoconf.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Babel protocol */
#define CONFIG_BABEL 1

/* BFD protocol */
#define CONFIG_BFD 1

/* BGP protocol */
#define CONFIG_BGP 1

/* BMP protocol */
/* #undef CONFIG_BMP */

/* MRT protocol */
#define CONFIG_MRT 1

/* OSPF protocol */
#define CONFIG_OSPF 1

/* Pipe protocol */
#define CONFIG_PIPE 1

/* RAdv protocol */
#define CONFIG_RADV 1

/* RIP protocol */
#define CONFIG_RIP 1

/* RPKI protocol */
#define CONFIG_RPKI 1

/* Static protocol */
#define CONFIG_STATIC 1

/* Define to 1 if cpu is big endian */
/* #undef CPU_BIG_ENDIAN */

/* Define to 1 if cpu is little endian */
#define CPU_LITTLE_ENDIAN 1

/* Define to 1 if debugging is enabled */
/* #undef DEBUGGING */

/* Define to 1 if you want 4-way tries instead of 16-way ones. */
/* #undef ENABLE_COMPACT_TRIES */

/* Define to 1 if you want to run expensive consistency checks. */
/* #undef ENABLE_EXPENSIVE_CHECKS */

/* Define to 1 if you have the <alloca.h> header file. */
#define HAVE_ALLOCA_H 1

/* Define to 1 if you have the <curses.h> header file. */
#define HAVE_CURSES_H 1

/* Define to 1 if you have the <execinfo.h> header file. */
/* #undef HAVE_EXECINFO_H */

/* Define to 1 if you have the `getentropy' function. */
#define HAVE_GETENTROPY 1

/* Define to 1 if you have the `getrandom' function. */
#define HAVE_GETRANDOM 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `dmalloc' library (-ldmalloc). */
/* #undef HAVE_LIBDMALLOC */

/* Define to 1 if you have the `efence' library (-lefence). */
/* #undef HAVE_LIBEFENCE */

/* Define to 1 if you have the `ssh' library (-lssh). */
/* #undef HAVE_LIBSSH */

/* Define to 1 if mmap() is available. */
#define HAVE_MMAP 1

/* Define to 1 if kernel is MPLS capable */
#define HAVE_MPLS_KERNEL 1

/* Define to 1 if you have the <readline/history.h> header file. */
#define HAVE_READLINE_HISTORY_H 1

/* Define to 1 if you have the <readline/readline.h> header file. */
#define HAVE_READLINE_READLINE_H 1

/* Define to 1 if you have rl_crlf() */
#define HAVE_RL_CRLF 1

/* Define to 1 if you have rl_ding() */
#define HAVE_RL_DING 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if `sa_len' is a member of `struct sockaddr'. */
/* #undef HAVE_STRUCT_SOCKADDR_SA_LEN */

/* Define to 1 if you have the <syslog.h> header file. */
#define HAVE_SYSLOG_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if _Thread_local is available */
#define HAVE_THREAD_LOCAL 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define to 1 if all of the C90 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Which sysdep header to include */
#define SYSCONF_INCLUDE "./sysdep/cf/linux.h"

/* Define to 1 if pthreads are enabled */
#define USE_PTHREADS 1

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to 1 if the system supports IPswen */
#define HAVE_IPSWEN 1
