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

SEC("xdp_router")
int xdp_route_handler(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

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

    if (iph->protocol != IPPROTO_TCP)
        return XDP_PASS;
    
    if (daddr == OUTER_ADDR) {
        // from outter to inner
        if (ext == NULL)
            return XDP_PASS;
        daddr = INNER_PREFIX | ext_daddr;
    }
    else if ((saddr >> 8) == (INNER_PREFIX >> 8)) {
        // from inner to outter
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
            csum = csum16_add(csum, OPT_EXT_SRC + (3 << 8));
            csum = csum16_add(csum, (OPT_EXT_DEST << 8) + 3);
        }
        #endif
        ext_saddr = saddr & 0xff;
        saddr = OUTER_ADDR;
    } else
        return XDP_PASS;
    
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
        csum = csum16_add(csum, ~iph->tot_len);
        csum = csum16_add(csum, ip_len);
        csum = csum16_add(csum, ~(__u16)(ext->saddr));
        csum = csum16_add(csum, (__u16)(ext_saddr));
        csum = csum16_add(csum, ~((__u16)(ext->daddr) << 8));
        csum = csum16_add(csum, (__u16)(ext_daddr) << 8);
        ext->daddr = ext_daddr;
        ext->saddr = ext_saddr;
        iph->tot_len = ip_len;
    }
    bpf_printk("%x %x -> %x %x csum %x -> %x\n", iph->saddr, iph->daddr, saddr, daddr, iph->check, csum);
    iph->check = ~csum;

    struct tcphdr *tcph = (struct tcphdr *)((__u8 *)iph + ip_hdr_len);
    if (tcph + 1 > data_end)
        return XDP_PASS;
    __u16 tcp_csum = ~tcph->check;
    tcp_csum = csum16_add(tcp_csum, addr_csum);
    tcph->check = ~tcp_csum;

    iph->saddr = saddr;
    iph->daddr = daddr;
    // bpf_printk("ext->pad1=%u ext->pad0=%u\n", ext->pad1, ext->pad0);
    // bpf_printk("IP options: src=%d, dest=%d\n", ext->saddr, ext->daddr);
    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
