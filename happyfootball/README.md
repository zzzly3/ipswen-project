# happyfootball —— Happy Eyeballs 多路径连接 + IP 选项实验 / Happy Eyeballs Multipath Connection + IP Options Experiment

## 概述 / Overview

`happyfootball` 是 RFC 8305 (Happy Eyeballs v2) 的实验性扩展——在标准 Happy Eyeballs（同时尝试 IPv4 和 IPv6、选用先成功的）基础上，增加了对 **IP 选项**（含实验性自定义类型）和 **多网口绑定**（`SO_BINDTODEVICE`）的支持。
`happyfootball` is an experimental extension of RFC 8305 (Happy Eyeballs v2) — building on standard Happy Eyeballs (simultaneously trying IPv4 and IPv6, using the one that succeeds first) with support for **IP Options** (including experimental custom types) and **multi-interface binding** (`SO_BINDTODEVICE`).

**核心问题：** 当到达同一目的地存在多条路径（不同物理接口、不同 IP 选项配置），如何选择？指标是 TCP 连接建立速度和端到端性能（MSS、吞吐量）。
**Core Problem:** When multiple paths exist to the same destination (different physical interfaces, different IP option configurations), how should we choose? The metrics are TCP connection establishment speed and end-to-end performance (MSS, throughput).

---

## 目录结构 / Directory Structure

```
happyfootball/
├── mysocket.hpp           # C++ HappyTCP 类接口声明 / C++ HappyTCP class interface declaration
├── mysocket.cpp           # ★ 核心 happy_socket() 算法完整实现 / Core happy_socket() algorithm full implementation
├── send.cpp / send        # TCP connect + MSS 测量 CLI 工具 / TCP connect + MSS measurement CLI tool
├── test.cpp / test        # 非阻塞 TCP connect 测试 / Non-blocking TCP connect test
├── getmss.py              # 基于 Scapy 的 SYN→MSS 探测 / Scapy-based SYN→MSS probing
├── ping.py                # 定时 GRE Ping 调度器 / Scheduled GRE Ping dispatcher
├── httpserver.py          # 简单 HTTP 测试端点 (端口 80) / Simple HTTP test endpoint (port 80)
├── recv.py                # TCP 接收吞吐量测量 / TCP receive throughput measurement
├── 1.ipynb                # Jupyter notebook（实验记录 / Experiment records）
└── .vscode/
```

---

## 核心算法：happy_socket() 完整源码走读 / Core Algorithm: happy_socket() Full Source Walkthrough

### 函数签名 / Function Signature

```c
int happy_socket(
    const struct sockaddr *addr,  // 目标地址 / Destination address
    int sock1,                     // 主路径 socket / Primary path socket
    int sock2,                     // 备用路径 socket / Backup path socket
    int timeout,                   // 首个连接最长等几秒 / Max seconds to wait for first connection
    int wait2us                    // 主路径成功后的宽限期（微秒）/ Grace period after primary succeeds (microseconds)
);
```

### 参数语义表 / Parameter Semantics Table

| 参数 / Parameter | 典型值 / Typical Value | 含义 / Meaning |
|------|--------|------|
| `addr` | `192.168.10.2:12345` | 目标 IP:Port / Destination IP:Port |
| `sock1` | `socket(AF_INET, SOCK_STREAM, 0)` | 主路径（通常更快但不一定有 IP 选项）/ Primary path (usually faster but may not have IP options) |
| `sock2` | `socket(AF_INET, SOCK_DGRAM, 0)` | 备用路径（通常更优但因 IP 选项可能较慢）/ Backup path (usually better but may be slower due to IP options) |
| `timeout` | 5 | 如果 5 秒内没有任何一个连接成功 → 放弃 / Abort if no connection succeeds within 5 seconds |
| `wait2us` | 250000（=250ms） | sock1 先成功后，再等 sock2 最多 250ms / Wait up to 250ms for sock2 after sock1 succeeds first |

### 完整步骤分解 / Complete Step Breakdown

**Step 0: 获取时间戳 / Get Timestamp**
```c
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;  // 纳秒精度 / Nanosecond precision
}
```
用于 `log()` 宏打印带时间戳的日志。
Used by the `log()` macro to print timestamped logs.

**Step 1: 设非阻塞 / Set Non-blocking**
```c
long arg = fcntl(sock1, F_GETFL, NULL);
arg |= O_NONBLOCK;
fcntl(sock1, F_SETFL, arg);
// sock2 同理（重复上述代码）/ Same for sock2 (repeating the above code)
```

**Step 2: 同时发起 connect / Initiate connect Simultaneously**
```c
connect(sock1, addr, sizeof(*addr));  // 非阻塞 connect 立即返回 EINPROGRESS / Non-blocking connect returns EINPROGRESS immediately
connect(sock2, addr, sizeof(*addr));
```
两个 connect 并行执行，不等待彼此。
Both connects execute in parallel, without waiting for each other.

**Step 3: select 等待可写 / select Waiting for Writable**
```c
std::vector<int> socks = {sock1, sock2};
struct timeval tv1 = {timeout, 0};   // 最长等 timeout 秒 / Wait at most timeout seconds
int sock = select_socket(socks, tv1);
```

**`select_socket()` 内部逻辑：/ Internal Logic:**
```c
int select_socket(std::vector<int> &socks, struct timeval& tv) {
    int sock = wait_writeable(socks, tv);  // fd_set + select()
    if (sock == -1) {         // 超时：全失败 / Timeout: all failed
        for (int s : socks) close(s);
        return -1;
    }
    socks.erase(std::remove(socks.begin(), socks.end(), sock), socks.end());
    int soerr = get_socketerror(sock);    // 检查 SO_ERROR / Check SO_ERROR
    if (soerr == 0) {
        return sock;          // connect 真正成功 / connect truly succeeded
    } else {
        close(sock);          // connect 假成功（select 可写但实际错误）/ False success (select writable but actual error)
        if (socks.empty()) return -1;
        return select_socket(socks, tv);  // 递归等剩下的 / Recursively wait for remaining
    }
}
```

`wait_writeable()` 是 `select()` 封装：构建 fd_set，调用 `select(socks.max()+1, NULL, &fd_set, NULL, &tv)`，返回第一个可写的 fd。
`wait_writeable()` is a `select()` wrapper: builds an fd_set, calls `select(socks.max()+1, NULL, &fd_set, NULL, &tv)`, returns the first writable fd.

**Step 4: 宽限期决策 / Grace Period Decision**
```c
if (sock == sock1) {                     // 主路径先成功 / Primary path succeeded first
    log("wait for sock2");
    struct timeval tv2 = {0, wait2us};   // 等 250ms / Wait 250ms
    if (select_socket(socks, tv2) == sock2) {
        log("use sock2");                // 备用路径也成功了 → 选它！/ Backup path also succeeded → pick it!
        sock = sock2;
    } else {
        log("use sock1");                // 超时 → 选主路径 / Timeout → pick primary
    }
} else if (sock == sock2) {
    log("use sock2");                    // 备用先成功 → 直接用 / Backup succeeded first → use directly
} else {
    log("use nothing");                  // 都失败了 / All failed
}
```

**设计哲学：/ Design Philosophy:**
- **偏向 sock2（备用/更好的路径）**：只要 sock2 也成功（即使在主路径之后），就选它 / **Bias toward sock2 (backup/better path):** As long as sock2 also succeeds (even after the primary), pick it
- **不牺牲延迟**：只在主路径先成功时才等待，如果备用先成功就不等 / **No latency sacrifice:** Only wait when the primary succeeds first; if the backup succeeds first, don't wait
- **宽限期极短**：250ms 对于人类感知几乎无影响，但足够让第二个 TCP 握手完成 / **Extremely short grace period:** 250ms is nearly imperceptible to humans, but long enough for the second TCP handshake to complete

**Step 5: 清理 / Cleanup**
```c
for (int sock : socks) close(sock);  // 关闭未使用的 socket / Close unused sockets
return sock;                          // 返回选中的 fd / Return the selected fd
```

---

## 辅助函数详解 / Helper Functions Explained

### get_socketerror()
```c
int get_socketerror(int sock) {
    int so_error;
    socklen_t len = sizeof so_error;
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
    return so_error;
}
```
非阻塞 connect 后，`select()` 报告可写可能因为连接成功也可能因为错误。`SO_ERROR` 给出真实结果：0 = 成功，非 0 = `errno` 值。
After non-blocking connect, `select()` reporting writable could mean success or an error. `SO_ERROR` reveals the truth: 0 = success, non-zero = `errno` value.

### bind_device()
```c
void bind_device(int sock, const char *device) {
    setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, device, strlen(device));
}
```
将 socket 绑定到特定网口（如 `"eth0"`、`"tun-2531"`）。这样两个 socket 可走不同的物理路径。
Binds a socket to a specific network interface (e.g., `"eth0"`, `"tun-2531"`). This allows two sockets to take different physical paths.

### add_ip_options()
```c
void add_ip_options(int sock, int size) {
    unsigned char ip_options[64] = {0};
    ip_options[0] = 0b10100000;      // Copy-on-Frag=1, Class=2(Debug), Number=0
    ip_options[1] = size + 1;        // 长度（含自身）/ Length (including itself)
    setsockopt(sock, IPPROTO_IP, IP_OPTIONS, ip_options, size + 1);
}
```

IP Option Type 字节 (0xA0) 的位结构：
Bit structure of the IP Option Type byte (0xA0):
```
Bit 7 (0x80): Copy-on-Fragmentation = 1 (所有分片都携带此选项 / All fragments carry this option)
Bit 6-5:      Option Class = 2 (Debug/Measurement)
Bit 4-0:      Option Number = 0
```

### main() 演示 / main() Demo
```c
int main() {
    // 准备目标 / Prepare target
    sockaddr_in addr; addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr("192.168.10.2");

    // 创建两个 socket / Create two sockets
    int sock1 = socket(AF_INET, SOCK_DGRAM, 0);
    int sock2 = socket(AF_INET, SOCK_STREAM, 0);

    add_ip_options(sock1);       // 只在 sock1 加 IP 选项 / Add IP options only to sock1
    // bind_device(sock1, "ipiptun");  // 可选绑定到特定接口 / Optionally bind to a specific interface

    sendto(sock1, "wogjowigojwe", 12, 0, ...);
    // 或: happy_socket((sockaddr*)&addr, sock1, sock2, 5, 250000); / Or: happy_socket(...)
    return 0;
}
```

---

## send.cpp —— TCP 连接 + MSS 测量工具 / TCP Connection + MSS Measurement Tool

### 用法与行为 / Usage and Behavior

```bash
./send <ip>[:port] <ip-option-size>
```

**示例：/ Examples:**
```bash
./send 192.168.10.2:1110 0     # 无选项（基线）/ No options (baseline)
./send 192.168.10.2:1110 4     # 4 字节选项 / 4-byte option
./send 192.168.10.2:1110 40    # 40 字节选项（最大）/ 40-byte option (maximum)
```

**输出：/ Output:**
```
addr: 192.168.10.2:1110
time: 0.01234
mss: 1448
```

### 源码逻辑 / Source Code Logic

```c
int main(int argc, char **argv) {
    // 解析目标 / Parse target
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = strchr(argv[1], ':') ? htons(atoi(port)) : htons(1110);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int size = atoi(argv[2]);
    add_ip_options(sock, size);       // 设置变长 IP 选项 / Set variable-length IP options

    double t1 = get_time();
    if (connect(sock, ...)) { perror("connect failed"); return -1; }
    printf("time: %lf\n", get_time() - t1);

    printf("mss: %d\n", get_mss(sock));  // TCP_MAXSEG
    close(sock);
}
```

`get_mss()`: `getsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, &mss, &len)` 获取内核协商的 MSS。MSS = 路径 MTU - 40 (TCP/IP 头) - IP 选项字节数（对齐后）。
`get_mss()`: `getsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, &mss, &len)` retrieves the kernel-negotiated MSS. MSS = Path MTU - 40 (TCP/IP header) - IP option bytes (after alignment).

---

## C++ 编译 / C++ Compilation

```bash
g++ -std=c++11 -o mysocket mysocket.cpp
g++ -std=c++11 -o send send.cpp
g++ -std=c++11 -o test test.cpp
```

**要求：/ Requirements:**
- GCC 4.8+ 支持 `-std=c++11` / GCC 4.8+ with `-std=c++11` support
- Linux 内核支持 `SO_BINDTODEVICE` / Linux kernel with `SO_BINDTODEVICE` support
- root 权限（`IP_OPTIONS` setsockopt 需要 `CAP_NET_RAW`）/ root privileges (`IP_OPTIONS` setsockopt requires `CAP_NET_RAW`)

---

## 实验指南 / Experiment Guide

### 实验 1: 测量不同 IP 选项大小的性能影响 / Experiment 1: Measure Performance Impact of Different IP Option Sizes

```bash
for size in 0 4 8 12 16 24 32 40; do
    echo "=== Size=$size ==="
    ./send 192.168.10.2:80 $size
done
```
预期：MSS 随 size 增大而递减，连接时间可能略有增加。
Expected: MSS decreases as size increases; connection time may increase slightly.

### 实验 2: Happy Eyeballs 效果对比 / Experiment 2: Happy Eyeballs Effect Comparison

修改 `mysocket.cpp` 的 `main()` 函数，分别测试：
Modify the `main()` function in `mysocket.cpp` to test:
- `happy_socket()` 多路径选择 / Multipath selection
- 单独 `connect(sock1)` 单路径 / Single path `connect(sock1)` alone
对比连接延迟和最终选用的路径。
Compare connection latency and the final path selected.

### 实验 3: 改变宽限期 / Experiment 3: Vary the Grace Period

```c
// 无等待（最激进——只要主路径成功立即用）/ No wait (most aggressive — use primary as soon as it succeeds)
int fd = happy_socket(addr, s1, s2, 5, 0);

// 长时间等待（最保守——尽可能用备用路径）/ Long wait (most conservative — prefer backup path whenever possible)
int fd = happy_socket(addr, s1, s2, 10, 2000000);  // 2s
```

---

## 注意事项 / Notes

- 实验 IP 选项 type=0xA0 非 IANA 标准，可能被中间设备丢弃 / Experimental IP option type=0xA0 is non-IANA standard, may be dropped by intermediate devices
- `SO_BINDTODEVICE` 需 `CAP_NET_RAW` 或 root / `SO_BINDTODEVICE` requires `CAP_NET_RAW` or root
- `select_socket()` 是递归的——在 2 个 socket 场景下最多递归 1 次，无栈溢出风险 / `select_socket()` is recursive — recurses at most once for 2 sockets, no stack overflow risk
- `wait2us` 微秒级参数在高负载系统上实际精度有限（受 `HZ` 和调度器影响）/ The microsecond-level `wait2us` parameter has limited precision on high-load systems (affected by `HZ` and the scheduler)

---

## 相关项目 / Related Projects

- **autoconfig** —— GRE 隧道作为备选物理路径 / GRE tunnels as alternative physical paths
- **mrtdb** —— 互联网规模 IP 选项连通性测试 / Internet-scale IP option connectivity testing
- **bird** —— BGP 路由 / BGP routing

---

## 附录 A: MSS 理论计算 / MSS Theoretical Calculation

MSS（Maximum Segment Size）= 路径 MTU - 40 (标准 TCP/IP 头) - IP 选项字节数（对齐到 4 字节）。

| 选项大小 | 对齐 | MSS(1500 MTU) | MSS(1450 MTU) |
|---------|------|-------------|-------------|
| 0 | 0 | 1460 | 1410 |
| 4 | 4 | 1456 | 1406 |
| 8 | 8 | 1452 | 1402 |
| 12 | 12 | 1448 | 1398 |
| 20 | 20 | 1440 | 1390 |
| 40 | 40 | 1420 | 1370 |

隧道内 MSS 更低：GRE overhead = 4(GRE) + 20(外层IP) = 24 字节。

## 附录 B: happy_socket() 决策矩阵 / happy_socket() Decision Matrix

| 场景 | sock1 结果 | sock2 结果 | 最终选择 | 原因 |
|------|----------|----------|---------|------|
| 1 | 成功 (0.5s) | 成功 (0.8s) | sock2 | 宽限期内备用也成功 → 优选 |
| 2 | 成功 (0.5s) | 成功 (1.5s) | sock1 | 过了宽限期 → 不等了 |
| 3 | 成功 (0.5s) | 超时 | sock1 | 备用失败 → 用主路径 |
| 4 | 成功 (0.8s) | 成功 (0.3s) | sock2 | 备用先成功 → 立即选 |
| 5 | 超时 | 成功 (0.3s) | sock2 | 主路径超时 → 唯一可用 |
| 6 | 超时 | 超时 | -1 | 都失败 |

## 附录 C: setsockopt 参数参考 / setsockopt Parameter Reference

```c
// IP 选项 (IPPROTO_IP level)
int optname = IP_OPTIONS;
void *optval = ip_options_bytes;
socklen_t optlen = sizeof(ip_options_bytes);
setsockopt(sock, IPPROTO_IP, optname, optval, optlen);

// 绑定到设备 (SOL_SOCKET level)
int optname = SO_BINDTODEVICE;
void *optval = "eth0";
socklen_t optlen = 4;
setsockopt(sock, SOL_SOCKET, optname, optval, optlen);

// 获取 TCP MSS (IPPROTO_TCP level)
int mss; socklen_t len = sizeof(mss);
getsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, &mss, &len);

// 获取 socket 错误
int so_error; socklen_t len = sizeof(so_error);
getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
```
