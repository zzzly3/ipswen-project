#!/usr/bin/env python3
from scapy.all import *
import argparse
import csv
import time
import os
import sys
from collections import defaultdict

conf.check_TCPerror_seqack = True

IP_OPTIONS = {
    "none": b"",
    "nop": b"\x01",
    "rr": bytes([7, 3 + 9 * 4, 4] + [0] * 9 * 4),
    "unkctl3": bytes([156, 3, 233]),
    "unkres3": bytes([252, 3, 233]),
    "unkctl40": bytes([157, 40] + [233] * 38),
    "unkres40": bytes([253, 40] + [233] * 38),
}


def parse_list(s, all_items):
    if s == "all":
        return list(all_items)
    items = [x.strip() for x in s.split(",") if x.strip()]
    bad = [x for x in items if x not in all_items]
    if bad:
        raise ValueError("Unknown items: {}".format(",".join(bad)))
    return items


def make_packets(dst, option_bytes, option_name, mode, ttl_min, ttl_max, repeat,
                 tcp_port, udp_base_port, sport_base, inner_src, inner_dst):
    packets = []
    meta = {}

    for r in range(repeat):
        for ttl in range(ttl_min, ttl_max + 1):
            tag = r * 1000 + ttl

            if mode == "icmp":
                pkt = (
                    IP(dst=dst, options=option_bytes, ttl=ttl)
                    / ICMP(id=(10000 + tag) & 0xffff, seq=ttl)
                )

            elif mode == "tcp80":
                pkt = (
                    IP(dst=dst, options=option_bytes, ttl=ttl)
                    / TCP(
                        sport=sport_base + tag,
                        dport=80,
                        flags="S",
                        seq=100000 + tag,
                    )
                )

            elif mode == "tcp443":
                pkt = (
                    IP(dst=dst, options=option_bytes, ttl=ttl)
                    / TCP(
                        sport=sport_base + tag,
                        dport=443,
                        flags="S",
                        seq=200000 + tag,
                    )
                )

            elif mode == "tcp":
                pkt = (
                    IP(dst=dst, options=option_bytes, ttl=ttl)
                    / TCP(
                        sport=sport_base + tag,
                        dport=tcp_port,
                        flags="S",
                        seq=300000 + tag,
                    )
                )

            elif mode == "udp":
                pkt = (
                    IP(dst=dst, options=option_bytes, ttl=ttl)
                    / UDP(
                        sport=sport_base + tag,
                        dport=udp_base_port + ttl + r * 100,
                    )
                )

            elif mode == "gre":
                pkt = (
                    IP(dst=dst, options=option_bytes, ttl=ttl)
                    / GRE()
                    / IP(src=inner_src, dst=inner_dst)
                    / ICMP(id=(20000 + tag) & 0xffff, seq=ttl)
                )

            else:
                raise ValueError("Unknown mode: {}".format(mode))

            packets.append(pkt)
            meta[id(pkt)] = {
                "option": option_name,
                "mode": mode,
                "ttl": ttl,
                "repeat": r,
            }

    return packets, meta


def classify_reply(reply):
    """
    Return: reply_type, detail
    """
    if IP not in reply:
        return "non-ip", reply.summary()

    if ICMP in reply:
        icmp = reply[ICMP]

        if icmp.type == 0:
            return "icmp-echo-reply", "target replied"

        if icmp.type == 3:
            code_map = {
                0: "network-unreachable",
                1: "host-unreachable",
                2: "protocol-unreachable",
                3: "port-unreachable",
                4: "fragmentation-needed",
                9: "network-admin-prohibited",
                10: "host-admin-prohibited",
                13: "communication-admin-prohibited",
            }
            return "icmp-destination-unreachable", code_map.get(icmp.code, "code={}".format(icmp.code))

        if icmp.type == 11:
            return "icmp-time-exceeded", "ttl expired"

        if icmp.type == 12:
            return "icmp-parameter-problem", "bad ip header or option"

        return "icmp-other", "type={}, code={}".format(icmp.type, icmp.code)

    if TCP in reply:
        flags = reply[TCP].flags
        flags_str = str(flags)

        if "S" in flags_str and "A" in flags_str:
            return "tcp-synack", "port open or reachable"

        if "R" in flags_str:
            return "tcp-rst", "port closed or rejected"

        return "tcp-other", "flags={}".format(flags_str)

    return "other-ip", reply.summary()


def run_one_test(args, option_name, option_bytes, mode):
    packets, meta = make_packets(
        dst=args.dst,
        option_bytes=option_bytes,
        option_name=option_name,
        mode=mode,
        ttl_min=args.min_ttl,
        ttl_max=args.max_ttl,
        repeat=args.repeat,
        tcp_port=args.tcp_port,
        udp_base_port=args.udp_base_port,
        sport_base=args.sport_base,
        inner_src=args.inner_src,
        inner_dst=args.inner_dst,
    )

    print("\n=== Test: option={}, mode={}, packets={} ===".format(
        option_name, mode, len(packets)
    ))

    answered, unanswered = sr(
        packets,
        filter=args.bpf_filter,
        timeout=args.timeout,
        retry=0,
        verbose=args.verbose,
    )

    rows = []
    answered_ids = set()

    for sent, reply in answered:
        m = meta.get(id(sent), {})
        answered_ids.add(id(sent))

        reply_type, detail = classify_reply(reply)

        row = {
            "option": m.get("option", option_name),
            "mode": m.get("mode", mode),
            "repeat": m.get("repeat", ""),
            "ttl": m.get("ttl", sent[IP].ttl if IP in sent else ""),
            "answered": 1,
            "reply_src": reply[IP].src if IP in reply else "",
            "reply_dst": reply[IP].dst if IP in reply else "",
            "reply_type": reply_type,
            "detail": detail,
            "sent_summary": sent.summary(),
            "reply_summary": reply.summary(),
        }
        rows.append(row)

    for sent in unanswered:
        m = meta.get(id(sent), {})

        row = {
            "option": m.get("option", option_name),
            "mode": m.get("mode", mode),
            "repeat": m.get("repeat", ""),
            "ttl": m.get("ttl", sent[IP].ttl if IP in sent else ""),
            "answered": 0,
            "reply_src": "",
            "reply_dst": "",
            "reply_type": "no-reply",
            "detail": "timeout",
            "sent_summary": sent.summary(),
            "reply_summary": "",
        }
        rows.append(row)

    rows.sort(key=lambda x: (int(x["repeat"]), int(x["ttl"]), x["reply_type"]))

    total = len(rows)
    got = sum(1 for r in rows if r["answered"] == 1)

    print("Result: answered={}/{} unanswered={}".format(got, total, total - got))

    # 简洁打印每个 TTL 的结果
    for r in rows:
        if r["answered"] == 1:
            print(
                "TTL={:<2} reply_from={:<15} type={:<30} detail={}".format(
                    r["ttl"], r["reply_src"], r["reply_type"], r["detail"]
                )
            )

    return rows


def write_csv(path, rows):
    if not rows:
        return

    fields = [
        "option",
        "mode",
        "repeat",
        "ttl",
        "answered",
        "reply_src",
        "reply_dst",
        "reply_type",
        "detail",
        "sent_summary",
        "reply_summary",
    ]

    with open(path, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fields)
        writer.writeheader()
        for r in rows:
            writer.writerow(r)


def print_summary(all_rows):
    groups = defaultdict(list)
    for r in all_rows:
        groups[(r["option"], r["mode"])].append(r)

    print("\n================ Summary ================")
    print("{:<10} {:<8} {:>8} {:>8} {:>10}".format(
        "option", "mode", "sent", "reply", "reply_rate"
    ))

    for key in sorted(groups.keys()):
        option, mode = key
        rows = groups[key]
        sent = len(rows)
        reply = sum(1 for r in rows if r["answered"] == 1)
        rate = reply / sent if sent else 0

        print("{:<10} {:<8} {:>8} {:>8} {:>9.1f}%".format(
            option, mode, sent, reply, rate * 100
        ))


def main():
    parser = argparse.ArgumentParser(
        description="Automatic IP Options reachability test with ICMP/TCP/UDP/GRE."
    )

    parser.add_argument("--dst", required=True, help="destination IP or hostname")
    parser.add_argument("--min-ttl", type=int, default=1)
    parser.add_argument("--max-ttl", type=int, default=30)
    parser.add_argument("--timeout", type=float, default=5.0)
    parser.add_argument("--repeat", type=int, default=1)

    parser.add_argument(
        "--options",
        default="none,unkctl3,unkres3,unkctl40,unkres40",
        help="comma-separated options: none,nop,rr,unkctl3,unkres3,unkctl40,unkres40 or all",
    )

    parser.add_argument(
        "--modes",
        default="icmp,tcp80,tcp443,udp,gre",
        help="comma-separated modes: icmp,tcp80,tcp443,tcp,udp,gre",
    )

    parser.add_argument("--tcp-port", type=int, default=80)
    parser.add_argument("--udp-base-port", type=int, default=33434)
    parser.add_argument("--sport-base", type=int, default=40000)

    parser.add_argument("--inner-src", default="192.168.1.1")
    parser.add_argument("--inner-dst", default="192.168.1.2")

    parser.add_argument("--csv", default="ip_options_result.csv")
    parser.add_argument("--gap", type=float, default=0.5, help="sleep seconds between test groups")

    parser.add_argument(
        "--bpf-filter",
        default="icmp or tcp",
        help="BPF filter for sniffing replies. Default: 'icmp or tcp'",
    )

    parser.add_argument("--verbose", action="store_true")

    args = parser.parse_args()

    if os.geteuid() != 0:
        print("Warning: Scapy raw packets usually require root. Please run with sudo.", file=sys.stderr)

    option_names = parse_list(args.options, IP_OPTIONS.keys())
    mode_names = parse_list(args.modes, ["icmp", "tcp80", "tcp443", "tcp", "udp", "gre"])

    all_rows = []

    for option_name in option_names:
        option_bytes = IP_OPTIONS[option_name]

        for mode in mode_names:
            rows = run_one_test(args, option_name, option_bytes, mode)
            all_rows.extend(rows)
            time.sleep(args.gap)

    write_csv(args.csv, all_rows)
    print_summary(all_rows)

    print("\nCSV saved to: {}".format(args.csv))


if __name__ == "__main__":
    main()