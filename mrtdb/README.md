# mrtdb —— 全球路由连通性测试 Platform / Global Routing Connectivity Testing Platform

## 项目概述 / Project Overview

`mrtdb` 是整个研究的核心实验平台，用于开展 **互联网规模的路由连通性测试**。它整合了全球 BGP 路由数据、自研的高性能内存数据库引擎、AS 级 IP 映射算法，以及基于 Scapy 的大规模主动探测框架。
`mrtdb` is the core experimental platform for the entire research, used for conducting **Internet-scale routing connectivity testing**. It integrates global BGP routing data, a self-developed high-performance in-memory database engine, AS-level IP mapping algorithms, and a large-scale active probing framework based on Scapy.

**核心研究问题：**
**Core Research Questions:**
1. 从测试床出发，全球哪些 AS/IP 是可达的、哪些不可达
   From the testbed, which ASes/IPs are reachable globally and which are not
2. 互联网路由器如何处理标准和非标准的 IP 选项（透明性测量）
   How Internet routers handle standard and non-standard IP options (transparency measurement)
3. GRE 隧道封装下、带 IP 选项时的连通性表现
   Connectivity performance under GRE tunnel encapsulation with IP options
4. AS 级转发路径发现和 Tier-1 运营商覆盖范围映射
   AS-level forwarding path discovery and Tier-1 ISP coverage mapping

**数据流水线：** 真实 BGP RIB dump → bgpdump 解析 → C 语言内存数据库索引 → Python API 查表 → Scapy 大规模主动探测 → 结果汇总分析
**Data Pipeline:** Real BGP RIB dump → bgpdump parsing → C language in-memory database indexing → Python API lookup → Scapy large-scale active probing → Result aggregation and analysis

---

## 目录结构 / Directory Structure

```
mrtdb/
│
├── 【顶层 Python 源码 —— 因交叉 import，不可拆分到子目录】/ [Top-level Python source — cannot be split into subdirectories due to cross imports]
│   ├── BGPdump.py              # MRT 文件解析器（CFFI 封装 libbgpdump）/ MRT file parser (CFFI wrapper for libbgpdump)
│   ├── CDefs.py                # C 类型定义（bgpdump 内部结构体）/ C type definitions (bgpdump internal structs)
│   ├── mrtdb_user.py           # MRT DB Python API（封装 cmrtdb.so）/ MRT DB Python API (wraps cmrtdb.so)
│   ├── pktest.py               # 核心探测框架引擎 / Core probing framework engine
│   ├── optest.py               # IP 选项路径发现 (traceroute+选项) / IP option path discovery (traceroute+options)
│   ├── test_ip.py              # 大规模多 Worker 并行测试器 / Large-scale multi-worker parallel tester
│   ├── test_retry.py           # TCP 重试分析 / TCP retry analysis
│   ├── asprefix.py             # 前缀→ASN 查表（纯 Python 前缀树）/ Prefix-to-ASN lookup (pure Python prefix tree)
│   ├── mrtdb_test.py           # Tier-1 IP 选取（BGP数据驱动）/ Tier-1 IP selection (BGP data driven)
│   ├── gre_test.py             # GRE+IP选项综合测试 CLI / GRE+IP option comprehensive test CLI
│   ├── ip_options_auto_test.py # 自动化多模式 IP 选项测试 / Automated multi-mode IP option testing
│   ├── ping.py                 # 定时 GRE Ping 调度器 / Scheduled GRE Ping scheduler
│   ├── rt.py                   # 交互式 Scapy traceroute / Interactive Scapy traceroute
│   ├── cli.py / 80.py / srv.py # Socket 级测试工具 / Socket-level test tools
│   ├── udpclient.py / udpserver.py    # Socket 级 UDP 测试工具 / Socket-level UDP test tools
│   └── generate_bird_config.py # BIRD 配置自动生成 / BIRD configuration auto-generation
│
├── notebooks/                  # Jupyter Notebook (5个) / Jupyter Notebook (5 files)
├── testdata/                   # JSONL 测试数据 (~18个文件，含 ~300MB all_targets.jsonl) / JSONL test data (~18 files, including ~300MB all_targets.jsonl)
├── results/                    # 抓包(.cap)、CSV、图片 / Packet captures (.cap), CSV, images
├── cmrtdb/                     # 自研 C 语言 MRT 数据库引擎 / Self-developed C language MRT database engine
├── libbgpdump-1.6.0/           # 第三方 MRT 解析 C 库 / Third-party MRT parsing C library
├── mrt_data/                   # MRT 原始解析数据 (~130个文件) / MRT raw parsed data (~130 files)
├── control/                    # 实验编排（SSH多节点控制、ping变体）/ Experiment orchestration (SSH multi-node control, ping variants)
└── .vscode/                    # VS Code 工作区 / VS Code workspace
```

---

## 核心组件详细源码解析 / Detailed Source Code Analysis of Core Components

### 1. BGPdump.py —— MRT 解析器（最底层）/ MRT Parser (Lowest Level)

这是整个平台的基础层。通过 Python CFFI 封装 C 语言 `libbgpdump`，在 Python 中直接解析 MRT 二进制格式。
This is the foundational layer of the entire platform. It wraps the C library `libbgpdump` via Python CFFI, allowing direct parsing of MRT binary format in Python.

#### 地址族识别（3 个判断函数）/ Address Family Identification (3 Judgment Functions)

```python
def addr_is_IPv4(addr):
    b = struct.unpack('IIHHI', addr)
    return True if b[0] == 0 and b[1] == 0 and b[2] == 0 and b[3] == 0xffff else False
```
MRT 文件中所有地址统一为 16 字节。IPv4-mapped 地址格式为 `::ffff:A.B.C.D`（即前 10 字节为 0、第 11-12 字节为 `0xffff`、末尾 4 字节为实际 IPv4）。这里用 `struct.unpack('IIHHI')` 将 16 字节解释为：2 个 32 位无符号整数 + 1 个 16 位 + 1 个 16 位 + 1 个 32 位，分别检查。
All addresses in MRT files are uniformly 16 bytes. The IPv4-mapped address format is `::ffff:A.B.C.D` (first 10 bytes are 0, bytes 11-12 are `0xffff`, last 4 bytes are the actual IPv4 address). Here, `struct.unpack('IIHHI')` interprets the 16 bytes as: 2 unsigned 32-bit integers + 1 16-bit + 1 16-bit + 1 32-bit, checking each respectively.

```python
def addr_is_IPswen(addr):
    b = struct.unpack('BBBBBBBBBBBBBBBb', addr)
    return True if b[0] == 0 and b[1] == 0 and b[2] == 0 and b[3] == 0xff else False
```
IPswen 地址的 MRT 表示：前 4 字节为 0，第 5 字节为 `0xff`，后续字节包含 IPv4 部分 + level + 扩展。按 `BBBBBBBBBBBBBBBb` 解包为 16 个无符号字节 + 1 个有符号字节（最后一个字节为 level）。
The MRT representation of an IPswen address: first 4 bytes are 0, the 5th byte is `0xff`, subsequent bytes contain the IPv4 portion + level + extension. Unpacked as `BBBBBBBBBBBBBBBb` into 16 unsigned bytes + 1 signed byte (the last byte is the level).

```python
def addr_is_IPv6(self, addr):
    return False if addr_is_IPv4(addr) or addr_is_IPswen(addr) else True
```
排他法：既不是 IPv4 也不是 IPswen，则为纯 IPv6。
By exclusion: if it is neither IPv4 nor IPswen, it is pure IPv6.

#### ip2string() 地址格式化 / Address Formatting

```python
def ip2string(ip):
    if addr_is_IPv4(ip):
        return inet_ntop(AF_INET, ip[12:])
    if addr_is_IPswen(ip):
        b = struct.unpack('BBBBBBBBBBBBBBBb', ip)
        if b[-1] == 0:
            return inet_ntop(AF_INET, ip[4:8])
        level = b[-1] if b[-1] > 0 else b[-1] * (-1)
        return '.'.join([str(i) for i in b[4:8]]) + ('(%d)' % (b[-1])) + \
               '.'.join([str(b[i]) for i in range(8, 8 + level)])
    return inet_ntop(AF_INET6, ip)
```

对于 IPswen 地址的处理逻辑：
Processing logic for IPswen addresses:
- `b[4:8]` 是内嵌的 IPv4 部分（4 个字节）
  These are the embedded IPv4 portion (4 bytes)
- `b[-1]` 是 level（有符号整数，正数表示前向扩展、负数表示后向）
  This is the level (signed integer; positive means forward extension, negative means backward)
- level=0 时等价于标准 IPv4，直接 `inet_ntop`
  When level=0, it is equivalent to standard IPv4, directly use `inet_ntop`
- level≠0 时，格式为 `A.B.C.D(L)E.F.G...`，其中扩展字节从 `b[8]` 开始、数量为 `|level|`
  When level≠0, the format is `A.B.C.D(L)E.F.G...`, where extension bytes start from `b[8]` and the count is `|level|`

**如何修改以适应新的地址格式：** 如果你的地址编码规则变了（比如 level 放在不同位置、扩展字节长度计算方式不同），只需修改这里的解析逻辑。`addr_is_*` 和 `ip2string` 是所有上层模块的入口，改一处即可全局生效。
**How to modify for new address formats:** If your address encoding rules change (e.g., level placed at a different position, different extension byte length calculation), you only need to modify the parsing logic here. `addr_is_*` and `ip2string` are the entry points for all upper-layer modules — change them once and it takes effect globally.

#### BGPAttributes 类 —— 路径属性解析 / Path Attribute Parsing

```python
class BGPAttributes:
    def __init__(self, bgp, attributes):
        self.bgp = bgp              # 父 BGP 对象（持有 FFI 实例）/ Parent BGP object (holds FFI instance)
        self.attributes = attributes # C 结构体指针 / C struct pointer
        self._cluster = None        # Cluster List 延迟解析 / Cluster List lazy parsing
```

**nexthop 属性的 IPswen 处理：**
**IPswen handling of the nexthop attribute:**
```python
@property
def nexthop(self):
    if self.attributes.mp_info.announce[2][1] != self.bgp.ffi.NULL:
        nh = self.bgp.ffi.buffer(
            self.bgp.ffi.addressof(self.attributes.mp_info.announce[2][1].nexthop.v6_addr))
        return ip2string(nh[0:4] + nh[12:16] + nh[4:12] if addr_is_IPswen(nh) else nh)
    return inet_ntop(AF_INET, self.bgp.ffi.buffer(
        self.bgp.ffi.addressof(self.attributes.nexthop))[:])
```

这段代码是 IPswen 兼容的关键：如果路由是通过 MP_REACH_NLRI（BGP 的 Multiprotocol Reachable NLRI 属性）宣告的，它检查 `mp_info.announce[2][1]` 指针是否非空（即存在 MP 下一跳信息）；如果该下一跳被判定为 IPswen 地址，则**重排字节顺序**：`nh[0:4] + nh[12:16] + nh[4:12]`。这是因为 bgpdump 库内部对 IPv6 地址的字节排列与 IPswen 预期不同。
This code is key to IPswen compatibility: if a route is advertised via MP_REACH_NLRI (BGP's Multiprotocol Reachable NLRI attribute), it checks whether the `mp_info.announce[2][1]` pointer is non-null (i.e., MP next-hop information exists); if the next-hop is determined to be an IPswen address, it **reorders the bytes** as `nh[0:4] + nh[12:16] + nh[4:12]`. This is because the internal byte ordering of IPv6 addresses in the bgpdump library differs from what IPswen expects.

**asPath 的 AS Set 展开逻辑：**
**AS Set expansion logic for asPath:**
```python
@property
def asPath(self):
    if self.attributes.aspath == self.bgp.ffi.NULL:
        return []
    as_path = self.bgp.ffi.string(self.attributes.aspath.str).decode().split()
    as_path_len = len(as_path)
    as_path_cnt = 1
    for i in as_path:
        if i[0] == '{':
            j = i[1:-1].split(',')
            as_path_cnt *= len(j)          # 笛卡尔积总数 / Cartesian product total count
    as_path_list = []
    for i in range(as_path_cnt):
        cnt = as_path_cnt
        ls = []
        for j in as_path:
            if ord(j[0]) >= ord('1') and ord(j[0]) <= ord('9'):
                ls.append(int(j))          # 普通 ASN / Regular ASN
            elif j[0] == '{':
                k = j[1:-1].split(',')
                klen = len(k)
                cnt = cnt // klen
                ls.append(int(k[i // cnt % klen]))  # 展开 AS Set / Expand AS Set
        as_path_list.append(ls)
    return as_path_list
```

bgpdump 库输出的 AS Path 文本格式中，AS Set 用 `{AS1,AS2,AS3}` 表示。这段代码将它展开为所有可能的路径组合（笛卡尔积）。举例：AS Path `65001 {65002,65003} 65004` 展开为 `[[65001,65002,65004], [65001,65003,65004]]`。
In the AS Path text format output by the bgpdump library, AS Sets are represented as `{AS1,AS2,AS3}`. This code expands them into all possible path combinations (Cartesian product). For example: AS Path `65001 {65002,65003} 65004` expands to `[[65001,65002,65004], [65001,65003,65004]]`.

**如何修改：** 如果你的 BGP 数据包含你自定义的路径属性（如 Large Community、自定义 Attribute），可以在 `BGPAttributes` 中添加新的 `@property`，通过 CFFI 指针访问对应的 C 结构体字段。
**How to modify:** If your BGP data contains custom path attributes (such as Large Community or custom Attributes), you can add new `@property` methods in `BGPAttributes` to access the corresponding C struct fields via CFFI pointers.

---

---

### 2. CDefs.py —— C 类型定义 / C Type Definitions

CDefs.py 是一个长字符串 `CTypes`，内容是 C 语言头文件风格的类型声明，供 `cffi.FFI().cdef()` 解析。它定义了：
CDefs.py is a long string `CTypes` containing C header-file-style type declarations, parsed by `cffi.FFI().cdef()`. It defines:

- 基础类型：`u_char`, `u_int8_t`, `u_int16_t`, `u_int32_t`, `time_t` / Basic types: `u_char`, `u_int8_t`, `u_int16_t`, `u_int32_t`, `time_t`
- 网络结构体：`struct in_addr`（`in_addr_t s_addr`），`struct in6_addr`（union 含 3 种访问方式） / Network structs: `struct in_addr` (`in_addr_t s_addr`), `struct in6_addr` (union with 3 access methods)
- bgpdump 内部：`struct _CFRFILE`（压缩文件抽象层的 10 个字段）、BGP 属性标志位常量（`BGP_ATTR_FLAG_OPTIONAL=0x80` 等）、属性类型码（`BGP_ATTR_ORIGIN=1` 到 `BGP_ATTR_LARGE_COMMUNITIES=32`） / bgpdump internals: `struct _CFRFILE` (10 fields of compressed file abstraction layer), BGP attribute flag constants (`BGP_ATTR_FLAG_OPTIONAL=0x80`, etc.), attribute type codes (`BGP_ATTR_ORIGIN=1` to `BGP_ATTR_LARGE_COMMUNITIES=32`)
- AS Path 段类型：`AS_SET=1`, `AS_SEQUENCE=2` / AS Path segment types: `AS_SET=1`, `AS_SEQUENCE=2`
- MP-BGP 地址族：`AFI_IP=1`, `AFI_IP6=2`, `SAFI_UNICAST=1` 等 / MP-BGP address families: `AFI_IP=1`, `AFI_IP6=2`, `SAFI_UNICAST=1`, etc.

**如何修改：** 如果 libbgpdump 被升级或你打了新的补丁（比如增加新的 MRT 子类型），需要同步更新 CDefs.py 中的常量定义和结构体声明。CFFI 要求 cdef 中的声明与 C 头文件完全一致。
**How to modify:** If libbgpdump is upgraded or you apply new patches (e.g., adding new MRT subtypes), you need to synchronously update the constant definitions and struct declarations in CDefs.py. CFFI requires that the declarations in cdef exactly match the C header files.

---

---

### 3. mrtdb_user.py —— MRT 数据库 Python API / MRT Database Python API

这是用户与 MRT 数据库交互的主要入口。核心是 `MRTDB` 类。
This is the main entry point for users to interact with the MRT database. The core is the `MRTDB` class.

#### 初始化流程 / Initialization Flow

```python
class MRTDB:
    __slots__ = ['ffi', 'libc', 'libcmrtdb', 'CTypes', 'mrtdb']
    def __init__(self, dir_name = './data'):
        self.CTypes = ''' ... '''  # 70+ 行 C 结构体声明
        self.ffi = FFI()
        self.libc = self.ffi.dlopen(None)         # 加载 libc（标准 C 库）
        self.libcmrtdb = self.ffi.dlopen('./cmrtdb/libcmrtdb.so')  # 加载自研库
        self.ffi.cdef(self.CTypes)                 # 解析 C 类型声明
        self.mrtdb = self.libcmrtdb.mrtdb_init(dir_name.encode())  # 打开数据库
```

关键点：
Key points:
- `__slots__` 限制实例只能有 5 个属性，节省内存
  `__slots__` restricts instances to only 5 attributes, saving memory
- `self.libc = self.ffi.dlopen(None)` —— `None` 意味着加载当前进程的全局符号表（即 libc），可以调用 `malloc`/`free` 等
  `None` means loading the current process's global symbol table (i.e., libc), allowing calls to `malloc`/`free` etc.
- `mrtdb_init()` 返回一个 `struct mrtdb*` 指针，保存在 `self.mrtdb` 中
  `mrtdb_init()` returns a `struct mrtdb*` pointer, stored in `self.mrtdb`

#### CTypes 中的结构体内存布局 / Structure Memory Layout in CTypes

内部声明的 `struct mrtdb` 布局：
```c
struct mrtdb {
    char dir_name[32];
    int prefix_ipv4_cnt;   // IPv4 前缀计数 / IPv4 prefix count
    int prefix_ipv6_cnt;   // IPv6 前缀计数 / IPv6 prefix count
    int prefix_ipswen_cnt; // IPswen 前缀计数 / IPswen prefix count
    int route_ipv4_cnt;    // IPv4 路由计数 / IPv4 route count
    int route_ipv6_cnt;
    int route_ipswen_cnt;
    uint32_t key_list_len[4];       // 每种 key 类型的链表长度 / Linked list length for each key type
    struct list_head key_list[4];   // 4 个双向链表头（prefix/as/as_path/next_hop）/ 4 doubly-linked list heads (prefix/as/as_path/next_hop)
    struct rb_root key_rbtree[4];   // 4 棵红黑树根（O(log n)查找）/ 4 red-black tree roots (O(log n) lookup)
};
```

四个 key 索引的含义（从代码注释可知）：
Meaning of the four key indices (from code comments):
- `key_list[0]` → **prefix**：按前缀组织的路由
  Routes organized by prefix
- `key_list[1]` → **as**：按 ASN 组织的路由（可查一个 AS 宣告的所有前缀）
  Routes organized by ASN (can query all prefixes announced by an AS)
- `key_list[2]` → **as_path**：按 AS Path 哈希组织的路由
  Routes organized by AS Path hash
- `key_list[3]` → **next_hop**：按下一跳地址组织的路由
  Routes organized by next-hop address

**如何修改：** 如果要增加新的索引维度（如按 Community 值索引），需要在 C 端 `mrtdb_format.h` 中扩展数组大小和增加新的 key 类型，在 Python 端同步修改 `CTypes` 字符串和访问逻辑。
**How to modify:** To add a new index dimension (e.g., indexing by Community value), you need to extend the array size and add a new key type in the C-side `mrtdb_format.h`, and synchronously update the `CTypes` string and access logic on the Python side.

#### ip2asn() —— 最长前缀匹配 / Longest Prefix Match

```python
def ip2asn(self, ip):
    prefix = self.libcmrtdb.mrtdb_query_prefix(self.mrtdb, ip.encode())
    asn = [0]
    if prefix != self.ffi.NULL:
        asn = [prefix.origin_asn[i] for i in range(prefix.origin_asn_len)]
    return asn
```

调用 C 的 `mrtdb_query_prefix()` 执行红黑树最长前缀匹配。返回 `mrtdb_list_t*` 指针（NULL 表示未找到）。结果中的 `origin_asn_len` 表示起源 AS 的数量（可能有多个 AS 宣告同一前缀），`origin_asn[i]` 逐个获取。返回值是列表（多 AS 情况下多个值）。
Calls C's `mrtdb_query_prefix()` to perform a red-black tree longest prefix match. Returns a `mrtdb_list_t*` pointer (NULL means not found). `origin_asn_len` in the result indicates the number of origin ASes (multiple ASes may announce the same prefix), and `origin_asn[i]` retrieves them one by one. The return value is a list (multiple values in the case of multiple ASes).

#### stat() —— 数据库统计 / Database Statistics

```python
def stat(self):
    return [self.mrtdb.prefix_ipv4_cnt, self.mrtdb.prefix_ipv6_cnt,
            self.mrtdb.prefix_ipswen_cnt, self.mrtdb.route_ipv4_cnt,
            self.mrtdb.route_ipv6_cnt, self.mrtdb.route_ipswen_cnt]
```

直接读取 C 结构体字段（CFFI 透明访问），返回 6 个统计数据。
Directly reads C struct fields (transparent access via CFFI), returning 6 statistical values.

#### get_total_ip() —— 全量 AS→IP 导出 / Full AS-to-IP Export

遍历 key_list[1]（AS 维度）的所有条目，对每个 AS 加载其 value_list[2]（起源于该 AS 的所有前缀），用 `ip2string()` 格式化每个前缀，输出到 `total_ip.jsonl`。这是一个重量级操作，遍历整个数据库。
Iterates through all entries in key_list[1] (AS dimension), for each AS loads its value_list[2] (all prefixes originating from that AS), formats each prefix using `ip2string()`, and outputs to `total_ip.jsonl`. This is a heavyweight operation that traverses the entire database.

**如何用于自定义实验：** 如果你需要按其他维度导出数据（如按 Community 过滤、按地理位置过滤），可以在 `get_total_ip()` 的基础上修改遍历逻辑和过滤条件。
**How to use for custom experiments:** If you need to export data by other dimensions (e.g., filtering by Community or geographic location), you can modify the traversal logic and filter conditions based on `get_total_ip()`.

---

---

### 4. pktest.py —— 主动探测框架（完整源码解剖）/ Active Probing Framework (Complete Source Code Anatomy)

#### IP 选项定义 / IP Option Definitions

所有 IP 选项以 Python bytes 定义，遵循 RFC 791 的选项格式：
All IP options are defined as Python bytes, following the option format of RFC 791:
- 字节 0：选项类型（含 Copy-on-Fragmentation 位、Class 位、Number 位）
  Byte 0: Option type (includes Copy-on-Fragmentation bit, Class bits, Number bits)
- 字节 1：选项长度（含自身）
  Byte 1: Option length (includes itself)
- 字节 2+：选项数据
  Byte 2+: Option data

```python
# 标准选项 / Standard Options
none_opt = b''                               # 无选项 / no option
rr_opt = bytes([7, 39, 4] + [0] * 36)        # Record Route: type=7, len=39, ptr=4, 9个槽位 / 9 slots
nop_opt = b'\1'                              # No-Operation: type=1
ts_opt = bytes([68, 40, 5, 0] + [0] * 36)   # Timestamp: type=68, len=40, ptr=5, flag=0

# 实验性选项（type 号覆盖 IANA 空间的各种位组合）/ Experimental options (type numbers covering various bit combinations in the IANA space)
x1_opt = bytes([28, 3, 0])      # type=28  (0x1C): class=0, number=12, copy=0
x2_opt = bytes([156, 3, 0])     # type=156 (0x9C): class=2, number=12, copy=1
x3_opt = bytes([92, 3, 0])      # type=92  (0x5C): class=1, number=12, copy=0
# x4/x5/x6 同理覆盖 copy=1 的 class=2/1/3 / x4/x5/x6 similarly cover copy=1 for classes 2/1/3
```

选项 type 字节的位布局（RFC 791）：
Bit layout of the option type byte (RFC 791):
```
bit 7 (0x80): Copy-on-Fragmentation
bit 6-5:      Class (0=control, 1=reserved, 2=debug, 3=reserved)
bit 4-0:      Option Number
```

所以 x1=0x1C=00011100 表示 class=0, number=12；x2=0x9C=10011100 表示 copy=1, class=2, number=12。这组选项的设计意图是覆盖 copy 位和 class 位的所有组合，系统性测试路由器对它们的处理。
So x1=0x1C=00011100 means class=0, number=12; x2=0x9C=10011100 means copy=1, class=2, number=12. The design intent of this set of options is to cover all combinations of the copy bit and class bits, systematically testing how routers handle them.

#### 变长选项生成 / Variable-Length Option Generation

```python
x1_opt_variants = {}
for i in range(2, 41):
    if i != 3:
        x1_opt_variants['x1_' + str(i)] = bytes([28, i] + [0] * (i - 2))
```
生成 `x1_2` 到 `x1_40`（跳过 `x1_3` 因为与 x1 重复），测试不同长度的选项。注意长度字节 `i` 包括了 type 和 length 自身（即实际 payload 为 `i-2` 字节）。
Generates `x1_2` through `x1_40` (skipping `x1_3` since it duplicates x1), testing options of different lengths. Note that the length byte `i` includes the type and length bytes themselves (i.e., the actual payload is `i-2` bytes).

```python
rr_opt_variants = {}
for i in range(1, 9):
    rr_opt_variants['rr_' + str(i)] = bytes([7, 3 + i * 4, 4] + [0] * i * 4)
```
生成 `rr_1` 到 `rr_8`，对应 1-8 个地址槽位的 Record Route。长度公式：3（type+len+ptr）+ 4×i（每个槽位 4 字节 IPv4 地址）。
Generates `rr_1` through `rr_8`, corresponding to Record Route with 1-8 address slots. Length formula: 3 (type+len+ptr) + 4×i (each slot is 4 bytes for an IPv4 address).

**如何设计自己的实验选项：**
**How to design your own experimental options:**
```python
# 示例：定义一个新的实验选项
# Example: define a new experimental option
options['my_opt'] = bytes([30, 6, 1, 2, 3, 4])  # type=30, len=6, payload=4字节 / payload=4 bytes

# 示例：批量生成变体 / Example: batch generate variants
for payload_len in range(0, 128):
    options[f'my_opt_{payload_len}'] = bytes([30, payload_len + 2] + [0] * payload_len)
```

#### make_packet() —— 探测包构造 / Probe Packet Construction

```python
def make_packet(dst, l4, opt, ttl) -> Packet:
    try:
        flag = list(options.keys()).index(opt) + ttl * len(options)
        if l4 == 'icmp':
            pkt = IP(dst=dst, options=options[opt], ttl=ttl) / ICMP(id=flag, seq=10000-flag)
        elif l4 == 'tcp':
            pkt = IP(dst=dst, options=options[opt], ttl=ttl) / TCP(seq=flag)
        else:
            raise ValueError(f'Unsupported L4 protocol: {l4}')
        return pkt
    except:
        traceback.print_exc()
        exit(1)
```

`flag` 编码了选项类型和 TTL 信息：`options.keys().index(opt)` 是选项在字典中的序号，加上 `ttl * len(options)` 可以唯一标识是哪个探测包。ICMP 模式下用 ICMP ID 和 seq 承载 flag，TCP 模式下用 TCP seq 承载。
`flag` encodes the option type and TTL information: `options.keys().index(opt)` is the index of the option in the dictionary, added to `ttl * len(options)` to uniquely identify which probe packet it is. In ICMP mode, the flag is carried in the ICMP ID and seq; in TCP mode, it is carried in the TCP seq.

**如何扩展：** 要增加新的 L4 协议（如 UDP），只需添加：
**How to extend:** To add a new L4 protocol (e.g., UDP), simply add:
```python
elif l4 == 'udp':
    pkt = IP(dst=dst, options=options[opt], ttl=ttl) / UDP(sport=flag, dport=33434 + ttl)
```

#### TestGroupBase 抽象基类 —— 探测任务生命周期 / Abstract Base Class - Probe Task Lifecycle

```python
class TestGroupBase(ABC):
    def __init__(self, max_retry=4, max_fail=4):
        self.max_retry = max_retry  # 同一 TTL/包的连续重试上限 / Max consecutive retries for the same TTL/packet
        self.max_fail = max_fail    # 连续失败（无响应）上限 / Max consecutive failures (no response)
        self.retry = 0
        self.fail = 0
        self.complete = False

    def update(self, rev: Packet):
        if rev is not None:
            self.fail = 0       # 有响应 → 重置失败计数 / Response received → reset failure counter
            self.retry = 0      # 重置重试计数 / Reset retry counter
            self.update_result(rev)  # 子类实现，记录结果 / Subclass implements, record result
            self.next_packet()       # 子类实现，前进到下一个 TTL / Subclass implements, advance to next TTL
        else:
            self.retry += 1
            if self.retry > self.max_retry:
                self.retry = 0
                self.fail += 1
                if self.fail > self.max_fail:
                    self.complete = True  # 连续 4 次 TTL 都无响应 → 放弃 / 4 consecutive TTLs with no response → give up
                self.next_packet()
```

重试/失败逻辑的设计思想：
Design rationale for retry/failure logic:
- **重试（retry）**：同一个 TTL 没收到响应 → 重发（保持当前 TTL）
  **Retry:** No response for the same TTL → resend (keep current TTL)
- **失败（fail）**：连续 N 个不同 TTL 都没响应 → 判定路径通不了，标记完成
  **Failure:** N consecutive different TTLs with no response → path is deemed unreachable, mark as complete

6 个抽象方法必须由子类实现：`next_packet()`, `get_packet()`, `update_result()`, `get_result()`, `get_key()`, `get_packet_key()` (static), `make_groups()` (static)。
6 abstract methods must be implemented by subclasses: `next_packet()`, `get_packet()`, `update_result()`, `get_result()`, `get_key()`, `get_packet_key()` (static), `make_groups()` (static).

#### TestRunner —— 批量收发编排器 / Batch Send/Receive Orchestrator

```python
class TestRunner:
    def __init__(self):
        self.type = None       # 测试组类型（运行时确定，所有测试必须是同一类型）/ Test group type (determined at runtime, all tests must be the same type)
        self.todo = set()      # 待完成的测试 key 集合 / Set of test keys yet to complete
        self.tests = dict()    # key → TestGroup 实例映射 / key → TestGroup instance mapping

    def add_tests(self, tests: Iterable[TestGroupBase]):
        for t in tests:
            if self.type is None:
                self.type = type(tests[0])
            assert type(t) == self.type   # 禁止混入不同类型的 TestGroup / Mixing different TestGroup types is prohibited
            self.todo.add(t.get_key())
            self.tests[t.get_key()] = t
```

`self.todo` 是一个 `set`，存储所有尚未完成的测试的 key。当测试完成（`done()` 返回 True）时，从 `todo` 中移除。`run_batch()` 从 `todo` 中取前 N 个测试获取其当前数据包、批量发送、匹配响应、更新状态。
`self.todo` is a `set` that stores the keys of all incomplete tests. When a test completes (`done()` returns True), its key is removed from `todo`. `run_batch()` takes the first N tests from `todo`, gets their current packets, sends them in batch, matches responses, and updates status.

```python
def send_packets(self, pkts: List[Packet], *args, **kwargs):
    if len(pkts) == 0:
        return
    ans, unans = sr(pkts, filter='ip', *args, **kwargs)
    for i in unans:
        k = self.type.get_packet_key(i)
        self.tests[k].update(None)    # 无响应 → update(None) / No response → update(None)
        if self.tests[k].done():
            self.todo.remove(k)
    for snd, rev in ans:
        k = self.type.get_packet_key(snd)
        self.tests[k].update(rev)     # 有响应 → update(响应包) / Response received → update(response packet)
        if self.tests[k].done():
            self.todo.remove(k)
```

使用 Scapy 的 `sr()`（Layer 3 send/receive）批量发送和匹配响应。`filter='ip'` 限定只接收 IP 包。异常处理已内置——如果某类包触发异常，`traceback.print_exc()` 会打印堆栈并退出。
Uses Scapy's `sr()` (Layer 3 send/receive) for batch sending and response matching. `filter='ip'` restricts reception to IP packets only. Error handling is built-in — if a packet type triggers an exception, `traceback.print_exc()` prints the stack trace and exits.

**如何设计自己的大规模实验：**
**How to design your own large-scale experiments:**
1. 继承 `TestGroupBase`，实现 7 个抽象方法
   Inherit from `TestGroupBase`, implement the 7 abstract methods
2. `get_key()` 返回能唯一确定此测试的元组
   `get_key()` returns a tuple that uniquely identifies this test
3. `get_packet_key()` 是静态方法，从发出的包中提取 key
   `get_packet_key()` is a static method that extracts the key from the sent packet
4. `make_groups()` 是静态工厂方法，批量创建测试组
   `make_groups()` is a static factory method that batch-creates test groups
5. 实例化 `TestRunner`，`add_tests()` 注册，循环 `run_batch()` 直到 `done()`
   Instantiate `TestRunner`, register with `add_tests()`, loop `run_batch()` until `done()`

#### TestGroupTTL —— TTL 递增 traceroute 实现 / TTL-Incrementing Traceroute Implementation

```python
class TestGroupTTL(TestGroupBase):
    def __init__(self, dst, l4, opt, ttl_min=1, ttl_max=30, *args, **kwargs):
        TestGroupBase.__init__(self, *args, **kwargs)
        self.dst = dst
        self.l4 = l4
        self.opt = opt
        self.ttl_min = ttl_min
        self.ttl_max = ttl_max
        self.ttl = ttl_min       # 当前 TTL / Current TTL
        self.result = []         # 累积的路径信息 / Accumulated path information

    def next_packet(self):
        self.ttl += 1            # TTL 递增 / Increment TTL
        if self.ttl > self.ttl_max:
            self.complete = True

    def get_packet(self):
        if self.complete:
            return None
        return make_packet(self.dst, self.l4, self.opt, self.ttl)

    def update_result(self, rev: Packet):
        if rev[IP].src == self.dst:    # 响应源 IP == 目标 → 到达了 / Response source IP == target → arrived
            self.complete = True
        self.result.append((self.ttl, rev[IP].src))

    def get_result(self):
        if len(self.result) == 0:
            return []
        self.result.sort()
        path = ['*'] * (self.result[-1][0] - self.ttl_min + 1)
        for i, p in self.result:
            path[i - self.ttl_min] = p
        return path     # 如 ['*', '10.0.0.1', '10.0.1.1', '目标IP'] / e.g. ['*', '10.0.0.1', '10.0.1.1', 'target IP']
```

`get_result()` 的输出是一个列表，索引对应 TTL，值为该跳的响应 IP 或 `'*'`（无响应）。
The output of `get_result()` is a list where the index corresponds to the TTL and the value is the responding IP or `'*'` (no response).

`make_groups()` 是类方法，支持笛卡尔积生成：`dst × l4 × opt`，对 `'all'` 字符串自动展开为所有可能的值。
`make_groups()` is a class method that supports Cartesian product generation: `dst × l4 × opt`. The `'all'` string is automatically expanded to all possible values.

#### TestGroupASIP —— 多 IP 尝试单个 AS / Multi-IP Attempt for a Single AS

```python
class TestGroupASIP(TestGroupBase):
    def __init__(self, asn: int, iplist: List[str], demand=1, ...):
        self.asn = asn
        self.iplist = iplist     # 该 AS 的候选 IP 列表 / Candidate IP list for this AS
        self.pos = 0             # 当前尝试到第几个 IP / Current position in the IP list
        self.demand = demand     # 需要多少个成功的 IP / How many successful IPs are needed
        self.result = []
        self.test = TestGroupTTL(iplist[0], l4, opt, ...)

    def update(self, rev: Packet):
        self.test.update(rev)
        if self.test.done():
            r = self.test.get_result()
            if len(r) > 0 and r[-1] == self.iplist[self.pos]:
                self.result.append(r[-1])    # 这个 IP 可达 / This IP is reachable
            self.pos += 1
            if self.pos >= len(self.iplist) or len(self.result) >= self.demand:
                self.complete = True
                return
            self.test = TestGroupTTL(self.iplist[self.pos], ...)  # 换下一个 IP / Switch to the next IP
```

对每个 AS 有多个候选 IP，逐个尝试 traceroute，直到找到 `demand` 个可达的或尝试完所有候选。这个设计解决了"一个 AS 可能有多个 IP，选哪个做探测"的问题。
For each AS with multiple candidate IPs, traceroute is attempted one by one until `demand` reachable IPs are found or all candidates are exhausted. This design solves the problem of "which IP to choose for probing when an AS has multiple IPs."

---

---

### 5. optest.py —— IP 选项路径发现 / IP Option Path Discovery

定义了自己的选项集合（与 pktest.py 不同，更注重未知/保留类型）：

```python
options = {
    'none': b'',
    'rr': bytes([7, 11, 4, 0, 0, 0, 0, 0, 0, 0, 0]),   # 短版 RR（1个槽位）/ Short RR (1 slot)
    'nop': b'\1',
    'unkctl3': bytes([156, 3, 233]),     # type=156(0x9C), len=3, payload=233
    'unkres3': bytes([252, 3, 233]),     # type=252(0xFC), len=3, payload=233
    'unkctl40': bytes([157, 40] + [233] * 38),   # type=157, len=40
    'unkres40': bytes([253, 40] + [233] * 38),   # type=253, len=40
}
```

对比短 RR（11 字节）和长选项（40 字节）的路径差异，可以分析路由器是否因选项长度不同而行为不同。
By comparing path differences between short RR (11 bytes) and long options (40 bytes), you can analyze whether routers behave differently based on option length.

`make_packet()` 用 ICMP ID 编码选项索引和 TTL，`get_packet_group()` 从响应中反向解码。`TestGroup` 类实现了 TTL 递增 + 路径记录。
`make_packet()` encodes the option index and TTL in the ICMP ID, and `get_packet_group()` decodes it back from the response. The `TestGroup` class implements TTL incrementing + path recording.

---

---

### 6. test_ip.py —— 大规模多 Worker 并行测试 / Large-Scale Multi-Worker Parallel Testing

```python
workers = 1    # 修改此变量改变 Worker 数量 / Modify this variable to change the number of workers

def get_slice(all, i):
    n = len(all)
    return all[i * n // workers: (i + 1) * n // workers]
```

`get_slice()` 将 IP 列表均等切分给各 Worker。每个 Worker 只需加载自己那部分数据。
`get_slice()` evenly divides the IP list among workers. Each worker only loads its own portion of the data.

```python
ip = {}
with open('asip.jsonl') as f:
    for line in f:
        a = json.loads(line)
        ip[a[2]] = max(ip.get(a[2], -9999), a[1])   # 每个 AS 保留 score 最高的 IP / Keep the IP with the highest score for each AS
ip = list(ip.items())
np.random.shuffle(ip)    # 随机打乱，避免顺序偏差 / Random shuffle to avoid ordering bias
```

`max(score)` 确保每个 AS 只取 BGP 表中出现次数最多的那个 IP。
`max(score)` ensures each AS only takes the IP with the highest occurrence count in the BGP table.

**如何修改以适应自己的实验：**
**How to modify for your own experiments:**
- 修改 `l4` 和 `opt` 列表改变测什么协议和选项
  Modify the `l4` and `opt` lists to change which protocols and options to test
- 修改 `workers` 控制并行度
  Modify `workers` to control parallelism
- 修改 `ttl_min` / `ttl_max` 改变 traceroute 的 TTL 范围
  Modify `ttl_min` / `ttl_max` to change the TTL range of traceroute
- 将 `asip.jsonl` 替换为自己的目标 IP 数据源
  Replace `asip.jsonl` with your own target IP data source

---

---

### 7. test_retry.py —— TCP 重试分析 / TCP Retry Analysis

```python
l4 = 'tcp'
opt = 'all'
retry = 1

ip = {}
with open('asip.jsonl') as f:
    for line in f:
        a = json.loads(line)
        ip[a[2]] = max(ip.get(a[2], -9999), a[1])
ip = list(ip.items())
np.random.shuffle(ip)
ip = ip[:100]    # 只取 100 个随机 IP / Take only 100 random IPs

runner = TestRunner()
runner.add_tests(TestGroupTTL.make_groups([i[0] for i in ip], l4, opt,
    ttl_min=5, ttl_max=30, max_retry=retry))
with tqdm(total=runner.count_remain(), unit='tests') as pbar:
    while not runner.done():
        pbar.update(runner.run_batch())

stats = {}
for k, p in runner.get_results():
    stats.setdefault(k[2], 0)     # k[2] 是选项名 / k[2] is the option name
    stats[k[2]] += len([i for i in p if i != '*'])
print(stats)    # 每种选项有多少 IP 可达 / How many IPs are reachable for each option type
```

统计每种 IP 选项下，100 个目标中有多少个可达。这是个快速诊断工具——运行 5-10 分钟即可得出"哪种选项在互联网上最透明"的粗略结论。
Counts how many of the 100 targets are reachable under each IP option. This is a quick diagnostic tool — running it for 5-10 minutes yields a rough conclusion on "which options are most transparent on the Internet."

---

---

### 8. asprefix.py —— 前缀→ASN 查表 / Prefix-to-ASN Lookup

纯 Python 实现，不依赖 cmrtdb。
Pure Python implementation, does not depend on cmrtdb.

```python
prefix_as = dict()
for asn, pl in as_prefix.items():
    for prefix in pl:
        ip, n = prefix.split('/')
        i = ipstr2int(ip)
        prefix_as[i] = (int(n), asn)

# 过滤非最小前缀 / Filter out non-minimal prefixes
prefix_as2 = dict()
for i, (n, asn) in prefix_as.items():
    add = True
    m = n
    while m > 0:
        m -= 1
        j = i & (0xffffffff << (32 - m))
        if j in prefix_as and prefix_as[j][0] <= m:
            add = False
            break
    if add:
        prefix_as2[i] = (n, asn)

def get_ip_asn(ip):
    i = ipstr2int(ip)
    for n in range(32, -1, -1):
        j = i & (0xffffffff << (32 - n))
        if j in prefix_as2 and prefix_as2[j][0] <= n:
            return prefix_as2[j]
```

过滤逻辑：如果 `10.0.0.0/8` 在表中，且 `10.0.0.0/24` 也存在，则只保留 `/24`——因为它更具体、更能准确反映该 IP 段的归属。`get_ip_asn()` 从 /32 向下逐位缩短查找，第一个匹配即返回。
Filtering logic: If `10.0.0.0/8` is in the table and `10.0.0.0/24` also exists, only `/24` is kept — because it is more specific and more accurately reflects the ownership of that IP range. `get_ip_asn()` searches from /32 downward, shortening the prefix bit by bit, and returns the first match.

---

---

### 9. ip_options_auto_test.py —— 自动化多模式测试（完整 CLI）/ Automated Multi-Mode Testing (Full CLI)

#### 六种测试模式的包构造差异 / Packet Construction Differences Across Six Test Modes

```python
if mode == "icmp":
    pkt = IP(dst=dst, options=option_bytes, ttl=ttl) / ICMP(id=..., seq=ttl)
elif mode == "tcp80":
    pkt = IP(...) / TCP(sport=..., dport=80, flags="S", seq=...)
elif mode == "tcp443":
    pkt = IP(...) / TCP(sport=..., dport=443, flags="S", seq=...)
elif mode == "tcp":
    pkt = IP(...) / TCP(sport=..., dport=tcp_port, flags="S", seq=...)
elif mode == "udp":
    pkt = IP(...) / UDP(sport=..., dport=udp_base_port + ttl + r * 100)
elif mode == "gre":
    pkt = IP(...) / GRE() / IP(src=inner_src, dst=inner_dst) / ICMP(...)
```

每种模式的 `tag = r * 1000 + ttl`，确保不同 repeat 和 TTL 的探测包有唯一标识。
Each mode uses `tag = r * 1000 + ttl` to ensure unique identification for probe packets with different repeat counts and TTLs.

#### classify_reply() 详解 / Detailed Explanation of classify_reply()

```python
def classify_reply(reply):
    if IP not in reply:
        return "non-ip", reply.summary()
    if ICMP in reply:
        icmp = reply[ICMP]
        if icmp.type == 0:   return "icmp-echo-reply", "target replied"
        if icmp.type == 3:   # Destination Unreachable，按 code 细分 / Destination Unreachable, subdivided by code
            code_map = {0: "network-unreachable", 1: "host-unreachable",
                        3: "port-unreachable", 4: "fragmentation-needed",
                        9: "network-admin-prohibited", 10: "host-admin-prohibited",
                        13: "communication-admin-prohibited"}
            return "icmp-destination-unreachable", code_map.get(icmp.code, f"code={icmp.code}")
        if icmp.type == 11:  return "icmp-time-exceeded", "ttl expired"
        if icmp.type == 12:  return "icmp-parameter-problem", "bad ip header or option"
```

ICMP type=12 是关键指标——如果路由器不理解或拒绝某个 IP 选项，它会返回 Parameter Problem。这是判断"选项是否被中间设备拦截"的直接证据。
ICMP type=12 is a key indicator — if a router does not understand or rejects an IP option, it returns a Parameter Problem. This is direct evidence for determining "whether an option is being blocked by intermediate devices."

TCP 响应分类：
TCP response classification:
```python
    if TCP in reply:
        flags = reply[TCP].flags
        if "S" in str(flags) and "A" in str(flags):
            return "tcp-synack", "port open or reachable"
        if "R" in str(flags):
            return "tcp-rst", "port closed or rejected"
```

#### 命令行参数 / Command Line Arguments

```python
parser.add_argument("--dst", required=True, help="目标 IP 地址 / Target IP address")
parser.add_argument("--mode", default="icmp", help="icmp,tcp80,tcp443,tcp,udp,gre 或 all / icmp,tcp80,tcp443,tcp,udp,gre or all")
parser.add_argument("--option", default="none", help="none,nop,rr,unkctl3,unkres3,unkctl40,unkres40 或 all / none,nop,rr,unkctl3,unkres3,unkctl40,unkres40 or all")
parser.add_argument("--min-ttl", type=int, default=1)
parser.add_argument("--max-ttl", type=int, default=30)
parser.add_argument("--repeat", type=int, default=1)
parser.add_argument("--timeout", type=int, default=2)
parser.add_argument("--output", default=None, help="输出 CSV 文件路径 / Output CSV file path")
parser.add_argument("--tcp-port", type=int, default=80)
parser.add_argument("--inner-src", default="192.168.1.254", help="GRE 内层源IP / GRE inner source IP")
parser.add_argument("--inner-dst", default="192.168.1.255", help="GRE 内层目的IP / GRE inner destination IP")
```

---

## 数据文件格式详解 / Detailed Data File Format Explanation

### asip.jsonl
```json
["IP地址", score, ASN]  # ["IP Address", score, ASN]
```
score 是该 IP 在 BGP 路由表中的加权出现次数。`max(score)` 选取每个 AS 最具代表性的 IP。
`score` is the weighted occurrence count of the IP in the BGP routing table. `max(score)` selects the most representative IP for each AS.

### agg*.jsonl（汇总探测结果）/ Aggregated Probe Results
```json
["协议", "选项类型", "目标IP", ["跳1", "跳2", ..., "最终IP或*"]]  # ["Protocol", "Option Type", "Target IP", ["Hop1", "Hop2", ..., "Final IP or *"]]
```
每天一个文件，按日期命名（aggMMDD.jsonl）。
One file per day, named by date (aggMMDD.jsonl).

### target_ip.jsonl
```json
[ASN, ["ip1", "ip2", ...]]
```
由 `mrtdb_test.py` 生成，每个 AS 保存一个代表性 IP 列表。
Generated by `mrtdb_test.py`, each AS stores a list of representative IPs.

### res.txt
```
<IP地址> <TTL> <延迟毫秒> / <IP Address> <TTL> <Latency ms>
24.56.172.1 5 197.471
```

---

## 完整实验操作流程 / Complete Experiment Workflow

### 环境准备 / Environment Setup

```bash
# 系统依赖 / System dependencies
sudo apt update
sudo apt install -y build-essential cmake gcc g++ make python3 python3-pip python3-dev libpcap-dev tcpdump iproute2 net-tools zlib1g-dev libbz2-dev

# Python 依赖（必须精确安装这些版本以兼容）/ Python dependencies (must install these exact versions for compatibility)
pip3 install --upgrade pip
pip3 install scapy==2.5.0 cffi==1.16.0 numpy tqdm paramiko graphviz dnspython jupyter

# 验证：确保 Scapy 能正常导入 / Verify that Scapy can be imported properly
python3 -c "from scapy.all import *; print('Scapy OK:', conf.version)"
python3 -c "from cffi import FFI; print('CFFI OK')"
```

### 编译 C 组件 / Compiling C Components

```bash
# 1. libbgpdump
cd mrtdb/libbgpdump-1.6.0
chmod +x configure
./configure 2>&1 | tee configure.log
# 检查 configure.log 确保没有错误 / Check configure.log to ensure no errors
make -j$(nproc)
# 验证产物 / Verify compiled artifacts
ls -lh libbgpdump.so libbgpdump.a bgpdump
cd ../..

# 2. cmrtdb 数据库引擎 / cmrtdb database engine
cd mrtdb/cmrtdb
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
ls -lh libcmrtdb.so
cd ../../..

# 3. 设置动态库搜索路径（写入 ~/.bashrc 可永久生效）/ Set dynamic library search path (write to ~/.bashrc for permanent effect)
export LD_LIBRARY_PATH=$(pwd)/mrtdb/cmrtdb/build:$(pwd)/mrtdb/libbgpdump-1.6.0:$LD_LIBRARY_PATH

# 4. 验证 / Verify
cd mrtdb
python3 -c "
import sys; sys.path.insert(0, '.')
from mrtdb_user import MRTDB
m = MRTDB('./mrt_data')
print('MRTDB loaded:', m.stat())
"
```

### 数据流水线 / Data Pipeline

```bash
cd mrtdb

# Step 1: 用 bgpdump 解析 MRT RIB dump / Parse MRT RIB dump using bgpdump
cd libbgpdump-1.6.0
./bgpdump -m -O ../mrt_data/rib_parsed.txt <你的MRT文件.bz2>
# -m: 输出完整 NLRI; -O: 输出文件 / -m: output full NLRI; -O: output file
cd ..

# Step 2: 构建 cmrtdb 索引（如果 MRT 数据已放在 mrt_data/）/ Build cmrtdb index (if MRT data is already placed in mrt_data/)
python3 -c "from mrtdb_user import MRTDB; m = MRTDB('./mrt_data'); print(m.stat())"

# Step 3: 生成 AS→前缀映射 / Generate AS→prefix mapping
python3 asprefix.py

# Step 4: Tier-1 IP 选取 / Tier-1 IP selection
python3 mrtdb_test.py
```

### 运行实验 / Running Experiments

```bash
cd mrtdb

# === 快速诊断：哪种 IP 选项在互联网上最透明 === / === Quick diagnosis: which IP options are most transparent on the Internet ===
sudo python3 test_retry.py

# === IP 选项路径发现（单目标）/ IP option path discovery (single target) ===
sudo python3 optest.py

# === 交互式 traceroute / Interactive traceroute ===
sudo python3 rt.py

# === 自动化多模式测试（单目标、多选项）/ Automated multi-mode testing (single target, multiple options) ===
sudo python3 ip_options_auto_test.py \
    --dst 8.8.8.8 \
    --mode icmp,tcp80,gre \
    --option all \
    --repeat 3 \
    --timeout 3 \
    --output results_$(date +%Y%m%d).csv

# === GRE 隧道 + IP 选项 / GRE tunnel + IP options ===
sudo python3 gre_test.py \
    --dst <GRE对端公网IP> \
    --inner-src 192.168.1.254 \
    --inner-dst 192.168.1.255 \
    --iface-tun tun-XXXX \
    --option unkctl3 \
    --count 5

# === 大规模多 Worker 并行 / Large-scale multi-worker parallel ===
# Worker 0（在终端 0）/ Worker 0 (in terminal 0)
sudo python3 test_ip.py --id 0 > worker0.log 2>&1 &
# Worker 1（在终端 1）/ Worker 1 (in terminal 1)
sudo python3 test_ip.py --id 1 > worker1.log 2>&1 &
# 使用 tmux/screen 管理多个 Worker / Use tmux/screen to manage multiple workers

# === 生成 BIRD 配置 / Generate BIRD configuration ===
python3 generate_bird_config.py \
    --config ../bird/configs/bird.conf \
    --family ipswen \
    --prefix-count 1000000 \
    --level 3 \
    --output my_ipswen_level3.conf
```

---

## 自定义实验指南 / Custom Experiment Guide

### 怎样添加新的 IP 选项类型 / How to Add New IP Option Types

1. 在 `pktest.py` 的选项定义区添加：
   Add it in the option definition area of `pktest.py`:
```python
# 在 options dict 附近 / Near the options dict
my_new_opt = bytes([30, 6, 1, 2, 3, 4])  # type=30, len=6
options['my_new_opt'] = my_new_opt
```

2. 如果要在 `optest.py` 和 `ip_options_auto_test.py` 中也使用，需同步添加。或者统一从 `pktest.py` import（需确保无循环 import）。
   If you also want to use it in `optest.py` and `ip_options_auto_test.py`, add it there as well. Alternatively, import consistently from `pktest.py` (ensure no circular imports).

### 怎样修改探测目标集 / How to Modify Probe Target Sets

1. 修改 `mrtdb_test.py` 中的 Tier-1 AS 列表和已知 BGP Speaker IP
   Modify the Tier-1 AS list and known BGP Speaker IPs in `mrtdb_test.py`
2. 或直接编辑 `testdata/target_ip.jsonl` 格式为 `[asn, [ip1, ip2]]`
   Or directly edit `testdata/target_ip.jsonl` in the format `[asn, [ip1, ip2]]`
3. `test_retry.py` 中用到的 AS 列表可在代码内直接修改
   The AS list used in `test_retry.py` can be modified directly in the code

### 怎样添加新的 L4 协议模式 / How to Add New L4 Protocol Modes

1. 在 `pktest.py` 的 `make_packet()` 中添加 `elif l4 == 'sctp': ...`
   Add `elif l4 == 'sctp': ...` in `make_packet()` of `pktest.py`
2. 在 `pktest.py` 的 `get_packet_l4()` 中添加 `elif pkt.haslayer(SCTP): return 'sctp'`
   Add `elif pkt.haslayer(SCTP): return 'sctp'` in `get_packet_l4()` of `pktest.py`
3. 在 `gre_test.py` 和 `ip_options_auto_test.py` 的 `make_packets()` 中同样添加
   Also add it in `make_packets()` of `gre_test.py` and `ip_options_auto_test.py`

### 怎样修改重试策略 / How to Modify Retry Strategy

修改 `TestGroupBase.__init__()` 的默认参数：
Modify the default parameters of `TestGroupBase.__init__()`:
```python
def __init__(self, max_retry=8, max_fail=6):  # 更激进的重试 / More aggressive retry
```
或在创建测试组时传入：
Or pass them when creating the test group:
```python
TestGroupTTL(dst, l4, opt, max_retry=8, max_fail=6)
```

### 怎样增加拓扑可视化 / How to Add Topology Visualization

`mrtdb_user.py` 已经 import 了 `graphviz`。可以添加：
`mrtdb_user.py` has already imported `graphviz`. You can add:
```python
def draw_topology(self, asn):
    dot = Digraph(comment=f'Topology for AS{asn}')
    # ... 添加节点和边 / ... add nodes and edges
    dot.render(f'topology_as{asn}.gv', view=True)
```

### 怎样导出数据用于外部分析 / How to Export Data for External Analysis

所有内部数据都是 Python 原生对象，直接用 `json.dumps()` 导出即可。示例：
All internal data are Python native objects, simply export them using `json.dumps()`. Example:
```python
from mrtdb_user import MRTDB; m = MRTDB()
import json
# 导出所有前缀 / Export all prefixes
with open('all_prefixes.jsonl', 'w') as f:
    # ... 遍历 key_list[0] 并格式化 / ... traverse key_list[0] and format
```

---

## 重要注意事项 / Important Notes

### 交叉 Import 关系（文件不可拆分）/ Cross-Import Relationships (Files Cannot Be Split)
```
mrtdb_user.py ──import──► BGPdump (addr_is_IPv4, addr_is_IPswen, addr_is_IPv6, ip2string)
mrtdb_test.py ──import──► mrtdb_user (MRTDB class)
test_retry.py  ──import──► pktest (TestRunner, TestGroupTTL, ...)
test_ip.py     ──import──► pktest
rt.py          ──import──► pktest (from mrtdb.pktest import *)
```
所有 `.py` 必须保持在 `mrtdb/` 目录顶层平级。
All `.py` files must remain at the top level of the `mrtdb/` directory, at the same level.

### Root 权限 / Root Privileges
Scapy 的 `sr()` / `send()` 需要 raw socket，必须 `sudo` 运行。如果在容器或无特权环境中运行，可以改用 `sendp()`（Layer 2），但需要修改代码。
Scapy's `sr()` / `send()` require raw sockets and must be run with `sudo`. If running in a container or unprivileged environment, you can use `sendp()` (Layer 2) instead, but the code will need modification.

### 大数据内存 / Large Data Memory
- `all_targets.jsonl` ≈ 300MB，加载需要 2-4GB 可用 RAM
  Loading requires 2-4GB of available RAM
- 建议小于 16GB RAM 的机器使用 `all_targets_sample.jsonl`
  Machines with less than 16GB RAM should use `all_targets_sample.jsonl`
- cmrtdb 红黑树内存：10 万前缀 ≈ 50MB，100 万前缀 ≈ 600MB
  cmrtdb red-black tree memory: 100K prefixes ≈ 50MB, 1M prefixes ≈ 600MB

### IP 选项兼容性警告 / IP Option Compatibility Warning
实验性选项（type=28/92/124/156/220/252）**不是 IANA 分配的标准选项**。在互联网上：
Experimental options (type=28/92/124/156/220/252) are **not IANA-assigned standard options**. On the Internet:
- 约 40-60% 路径会透明转发（取决于路径上的设备品牌和配置）
  About 40-60% of paths will transparently forward them (depending on device brand and configuration along the path)
- 约 20-30% 会丢弃
  About 20-30% will drop them
- 约 10-20% 会返回 ICMP Parameter Problem
  About 10-20% will return ICMP Parameter Problem

结果因网络路径、时间、运营商策略而异，需要重复实验取统计显著性。
Results vary by network path, time, and ISP policy; repeated experiments are needed for statistical significance.

---

## 故障排查 / Troubleshooting

| 症状 / Symptom | 诊断步骤 / Diagnosis | 解决方案 / Solution |
|------|---------|---------|
| `ImportError: BGPdump` | `pwd` 是否在 mrtdb/? `ls *.py` / Is `pwd` in mrtdb/? `ls *.py` | `cd mrtdb/` |
| `libcmrtdb.so: cannot open` | `ls cmrtdb/build/libcmrtdb.so` | 重新编译 cmrtdb + 设置 LD_LIBRARY_PATH / Recompile cmrtdb + set LD_LIBRARY_PATH |
| `undefined symbol` from CFFI | libbgpdump 版本不匹配 / libbgpdump version mismatch | 重新编译 libbgpdump，确保 CDefs.py 与 .h 一致 / Recompile libbgpdump, ensure CDefs.py matches .h |
| Scapy `OSError: [Errno 99]` | `ip link show` 检查接口 / Check interface with `ip link show` | 检查 `--iface-outer`/`--iface-tun` 参数 / Check `--iface-outer`/`--iface-tun` parameters |
| 所有探测超时 / All probes timeout | 先用 none 选项测试 / Test with none option first | `--option none` 验证基本连通性 / Verify basic connectivity with `--option none` |
| CSV 输出为空 / CSV output is empty | 检查模式/选项是否正确 / Check mode/option correctness | `--option all --mode all` 然后用 `wc -l` 检查 / Use `--option all --mode all` then check with `wc -l` |
| 内存 OOM / Out of memory | `htop` 检查内存使用 / Check memory usage with `htop` | 使用 sample 数据或增加 swap / Use sample data or increase swap |
| 发包速率太慢 / Packet send rate too slow | 内核速率限制 / Kernel rate limiting | 减小 batch size 到 200，增加 timeout 到 5 / Reduce batch size to 200, increase timeout to 5 |

---

## 附录: 大文件分块说明（重要！）/ Appendix: Large File Splitting Instructions (Important!)

### 为什么数据文件被分块？/ Why Are Data Files Split?

`mrtdb/` 中有 12 个文件超过 GitHub 的 **100 MB** 限制，已被拆分为 `.part*` 分块文件。使用前必须还原。
There are 12 files in `mrtdb/` that exceed GitHub's **100 MB** limit; they have been split into `.part*` chunk files. They must be restored before use.

### mrt_data/ 受影响文件（8 个）/ Affected Files in mrt_data/ (8 files)

| 原始文件 | 大小 | 分块数 | 文件类型 |
| Original File | Size | Chunks | File Type |
|---------|------|--------|---------|
| `prefix.out` | 1.6 GB | 17 | BGP 前缀文本转储 / BGP prefix text dump |
| `prefix_value.bin` | 1.2 GB | 13 | 前缀值二进制索引 / Prefix value binary index |
| `as_path.out` | 274 MB | 3 | AS 路径文本转储 / AS path text dump |
| `prefix_key.bin` | 175 MB | 2 | 前缀键二进制索引 / Prefix key binary index |
| `as.out` | 167 MB | 2 | AS 信息文本转储 / AS info text dump |
| `as_value.bin` | 119 MB | 2 | AS 值二进制索引 / AS value binary index |
| `as_path.bin` | 104 MB | 2 | AS 路径二进制索引 / AS path binary index |
| `rib4.bz2` | 103 MB | 2 | 压缩的 RIB 转储 / Compressed RIB dump |

### testdata/ 受影响文件（4 个）/ Affected Files in testdata/ (4 files)

| 原始文件 | 大小 | 分块数 | 说明 |
| Original File | Size | Chunks | Description |
|---------|------|--------|------|
| `agg0424.jsonl` | 296 MB | 4 | 4月24日汇总探测结果 / Aggregated probe results for Apr 24 |
| `all_targets.jsonl` | 287 MB | 4 | 完整目标IP集合 / Complete target IP set |
| `agg0421.jsonl` | 120 MB | 2 | 4月21日汇总探测结果 / Aggregated probe results for Apr 21 |
| `agg0310.jsonl` | 104 MB | 2 | 3月10日汇总探测结果 / Aggregated probe results for Mar 10 |

### 还原方法 / Restoration Methods

**方法 1: 一键还原（推荐）**
**Method 1: One-click restoration (recommended)**
```bash
cd /path/to/ipswen-project
bash reassemble.sh
```

**方法 2: 逐文件手动还原（适用于只需还原特定文件）**
**Method 2: Manual file-by-file restoration (for restoring specific files only)**
```bash
cd mrtdb/mrt_data
cat prefix_key.bin.part00 prefix_key.bin.part01 > prefix_key.bin

# 还原大文件需要更多磁盘空间 / Restoring large files requires more disk space
cat prefix.out.part00 prefix.out.part01 ... prefix.out.part16 > prefix.out
ls prefix.out.part* | wc -l   # 确认 part 数量：应为 17 / Confirm part count: should be 17
```

**方法 3: 仅还原 JSONL 测试数据（不需要编译 cmrtdb 时）**
**Method 3: Restore only JSONL test data (when cmrtdb compilation is not needed)**
```bash
cd mrtdb/testdata
for f in agg0424.jsonl all_targets.jsonl agg0421.jsonl agg0310.jsonl; do
    cat ${f}.part* > ${f}
done
```

### 验证还原完整性 / Verify Restoration Integrity

```bash
head -1 mrtdb/testdata/agg0421.jsonl | python3 -m json.tool   # JSON 是否合法 / Check if JSON is valid
ls -lh mrtdb/mrt_data/prefix_key.bin   # 应为 ~175 MB / Should be ~175 MB
ls -lh mrtdb/mrt_data/rib4.bz2          # 应为 ~103 MB / Should be ~103 MB
wc -l mrtdb/mrt_data/prefix.out         # BGP 前缀数量 / BGP prefix count
```

### 磁盘空间需求 / Disk Space Requirements

| 目录 | 需要还原的总大小 |
| Directory | Total Size to Restore |
|------|---------------|
| `mrtdb/mrt_data/` | ~3.7 GB |
| `mrtdb/testdata/` | ~807 MB |
| **合计 / Total** | **~4.5 GB** |

### 注意事项 / Notes

- 还原 `mrt_data/` 是**使用 cmrtdb 引擎的前提**——`MRTDB('./mrt_data')` 需要这些文件
  Restoring `mrt_data/` is **a prerequisite for using the cmrtdb engine** — `MRTDB('./mrt_data')` needs these files
- 还原 `testdata/` 的 JSONL 是**运行 `asprefix.py`、`mrtdb_test.py` 的前提**
  Restoring `testdata/` JSONL files is **a prerequisite for running `asprefix.py` and `mrtdb_test.py`**
- `.part` 文件在还原后会自动删除（`reassemble.sh` 的行为）
  `.part` files are automatically deleted after restoration (behavior of `reassemble.sh`)
- 如果仅需运行 Scapy 探测脚本（`pktest.py`、`gre_test.py` 等），不需要还原 `mrt_data/`
  If you only need to run Scapy probing scripts (`pktest.py`, `gre_test.py`, etc.), you do not need to restore `mrt_data/`
