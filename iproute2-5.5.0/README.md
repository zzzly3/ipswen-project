# iproute2-5.5.0 —— 支持 IPSWEN 的 iproute2 修改版

## 概述

**iproute2 5.5.0 修改版**——Linux 核心网络管理套件 `ip` 命令的 IPSWEN 感知版本。增加了对 IPSWEN 地址的解析（`ipswen_aton`）、格式化（`ipswen_ntoa`）和路由管理能力，是用户态与 IPSWEN 内核交互的主要接口。

没有这个修改版，`ip route show` 将无法正确显示带 `(level)` 记法的 IPSWEN 路由，`ip route add` 也无法接受 IPSWEN 格式的前缀。

---

## 目录结构

```
iproute2-5.5.0/
├── ip/
│   └── iproute.c      # ★ 路由显示/增删（多处 #ifdef HAVE_IPSWEN）
├── lib/
│   └── utils.c        # ★ ipswen_aton() / ipswen_ntoa() 实现
├── include/           # 头文件
├── tc/ bridge/ devlink/ genl/ tipc/ rdma/   # 其他 iproute2 子工具
├── bash-completion/   # Shell 补齐
├── man/ doc/ examples/ misc/ testsuite/
├── configure          # 构建配置脚本（自动检测系统是否定义 HAVE_IPSWEN）
├── Makefile
└── README             # 原始 iproute2 自带的 README
```

---

## IPSWEN 修改源码详解

### lib/utils.c —— 地址的解析和格式化

这是整个 IPSWEN 地址处理的**核心函数**所在：

```c
#ifdef HAVE_IPSWEN

// 字符串 → 二进制
int ipswen_aton(struct ipswen_addr *addr, const char *text) {
    // 输入格式: "A.B.C.D(L)E.F.G.H"
    // 解析步骤:
    //   1. sscanf 提取 A.B.C.D 部分 → addr->base_ip
    //   2. 查找 '(' → 提取 level（带符号整数）→ addr->ext.level
    //   3. 根据 |level| 读取后续的扩展字节 → addr->ext.ext[0..|level|-1]
    //   4. 验证 level ∈ [-IPSWEN_MAX_LEVEL, +IPSWEN_MAX_LEVEL]
    //   5. 验证扩展字节数量 = |level|
    // 返回值: 0=成功, 非0=失败
}

// 二进制 → 字符串
char *ipswen_ntoa(const struct ipswen_addr *addr, char *buf, int len) {
    // 输出格式: "A.B.C.D(L)E.F.G.H"
    // 如果 level=0, 输出标准 "A.B.C.D"
}

#endif // HAVE_IPSWEN
```

这两个函数被 `ip/iproute.c` 中的路由显示和路由添加逻辑大量调用。它们是 IPSWEN 记法和内核二进制表示之间的翻译层。

**设计考量：**
- `ipswen_aton()` 需要通过字符串解析来判断一个地址是标准 IPv4 还是 IPSWEN。如果 level=0 且无扩展字节，返回等价于标准 IPv4 解析的结果。
- `ipswen_ntoa()` 的缓冲区至少需要 `IPSWEN_ADDRSTRLEN` 字节（约 INET_ADDRSTRLEN + 8*IPSWEN_MAX_LEVEL）。

### ip/iproute.c —— 路由处理中的 IPSWEN

这是一个拥有约 2500 行的大文件（标准 iproute2 中也如此）。IPSWEN 修改散布在约 10 个 `#ifdef HAVE_IPSWEN` 块中：

#### 路由显示时

```c
#ifdef HAVE_IPSWEN
    // 检查路由条目中是否有 RTA_DST_EXT 属性
    if (tb[RTA_DST_EXT]) {
        struct ipswen_addr ips;
        ips.base_ip = *(u32 *)RTA_DATA(tb[RTA_DST]);
        ips.ext = *(struct ipswen_ext *)RTA_DATA(tb[RTA_DST_EXT]);
        ipswen_ntoa(&ips, gateway, sizeof(gateway));
        // 现在 gateway 字符串中包含了完整的 IPSWEN 记法
    }
#endif
```

显示下一跳时：
```c
#ifdef HAVE_IPSWEN
    struct ipswen_addr ips;
    ips.ext = *(struct ipswen_ext *)RTA_DATA(rtb);
    ipswen_ntoa(&ips, gateway, sizeof(gateway));
#endif
```

#### 路由添加时

```c
#ifdef HAVE_IPSWEN
    // 如果用户输入的地址包含 '(' 字符，说明是 IPSWEN 地址
    if (strchr(dest_str, '(')) {
        struct ipswen_addr ips;
        ipswen_aton(&ips, dest_str);
        // 将扩展部分放入 RTA_DST_EXT 属性
        addattr_l(&req.n, sizeof(req), RTA_DST_EXT, &ips.ext, sizeof(ips.ext));
    }
#endif
```

#### Netlink 消息构造

```c
#ifdef HAVE_IPSWEN
    // 在构造发送给内核的 Netlink 消息时，增加 RTA_DST_EXT
    addattr_l(&req.n, sizeof(req), RTA_DST_EXT, &ipswen_ext, sizeof(ipswen_ext));
#endif
```

`RTA_DST_EXT` 是一个自定义的 Netlink Routing Attribute（路由属性）。标准 Linux 内核不包含此属性——只有安装了 IPSWEN 补丁的内核才会处理它。非 IPSWEN 内核会忽略不认识的属性（Netlink 协议的容错设计），所以不会导致错误。

---

## 编译

### 完整流程

```bash
cd iproute2-5.5.0

# 1. 检查依赖
dpkg -l | grep -E "libnl-3-dev|libnl-route-3-dev|bison|flex"
# 如果缺少：
# sudo apt install libnl-3-dev libnl-route-3-dev bison flex

# 2. 确认 IPSWEN 头文件已安装
ls /usr/src/linux-headers-*/include/net/ipswen.h
# 如果没有，先安装:
# sudo dpkg -i ../linux-kernel/linux-headers-*.deb

# 3. 配置
./configure 2>&1 | tee configure.log

# 4. 验证 HAVE_IPSWEN 已启用
grep HAVE_IPSWEN config.h
# 应输出: #define HAVE_IPSWEN 1

# 5. 编译
make -j$(nproc) 2>&1 | tee build.log

# 6. 安装
sudo make install

# 7. 验证
which ip
ip -V
```

### 常见编译错误

| 错误 | 原因 | 解决方案 |
|------|------|---------|
| `configure: error: Need libnl` | libnl 未安装 | `sudo apt install libnl-3-dev libnl-route-3-dev` |
| `HAVE_IPSWEN not defined` | 头文件不含 IPSWEN | 安装 IPSWEN headers .deb |
| `implicit declaration of 'ipswen_aton'` | 头文件中缺少声明 | 确认 `ipswen.h` 存在且被 `utils.c` include |
| `bison: command not found` | Bison 未安装 | `sudo apt install bison` |
| `flex: command not found` | Flex 未安装 | `sudo apt install flex` |

---

## IPSWEN 感知命令

```bash
# 添加一条带 IPSWEN 前缀的路由
sudo ip route add 10.0.0.0(2)128.129/33 via 192.168.1.1

# 查看路由表（正确显示 IPSWEN 地址）
ip route show
# 输出示例: 10.0.0.0(2)128.129/33 via 192.168.1.1 dev eth0

# 过滤 IPSWEN 路由
ip route show | grep "("

# 为接口分配 IPSWEN 地址
sudo ip addr add 10.0.0.1(1)200/25 dev eth0

# 查看接口地址
ip addr show eth0

# 删除 IPSWEN 路由
sudo ip route del 10.0.0.0(2)128.129/33
```

---

## 如何修改实验参数

### 修改 IPSWEN 地址格式

如果需要改变地址记法（如从 `(L)` 改为 `:L:`），在 `lib/utils.c` 的 `ipswen_aton()` 中修改：
```c
// 原: 查找 '(' 
char *level_str = strchr(text, '(');
// 新: 查找 ':'
char *level_str = strchr(text, ':');
```

### 添加新的路由属性

在 `ip/iproute.c` 中找到 `#ifdef HAVE_IPSWEN` 块，参考 `RTA_DST_EXT` 的处理模式，添加新的 `addattr_l()` 调用。

### 调试 Netlink 交互

```bash
# 追踪 ip 命令发送和接收的 Netlink 消息
sudo strace -f -e trace=sendmsg,recvmsg -s 512 \
    ip route add 10.0.0.0(2)128.129/33 via 192.168.1.1

# 查看原始 Netlink 消息
sudo strace -f -e trace=sendto -s 256 ip route show
```

---

## 注意事项

- IPSWEN 支持是**条件编译**——在无 IPSWEN 头文件的系统上，编译结果等同于标准 iproute2 5.5.0
- `RTA_DST_EXT` 是自定义 Netlink 属性，**内核也必须支持**（需要 IPSWEN 内核补丁）
- iproute2 5.5.0 是特定版本，可能与比它新的内核不完全兼容（但与 5.4.0 内核匹配良好）
- 安装后可能会**覆盖系统原有的 `ip` 命令**——建议先备份 `/sbin/ip`

---

## 相关项目

- **linux-kernel** (`../linux-kernel/`) —— IPSWEN 内核（提供 `ipswen_addr` 结构体和 `RTA_DST_EXT` 属性处理）
- **net-tools-1.60** (`../net-tools-1.60/`) —— 传统 `ifconfig`/`route` 的 IPSWEN 版
- **bird** (`../bird/`) —— 通过 `ip route` 安装路由的 BGP 守护进程

---

## 附录 A: IPSWEN 路由操作速查

```bash
# 添加路由
ip route add <prefix> via <gw>
ip route add 10.0.0.0(2)128.129/33 via 192.168.1.1

# 查看
ip route show
ip route show | grep "("
ip route show table all

# 删除
ip route del 10.0.0.0(2)128.129/33

# 接口地址
ip addr add 10.0.0.1(1)200/25 dev eth0
ip addr show dev eth0

# 验证内核接收
cat /proc/net/fib_trie | grep "("
cat /proc/net/route
```

## 附录 B: 编译验证清单

```bash
# 编译后检查
file ip/ip                # ELF 64-bit
ldd ip/ip                 # 检查动态链接
strings ip/ip | grep -i ipswen  # 确认包含 IPSWEN 符号
./ip/ip -V                # 版本
sudo ./ip/ip route add 10.0.0.0(2)128.129/33 via 127.0.0.1 2>&1
# 成功/失败消息说明 IPSWEN 代码路径已进入
```

## 附录 C: 与内核 Netlink 交互数据流

```
用户: ip route add "10.0.0.0(2)128.129/33" via 1.1.1.1
  │
  ├─ ipswen_aton() 解析 → struct ipswen_addr
  ├─ 构造 RTM_NEWROUTE Netlink 消息
  │   ├─ RTA_DST: 10.0.0.0 (标准属性)
  │   ├─ RTA_DST_EXT: {level=2, ext=[128,129]} (IPSWEN 属性)
  │   └─ RTA_GATEWAY: 1.1.1.1 (标准属性)
  ├─ sendmsg() → 内核
  │
内核:
  ├─ rtnetlink_rcv_msg() 接收
  ├─ 解析 RTA_DST_EXT
  ├─ fib_table_insert() → FIB
  └─ 返回 0 (成功)
```
