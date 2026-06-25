# optmap — eBPF/XDP-based IP Option Routing and Fast NAT Gateway / optmap —— 基于 eBPF/XDP 的 IP 选项路由和快速转发 NAT 网关

## Overview / 概述

`optmap` is an **eBPF/XDP (eXpress Data Path)**-based kernel-level packet routing and Network Address Translation (NAT) framework. It leverages the Linux kernel's XDP technology to intercept packets at the NIC driver layer (the earliest path after packet reception, before the kernel network stack), performs wire-speed packet rewriting and forwarding based on **custom IP options** (extended source/destination addresses encoded in the `opt_ext` structure), and implements an ultra-low-latency gateway that bypasses the kernel network stack (zero sk_buff allocation).

`optmap` 是一个基于 **eBPF/XDP（eXpress Data Path）** 的内核级数据包路由与网络地址转换（NAT）框架。它利用 Linux 内核的 XDP 技术在网络驱动层（NIC 收包后、内核协议栈之前的最早路径）拦截数据包，根据**自定义 IP 选项**（`opt_ext` 结构体编码的扩展源/目标地址）执行线速的数据包改写和转发，实现了一个不经过内核协议栈（零 sk_buff 分配）的超低延迟网关。

**Core Research Questions / 核心研究问题：**
1. Can deterministic packet routing be achieved based on custom IP options (instead of routing tables or NAT tables)? / 能否基于自定义 IP 选项（而非路由表或 NAT 表）实现确定性的数据包路由
2. What level of throughput and latency can address rewriting achieve at the earliest kernel path (XDP)? / 在内核最早路径（XDP）上做地址改写能达到什么级别的吞吐量和延迟
3. When packets do not have preset IP options, the feasibility and performance impact of automatically allocating extended address space (`ALWAYS_ALLOC_EXT`) / 当数据包没有预置 IP 选项时，自动分配扩展地址空间（`ALWAYS_ALLOC_EXT`）的可行性和性能影响
4. The actual performance of combining the eBPF helper function `bpf_fib_lookup` with XDP redirect as a replacement for traditional iptables NAT / eBPF 辅助函数 `bpf_fib_lookup` 与 XDP redirect 组合使用替代传统 iptables NAT 的实际表现

**Design Differences Between the Two Versions / 两个版本的设计差异：**

| Feature / 特性 | optmap.c (Full Version / 复杂版) | optmap2.c (Simplified Version / 简化版) |
|------|---------------------|----------------------|
| Route Direction Decision / 路由方向判定 | WAN→LAN: daddr==OUTER_ADDR; LAN→WAN: saddr matches INNER_PREFIX / 外→内：daddr==OUTER_ADDR；内→外：saddr 匹配 INNER_PREFIX | WAN→LAN: daddr matches INNER_PREFIX; LAN→WAN: saddr==OUTER_ADDR / 外→内：daddr 匹配 INNER_PREFIX；内→外：saddr==OUTER_ADDR |
| L4 Protocol Support / L4 协议支持 | TCP, UDP, ICMP (including recursive rewriting of embedded IP headers in ICMP error messages) / TCP、UDP、ICMP（含 ICMP 错误消息中内嵌 IP 头的递归改写） | TCP only / 仅 TCP |
| Checksum Engineering / 校验和工程 | Full incremental checksum (csum_opt_ext_addr_delta) / 完整的增量校验和（csum_opt_ext_addr_delta） | Simplified checksum recalculation / 简化的校验和重算 |
| ICMP Error Handling / ICMP 错误处理 | Recursive modification of embedded IP header addresses / 递归修改内嵌 IP 头地址 | Not supported / 不支持 |
| Purpose / 用途 | Production-grade general-purpose NAT gateway / 生产级通用 NAT 网关 | TCP-only lightweight demonstration / TCP-only 轻量级演示 |

**Why Keep Both Versions / 为什么同时保留两个版本：** optmap2.c is an early proof-of-concept (more concise and easier to understand), while optmap.c is the full version built upon it with UDP/ICMP support and robust checksum handling. Both have independent research value — optmap2 is better suited for teaching and learning eBPF concepts, while optmap is more suitable for actual deployment and performance measurement.

optmap2.c 是早期的概念验证（更简洁、更易理解），optmap.c 是在其基础上增加了 UDP/ICMP 支持和健壮的校验和处理的完整版。两者均有独立的研究价值——optmap2 更适合教学和学习 eBPF 概念，optmap 更适合实际部署和性能测量。

---

## Directory Structure / 目录结构

```
optmap/
├── optmap.h                # ★ Core header: opt_ext struct, address masks, checksum utilities / ★ 核心头文件：opt_ext 结构体、地址掩码、校验和工具函数
├── optmap.c                # ★ eBPF/XDP main program (full version: TCP+UDP+ICMP+full checksum) / ★ eBPF/XDP 主程序（复杂版：TCP+UDP+ICMP+完整校验和）
├── optmap.o                # Build artifact (eBPF bytecode ELF) / 编译产物（eBPF 字节码 ELF）
│
├── optmap2.c               # eBPF/XDP simplified version (TCP-only) / eBPF/XDP 简化版（TCP-only）
├── optmap2.o               # Build artifact / 编译产物
│
├── ebpf.c                  # eBPF/XDP alternative routing: FIB cache-based routing / eBPF/XDP 备选路由方案：基于 FIB 缓存的路由
├── ebpf.o                  # Build artifact / 编译产物
│
├── mynat.c                 # eBPF/XDP minimal NAT example (address rewrite only, no routing) / eBPF/XDP 最小 NAT 示例（仅修改地址，不查路由）
│
├── send.c / send           # UDP send test tool (IPv4 UDP sender with IP options) / UDP 发送测试工具（设置 IP 选项的 IPv4 UDP 发送器）
├── recv.c / recv           # UDP receive test tool (IPv4 UDP receiver with IP options) / UDP 接收测试工具（接收 IP 选项的 IPv4 UDP 接收器）
├── send2.c / send2         # TCP send test tool (IPv4 TCP client with IP options) / TCP 发送测试工具（设置 IP 选项的 IPv4 TCP 客户端）
├── recv2.c / recv2         # TCP receive test tool (IPv4 TCP server) / TCP 接收测试工具（IPv4 TCP 服务器）
│
├── mytcp.c / mytcp.so      # LD_PRELOAD TCP socket wrapper (auto-inject IP options) / LD_PRELOAD TCP socket wrapper（自动注入 IP 选项）
├── addnat.py               # iptables DNAT rule batch generation script / iptables DNAT 规则批量生成脚本
├── parse_trace.py          # eBPF trace log parsing and statistical analysis / eBPF trace log 解析和统计分析
│
├── 1.cap / 2.cap           # tcpdump capture files (experiment records) / tcpdump 抓包文件（实验记录）
└── Makefile                # Build and deployment script / 编译和部署脚本
```

---

## Core Header File Detail: optmap.h / 核心头文件详解：optmap.h

### Option Type Definitions / 选项类型定义

```c
#define OPT_EXT_SRC  233   // Source address extension option type / 源地址扩展选项类型号
#define OPT_EXT_DEST 241   // Destination address extension option type / 目标地址扩展选项类型号
```

These two custom option type numbers (233 and 241) are in the unallocated range of the IANA IP Option Numbers space and do not conflict with any standard IP options. The bit layout of the option type byte follows RFC 791:

这两个自定义选项类型号（233 和 241）在 IANA IP Option Numbers 空间中属于未分配范围，不与任何标准 IP 选项冲突。选项类型字节的位布局遵循 RFC 791：

```
OPT_EXT_SRC  = 233 = 0xE9 = 11101001
  Bit 7 (0x80): Copy-on-Fragmentation = 1 (all fragments carry this option) / (所有分片携带此选项)
  Bit 6-5:      Option Class = 2 (Debug/Measurement)
  Bit 4-0:      Option Number = 9

OPT_EXT_DEST = 241 = 0xF1 = 11110001
  Bit 7 (0x80): Copy-on-Fragmentation = 1
  Bit 6-5:      Option Class = 2
  Bit 4-0:      Option Number = 17
```

### Configurable Constants / 可配置常数

```c
#ifndef OPT_ADDR_EXT_BYTES
#define OPT_ADDR_EXT_BYTES 1    // Default: 1 byte of extended address per direction / 默认每个方向的扩展地址占 1 字节
#endif

#if OPT_ADDR_EXT_BYTES < 1 || OPT_ADDR_EXT_BYTES > 3
#error "OPT_ADDR_EXT_BYTES must be 1, 2 or 3"
#endif

#define OPT_ADDR_EXT_BITS   (OPT_ADDR_EXT_BYTES * 8)       // Extended address bits / 扩展地址的位数
#define OPT_ADDR_MASK       ((__u32)((1ULL << OPT_ADDR_EXT_BITS) - 1ULL))  // e.g., 0xFF (1 byte) / 如 0xFF (1 字节)
#define OPT_ADDR_PREFIX_MASK ((__u32)(~OPT_ADDR_MASK))     // e.g., 0xFFFFFF00 / 如 0xFFFFFF00
```

**Design Space of OPT_ADDR_EXT_BYTES / OPT_ADDR_EXT_BYTES 的设计空间：**

| Value / 值 | Extended Address Bits / 扩展地址位数 | Addresses Encodable Per Direction / 每个方向可编码的地址数 | Use Case / 实际用途 |
|----|-------------|----------------------|---------|
| 1 | 8 bits | 256 | Small testbed (default) / 小型测试床（默认） |
| 2 | 16 bits | 65536 | Medium deployment / 中型部署 |
| 3 | 24 bits | 16M | Large network / 大型网络 |

### opt_ext Structure Memory Layout / opt_ext 结构体内存布局

```c
struct opt_ext {
    __u8 _scode;                     // 0xE9 (OPT_EXT_SRC)
    __u8 _slen;                      // 0x03 (OPT_OPT_LEN = 2 + 1 = 3)
    __u8 saddr[OPT_ADDR_EXT_BYTES];  // Source address extended bytes / 源地址扩展字节
    __u8 _dcode;                     // 0xF1 (OPT_EXT_DEST)
    __u8 _dlen;                      // 0x03
    __u8 daddr[OPT_ADDR_EXT_BYTES];  // Destination address extended bytes / 目标地址扩展字节
#if OPT_EXT_PAD_LEN == 2
    __u8 pad0;                       // Alignment padding (needs 2 bytes when total length % 4 == 2) / 对齐填充（当总长度 % 4 == 2 时需要 2 字节）
    __u8 pad1;
#endif
};
```

**Byte layout in memory (with OPT_ADDR_EXT_BYTES=1 as example) / 内存中的字节布局（以 OPT_ADDR_EXT_BYTES=1 为例）：**
```
偏移  0    1    2    3    4    5    6    7
    +----+----+----+----+----+----+----+----+
    |E9  |03  |s[0]|F1  |03  |d[0]|00  |00  |  总长 8 字节
    +----+----+----+----+----+----+----+----+
     \_scode   \_saddr   \_daddr    \_pad
           \__slen      \__dlen
```

The total length of IP options must be a multiple of 4 bytes (RFC 791 requirement). Calculation of `OPT_EXT_PAD_LEN`:
```c
#define OPT_EXT_RAW_LEN (2 * OPT_OPT_LEN_RAW)   // 2 * (2 + OPT_ADDR_EXT_BYTES)
#define OPT_EXT_PAD_LEN ((4 - (OPT_EXT_RAW_LEN & 0x3)) & 0x3)
// When OPT_ADDR_EXT_BYTES=1: RAW_LEN=6, PAD_LEN=2 (6→8) / 当 OPT_ADDR_EXT_BYTES=1: RAW_LEN=6, PAD_LEN=2 (6→8)
// When OPT_ADDR_EXT_BYTES=2: RAW_LEN=8, PAD_LEN=0 (8→8 already aligned) / 当 OPT_ADDR_EXT_BYTES=2: RAW_LEN=8, PAD_LEN=0 (8→8 已对齐)
// When OPT_ADDR_EXT_BYTES=3: RAW_LEN=10, PAD_LEN=2 (10→12) / 当 OPT_ADDR_EXT_BYTES=3: RAW_LEN=10, PAD_LEN=2 (10→12)
```

### Network Configuration Constants / 网络配置常量

```c
#define WAN_IFINDEX 3       // WAN interface ifindex (obtained via if_nametoindex) / 外网接口的 ifindex（通过 if_nametoindex 获取）
#define LAN_IFINDEX 4       // LAN interface ifindex / 内网接口的 ifindex
#define OUTER_ADDR 0x0a010101   // WAN address 10.1.1.1 (host order) / 外网地址 10.1.1.1（host order）
// #define OUTER_ADDR 0xc0a80201  // Alternative: 192.168.2.1 / 备选：192.168.2.1
#define INNER_PREFIX 0x0a010200 // LAN prefix 10.1.2.0/24 (only supports 24-bit prefix) / 内网前缀 10.1.2.0/24（仅支持 24 位前缀）
```

**How to determine ifindex / ifindex 如何确定：**
```bash
# Method 1: Check /sys/class/net/ / 方法 1：查看 /sys/class/net/
cat /sys/class/net/eth1/ifindex   # Output: 3 / 输出 3
cat /sys/class/net/eth2/ifindex   # Output: 4 / 输出 4

# Method 2: Use ip command / 方法 2：使用 ip 命令
ip link show eth1 | head -1       # Output: "3: eth1: ..." / 输出 "3: eth1: ..."
```

**Why ifindex instead of interface name / 为什么用 ifindex 而非接口名：** eBPF's `bpf_redirect()` and `bpf_fib_lookup()` use `ifindex` (integer), not the interface name string. Integer comparison is much faster than string comparison, and the eBPF verifier imposes fewer restrictions on integer operations.

为什么用 ifindex 而非接口名： eBPF 的 `bpf_redirect()` 和 `bpf_fib_lookup()` 使用 `ifindex`（整数），而非接口名字符串。int 比较比字符串比较快得多，且 eBPF 验证器对整数操作的限制更少。

### Checksum Utility Functions / 校验和工具函数

```c
static inline __u16 csum16_add(__u16 csum, __u16 addend) {
    csum += addend;
    return csum + (csum < addend);  // Handle carry / 处理进位（carry）
}
```

IP checksum (RFC 1071) is a 16-bit one's complement sum. The mathematical principle of incremental checksum modification:

IP 校验和（RFC 1071）是 16 位反码和。增量修改校验和的数学原理：

```
new_csum = ~(~old_csum + ~old_value + new_value)
         = old_csum_partial + ~old_value + new_value (带进位回卷)
```

`csum16_add` implements "16-bit addition with carry wrap-around", which is the core primitive for incremental checksum updates.

`csum16_add` 实现了"带进位回卷的 16 位加法"，是增量校验和更新的核心原语。

```c
static inline __u16 csum_replace_u32(__u16 csum, __u32 old, __u32 new) {
    csum = csum16_add(csum, ~(__u16)(old >> 16));  // Remove high 16 bits of old value / 移除旧值的高 16 位
    csum = csum16_add(csum, ~(__u16)old);           // Remove low 16 bits of old value / 移除旧值的低 16 位
    csum = csum16_add(csum, (__u16)(new >> 16));    // Add high 16 bits of new value / 加入新值的高 16 位
    csum = csum16_add(csum, (__u16)new);            // Add low 16 bits of new value / 加入新值的低 16 位
    return csum;
}
```

---

## Core XDP Program Source Analysis: optmap.c / 核心 XDP 程序源码解析：optmap.c

### 1. Program Entry and Packet Parsing / 程序入口与数据包解析

```c
SEC("xdp_router")
int xdp_route_handler(struct xdp_md *ctx) {
    // ★ XDP context only contains 3 fields: / ★ XDP 上下文只包含 3 个字段：
    //   ctx->data          → Packet start pointer / → 数据包起始指针
    //   ctx->data_end      → Packet end pointer / → 数据包末尾指针
    //   ctx->ingress_ifindex → Ingress interface ifindex / → 收包接口的 ifindex

    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    // Parse layer by layer, boundary check at each layer (eBPF verifier requirement) / 逐层解析，每层都做边界检查（eBPF 验证器强制要求）
    struct ethhdr *eth = data;
    if (eth + 1 > data_end) return XDP_PASS;     // Ethernet header incomplete → pass / 以太网头不完整 → 放行
    if (eth->h_proto != bpf_htons(ETH_P_IP))
        return XDP_PASS;                          // Not IPv4 → pass / 非 IPv4 → 放行

    struct iphdr *iph = data + sizeof(*eth);
    if (iph + 1 > data_end) return XDP_PASS;     // IP header incomplete / IP 头不完整
    int ip_hdr_len = iph->ihl * 4;               // IHL unit is 4 bytes / IHL 单位是 4 字节
    __u16 csum = ~iph->check;                    // Invert checksum → get partial sum / 对校验和取反 → 得到部分和
```

**Why use `~iph->check` instead of directly using check / 为什么用 `~iph->check` 而非直接用 check：** IP 校验和以反码形式存储。`~iph->check` 将反码转回正常的 16 位部分和形式，便于后续的 csum16_add 增量更新。

### 2. IP Option Extraction and Validation / IP 选项提取与验证

```c
    int options_len = ip_hdr_len - sizeof(*iph);  // sizeof(struct iphdr) = 20
    if (options_len > 0) {
        if (options_len < sizeof(struct opt_ext))
            return XDP_PASS;                      // Option too short, not our format / 选项太短，不是我们的格式
        ext = (struct opt_ext *)(iph + 1);        // opt_ext immediately follows IP header / opt_ext 紧跟在 IP 头后面
        if (ext + 1 > data_end || !validate_opt_ext(ext))
            return XDP_PASS;                      // Out of bounds or validation failed / 越界或校验失败
        ext_daddr = opt_ext_get_daddr(ext);
        ext_saddr = opt_ext_get_saddr(ext);
    }
```

**validate_opt_ext() validates three conditions / validate_opt_ext() 验证三个条件：**
```c
static inline int validate_opt_ext(struct opt_ext *ext) {
    if (ext->_scode != OPT_EXT_SRC || ext->_dcode != OPT_EXT_DEST) return 0;
    if (ext->_slen != OPT_OPT_LEN || ext->_dlen != OPT_OPT_LEN) return 0;
    return 1;
}
```

### 3. Bidirectional Route Decision / 双向路由决策

```c
    if (daddr == OUTER_ADDR) {
        // ★ WAN → LAN: packet destination is WAN address (10.1.1.1) / ★ 外网 → 内网：数据包目标是外网地址（10.1.1.1）
        // Must have IP options, extract LAN destination from them / 必须有 IP 选项，从中提取内网目标
        if (ingress_ifindex != WAN_IFINDEX) return XDP_PASS;  // Only trust WAN interface / 只信任 WAN 接口
        if (ext == NULL) return XDP_PASS;
        daddr = (INNER_PREFIX & OPT_ADDR_PREFIX_MASK) | (ext_daddr & OPT_ADDR_MASK);
        //      └── 10.1.2.0 & 0xFFFFFF00 = 10.1.2.0 ──┘ └── ext_daddr & 0xFF → LAN host number / → 内网主机号
        redirect_ifindex = LAN_IFINDEX;
    }
    else if ((saddr & OPT_ADDR_PREFIX_MASK) == (INNER_PREFIX & OPT_ADDR_PREFIX_MASK)) {
        // ★ LAN → WAN: packet source IP matches LAN prefix (10.1.2.x) / ★ 内网 → 外网：数据包源 IP 匹配内网前缀（10.1.2.x）
        if (ingress_ifindex != LAN_IFINDEX) return XDP_PASS;  // Only trust LAN interface / 只信任 LAN 接口
        #ifdef ALWAYS_ALLOC_EXT
        if (ext == NULL) {
            alloc_space_for_ext(ctx);  // ★ Automatically allocate IP option space / ★ 自动分配 IP 选项空间
        }
        #endif
        ext_saddr = saddr & OPT_ADDR_MASK;  // Extract LAN host number into ext / 提取内网主机号存入 ext
        saddr = OUTER_ADDR;                  // Change source address to WAN address / 源地址改为外网地址
        redirect_ifindex = WAN_IFINDEX;
    }
```

**Bidirectional Routing Logic Table / 双向路由的逻辑表：**

| Direction / 方向 | Decision Condition / 判定条件 | Rewritten Fields / 被改写字段 | Optional Injection / 可选注入 | Egress / 出口 |
|------|---------|-----------|---------|------|
| WAN→LAN / 外→内 | `daddr == OUTER_ADDR (10.1.1.1)` | `daddr ← INNER_PREFIX \| ext_daddr` | Not needed / 不需要 | LAN |
| LAN→WAN / 内→外 | `saddr matches INNER_PREFIX (10.1.2.0/24)` / `saddr 匹配 INNER_PREFIX (10.1.2.0/24)` | `saddr ← OUTER_ADDR`; `ext_saddr ← low byte of original saddr` / `saddr ← OUTER_ADDR`；`ext_saddr ← 原 saddr 低字节` | Injected when ALWAYS_ALLOC_EXT / ALWAYS_ALLOC_EXT 时注入 | WAN |
| No match / 不匹配 | — | None / 无 | — | XDP_PASS |

### 4. Automatic IP Option Space Allocation (ALWAYS_ALLOC_EXT) / 自动 IP 选项空间分配（ALWAYS_ALLOC_EXT）

```c
#ifdef ALWAYS_ALLOC_EXT
int alloc_space_for_ext(struct xdp_md *ctx) {
    struct opt_ext ext;
    init_opt_ext(&ext);                    // Initialize opt_ext to 0 / 初始化为 0 的 opt_ext

    // Step 1: Use bpf_xdp_adjust_head() to extend sizeof(opt_ext) bytes at packet front
    // Step 1: 用 bpf_xdp_adjust_head() 在数据包前面扩展 sizeof(opt_ext) 字节
    if (bpf_xdp_adjust_head(ctx, -(__s32)sizeof(struct opt_ext)))
        return 0;                          // Failure → abandon / 失败 → 放弃

    // Step 2: Move original eth+iph forward by sizeof(ext) bytes / Step 2: 将原有的 eth+iph 向前移动 sizeof(ext) 字节
    memmove((void *)(long)ctx->data,        // Destination: new head / 目标：新 head
            (void *)(long)ctx->data + sizeof(ext),  // Source: old head / 源：旧 head
            sizeof(struct ethhdr) + sizeof(struct iphdr));

    // Step 3: Fill opt_ext into the gap between eth and iph (actually after IP header)
    // Step 3: 将 opt_ext 填入 eth 和 iph 之间的空隙（实际上是填到 IP 头后面）
    memmove((void *)(long)ctx->data + sizeof(struct ethhdr) + sizeof(struct iphdr),
            &ext, sizeof(ext));

    return 1;
}
#endif
```

**How `bpf_xdp_adjust_head` works / `bpf_xdp_adjust_head` 的工作原理：** 它将数据包的 `data` 指针向前（负偏移）或向后（正偏移）移动，从而在头部创建或删除空间。这里用负偏移创建 `sizeof(opt_ext)` 字节的新空间，然后用 `memmove` 将原数据前移，最后在 IP 头后面插入 opt_ext。

**Key limitation / 关键限制：** `bpf_xdp_adjust_head` can only be called once. Additionally, the new head must not exceed the packet's allocated memory boundary (typically headroom is 256 bytes, sufficient for small option insertion).

关键限制： `bpf_xdp_adjust_head` 只能调用一次。且新 head 不能超出数据包在内存中的分配边界（通常 headroom 为 256 字节，足够小选项的插入）。

### 5. Incremental Checksum Update (The Most Complex Part) / 增量校验和更新（最复杂的部分）

```c
    // IP checksum updates (4 sources of modification): / IP 校验和更新（3 个来源的修改）：
    //   1. saddr changed (if LAN→WAN direction) / saddr 改变（如果内→外方向）
    //   2. daddr changed (if WAN→LAN direction) / daddr 改变（如果外→内方向）
    //   3. tot_len changed (if IP options injected) / tot_len 改变（如果注入了 IP 选项）
    //   4. IP option content changed (if ext_saddr / ext_daddr modified) / IP 选项内容改变（如果 ext_saddr / ext_daddr 被修改）

    // L4 checksum updates: / L4 校验和更新：
    //   TCP checksum: covers TCP pseudo-header (includes src/dst IP) / TCP 校验和：覆盖 TCP 伪头（含 src/dst IP）
    //   UDP checksum: covers UDP pseudo-header (same as above) / UDP 校验和：覆盖 UDP 伪头（同上）
    //   ICMP error messages: recursively modify embedded IP header addresses / ICMP 错误消息：递归修改内嵌 IP 头的地址
```

**Recursive address rewriting for ICMP error messages (the most intricate part) / ICMP 错误消息的递归地址改写**（最精妙的部分）：

```c
if (iph->protocol == IPPROTO_ICMP) {
    struct icmphdr *icmph = (struct icmphdr *)((__u8 *)iph + ip_hdr_len);

    if (icmph->type == ICMP_DEST_UNREACH || icmph->type == ICMP_TIME_EXCEEDED ||
        icmph->type == ICMP_PARAMETERPROB || ...) {
        // ICMP error message contains first 28+ bytes of original IP header
        // ICMP 错误消息内部包含原始 IP 头的前 28+ 字节
        struct iphdr *inner_iph = (struct iphdr *)(icmph + 1);

        if (redirect_ifindex == LAN_IFINDEX) {
            // WAN→LAN direction: inner IP header saddr (WAN address) needs to change to LAN address
            // 外→内方向：内嵌 IP 头的 saddr（外网地址）需要改为内网地址
            if (bpf_ntohl(inner_iph->saddr) == OUTER_ADDR) {
                // Modify inner_iph->saddr → LAN address / 修改 inner_iph->saddr → 内网地址
                // Also update inner_iph->check and icmph->checksum / 同时更新 inner_iph->check 和 icmph->checksum
            }
        } else if (redirect_ifindex == WAN_IFINDEX) {
            // LAN→WAN direction: inner IP header daddr (LAN address) needs to change to WAN address
            // 内→外方向：内嵌 IP 头的 daddr（内网地址）需要改为外网地址
            if ((bpf_ntohl(inner_iph->daddr) & OPT_ADDR_PREFIX_MASK) ==
                (INNER_PREFIX & OPT_ADDR_PREFIX_MASK)) {
                // Modify inner_iph->daddr → OUTER_ADDR / 修改 inner_iph->daddr → OUTER_ADDR
            }
        }
    }
}
```

**Why ICMP handling is so complex / 为什么 ICMP 处理如此复杂：** traceroute etc. rely on ICMP Time Exceeded messages. If the embedded IP header still contains LAN addresses (e.g., 10.1.2.x), external traceroute senders cannot understand. Recursive rewriting ensures all levels of addresses are consistent.

traceroute 等工具依赖 ICMP Time Exceeded 消息。如果内嵌 IP 头中仍包含内网地址（如 10.1.2.x），外部 traceroute 发起方无法理解。递归改写保证所有层级的地址都一致。

---

## Simplified Version optmap2.c Difference Analysis / 简化版 optmap2.c 的差异分析

Key differences between optmap2.c and optmap.c / optmap2.c 与 optmap.c 的关键差别：

| Aspect / 方面 | optmap.c | optmap2.c |
|------|----------|-----------|
| **Direction Decision / 方向判定** | `daddr == OUTER_ADDR` → WAN→LAN / 外→内 | `(daddr & PREFIX_MASK) == INNER_PREFIX` → WAN→LAN / 外→内 |
| **L4 Protocol / L4 协议** | TCP, UDP, ICMP / TCP、UDP、ICMP | TCP only / 仅 TCP (`if (iph->protocol != IPPROTO_TCP) return XDP_PASS;`) |
| **Checksum / 校验和** | `csum_opt_ext_addr_delta()` incremental update (considering all byte positions) / `csum_opt_ext_addr_delta()` 增量更新（考虑所有字节位置） | Simplified byte-by-byte checksum replacement / 简化的逐字节校验和替换 |
| **ICMP Recursion / ICMP 递归** | Supported / 支持 | Not supported / 不支持 |
| **redirect** | `return bpf_redirect(ifindex, 0)` | `return XDP_PASS` (relies on kernel routing) / `return XDP_PASS`（依赖内核路由） |

**How to choose a version / 如何选择版本：**
- TCP only and understand the principle → optmap2.c (40% less code, clearer) / 只需要 TCP 且理解原理 → optmap2.c（代码量少 40%，更清晰）
- Need full UDP/TCP/ICMP support and production-grade checksum → optmap.c / 需要完整的 UDP/TCP/ICMP 支持和生产级校验和 → optmap.c

---

## ebpf.c — Alternative Routing Scheme / ebpf.c —— 备选路由方案

`ebpf.c` implements another, more general routing strategy — instead of relying on predefined address mappings, it queries the kernel FIB (Forwarding Information Base) via eBPF's `bpf_fib_lookup()`, and caches the query results in an eBPF Map:

`ebpf.c` 实现了另一种更通用的路由策略——不依赖预定义的地址映射，而是通过 eBPF 的 `bpf_fib_lookup()` 查询内核 FIB（Forwarding Information Base，转发信息库），并将查询结果缓存在 eBPF Map 中：

```c
struct {
    __uint(type, BPF_MAP_TYPE_LRU_HASH);   // LRU auto-aging / LRU 自动老化
    __uint(max_entries, 100);
    __type(key, int);                      // key = daddr (IPv4)
    __type(value, struct rt_item);         // value = {ifindex, dmac, smac}
} rtcache_map SEC("maps");

// Processing flow: / 处理流程：
// 1. Packet has IP options (ihl > 5) → enter fast path / 数据包有 IP 选项（ihl > 5） → 进入快速路径
//    a. Lookup rtcache_map → hit → directly rewrite MAC + redirect / 查 rtcache_map → 命中 → 直接 rewrite MAC + redirect
//    b. Lookup bpf_fib_lookup() → hit → insert into rtcache_map + redirect / 查 bpf_fib_lookup() → 命中 → 插入 rtcache_map + redirect
//    c. Both fail → XDP_PASS (fall back to kernel stack) / 都失败 → XDP_PASS（回退到内核协议栈）
// 2. Packet has no IP options → XDP_PASS (goes through normal kernel routing) / 数据包无 IP 选项 → XDP_PASS（走正常内核路由）
```

**Advantages of this approach / 这个方案的优势：**
- Does not rely on hardcoded constants like `WAN_IFINDEX`/`LAN_IFINDEX`/`OUTER_ADDR` / 不依赖硬编码的 `WAN_IFINDEX`/`LAN_IFINDEX`/`OUTER_ADDR` 等常量
- Automatically adapts to routing table changes (`BPF_MAP_TYPE_LRU_HASH` automatically ages out expired FIB entries) / 自动适应路由表变化（`BPF_MAP_TYPE_LRU_HASH` 会自动老化过期的 FIB 条目）
- Can be used to build a more general "IP option-based fast path router" / 可用于构建更通用的"基于 IP 选项的快速路径路由器"

---

## Test Tools Detail / 测试工具详解

### send.c / send2.c — UDP/TCP Sender / send.c / send2.c —— UDP/TCP 发送器

```c
// send.c: UDP Sender / send.c: UDP 发送器
int main(int argc, char *argv[]) {
    // Usage: ./send <destination IP> <destination opt>
    // e.g., ./send 10.1.1.1 5  → send to 10.1.1.1, extended destination = 5 / 例如: ./send 10.1.1.1 5  → 发送到 10.1.1.1，扩展目标地址 = 5

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    // Set IP options (including extended destination address) / 设置 IP 选项（含扩展目标地址）
    struct opt_ext ext;
    init_opt_ext(&ext);
    opt_ext_set_daddr(&ext, (__u32)dest_opt);
    setsockopt(sock, IPPROTO_IP, IP_OPTIONS, &ext, sizeof(ext));

    // Send / 发送
    sendto(sock, message, sizeof(message), 0, (struct sockaddr *)&dest, sizeof(dest));
}

// send2.c: TCP Sender (same logic, SOCK_STREAM + connect + send) / send2.c: TCP 发送器（逻辑相同，SOCK_STREAM + connect + send）
```

### recv.c / recv2.c — UDP/TCP Receiver / recv.c / recv2.c —— UDP/TCP 接收器

```c
// recv.c: UDP Receiver, use recvmsg to get IP options / recv.c: UDP 接收器，使用 recvmsg 获取 IP 选项
setsockopt(sockfd, IPPROTO_IP, IP_RECVOPTS, &opt, sizeof(opt));

// Get IP options through msg_control (ancillary data) / 通过 msg_control (ancillary data) 获取 IP 选项
recvmsg(sockfd, &msg, 0);
for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
    if (cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == IP_RECVOPTS) {
        // Print hex content of received IP options / 打印接收到的 IP 选项的十六进制内容
    }
}
```

### mytcp.c — LD_PRELOAD Socket Wrapper / mytcp.c —— LD_PRELOAD Socket Wrapper

```c
// Hijack socket() call, automatically set IP options for all created sockets
// 劫持 socket() 调用，自动为所有创建的 socket 设置 IP 选项
int socket(int domain, int type, int protocol) {
    int sockfd = __socket(domain, type, protocol);

    // Check environment variable DEST_EXT, inject IP option if set
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

**Usage / 使用方法：**
```bash
DEST_EXT=5 LD_PRELOAD=./mytcp.so ./your_legacy_app
# All connections created by sockets automatically get ext_daddr=5 / 所有 socket 创建的连接都自动带上 ext_daddr=5
```

### addnat.py — DNAT Rule Generation / addnat.py —— DNAT 规则生成

```python
for i in range(1000, 65535):
    os.system(f'iptables -t nat -A PREROUTING -i eth2 -p tcp --dport {i} '
              f'-j DNAT --to-destination 10.1.3.2:888')
```

Redirects all TCP traffic arriving on eth2 on ports 1000-65534 to a single internal address on port 888. Used to test XDP program processing capability under large port space.

将所有到达 eth2 的 TCP 端口 1000-65534 的流量 DNAT 到单一内网地址的 888 端口。用于测试 XDP 程序在大规模端口空间下的处理能力。

### parse_trace.py — Performance Log Parsing / parse_trace.py —— 性能日志解析

Parses log files in multiple formats, extracts performance metrics and computes statistical distributions (mean, median, P90, standard deviation):

解析多种格式的日志文件，提取性能指标并计算统计分布（均值、中位数、P90、标准差）：

```
direction, count, avg_ns, median_ns, p90_ns, stddev_ns
```

---

## Complete Compilation Guide / 编译完整指南

### Environment Requirements / 环境要求

```bash
# System dependencies / 系统依赖
sudo apt update && sudo apt install -y \
    build-essential clang llvm gcc g++ make \
    linux-headers-$(uname -r) \
    libbpf-dev bpftool \
    python3

# Verify eBPF support / 验证 eBPF 支持
llc --version                  # LLVM needs BPF target support / LLVM 需要支持 BPF 目标
clang -target bpf -v 2>&1 | head -5  # Confirm BPF target is available / 确认 BPF target 可用

# Verify kernel XDP support / 验证内核 XDP 支持
uname -r                       # Need 4.18+ (recommend 5.4+) / 需要 4.18+ (推荐 5.4+)
grep CONFIG_XDP /boot/config-$(uname -r)  # Should contain CONFIG_XDP=y or =m / 应包含 CONFIG_XDP=y 或 =m
```

### Step 1: Compile the eBPF Program / 第一步：编译 eBPF 程序

```bash
cd optmap

# Compile full version (TCP+UDP+ICMP) / 编译复杂版（TCP+UDP+ICMP）
clang -O2 -target bpf -c optmap.c -o optmap.o \
    -I/usr/include/x86_64-linux-gnu \
    -DOPT_ADDR_EXT_BYTES=1

# Compile simplified version (TCP-only) / 编译简化版（TCP-only）
clang -O2 -target bpf -c optmap2.c -o optmap2.o \
    -I/usr/include/x86_64-linux-gnu

# Compile alternative routing version / 编译备选路由版
clang -O2 -target bpf -c ebpf.c -o ebpf.o \
    -I/usr/include/x86_64-linux-gnu

# Verify compilation artifacts / 验证编译产物
file optmap.o       # ELF 64-bit LSB relocatable, eBPF
llvm-objdump -h optmap.o | grep xdp_router  # Confirm section exists / 确认 section 存在
```

### Step 2: Compile User-space Test Tools / 第二步：编译用户态测试工具

```bash
# UDP test tools / UDP 测试工具
gcc -o send send.c
gcc -o recv recv.c

# TCP test tools / TCP 测试工具
gcc -o send2 send2.c
gcc -o recv2 recv2.c

# LD_PRELOAD wrapper
gcc -shared -fPIC -o mytcp.so mytcp.c -ldl
```

### Step 3: Verify the eBPF Program / 第三步：验证 eBPF 程序

```bash
# Check program with bpftool / 用 bpftool 检查程序
bpftool prog load optmap.o /sys/fs/bpf/optmap_test type xdp 2>&1
# If successful, the program can be loaded into the kernel / 如果成功，程序可以加载到内核
bpftool prog show | grep optmap

# Clean up test load / 清理测试加载
rm /sys/fs/bpf/optmap_test 2>/dev/null
```

### Common Compilation Errors / 常见编译错误

| Error / 错误 | Cause / 原因 | Solution / 解决 |
|------|------|------|
| `fatal error: 'bpf/bpf_helpers.h' file not found` | libbpf-dev not installed or wrong path / libbpf-dev 未安装或路径不对 | `sudo apt install libbpf-dev`; or `-I/usr/include/x86_64-linux-gnu` / 或 `-I/usr/include/x86_64-linux-gnu` |
| `error: unknown target 'bpf'` | LLVM/Clang version too old / LLVM/Clang 版本过低 | Need Clang 6+, recommend Clang 11+ / 需要 Clang 6+，推荐 Clang 11+ |
| `could not build bpf program: Invalid argument` | eBPF verifier rejected the program / eBPF 验证器拒绝程序 | Check boundary checks are complete, or use `bpftool prog tracelog` to view verifier errors / 检查边界检查是否完整，或使用 `bpftool prog tracelog` 查看验证器错误 |
| `undefined reference to 'bpf_ktime_get_ns'` | BPF helper not provided at link time / 链接时未提供 BPF helper | Ensure using `-target bpf` instead of direct linking / 确保用 `-target bpf` 而不是直接链接 |
| `error: <linux/ip.h> redefinition` | Header file conflict / 头文件冲突 | Use `-I` to point to the correct kernel header path / 使用 `-I` 指向正确的内核头文件路径 |

---

## Deployment and Operation / 部署与运行

### Preparation / 准备工作

```bash
# Determine interface names and corresponding ifindex values / 确定接口名称和对应的 ifindex
ip link show
# Assumption: eth1 (ifindex=3) = WAN, eth2 (ifindex=4) = LAN / 假设：eth1 (ifindex=3) = WAN, eth2 (ifindex=4) = LAN

# Enable IPv4 forwarding / 开启 IPv4 转发
echo 1 | sudo tee /proc/sys/net/ipv4/conf/all/forwarding
echo 1 | sudo tee /proc/sys/net/ipv4/conf/all/accept_local

# Configure interface addresses / 配置接口地址
sudo ip addr add 10.1.1.1/24 dev eth1    # WAN address / 外网地址
sudo ip addr add 10.1.2.1/24 dev eth2    # LAN gateway / 内网网关
```

### Deploy the eBPF Program / 部署 eBPF 程序

```bash
# Method A: Use Makefile install target / 方法 A：使用 Makefile 中的 install target
sudo make install WAN_DEV=eth1 LAN_DEV=eth2

# Method B: Manual execution / 方法 B：手动执行
sudo ip link set dev eth1 xdp obj optmap.o sec xdp_router
sudo ip link set dev eth2 xdp obj optmap.o sec xdp_router

# Verify loading / 验证加载
sudo bpftool net list
# Should show xdp programs attached to eth1 and eth2 / 应显示 eth1 和 eth2 上挂载了 xdp 程序

sudo ip link show eth1 | grep xdp
# Should contain "xdp" / 应包含 "xdp" 字样
```

### Uninstallation / 卸载

```bash
# Method A: Use Makefile / 方法 A：使用 Makefile
sudo make uninstall WAN_DEV=eth1 LAN_DEV=eth2

# Method B: Manual / 方法 B：手动
sudo ip link set dev eth1 xdp off
sudo ip link set dev eth2 xdp off
```

### End-to-End Testing / 端到端测试

```bash
# Terminal 1: Start receiver on WAN node / 终端 1：在外网节点上启动接收器
./recv          # Listen on UDP 12345 / 监听 UDP 12345
# or / 或
./recv2         # Listen on TCP 12345 / 监听 TCP 12345

# Terminal 2: Start sender on LAN node / 终端 2：在内网节点上启动发送器
LD_PRELOAD=./mytcp.so DEST_EXT=5 ./send2 10.1.1.1 5
# 10.1.1.1 is the WAN address (OUTER_ADDR) / 10.1.1.1 是外网地址（OUTER_ADDR）
# DEST_EXT=5 and the trailing 5 are both ext_daddr (LAN target host number) / DEST_EXT=5 和最后的 5 都是 ext_daddr（内网目标主机号）

# Terminal 3: Monitor XDP processing statistics / 终端 3：监控 XDP 处理统计
sudo bpftool prog show
sudo bpftool prog tracelog
```

### Performance Testing / 性能测试

```bash
# XDP processing latency (via bpftrace or eBPF tracepoint) / XDP 处理延迟（通过 bpftrace 或 eBPF tracepoint）
# Timing code bpf_ktime_get_ns() already in optmap.c / 在 optmap.c 中已有 bpf_ktime_get_ns() 计时代码
# Parse logs with parse_trace.py / 用 parse_trace.py 解析日志

# Throughput testing (using iperf3 or pktgen) / 吞吐量测试（使用 iperf3 或 pktgen）
# On LAN node / 在内网节点：
iperf3 -c 10.1.1.1 -t 30    # TCP throughput through XDP NAT / 经过 XDP NAT 的 TCP 吞吐量
# Control group: disable XDP / 对照组：关闭 XDP
sudo ip link set dev eth1 xdp off
iperf3 -c 10.1.1.1 -t 30    # Kernel NAT throughput without XDP / 无 XDP 的内核 NAT 吞吐量
```

---

## Experiment Customization Guide / 实验定制指南

### Experiment 1: Modify Extended Address Size / 实验 1：修改扩展地址大小

```bash
# Change from 1-byte extension to 2-byte (supports 65536 LAN addresses) / 从 1 字节扩展改为 2 字节（支持 65536 个内网地址）
clang -O2 -target bpf -c optmap.c -o optmap.o \
    -DOPT_ADDR_EXT_BYTES=2 \
    -I/usr/include/x86_64-linux-gnu

# Redeploy / 重新部署
sudo ip link set dev eth1 xdp off
sudo ip link set dev eth1 xdp obj optmap.o sec xdp_router
```

### Experiment 2: Modify Network Topology Constants / 实验 2：修改网络拓扑常量

```c
// Edit optmap.h, modify: / 编辑 optmap.h，修改：
#define WAN_IFINDEX 5       // Change to your actual value / 改成你的实际值
#define LAN_IFINDEX 6
#define OUTER_ADDR 0xc0a80101  // 192.168.1.1
#define INNER_PREFIX 0xc0a80200  // 192.168.2.0/24
```

### Experiment 3: Measure ALWAYS_ALLOC_EXT Performance Overhead / 实验 3：测量 ALWAYS_ALLOC_EXT 的性能开销

```bash
# Compile two versions: / 编译两个版本：
# A. With ALWAYS_ALLOC_EXT / 带 ALWAYS_ALLOC_EXT
clang -O2 -target bpf -c optmap.c -o optmap_alloc.o -DALWAYS_ALLOC_EXT=1

# B. Without ALWAYS_ALLOC_EXT / 不带 ALWAYS_ALLOC_EXT
clang -O2 -target bpf -c optmap.c -o optmap_noalloc.o

# Deploy separately and measure throughput difference / 分别部署并测量吞吐量差异
```

### Experiment 4: Add IPswen Level Support / 实验 4：添加对 IPswen level 的支持

```c
// Add level field to opt_ext / 在 opt_ext 中增加 level 字段
struct opt_ext_v2 {
    __u8 _scode, _slen;
    __s8 level;                          // ★ New: signed level / ★ 新增：有符号 level
    __u8 saddr[OPT_ADDR_EXT_BYTES];
    __u8 _dcode, _dlen;
    __u8 daddr[OPT_ADDR_EXT_BYTES];
};

// Consider level in routing decision / 在路由决策中考虑 level
if (ext->level > 0) {
    // Forward extension: append level bytes after LAN address / 前向扩展：在内网地址后附加 level 字节
    // ...
} else if (ext->level < 0) {
    // Backward extension: prepend |level| bytes before LAN address / 后向扩展：在内网地址前附加 |level| 字节
    // ...
}
```

### Experiment 5: Implement XDP-Level Connection Tracking / 实验 5：实现 XDP 级别的连接跟踪

```c
// Store connection state in eBPF Map / 在 eBPF Map 中存储连接状态
struct {
    __uint(type, BPF_MAP_TYPE_LRU_HASH);
    __uint(max_entries, 10000);
    __type(key, struct conn_key);    // {saddr, daddr, sport, dport, proto}
    __type(value, struct conn_state); // {state, last_seen, ...}
} conntrack SEC("maps");
```

---

## eBPF/XDP Technical Background / eBPF/XDP 技术背景

### XDP Data Path Position / XDP 数据通路位置

```
  ┌─────────────┐
  │     NIC     │
  └──────┬──────┘
         │ DMA
  ┌──────▼──────────────┐
  │   Driver (RX ring)  │
  └──────┬──────────────┘
         │
  ┌──────▼──────┐  ← XDP program executes here (earliest path) / ← XDP 程序在此执行（最早路径）
  │   XDP hook  │     Packet is still in driver linear memory / 数据包还在驱动层的线性内存中
  └──────┬──────┘     No sk_buff allocation / 没有 sk_buff 分配
         │
  ┌──────▼──────────────┐
  │  Kernel Network     │
  │  Stack (sk_buff)    │  ← XDP_PASS only reaches here / ← XDP_PASS 才到这里
  └─────────────────────┘
```

### XDP Return Codes / XDP 返回码

| Return Code / 返回码 | Meaning / 含义 | optmap Usage / optmap 使用场景 |
|--------|------|----------------|
| `XDP_PASS` | Pass to kernel network stack / 交给内核协议栈 | Unmatched packets, non-IP packets / 不匹配的数据包、非 IP 包 |
| `XDP_REDIRECT` | Redirect to another NIC / 重定向到其他网卡 | Matched packets (WAN→LAN or LAN→WAN) / 匹配的数据包（外→内 或 内→外） |
| `XDP_DROP` | Drop / 丢弃 | Not used / 未使用 |
| `XDP_TX` | Send back from the same NIC / 从同一网卡发回 | Not used / 未使用 |
| `XDP_ABORTED` | Error, triggers tracepoint / 错误，触发 tracepoint | Not used / 未使用 |

### eBPF Verifier Limitations / eBPF 验证器限制

XDP programs are subject to strict restrictions from the eBPF verifier:
XDP 程序受到 eBPF 验证器的严格限制：
- Maximum instructions: 1 million (5.4 kernel), optmap.c ~500 / 最大指令数：100 万条（5.4 内核），optmap.c 约 500 条
- Must perform boundary checks on all branches (all pointer dereferences have `> data_end` checks) / 必须对所有分支做边界检查（所有指针解引用前都有 `> data_end` 检查）
- Cannot use infinite loops (each `for` loop is unrolled with `#pragma clang loop unroll(full)`) / 不能使用无限循环（每行 `for` 循环用 `#pragma clang loop unroll(full)` 展开）
- Cannot call arbitrary kernel functions, only helper functions listed in `bpf_helper_defs.h` / 不能调用任意内核函数，只能使用 `bpf_helper_defs.h` 中列出的辅助函数

---

## Notes / 注意事项

- **ifindex is hardcoded / ifindex 是硬编码的：** Changing interface names or replugging NICs may change ifindex values. Use `ip link show` to confirm actual values and update `optmap.h` / 修改接口名或重新插拔网卡可能导致 ifindex 变化。用 `ip link show` 确认实际值后更新 `optmap.h`
- **Only supports 24-bit prefix / 仅支持 24 位前缀：** `INNER_PREFIX` assumes `/24`. For `/16` or `/23`, modify the `OPT_ADDR_PREFIX_MASK` formula / `INNER_PREFIX` 假设为 `/24`。如果需要 `/16` 或 `/23`，需要修改 `OPT_ADDR_PREFIX_MASK` 的计算公式
- **eBPF programs cannot have global mutable state / eBPF 程序不能有全局可变状态：** All shared state must be stored in eBPF Maps / 所有共享状态必须存储在 eBPF Map 中
- **XDP programs run at the driver layer / XDP 程序在驱动层运行：** A crash will cause packet loss but not a kernel panic / 崩溃会导致数据包丢失，但不会导致内核 panic
- **Does not support GRO/GSO / 不支持 GRO/GSO：** Segmented packets cannot be processed by XDP (because GRO runs after XDP) / 分段数据包无法被 XDP 处理（因为 GRO 在 XDP 之后）
- **IPv4 only / 仅处理 IPv4：** The program checks `eth->h_proto != ETH_P_IP` at the start, does not handle IPv6 / 程序开头检查 `eth->h_proto != ETH_P_IP`，不处理 IPv6
- **bpf_printk performance impact / bpf_printk 性能影响：** Enabling `bpf_printk` (uncomment `#define bpf_printk(...)`) causes severe performance degradation, for debugging only / 开启 `bpf_printk`（取消 `#define bpf_printk(...)` 的注释）会导致严重的性能下降，仅用于调试

---

## Related Projects / 相关项目

- **fakedns** — DNS proxy at the LD_PRELOAD layer, can automatically inject IP options for applications (`inject_dest_ext_option()`) / LD_PRELOAD 层的 DNS 代理，可自动为应用程序注入 IP 选项（`inject_dest_ext_option()`）
- **happyfootball** — Happy Eyeballs + IP option multipath connection experiment, optmap can serve as its underlying forwarding layer / Happy Eyeballs + IP 选项的多路径连接实验，optmap 可作为其底层转发层
- **iproute2-5.5.0** — IPswen enhanced `ip route`, can set routes used in conjunction with XDP / IPswen 增强版 `ip route`，可设置 XDP 配合使用的路由
- **linux-kernel** — IPswen kernel, provides FIB extensions needed by `bpf_fib_lookup` / IPswen 内核，提供 `bpf_fib_lookup` 所需的 FIB 扩展

---

## Appendix A: Quick Reference Card / 附录 A：快速操作卡片

### Compile / 编译
```bash
clang -O2 -target bpf -c optmap.c -o optmap.o
gcc -o send send.c && gcc -o recv recv.c
gcc -o send2 send2.c && gcc -o recv2 recv2.c
gcc -shared -fPIC -o mytcp.so mytcp.c -ldl
```

### Deploy / 部署
```bash
sudo ip link set dev eth1 xdp obj optmap.o sec xdp_router
sudo ip link set dev eth2 xdp obj optmap.o sec xdp_router
```

### Uninstall / 卸载
```bash
sudo ip link set dev eth1 xdp off
sudo ip link set dev eth2 xdp off
```

### Test / 测试
```bash
# UDP
./recv &  # WAN node / 外网节点
LD_PRELOAD=./mytcp.so DEST_EXT=5 ./send 10.1.1.1 5  # LAN node / 内网节点

# TCP
./recv2 &
LD_PRELOAD=./mytcp.so DEST_EXT=5 ./send2 10.1.1.1 5
```

### File Location Quick Reference / 文件位置速查
| Content / 内容 | Path / 路径 |
|------|------|
| Core header file / 核心头文件 | `optmap/optmap.h` |
| XDP main program / XDP 主程序 | `optmap/optmap.c` |
| XDP simplified version / XDP 简化版 | `optmap/optmap2.c` |
| FIB cache version / FIB 缓存版 | `optmap/ebpf.c` |
| UDP sender / UDP 发送器 | `optmap/send` |
| UDP receiver / UDP 接收器 | `optmap/recv` |
| TCP sender / TCP 发送器 | `optmap/send2` |
| TCP receiver / TCP 接收器 | `optmap/recv2` |
| Socket wrapper | `optmap/mytcp.so` |
| Build artifacts / 编译产物 | `optmap/optmap.o` |
| Performance log parser / 性能日志解析 | `optmap/parse_trace.py` |

## Appendix B: Checksum Incremental Update Quick Reference / 附录 B：校验和增量更新速查

```c
// Replace IP address (32-bit) / 替换 IP 地址（32 位）
csum = csum_replace_u32(csum, old_ip, new_ip);

// Replace IP address in TCP/UDP pseudo-header / 替换 TCP/UDP 伪头中的 IP 地址
__u32 addr_delta = ~old_ip + new_ip + carry;
tcp_csum = csum16_add(tcp_csum, addr_delta);
udp_csum = csum16_add(udp_csum, addr_delta);

// Replace IP total length / 替换 IP 总长度
csum = csum_replace_u16(csum, old_tot_len, new_tot_len);

// Recalculate entire IP checksum (not recommended, lower performance than incremental update) / 重新计算整个 IP 校验和（不推荐，低于增量更新的性能）
iph->check = 0;
iph->check = ip_fast_csum((__u8 *)iph, iph->ihl);  // Only available in non-XDP path / 仅在非 XDP 路径可用
```

## Appendix C: eBPF Map Type Selection Guide / 附录 C：eBPF Map 类型选择指南

| Map Type / Map 类型 | Feature / 特性 | Use Case / 适用场景 |
|---------|------|---------|
| `BPF_MAP_TYPE_HASH` | General hash table, no automatic expiration / 通用哈希表，无自动过期 | Persistent state / 需要持久化的状态 |
| `BPF_MAP_TYPE_LRU_HASH` | With LRU automatic expiration / 带 LRU 自动过期 | Cache-type data (e.g., FIB cache) / 缓存类数据（如 FIB 缓存） |
| `BPF_MAP_TYPE_ARRAY` | Fixed-size array, fastest / 固定大小数组，最快 | Direct indexing of predefined rules / 预定义规则的直接索引 |
| `BPF_MAP_TYPE_PERCPU_HASH` | Per-CPU independent hash table / 每 CPU 独立哈希表 | Lock-free per-CPU counters / 无锁的每 CPU 计数器 |
| `BPF_MAP_TYPE_LPM_TRIE` | Longest prefix match trie / 最长前缀匹配树 | Routing table (alternative to FIB lookup) / 路由表（替代 FIB lookup） |
