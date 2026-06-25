# unpv13e —— Unix 网络编程示例集（含 IPswen Ping） / Unix Network Programming Examples Collection (with IPswen Ping)

## 概述 / Overview

这是 W. Richard Stevens 经典著作《Unix Network Programming, Volume 1, Third Edition》（简称 UNP）配套示例代码的修改版。原书是全球网络编程领域的圣经级参考资料——全面覆盖 TCP/UDP、Socket API、路由、多播、信号驱动 I/O、线程等主题。
This is a modified version of the companion example code for W. Richard Stevens' classic book "Unix Network Programming, Volume 1, Third Edition" (UNP). The original book is a seminal reference in the field of network programming — comprehensively covering TCP/UDP, Socket API, routing, multicast, signal-driven I/O, threads, and other topics.

本版本在 `ping` 工具中集成了 IPswen 地址支持。其余 20+ 章的代码保持原样，作为标准网络编程的可信参考基线，用于与 IPswen 修改代码进行对照验证。
This version integrates IPswen address support in the `ping` tool. The code for the remaining 20+ chapters is kept unchanged, serving as a trusted reference baseline for standard network programming, used for comparison and verification with IPswen-modified code.

---

## 目录结构 / Directory Structure

```
unpv13e/
├── ping/             # ★ ICMP Ping（IPswen 修改所在） / IPswen modifications located here
│   ├── main.c        # EXPERIMENTAL_IPswen 宏，ipswen_aton/ntoa 调用 / EXPERIMENTAL_IPswen macro, ipswen_aton/ntoa calls
│   ├── ping.h        # 协议头 / Protocol header
│   └── old/          # 旧版 ping / Legacy ping
│
├── lib/              # 共享函数库（wrap 系统调用，错误处理） / Shared function library (wrapping system calls, error handling)
├── libfree/          # 替代/自由实现函数 / Alternative/libre implementations
├── libgai/           # getaddrinfo 实现 / getaddrinfo implementation
├── libroute/         # 路由 socket 库 / Routing socket library
│
├── tcpcliserv/       # TCP 客户端/服务器（第5章） / TCP client/server (Chapter 5)
├── udpcliserv/       # UDP 客户端/服务器（第8章） / UDP client/server (Chapter 8)
├── select/           # I/O 多路复用（第6章） / I/O multiplexing (Chapter 6)
├── server/           # 并发服务器设计（第27章） / Concurrent server design (Chapter 27)
├── ioctl/            # ioctl 操作（第17章） / ioctl operations (Chapter 17)
├── route/            # 路由 socket（第18章） / Routing socket (Chapter 18)
├── key/              # 密钥管理 socket（第19章） / Key management socket (Chapter 19)
├── sock/             # 原始 socket（第28章） / Raw socket (Chapter 28)
├── sockopt/          # Socket 选项（第7章） / Socket options (Chapter 7)
├── ipopts/           # IP 选项（第27章）★ IPswen 直接相关 / IP options (Chapter 27) ★ Directly IPswen-related
├── traceroute/       # Traceroute（第28章） / Traceroute (Chapter 28)
├── advio/            # 高级 I/O（第14章） / Advanced I/O (Chapter 14)
├── nonblock/         # 非阻塞 I/O（第16章）★ Happy Eyeballs 参考 / Non-blocking I/O (Chapter 16) ★ Happy Eyeballs reference
├── sigio/            # SIGIO 驱动 I/O（第25章） / SIGIO-driven I/O (Chapter 25)
├── threads/          # 线程（第26章） / Threads (Chapter 26)
├── bcast/            # 广播（第20章） / Broadcasting (Chapter 20)
├── mcast/            # 多播（第21章） / Multicasting (Chapter 21)
├── unixdomain/       # Unix 域 socket（第15章） / Unix domain sockets (Chapter 15)
├── names/            # 名称/地址转换（第11章） / Name/address translation (Chapter 11)
├── oob/              # 带外数据（第24章） / Out-of-band data (Chapter 24)
├── inetd/            # inetd 守护（第13章） / inetd daemon (Chapter 13)
├── rtt/              # RTT 测量（第28章） / RTT measurement (Chapter 28)
├── icmpd/            # ICMP 守护（第28章） / ICMP daemon (Chapter 28)
├── sctp/             # SCTP（第23章） / SCTP (Chapter 23)
├── ssntp/            # SNTP 客户端（第28章） / SNTP client (Chapter 28)
├── streams/          # STREAMS / STREAMS
├── test/             # 测试程序 / Test programs
│
├── config.h.in       # 配置模板（#define HAVE_IPswen 1） / Configuration template (#define HAVE_IPswen 1)
├── configure / configure.in
├── Makefile.in / Make.defines.in
├── aclocal.m4 / config.guess / config.sub
├── VERSION / README / LICENSE
```

---

## IPswen 修改详解 / IPswen Modifications Detail

### ping/main.c —— 地址解析流 / Address Resolution Flow

```c
#define EXPERIMENTAL_IPswen    // ★ 开启 IPswen 路径 / Enable IPswen Path

int main(int argc, char **argv) {
    // ... 参数解析 ... / Parameter parsing ...

    host = argv[optind];        // 目标主机名或地址 / Target host name or address

#ifndef EXPERIMENTAL_IPswen
    // === 标准路径 === / Standard Path
    struct hostent *hp = gethostbyname(host);
    // 或 inet_aton(host, &addr) / Or inet_aton(host, &addr)
#else
    // === IPswen 增强路径 === / IPswen Enhanced Path
    struct addrinfo *ai;

    // 先尝试 IPswen 解析 / Try IPswen resolution first
    if (!ipswen_aton(&((struct sockaddr_in *)ai->ai_addr)->swen_addr, host)) {
        // 成功解析为 IPswen 格式 / Successfully resolved to IPswen format
        // → 直接使用解析结果，不经过 DNS / Use result directly, bypass DNS
    } else {
        // 回退：先尝试标准 gethostbyname / Fallback: try standard gethostbyname first
        // 再尝试标准 inet_aton / Then try standard inet_aton
    }

    // 回显解析结果 / Echo the resolved result
    ipswen_ntoa(&..., buffer, sizeof(buffer));
    printf("PING %s ...\n", buffer);
#endif
}
```

**解析优先级：** IPswen 记法 → gethostbyname (DNS) → inet_aton (标准 IPv4)
**Resolution Priority:** IPswen notation → gethostbyname (DNS) → inet_aton (standard IPv4)
这种设计确保了：
This design ensures:
- 输入 `10.0.0.1(2)128.129` 被识别为 IPswen 地址
- Input `10.0.0.1(2)128.129` is recognized as an IPswen address
- 输入 `www.example.com` 走 DNS 解析
- Input `www.example.com` goes through DNS resolution
- 输入 `192.168.1.1` 走标准 IPv4 解析
- Input `192.168.1.1` goes through standard IPv4 resolution

**为什么放在 ping 中？** ping 是最常用的网络诊断工具。通过在 ping 中集成 IPswen，研究人员可以直接用 `ping 10.0.0.1(2)128.129` 测试 IPswen 地址的连通性——无需任何额外的测试工具。
**Why put it in ping?** ping is the most commonly used network diagnostic tool. By integrating IPswen into ping, researchers can directly use `ping 10.0.0.1(2)128.129` to test IPswen address connectivity — without any additional testing tools.

### config.h.in / config.h.in

```c
/* Define to 1 if the system supports IPswen */
#define HAVE_IPswen 1
```

默认启用。`configure` 脚本执行时会检查系统头文件，确认 `ipswen_aton`/`ipswen_ntoa` 函数可用。
Enabled by default. The `configure` script checks system header files to verify that the `ipswen_aton`/`ipswen_ntoa` functions are available.

---

## 各章示例与 IPswen 研究的关联 / Chapter Examples and IPswen Research Relevance

| 目录 | 章节主题 | 对 IPswen 研究的作用 |
| Directory | Chapter Topic | Relevance to IPswen Research |
|------|---------|-------------------|
| `ping/` | ICMP Ping | **已修改为支持 IPswen 地址** / **Modified to support IPswen addresses** |
| `ipopts/` | IP 选项 / IP Options | IP 选项 socket API 的标准参考——理解 `setsockopt(IP_OPTIONS)` 的正确用法 / Standard reference for IP option socket API — understanding correct usage of `setsockopt(IP_OPTIONS)` |
| `traceroute/` | 路由追踪 / Route Tracing | 标准 traceroute 实现，可对比 IPswen traceroute / Standard traceroute implementation for comparison with IPswen traceroute |
| `sockopt/` | Socket 选项 / Socket Options | `IP_OPTIONS`、`SO_BINDTODEVICE`、`SO_ERROR` 的权威参考 / Authoritative reference for `IP_OPTIONS`, `SO_BINDTODEVICE`, `SO_ERROR` |
| `nonblock/` | 非阻塞 I/O / Non-blocking I/O | 异步 connect 模式——happyfootball 的 `select_socket()` 即基于此模式 / Async connect pattern — happyfootball's `select_socket()` is based on this pattern |
| `route/` | 路由 Socket / Routing Socket | 内核路由表交互——理解 IPswen FIB 的用户态视角 / Kernel routing table interaction — understanding IPswen FIB from userspace perspective |
| `tcpcliserv/` | TCP C/S | TCP 标准实现基线 / TCP standard implementation baseline |
| `udpcliserv/` | UDP C/S | UDP 标准实现基线 / UDP standard implementation baseline |
| `select/` | I/O 多路复用 / I/O Multiplexing | `select()` 的标准用法 / Standard usage of `select()` |
| `advio/` | 高级 I/O / Advanced I/O | `sendfile`、`splice` 等 / `sendfile`, `splice`, etc. |

---

## 编译与使用 / Compilation and Usage

### 编译全部 / Build All

```bash
cd unpv13e
./configure
make -j$(nproc)
```

### 仅编译 ping / Build ping Only

```bash
cd ping
make ping
```

### IPswen Ping 使用 / IPswen Ping Usage

```bash
# Ping 一个 IPswen 地址（level=2, 扩展字节 128.129）/ Ping an IPswen address (level=2, extension bytes 128.129)
sudo ./ping 10.0.0.1(2)128.129

# Ping 标准 IPv4（向后兼容）/ Ping standard IPv4 (backward compatible)
sudo ./ping 192.168.1.1

# Ping DNS 名称 / Ping a DNS name
sudo ./ping www.example.com

# 带 verbose / With verbose output
sudo ./ping -v 10.0.0.1(2)128.129
```

### 运行其他示例 / Run Other Examples

```bash
# TCP echo 客户端 / TCP echo client
cd tcpcliserv
./tcpserv01 &     # 启动服务器 / Start the server
./tcpcli01 127.0.0.1
```

---

## 实验定制指南 / Experimental Customization Guide

### 怎样暂时禁用 IPswen / How to Temporarily Disable IPswen

注释掉 `ping/main.c` 中的 `#define EXPERIMENTAL_IPswen`，重新编译。这将使 ping 恢复标准 IPv4-only 行为。
Comment out `#define EXPERIMENTAL_IPswen` in `ping/main.c` and recompile. This will restore ping to standard IPv4-only behavior.

### 怎样在 traceroute 中加入 IPswen / How to Add IPswen to traceroute

参考 `ping/main.c` 的 `#ifdef EXPERIMENTAL_IPswen` 模式，在 `traceroute/main.c` 的目标地址解析部分加入相同逻辑。
Refer to the `#ifdef EXPERIMENTAL_IPswen` pattern in `ping/main.c` and add the same logic to the target address resolution section of `traceroute/main.c`.

---

## 注意事项 / Notes

- `EXPERIMENTAL_IPswen` 宏硬编码在 `main.c`——不需要修改 `config.h` 或其他头文件 / The `EXPERIMENTAL_IPswen` macro is hardcoded in `main.c` — no need to modify `config.h` or other header files
- `ipswen_aton()` 和 `ipswen_ntoa()` 是**外部函数**——由系统的 C 库或 IPswen 支持库提供（通常来自内核头文件编译的支持库） / `ipswen_aton()` and `ipswen_ntoa()` are **external functions** — provided by the system's C library or IPswen support library (typically from a support library compiled from kernel headers)
- 如果系统没有安装 IPswen 头文件，ping 编译会因缺少这些符号而失败——此时需注释掉 `#define EXPERIMENTAL_IPswen` / If the system does not have IPswen headers installed, ping compilation will fail due to missing these symbols — in this case, comment out `#define EXPERIMENTAL_IPswen`
- 其余 20+ 章代码是**原始 unpv13e**——它们是标准网络编程的权威参考，不受 IPswen 修改影响 / The remaining 20+ chapters are the **original unpv13e** — they serve as the authoritative reference for standard network programming and are unaffected by IPswen modifications

---

## 相关项目 / Related Projects

- **linux-kernel** (`../linux-kernel/`) —— 提供 IPswen Socket API 的内核 / The kernel providing the IPswen Socket API
- **happyfootball** (`../happyfootball/`) —— 基于非阻塞 connect 的地址选择实验 / Address selection experiment based on non-blocking connect
- **iproute2 / net-tools** —— IPswen 用户态工具链 / IPswen userspace toolchain
- **bird** —— IPswen BGP 路由 / IPswen BGP routing

---

## 附录 A: 完整目章节索引 / Appendix A: Complete Chapter Index

| 章节 | 目录 | 内容 | IPswen关联 |
| Chapter | Directory | Content | IPswen Relevance |
|------|------|------|-----------|
| 1 | intro/ | 简介 / Introduction | 无 / None |
| 3 | sock/ | 原始Socket / Raw Socket | 无 / None |
| 4 | tcpcliserv/ | TCP 基本 / TCP Basics | 基线参考 / Baseline reference |
| 5 | tcpcliserv/ | TCP C/S 示例 / TCP C/S Examples | 基线参考 / Baseline reference |
| 6 | select/ | I/O 多路复用 / I/O Multiplexing | 低 / Low |
| 7 | sockopt/ | Socket 选项 / Socket Options | ★ 高 (IP_OPTIONS, SO_BINDTODEVICE) / ★ High |
| 8 | udpcliserv/ | UDP 基本 / UDP Basics | 基线参考 / Baseline reference |
| 11 | names/ | 地址转换 / Address Translation | 中 (gethostbyname vs ipswen_aton) / Medium |
| 14 | advio/ | 高级 I/O / Advanced I/O | 低 / Low |
| 16 | nonblock/ | 非阻塞 I/O / Non-blocking I/O | ★ 高 (异步connect参考) / ★ High (async connect reference) |
| 17 | ioctl/ | ioctl / ioctl | 中 / Medium |
| 18 | route/ | 路由 Socket / Routing Socket | ★ 高 (内核FIB交互) / ★ High (kernel FIB interaction) |
| 20 | bcast/ | 广播 / Broadcasting | 低 / Low |
| 21 | mcast/ | 多播 / Multicasting | 低 / Low |
| 24 | oob/ | 带外数据 / Out-of-band Data | 低 / Low |
| 25 | sigio/ | 信号驱动 / Signal-driven I/O | 低 / Low |
| 26 | threads/ | 线程 / Threads | 低 / Low |
| 27 | server/ | 并发服务器 / Concurrent Server | 中 / Medium |
| 27 | ipopts/ | IP 选项 / IP Options | ★ 极高 / ★ Very High |
| 28 | traceroute/ | 路由追踪 / Route Tracing | ★ 极高 / ★ Very High |
| 28 | ping/ | Ping / Ping | ★ 已修改 / ★ Modified |
| 28 | icmpd/ | ICMP 守护 / ICMP Daemon | 中 / Medium |

## 附录 B: 如何新增 IPswen 支持到其他示例 / Appendix B: How to Add IPswen Support to Other Examples

以 `traceroute` 为例：
Using `traceroute` as an example:
1. 打开 `traceroute/main.c` / Open `traceroute/main.c`
2. 在地址解析处添加：/ Add at the address resolution section:
```c
#ifdef EXPERIMENTAL_IPswen
    if (!ipswen_aton(&sin->swen_addr, host)) {
        // 使用 IPswen 解析 / Use IPswen resolution
    } else
#endif
    // 原有 gethostbyname 回退 / Original gethostbyname fallback
```
3. 重新编译：`cd traceroute && make` / Recompile: `cd traceroute && make`

## 附录 C: 标准网络编程速记 / Appendix C: Standard Network Programming Quick Reference

```c
// 创建 TCP socket / Create a TCP socket
int sock = socket(AF_INET, SOCK_STREAM, 0);

// 设置 IP 选项 / Set IP options
setsockopt(sock, IPPROTO_IP, IP_OPTIONS, opt, optlen);

// 绑定到网口 / Bind to a network interface
setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, "eth0", 4);

// 非阻塞 connect / Non-blocking connect
fcntl(sock, F_SETFL, O_NONBLOCK);
connect(sock, ...);
select(sock+1, NULL, &wfds, NULL, &tv);
int err; getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len);
```
