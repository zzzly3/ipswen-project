#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <netinet/in.h>
#include <string.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#define MAX_OPTIONS_ROUTES 256

// 最简单的一个转发表项
struct rt_item {
	int ifindex; // 转发出去的接口
	char eth_source[ETH_ALEN]; // 封装帧的源MAC地址。
	char eth_dest[ETH_ALEN]; // 封装帧的目标MAC地址。
};

// 路由转发表缓存
struct {
	__uint(type, BPF_MAP_TYPE_LRU_HASH); // 采用LRU机制，自动老化表项
	__uint(max_entries, 100);
	__type(key, int);
	__type(value, struct rt_item);
} rtcache_map SEC("maps");

SEC("xdp_router")
int xdp_route_handler(struct xdp_md *ctx) {
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    
    char daddr_int[] = "dst_int = %d\n";
    char daddr_info[] = "dst_ipv4 = %pI4\n";
    char nooption_info[] = "No option to kernel[%d]\n";
    char fast_info[] = "Fast path to [%d]\n";
    char slow_info[] = "Slow path to [%d]\n";
    
    struct ethhdr *eth = data;
    if (eth + 1 > data_end)
        return XDP_PASS;

    // 只处理IPv4数据包
    if (eth->h_proto != bpf_htons(ETH_P_IP))
        return XDP_PASS;

    struct iphdr *iph = data + sizeof(*eth);
    if (iph + 1 > data_end)
        return XDP_PASS;

    // 检查IP选项：IHL > 5表示有选项
    if (iph->ihl <= 5) {
        //bpf_trace_printk(nooption_info, sizeof(nooption_info), -1);
        return XDP_PASS;  // 无选项走内核协议栈
    }

    // 开始处理带选项的数据包
    __u32 daddr = iph->daddr;
    //bpf_trace_printk(daddr_int, sizeof(daddr_int), iph->ihl - 5);
    //bpf_trace_printk(daddr_info, sizeof(daddr_info), &daddr);

	struct rt_item* pitem = bpf_map_lookup_elem(&rtcache_map, &daddr);
	// 首先精确查找转发表，如果找到就直接转发，不必再经历最长前缀匹配的慢速通配查找
	if (pitem) {
		__builtin_memcpy(eth->h_dest, pitem->eth_dest, ETH_ALEN);
		__builtin_memcpy(eth->h_source, pitem->eth_source, ETH_ALEN);
		//bpf_trace_printk(fast_info, sizeof(fast_info), pitem->ifindex);
		return bpf_redirect(pitem->ifindex, 0);
	}

	
	// 否则只能执行最长前缀匹配了
	struct bpf_fib_lookup ifib;
	__builtin_memset(&ifib, 0, sizeof(ifib));
	ifib.family = AF_INET;
	ifib.tos = iph->tos;
	ifib.l4_protocol = iph->protocol;
	ifib.tot_len = bpf_ntohs(iph->tot_len);
	ifib.ipv4_src = iph->saddr;
	ifib.ipv4_dst = iph->daddr;
	ifib.ifindex = ctx->ingress_ifindex;

	int rc = bpf_fib_lookup(ctx, &ifib, sizeof(ifib), 0);
	
	// 调用eBPF封装的路由查找函数，虽然所谓慢速查找，也依然不会进入协议栈的。
	if (rc == 0) {
		
		if (ifib.ifindex == ctx->ingress_ifindex)
            		return XDP_PASS; // 防止环回
		
		struct rt_item nitem;

		__builtin_memset(&nitem, 0, sizeof(nitem));
		__builtin_memcpy(&nitem.eth_dest, ifib.dmac, ETH_ALEN);
		__builtin_memcpy(&nitem.eth_source, ifib.smac, ETH_ALEN);
		nitem.ifindex = ifib.ifindex;
		// 插入新的表项
		bpf_map_update_elem(&rtcache_map, &daddr, &nitem, BPF_ANY);
		__builtin_memcpy(eth->h_dest, ifib.dmac, ETH_ALEN);
		__builtin_memcpy(eth->h_source, ifib.smac, ETH_ALEN);
		//bpf_trace_printk(slow_info, sizeof(slow_info), ifib.ifindex);
		return bpf_redirect(ifib.ifindex, 0);
	}

	return XDP_PASS;
}

char _license[] SEC("license") = "GPL";