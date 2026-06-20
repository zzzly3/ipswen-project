# optmap —— 基于 eBPF/XDP 的 IP 选项路由和快速转发 NAT 网关

## 概述

`optmap` 是一个基于 **eBPF/XDP（eXpress Data Path）** 的内核级数据包路由与网络地址转换（NAT）框架。它利用 Linux 内核的 XDP 技术在网络驱动层（NIC 收包后、内核协议栈之前的最早路径）拦截数据包，根据**自定义 IP 选项**（`opt_ext` 结构体编码的扩展源/目标地址）执行线速的数据包改写和转发，实现了一个不经过内核协议栈（零 sk_buff 分配）的超低延迟网关。

**核心研究问题：**
1. 能否基于自定义 IP 选项（而非路由表或 NAT 表）实现确定性的数据包路由
2. 在内核最早路径（XDP）上做地址改写能达到什么级别的吞吐量和延迟
3. 当数据包没有预置 IP 选项时，自动分配扩展地址空间（`ALWAYS_ALLOC_EXT`）的可行性和性能影响
4. eBPF 辅助函数 `bpf_fib_lookup` 与 XDP redirect 组合使用替代传统 iptables NAT 的实际表现

**两个版本的设计差异：**

| 特性 | optmap.c（复杂版） | optmap2.c（简化版） |
|------|---------------------|----------------------|
| 路由方向判定 | 外→内：daddr==OUTER_ADDR；内→外：saddr 匹配 INNER_PREFIX | 外→内：daddr 匹配 INNER_PREFIX；内→外：saddr==OUTER_ADDR |
| L4 协议支持 | TCP、UDP、ICMP（含 ICMP 错误消息中内嵌 IP 头的递归改写） | 仅 TCP |
| 校验和工程 | 完整的增量校验和（csum_opt_ext_addr_delta） | 简化的校验和重算 |
| ICMP 错误处理 | 递归修改内嵌 IP 头地址 | 不支持 |
| 用途 | 生产级通用 NAT 网关 | TCP-only 轻量级演示 |

**为什么同时保留两个版本：** optmap2.c 是早期的概念验证（更简洁、更易理解），optmap.c 是在其基础上增加了 UDP/ICMP 支持和健壮的校验和处理的完整版。两者均有独立的研究价值——optmap2 更适合教学和学习 eBPF 概念，optmap 更适合实际部署和性能测量。

---

## 目录结构

```
optmap/
├── optmap.h                # ★ 核心头文件：opt_ext 结构体、地址掩码、校验和工具函数
├── optmap.c                # ★ eBPF/XDP 主程序（复杂版：TCP+UDP+ICMP+完整校验和）
├── optmap.o                # 编译产物（eBPF 字节码 ELF）
│
├── optmap2.c               # eBPF/XDP 简化版（TCP-only）
├── optmap2.o               # 编译产物
│
├── ebpf.c                  # eBPF/XDP 备选路由方案：基于 FIB 缓存的路由
├── ebpf.o                  # 编译产物
│
├── mynat.c                 # eBPF/XDP 最小 NAT 示例（仅修改地址，不查路由）
│
├── send.c / send           # UDP 发送测试工具（设置 IP 选项的 IPv4 UDP 发送器）
├── recv.c / recv           # UDP 接收测试工具（接收 IP 选项的 IPv4 UDP 接收器）
├── send2.c / send2         # TCP 发送测试工具（设置 IP 选项的 IPv4 TCP 客户端）
├── recv2.c / recv2         # TCP 接收测试工具（IPv4 TCP 服务器）
│
├── mytcp.c / mytcp.so      # LD_PRELOAD TCP socket wrapper（自动注入 IP 选项）
├── addnat.py               # iptables DNAT 规则批量生成脚本
├── parse_trace.py          # eBPF trace log 解析和统计分析
│
├── 1.cap / 2.cap           # tcpdump 抓包文件（实验记录）
└── Makefile                # 编译和部署脚本
```

---

## 核心头文件详解：optmap.h

### 选项类型定义

```c
#define OPT_EXT_SRC  233   // 源地址扩展选项类型号
#define OPT_EXT_DEST 241   // 目标地址扩展选项类型号
```

这两个自定义选项类型号（233 和 241）在 IANA IP Option Numbers 空间中属于未分配范围，不与任何标准 IP 选项冲突。选项类型字节的位布局遵循 RFC 791：

```
OPT_EXT_SRC  = 233 = 0xE9 = 11101001
  Bit 7 (0x80): Copy-on-Fragmentation = 1 (所有分片携带此选项)
  Bit 6-5:      Option Class = 2 (Debug/Measurement)
  Bit 4-0:      Option Number = 9

OPT_EXT_DEST = 241 = 0xF1 = 11110001
  Bit 7 (0x80): Copy-on-Fragmentation = 1
  Bit 6-5:      Option Class = 2
  Bit 4-0:      Option Number = 17
```

### 可配置常数

```c
#ifndef OPT_ADDR_EXT_BYTES
#define OPT_ADDR_EXT_BYTES 1    // 默认每个方向的扩展地址占 1 字节
#endif

#if OPT_ADDR_EXT_BYTES < 1 || OPT_ADDR_EXT_BYTES > 3
#error "OPT_ADDR_EXT_BYTES must be 1, 2 or 3"
#endif

#define OPT_ADDR_EXT_BITS   (OPT_ADDR_EXT_BYTES * 8)       // 扩展地址的位数
#define OPT_ADDR_MASK       ((__u32)((1ULL << OPT_ADDR_EXT_BITS) - 1ULL))  // 如 0xFF (1 字节)
#define OPT_ADDR_PREFIX_MASK ((__u32)(~OPT_ADDR_MASK))     // 如 0xFFFFFF00
```

**OPT_ADDR_EXT_BYTES 的设计空间：**

| 值 | 扩展地址位数 | 每个方向可编码的地址数 | 实际用途 |
|----|-------------|----------------------|---------|
| 1 | 8 bits | 256 | 小型测试床（默认） |
| 2 | 16 bits | 65536 | 中型部署 |
| 3 | 24 bits | 16M | 大型网络 |

### opt_ext 结构体内存布局

```c
struct opt_ext {
    __u8 _scode;                     // 0xE9 (OPT_EXT_SRC)
    __u8 _slen;                      // 0x03 (OPT_OPT_LEN = 2 + 1 = 3)
    __u8 saddr[OPT_ADDR_EXT_BYTES];  // 源地址扩展字节
    __u8 _dcode;                     // 0xF1 (OPT_EXT_DEST)
    __u8 _dlen;                      // 0x03
    __u8 daddr[OPT_ADDR_EXT_BYTES];  // 目标地址扩展字节
#if OPT_EXT_PAD_LEN == 2
    __u8 pad0;                       // 对齐填充（当总长度 % 4 == 2 时需要 2 字节）
    __u8 pad1;
#endif
};
```

**内存中的字节布局（以 OPT_ADDR_EXT_BYTES=1 为例）：**
```
偏移  0    1    2    3    4    5    6    7
    +----+----+----+----+----+----+----+----+
    |E9  |03  |s[0]|F1  |03  |d[0]|00  |00  |  总长 8 字节
    +----+----+----+----+----+----+----+----+
     \_scode   \_saddr   \_daddr    \_pad
           \__slen      \__dlen
```

IP 选项的总长度必须是 4 字节的倍数（RFC 791 要求）。`OPT_EXT_PAD_LEN` 的计算：
```c
#define OPT_EXT_RAW_LEN (2 * OPT_OPT_LEN_RAW)   // 2 * (2 + OPT_ADDR_EXT_BYTES)
#define OPT_EXT_PAD_LEN ((4 - (OPT_EXT_RAW_LEN & 0x3)) & 0x3)
// 当 OPT_ADDR_EXT_BYTES=1: RAW_LEN=6, PAD_LEN=2 (6→8)
// 当 OPT_ADDR_EXT_BYTES=2: RAW_LEN=8, PAD_LEN=0 (8→8 已对齐)
// 当 OPT_ADDR_EXT_BYTES=3: RAW_LEN=10, PAD_LEN=2 (10→12)
```

### 网络配置常量

```c
#define WAN_IFINDEX 3       // 外网接口的 ifindex（通过 if_nametoindex 获取）
#define LAN_IFINDEX 4       // 内网接口的 ifindex
#define OUTER_ADDR 0x0a010101   // 外网地址 10.1.1.1（host order）
// #define OUTER_ADDR 0xc0a80201  // 备选：192.168.2.1
#define INNER_PREFIX 0x0a010200 // 内网前缀 10.1.2.0/24（仅支持 24 位前缀）
```

**ifindex 如何确定：**
```bash
# 方法 1：查看 /sys/class/net/
cat /sys/class/net/eth1/ifindex   # 输出 3
cat /sys/class/net/eth2/ifindex   # 输出 4

# 方法 2：使用 ip 命令
ip link show eth1 | head -1       # 输出 "3: eth1: ..."
```

**为什么用 ifindex 而非接口名：** eBPF 的 `bpf_redirect()` 和 `bpf_fib_lookup()` 使用 `ifindex`（整数），而非接口名字符串。int 比较比字符串比较快得多，且 eBPF 验证器对整数操作的限制更少。

### 校验和工具函数

```c
static inline __u16 csum16_add(__u16 csum, __u16 addend) {
    csum += addend;
    return csum + (csum < addend);  // 处理进位（carry）
}
```

IP 校验和（RFC 1071）是 16 位反码和。增量修改校验和的数学原理：

```
new_csum = ~(~old_csum + ~old_value + new_value)
         = old_csum_partial + ~old_value + new_value (带进位回卷)
```

`csum16_add` 实现了"带进位回卷的 16 位加法"，是增量校验和更新的核心原语。

```c
static inline __u16 csum_replace_u32(__u16 csum, __u32 old, __u32 new) {
    csum = csum16_add(csum, ~(__u16)(old >> 16));  // 移除旧值的高 16 位
    csum = csum16_add(csum, ~(__u16)old);           // 移除旧值的低 16 位
    csum = csum16_add(csum, (__u16)(new >> 16));    // 加入新值的高 16 位
    csum = csum16_add(csum, (__u16)new);            // 加入新值的低 16 位
    return csum;
}
```

---

## 核心 XDP 程序源码解析：optmap.c

### 1. 程序入口与数据包解析

```c
SEC("xdp_router")
int xdp_route_handler(struct xdp_md *ctx) {
    // ★ XDP 上下文只包含 3 个字段：
    //   ctx->data          → 数据包起始指针
    //   ctx->data_end      → 数据包末尾指针
    //   ctx->ingress_ifindex → 收包接口的 ifindex

    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    // 逐层解析，每层都做边界检查（eBPF 验证器强制要求）
    struct ethhdr *eth = data;
    if (eth + 1 > data_end) return XDP_PASS;     // 以太网头不完整 → 放行
    if (eth->h_proto != bpf_htons(ETH_P_IP))
        return XDP_PASS;                          // 非 IPv4 → 放行

    struct iphdr *iph = data + sizeof(*eth);
    if (iph + 1 > data_end) return XDP_PASS;     // IP 头不完整
    int ip_hdr_len = iph->ihl * 4;               // IHL 单位是 4 字节
    __u16 csum = ~iph->check;                    // 对校验和取反 → 得到部分和
```

**为什么用 `~iph->check` 而非直接用 check：** IP 校验和以反码形式存储。`~iph->check` 将反码转回正常的 16 位部分和形式，便于后续的 csum16_add 增量更新。

### 2. IP 选项提取与验证

```c
    int options_len = ip_hdr_len - sizeof(*iph);  // sizeof(struct iphdr) = 20
    if (options_len > 0) {
        if (options_len < sizeof(struct opt_ext))
            return XDP_PASS;                      // 选项太短，不是我们的格式
        ext = (struct opt_ext *)(iph + 1);        // opt_ext 紧跟在 IP 头后面
        if (ext + 1 > data_end || !validate_opt_ext(ext))
            return XDP_PASS;                      // 越界或校验失败
        ext_daddr = opt_ext_get_daddr(ext);
        ext_saddr = opt_ext_get_saddr(ext);
    }
```

**validate_opt_ext() 验证三个条件：**
```c
static inline int validate_opt_ext(struct opt_ext *ext) {
    if (ext->_scode != OPT_EXT_SRC || ext->_dcode != OPT_EXT_DEST) return 0;
    if (ext->_slen != OPT_OPT_LEN || ext->_dlen != OPT_OPT_LEN) return 0;
    return 1;
}
```

### 3. 双向路由决策

```c
    if (daddr == OUTER_ADDR) {
        // ★ 外网 → 内网：数据包目标是外网地址（10.1.1.1）
        // 必须有 IP 选项，从中提取内网目标
        if (ingress_ifindex != WAN_IFINDEX) return XDP_PASS;  // 只信任 WAN 接口
        if (ext == NULL) return XDP_PASS;
        daddr = (INNER_PREFIX & OPT_ADDR_PREFIX_MASK) | (ext_daddr & OPT_ADDR_MASK);
        //      └── 10.1.2.0 & 0xFFFFFF00 = 10.1.2.0 ──┘ └── ext_daddr & 0xFF → 内网主机号
        redirect_ifindex = LAN_IFINDEX;
    }
    else if ((saddr & OPT_ADDR_PREFIX_MASK) == (INNER_PREFIX & OPT_ADDR_PREFIX_MASK)) {
        // ★ 内网 → 外网：数据包源 IP 匹配内网前缀（10.1.2.x）
        if (ingress_ifindex != LAN_IFINDEX) return XDP_PASS;  // 只信任 LAN 接口
        #ifdef ALWAYS_ALLOC_EXT
        if (ext == NULL) {
            alloc_space_for_ext(ctx);  // ★ 自动分配 IP 选项空间
        }
        #endif
        ext_saddr = saddr & OPT_ADDR_MASK;  // 提取内网主机号存入 ext
        saddr = OUTER_ADDR;                  // 源地址改为外网地址
        redirect_ifindex = WAN_IFINDEX;
    }
```

**双向路由的逻辑表：**

| 方向 | 判定条件 | 被改写字段 | 可选注入 | 出口 |
|------|---------|-----------|---------|------|
| 外→内 | `daddr == OUTER_ADDR (10.1.1.1)` | `daddr ← INNER_PREFIX \| ext_daddr` | 不需要 | LAN |
| 内→外 | `saddr 匹配 INNER_PREFIX (10.1.2.0/24)` | `saddr ← OUTER_ADDR`；`ext_saddr ← 原 saddr 低字节` | ALWAYS_ALLOC_EXT 时注入 | WAN |
| 不匹配 | — | 无 | — | XDP_PASS |

### 4. 自动 IP 选项空间分配（ALWAYS_ALLOC_EXT）

```c
#ifdef ALWAYS_ALLOC_EXT
int alloc_space_for_ext(struct xdp_md *ctx) {
    struct opt_ext ext;
    init_opt_ext(&ext);                    // 初始化为 0 的 opt_ext

    // Step 1: 用 bpf_xdp_adjust_head() 在数据包前面扩展 sizeof(opt_ext) 字节
    if (bpf_xdp_adjust_head(ctx, -(__s32)sizeof(struct opt_ext)))
        return 0;                          // 失败 → 放弃

    // Step 2: 将原有的 eth+iph 向前移动 sizeof(ext) 字节
    memmove((void *)(long)ctx->data,        // 目标：新 head
            (void *)(long)ctx->data + sizeof(ext),  // 源：旧 head
            sizeof(struct ethhdr) + sizeof(struct iphdr));

    // Step 3: 将 opt_ext 填入 eth 和 iph 之间的空隙（实际上是填到 IP 头后面）
    memmove((void *)(long)ctx->data + sizeof(struct ethhdr) + sizeof(struct iphdr),
            &ext, sizeof(ext));

    return 1;
}
#endif
```

**`bpf_xdp_adjust_head` 的工作原理：** 它将数据包的 `data` 指针向前（负偏移）或向后（正偏移）移动，从而在头部创建或删除空间。这里用负偏移创建 `sizeof(opt_ext)` 字节的新空间，然后用 `memmove` 将原数据前移，最后在 IP 头后面插入 opt_ext。

**关键限制：** `bpf_xdp_adjust_head` 只能调用一次。且新 head 不能超出数据包在内存中的分配边界（通常 headroom 为 256 字节，足够小选项的插入）。

### 5. 增量校验和更新（最复杂的部分）

```c
    // IP 校验和更新（3 个来源的修改）：
    //   1. saddr 改变（如果内→外方向）
    //   2. daddr 改变（如果外→内方向）
    //   3. tot_len 改变（如果注入了 IP 选项）
    //   4. IP 选项内容改变（如果 ext_saddr / ext_daddr 被修改）

    // L4 校验和更新：
    //   TCP 校验和：覆盖 TCP 伪头（含 src/dst IP）
    //   UDP 校验和：覆盖 UDP 伪头（同上）
    //   ICMP 错误消息：递归修改内嵌 IP 头的地址
```

**ICMP 错误消息的递归地址改写**（最精妙的部分）：

```c
if (iph->protocol == IPPROTO_ICMP) {
    struct icmphdr *icmph = (struct icmphdr *)((__u8 *)iph + ip_hdr_len);

    if (icmph->type == ICMP_DEST_UNREACH || icmph->type == ICMP_TIME_EXCEEDED ||
        icmph->type == ICMP_PARAMETERPROB || ...) {
        // ICMP 错误消息内部包含原始 IP 头的前 28+ 字节
        struct iphdr *inner_iph = (struct iphdr *)(icmph + 1);

        if (redirect_ifindex == LAN_IFINDEX) {
            // 外→内方向：内嵌 IP 头的 saddr（外网地址）需要改为内网地址
            if (bpf_ntohl(inner_iph->saddr) == OUTER_ADDR) {
                // 修改 inner_iph->saddr → 内网地址
                // 同时更新 inner_iph->check 和 icmph->checksum
            }
        } else if (redirect_ifindex == WAN_IFINDEX) {
            // 内→外方向：内嵌 IP 头的 daddr（内网地址）需要改为外网地址
            if ((bpf_ntohl(inner_iph->daddr) & OPT_ADDR_PREFIX_MASK) ==
                (INNER_PREFIX & OPT_ADDR_PREFIX_MASK)) {
                // 修改 inner_iph->daddr → OUTER_ADDR
            }
        }
    }
}
```

**为什么 ICMP 处理如此复杂：** traceroute 等工具依赖 ICMP Time Exceeded 消息。如果内嵌 IP 头中仍包含内网地址（如 10.1.2.x），外部 traceroute 发起方无法理解。递归改写保证所有层级的地址都一致。

---

## 简化版 optmap2.c 的差异分析

optmap2.c 与 optmap.c 的关键差别：

| 方面 | optmap.c | optmap2.c |
|------|----------|-----------|
| **方向判定** | `daddr == OUTER_ADDR` → 外→内 | `(daddr & PREFIX_MASK) == INNER_PREFIX` → 外→内 |
| **L4 协议** | TCP、UDP、ICMP | 仅 TCP (`if (iph->protocol != IPPROTO_TCP) return XDP_PASS;`) |
| **校验和** | `csum_opt_ext_addr_delta()` 增量更新（考虑所有字节位置） | 简化的逐字节校验和替换 |
| **ICMP 递归** | 支持 | 不支持 |
| **redirect** | `return bpf_redirect(ifindex, 0)` | `return XDP_PASS`（依赖内核路由） |

**如何选择版本：**
- 只需要 TCP 且理解原理 → optmap2.c（代码量少 40%，更清晰）
- 需要完整的 UDP/TCP/ICMP 支持和生产级校验和 → optmap.c

---

## ebpf.c —— 备选路由方案

`ebpf.c` 实现了另一种更通用的路由策略——不依赖预定义的地址映射，而是通过 eBPF 的 `bpf_fib_lookup()` 查询内核 FIB（Forwarding Information Base，转发信息库），并将查询结果缓存在 eBPF Map 中：

```c
struct {
    __uint(type, BPF_MAP_TYPE_LRU_HASH);   // LRU 自动老化
    __uint(max_entries, 100);
    __type(key, int);                      // key = daddr (IPv4)
    __type(value, struct rt_item);         // value = {ifindex, dmac, smac}
} rtcache_map SEC("maps");

// 处理流程：
// 1. 数据包有 IP 选项（ihl > 5） → 进入快速路径
//    a. 查 rtcache_map → 命中 → 直接 rewrite MAC + redirect
//    b. 查 bpf_fib_lookup() → 命中 → 插入 rtcache_map + redirect
//    c. 都失败 → XDP_PASS（回退到内核协议栈）
// 2. 数据包无 IP 选项 → XDP_PASS（走正常内核路由）
```

**这个方案的优势：**
- 不依赖硬编码的 `WAN_IFINDEX`/`LAN_IFINDEX`/`OUTER_ADDR` 等常量
- 自动适应路由表变化（`BPF_MAP_TYPE_LRU_HASH` 会自动老化过期的 FIB 条目）
- 可用于构建更通用的"基于 IP 选项的快速路径路由器"

---

## 测试工具详解

### send.c / send2.c —— UDP/TCP 发送器

```c
// send.c: UDP 发送器
int main(int argc, char *argv[]) {
    // Usage: ./send <destination IP> <destination opt>
    // 例如: ./send 10.1.1.1 5  → 发送到 10.1.1.1，扩展目标地址 = 5

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    // 设置 IP 选项（含扩展目标地址）
    struct opt_ext ext;
    init_opt_ext(&ext);
    opt_ext_set_daddr(&ext, (__u32)dest_opt);
    setsockopt(sock, IPPROTO_IP, IP_OPTIONS, &ext, sizeof(ext));

    // 发送
    sendto(sock, message, sizeof(message), 0, (struct sockaddr *)&dest, sizeof(dest));
}

// send2.c: TCP 发送器（逻辑相同，SOCK_STREAM + connect + send）
```

### recv.c / recv2.c —— UDP/TCP 接收器

```c
// recv.c: UDP 接收器，使用 recvmsg 获取 IP 选项
setsockopt(sockfd, IPPROTO_IP, IP_RECVOPTS, &opt, sizeof(opt));

// 通过 msg_control (ancillary data) 获取 IP 选项
recvmsg(sockfd, &msg, 0);
for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
    if (cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == IP_RECVOPTS) {
        // 打印接收到的 IP 选项的十六进制内容
    }
}
```

### mytcp.c —— LD_PRELOAD Socket Wrapper

```c
// 劫持 socket() 调用，自动为所有创建的 socket 设置 IP 选项
int socket(int domain, int type, int protocol) {
    int sockfd = __socket(domain, type, protocol);

    // 检查环境变量 DEST_EXT，如果设置了就注入 IP 选项
    const char *opt = getenv("DEST_EXT");
    if (opt) {
        int daddr = atoi(opt);
        struct opt_ext ext;
        init_opt_ext(&ext);
        opt_ext_set_daddr(&ext, (__u32)daddr);
        setsockopt(sockfd, IPPROTO_IP, IP_OPTIONS, &ext, sizeof(ext));
    }
    return sockfd;
}
```

**使用方法：**
```bash
DEST_EXT=5 LD_PRELOAD=./mytcp.so ./your_legacy_app
# 所有 socket 创建的连接都自动带上 ext_daddr=5
```

### addnat.py —— DNAT 规则生成

```python
for i in range(1000, 65535):
    os.system(f'iptables -t nat -A PREROUTING -i eth2 -p tcp --dport {i} '
              f'-j DNAT --to-destination 10.1.3.2:888')
```

将所有到达 eth2 的 TCP 端口 1000-65534 的流量 DNAT 到单一内网地址的 888 端口。用于测试 XDP 程序在大规模端口空间下的处理能力。

### parse_trace.py —— 性能日志解析

解析多种格式的日志文件，提取性能指标并计算统计分布（均值、中位数、P90、标准差）：

```
direction, count, avg_ns, median_ns, p90_ns, stddev_ns
```

---

## 编译完整指南

### 环境要求

```bash
# 系统依赖
sudo apt update && sudo apt install -y \
    build-essential clang llvm gcc g++ make \
    linux-headers-$(uname -r) \
    libbpf-dev bpftool \
    python3

# 验证 eBPF 支持
llc --version                  # LLVM 需要支持 BPF 目标
clang -target bpf -v 2>&1 | head -5  # 确认 BPF target 可用

# 验证内核 XDP 支持
uname -r                       # 需要 4.18+ (推荐 5.4+)
grep CONFIG_XDP /boot/config-$(uname -r)  # 应包含 CONFIG_XDP=y 或 =m
```

### 第一步：编译 eBPF 程序

```bash
cd optmap

# 编译复杂版（TCP+UDP+ICMP）
clang -O2 -target bpf -c optmap.c -o optmap.o \
    -I/usr/include/x86_64-linux-gnu \
    -DOPT_ADDR_EXT_BYTES=1

# 编译简化版（TCP-only）
clang -O2 -target bpf -c optmap2.c -o optmap2.o \
    -I/usr/include/x86_64-linux-gnu

# 编译备选路由版
clang -O2 -target bpf -c ebpf.c -o ebpf.o \
    -I/usr/include/x86_64-linux-gnu

# 验证编译产物
file optmap.o       # ELF 64-bit LSB relocatable, eBPF
llvm-objdump -h optmap.o | grep xdp_router  # 确认 section 存在
```

### 第二步：编译用户态测试工具

```bash
# UDP 测试工具
gcc -o send send.c
gcc -o recv recv.c

# TCP 测试工具
gcc -o send2 send2.c
gcc -o recv2 recv2.c

# LD_PRELOAD wrapper
gcc -shared -fPIC -o mytcp.so mytcp.c -ldl
```

### 第三步：验证 eBPF 程序

```bash
# 用 bpftool 检查程序
bpftool prog load optmap.o /sys/fs/bpf/optmap_test type xdp 2>&1
# 如果成功，程序可以加载到内核
bpftool prog show | grep optmap

# 清理测试加载
rm /sys/fs/bpf/optmap_test 2>/dev/null
```

### 常见编译错误

| 错误 | 原因 | 解决 |
|------|------|------|
| `fatal error: 'bpf/bpf_helpers.h' file not found` | libbpf-dev 未安装或路径不对 | `sudo apt install libbpf-dev`；或 `-I/usr/include/x86_64-linux-gnu` |
| `error: unknown target 'bpf'` | LLVM/Clang 版本过低 | 需要 Clang 6+，推荐 Clang 11+ |
| `could not build bpf program: Invalid argument` | eBPF 验证器拒绝程序 | 检查边界检查是否完整，或使用 `bpftool prog tracelog` 查看验证器错误 |
| `undefined reference to 'bpf_ktime_get_ns'` | 链接时未提供 BPF helper | 确保用 `-target bpf` 而不是直接链接 |
| `error: <linux/ip.h> redefinition` | 头文件冲突 | 使用 `-I` 指向正确的内核头文件路径 |

---

## 部署与运行

### 准备工作

```bash
# 确定接口名称和对应的 ifindex
ip link show
# 假设：eth1 (ifindex=3) = WAN, eth2 (ifindex=4) = LAN

# 开启 IPv4 转发
echo 1 | sudo tee /proc/sys/net/ipv4/conf/all/forwarding
echo 1 | sudo tee /proc/sys/net/ipv4/conf/all/accept_local

# 配置接口地址
sudo ip addr add 10.1.1.1/24 dev eth1    # 外网地址
sudo ip addr add 10.1.2.1/24 dev eth2    # 内网网关
```

### 部署 eBPF 程序

```bash
# 方法 A：使用 Makefile 中的 install target
sudo make install WAN_DEV=eth1 LAN_DEV=eth2

# 方法 B：手动执行
sudo ip link set dev eth1 xdp obj optmap.o sec xdp_router
sudo ip link set dev eth2 xdp obj optmap.o sec xdp_router

# 验证加载
sudo bpftool net list
# 应显示 eth1 和 eth2 上挂载了 xdp 程序

sudo ip link show eth1 | grep xdp
# 应包含 "xdp" 字样
```

### 卸载

```bash
# 方法 A：使用 Makefile
sudo make uninstall WAN_DEV=eth1 LAN_DEV=eth2

# 方法 B：手动
sudo ip link set dev eth1 xdp off
sudo ip link set dev eth2 xdp off
```

### 端到端测试

```bash
# 终端 1：在外网节点上启动接收器
./recv          # 监听 UDP 12345
# 或
./recv2         # 监听 TCP 12345

# 终端 2：在内网节点上启动发送器
LD_PRELOAD=./mytcp.so DEST_EXT=5 ./send2 10.1.1.1 5
# 10.1.1.1 是外网地址（OUTER_ADDR）
# DEST_EXT=5 和最后的 5 都是 ext_daddr（内网目标主机号）

# 终端 3：监控 XDP 处理统计
sudo bpftool prog show
sudo bpftool prog tracelog
```

### 性能测试

```bash
# XDP 处理延迟（通过 bpftrace 或 eBPF tracepoint）
# 在 optmap.c 中已有 bpf_ktime_get_ns() 计时代码
# 用 parse_trace.py 解析日志

# 吞吐量测试（使用 iperf3 或 pktgen）
# 在内网节点：
iperf3 -c 10.1.1.1 -t 30    # 经过 XDP NAT 的 TCP 吞吐量
# 对照组：关闭 XDP
sudo ip link set dev eth1 xdp off
iperf3 -c 10.1.1.1 -t 30    # 无 XDP 的内核 NAT 吞吐量
```

---

## 实验定制指南

### 实验 1：修改扩展地址大小

```bash
# 从 1 字节扩展改为 2 字节（支持 65536 个内网地址）
clang -O2 -target bpf -c optmap.c -o optmap.o \
    -DOPT_ADDR_EXT_BYTES=2 \
    -I/usr/include/x86_64-linux-gnu

# 重新部署
sudo ip link set dev eth1 xdp off
sudo ip link set dev eth1 xdp obj optmap.o sec xdp_router
```

### 实验 2：修改网络拓扑常量

```c
// 编辑 optmap.h，修改：
#define WAN_IFINDEX 5       // 改成你的实际值
#define LAN_IFINDEX 6
#define OUTER_ADDR 0xc0a80101  // 192.168.1.1
#define INNER_PREFIX 0xc0a80200  // 192.168.2.0/24
```

### 实验 3：测量 ALWAYS_ALLOC_EXT 的性能开销

```bash
# 编译两个版本：
# A. 带 ALWAYS_ALLOC_EXT
clang -O2 -target bpf -c optmap.c -o optmap_alloc.o -DALWAYS_ALLOC_EXT=1

# B. 不带 ALWAYS_ALLOC_EXT
clang -O2 -target bpf -c optmap.c -o optmap_noalloc.o

# 分别部署并测量吞吐量差异
```

### 实验 4：添加对 IPSWEN level 的支持

```c
// 在 opt_ext 中增加 level 字段
struct opt_ext_v2 {
    __u8 _scode, _slen;
    __s8 level;                          // ★ 新增：有符号 level
    __u8 saddr[OPT_ADDR_EXT_BYTES];
    __u8 _dcode, _dlen;
    __u8 daddr[OPT_ADDR_EXT_BYTES];
};

// 在路由决策中考虑 level
if (ext->level > 0) {
    // 前向扩展：在内网地址后附加 level 字节
    // ...
} else if (ext->level < 0) {
    // 后向扩展：在内网地址前附加 |level| 字节
    // ...
}
```

### 实验 5：实现 XDP 级别的连接跟踪

```c
// 在 eBPF Map 中存储连接状态
struct {
    __uint(type, BPF_MAP_TYPE_LRU_HASH);
    __uint(max_entries, 10000);
    __type(key, struct conn_key);    // {saddr, daddr, sport, dport, proto}
    __type(value, struct conn_state); // {state, last_seen, ...}
} conntrack SEC("maps");
```

---

## eBPF/XDP 技术背景

### XDP 数据通路位置

```
  ┌─────────────┐
  │     NIC     │
  └──────┬──────┘
         │ DMA
  ┌──────▼──────────────┐
  │   Driver (RX ring)  │
  └──────┬──────────────┘
         │
  ┌──────▼──────┐  ← XDP 程序在此执行（最早路径）
  │   XDP hook  │     数据包还在驱动层的线性内存中
  └──────┬──────┘     没有 sk_buff 分配
         │
  ┌──────▼──────────────┐
  │  Kernel Network     │
  │  Stack (sk_buff)    │  ← XDP_PASS 才到这里
  └─────────────────────┘
```

### XDP 返回码

| 返回码 | 含义 | optmap 使用场景 |
|--------|------|----------------|
| `XDP_PASS` | 交给内核协议栈 | 不匹配的数据包、非 IP 包 |
| `XDP_REDIRECT` | 重定向到其他网卡 | 匹配的数据包（外→内 或 内→外） |
| `XDP_DROP` | 丢弃 | 未使用 |
| `XDP_TX` | 从同一网卡发回 | 未使用 |
| `XDP_ABORTED` | 错误，触发 tracepoint | 未使用 |

### eBPF 验证器限制

XDP 程序受到 eBPF 验证器的严格限制：
- 最大指令数：100 万条（5.4 内核），optmap.c 约 500 条
- 必须对所有分支做边界检查（所有指针解引用前都有 `> data_end` 检查）
- 不能使用无限循环（每行 `for` 循环用 `#pragma clang loop unroll(full)` 展开）
- 不能调用任意内核函数，只能使用 `bpf_helper_defs.h` 中列出的辅助函数

---

## 注意事项

- **ifindex 是硬编码的：** 修改接口名或重新插拔网卡可能导致 ifindex 变化。用 `ip link show` 确认实际值后更新 `optmap.h`
- **仅支持 24 位前缀：** `INNER_PREFIX` 假设为 `/24`。如果需要 `/16` 或 `/23`，需要修改 `OPT_ADDR_PREFIX_MASK` 的计算公式
- **eBPF 程序不能有全局可变状态：** 所有共享状态必须存储在 eBPF Map 中
- **XDP 程序在驱动层运行：** 崩溃会导致数据包丢失，但不会导致内核 panic
- **不支持 GRO/GSO：** 分段数据包无法被 XDP 处理（因为 GRO 在 XDP 之后）
- **仅处理 IPv4：** 程序开头检查 `eth->h_proto != ETH_P_IP`，不处理 IPv6
- **bpf_printk 性能影响：** 开启 `bpf_printk`（取消 `#define bpf_printk(...)` 的注释）会导致严重的性能下降，仅用于调试

---

## 相关项目

- **fakedns** —— LD_PRELOAD 层的 DNS 代理，可自动为应用程序注入 IP 选项（`inject_dest_ext_option()`）
- **happyfootball** —— Happy Eyeballs + IP 选项的多路径连接实验，optmap 可作为其底层转发层
- **iproute2-5.5.0** —— IPSWEN 增强版 `ip route`，可设置 XDP 配合使用的路由
- **linux-kernel** —— IPSWEN 内核，提供 `bpf_fib_lookup` 所需的 FIB 扩展

---

## 附录 A：快速操作卡片

### 编译
```bash
clang -O2 -target bpf -c optmap.c -o optmap.o
gcc -o send send.c && gcc -o recv recv.c
gcc -o send2 send2.c && gcc -o recv2 recv2.c
gcc -shared -fPIC -o mytcp.so mytcp.c -ldl
```

### 部署
```bash
sudo ip link set dev eth1 xdp obj optmap.o sec xdp_router
sudo ip link set dev eth2 xdp obj optmap.o sec xdp_router
```

### 卸载
```bash
sudo ip link set dev eth1 xdp off
sudo ip link set dev eth2 xdp off
```

### 测试
```bash
# UDP
./recv &  # 外网节点
LD_PRELOAD=./mytcp.so DEST_EXT=5 ./send 10.1.1.1 5  # 内网节点

# TCP
./recv2 &
LD_PRELOAD=./mytcp.so DEST_EXT=5 ./send2 10.1.1.1 5
```

### 文件位置速查
| 内容 | 路径 |
|------|------|
| 核心头文件 | `optmap/optmap.h` |
| XDP 主程序 | `optmap/optmap.c` |
| XDP 简化版 | `optmap/optmap2.c` |
| FIB 缓存版 | `optmap/ebpf.c` |
| UDP 发送器 | `optmap/send` |
| UDP 接收器 | `optmap/recv` |
| TCP 发送器 | `optmap/send2` |
| TCP 接收器 | `optmap/recv2` |
| Socket wrapper | `optmap/mytcp.so` |
| 编译产物 | `optmap/optmap.o` |
| 性能日志解析 | `optmap/parse_trace.py` |

## 附录 B：校验和增量更新速查

```c
// 替换 IP 地址（32 位）
csum = csum_replace_u32(csum, old_ip, new_ip);

// 替换 TCP/UDP 伪头中的 IP 地址
__u32 addr_delta = ~old_ip + new_ip + carry;
tcp_csum = csum16_add(tcp_csum, addr_delta);
udp_csum = csum16_add(udp_csum, addr_delta);

// 替换 IP 总长度
csum = csum_replace_u16(csum, old_tot_len, new_tot_len);

// 重新计算整个 IP 校验和（不推荐，低于增量更新的性能）
iph->check = 0;
iph->check = ip_fast_csum((__u8 *)iph, iph->ihl);  // 仅在非 XDP 路径可用
```

## 附录 C：eBPF Map 类型选择指南

| Map 类型 | 特性 | 适用场景 |
|---------|------|---------|
| `BPF_MAP_TYPE_HASH` | 通用哈希表，无自动过期 | 需要持久化的状态 |
| `BPF_MAP_TYPE_LRU_HASH` | 带 LRU 自动过期 | 缓存类数据（如 FIB 缓存） |
| `BPF_MAP_TYPE_ARRAY` | 固定大小数组，最快 | 预定义规则的直接索引 |
| `BPF_MAP_TYPE_PERCPU_HASH` | 每 CPU 独立哈希表 | 无锁的每 CPU 计数器 |
| `BPF_MAP_TYPE_LPM_TRIE` | 最长前缀匹配树 | 路由表（替代 FIB lookup） |
