# happyfootball —— Happy Eyeballs 多路径连接 + IP 选项实验

## 概述

`happyfootball` 是 RFC 8305 (Happy Eyeballs v2) 的实验性扩展——在标准 Happy Eyeballs（同时尝试 IPv4 和 IPv6、选用先成功的）基础上，增加了对 **IP 选项**（含实验性自定义类型）和 **多网口绑定**（`SO_BINDTODEVICE`）的支持。

**核心问题：** 当到达同一目的地存在多条路径（不同物理接口、不同 IP 选项配置），如何选择？指标是 TCP 连接建立速度和端到端性能（MSS、吞吐量）。

---

## 目录结构

```
happyfootball/
├── mysocket.hpp           # C++ HappyTCP 类接口声明
├── mysocket.cpp           # ★ 核心 happy_socket() 算法完整实现
├── send.cpp / send        # TCP connect + MSS 测量 CLI 工具
├── test.cpp / test        # 非阻塞 TCP connect 测试
├── getmss.py              # 基于 Scapy 的 SYN→MSS 探测
├── ping.py                # 定时 GRE Ping 调度器
├── httpserver.py          # 简单 HTTP 测试端点 (端口 80)
├── recv.py                # TCP 接收吞吐量测量
├── 1.ipynb                # Jupyter notebook（实验记录）
└── .vscode/
```

---

## 核心算法：happy_socket() 完整源码走读

### 函数签名

```c
int happy_socket(
    const struct sockaddr *addr,  // 目标地址
    int sock1,                     // 主路径 socket
    int sock2,                     // 备用路径 socket
    int timeout,                   // 首个连接最长等几秒
    int wait2us                    // 主路径成功后的宽限期（微秒）
);
```

### 参数语义表

| 参数 | 典型值 | 含义 |
|------|--------|------|
| `addr` | `192.168.10.2:12345` | 目标 IP:Port |
| `sock1` | `socket(AF_INET, SOCK_STREAM, 0)` | 主路径（通常更快但不一定有 IP 选项） |
| `sock2` | `socket(AF_INET, SOCK_DGRAM, 0)` | 备用路径（通常更优但因 IP 选项可能较慢） |
| `timeout` | 5 | 如果 5 秒内没有任何一个连接成功 → 放弃 |
| `wait2us` | 250000（=250ms） | sock1 先成功后，再等 sock2 最多 250ms |

### 完整步骤分解

**Step 0: 获取时间戳**
```c
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;  // 纳秒精度
}
```
用于 `log()` 宏打印带时间戳的日志。

**Step 1: 设非阻塞**
```c
long arg = fcntl(sock1, F_GETFL, NULL);
arg |= O_NONBLOCK;
fcntl(sock1, F_SETFL, arg);
// sock2 同理（重复上述代码）
```

**Step 2: 同时发起 connect**
```c
connect(sock1, addr, sizeof(*addr));  // 非阻塞 connect 立即返回 EINPROGRESS
connect(sock2, addr, sizeof(*addr));
```
两个 connect 并行执行，不等待彼此。

**Step 3: select 等待可写**
```c
std::vector<int> socks = {sock1, sock2};
struct timeval tv1 = {timeout, 0};   // 最长等 timeout 秒
int sock = select_socket(socks, tv1);
```

**`select_socket()` 内部逻辑：**
```c
int select_socket(std::vector<int> &socks, struct timeval& tv) {
    int sock = wait_writeable(socks, tv);  // fd_set + select()
    if (sock == -1) {         // 超时：全失败
        for (int s : socks) close(s);
        return -1;
    }
    socks.erase(std::remove(socks.begin(), socks.end(), sock), socks.end());
    int soerr = get_socketerror(sock);    // 检查 SO_ERROR
    if (soerr == 0) {
        return sock;          // connect 真正成功
    } else {
        close(sock);          // connect 假成功（select 可写但实际错误）
        if (socks.empty()) return -1;
        return select_socket(socks, tv);  // 递归等剩下的
    }
}
```

`wait_writeable()` 是 `select()` 封装：构建 fd_set，调用 `select(socks.max()+1, NULL, &fd_set, NULL, &tv)`，返回第一个可写的 fd。

**Step 4: 宽限期决策**
```c
if (sock == sock1) {                     // 主路径先成功
    log("wait for sock2");
    struct timeval tv2 = {0, wait2us};   // 等 250ms
    if (select_socket(socks, tv2) == sock2) {
        log("use sock2");                // 备用路径也成功了 → 选它！
        sock = sock2;
    } else {
        log("use sock1");                // 超时 → 选主路径
    }
} else if (sock == sock2) {
    log("use sock2");                    // 备用先成功 → 直接用
} else {
    log("use nothing");                  // 都失败了
}
```

**设计哲学：**
- **偏向 sock2（备用/更好的路径）**：只要 sock2 也成功（即使在主路径之后），就选它
- **不牺牲延迟**：只在主路径先成功时才等待，如果备用先成功就不等
- **宽限期极短**：250ms 对于人类感知几乎无影响，但足够让第二个 TCP 握手完成

**Step 5: 清理**
```c
for (int sock : socks) close(sock);  // 关闭未使用的 socket
return sock;                          // 返回选中的 fd
```

---

## 辅助函数详解

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

### bind_device()
```c
void bind_device(int sock, const char *device) {
    setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, device, strlen(device));
}
```
将 socket 绑定到特定网口（如 `"eth0"`、`"tun-2531"`）。这样两个 socket 可走不同的物理路径。

### add_ip_options()
```c
void add_ip_options(int sock, int size) {
    unsigned char ip_options[64] = {0};
    ip_options[0] = 0b10100000;      // Copy-on-Frag=1, Class=2(Debug), Number=0
    ip_options[1] = size + 1;        // 长度（含自身）
    setsockopt(sock, IPPROTO_IP, IP_OPTIONS, ip_options, size + 1);
}
```

IP Option Type 字节 (0xA0) 的位结构：
```
Bit 7 (0x80): Copy-on-Fragmentation = 1 (所有分片都携带此选项)
Bit 6-5:      Option Class = 2 (Debug/Measurement)
Bit 4-0:      Option Number = 0
```

### main() 演示
```c
int main() {
    // 准备目标
    sockaddr_in addr; addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr("192.168.10.2");

    // 创建两个 socket
    int sock1 = socket(AF_INET, SOCK_DGRAM, 0);
    int sock2 = socket(AF_INET, SOCK_STREAM, 0);

    add_ip_options(sock1);       // 只在 sock1 加 IP 选项
    // bind_device(sock1, "ipiptun");  // 可选绑定到特定接口

    sendto(sock1, "wogjowigojwe", 12, 0, ...);
    // 或: happy_socket((sockaddr*)&addr, sock1, sock2, 5, 250000);
    return 0;
}
```

---

## send.cpp —— TCP 连接 + MSS 测量工具

### 用法与行为

```bash
./send <ip>[:port] <ip-option-size>
```

**示例：**
```bash
./send 192.168.10.2:1110 0     # 无选项（基线）
./send 192.168.10.2:1110 4     # 4 字节选项
./send 192.168.10.2:1110 40    # 40 字节选项（最大）
```

**输出：**
```
addr: 192.168.10.2:1110
time: 0.01234
mss: 1448
```

### 源码逻辑

```c
int main(int argc, char **argv) {
    // 解析目标
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_port = strchr(argv[1], ':') ? htons(atoi(port)) : htons(1110);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int size = atoi(argv[2]);
    add_ip_options(sock, size);       // 设置变长 IP 选项

    double t1 = get_time();
    if (connect(sock, ...)) { perror("connect failed"); return -1; }
    printf("time: %lf\n", get_time() - t1);

    printf("mss: %d\n", get_mss(sock));  // TCP_MAXSEG
    close(sock);
}
```

`get_mss()`: `getsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, &mss, &len)` 获取内核协商的 MSS。MSS = 路径 MTU - 40 (TCP/IP 头) - IP 选项字节数（对齐后）。

---

## C++ 编译

```bash
g++ -std=c++11 -o mysocket mysocket.cpp
g++ -std=c++11 -o send send.cpp
g++ -std=c++11 -o test test.cpp
```

**要求：**
- GCC 4.8+ 支持 `-std=c++11`
- Linux 内核支持 `SO_BINDTODEVICE`
- root 权限（`IP_OPTIONS` setsockopt 需要 `CAP_NET_RAW`）

---

## 实验指南

### 实验 1: 测量不同 IP 选项大小的性能影响

```bash
for size in 0 4 8 12 16 24 32 40; do
    echo "=== Size=$size ==="
    ./send 192.168.10.2:80 $size
done
```
预期：MSS 随 size 增大而递减，连接时间可能略有增加。

### 实验 2: Happy Eyeballs 效果对比

修改 `mysocket.cpp` 的 `main()` 函数，分别测试：
- `happy_socket()` 多路径选择
- 单独 `connect(sock1)` 单路径
对比连接延迟和最终选用的路径。

### 实验 3: 改变宽限期

```c
// 无等待（最激进——只要主路径成功立即用）
int fd = happy_socket(addr, s1, s2, 5, 0);

// 长时间等待（最保守——尽可能用备用路径）
int fd = happy_socket(addr, s1, s2, 10, 2000000);  // 2s
```

---

## 注意事项

- 实验 IP 选项 type=0xA0 非 IANA 标准，可能被中间设备丢弃
- `SO_BINDTODEVICE` 需 `CAP_NET_RAW` 或 root
- `select_socket()` 是递归的——在 2 个 socket 场景下最多递归 1 次，无栈溢出风险
- `wait2us` 微秒级参数在高负载系统上实际精度有限（受 `HZ` 和调度器影响）

---

## 相关项目

- **autoconfig** —— GRE 隧道作为备选物理路径
- **mrtdb** —— 互联网规模 IP 选项连通性测试
- **bird** —— BGP 路由

---

## 附录 A: MSS 理论计算

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

## 附录 B: happy_socket() 决策矩阵

| 场景 | sock1 结果 | sock2 结果 | 最终选择 | 原因 |
|------|----------|----------|---------|------|
| 1 | 成功 (0.5s) | 成功 (0.8s) | sock2 | 宽限期内备用也成功 → 优选 |
| 2 | 成功 (0.5s) | 成功 (1.5s) | sock1 | 过了宽限期 → 不等了 |
| 3 | 成功 (0.5s) | 超时 | sock1 | 备用失败 → 用主路径 |
| 4 | 成功 (0.8s) | 成功 (0.3s) | sock2 | 备用先成功 → 立即选 |
| 5 | 超时 | 成功 (0.3s) | sock2 | 主路径超时 → 唯一可用 |
| 6 | 超时 | 超时 | -1 | 都失败 |

## 附录 C: setsockopt 参数参考

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
