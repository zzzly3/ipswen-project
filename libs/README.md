# libs —— 第三方库（BGPstream & wandio） / Third-Party Libraries (BGPstream & wandio)

## 概述 / Overview

本目录包含两个第三方 C 语言开源库——**BGPstream 2.2.0**（CAIDA 的 BGP 数据采集框架）和 **wandio 4.2.4**（WAND 的 I/O 抽象库）——以及它们原始源码的压缩包。这两个库是 BGP 数据研究流水线的**上游依赖**，为 `../mrtdb/` 中的路由数据处理提供数据获取和透明解压能力。

This directory contains two third-party C language open-source libraries — **BGPstream 2.2.0** (CAIDA's BGP data collection framework) and **wandio 4.2.4** (WAND's I/O abstraction library) — along with their original source code archives. These two libraries are **upstream dependencies** of the BGP data research pipeline, providing data acquisition and transparent decompression capabilities for routing data processing in `../mrtdb/`.

两个库均为**未经修改的原始上游版本**，编译产物已在本地保留。

Both libraries are **unmodified original upstream versions**, with build artifacts retained locally.

---

## 目录结构 / Directory Structure

```
libs/
├── libbgpstream-2.2.0/           # BGPstream（含构建产物）/ with build artifacts
│   ├── lib/          # libbgpstream 核心: MRT 读取、过滤、流式处理 / Core: MRT reading, filtering, streaming
│   ├── tools/        # CLI: bgpstream, bgpstream-json etc.
│   ├── common/       # 跨平台公共函数 / Cross-platform common functions
│   ├── test/         # 单元测试和集成测试 / Unit and integration tests
│   ├── docs/         # 文档 / Documentation
│   ├── m4/           # Autoconf 宏 / Autoconf macros
│   ├── configure / configure.ac / Makefile.am / Makefile.in
│   ├── Makefile / config.h / config.log / config.status  # 构建产物 / Build artifacts
│   └── AUTHORS / LICENSE / ChangeLog / README
│
├── wandio-4.2.4-1/               # wandio（含构建产物）/ with build artifacts
│   ├── lib/          # libwandio 核心: 透明压缩读写 / Core: transparent compressed I/O
│   ├── tools/        # wandio CLI 转换工具 / wandio CLI conversion tools
│   ├── test/         # 测试 / Tests
│   ├── m4/           # Autoconf 宏 / Autoconf macros
│   ├── debian/       # Debian 打包脚本 / Debian packaging scripts
│   ├── rpm/          # RPM 打包脚本 / RPM packaging scripts
│   ├── autom4te.cache/  # Autoconf 缓存 / Autoconf cache
│   ├── configure / configure.ac / Makefile.am / Makefile.in
│   ├── Makefile / config.h / config.log / config.status
│   └── COPYING / AUTHORS / README
│
├── libbgpstream-2.2.0.tar.gz     # 原始源码包 / Original source archive
└── 4.2.4-1.tar.gz                # wandio 原始源码包 / wandio original source archive
```

---

## libbgpstream-2.2.0 —— BGP 监测框架 / BGP Monitoring Framework

### 概述 / Overview

BGPstream 是 CAIDA（Center for Applied Internet Data Analysis，互联网应用数据分析中心）开发的 C 语言开源框架。它通过一套统一的 API 屏蔽了不同 BGP 数据源的差异，让研究人员只需写一次代码就能从 Route Views（俄勒冈大学）、RIPE RIS（欧洲）、BGPmon 等采集器获取实时和历史 BGP 数据。

BGPstream is an open-source C framework developed by CAIDA (Center for Applied Internet Data Analysis). It abstracts away the differences between various BGP data sources through a unified API, allowing researchers to write code once and retrieve real-time and historical BGP data from collectors such as Route Views (University of Oregon), RIPE RIS (Europe), and BGPmon.

### 关键能力 / Key Capabilities

- **数据源抽象：** 一套 API 同时支持 Route Views、RIPE RIS、BGPmon、OpenBMP / One API supporting Route Views, RIPE RIS, BGPmon, OpenBMP
- **实时和历史：** `bgpstream_create()` 可选择实时流或历史回放模式 / Selectable live stream or historical replay mode
- **记录过滤：** 按 IP 前缀、ASN、对等体 ASN、采集器名称、记录类型过滤 / Filter by IP prefix, ASN, peer ASN, collector name, record type
- **MRT 格式：** 读取标准 MRT TABLE_DUMP_V2 和 UPDATE 消息 / Read standard MRT TABLE_DUMP_V2 and UPDATE messages
- **内存管理：** 内置 `bgpstream_record_*` API 管理记录生命周期 / Built-in `bgpstream_record_*` API for record lifecycle management

### 在流水线中的角色 / Role in the Pipeline

```
[Route Views / RIPE RIS 服务器 / Servers]
         │
         ▼
  libbgpstream ← 通过 bgpstream API 获取实时/历史数据 / Fetch live/historical data via bgpstream API
         │
         ▼
  MRT .bz2 文件 ← 保存到本地磁盘 / Save to local disk
         │
         ▼
  wandio ← 透明解压 .bz2 / Transparent decompression of .bz2
         │
         ▼
  bgpdump (在 ../mrtdb/libbgpdump-1.6.0/) ← 解析 MRT 二进制 / Parse MRT binary
         │
         ▼
  cmrtdb / mrtdb_user ← 索引和查询 / Indexing and querying
```

## wandio-4.2.4-1 —— I/O 抽象库 / I/O Abstraction Library

### 概述 / Overview

wandio 是新西兰怀卡托大学 WAND 研究组开发的高性能 I/O 抽象库。它在一个接口下统一了原始文件和各种压缩格式的读写，使得上层应用不需要关心数据是 `.gz`、`.bz2` 还是 raw。

wandio is a high-performance I/O abstraction library developed by the WAND research group at the University of Waikato, New Zealand. It unifies reading and writing of raw files and various compressed formats under a single interface, allowing upper-layer applications to not care whether data is `.gz`, `.bz2`, or raw.

### 关键能力 / Key Capabilities

- **格式透明：** `wandio_create()` 自动检测 magic 字节，选择正确的解压器 / Auto-detects magic bytes, selects the correct decompressor
- **多格式支持：** raw、gzip、bzip2、LZO、LZMA (xz)、ZSTD
- **线程安全：** 支持多线程并发读取 / Supports multi-threaded concurrent reading
- **零拷贝：** peak/write 接口减少内存复制 / peak/write interfaces reduce memory copying
- **高吞吐：** 专为大规模网络数据处理优化（数百 MB/s 级）/ Optimized for large-scale network data processing (hundreds of MB/s)

### 在流水线中的角色 / Role in the Pipeline

wandio 最直接的应用是透明读取 `.bz2` 压缩的 MRT 转储文件（如 `../mrtdb/mrt_data/rib4.bz2`），无需预先手动解压就能直接解析。

The most direct application of wandio is transparently reading `.bz2` compressed MRT dump files (e.g., `../mrtdb/mrt_data/rib4.bz2`), allowing direct parsing without prior manual decompression.

---

## 编译 / Compilation

### wandio（先编译——因为 bgpstream 依赖它）/ Build wandio first (bgpstream depends on it)

```bash
cd wandio-4.2.4-1

# 安装依赖 / Install dependencies
sudo apt install -y zlib1g-dev libbz2-dev liblzma-dev liblzo2-dev libzstd-dev

# 生成 configure（如果修改了 configure.ac）/ Regenerate configure (if configure.ac was modified)
./bootstrap.sh

# 配置、编译、安装 / Configure, build, install
./configure --prefix=/usr/local
make -j$(nproc)
sudo make install

# 验证 / Verify
ldconfig -p | grep libwandio
```

### libbgpstream / Build libbgpstream

```bash
cd libbgpstream-2.2.0

# 依赖 / Dependencies
sudo apt install -y libcurl4-openssl-dev librdkafka-dev

# 重新生成 configure（如果需要）/ Regenerate configure (if needed)
autoreconf -i

# 配置（确保能找到 wandio）/ Configure (ensure wandio is found)
./configure --prefix=/usr/local --with-wandio=/usr/local

# 编译、安装 / Build and install
make -j$(nproc)
sudo make install

# 验证 / Verify
ldconfig -p | grep libbgpstream
```

### 常见编译问题 / Common Build Issues

| 问题 / Issue | 原因 / Cause | 解决 / Solution |
|------|------|------|
| `configure: error: wandio not found` | wandio 未安装或路径不对 / wandio not installed or wrong path | `sudo make install` 确保 libwandio 在 `/usr/local/lib` |
| `libcurl not found` | libcurl 开发包缺失 / libcurl dev package missing | `sudo apt install libcurl4-openssl-dev` |
| `lzma.h: No such file` | xz 开发包缺失 / xz dev package missing | `sudo apt install liblzma-dev` |
| `zstd.h: No such file` | ZSTD 开发包缺失 / ZSTD dev package missing | `sudo apt install libzstd-dev` |
| `bzlib.h: No such file` | bzip2 开发包缺失 / bzip2 dev package missing | `sudo apt install libbz2-dev` |

---

## 原始源码包 / Original Source Archives

- `libbgpstream-2.2.0.tar.gz` —— BGPstream 2.2.0 官方发布包 / Official release archive
- `4.2.4-1.tar.gz` —— wandio 4.2.4 官方发布包 / Official release archive

这些包是完整的源代码压缩包，包含了 `configure.ac`、`Makefile.am` 等未生成的文件——与当前目录中的构建树互补。

These archives are complete source code packages containing ungenerated files like `configure.ac`, `Makefile.am`, etc. — complementary to the build tree in the current directory.

---

## 注意事项 / Notes

- **预构建状态：** 两个库均已在本目录中编译过。`.o` 文件、`config.status`、`config.log` 等构建产物均保留。/ Both libraries have been compiled in this directory; build artifacts (.o, config.status, config.log) are retained.
- **未经修改：** 与 `bird/`、`iproute2/` 等不同，这两个库没有 IPswen 相关修改。/ Unlike `bird/`, `iproute2/`, etc., these libraries have no IPswen-related modifications.
- **系统级安装：** 如果通过 `make install` 安装到系统，本目录中的副本可以作为备份。/ If installed system-wide via `make install`, the copies here serve as backup.
- **ABI 兼容性：** wandio 和 bgpstream 有版本依赖关系——bgpstream 2.2.0 需要 wandio 4.x。/ wandio and bgpstream have version dependencies — bgpstream 2.2.0 requires wandio 4.x.

---

## 相关项目 / Related Projects

- **mrtdb** (`../mrtdb/`) —— 主要消费者，使用这些库进行 BGP 数据处理 / Primary consumer, uses these libraries for BGP data processing
- **mrtdb/libbgpdump-1.6.0/** —— 另一个 MRT 格式解析库（互补关系：bgpdump 做解析，bgpstream 做数据采集）/ Another MRT format parsing library (complementary: bgpdump for parsing, bgpstream for data collection)
- **mrtdb/mrt_data/** —— MRT 转储文件 / MRT dump files

---

## 附录 A: 库版本依赖关系 / Library Version Dependencies

```
libbgpstream 2.2.0
├── wandio >= 4.0
├── libcurl >= 7.0
└── (可选 / Optional) librdkafka >= 0.11

wandio 4.2.4-1
├── zlib
├── libbz2
├── liblzma
├── liblzo2
└── libzstd
```

## 附录 B: 常用 API / Common API

### wandio
```c
io_t *wandio_create(const char *filename);
off_t wandio_read(io_t *io, void *buffer, off_t len);
void wandio_destroy(io_t *io);
```

### libbgpstream  
```c
bgpstream_t *bs = bgpstream_create();
bgpstream_add_filter(bs, BGPSTREAM_FILTER_TYPE_PREFIX, "192.0.0.0/8");
bgpstream_start(bs);
while (bgpstream_get_next_record(bs, &rec) > 0) {
    // process rec
}
bgpstream_destroy(bs);
```

## 附录 C: 文件大小参考 / File Size Reference

| 文件 | 大小 | 说明 |
|------|------|------|
| `libbgpstream-2.2.0.tar.gz` | ~800 KB | 源码包 |
| `4.2.4-1.tar.gz` | ~400 KB | wandio 源码包 |
| `libbgpstream.so` (编译后) | ~1.5 MB | 动态库 |
| `libwandio.so` (编译后) | ~200 KB | 动态库 |
| 构建目录 (`libbgpstream/`) | ~50 MB | 含 .o 文件 |（这些库的生产者和消费者）
