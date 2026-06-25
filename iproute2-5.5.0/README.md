# iproute2-5.5.0 —— 支持 IPswen 的 iproute2 修改版 / iproute2-5.5.0 — Modified Version with IPswen Support

## 概述 / Overview

**iproute2 5.5.0 修改版**——Linux 核心网络管理套件 `ip` 命令的 IPswen 感知版本。增加了对 IPswen 地址的解析（`ipswen_aton`）、格式化（`ipswen_ntoa`）和路由管理能力，是用户态与 IPswen 内核交互的主要接口。
**iproute2 5.5.0 Modified Version** — An IPswen-aware version of the Linux core network management suite `ip` command. It adds support for parsing (`ipswen_aton`), formatting (`ipswen_ntoa`), and route management of IPswen addresses, serving as the primary interface for user-space interaction with the IPswen kernel.

没有这个修改版，`ip route show` 将无法正确显示带 `(level)` 记法的 IPswen 路由，`ip route add` 也无法接受 IPswen 格式的前缀。
Without this modified version, `ip route show` cannot correctly display IPswen routes using the `(level)` notation, and `ip route add` cannot accept prefixes in IPswen format.

---

## 目录结构 / Directory Structure

```
iproute2-5.5.0/
├── ip/
│   └── iproute.c      # ★ 路由显示/增删（多处 #ifdef HAVE_IPswen） / Route display/add/delete (multiple #ifdef HAVE_IPswen)
├── lib/
│   └── utils.c        # ★ ipswen_aton() / ipswen_ntoa() 实现 / ipswen_aton() / ipswen_ntoa() implementation
├── include/           # 头文件 / Header files
├── tc/ bridge/ devlink/ genl/ tipc/ rdma/   # 其他 iproute2 子工具 / Other iproute2 sub-tools
├── bash-completion/   # Shell 补齐 / Shell completion
├── man/ doc/ examples/ misc/ testsuite/
├── configure          # 构建配置脚本（自动检测系统是否定义 HAVE_IPswen） / Build configuration script (auto-detects HAVE_IPswen)
├── Makefile
└── README             # 原始 iproute2 自带的 README / Original iproute2 README
```

---

## IPswen 修改源码详解 / IPswen Source Modifications Explained

### lib/utils.c —— 地址的解析和格式化 / lib/utils.c — Address Parsing and Formatting

这是整个 IPswen 地址处理的**核心函数**所在：
This file contains the **core functions** for all IPswen address handling:

```c
#ifdef HAVE_IPswen

// 字符串 → 二进制 / String → Binary
int ipswen_aton(struct ipswen_addr *addr, const char *text) {
    // 输入格式: "A.B.C.D(L)E.F.G.H" / Input format: "A.B.C.D(L)E.F.G.H"
    // 解析步骤: / Parsing steps:
    //   1. sscanf 提取 A.B.C.D 部分 → addr->base_ip / sscanf extracts A.B.C.D part → addr->base_ip
    //   2. 查找 '(' → 提取 level（带符号整数）→ addr->ext.level / Find '(' → extract level (signed integer) → addr->ext.level
    //   3. 根据 |level| 读取后续的扩展字节 → addr->ext.ext[0..|level|-1] / Read extension bytes based on |level| → addr->ext.ext[0..|level|-1]
    //   4. 验证 level ∈ [-IPSWEN_MAX_LEVEL, +IPSWEN_MAX_LEVEL] / Validate level ∈ [-IPSWEN_MAX_LEVEL, +IPSWEN_MAX_LEVEL]
    //   5. 验证扩展字节数量 = |level| / Validate extension byte count = |level|
    // 返回值: 0=成功, 非0=失败 / Return: 0=success, non-zero=failure
}

// 二进制 → 字符串 / Binary → String
char *ipswen_ntoa(const struct ipswen_addr *addr, char *buf, int len) {
    // 输出格式: "A.B.C.D(L)E.F.G.H" / Output format: "A.B.C.D(L)E.F.G.H"
    // 如果 level=0, 输出标准 "A.B.C.D" / If level=0, output standard "A.B.C.D"
}

#endif // HAVE_IPswen
```

这两个函数被 `ip/iproute.c` 中的路由显示和路由添加逻辑大量调用。它们是 IPswen 记法和内核二进制表示之间的翻译层。
These two functions are heavily called by the route display and route addition logic in `ip/iproute.c`. They serve as the translation layer between IPswen notation and the kernel's binary representation.

**设计考量：** / **Design Considerations:**
- `ipswen_aton()` 需要通过字符串解析来判断一个地址是标准 IPv4 还是 IPswen。如果 level=0 且无扩展字节，返回等价于标准 IPv4 解析的结果。
- `ipswen_aton()` needs to determine from string parsing whether an address is standard IPv4 or IPswen. If level=0 with no extension bytes, it returns a result equivalent to standard IPv4 parsing.
- `ipswen_ntoa()` 的缓冲区至少需要 `IPSWEN_ADDRSTRLEN` 字节（约 INET_ADDRSTRLEN + 8*IPSWEN_MAX_LEVEL）。
- `ipswen_ntoa()`'s buffer requires at least `IPSWEN_ADDRSTRLEN` bytes (approximately INET_ADDRSTRLEN + 8*IPSWEN_MAX_LEVEL).

### ip/iproute.c —— 路由处理中的 IPswen / ip/iproute.c — IPswen in Route Processing

这是一个拥有约 2500 行的大文件（标准 iproute2 中也如此）。IPswen 修改散布在约 10 个 `#ifdef HAVE_IPswen` 块中：
This is a large file with approximately 2500 lines (as in standard iproute2). IPswen modifications are scattered across about 10 `#ifdef HAVE_IPswen` blocks:

#### 路由显示时 / When Displaying Routes

```c
#ifdef HAVE_IPswen
    // 检查路由条目中是否有 RTA_DST_EXT 属性 / Check if RTA_DST_EXT attribute exists in the route entry
    if (tb[RTA_DST_EXT]) {
        struct ipswen_addr ips;
        ips.base_ip = *(u32 *)RTA_DATA(tb[RTA_DST]);
        ips.ext = *(struct ipswen_ext *)RTA_DATA(tb[RTA_DST_EXT]);
        ipswen_ntoa(&ips, gateway, sizeof(gateway));
        // 现在 gateway 字符串中包含了完整的 IPswen 记法 / Now gateway contains the full IPswen notation
    }
#endif
```

显示下一跳时： / When displaying the next hop:
```c
#ifdef HAVE_IPswen
    struct ipswen_addr ips;
    ips.ext = *(struct ipswen_ext *)RTA_DATA(rtb);
    ipswen_ntoa(&ips, gateway, sizeof(gateway));
#endif
```

#### 路由添加时 / When Adding Routes

```c
#ifdef HAVE_IPswen
    // 如果用户输入的地址包含 '(' 字符，说明是 IPswen 地址 / If the user-entered address contains '(', it's an IPswen address
    if (strchr(dest_str, '(')) {
        struct ipswen_addr ips;
        ipswen_aton(&ips, dest_str);
        // 将扩展部分放入 RTA_DST_EXT 属性 / Put the extension part into the RTA_DST_EXT attribute
        addattr_l(&req.n, sizeof(req), RTA_DST_EXT, &ips.ext, sizeof(ips.ext));
    }
#endif
```

#### Netlink 消息构造 / Netlink Message Construction

```c
#ifdef HAVE_IPswen
    // 在构造发送给内核的 Netlink 消息时，增加 RTA_DST_EXT / Add RTA_DST_EXT when constructing Netlink messages to the kernel
    addattr_l(&req.n, sizeof(req), RTA_DST_EXT, &ipswen_ext, sizeof(ipswen_ext));
#endif
```

`RTA_DST_EXT` 是一个自定义的 Netlink Routing Attribute（路由属性）。标准 Linux 内核不包含此属性——只有安装了 IPswen 补丁的内核才会处理它。非 IPswen 内核会忽略不认识的属性（Netlink 协议的容错设计），所以不会导致错误。
`RTA_DST_EXT` is a custom Netlink Routing Attribute. The standard Linux kernel does not include this attribute — only kernels with the IPswen patch will process it. Non-IPswen kernels will silently ignore unknown attributes (due to Netlink protocol's fault-tolerant design), so this does not cause errors.

---

## 编译 / Compilation

### 完整流程 / Full Build Process

```bash
cd iproute2-5.5.0

# 1. 检查依赖 / Check dependencies
dpkg -l | grep -E "libnl-3-dev|libnl-route-3-dev|bison|flex"
# 如果缺少： / If missing:
# sudo apt install libnl-3-dev libnl-route-3-dev bison flex

# 2. 确认 IPswen 头文件已安装 / Verify IPswen headers are installed
ls /usr/src/linux-headers-*/include/net/ipswen.h
# 如果没有，先安装: / If not, install first:
# sudo dpkg -i ../linux-kernel/linux-headers-*.deb

# 3. 配置 / Configure
./configure 2>&1 | tee configure.log

# 4. 验证 HAVE_IPswen 已启用 / Verify HAVE_IPswen is enabled
grep HAVE_IPswen config.h
# 应输出: #define HAVE_IPswen 1 / Expected output: #define HAVE_IPswen 1

# 5. 编译 / Build
make -j$(nproc) 2>&1 | tee build.log

# 6. 安装 / Install
sudo make install

# 7. 验证 / Verify
which ip
ip -V
```

### 常见编译错误 / Common Build Errors

| 错误 / Error | 原因 / Cause | 解决方案 / Solution |
|------|------|---------|
| `configure: error: Need libnl` | libnl 未安装 / libnl not installed | `sudo apt install libnl-3-dev libnl-route-3-dev` |
| `HAVE_IPswen not defined` | 头文件不含 IPswen / Headers lack IPswen | 安装 IPswen headers .deb / Install IPswen headers .deb |
| `implicit declaration of 'ipswen_aton'` | 头文件中缺少声明 / Missing declaration in headers | 确认 `ipswen.h` 存在且被 `utils.c` include / Verify `ipswen.h` exists and is included by `utils.c` |
| `bison: command not found` | Bison 未安装 / Bison not installed | `sudo apt install bison` |
| `flex: command not found` | Flex 未安装 / Flex not installed | `sudo apt install flex` |

---

## IPswen 感知命令 / IPswen-Aware Commands

```bash
# 添加一条带 IPswen 前缀的路由 / Add a route with an IPswen prefix
sudo ip route add 10.0.0.0(2)128.129/33 via 192.168.1.1

# 查看路由表（正确显示 IPswen 地址） / View routing table (correctly displays IPswen addresses)
ip route show
# 输出示例: 10.0.0.0(2)128.129/33 via 192.168.1.1 dev eth0 / Example output: 10.0.0.0(2)128.129/33 via 192.168.1.1 dev eth0

# 过滤 IPswen 路由 / Filter IPswen routes
ip route show | grep "("

# 为接口分配 IPswen 地址 / Assign an IPswen address to an interface
sudo ip addr add 10.0.0.1(1)200/25 dev eth0

# 查看接口地址 / View interface addresses
ip addr show eth0

# 删除 IPswen 路由 / Delete an IPswen route
sudo ip route del 10.0.0.0(2)128.129/33
```

---

## 如何修改实验参数 / How to Modify Experimental Parameters

### 修改 IPswen 地址格式 / Modify IPswen Address Format

如果需要改变地址记法（如从 `(L)` 改为 `:L:`），在 `lib/utils.c` 的 `ipswen_aton()` 中修改：
If you need to change the address notation (e.g., from `(L)` to `:L:`), modify `ipswen_aton()` in `lib/utils.c`:
```c
// 原: 查找 '('  / Original: find '('
char *level_str = strchr(text, '(');
// 新: 查找 ':' / New: find ':'
char *level_str = strchr(text, ':');
```

### 添加新的路由属性 / Add New Routing Attributes

在 `ip/iproute.c` 中找到 `#ifdef HAVE_IPswen` 块，参考 `RTA_DST_EXT` 的处理模式，添加新的 `addattr_l()` 调用。
Find the `#ifdef HAVE_IPswen` blocks in `ip/iproute.c` and follow the `RTA_DST_EXT` pattern to add new `addattr_l()` calls.

### 调试 Netlink 交互 / Debug Netlink Interaction

```bash
# 追踪 ip 命令发送和接收的 Netlink 消息 / Trace Netlink messages sent and received by ip command
sudo strace -f -e trace=sendmsg,recvmsg -s 512 \
    ip route add 10.0.0.0(2)128.129/33 via 192.168.1.1

# 查看原始 Netlink 消息 / View raw Netlink messages
sudo strace -f -e trace=sendto -s 256 ip route show
```

---

## 注意事项 / Important Notes

- IPswen 支持是**条件编译**——在无 IPswen 头文件的系统上，编译结果等同于标准 iproute2 5.5.0
- IPswen support is **conditionally compiled** — on systems without IPswen headers, the build result is identical to standard iproute2 5.5.0
- `RTA_DST_EXT` 是自定义 Netlink 属性，**内核也必须支持**（需要 IPswen 内核补丁）
- `RTA_DST_EXT` is a custom Netlink attribute — the **kernel must also support it** (requires the IPswen kernel patch)
- iproute2 5.5.0 是特定版本，可能与比它新的内核不完全兼容（但与 5.4.0 内核匹配良好）
- iproute2 5.5.0 is a specific version that may not be fully compatible with newer kernels (but works well with the 5.4.0 kernel)
- 安装后可能会**覆盖系统原有的 `ip` 命令**——建议先备份 `/sbin/ip`
- Installing may **overwrite the system's original `ip` command** — it is recommended to back up `/sbin/ip` first

---

## 相关项目 / Related Projects

- **linux-kernel** (`../linux-kernel/`) —— IPswen 内核（提供 `ipswen_addr` 结构体和 `RTA_DST_EXT` 属性处理） / IPswen kernel (provides `ipswen_addr` struct and `RTA_DST_EXT` attribute handling)
- **net-tools-1.60** (`../net-tools-1.60/`) —— 传统 `ifconfig`/`route` 的 IPswen 版 / IPswen version of traditional `ifconfig`/`route`
- **bird** (`../bird/`) —— 通过 `ip route` 安装路由的 BGP 守护进程 / BGP daemon that installs routes via `ip route`

---

## 附录 A: IPswen 路由操作速查 / Appendix A: IPswen Route Operations Quick Reference

```bash
# 添加路由 / Add route
ip route add <prefix> via <gw>
ip route add 10.0.0.0(2)128.129/33 via 192.168.1.1

# 查看 / View
ip route show
ip route show | grep "("
ip route show table all

# 删除 / Delete
ip route del 10.0.0.0(2)128.129/33

# 接口地址 / Interface address
ip addr add 10.0.0.1(1)200/25 dev eth0
ip addr show dev eth0

# 验证内核接收 / Verify kernel reception
cat /proc/net/fib_trie | grep "("
cat /proc/net/route
```

## 附录 B: 编译验证清单 / Appendix B: Build Verification Checklist

```bash
# 编译后检查 / Post-build checks
file ip/ip                # ELF 64-bit
ldd ip/ip                 # 检查动态链接 / Check dynamic linking
strings ip/ip | grep -i ipswen  # 确认包含 IPswen 符号 / Verify IPswen symbols are included
./ip/ip -V                # 版本 / Version
sudo ./ip/ip route add 10.0.0.0(2)128.129/33 via 127.0.0.1 2>&1
# 成功/失败消息说明 IPswen 代码路径已进入 / Success/failure message indicates IPswen code path was triggered
```

## 附录 C: 与内核 Netlink 交互数据流 / Appendix C: Netlink Interaction Data Flow with Kernel

```
用户: ip route add "10.0.0.0(2)128.129/33" via 1.1.1.1 / User: ip route add "10.0.0.0(2)128.129/33" via 1.1.1.1
  │
  ├─ ipswen_aton() 解析 → struct ipswen_addr / ipswen_aton() parses → struct ipswen_addr
  ├─ 构造 RTM_NEWROUTE Netlink 消息 / Construct RTM_NEWROUTE Netlink message
  │   ├─ RTA_DST: 10.0.0.0 (标准属性 / standard attribute)
  │   ├─ RTA_DST_EXT: {level=2, ext=[128,129]} (IPswen 属性 / IPswen attribute)
  │   └─ RTA_GATEWAY: 1.1.1.1 (标准属性 / standard attribute)
  ├─ sendmsg() → 内核 / sendmsg() → kernel
  │
内核: / Kernel:
  ├─ rtnetlink_rcv_msg() 接收 / rtnetlink_rcv_msg() receives
  ├─ 解析 RTA_DST_EXT / Parse RTA_DST_EXT
  ├─ fib_table_insert() → FIB
  └─ 返回 0 (成功) / Return 0 (success)
```
