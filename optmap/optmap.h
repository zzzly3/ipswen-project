#include <linux/types.h>

#define OPT_EXT_SRC 233
#define OPT_EXT_DEST 241

/* Address-extension bytes stored in each option. Supported: 1, 2, 3. */
#ifndef OPT_ADDR_EXT_BYTES
#define OPT_ADDR_EXT_BYTES 1
#endif

#if OPT_ADDR_EXT_BYTES < 1 || OPT_ADDR_EXT_BYTES > 3
#error "OPT_ADDR_EXT_BYTES must be 1, 2 or 3"
#endif

#define OPT_ADDR_EXT_BITS (OPT_ADDR_EXT_BYTES * 8)
#define OPT_ADDR_MASK ((__u32)((1ULL << OPT_ADDR_EXT_BITS) - 1ULL))
#define OPT_ADDR_PREFIX_MASK ((__u32)(~OPT_ADDR_MASK))
#define OPT_OPT_LEN_RAW (2 + OPT_ADDR_EXT_BYTES)
#define OPT_OPT_LEN ((__u8)OPT_OPT_LEN_RAW)
#define OPT_EXT_RAW_LEN (2 * OPT_OPT_LEN_RAW)
#define OPT_EXT_PAD_LEN ((4 - (OPT_EXT_RAW_LEN & 0x3)) & 0x3)

/* TRACE: CFG_IFINDEX - eBPF uses ifindex (not ifname) to redirect packets. */
#define WAN_IFINDEX 3
#define LAN_IFINDEX 4

/* TRACE: CFG_ADDR - configurable NAT addresses. */
#define OUTER_ADDR 0x0a010101
// #define OUTER_ADDR 0xc0a80201
#define INNER_PREFIX 0x0a010200 // ONLY support 24 bits

// FIXME: hardcoded offset. DON'T MODIFY THE ORDER.
struct opt_ext {
    __u8 _scode;
    __u8 _slen;
    __u8 saddr[OPT_ADDR_EXT_BYTES];
    __u8 _dcode;
    __u8 _dlen;
    __u8 daddr[OPT_ADDR_EXT_BYTES];
#if OPT_EXT_PAD_LEN == 2
    __u8 pad0;
    __u8 pad1;
#endif
}; 

static inline int validate_opt_ext(struct opt_ext *ext) {
    if (!ext)
        return 0;
    if (ext->_scode != OPT_EXT_SRC || ext->_dcode != OPT_EXT_DEST)
        return 0;
    if (ext->_slen != OPT_OPT_LEN || ext->_dlen != OPT_OPT_LEN)
        return 0;
    return 1;
}

static inline __u32 opt_ext_get_addr(const __u8 *addr)
{
    __u32 val = 0;
#pragma clang loop unroll(full)
    for (int i = 0; i < OPT_ADDR_EXT_BYTES; i++)
        val = (val << 8) | addr[i];
    return val;
}

static inline void opt_ext_set_addr(__u8 *addr, __u32 val)
{
#pragma clang loop unroll(full)
    for (int i = 0; i < OPT_ADDR_EXT_BYTES; i++) {
        int shift = (OPT_ADDR_EXT_BYTES - 1 - i) * 8;
        addr[i] = (__u8)((val >> shift) & 0xff);
    }
}

static inline __u32 opt_ext_get_saddr(struct opt_ext *ext)
{
    return opt_ext_get_addr(ext->saddr);
}

static inline __u32 opt_ext_get_daddr(struct opt_ext *ext)
{
    return opt_ext_get_addr(ext->daddr);
}

static inline void opt_ext_set_saddr(struct opt_ext *ext, __u32 val)
{
    opt_ext_set_addr(ext->saddr, val & OPT_ADDR_MASK);
}

static inline void opt_ext_set_daddr(struct opt_ext *ext, __u32 val)
{
    opt_ext_set_addr(ext->daddr, val & OPT_ADDR_MASK);
}

static inline void init_opt_ext(struct opt_ext *ext) {
    if (!ext)
        return;
    ext->_scode = OPT_EXT_SRC;
    ext->_slen = OPT_OPT_LEN;
    opt_ext_set_saddr(ext, 0);
    ext->_dcode = OPT_EXT_DEST;
    ext->_dlen = OPT_OPT_LEN;
    opt_ext_set_daddr(ext, 0);
#if OPT_EXT_PAD_LEN == 2
    ext->pad0 = 0;
    ext->pad1 = 0;
#endif
}

static inline void fix_dest_addr(__u32 *daddr, __u32 *ext_daddr) {
    if (*daddr == OUTER_ADDR) {
        *daddr = (INNER_PREFIX & OPT_ADDR_PREFIX_MASK) | (*ext_daddr & OPT_ADDR_MASK);
    }
}

static inline void fix_src_addr(__u32 *saddr, __u32 *ext_saddr) {
    if ((*saddr & OPT_ADDR_PREFIX_MASK) == (INNER_PREFIX & OPT_ADDR_PREFIX_MASK)) {
        *ext_saddr = *saddr & OPT_ADDR_MASK;
        *saddr = OUTER_ADDR;
    }
}

static inline __u16 csum16_add(__u16 csum, __u16 addend)
{
	csum += addend;
	return csum + (csum < addend);
}
