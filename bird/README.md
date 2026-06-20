# bird —— BIRD Internet Routing Daemon（IPswen 扩展版）

## 概述

本目录包含 **BIRD 2.13.1 的修改版本**，增加了对 **IPswen** 的实验性支持。BIRD 是 Linux/BSD 平台上最成熟的开源 BGP 路由守护进程之一，支持 BGP、OSPF、RIPv2、RIPng、Babel、BFD 等协议。此修改版使 BIRD 能作为 IPswen 地址族的路由反射器/边界路由器。

采用 **源码/二进制/配置三合一** 组织，所有 BIRD 相关文件集中管理。

---

## 目录结构

```
bird/
├── src/                         ← 完整 BIRD 2.13.1 源码（IPswen 补丁）
│   ├── proto/bgp/               ★ BGP 协议核心修改
│   │   ├── bgp.h                SAFI_IPswen=127, AF_IPswen, ipswen_extension
│   │   ├── bgp.c                能力协商、会话管理
│   │   └── attrs.c              路由验证、属性处理
│   ├── proto/mrt/               ★ MRT 转储修改
│   │   ├── mrt.h                MRT_RIB_IPswen=13
│   │   └── mrt.c                IPswen 路由读写
│   ├── nest/ / filter/ / conf/ / lib/ / sysdep/
│   ├── client/                  birdc CLI
│   ├── conf.py                  Python 配置生成
│   ├── control.py               SSH 多节点控制
│   ├── dist.py                  多节点分发
│   ├── start.sh / stop.sh.template
│   ├── nodes.txt                测试床拓扑
│   └── configure / configure.ac / Makefile
│
├── bin/                         编译产物
│   ├── bird  birdc  bird.ctl  bird.old
│
└── configs/                     BIRD 配置
    ├── bird.conf                基础配置
    ├── bird.generated.*.conf    自动生成配置（4个）
    └── node1_*_1000000.conf     百万路由测试（9个: level0-7 + IPv4）
```

---

## IPswen 修改源码详解

### IPswen 地址格式

`A.B.C.D(L)E.F.G...` —— `(L)` 是有符号整数 level（`-7..7`），`E.F.G...` 是 `|L|` 个扩展字节。level=0 退化为标准 IPv4。

### 文件 1：bgp.h —— 协议类型定义

```c
#ifdef HAVE_IPswen
#define BGP_SAFI_IPswen 127
#define BGP_AF_IPswen BGP_AF(BGP_AFI_IPV4, BGP_SAFI_IPswen)
#endif

struct bgp_proto {
    // ...existing fields...
    u8 ipswen_extension;  /* IPswen capability negotiated */
};
```

**设计决策分析：**
- SAFI 127 属于 IANA "Private Use" 范围 (128-255 是私有/实验)，但选 127 是为了靠近标准范围。如果后续需要标准化，可改为 IANA 分配的值。
- IPswen 被归入 AFI=1 (IPv4) 而非 AFI=2 (IPv6)，因为它在语义上是 IPv4 的扩展而非独立协议。
- `BGP_AF` 宏将 AFI 和 SAFI 编码为一个 32 位值，BIRD 内部统一用此值标识地址族。
- `ipswen_extension` 是 `u8` 而非 `bool`，预留了未来扩展为多位标志的可能性（如版本协商）。

**如何自定义：** 如果要支持自己的地址族（如 AFI=1, SAFI=128），在此处添加：
```c
#define BGP_SAFI_MYFAMILY 128
#define BGP_AF_MYFAMILY BGP_AF(BGP_AFI_IPV4, BGP_SAFI_MYFAMILY)
```

### 文件 2：bgp.c —— 会话管理与能力协商

```c
#ifdef HAVE_IPswen
  const struct bgp_af_caps *ipswen_loc = bgp_find_af_caps(local, BGP_AF_IPswen);
  // Compare local and remote capabilities
  // Set p->ipswen_extension = 1 if both sides support it
#endif
```

能力协商流程：
1. 在 BGP OPEN 消息处理中调用 `bgp_find_af_caps()` 查找本地和远端的能力列表
2. 如果两边都包含 `(AFI=1, SAFI=127)`，设置 `p->ipswen_extension = 1`
3. 后续 BGP UPDATE 处理中检查此标志，决定是否接受 IPswen NLRI

**前提条件：** IPswen 通道依赖已有的 IPv4 通道——必须先建立 IPv4 Unicast 会话。这是设计决定的约束：IPswen 被视为 IPv4 的扩展，共享 TCP 连接和 BGP 状态机。

### 文件 3：attrs.c —— 路由验证

```c
#ifdef HAVE_IPswen
  /* Reject IPswen routes for non-IPswen peers */
  if (!p->ipswen_extension)
      // LOG warning and skip the NLRI
#endif
```

防御性编程：即使某 peer 意外发送了 IPswen NLRI（比如因为配置错误），只要能力没有协商，就会拒绝。这避免了将 IPswen 路由注入到不支持的对等体。

### 文件 4+5：mrt.h + mrt.c —— MRT 转储

```c
// mrt.h
#define MRT_RIB_IPswen 13   /* Experimental RIB subtype */

// mrt.c
#ifdef HAVE_IPswen
    // When dumping: write RIB subtype 13 for IPswen entries
    // When reading: parse subtype 13 as IPswen format
#endif
```

MRT TABLE_DUMP_V2 的 RIB Subtype 字段：2=IPv4 Unicast, 4=IPv6 Unicast。13 是实验性值——标准 bgpdump 工具会将其标记为 "Unknown"，需要配合 `../mrtdb/` 中修改版的 MRT 解析。

---

## 配置文件深度解析

### bird.conf

```bird
router id 192.168.1.254;           # 必须全网唯一，通常选 loopback 或管理 IP
debug protocols all;                # 开启所有调试（生产环境应关闭）
log "bird.log" all;

protocol kernel {
    learn;                          # 从 kernel 学习路由（被动获取）
    scan time 10;                   # 每 10 秒扫描一次 kernel 路由表
    ipv4 {
        import filter {
            if (net ~ [192.0.0.0/8+]) then accept;
            reject;
        };
    };
}
```

kernel 协议的 import filter：只从 kernel 接受 `192.0.0.0/8` 范围内的路由。`+` 后缀表示"所有可能的子网掩码"（从 /8 到 /32 都匹配）。其他范围（如 10.0.0.0/8）不会进入 BIRD 路由表。

```bird
protocol static {
    ipv4;
    include "node1_IPSWEN_level0_1000000.conf";  # 100 万条静态路由
}
```

`include` 指令将外部文件的内容直接插入配置。100 万条路由从独立文件加载，不污染主配置文件。

```bird
protocol bgp {
    neighbor fc70::1 as 65151;          # 对端 IPv6 ULA + 私有 ASN
    local fc70::2 port 1109 as 65150;   # 本地非标准端口 + 私有 ASN
    graceful restart;
    multihop;                            # 允许多跳（TTL 默认 64）
}
```

为什么 BGP 用 IPv6 传输？在测试床中，IPv4 地址被 IPswen 和 GRE 隧道占用，使用 IPv6 ULA (`fc70::/7`) 作为带外管理通道避免了地址冲突。端口 1109 非标准——用于绕过可能的 179 端口限制。

### 百万路由文件

`node1_IPSWEN_levelN_1000000.conf` 由 `../mrtdb/generate_bird_config.py` 程序化生成，格式：
```bird
route 10.0.0.0(3)128.129.130/35 via 10.10.10.1;
route 10.0.0.1(3)128.129.130/35 via 10.10.10.1;
... (重复 100 万次，前缀递进)
```

每个 level 对应不同的扩展字节数量。level 0 无扩展（标准 IPv4），level 7 有 7 个扩展字节（前缀长度可达 /39）。

---

## 编译完整指南

### 第一步：环境准备

```bash
sudo apt update && sudo apt install -y \
    build-essential gcc g++ make bison flex \
    autoconf automake libtool pkg-config \
    libreadline-dev libncurses-dev \
    libssh-dev libssl-dev

# 必须安装 IPswen 内核头文件
sudo dpkg -i ../linux-kernel/linux-headers-5.4.0-ipswen-v1-20240124_001_amd64.deb
```

### 第二步：生成构建系统

```bash
cd bird/src

# 如果修改了 configure.ac
autoreconf -i -f

# 如果未修改，直接配置
./configure --prefix=/usr/local 2>&1 | tee configure.log
```

检查 IPswen 支持是否启用：
```bash
grep -i "ipswen" config.h
# 应看到: #define HAVE_IPswen 1
# 如果没有 → 内核头文件未包含 IPswen 定义
```

### 第三步：编译

```bash
make -j$(nproc) 2>&1 | tee build.log

# 错误检查
grep -i "error" build.log
grep -i "warning" build.log | grep -v "deprecated"
```

### 第四步：验证

```bash
ls -lh bird birdc birdcl
file bird          # ELF 64-bit executable
ldd bird           # 检查动态库依赖
./bird --version   # BIRD 2.13.1
```

### 常见编译错误

| 错误 | 原因 | 解决 |
|------|------|------|
| `configure: error: Bison is missing` | Bison 未安装 | `sudo apt install bison` |
| `fatal: Flex not found` | Flex 未安装 | `sudo apt install flex` |
| `#define HAVE_IPswen` 缺失 | 内核头文件不含 IPswen | 安装 headers deb 包 |
| `undefined reference to 'ipswen_aton'` | 链接失败 | 内核头文件已装但 libc 不包含 → 检查 config.log |
| `lex.yy.c` 错误 | Flex/Bison 版本不兼容 | `sudo apt install flex bison` 更新版本 |

---

## 运行与管理

### 启动

```bash
cd bird/bin

# 前台调试
sudo ./bird -l -f

# 后台
sudo ./bird

# 用 start.sh（含 sudo 密码处理）
cd ../src && sudo ./start.sh
```

### birdc 交互式控制

```
bird> show protocols
bird> show route count
bird> show route table master | wc -l      # 统计路由数
bird> show bgp sessions                    # BGP 邻居
bird> show bgp sessions all | grep "routes"  # 路由交换统计
bird> show memory                          # 内存使用
bird> configure soft                       # 不中断会话的软重载
bird> restart                              # 重启
bird> shutdown                             # 退出
```

### 多节点分发

```bash
cd bird/src

# 推到所有节点
python3 dist.py bird birdc birdcl start.sh.template stop.sh.template

# 只推配置
python3 dist.py ../configs/bird.conf ../configs/node1_*.conf
```

`dist.py` 读取 `nodes.txt`（`节点ID IP 端口 用户名 密码`），sshpass+scp 推送。

---

## 自定义实验指南

### 怎样测试不同 IPswen level 的路由收敛时间

```bash
# 加载 level=0（无扩展）
sudo ./bird -c ../configs/bird.conf.with.level0

# 测量收敛
time birdc show route count  # 等待路由稳定

# 切换到 level=7（最复杂）
sudo ./bird -c ../configs/bird.conf.with.level7
# 对比收敛时间
```

### 怎样修改 BGP 参数做压力测试

```bird
protocol bgp {
    neighbor fc70::1 as 65151;
    hold time 10;              # 极短的 hold timer（默认 240）
    keepalive time 3;          # 频繁 keepalive
    export all;                # 导出所有路由（增大 UPDATE 体积）
    import all;                # 接受所有路由
    enable route refresh on;   # 开启 ROUTE_REFRESH 能力
}
```

### 怎样添加新的 BGP 能力协商

在 `bgp.h` 中定义新的能力码，在 `bgp.c` 的 OPEN 构建和解析中添加对应的 Capability TLV。

### 怎样监控 BIRD 性能随时间的变化

```bash
# 脚本化监控
while true; do
    echo "$(date +%s) $(birdc 'show route count' | grep -oP '\d+')" >> route_count.log
    sleep 60
done
```

---

## 注意事项

- **内存：** 100 万条静态路由 ≈ 1-2 GB；1000 万条 ≈ 10-15 GB
- **IPswen 条件编译：** 无 IPswen 头文件时 BIRD 等同于上游版本
- **私有 AS：** 65150/65151 仅在实验网络有效
- **配置原子性：** `birdc configure` 不会中断已建立的 BGP 会话
- **日志：** `bird.log` 持续增长——定期轮转

---

## 相关项目

- **linux-kernel** —— IPswen 内核基础
- **iproute2** —— `ip route` 的 IPswen 版
- **net-tools** —— `ifconfig`/`route` 的 IPswen 版
- **mrtdb** —— BGP 数据分析、BIRD 配置生成
- **autoconfig** —— BIRD 对等体间 GRE 隧道

---

## 附录 A: 快速操作卡片

### 启动/停止
```bash
sudo ./bird -l                          # 前台调试
sudo ./bird                             # 后台
sudo killall bird                       # 停止
```

### birdc 常用命令速查
```
show protocols           # 协议状态
show route count         # 路由总数
show route table master  # 主路由表
show bgp sessions        # BGP 邻居
show memory              # 内存使用
configure soft           # 热重载（不中断会话）
shutdown                 # 优雅退出
```

### 文件位置速查
| 内容 | 路径 |
|------|------|
| 源码 | `bird/src/` |
| 编译产物 | `bird/bin/` |
| 配置文件 | `bird/configs/` |
| 启动脚本 | `bird/src/start.sh` |
| 节点定义 | `bird/src/nodes.txt` |

## 附录 B: 测试床拓扑模板

`nodes.txt` 文件格式例：
```
1 192.168.1.1 22 root mypass
2 192.168.1.2 22 root mypass
...
8 192.168.1.8 22 root mypass
```
每行: `节点ID IP SSH端口 用户名 密码`。空行分隔不同网络（如 IPv4 / IPv6 / IPswen 子网）。`dist.py` 和 `control.py` 解析此文件执行多节点操作。

## 附录 C: 基准性能数据参考

| 场景 | 路由数 | 内存 | 收敛时间 |
|------|--------|------|---------|
| 空配置 | 0 | ~15 MB | <1s |
| 100 万条静态 | 1M | ~1.2 GB | ~30s |
| 100 万条 BGP | 1M | ~1.5 GB | ~2-5 min |

建议实验前用 `free -h` 确认可用内存 > 所需内存的 1.5 倍。

---

## 附录 E: 大文件分块说明（重要！）

### 为什么配置文件被分块？

`bird/configs/` 中的 9 个百万路由配置文件每个超过 **100 MB**（GitHub 单文件限制），已被拆分为 `.part*` 分块文件。每个文件被分成 2 个 95MB 的块。

### 受影响的文件

| 原始文件 | 大小 | 分块数 |
|---------|------|--------|
| `node1_IPv4_1000000.conf` | 150 MB | 2 |
| `node1_IPSWEN_level0_1000000.conf` | 152 MB | 2 |
| `node1_IPSWEN_level1_1000000.conf` | 155 MB | 2 |
| `node1_IPSWEN_level2_1000000.conf` | 158 MB | 2 |
| `node1_IPSWEN_level3_1000000.conf` | 161 MB | 2 |
| `node1_IPSWEN_level4_1000000.conf` | 165 MB | 2 |
| `node1_IPSWEN_level5_1000000.conf` | 168 MB | 2 |
| `node1_IPSWEN_level6_1000000.conf` | 172 MB | 2 |
| `node1_IPSWEN_level7_1000000.conf` | 175 MB | 2 |

### 还原方法

**方法 1: 使用项目根目录的一键还原脚本（推荐）**
```bash
cd /path/to/ipswen-project
bash reassemble.sh
```
该脚本自动还原所有被分块的大文件并删除 .part 文件。

**方法 2: 手动还原单个文件**
```bash
cd bird/configs
cat node1_IPv4_1000000.conf.part00 node1_IPv4_1000000.conf.part01 > node1_IPv4_1000000.conf
wc -l node1_IPv4_1000000.conf   # 应输出 1000000
```

**方法 3: 仅还原你需要的 level**
```bash
cd bird/configs
cat node1_IPSWEN_level3_1000000.conf.part00 node1_IPSWEN_level3_1000000.conf.part01 > node1_IPSWEN_level3_1000000.conf
```

### 验证还原完整性

```bash
wc -l bird/configs/node1_*.conf    # 每个文件应包含 100 万条 route 语句
ls -lh bird/configs/node1_IPSWEN_level*.conf
```

### 注意事项

- **磁盘空间**: 还原 9 个文件共需约 1.5 GB 额外空间
- `.part` 文件在还原后可删除（`reassemble.sh` 会自动删除）
- 还原后的原始文件名已加入 `.gitignore`，不会被重新提交
