# netmap —— 高性能数据包 I/O 框架与 VALE 软件交换机

## 概述

`netmap` 是一个用于超高速用户态数据包 I/O 的框架，配合 **VALE**（一个基于 netmap API 的内核级 L2 软件交换机），可在单个进程内处理**每秒数千万个数据包**，匹配 10Gbps 和 40Gbps 端口的线速（即使是最小帧）。

本仓库是 IPSWEN 项目环境中使用的 netmap 本地副本，包含 **Linux 内核模块**、**VALE 交换机**、**修改版 Intel/NVIDIA/Realtek 网卡驱动**（强制启用 netmap 感知）、以及完整的用户态编程 API。

**核心定位：** netmap 是 IPSWEN 实验平台的**底层高性能数据面**——其他组件（如 optmap 的 XDP 路由、fakedns 的地址转换、happyfootball 的多路径连接）运行在 netmap 之上的用户态协议栈中，netmap 负责在最底层以最高速度将原始数据包从网卡送到用户态应用，或从用户态应用发送到线路上。

**为什么在 IPSWEN 项目中使用 netmap（而非标准 Linux socket）：**

| 特性 | 标准 Linux Socket | Netmap |
|------|-------------------|--------|
| 零拷贝 | 需要在用户态/内核态间拷贝 | 内存映射，零拷贝 |
| 批量 I/O | 每次系统调用 1 个数据包 | 每次系统调用最多 1024 个数据包 |
| 每包系统调用数 | ~1 | ~0.001（批处理） |
| 缓冲区管理 | sk_buff 动态分配/释放 | 预分配环形缓冲区 |
| 吞吐量（64B 小包）| ~1-2 Mpps | ~14.88 Mpps（10G 线速）|
| CPU 利用率 | 高（数据拷贝 + 系统调用开销）| 低（零拷贝 + 批处理）|
| API 兼容性 | POSIX socket | 自定义 netmap API / libpcap 模拟 |

---

## 目录结构

```
netmap/
├── configure             # 顶层配置脚本（分发到 LINUX/configure）
├── GNUmakefile           # 顶层 Makefile
├── Kbuild                # Linux 内核构建文件
├── Makefile              # 用户态工具 Makefile
├── netmap.ko             # ★ 编译产物：内核模块
│
├── sys/                  # ★ 与 FreeBSD 共享的核心 netmap 源代码
│   └── net/              # netmap 框架核心：环形缓冲区、端口管理、VALE 交换机
│
├── LINUX/                # Linux 平台适配层
│   ├── configure         # ★ Linux 配置脚本（检测内核特性，生成 netmap_linux_config.h）
│   ├── netmap_linux.c    # Linux 内核模块入口
│   ├── bsd_glue.h        # FreeBSD→Linux 兼容层（提供 FreeBSD API 的 Linux 实现）
│   ├── README.md         # Linux 构建详细说明
│   └── patches/          # 网卡驱动补丁
│
├── apps/                 # 用户态示例与测试工具
│   ├── pkt-gen.c         # ★ 高速包生成器和接收器（吞吐量压测工具，最常用）
│   ├── bridge.c          # ★ 简单双向桥接（两个 netmap 端口间的 L2 转发）
│   ├── nmreplay.c        # pcap 文件回放工具
│   └── ...               # 其他工具
│
├── share/                # 共享资源（man pages, 脚本）
│   └── man/man4/         # netmap.4 手册页
│
├── netmap_linux_config.h # 配置脚本生成的编译配置头文件（自动生成，不应手动编辑）
├── drivers.mak           # 网卡驱动构建规则
├── drv-subdir.mak        # 驱动子目录构建规则
├── read-vars.mak         # Makefile 变量读取辅助脚本
├── default-config.mak    # 恢复默认配置的 Makefile 片段
│
│   # ★★ 网卡驱动源码（从内核中提取 + netmap 补丁）：
├── forcedeth.c           # NVIDIA nForce 以太网驱动（已打 netmap 补丁）
├── forcedeth.c.orig      # NVIDIA nForce 驱动原始版本
├── veth.c                # 虚拟以太网对（Virtual Ethernet）驱动（已打 netmap 补丁）
├── veth.c.orig           # veth 原始版本
│
├── e1000/                # Intel PRO/1000 驱动（1Gbps）—— get-e1000 脚本获取的版本
├── e1000e/               # Intel PRO/1000e 驱动（1Gbps PCIe）—— get-e1000e 脚本获取
├── igb/                  # Intel 82575/82576 驱动（1Gbps）—— get-igb 脚本获取
├── igc/                  # Intel I225/I226 驱动（2.5Gbps）—— get-igc 脚本获取
├── ixgbe/                # Intel 82599/82598 驱动（10Gbps）—— get-ixgbe 脚本获取
├── ixgbevf/              # Intel 82599 虚拟功能驱动 —— get-ixgbevf 脚本获取
├── i40e/                 # Intel X710/XL710 驱动（40Gbps）—— get-i40e 脚本获取
├── virtio_net.c/         # Virtio 网络驱动（虚拟机）—— get-virtio_net.c 脚本获取
├── vmxnet3/              # VMware vmxnet3 虚拟网卡驱动 —— get-vmxnet3 脚本获取
│
├── patches/              # 驱动补丁文件（补丁应用到驱动源码的 diff）
│
├── get-e1000             # ★ 获取 Intel e1000 驱动源码的脚本
├── get-e1000e            # ★ 获取 Intel e1000e 驱动源码的脚本
├── get-igb               # ★ 获取 Intel igb 驱动源码的脚本
├── get-igc               # ★ 获取 Intel igc 驱动源码的脚本
├── get-ixgbe             # ★ 获取 Intel ixgbe 驱动源码的脚本
├── get-ixgbevf           # ★ 获取 Intel ixgbevf 驱动源码的脚本
├── get-i40e              # ★ 获取 Intel i40e 驱动源码的脚本
├── get-forcedeth.c       # ★ 获取 NVIDIA forcedeth 驱动源码的脚本
├── get-veth.c            # ★ 获取 veth 驱动源码的脚本
├── get-virtio_net.c      # ★ 获取 virtio-net 驱动源码的脚本
├── get-vmxnet3           # ★ 获取 VMware vmxnet3 驱动源码的脚本
│
├── intel-fix.sh          # Intel 驱动修复脚本（修正与当前内核的兼容性问题）
├── tests/                # 单元测试和回归测试
├── build-apps/           # 独立编译应用程序的 Makefile
├── build-libnetmap/      # libnetmap 库构建
├── libnetmap/            # libnetmap C 库（提供 C 语言 API 封装）
├── utils/                # 辅助工具脚本
├── netmap.mak            # netmap 共享的 Makefile 变量
├── WINDOWS/              # Windows 支持（IPSWEN 项目中不使用）
│
├── config.log            # configure 执行日志
├── config.status         # configure 状态缓存
├── .netmap.ko.cmd        # 内核模块编译中间文件
├── Module.symvers        # 内核符号版本信息
├── modules.order         # 模块编译顺序
│
├── .travis.yml           # Travis CI 配置
├── .clang-format         # 代码风格配置
├── .gitignore            # Git 忽略规则
├── LICENSE               # BSD 许可证
├── README.md             # 本文件
├── README.ptnetmap.md    # ptnetmap 透传模式说明
├── PORTING               # 移植指南
└── SECURITY.md           # 安全策略
```

---

## 核心架构

### 数据通路架构

```
┌──────────────────────────────────────────────┐
│            用户态应用程序                      │
│  (pkt-gen, bridge, 或自定义 netmap 程序)      │
│                                               │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐    │
│  │ TX Ring  │  │ RX Ring  │  │ ...      │    │
│  │ (netmap  │  │ (netmap  │  │          │    │
│  │  slot[]) │  │  slot[]) │  │          │    │
│  └────┬─────┘  └────┬─────┘  └──────────┘    │
│       │              │                        │
│       │    mmap()    │                        │
└───────┼──────────────┼────────────────────────┘
        │              │
   ┌────▼──────────────▼────┐
   │    Netmap 内核模块      │
   │    (netmap.ko)          │
   │                        │
   │  ┌──────────────────┐  │
   │  │ Netmap Rings     │  │
   │  │ (预分配 DMA 缓冲区)│  │
   │  └──────┬───────────┘  │
   │         │               │
   │  ┌──────▼───────────┐  │
   │  │ netmap-aware     │  │
   │  │ NIC driver       │  │
   │  └──────┬───────────┘  │
   └─────────┼──────────────┘
             │
      ┌──────▼──────┐
      │    NIC      │
      │  (硬件)      │
      └─────────────┘
```

### 四个核心优化技术（按重要性排序）

1. **I/O 批处理** —— 一次系统调用传输多个数据包（通常 512+），将系统调用开销分摊到每个包上
2. **高效设备驱动** —— 驱动直接操作 netmap 的预分配环形缓冲区，绕过 sk_buff 的分配/释放
3. **预分配缓冲区** —— TX/RX 描述符环在打开时一次性分配（`NIOCREGIF`），数据路径中零动态内存分配
4. **内存映射缓冲区** —— 通过 `mmap()` 将内核环形缓冲区直接映射到用户态，用户程序直接读写数据包，零拷贝

**关键纠正：** 尽管叫"netmap"，内存映射（mmap）并不是最关键的性能优化。所有四个技术必须结合使用才能真正达到 10G/40G 线速。

### 数据包收发流程

#### 接收流程
```
1. 硬件 DMA 将数据包写入预分配的 RX 缓冲区
2. 驱动（netmap-aware）→ 更新 netmap RX ring 的 slot 描述符
3. 用户态通过 ioctl(NIOCTXSYNC) 或 select/poll 发现新数据包
4. 用户态直接通过 mmap 映射的内存读取数据包内容（零拷贝）
5. 处理完成后更新 slot 状态，确认已消费
```

#### 发送流程
```
1. 用户态通过 mmap 映射的内存填写数据包内容
2. 更新 netmap TX ring 的 slot 描述符（设置 buflen 等）
3. 用户态调用 ioctl(NIOCTXSYNC) 通知内核有数据要发送
4. 驱动从 TX ring 读取 slot，DMA 到硬件发送队列
5. 硬件发送完成后，驱动更新 slot 状态（如 tx completion）
```

---

## 子系统完整解析

Netmap 的模块化设计包含以下子系统，通过 configure 的 `--enable-*`/`--disable-*` 选项控制编译：

### 1. VALE（默认启用）—— 内核级 L2 软件交换机

VALE 是一个利用 netmap API 构建的**内核级 L2 学习型以太网交换机**。多个 netmap 客户端（或物理端口）可以连接到同一个 VALE 交换机实例，实现超高速的包交换。

```
       ┌───────────────┐
       │  VALE switch  │
       │   "vale0"     │
       ├───┬───┬───┬───┤
       │   │   │   │   │
    ┌──┘   │   │   │   └──┐
    ▼      ▼   ▼   ▼      ▼
 [app1] [app2] [eth0] [VM guest]
```

**VALE 学习桥的工作原理：**
1. 当交换机收到一个包，检查源 MAC 是否已在转发表中
2. 如果不在 → 记录（源 MAC → 接收端口）
3. 查目标 MAC 的转发表
4. 如果找到 → 单播到目标端口
5. 如果未找到（或目标 MAC 是广播）→ 泛洪到所有其他端口

**编译控制：**
```bash
./configure --enable-vale    # 启用 VALE（默认）
./configure --disable-vale   # 禁用 VALE
```

### 2. Pipe（默认启用）—— 背靠背虚拟端口对

Netmap pipe 是一对**背靠背连接**的虚拟 netmap 端口：一端发送的数据包从另一端直接接收，不经过任何网络设备。

```
┌──────────┐         ┌──────────┐
│  App A   │────────▶│  App B   │
│ (pipe A) │◀────────│ (pipe B) │
└──────────┘         └──────────┘
       零拷贝内核级管道（类似双向 pipe）
```

**用途：**
- 进程间通信（IPC）
- 在独立进程中测试网络协议栈（一个进程产生流量，另一个接收并验证）
- QEMU/KVM 虚拟机互联（通过 netmap pipe 后端）

### 3. Monitor（默认启用）—— 零拷贝端口监控

Netmap monitor 可以在**不中断流量**的情况下监控其他 netmap 端口：

- **复制模式：** 将数据包的拷贝发送给监控客户端
- **零拷贝模式：** 将数据包指针共享给监控客户端（更快，但原始客户端和监控客户端必须协调缓冲区所有权）

**应用场景：**
- 实时数据包分析（类似 tcpdump 但零拷贝）
- 性能调试（监控流量模式而不影响被测系统）
- 安全审计（在网络边界做被动流量检查）

### 4. Generic（默认启用）—— 通用适配器

Generic 适配器允许**任何已有 Linux 驱动**的网络接口以 netmap 模式打开，但性能低于原生 netmap-aware 驱动。

```bash
# 任何网卡都可以通过 generic 适配器以 netmap 模式打开
# 不需要驱动补丁——但性能会显著下降
./pkt-gen -i eth0    # eth0 无需特殊驱动即可用 netmap 模式
```

**工作原理：** Generic 适配器在 netmap 的缓冲区与内核的 sk_buff 之间做双向拷贝。每次收发仍有一次内存拷贝操作，但批处理和预分配缓冲区仍然有效。

### 5. ptnetmap（默认启用）—— 虚拟机透传

ptnetmap 允许将 netmap 端口**透传（pass-through）**到 QEMU/KVM 虚拟机内部，guest 内的应用程序可以直接使用 netmap API。

```
Host netmap port ──▶ ptnetmap ──▶ Guest netmap port
                                   └── Guest app 直接访问
```

### 6. Sink —— 丢弃设备

一个以 netmap 原生速度运行的**黑洞设备**—— 所有发往 sink 的数据包都被静默丢弃。

**用途：**
- 测试发送端性能（消除接收端的瓶颈）
- 模拟有丢包特性的链路（可配置丢包率）

---

## 网卡驱动详解

### 驱动获取策略（两种方法）

| 方法 | 描述 | 默认使用的驱动 |
|------|------|---------------|
| **方法 1：内核内建驱动 + 补丁** | 对内核源码树中的驱动打补丁 | `e1000`, `forcedeth`, `veth`, `virtio_net`, `r8169` |
| **方法 2：厂商独立驱动 + 补丁** | 我们选择的特定版本厂商驱动 + 补丁 | `e1000e`, `igb`, `ixgbe`, `ixgbevf`, `i40e` |

**configure 相关选项：**
```bash
./configure --show-drivers        # 列出支持的驱动
./configure --show-ext-drivers    # 列出使用方法 2 的驱动
./configure --no-ext-drivers      # 所有驱动改用方法 1
```

### 各驱动详解

#### Intel e1000 (1Gbps) —— 方法 1
```bash
./get-e1000
# 从内核源码树拷贝 drivers/net/ethernet/intel/e1000/ → netmap/e1000/
# 如果失败，从 Intel 下载 e1000 独立驱动
```

#### Intel e1000e (1Gbps PCIe) —— 方法 2
```bash
./get-e1000e
# 下载 Intel e1000e-3.8.4 独立驱动
# 支持的网卡：82571/82572/82573/82574/82577/82578/82579/I217/I218/I219
```

#### Intel igb (1Gbps) —— 方法 2
```bash
./get-igb
# 下载 Intel igb-5.3.5.4 独立驱动
# 支持的网卡：82575/82576/I210/I211/I350
```

#### Intel igc (2.5Gbps) —— 方法 2
```bash
./get-igc
# 下载 Intel igc 驱动
# 支持的网卡：I225/I226
```

#### Intel ixgbe (10Gbps) —— 方法 2
```bash
./get-ixgbe
# 下载 Intel ixgbe-5.3.7 独立驱动
# 支持的网卡：82598/82599/X520/X540/X550
# ★ IPSWEN 实验中最常用的 10G 网卡驱动
```

#### Intel ixgbevf (10Gbps 虚拟功能) —— 方法 2
```bash
./get-ixgbevf
# 下载 Intel ixgbevf 驱动
# 支持的网卡：82599/X540/X550 的 SR-IOV 虚拟功能（VF）
```

#### Intel i40e (40Gbps) —— 方法 2
```bash
./get-i40e
# 下载 Intel i40e-2.4.10 独立驱动
# 支持的网卡：X710/XL710/XXV710
# ★ IPSWEN 高性能实验节点的选择
```

#### NVIDIA forcedeth (1Gbps) —— 方法 1
```bash
./get-forcedeth.c
# 从内核拷贝 drivers/net/ethernet/nvidia/forcedeth.c
# 默认方法已启用，文件已在 netmap/ 根目录
```

#### Virtio-net (虚拟化) —— 方法 1
```bash
./get-virtio_net.c
# 从内核拷贝 drivers/net/virtio_net.c
# 用于 QEMU/KVM 虚拟机中的网络加速
```

#### Veth (虚拟以太网对) —— 方法 1
```bash
./get-veth.c
# 从内核拷贝 drivers/veth.c
# 用于容器/网络命名空间中的虚拟链路
```

### 驱动补丁的作用

Netmap 补丁对驱动的主要修改：

1. **注入 netmap 数据通路** —— 在驱动的 NAPI poll 和 start_xmit 函数中，添加 netmap 模式的分支判断
2. **替换缓冲区管理** —— 将 sk_buff 分配替换为 netmap 预分配缓冲区的索引操作
3. **添加 NDO 注册** —— 在驱动注册的网络设备操作（net_device_ops）中添加 netmap 特定的 ndo 处理函数
4. **中断优化** —— 在 netmap 模式下调整中断节流以减少开销

**补丁示例（原理解释，非实际代码）：**
```c
// 原始 e1000 驱动的 NAPI poll 函数
static int e1000_clean(struct napi_struct *napi, int budget) {
    // 从 RX 描述符环读取数据包
    // 分配 sk_buff 并填充
    // 调用 netif_receive_skb() 交给协议栈
}

// 补丁后：
static int e1000_clean(struct napi_struct *napi, int budget) {
+   if (netmap_rx_irq(netdev, ...)) {
+       // ★ netmap 模式：数据包直接进入预分配环形缓冲区
+       //   跳过 sk_buff 分配和协议栈处理
+       return 1;
+   }
    // 原始代码（非 netmap 模式走原路径）
}
```

---

## 编译完整指南

### 环境要求

```bash
# 系统依赖
sudo apt update && sudo apt install -y \
    build-essential gcc g++ make \
    linux-headers-$(uname -r) \
    linux-source \
    flex bison \
    libelf-dev libssl-dev \
    ethtool iproute2

# 验证内核头文件版本
uname -r
ls /lib/modules/$(uname -r)/build/include/linux/version.h
# 确保最后一条命令成功
```

### 第一步：运行 configure

```bash
cd netmap

# 基本配置（所有默认子系统）
./configure

# 查看配置选项
./configure --help

# 自定义配置示例：
# 只启用核心 netmap 和 VALE，禁用其他子系统
./configure --enable-vale --disable-pipe --disable-monitor

# 查看支持的驱动
./configure --show-drivers
./configure --show-ext-drivers

# 所有驱动使用方法 1（内建驱动 + 补丁）
./configure --no-ext-drivers

# configure 输出文件：
#   netmap_linux_config.h  —— 编译器宏定义
#   netmap.mak              —— Makefile 变量
#   drivers.mak             —— 驱动构建规则
```

**configure 的工作原理：**

configure 脚本通过编译微型测试程序来检测内核特性（而非信任 `/proc/version` 中的版本号）：
- 检查 `struct net_device_ops` 是否包含 `ndo_change_carrier` 成员
- 检查 `netif_carrier_off` / `netif_carrier_on` 是否可用
- 检查内核是否支持 XPS（Transmit Packet Steering）
- 检查 `time64_t` 类型是否可用
- 检查 RPS（Receive Packet Steering）相关函数

### 第二步：编译内核模块

```bash
# 编译 netmap.ko
make

# 编译过程：
# 1. 编译 sys/ 中的核心 netmap 代码（共享于 FreeBSD 和 Linux）
#    → netmap_common.o
# 2. 编译 LINUX/ 中的 Linux 适配代码
#    → netmap_linux.o
# 3. 编译各子系统
#    → netmap_vale.o, netmap_pipe.o, netmap_monitor.o, ...
# 4. 链接为单一内核模块 netmap.ko
```

**常见编译错误排查：**

| 错误 | 原因 | 解决 |
|------|------|------|
| `Makefile: No rule to make target '...e1000/...'` | 驱动源码未获取 | 运行 `./get-e1000`（或对应驱动） |
| `error: implicit declaration of function 'ndo_change_carrier'` | 内核 API 变化 | 编辑 `netmap_linux_config.h`，注释对应宏 |
| `error: 'struct net_device' has no member named '...'` | 内核版本不匹配 | 检查 `configure` 输出，可能需要更新内核 |
| `WARNING: "some_symbol" [...] undefined!` | 模块依赖问题 | `depmod -a`，或使用 `modprobe` 而非 `insmod` |
| `fatal error: linux/compiler-gcc7.h: No such file` | GCC 版本太新 | `sudo apt install gcc-8`，`export CC=gcc-8` |

### 第三步：编译用户态工具

```bash
# 编译 apps/ 中的程序
cd apps
make

# 或从 netmap/ 根目录
make apps

# 关键产物：
#   apps/pkt-gen/pkt-gen      —— 高速包生成器
#   apps/bridge/bridge        —— 双向桥接
#   apps/nmreplay/nmreplay    —— pcap 回放
```

### 第四步：安装

```bash
# 安装内核模块
sudo make install
# 实际操作：
#   1. cp netmap.ko /lib/modules/$(uname -r)/
#   2. depmod -a
#   3. 加载模块

# 安装用户态工具（可选）
sudo make install-apps
```

### 第五步：加载内核模块

```bash
# 方法 A：手动加载
sudo insmod netmap.ko

# 方法 B：使用 modprobe（自动解决依赖）
sudo modprobe netmap

# 验证加载成功
lsmod | grep netmap
dmesg | tail -20 | grep netmap
# 应看到类似：
#   netmap: loaded module
#   netmap: netmap version ...

# 检查 netmap 设备节点是否创建
ls -la /dev/netmap
# 应显示 crw------- 1 root root ...

# 如果设备节点未自动创建：
sudo mknod /dev/netmap c 10 54   # major 10, minor 54
```

### 第六步：替换网卡驱动

```bash
# 如果网卡当前使用的是标准内核驱动，需要卸载并替换为 netmap 补丁版

# 示例：替换 ixgbe 驱动
sudo rmmod ixgbe                      # 先卸载标准驱动
cd netmap/ixgbe
sudo insmod ixgbe.ko                  # 加载 netmap 补丁版驱动

# 确认替换成功
ethtool -i eth1 | grep driver         # 输出: ixgbe
dmesg | tail -5                       # 检查加载日志

# 恢复标准驱动方法（卸载 netmap 补丁版，重新加载内核自带驱动）
sudo rmmod ixgbe
sudo modprobe ixgbe                    # 加载发行版自带的驱动
```

---

## 运行与测试

### 使用 pkt-gen 进行吞吐量测试

`pkt-gen` 是 netmap 最常用的性能测试工具，支持发送和接收两种模式。

```bash
# 基本语法
# pkt-gen -i <interface> [options]

# 发送测试：生成 UDP 包发送到目标
sudo ./pkt-gen -i eth1 -f tx \
    -l 64 \           # 包大小 = 64 字节（最小以太网帧）
    -n 10000000 \     # 发送 1000 万个包
    -R 1000 \         # 每秒发送 1000 个包（速率限制）
    -d 10.0.0.2:1234  # 目标 IP:端口

# 接收测试：接收数据包并统计
sudo ./pkt-gen -i eth1 -f rx

# 收发同时测试（同一接口）
sudo ./pkt-gen -i eth1 -f tx -d 10.0.0.2:1234 &
sudo ./pkt-gen -i eth1 -f rx

# 关键输出指标：
#   pps    = packets per second（每秒数据包数）
#   bps    = bits per second（每秒比特数）
#   avg    = 平均包大小
```

**pkt-gen 全部选项速查：**

| 选项 | 含义 | 示例 |
|------|------|------|
| `-i eth1` | 网络接口 | `-i netmap:eth1`（强制 netmap 模式） |
| `-f tx` | 发送模式 | `-f rx`（接收模式） |
| `-l 64` | 包大小（字节） | `-l 1500`（标准 MTU 包） |
| `-n 10000000` | 包总数 | `-n 0`（无限） |
| `-R 1000` | 速率限制（pps） | `-R 14880000`（10G 线速 w/ 64B 包） |
| `-d ip:port` | 目标地址 | `-d 10.0.0.2:1234` |
| `-D mac` | 目标 MAC | `-D 00:11:22:33:44:55` |
| `-b burst` | 批处理大小 | `-b 512`（默认） |
| `-w wait` | 等待链路 | `-w 5`（等 5 秒） |

### 使用 bridge 进行双向桥接

```bash
# 在两个 netmap 端口之间双向转发流量
sudo ./bridge -i netmap:eth1 -i netmap:eth2
# bridge 从 eth1 收到的包直接转发到 eth2，反之亦然
```

### VALE 交换机测试

```bash
# 创建 VALE 交换机 "vale0"，连接两个 netmap 端口
sudo ./bridge -i vale0:0 -i vale0:1   # 两个虚拟端口背靠背

# 可以将物理端口连接到 VALE 交换机
sudo ./pkt-gen -i vale0:eth1 -f rx &   # eth1 作为 VALE 的一个端口
sudo ./pkt-gen -i vale0:2 -f tx        # 虚拟端口 2 发送包
```

### 性能验证 —— 达到 10G 线速

```bash
# 在双端口 10G 网卡上进行线速测试

# 终端 A（接收端）：
sudo ./pkt-gen -i eth1 -f rx

# 终端 B（发送端）：
sudo ./pkt-gen -i eth2 -f tx \
    -l 64 \                    # 最小帧（64 字节）
    -n 100000000 \             # 1 亿个包
    -b 512                     # 批处理 512 个包

# 预期结果（64B 包）：
#   发送端 pps ≈ 14,880,000 pps（≈ 10 Gbps 线速）
#   接收端 pps ≈ 14,880,000 pps（零丢包）

# 如果未达到线速，排查：
#   1. 确认使用了 netmap-aware 驱动（非 generic 模式）
#   2. 增加 -b 批处理大小（如 -b 1024）
#   3. 将 pkt-gen 绑定到特定 CPU 核心（taskset -c 2）
#   4. 关闭中断合并（ethtool -C eth1 rx-usecs 0）
```

---

## libnetmap —— C 语言 API 封装库

```bash
# 编译 libnetmap
cd build-libnetmap
make

# libnetmap 提供更高级的 C 接口：
#   nm_open()        —— 打开 netmap 端口
#   nm_close()       —— 关闭 netmap 端口
#   nm_inject()      —— 注入数据包
#   nm_dispatch()    —— 从接收环读取一批数据包
#   nm_mmap()        —— 内存映射
```

**使用 libnetmap 的最小化示例结构：**

```c
#include <net/netmap_user.h>

struct nm_desc *nmd = nm_open("netmap:eth0", NULL, 0, NULL);
struct netmap_ring *rxring = NETMAP_RXRING(nmd->nifp, 0);
struct netmap_ring *txring = NETMAP_TXRING(nmd->nifp, 0);

// 发送一个包
struct netmap_slot *slot = &txring->slot[txring->cur];
char *pkt = NETMAP_BUF(txring, slot->buf_idx);
memcpy(pkt, my_packet_data, len);
slot->len = len;
txring->cur = txring->head = nm_ring_next(txring, txring->cur);
ioctl(nmd->fd, NIOCTXSYNC, NULL);   // 通知内核

nm_close(nmd);
```

---

## 实验定制指南

### 实验 1：测量 netmap 在不同批大小下的吞吐量

```bash
# 脚本自动化测试
for burst in 1 2 4 8 16 32 64 128 256 512 1024; do
    echo "Testing burst=$burst"
    sudo ./pkt-gen -i eth1 -f tx -l 64 -n 5000000 -b $burst > result_${burst}.txt
done

# 提取 pps
grep "main_speed" result_*.txt
```

### 实验 2：对比 netmap 驱动 vs Generic 驱动性能

```bash
# 测试 A：使用 netmap-aware 驱动（如 ixgbe）
sudo ./pkt-gen -i eth1 -f tx -l 64 -n 10000000 > native_64.txt

# 测试 B：使用 Generic 驱动（强制非 netmap 模式）
# 卸载 ixgbe 驱动，加载标准内核 ixgbe 驱动
sudo rmmod ixgbe
sudo modprobe ixgbe
sudo ./pkt-gen -i eth1 -f tx -l 64 -n 10000000 > generic_64.txt

# 对比两个结果的 pps 值
```

### 实验 3：VALE 交换机转发延迟测量

```bash
# 配置：桥接端口 A (vale0:0) ↔ 端口 B (vale0:1)
sudo ./bridge -i vale0:0 -i vale0:1 &

# 在端口 A 发送带时间戳的包
sudo ./pkt-gen -i vale0:0 -f tx -l 64 -n 10000 -R 100 &
# 在端口 B 接收并记录延迟
sudo ./pkt-gen -i vale0:1 -f rx -n 10000
```

### 实验 4：测量 netmap 与 IPSWEN 项目其他组件的集成

```bash
# 方案：netmap → optmap (XDP) 管道的吞吐量测试
# 在 XDP 加载的情况下测量 netmap 收发性能
sudo ip link set dev eth1 xdp obj /path/to/optmap.o sec xdp_router
sudo ./pkt-gen -i eth1 -f tx -l 64 -n 10000000 | tee netmap_xdp_throughput.txt
sudo ip link set dev eth1 xdp off
# 对比 XDP 加载/未加载的吞吐量差异
```

### 实验 5：内核模块参数调优

```bash
# netmap 模块支持以下参数（insmod 时传递）
sudo insmod netmap.ko admode=2      # 修改 admode
#   admode=0: 不控制网卡进入 netmap 模式前的中断
#   admode=1: 被动模式
#   admode=2: 主动模式（默认）

# 查看当前模块参数
cat /sys/module/netmap/parameters/*
```

---

## IPSWEN 项目中的集成方案

在 IPSWEN 实验环境中，netmap 作为底层高性能数据面，为上层应用提供原始数据包 I/O：

```
┌──────────────────────────────────────────┐
│  IPSWEN 用户态应用                        │
│  (自定义协议栈 / fakedns / happyfootball) │
│                                          │
│        ↕ libnetmap API                   │
│                                          │
│  ┌──────────────────────────────────┐    │
│  │      netmap (netmap.ko)          │    │
│  │                                  │    │
│  │  ┌──────┐  ┌──────┐  ┌──────┐   │    │
│  │  │ eth0 │  │ eth1 │  │ vale │   │    │
│  │  │(WAN) │  │(LAN) │  │switch│   │    │
│  │  └──────┘  └──────┘  └──────┘   │    │
│  └──────────────────────────────────┘    │
└──────────────────────────────────────────┘
```

---

## 注意事项

- **需要 root 权限：** 加载内核模块、打开 netmap 设备、操作 DMA 缓冲区都需要 root 权限
- **网卡独占：** netmap 模式下的网卡不能同时被内核协议栈使用（IP 地址仍可配置，但数据包不会经过协议栈）
- **驱动冲突：** 如果加载了多个同名驱动（如标准 ixgbe + netmap 版 ixgbe），内核会选择先加载的
- **通用适配器性能：** 使用 Generic 模式（未打补丁的驱动）时，性能比 netmap-aware 模式低 3-10 倍
- **内核版本兼容性：** netmap 最稳定的支持范围是 Linux 4.x 和 5.x。6.x 内核可能需要手动调整补丁
- **中断处理：** 在 netmap 模式下，驱动通常禁用或节流中断，改用轮询模式（polling）——这意味着空闲时 CPU 使用率会升高
- **VALE MAC 学习表大小：** 默认为 1024 条，大型拓扑中可能需要调整（重新编译内核模块）
- **安全考虑：** `/dev/netmap` 设备节点的访问权限应严格控制（netmap 客户端可读写网卡内存中的任何数据包）

---

## 相关项目

- **optmap** —— 基于 eBPF/XDP 的 IP 选项内核级路由，可部署在 netmap 的数据路径上
- **fakedns** —— LD_PRELOAD DNS 代理，可在 netmap 支持的应用程序中透明使用
- **happyfootball** —— Happy Eyeballs 多路径实验，netmap 作为其高性能数据面
- **mrtdb** —— BGP 路由分析平台，其路由数据可导入 netmap 支持的软件路由器
- **libs/wandio-4.2.4-1** —— 高性能 I/O 库，可用于读取 netmap 抓取的数据包
- **FreeBSD netmap** —— FreeBSD 已将 netmap 集成到基本系统中（`src/sys/dev/netmap/`），API 与 Linux 版保持对齐

---

## 附录 A：快速命令汇总

### 获取驱动源码
```bash
cd netmap
./get-e1000          # → netmap/e1000/
./get-e1000e         # → netmap/e1000e/
./get-igb            # → netmap/igb/
./get-ixgbe          # → netmap/ixgbe/
./get-i40e           # → netmap/i40e/
./get-forcedeth.c    # → netmap/forcedeth.c（已在仓库中）
./get-veth.c         # → netmap/veth.c
./get-virtio_net.c   # → netmap/virtio_net.c/
```

### 编译内核模块
```bash
./configure && make
```

### 加载/卸载模块
```bash
sudo insmod netmap.ko              # 加载
sudo modprobe netmap               # 加载（推荐）
sudo rmmod netmap                  # 卸载
lsmod | grep netmap                # 检查
```

### 部署驱动
```bash
# 以 ixgbe 为例
cd ixgbe
make
sudo rmmod ixgbe                   # 卸载旧驱动
sudo insmod ixgbe.ko               # 加载 netmap 版
ethtool -i eth1 | grep ixgbe       # 验证
```

### 吞吐量测试
```bash
sudo ./pkt-gen -i eth1 -f tx -l 64 -n 10000000    # 发送 10M 个 64B 包
sudo ./pkt-gen -i eth1 -f rx                       # 接收并统计
```

### 桥接
```bash
sudo ./bridge -i netmap:eth1 -i netmap:eth2        # 双端口桥接
```

### 查看状态
```bash
cat /proc/net/netmap                   # netmap 状态信息
sudo bpftool prog show                 # 如果同时使用 XDP
```

## 附录 B：Netmap API 核心数据结构

### netmap_ring —— 环形缓冲区描述符

```c
struct netmap_ring {
    int64_t         buf_ofs;    // 缓冲区区域相对于映射起点的偏移
    uint32_t        num_slots;  // 环中的 slot 数量（通常 1024）
    uint32_t        nr_buf_size;// 每个缓冲区的字节数（通常 2048）
    uint16_t        ringid;     // 环索引（0-N）
    uint16_t        dir;        // TX 或 RX
    int32_t         head;       // ★ 用户态写入（TX环中放置新包的位置；RX环中消费包的位置）
    int32_t         cur;        // ★ 内核写入（下一个要处理或已发送的 slot）
    uint32_t        flags;      // 时间戳等标志
    struct netmap_slot slot[];  // ★ 可变长度数组：数据包描述符
};
```

### netmap_slot —— 单个数据包描述符

```c
struct netmap_slot {
    uint32_t buf_idx;   // 缓冲区索引（指向全局预分配缓冲池）
    uint16_t len;       // 数据包长度（字节）
    uint16_t flags;     // NS_BUF_CHANGED（缓冲区内容已更新）, NS_MOREFRAG（分片）
    uint64_t ptr;       // 保留/时间戳（取决于 ring flags）
};
```

### 数据包读写模板

```c
// 接收循环
for (;;) {
    ioctl(fd, NIOCRXSYNC, NULL);  // 或 select/poll
    while (rxring->cur != rxring->head) {
        struct netmap_slot *slot = &rxring->slot[rxring->cur];
        char *pkt = NETMAP_BUF(rxring, slot->buf_idx);
        process_packet(pkt, slot->len);   // ★ 直接访问包内容
        rxring->cur = nm_ring_next(rxring, rxring->cur);
    }
}

// 发送循环
for (;;) {
    txring->head = txring->cur;
    // 填写若干个 slot
    ioctl(fd, NIOCTXSYNC, NULL);
}
```

## 附录 C：驱动设备 ID 速查表

| 驱动 | PCI 设备 ID | 网卡型号 | 速度 |
|------|------------|---------|------|
| e1000 | 0x100E, 0x100F, ... | Intel 82540EM, 82541, ... | 1 Gbps |
| e1000e | 0x105E, 0x105F, ... | Intel I217-LM, I219-V, ... | 1 Gbps |
| igb | 0x1521, 0x1522, ... | Intel I350, I210, I211 | 1 Gbps |
| igc | 0x15F2, 0x15F3, ... | Intel I225-V, I226-V | 2.5 Gbps |
| ixgbe | 0x10FB, 0x10FC, ... | Intel X520, X540, X550 | 10 Gbps |
| ixgbevf | SR-IOV VF | 82599/X540/X550 VF | 10 Gbps |
| i40e | 0x1572, 0x1583, ... | Intel X710, XL710 | 40 Gbps |
| forcedeth | 0x01C3, 0x0066, ... | NVIDIA nForce MCP | 1 Gbps |

## 附录 D：线速计算参考

线速 pps 计算公式：
```
pps = line_rate / (672 + 8 * pkt_size)
```

其中 `line_rate` 为标称链路速率（bps），`pkt_size` 为实际包大小（含 MAC 头 + CRC）。

| 包大小 (B) | 100M | 1G | 10G | 40G |
|-----------|------|-----|------|------|
| 64 | 0.1488 Mpps | 1.488 Mpps | 14.88 Mpps | 59.52 Mpps |
| 128 | 0.0589 Mpps | 0.589 Mpps | 5.89 Mpps | 23.58 Mpps |
| 256 | 0.0367 Mpps | 0.367 Mpps | 3.67 Mpps | 14.70 Mpps |
| 512 | 0.0209 Mpps | 0.209 Mpps | 2.09 Mpps | 8.38 Mpps |
| 1024 | 0.0113 Mpps | 0.113 Mpps | 1.13 Mpps | 4.51 Mpps |
| 1518 | 0.0078 Mpps | 0.078 Mpps | 0.78 Mpps | 3.12 Mpps |

在 VALE 端口上，没有物理链路，吞吐量受限于 CPU/内存（取决于包大小）。
