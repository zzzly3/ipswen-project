#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <string.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <net/if.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#include "optmap.h"


static __always_inline int
xdp_redirect_ipv4(struct xdp_md *ctx, void *data, void *data_end, struct iphdr *iph)
{
    struct ethhdr *eth = data;

    if ((void *)(eth + 1) > data_end)
        return XDP_ABORTED;

    // ===== FIB 查询 =====
    struct bpf_fib_lookup fib = {};
    fib.family        = AF_INET;
    fib.tos           = iph->tos;
    fib.l4_protocol   = iph->protocol;
    fib.tot_len       = bpf_ntohs(iph->tot_len);
    fib.ipv4_src      = iph->saddr;
    fib.ipv4_dst      = iph->daddr;
    fib.ifindex       = ctx->ingress_ifindex;

    int rc = bpf_fib_lookup(ctx, &fib, sizeof(fib), 0);

    if (rc != BPF_FIB_LKUP_RET_SUCCESS)
        return XDP_PASS;

    // ===== 防止回环 =====
    if (fib.ifindex == ctx->ingress_ifindex)
        return XDP_PASS;

    // ===== 改写二层头 =====
    __builtin_memcpy(eth->h_dest, fib.dmac, ETH_ALEN);
    __builtin_memcpy(eth->h_source, fib.smac, ETH_ALEN);

    // ===== redirect =====
    return bpf_redirect(fib.ifindex, 0);
}


// #define bpf_printk(...)
#define ALWAYS_ALLOC_EXT 1

#ifdef ALWAYS_ALLOC_EXT
int alloc_space_for_ext(struct xdp_md *ctx) {
    struct opt_ext ext;
    init_opt_ext(&ext);
    if (bpf_xdp_adjust_head(ctx, -(__s32)sizeof(struct opt_ext)))
        return 0;
    if (ctx->data + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(ext) > ctx->data_end)
        return 0;
    memmove((void *)(long)ctx->data, (void *)(long)ctx->data + sizeof(ext), sizeof(struct ethhdr) + sizeof(struct iphdr));
    if (ctx->data + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(ext) > ctx->data_end)
        return 0;
    memmove((void *)(long)ctx->data + sizeof(struct ethhdr) + sizeof(struct iphdr), &ext, sizeof(ext));
    return 1;
}
#endif

static __always_inline __u16 opt_ext_sum_words(const struct opt_ext *ext)
{
    const __u8 *b = (const __u8 *)ext;
    __u16 sum = 0;

#pragma clang loop unroll(full)
    for (int i = 0; i < sizeof(struct opt_ext); i += 2) {
        __u16 w = (__u16)b[i] | ((__u16)b[i + 1] << 8);
        sum = csum16_add(sum, w);
    }

    return sum;
}

static __always_inline __u16 csum_opt_ext_addr_delta(__u16 csum, const struct opt_ext *old_ext,
                                                      const struct opt_ext *new_ext)
{
#pragma clang loop unroll(full)
    for (int i = 0; i < OPT_ADDR_EXT_BYTES; i++) {
        __u16 shift = ((2 + i) & 1) ? 8 : 0;
        csum = csum16_add(csum, ~((__u16)old_ext->saddr[i] << shift));
        csum = csum16_add(csum, ((__u16)new_ext->saddr[i] << shift));
    }

#pragma clang loop unroll(full)
    for (int i = 0; i < OPT_ADDR_EXT_BYTES; i++) {
        __u16 shift = ((4 + OPT_ADDR_EXT_BYTES + i) & 1) ? 8 : 0;
        csum = csum16_add(csum, ~((__u16)old_ext->daddr[i] << shift));
        csum = csum16_add(csum, ((__u16)new_ext->daddr[i] << shift));
    }

    return csum;
}

SEC("xdp_router")
int xdp_route_handler(struct xdp_md *ctx) {
    __u64 start_ns = bpf_ktime_get_ns();
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    /* TRACE: FLOW_IFINDEX - capture ingress ifindex for direction check. */
    __u32 ingress_ifindex = ctx->ingress_ifindex;
    /* TRACE: FLOW_REDIRECT - selected egress ifindex for matched packets. */
    __u32 redirect_ifindex = 0;

    struct ethhdr *eth = data;
    if (eth + 1 > data_end)
        return XDP_PASS;
    if (eth->h_proto != bpf_htons(ETH_P_IP))
        return XDP_PASS;
    
    struct iphdr *iph = data + sizeof(*eth);
    if (iph + 1 > data_end)
        return XDP_PASS;
    int ip_hdr_len = iph->ihl * 4;
    __u16 ip_len = bpf_ntohs(iph->tot_len);
    __u16 csum = ~iph->check;
    __u32 daddr = bpf_ntohl(iph->daddr);
    __u32 saddr = bpf_ntohl(iph->saddr);
    struct opt_ext *ext = NULL;
    __u32 ext_daddr = 0;
    __u32 ext_saddr = 0;
    int options_len = ip_hdr_len - sizeof(*iph);
    if (options_len > 0) {
        if (options_len < sizeof(struct opt_ext))
            return XDP_PASS;
        ext = (struct opt_ext *)(iph + 1);
        if (ext + 1 > data_end || !validate_opt_ext(ext))
            return XDP_PASS;
        ext_daddr = opt_ext_get_daddr(ext);
        ext_saddr = opt_ext_get_saddr(ext);
        
    }
    __u64 delta_ns = bpf_ktime_get_ns() - start_ns;    
    
    if (saddr == OUTER_ADDR) {
        // from inner to outter
        /* TRACE: LAN_IN_CHECK - only rewrite packets entering from LAN. */
        if (ingress_ifindex != LAN_IFINDEX)
            return XDP_PASS;
        if (ext == NULL)
            return XDP_PASS;
        saddr = (INNER_PREFIX & OPT_ADDR_PREFIX_MASK) | (ext_daddr & OPT_ADDR_MASK);
        /* TRACE: LAN_TO_WAN - matched flow is forwarded to WAN directly. */
        redirect_ifindex = WAN_IFINDEX;
    }
    else if ((daddr & OPT_ADDR_PREFIX_MASK) == (INNER_PREFIX & OPT_ADDR_PREFIX_MASK)) {
        // from outter to inner
        /* TRACE: WAN_IN_CHECK - only trust packets entering from WAN. */
        if (ingress_ifindex != WAN_IFINDEX)
            return XDP_PASS;
        #ifdef ALWAYS_ALLOC_EXT
        if (ext == NULL) {
            // alloc space for ext
            if (!alloc_space_for_ext(ctx))
                return XDP_PASS;
            data_end = (void *)(long)ctx->data_end;
            data = (void *)(long)ctx->data;
            iph = data + sizeof(*eth);
            if (iph + 1 > data_end)
                return XDP_PASS;
            iph->ihl += sizeof(struct opt_ext) / 4;
            ip_hdr_len += sizeof(struct opt_ext);
            ip_len += sizeof(struct opt_ext);
            csum = csum16_add(csum, sizeof(struct opt_ext) / 4);
            {
                struct opt_ext ext_init;
                init_opt_ext(&ext_init);
                csum = csum16_add(csum, opt_ext_sum_words(&ext_init));
            }
        }
        #endif
        ext_daddr = daddr & OPT_ADDR_MASK;
        daddr = OUTER_ADDR;
        /* TRACE: WAN_TO_LAN - matched flow is forwarded to LAN directly. */
        redirect_ifindex = LAN_IFINDEX;
    } else
        return XDP_PASS;

    #ifdef ALWAYS_ALLOC_EXT
    data_end = (void *)(long)ctx->data_end;
    iph = data + sizeof(*eth);
    if (iph + 1 > data_end)
        return XDP_PASS;
    ext = (struct opt_ext *)(iph + 1);
    if (ext + 1 > data_end)
        return XDP_PASS;
    #endif
    
    // fix checksum
    ip_len = bpf_htons(ip_len);
    daddr = bpf_htonl(daddr);
    saddr = bpf_htonl(saddr);
    __u16 addr_csum = 0;
    addr_csum = csum16_add(addr_csum, ~(__u16)(iph->saddr >> 16));
    addr_csum = csum16_add(addr_csum, ~(__u16)(iph->saddr));
    addr_csum = csum16_add(addr_csum, (__u16)(saddr >> 16));
    addr_csum = csum16_add(addr_csum, (__u16)(saddr));
    addr_csum = csum16_add(addr_csum, ~(__u16)(iph->daddr >> 16));
    addr_csum = csum16_add(addr_csum, ~(__u16)(iph->daddr));
    addr_csum = csum16_add(addr_csum, (__u16)(daddr >> 16));
    addr_csum = csum16_add(addr_csum, (__u16)(daddr));
    csum = csum16_add(csum, addr_csum);
    if (ext) {
        struct opt_ext old_ext;
        __builtin_memcpy(&old_ext, ext, sizeof(old_ext));
        csum = csum16_add(csum, ~iph->tot_len);
        csum = csum16_add(csum, ip_len);
        opt_ext_set_daddr(ext, ext_daddr);
        opt_ext_set_saddr(ext, ext_saddr);
        csum = csum_opt_ext_addr_delta(csum, &old_ext, ext);
        iph->tot_len = ip_len;
    }
    bpf_printk("%x %x -> %x %x csum %x -> %x\n", iph->saddr, iph->daddr, saddr, daddr, iph->check, csum);
    iph->check = ~csum;
    
    if (iph->protocol == IPPROTO_UDP) {
        struct udphdr *udph = (struct udphdr *)((__u8 *)iph + ip_hdr_len);
        if (udph + 1 > data_end)
            return XDP_PASS;
        __u16 udp_csum = ~udph->check;
        udp_csum = csum16_add(udp_csum, addr_csum);
        udph->check = ~udp_csum;
    }

    if (iph->protocol == IPPROTO_TCP) {
        struct tcphdr *tcph = (struct tcphdr *)((__u8 *)iph + ip_hdr_len);
        if (tcph + 1 > data_end)
            return XDP_PASS;
        __u16 tcp_csum = ~tcph->check;
        tcp_csum = csum16_add(tcp_csum, addr_csum);
        tcph->check = ~tcp_csum;
    }

    iph->saddr = saddr;
    iph->daddr = daddr;
    // bpf_printk("ext->pad1=%u ext->pad0=%u\n", ext->pad1, ext->pad0);
    // bpf_printk("IP options: src=%d, dest=%d\n", ext->saddr, ext->daddr);
    /* TRACE: XDP_BYPASS - bypass kernel stack for matched packets. */
    int action = xdp_redirect_ipv4(ctx, data, data_end, iph);
    return action;

}

char _license[] SEC("license") = "GPL";
