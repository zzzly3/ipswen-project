#!/bin/bash
# ============================================================
# IPSWEN Project — 大文件分块还原脚本
# 使用方法: bash reassemble.sh
# 将所有 .part* 分块文件合并还原为原始文件
# ============================================================
set -e
echo "开始还原大文件..."

# --- bird/configs/ (9 个配置文件，2 parts 每个) ---
echo "[1/4] 还原 bird/configs/ ..."
for f in \
    node1_IPswen_level0_1000000.conf node1_IPswen_level1_1000000.conf \
    node1_IPswen_level2_1000000.conf node1_IPswen_level3_1000000.conf \
    node1_IPswen_level4_1000000.conf node1_IPswen_level5_1000000.conf \
    node1_IPswen_level6_1000000.conf node1_IPswen_level7_1000000.conf \
    node1_IPv4_1000000.conf; do
    cat bird/configs/${f}.part* > bird/configs/${f}
    rm bird/configs/${f}.part*
    echo "  ✓ ${f} ($(du -h bird/configs/${f} | cut -f1))"
done

# --- mrtdb/mrt_data/ (8 个文件，2-17 parts) ---
echo "[2/4] 还原 mrtdb/mrt_data/ ..."
for f in prefix.out prefix_value.bin prefix_key.bin as_path.out as.out \
         as_value.bin as_path.bin rib4.bz2; do
    cat mrtdb/mrt_data/${f}.part* > mrtdb/mrt_data/${f}
    rm mrtdb/mrt_data/${f}.part*
    echo "  ✓ ${f} ($(du -h mrtdb/mrt_data/${f} | cut -f1))"
done

# --- mrtdb/testdata/ (4 个文件，2-4 parts) ---
echo "[3/4] 还原 mrtdb/testdata/ ..."
for f in agg0424.jsonl all_targets.jsonl agg0421.jsonl agg0310.jsonl; do
    cat mrtdb/testdata/${f}.part* > mrtdb/testdata/${f}
    rm mrtdb/testdata/${f}.part*
    echo "  ✓ ${f} ($(du -h mrtdb/testdata/${f} | cut -f1))"
done

# --- linux-kernel/ (1 个文件，2 parts) ---
echo "[4/4] 还原 linux-kernel/ ..."
f=linux-src-5.4.0-ipswen-v1-20240124.tar.gz
cat linux-kernel/${f}.part* > linux-kernel/${f}
rm linux-kernel/${f}.part*
echo "  ✓ ${f} ($(du -h linux-kernel/${f} | cut -f1))"

echo ""
echo "=== 全部还原完成 ==="
