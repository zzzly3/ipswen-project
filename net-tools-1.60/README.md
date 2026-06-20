# net-tools-1.60 —— 支持 IPswen 的传统网络工具

## 概述

这是 **net-tools 1.60 的修改版本**。net-tools 是 Unix/Linux 最经典的网络管理工具集，提供 `ifconfig`、`route`、`netstat`、`arp`、`hostname` 等命令。虽然 iproute2（`ip` 命令）是现代替代品，但保留此修改版用于兼容性验证和对照实验——确保在 IPswen 内核上，传统工具的行为与新工具一致。

本版本在 INET 地址处理核心库中增加了 IPswen 地址的解析、显示和配置支持。

---

## 目录结构

```
net-tools-1.60/
├── ifconfig.c         # 接口配置（IPswen 地址 add/del/display）
├── route.c            # 路由表管理（IPswen 路由显示）
├── netstat.c          # 网络统计（IPswen 地址显示）
├── arp.c              # ARP 表
├── hostname.c         # 主机名显示
├── lib/
│   └── inet.c         # ★ INET 地址核心库（几乎所有 IPswen 修改集中于此）
├── include/           # 头文件
├── config.in          # 构建配置（HAVE_IPswen 选项定义）
├── configure.sh       # 配置脚本
├── Makefile
├── man/               # 多语言 man 页（de_DE, en_US, fr_FR, pt_BR）
├── po/                # i18n 翻译文件
├── RPM/               # RPM 打包
└── README             # 原始 net-tools README
```

---

## 构建系统

net-tools 使用自己的菜单式配置系统（非 autotools）：

```bash
make config
```

这会显示一个交互式菜单，包含：
```
...
[*] INET (IPv4++) protocol with IPswen addressing extension (HAVE_IPswen)
...
```

选中后，`config.h` 中会生成 `#define HAVE_IPswen 1`。

---

## IPswen 修改详解

### lib/inet.c —— 核心 INET 地址库

所有 IPswen 相关的解析和显示集中在此文件，被 `ifconfig`、`route`、`netstat` 统一调用。

#### 地址解析 (inet_aton)

```c
#if HAVE_IPswen
    // 在标准 inet_aton() 之前先尝试 IPswen 解析
    if (ipswen_aton(&sin->swen_addr, name)) {
        sin->sin_family = AF_INET;
        // 标记为 IPswen 地址
        return 1;
    }
#endif
```

`ipswen_aton()` 识别诸如 `192.168.1.1(2)128.129` 这样的 IPswen 记法，将其解析为 `struct ipswen_addr` 结构体。如果解析失败（字符串不是 IPswen 格式），回退到标准 `inet_aton()`。

#### 地址格式化 (INET_rresolve)

```c
#if HAVE_IPswen
    const struct ipswen_addr *swen_addr;
    char buf[IPSWEN_ADDRSTRLEN];

    // 对于 socket 地址，如果 level ≠ 0 则用 IPswen 格式
    if (ipswen_ntoa(swen_addr, buf, sizeof(buf))) {
        safe_strncpy(name, buf, len);
        return;
    }
#endif
```

在 `ifconfig`、`netstat` 输出时，如果检测到 socket 地址有 IPswen 扩展（level ≠ 0），使用 `ipswen_ntoa()` 生成人类可读的 `A.B.C.D(L)E.F...` 格式。

#### 掩码处理 (INET_mask)

```c
#if HAVE_IPswen
    // IPswen 掩码计算需包含扩展字节
    ipswen_mask(&mask->swen_addr, level, prefix);
#endif
```

标准 IPv4 掩码是 `/N`（N=0..32）。IPswen 掩码是 `/N`（N=0..32+|level|×8），例如 level=2 时前缀可达 /48。掩码函数需要在计算时包含扩展字节。

### ifconfig.c —— 接口配置

```c
#if HAVE_IPswen
/* IPswen address add/del modes */
static int set_ifstate(char *parent,
                       struct ipswen_addr *addr,
                       struct ipswen_addr *nm,
                       struct ipswen_addr *bc);
```

扩展后的 `ifconfig` 支持在接口上添加/删除 IPswen 地址。

### route.c —— 路由表

路由显示和添加时，通过 `lib/inet.c` 的解析函数自动支持 IPswen 格式。

### netstat.c —— 网络统计

在显示 TCP/UDP 连接时，自动检测 IPswen socket 地址并用 `ipswen_ntoa()` 显示。

---

## 编译

```bash
cd net-tools-1.60

# 1. 配置（交互式）
make config
# 在菜单中按空格选中 HAVE_IPswen，然后保存退出

# 2. 验证
grep HAVE_IPswen config.h
# 应输出: #define HAVE_IPswen 1

# 3. 编译
make -j$(nproc)

# 4. 安装
sudo make install

# 5. 验证
ifconfig --version
```

---

## IPswen 命令

```bash
# 为接口分配 IPswen 地址
sudo ifconfig eth0 10.0.0.1(2)128.129 netmask 255.255.255.0

# 查看接口（正确显示 IPswen 地址）
ifconfig eth0

# 查看路由表
route -n

# 查看网络连接
netstat -an | grep "("

# 管理 ARP 表
arp -a
```

---

## 与 iproute2 的对照

| 功能 | net-tools (传统) | iproute2 (现代) |
|------|-----------------|-----------------|
| 接口地址配置 | `ifconfig eth0 ...` | `ip addr add ... dev eth0` |
| 路由查看 | `route -n` | `ip route show` |
| 连接查看 | `netstat -an` | `ss -an` |
| 隧道管理 | `ifconfig tun-XX` | `ip tun add` |
| IPswen 支持 | `HAVE_IPswen` 条件编译 | `HAVE_IPswen` 条件编译 |

两种工具集都打了 IPswen 补丁，可以互相验证。

---

## 注意事项

- net-tools 被视为**传统（legacy）工具**——iproute2 是现代替代品。此版本用于兼容性验证
- 构建系统不是 autotools，而是自己的 `config.in` + `configure.sh`
- 与 iproute2 一样，`HAVE_IPswen` 依赖内核头文件中的结构体定义
- 安装后可能会覆盖系统自带的 net-tools 命令——建议备份

---

## 相关项目

- **iproute2-5.5.0** (`../iproute2-5.5.0/`) —— 现代替代品（`ip` 命令）
- **linux-kernel** (`../linux-kernel/`) —— 提供 IPswen 内核数据结构

---

## 附录 A: 命令速查

```bash
# ifconfig
sudo ifconfig eth0 10.0.0.1(2)128.129 netmask 255.255.255.0
ifconfig eth0

# route
route -n
route add -net 10.0.0.0 netmask 255.255.255.0 gw 192.168.1.1

# netstat
netstat -an | grep "("
netstat -rn

# arp
arp -a
```

## 附录 B: 与 iproute2 完整对照

| 操作 | net-tools | iproute2 |
|------|----------|----------|
| 查看接口 | `ifconfig` | `ip addr show` / `ip link show` |
| 配 IP | `ifconfig eth0 1.2.3.4` | `ip addr add 1.2.3.4/24 dev eth0` |
| 查看路由 | `route -n` | `ip route show` |
| 加路由 | `route add -net ...` | `ip route add ...` |
| 查看连接 | `netstat -an` | `ss -an` |
| ARP 表 | `arp -a` | `ip neigh show` |
| 隧道 | `ifconfig tunX` | `ip tun add` |

## 附录 C: 构建系统内部

`config.in` 使用简单的键值对格式。配置流程：
1. `make config` → 调用 `Configure.sh`
2. `Configure.sh` 解析 `config.in` 生成菜单
3. 用户选择 → 写入 `config.h`
4. `make` 读取 `config.h` 编译

要手动启用 IPswen（跳过菜单）：
```bash
echo '#define HAVE_IPswen 1' >> config.h
make -j$(nproc)
```
