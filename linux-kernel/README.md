# linux-kernel —— IPswen 定制版 Linux 5.4.0 内核 / IPswen Custom Linux 5.4.0 Kernel

## 概述 / Overview

本目录包含一个打过 **IPswen** 补丁的 **Linux 5.4.0 LTS** 完整内核源码树，以及预编译的 Debian 安装包（`.deb`）。IPswen 是一种实验性 IPv4 地址扩展技术——在传统 4 字节 IPv4 地址基础上增加了一个带符号的层次 "level" 维度和对应的扩展字节，实现地址空间的树状扩展。
This directory contains a complete Linux 5.4.0 LTS kernel source tree patched with **IPswen**, along with precompiled Debian packages (`.deb`). IPswen is an experimental IPv4 address extension technology that adds a signed hierarchical "level" dimension and corresponding extension bytes on top of the traditional 4-byte IPv4 address, enabling a tree-like expansion of the address space.

内核修改涉及网络协议栈的六个关键层面，是整个 IPswen 研究生态的**系统基石**——所有用户态工具（iproute2、net-tools、BIRD、unpv13e）都依赖这些内核修改提供的系统调用和数据结构。
The kernel modifications span six key layers of the network protocol stack and serve as the **system foundation** of the entire IPswen research ecosystem — all userspace tools (iproute2, net-tools, BIRD, unpv13e) rely on the system calls and data structures provided by these kernel modifications.

---

## 目录结构 / Directory Structure

```
linux-kernel/
├── linux-src-5.4.0-ipswen-v1-20240124/     # 完整内核源码树 / Complete kernel source tree
│   ├── net/
│   │   ├── ipv4/         # ★ IPv4 协议栈核心（路由、FIB、IP 选项） / ★ IPv4 protocol stack core (routing, FIB, IP options)
│   │   ├── ipv6/         # IPv6 栈 / IPv6 stack
│   │   ├── core/         # Socket 层、Netlink / Socket layer, Netlink
│   │   ├── netfilter/    # Netfilter 钩子 / Netfilter hooks
│   │   └── bridge/       # 桥接 / Bridging
│   ├── include/
│   │   ├── net/ipswen.h  # ★ IPswen 地址结构体 / ★ IPswen address structure
│   │   └── uapi/linux/rtnetlink.h  # ★ RTA_DST_EXT 属性 / ★ RTA_DST_EXT attribute
│   ├── arch/    # x86, ARM 等架构 / x86, ARM and other architectures
│   ├── block/ certs/ crypto/ drivers/ fs/ mm/ kernel/
│   ├── KernelCodeSense.txt  # IPswen 修改涉及的文件索引 / Index of files modified by IPswen
│   ├── Makefile / .config
│   └── README
│
├── linux-src-5.4.0-ipswen-v1-20240124.tar.gz    # 源码包 / Source package
├── linux-headers-5.4.0-ipswen-v1-20240124_001_amd64.deb  # 头文件包 / Header files package
└── linux-image-5.4.0-ipswen-v1-20240124_001_amd64.deb   # 内核镜像包 / Kernel image package
```

---

## 内核版本号 / Kernel Version

```
5.4.0  -ipswen  -v1       -20240124
  │       │       │           │
  │       │       │           └ 构建日期: 2024年1月24日 / Build date: January 24, 2024
  │       │       └ IPswen 补丁版本号 / IPswen patch version
  │       └ IPswen 补丁系列标识 / IPswen patch series identifier
  └ 上游 Linux 5.4 LTS / Upstream Linux 5.4 LTS
```

基于 5.4 LTS（Long Term Support），该版本在 2025 年 12 月前持续获得安全更新，为实验提供了稳定的基础。
Based on 5.4 LTS (Long Term Support), which receives security updates until December 2025, providing a stable foundation for experimentation.

---

## IPswen 内核修改的六个层面 / Six Layers of IPswen Kernel Modifications

### 第 1 层：地址数据结构 / Layer 1: Address Data Structure

在 `include/net/ipswen.h` 和其他头文件中定义：
Defined in `include/net/ipswen.h` and other header files:

```c
struct ipswen_ext {
    s8 level;              // 有符号层次（-IPSWEN_MAX_LEVEL..+IPSWEN_MAX_LEVEL） / Signed level (-IPSWEN_MAX_LEVEL..+IPSWEN_MAX_LEVEL)
    u8 ext[IPSWEN_MAX_LEVEL];  // 扩展字节数组 / Extension byte array
};

struct ipswen_addr {
    u32 base_ip;           // 基础 IPv4 地址（4 字节） / Base IPv4 address (4 bytes)
    struct ipswen_ext ext; // 扩展部分 / Extension part
};
```

IPswen 地址在内存中占用 4（base）+ 1（level）+ N（ext 字节）字节。
An IPswen address occupies 4 (base) + 1 (level) + N (ext bytes) bytes in memory.

### 第 2 层：Socket 地址扩展 / Layer 2: Socket Address Extension

```c
// 扩展后的 sockaddr_in / Extended sockaddr_in
struct sockaddr_in {
    sa_family_t sin_family;  // AF_INET
    in_port_t sin_port;
    struct in_addr sin_addr;
    // ... 原有的 ... / ... original ...
    struct ipswen_addr swen_addr;  // ★ 新增 IPswen 扩展 / ★ New IPswen extension
};
```

应用可以通过标准的 `bind()`/`connect()`/`sendto()` 使用 IPswen 地址，Socket 层自动识别。
Applications can use IPswen addresses through standard `bind()`/`connect()`/`sendto()` calls, with the Socket layer automatically recognizing them.

### 第 3 层：FIB（转发信息库）扩展 / Layer 3: FIB (Forwarding Information Base) Extension

`net/ipv4/fib_semantics.c`、`net/ipv4/fib_frontend.c`、`net/ipv4/route.c`：

- 路由查找时，前缀匹配算法包含 IPswen 的扩展字节
- 下一跳（nexthop/gateway）可以是 IPswen 地址
- `fib_lookup()` 返回的结果中包含 IPswen 相关信息

The prefix matching algorithm includes IPswen extension bytes during route lookup.
The next hop (nexthop/gateway) can be an IPswen address.
The result returned by `fib_lookup()` contains IPswen-related information.

### 第 4 层：Netlink 协议扩展 / Layer 4: Netlink Protocol Extension

`include/uapi/linux/rtnetlink.h`：
```c
#define RTA_DST_EXT  (RTA_MAX + 1)   // 新路由属性 / New route attribute
```

`net/core/rtnetlink.c` 中处理此属性，允许 `ip route` 等命令通过 Netlink 向内核传递 IPswen 路由。
This attribute is handled in `net/core/rtnetlink.c`, allowing commands like `ip route` to pass IPswen routes to the kernel via Netlink.

### 第 5 层：Socket 选项 / Layer 5: Socket Options

`net/core/sock.c`：
- `IP_OPTIONS` setsockopt 在 IPswen 套接字上正常工作
- `SO_BINDTODEVICE` 允许绑定到特定网络设备

`IP_OPTIONS` setsockopt works correctly on IPswen sockets.
`SO_BINDTODEVICE` allows binding to a specific network device.

### 第 6 层：地址验证 / Layer 6: Address Validation

`net/ipv4/devinet.c` 等文件中验证：
- Level 值在合法范围内
- 扩展字节数量 = |level|
- 地址不与标准 IPv4 冲突

Validated in `net/ipv4/devinet.c` and other files:
- The level value is within the valid range
- The number of extension bytes equals |level|
- The address does not conflict with standard IPv4

---

## 内核-用户态交互架构 / Kernel-Userspace Interaction Architecture

```
┌─────────────────────────────────────────┐
│              用户态 (Userspace)          │
│  ┌──────────┐ ┌───────────┐ ┌────────┐  │
│  │ iproute2 │ │ net-tools │ │  BIRD  │  │
│  │ (ip命令) │ │(ifconfig) │ │(bgp)   │  │
│  └────┬─────┘ └─────┬─────┘ └───┬────┘  │
│       │             │           │        │
│       ▼             ▼           ▼        │
│  ┌──────────────────────────────────┐    │
│  │   Netlink Socket (NETLINK_ROUTE) │    │
│  └──────────────┬───────────────────┘    │
├─────────────────┼───────────────────────┤
│                 ▼            内核态      │
│  ┌──────────────────────────────────┐    │
│  │  IPswen 扩展 FIB (路由子系统)     │    │
│  │  - 前缀匹配含 level+扩展字节      │    │
│  │  - RTA_DST_EXT 属性处理          │    │
│  │  - IPswen nexthop 处理           │    │
│  └──────────────┬───────────────────┘    │
│                 ▼                        │
│  ┌──────────────────────────────────┐    │
│  │  网络设备驱动层                    │    │
│  └──────────────────────────────────┘    │
└─────────────────────────────────────────┘
```

*Top: Userspace tools communicate with the kernel via Netlink Socket. Bottom: The IPswen-extended FIB handles route lookup with level+extension bytes, RTA_DST_EXT attribute processing, and IPswen nexthop processing, down to the network device driver layer.*

---

## KernelCodeSense.txt 说明 / KernelCodeSense.txt Description

此文件是 IPswen 相关内核源文件的索引。它列出了 `block/`、`certs/`、`crypto/` 等子系统的文件——暗示 IPswen 修改的触及面不限于纯网络栈。可能与内核模块签名（`certs/`）、加密 API 兼容性（`crypto/`）或存储层（`block/`）有关。
This file is an index of IPswen-related kernel source files. It lists files from subsystems such as `block/`, `certs/`, `crypto/` — suggesting that the scope of IPswen modifications extends beyond the pure network stack. It may be related to kernel module signing (`certs/`), cryptographic API compatibility (`crypto/`), or the storage layer (`block/`).

---

## 从源码编译（完整指南） / Building from Source (Complete Guide)

### 第一步：安装编译依赖 / Step 1: Install Build Dependencies

```bash
sudo apt update
sudo apt install -y \
    build-essential gcc g++ binutils make \
    libncurses-dev libssl-dev \
    flex bison bc rsync kmod cpio \
    dwarves elfutils libelf-dev
```

*Install essential build tools, libraries, and utilities required for kernel compilation.*

### 第二步：配置内核 / Step 2: Configure the Kernel

```bash
cd linux-src-5.4.0-ipswen-v1-20240124

# 方法 A: 使用已有 .config / Method A: Use existing .config
make olddefconfig

# 方法 B: 交互式配置 / Method B: Interactive configuration
make menuconfig
# 推荐检查以下选项： / Recommended options to verify:
#   CONFIG_NET_IPGRE=y/m (GRE 隧道 / GRE tunnel)
#   CONFIG_IP_ADVANCED_ROUTER=y
#   CONFIG_IP_MULTIPLE_TABLES=y
```

### 第三步：编译 / Step 3: Compile

```bash
# 编译内核（使用所有 CPU 核心） / Build kernel (using all CPU cores)
make -j$(nproc) 2>&1 | tee build_kernel.log

# 编译模块 / Build modules
make modules -j$(nproc) 2>&1 | tee build_modules.log

# 检查错误 / Check for errors
grep -i "error:" build_*.log
```

**时间预估：** / **Estimated time:**
- 8 核 CPU: ~30-40 分钟 / 8-core CPU: ~30-40 minutes
- 4 核 CPU: ~60-90 分钟 / 4-core CPU: ~60-90 minutes
- 2 核 CPU: ~2-3 小时 / 2-core CPU: ~2-3 hours

**磁盘空间：** 完整构建约需 15-18 GB。
**Disk space:** A full build requires approximately 15-18 GB.

### 第四步：安装 / Step 4: Install

```bash
sudo make modules_install
sudo make install
sudo update-grub
sudo reboot
```

### 第五步：验证 / Step 5: Verify

```bash
uname -r
# 应输出: 5.4.0-ipswen-v1-20240124 / Should output: 5.4.0-ipswen-v1-20240124

# 检查 IPswen 支持 / Check IPswen support
cat /proc/sys/net/ipv4/ipswen_enabled  # 如果存在 / If it exists
# 或 / Or
dmesg | grep -i ipswen
```

### 只编译网络栈（快速验证修改） / Compile Only the Network Stack (Quick Verification)

```bash
# 只重新编译网络子系统（不编译整个内核） / Only recompile the network subsystem (not the full kernel)
make net/ipv4/
make net/core/
# 然后手动替换模块或重启 / Then manually replace modules or reboot
```

---

## 预编译包安装 / Installing Precompiled Packages

```bash
# 安装内核镜像（需重启） / Install kernel image (requires reboot)
sudo dpkg -i linux-image-5.4.0-ipswen-v1-20240124_001_amd64.deb
sudo update-grub
sudo reboot

# 安装头文件（编译用户态工具的依赖） / Install header files (dependency for compiling userspace tools)
sudo dpkg -i linux-headers-5.4.0-ipswen-v1-20240124_001_amd64.deb

# 验证头文件 / Verify header files
ls /usr/src/linux-headers-5.4.0-ipswen-v1-20240124/include/net/ipswen.h
```

---

## 实验定制 / Experimental Customization

### 怎样修改最大 level / How to Change the Maximum Level

```bash
# 编辑头文件 / Edit the header file
vim include/net/ipswen.h
# 找到 IPSWEN_MAX_LEVEL / Find IPSWEN_MAX_LEVEL
# 默认: #define IPSWEN_MAX_LEVEL 7 / Default: #define IPSWEN_MAX_LEVEL 7
# 改为: #define IPSWEN_MAX_LEVEL 15 / Change to: #define IPSWEN_MAX_LEVEL 15

# 重新编译 / Recompile
make net/ipv4/ && make modules && sudo make modules_install && sudo reboot
```

### 怎样添加新的 Netlink 属性 / How to Add a New Netlink Attribute

```bash
# 1. 在 include/uapi/linux/rtnetlink.h 中添加 / 1. Add in include/uapi/linux/rtnetlink.h
#define RTA_MY_ATTR (RTA_MAX + 2)

# 2. 在 net/core/rtnetlink.c 的 rtm_ipv4_policy 中添加处理 / 2. Add handling in rtm_ipv4_policy in net/core/rtnetlink.c
# 3. 重新编译 / 3. Recompile
```

### 怎样调试内核 IPswen 路由 / How to Debug IPswen Kernel Routing

```bash
# 启用动态调试 / Enable dynamic debugging
echo 'file net/ipv4/route.c +p' | sudo tee /sys/kernel/debug/dynamic_debug/control
echo 'file net/ipv4/fib_semantics.c +p' | sudo tee -a /sys/kernel/debug/dynamic_debug/control

# 监控 / Monitor
sudo dmesg -wT | grep -i ipswen

# 用 ftrace 追踪 / Trace with ftrace
cd /sys/kernel/debug/tracing
echo function > current_tracer
echo '*ipswen*' > set_ftrace_filter
cat trace_pipe
```

### 怎样验证 IPswen 路由 / How to Verify IPswen Routing

```bash
# 添加 IPswen 路由（需要 iproute2-5.5.0） / Add IPswen route (requires iproute2-5.5.0)
sudo ip route add 10.0.0.0(2)128.129/33 via 192.168.1.1

# 查看内核 FIB / View kernel FIB
cat /proc/net/fib_trie | grep "("

# 查看路由缓存 / View route cache
ip route show cache | grep "("
```

---

## 内核模块签名（可选） / Kernel Module Signing (Optional)

如果启用了 Secure Boot 或内核模块签名：
If Secure Boot or kernel module signing is enabled:

```bash
# 生成密钥 / Generate key
openssl req -new -nodes -utf8 -sha512 -days 36500 \
    -batch -x509 -outform PEM \
    -out kernel_key.pem \
    -keyout kernel_key.pem

# 签名模块 / Sign module
scripts/sign-file sha512 kernel_key.pem kernel_key.x509 <module>.ko
```

---

## 排查常见问题 / Troubleshooting Common Issues

| 问题 / Issue | 诊断 / Diagnosis | 解决 / Solution |
|------|------|------|
| 新内核无法启动 / New kernel won't boot | GRUB 选旧内核进入 / Boot old kernel from GRUB | 检查 `dmesg` 日志 / Check `dmesg` logs |
| `make menuconfig` 乱码 / Gibberish in `make menuconfig` | 终端不支持 / Terminal not supported | `export TERM=xterm` |
| 编译 OOM / Build OOM | 并行数多 / Too many parallel jobs | `make -j2` 降低并行 / Reduce parallelism with `make -j2` |
| 模块签名失败 / Module signing fails | Secure Boot | 进 BIOS 禁用 Secure Boot / Disable Secure Boot in BIOS |
| 头文件无 `ipswen.h` / `ipswen.h` missing in headers | 未装 headers 包 / Headers package not installed | `sudo dpkg -i linux-headers-*.deb` |
| `ip route` 不认识 `(` 记法 / `ip route` doesn't recognize `(` notation | iproute2 未用 IPswen 版 / iproute2 not using IPswen version | 安装 `../iproute2-5.5.0/` / Install `../iproute2-5.5.0/` |

---

## 注意事项 / Important Notes

- 5.4 LTS 内核，IPswen 修改是**附加式**的——不影响标准 IPv4/IPv6 功能
  The IPswen modifications to the 5.4 LTS kernel are **additive** — they do not affect standard IPv4/IPv6 functionality
- `.deb` 包仅适用于 **amd64** 架构 (x86_64)
  `.deb` packages are only for the **amd64** architecture (x86_64)
- 头文件包是编译 iproute2、net-tools 等工具的前置条件
  The header package is a prerequisite for compiling tools like iproute2, net-tools, etc.
- 内核源码树约 800-900 MB，完整编译后约 15-18 GB
  The kernel source tree is approximately 800-900 MB, and a full build takes about 15-18 GB
- 如果遇到 Kernel Panic，在 GRUB 菜单中选择旧内核即可回退
  If you encounter a Kernel Panic, select the old kernel in the GRUB menu to roll back

---

## 相关项目 / Related Projects

- **iproute2-5.5.0** —— 与本内核交互的 `ip` 命令 / The `ip` command that interacts with this kernel
- **net-tools-1.60** —— `ifconfig`/`route`
- **bird** —— 向本内核 FIB 注入 IPswen 路由的 BGP 守护进程 / BGP daemon that injects IPswen routes into this kernel's FIB
- **unpv13e** —— 使用本内核 IPswen Socket API 的示例代码 / Example code using this kernel's IPswen Socket API
- **happyfootball** —— Socket 级 IP 选项实验 / Socket-level IP option experiments

---

## 附录 A: 内核配置关键选项 / Appendix A: Key Kernel Configuration Options

编译前确认以下配置（`make menuconfig`）：
Verify the following configuration before compilation (`make menuconfig`):
```
[*] Networking support → Networking options →
    [*] TCP/IP networking
    [*]   IP: advanced router
    [*]   IP: policy routing
    [*]   IP: equal cost multipath
    [*]   IP: verbose route monitoring
    <M>   IP: tunneling
    <M>   IP: GRE demultiplexer
    <M>   IP: GRE tunnel over IP
```

## 附录 B: 内核日志关键字 / Appendix B: Kernel Log Keywords

调试时搜索 `dmesg` 的关键字：
Keywords to search in `dmesg` during debugging:
- `ipswen` — IPswen 相关日志 / IPswen-related logs
- `RTNETLINK` — Netlink 错误 / Netlink errors
- `FIB` — 转发信息库 / Forwarding Information Base
- `ICMP` — ICMP 消息 / ICMP messages
- `GRE` — GRE 隧道 / GRE tunnels

```bash
# 实时监控 / Real-time monitoring
sudo dmesg -wT | grep -E "ipswen|RTNETLINK|FIB"

# 查看启动日志 / View boot logs
sudo journalctl -k | grep -i ipswen
```

## 附录 C: 快速内核回退 / Appendix C: Quick Kernel Rollback

如果新内核无法启动：
If the new kernel fails to boot:
1. 重启，在 GRUB 菜单选择 "Advanced options" / Reboot and select "Advanced options" in the GRUB menu
2. 选择旧内核（之前能启动的版本） / Select the old kernel (a previously working version)
3. 进入系统后移除问题内核： / After booting, remove the problematic kernel:
```bash
dpkg -l | grep linux-image
sudo apt remove linux-image-5.4.0-ipswen-v1-20240124
sudo update-grub
```

---

## 附录 D: 大文件分块说明（重要！） / Appendix D: Large File Chunking Notes (Important!)

### 为什么源码包被分块？ / Why Was the Source Package Chunked?

`linux-src-5.4.0-ipswen-v1-20240124.tar.gz` 大小为 **171 MB**，超过 GitHub 的 100 MB 限制。已拆分为 2 个 95MB 的分块文件。
The file is **171 MB**, exceeding GitHub's 100 MB limit. It has been split into 2 chunks of 95 MB each.

### 受影响的文件 / Affected Files

| 原始文件 / Original File | 大小 / Size | 分块 / Chunks |
|---------|------|------|
| `linux-src-5.4.0-ipswen-v1-20240124.tar.gz` | 171 MB | `.part00` + `.part01` |

**重要**: `linux-src-5.4.0-ipswen-v1-20240124/` 目录中的完整内核源码树不受影响——它是 git 仓库的一部分。只有 `.tar.gz` 被分块。
**Important**: The complete kernel source tree in the `linux-src-5.4.0-ipswen-v1-20240124/` directory is NOT affected — it is part of the git repository. Only the `.tar.gz` file has been chunked.

### 还原方法 / How to Reassemble

**方法 1: 一键还原（推荐） / Method 1: One-click Reassembly (Recommended)**
```bash
cd /path/to/ipswen-project
bash reassemble.sh
```

**方法 2: 手动还原 / Method 2: Manual Reassembly**
```bash
cd linux-kernel
cat linux-src-5.4.0-ipswen-v1-20240124.tar.gz.part00 \
    linux-src-5.4.0-ipswen-v1-20240124.tar.gz.part01 \
    > linux-src-5.4.0-ipswen-v1-20240124.tar.gz

tar tzf linux-src-5.4.0-ipswen-v1-20240124.tar.gz | head -5   # 验证 / Verify
```

### 验证 / Verification

```bash
ls -lh linux-kernel/linux-src-5.4.0-ipswen-v1-20240124.tar.gz   # 应为约 171 MB / Should be approximately 171 MB
tar tzf linux-kernel/linux-src-5.4.0-ipswen-v1-20240124.tar.gz | wc -l
```

### 注意事项 / Notes

- 完整的源码树已在仓库中——**如果只需要编译和修改内核，不需要还原 `.tar.gz`**
  The complete source tree is already in the repository — **if you only need to compile and modify the kernel, you do not need to reassemble `.tar.gz`**
- `.tar.gz` 是备份副本，可用于在其他机器快速部署、归档保存、生成 checksum
  `.tar.gz` is a backup copy that can be used for rapid deployment on other machines, archival storage, and checksum generation
