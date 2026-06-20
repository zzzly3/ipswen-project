#!/usr/bin/env python3
import re
import sys
import math
from collections import defaultdict

logfile = sys.argv[1] if len(sys.argv) > 1 else '/home/net/trace.log'
pat = [
    re.compile(r'ingress_ifindex\s*(\d+)\D+redirect_ifindex\s*\d+\D+processing_time_ns\s*(\d+)'),
    re.compile(r'64 bytes from ([0-9a-fA-F:.]+): icmp_seq=\d+ ttl=63 time=([0-9.]+) ms'),
    re.compile(r'\[\s+\d+\]\s+[0-9.-]+\s+(sec)\s+\d+ MBytes\s+([0-9.]+) Gbits/sec'),
    re.compile(r'([0-9.]+): bpf_trace_printk: ([012a]+ [012a]+) ->')
]

data = defaultdict(list)
try:
    with open(logfile,'r') as f:
        for line in f:
            m = None
            for p in pat:
                m = p.search(line)
                if m:
                    break
            if not m:
                continue
            try:
                key = m.group(1)
                ns = float(m.group(2))
            except ValueError:
                ns = float(m.group(1))
                key = m.group(2)
            data[key].append(ns)
except FileNotFoundError:
    print('log file not found:', logfile)
    sys.exit(2)

if len(data) == 2:
    key1, key2 = list(data.keys())
    if len(data[key1]) == len(data[key2]):
        data['diff'] = [x-y for x,y in zip(data[key2], data[key1])]

if not data:
    print('no entries parsed')
    sys.exit(0)

def median(lst):
    s = sorted(lst)
    n = len(s)
    mid = n//2
    if n%2==1:
        return s[mid]
    return (s[mid-1]+s[mid])/2

def pctile(lst, p):
    if not lst:
        return None
    s = sorted(lst)
    n = len(s)
    idx = int(math.ceil(p/100.0 * n)) - 1
    idx = max(0, min(idx, n-1))
    return s[idx]

def stddev(lst):
    if not lst:
        return None
    n = len(lst)
    mean = sum(lst)/n
    var = sum((x - mean) ** 2 for x in lst) / n
    return math.sqrt(var)

print('direction, count, avg_ns, median_ns, p90_ns, stddev_ns')
for k in sorted(data.keys()):
    lst = data[k]
    cnt = len(lst)
    avg = sum(lst)/cnt
    med = median(lst)
    p90 = pctile(lst,90)
    std = stddev(lst)
    print(f"{k}, {cnt}, {avg}, {med}, {p90}, {std}")
