# unpv13e —— Unix 网络编程示例集（含 IPSWEN Ping）

## 概述

这是 W. Richard Stevens 经典著作《Unix Network Programming, Volume 1, Third Edition》（简称 UNP）配套示例代码的修改版。原书是全球网络编程领域的圣经级参考资料——全面覆盖 TCP/UDP、Socket API、路由、多播、信号驱动 I/O、线程等主题。

本版本在 `ping` 工具中集成了 IPSWEN 地址支持。其余 20+ 章的代码保持原样，作为标准网络编程的可信参考基线，用于与 IPSWEN 修改代码进行对照验证。

---

## 目录结构

```
unpv13e/
├── ping/             # ★ ICMP Ping（IPSWEN 修改所在）
│   ├── main.c        # EXPERIMENTAL_IPSWEN 宏，ipswen_aton/ntoa 调用
│   ├── ping.h        # 协议头
│   └── old/          # 旧版 ping
│
├── lib/              # 共享函数库（wrap 系统调用，错误处理）
├── libfree/          # 替代/自由实现函数
├── libgai/           # getaddrinfo 实现
├── libroute/         # 路由 socket 库
│
├── tcpcliserv/       # TCP 客户端/服务器（第5章）
├── udpcliserv/       # UDP 客户端/服务器（第8章）
├── select/           # I/O 多路复用（第6章）
├── server/           # 并发服务器设计（第27章）
├── ioctl/            # ioctl 操作（第17章）
├── route/            # 路由 socket（第18章）
├── key/              # 密钥管理 socket（第19章）
├── sock/             # 原始 socket（第28章）
├── sockopt/          # Socket 选项（第7章）
├── ipopts/           # IP 选项（第27章）★ IPSWEN 直接相关
├── traceroute/       # Traceroute（第28章）
├── advio/            # 高级 I/O（第14章）
├── nonblock/         # 非阻塞 I/O（第16章）★ Happy Eyeballs 参考
├── sigio/            # SIGIO 驱动 I/O（第25章）
├── threads/          # 线程（第26章）
├── bcast/            # 广播（第20章）
├── mcast/            # 多播（第21章）
├── unixdomain/       # Unix 域 socket（第15章）
├── names/            # 名称/地址转换（第11章）
├── oob/              # 带外数据（第24章）
├── inetd/            # inetd 守护（第13章）
├── rtt/              # RTT 测量（第28章）
├── icmpd/            # ICMP 守护（第28章）
├── sctp/             # SCTP（第23章）
├── ssntp/            # SNTP 客户端（第28章）
├── streams/          # STREAMS
├── test/             # 测试程序
│
├── config.h.in       # 配置模板（#define HAVE_IPSWEN 1）
├── configure / configure.in
├── Makefile.in / Make.defines.in
├── aclocal.m4 / config.guess / config.sub
├── VERSION / README / LICENSE
```

---

## IPSWEN 修改详解

### ping/main.c —— 地址解析流

```c
#define EXPERIMENTAL_IPSWEN    // ★ 开启 IPSWEN 路径

int main(int argc, char **argv) {
    // ... 参数解析 ...

    host = argv[optind];        // 目标主机名或地址

#ifndef EXPERIMENTAL_IPSWEN
    // === 标准路径 ===
    struct hostent *hp = gethostbyname(host);
    // 或 inet_aton(host, &addr)
#else
    // === IPSWEN 增强路径 ===
    struct addrinfo *ai;

    // 先尝试 IPSWEN 解析
    if (!ipswen_aton(&((struct sockaddr_in *)ai->ai_addr)->swen_addr, host)) {
        // 成功解析为 IPSWEN 格式
        // → 直接使用解析结果，不经过 DNS
    } else {
        // 回退：先尝试标准 gethostbyname
        // 再尝试标准 inet_aton
    }

    // 回显解析结果
    ipswen_ntoa(&..., buffer, sizeof(buffer));
    printf("PING %s ...\n", buffer);
#endif
}
```

**解析优先级：** IPSWEN 记法 → gethostbyname (DNS) → inet_aton (标准 IPv4)
这种设计确保了：
- 输入 `10.0.0.1(2)128.129` 被识别为 IPSWEN 地址
- 输入 `www.example.com` 走 DNS 解析
- 输入 `192.168.1.1` 走标准 IPv4 解析

**为什么放在 ping 中？** ping 是最常用的网络诊断工具。通过在 ping 中集成 IPSWEN，研究人员可以直接用 `ping 10.0.0.1(2)128.129` 测试 IPSWEN 地址的连通性——无需任何额外的测试工具。

### config.h.in

```c
/* Define to 1 if the system supports IPSWEN */
#define HAVE_IPSWEN 1
```

默认启用。`configure` 脚本执行时会检查系统头文件，确认 `ipswen_aton`/`ipswen_ntoa` 函数可用。

---

## 各章示例与 IPSWEN 研究的关联

| 目录 | 章节主题 | 对 IPSWEN 研究的作用 |
|------|---------|-------------------|
| `ping/` | ICMP Ping | **已修改为支持 IPSWEN 地址** |
| `ipopts/` | IP 选项 | IP 选项 socket API 的标准参考——理解 `setsockopt(IP_OPTIONS)` 的正确用法 |
| `traceroute/` | 路由追踪 | 标准 traceroute 实现，可对比 IPSWEN traceroute |
| `sockopt/` | Socket 选项 | `IP_OPTIONS`、`SO_BINDTODEVICE`、`SO_ERROR` 的权威参考 |
| `nonblock/` | 非阻塞 I/O | 异步 connect 模式——happyfootball 的 `select_socket()` 即基于此模式 |
| `route/` | 路由 Socket | 内核路由表交互——理解 IPSWEN FIB 的用户态视角 |
| `tcpcliserv/` | TCP C/S | TCP 标准实现基线 |
| `udpcliserv/` | UDP C/S | UDP 标准实现基线 |
| `select/` | I/O 多路复用 | `select()` 的标准用法 |
| `advio/` | 高级 I/O | `sendfile`、`splice` 等 |

---

## 编译与使用

### 编译全部

```bash
cd unpv13e
./configure
make -j$(nproc)
```

### 仅编译 ping

```bash
cd ping
make ping
```

### IPSWEN Ping 使用

```bash
# Ping 一个 IPSWEN 地址（level=2, 扩展字节 128.129）
sudo ./ping 10.0.0.1(2)128.129

# Ping 标准 IPv4（向后兼容）
sudo ./ping 192.168.1.1

# Ping DNS 名称
sudo ./ping www.example.com

# 带 verbose
sudo ./ping -v 10.0.0.1(2)128.129
```

### 运行其他示例

```bash
# TCP echo 客户端
cd tcpcliserv
./tcpserv01 &     # 启动服务器
./tcpcli01 127.0.0.1
```

---

## 实验定制指南

### 怎样暂时禁用 IPSWEN

注释掉 `ping/main.c` 中的 `#define EXPERIMENTAL_IPSWEN`，重新编译。这将使 ping 恢复标准 IPv4-only 行为。

### 怎样在 traceroute 中加入 IPSWEN

参考 `ping/main.c` 的 `#ifdef EXPERIMENTAL_IPSWEN` 模式，在 `traceroute/main.c` 的目标地址解析部分加入相同逻辑。

---

## 注意事项

- `EXPERIMENTAL_IPSWEN` 宏硬编码在 `main.c`——不需要修改 `config.h` 或其他头文件
- `ipswen_aton()` 和 `ipswen_ntoa()` 是**外部函数**——由系统的 C 库或 IPSWEN 支持库提供（通常来自内核头文件编译的支持库）
- 如果系统没有安装 IPSWEN 头文件，ping 编译会因缺少这些符号而失败——此时需注释掉 `#define EXPERIMENTAL_IPSWEN`
- 其余 20+ 章代码是**原始 unpv13e**——它们是标准网络编程的权威参考，不受 IPSWEN 修改影响

---

## 相关项目

- **linux-kernel** (`../linux-kernel/`) —— 提供 IPSWEN Socket API 的内核
- **happyfootball** (`../happyfootball/`) —— 基于非阻塞 connect 的地址选择实验
- **iproute2 / net-tools** —— IPSWEN 用户态工具链
- **bird** —— IPSWEN BGP 路由

---

## 附录 A: 完整目章节索引

| 章节 | 目录 | 内容 | IPSWEN关联 |
|------|------|------|-----------|
| 1 | intro/ | 简介 | 无 |
| 3 | sock/ | 原始Socket | 无 |
| 4 | tcpcliserv/ | TCP 基本 | 基线参考 |
| 5 | tcpcliserv/ | TCP C/S 示例 | 基线参考 |
| 6 | select/ | I/O 多路复用 | 低 |
| 7 | sockopt/ | Socket 选项 | ★ 高 (IP_OPTIONS, SO_BINDTODEVICE) |
| 8 | udpcliserv/ | UDP 基本 | 基线参考 |
| 11 | names/ | 地址转换 | 中 (gethostbyname vs ipswen_aton) |
| 14 | advio/ | 高级 I/O | 低 |
| 16 | nonblock/ | 非阻塞 I/O | ★ 高 (异步connect参考) |
| 17 | ioctl/ | ioctl | 中 |
| 18 | route/ | 路由 Socket | ★ 高 (内核FIB交互) |
| 20 | bcast/ | 广播 | 低 |
| 21 | mcast/ | 多播 | 低 |
| 24 | oob/ | 带外数据 | 低 |
| 25 | sigio/ | 信号驱动 | 低 |
| 26 | threads/ | 线程 | 低 |
| 27 | server/ | 并发服务器 | 中 |
| 27 | ipopts/ | IP 选项 | ★ 极高 |
| 28 | traceroute/ | 路由追踪 | ★ 极高 |
| 28 | ping/ | Ping | ★ 已修改 |
| 28 | icmpd/ | ICMP 守护 | 中 |

## 附录 B: 如何新增 IPSWEN 支持到其他示例

以 `traceroute` 为例：
1. 打开 `traceroute/main.c`
2. 在地址解析处添加：
```c
#ifdef EXPERIMENTAL_IPSWEN
    if (!ipswen_aton(&sin->swen_addr, host)) {
        // 使用 IPSWEN 解析
    } else
#endif
    // 原有 gethostbyname 回退
```
3. 重新编译：`cd traceroute && make`

## 附录 C: 标准网络编程速记

```c
// 创建 TCP socket
int sock = socket(AF_INET, SOCK_STREAM, 0);

// 设置 IP 选项
setsockopt(sock, IPPROTO_IP, IP_OPTIONS, opt, optlen);

// 绑定到网口
setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, "eth0", 4);

// 非阻塞 connect
fcntl(sock, F_SETFL, O_NONBLOCK);
connect(sock, ...);
select(sock+1, NULL, &wfds, NULL, &tv);
int err; getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len);
```
