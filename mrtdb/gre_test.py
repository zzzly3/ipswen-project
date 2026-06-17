#!/usr/bin/env python3
from scapy.all import *
import argparse
import time
import os
import sys

IP_OPTIONS = {
    "none": b"",
    "nop": b"\x01",
    "rr": bytes([7, 3 + 9 * 4, 4] + [0] * 9 * 4),
    "unkctl3": bytes([156, 3, 233]),
    "unkres3": bytes([252, 3, 233]),
    "unkctl40": bytes([157, 40] + [233] * 38),
    "unkres40": bytes([253, 40] + [233] * 38),
}


def pkt_desc(pkt):
    if IP not in pkt:
        return pkt.summary()

    # 外层 ICMP，比如 TTL exceeded
    if ICMP in pkt and pkt[ICMP].type == 11:
        return f"outer ICMP time-exceeded from {pkt[IP].src}"

    if ICMP in pkt and pkt[ICMP].type == 3:
        return f"outer ICMP unreachable from {pkt[IP].src}, code={pkt[ICMP].code}"

    # GRE 外层包
    if GRE in pkt:
        return f"GRE packet outer {pkt[IP].src} -> {pkt[IP].dst}: {pkt.summary()}"

    # 隧道解封装后的内层 ICMP
    if ICMP in pkt:
        icmp = pkt[ICMP]
        if icmp.type == 0:
            return f"inner ICMP echo-reply {pkt[IP].src} -> {pkt[IP].dst}"
        if icmp.type == 8:
            return f"inner ICMP echo-request {pkt[IP].src} -> {pkt[IP].dst}"
        return f"inner/normal ICMP type={icmp.type}, code={icmp.code}, {pkt[IP].src}->{pkt[IP].dst}"

    return pkt.summary()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dst", required=True, help="outer GRE peer public IP")
    parser.add_argument("--outer-src", default=None, help="optional outer source IP, e.g. 172.21.16.8")
    parser.add_argument("--inner-src", required=True, help="inner source IP, e.g. 192.168.1.254")
    parser.add_argument("--inner-dst", required=True, help="inner destination IP, e.g. 192.168.1.255")
    parser.add_argument("--iface-outer", default="eth0")
    parser.add_argument("--iface-tun", required=True, help="GRE tunnel interface, e.g. tun-2f7f4d")
    parser.add_argument("--option", default="none", choices=list(IP_OPTIONS.keys()))
    parser.add_argument("--ttl", type=int, default=64)
    parser.add_argument("--count", type=int, default=3)
    parser.add_argument("--timeout", type=float, default=5)
    args = parser.parse_args()

    if os.geteuid() != 0:
        print("Please run with sudo.", file=sys.stderr)
        sys.exit(1)

    option_bytes = IP_OPTIONS[args.option]

    # 同时监听 eth0 和 tunnel 接口
    sniffer = AsyncSniffer(
        iface=[args.iface_outer, args.iface_tun],
        filter="icmp or proto 47",
        store=True,
    )

    print(f"Start sniffing on {args.iface_outer} and {args.iface_tun} ...")
    sniffer.start()
    time.sleep(0.5)

    packets = []
    for i in range(args.count):
        outer = IP(dst=args.dst, options=option_bytes, ttl=args.ttl)
        if args.outer_src:
            outer.src = args.outer_src

        pkt = (
            outer
            / GRE()
            / IP(src=args.inner_src, dst=args.inner_dst)
            / ICMP(id=0x4321, seq=i + 1)
        )
        packets.append(pkt)

    print(f"Sending {len(packets)} GRE packets: option={args.option}, ttl={args.ttl}")
    send(packets, verbose=False)

    time.sleep(args.timeout)
    captured = sniffer.stop()

    print("\nCaptured packets:")
    matched = 0
    for pkt in captured:
        s = pkt_desc(pkt)

        # 只打印明显相关的包
        text = pkt.summary()
        if (
            args.dst in text
            or args.inner_src in text
            or args.inner_dst in text
            or "GRE" in text
            or "ICMP" in text
        ):
            matched += 1
            print(f"- {s}")

    print(f"\nMatched/captured related packets: {matched}")


if __name__ == "__main__":
    main()