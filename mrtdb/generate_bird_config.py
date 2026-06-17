#!/usr/bin/env python3
"""Generate a BIRD config with log/static/bgp only.

The tool reads neighbor/local settings from an existing bird.conf, then builds a
new config with one BGP session and generated static prefixes.
"""

from __future__ import annotations

import argparse
import ipaddress
import re
import sys
from pathlib import Path
from typing import Iterator, Optional, Tuple


def parse_family(value: str) -> str:
    normalized = value.strip().lower()
    if normalized in {"4", "v4", "ipv4"}:
        return "ipv4"
    if normalized in {"6", "v6", "ipv6"}:
        return "ipv6"
    if normalized in {"ipswen", "ip-swen"}:
        return "ipswen"
    raise argparse.ArgumentTypeError("Address family must be one of: ipv4, ipv6, ipswen, 4, 6")


def strip_inline_comment(line: str) -> str:
    return line.split("#", 1)[0]


def extract_first_bgp_block(conf_text: str) -> str:
    in_bgp = False
    depth = 0
    collected = []

    for raw in conf_text.splitlines():
        line = strip_inline_comment(raw)
        if not in_bgp:
            if re.search(r"\bprotocol\s+bgp\b", line):
                in_bgp = True
                collected.append(line)
                depth += line.count("{") - line.count("}")
                if depth <= 0:
                    break
        else:
            collected.append(line)
            depth += line.count("{") - line.count("}")
            if depth <= 0:
                break

    if not collected or depth > 0:
        raise ValueError("Could not find a complete active 'protocol bgp { ... }' block")

    return "\n".join(collected)


def extract_neighbor_local(conf_text: str) -> Tuple[str, str]:
    bgp_block = extract_first_bgp_block(conf_text)

    neighbor_match = re.search(r"\bneighbor\s+(.+?)\s+as\s+(\d+)\s*;", bgp_block)
    if not neighbor_match:
        raise ValueError("Could not find a valid 'neighbor ... as ...;' line in the first BGP block")
    neighbor_clause = f"{neighbor_match.group(1).strip()} as {neighbor_match.group(2)}"

    local_match = re.search(r"\blocal\s+(.+?)\s+as\s+(\d+)\s*;", bgp_block)
    if local_match:
        local_clause = f"{local_match.group(1).strip()} as {local_match.group(2)}"
    else:
        local_as_only = re.search(r"\blocal\s+as\s+(\d+)\s*;", bgp_block)
        if not local_as_only:
            raise ValueError("Could not find a valid 'local ... as ...;' line in the first BGP block")
        local_clause = f"as {local_as_only.group(1)}"

    return neighbor_clause, local_clause


def format_ipswen_prefix(network_int: int, prefix_len: int, level: int) -> str:
    total_bytes = 4 + level
    octets = []
    for idx in range(total_bytes):
        shift = 8 * (total_bytes - 1 - idx)
        octets.append((network_int >> shift) & 0xFF)

    ipv4_part = ".".join(str(x) for x in octets[:4])
    if level == 0:
        return f"{ipv4_part}(0)/{prefix_len}"

    extension = ".".join(str(x) for x in octets[4:])
    return f"{ipv4_part}({level}){extension}/{prefix_len}"


def max_ipswen_prefixes_with_nonzero_first_octet(prefix_len: int) -> int:
    if prefix_len <= 0:
        return 0
    if prefix_len < 8:
        return (1 << prefix_len) - 1
    return (1 << prefix_len) - (1 << (prefix_len - 8))


def generate_ipswen_prefixes(prefix_len: int, count: int, level: int) -> Iterator[str]:
    if level < 0:
        raise ValueError("IPswen level must be >= 0")

    total_bits = (4 + level) * 8
    if not (0 <= prefix_len <= total_bits):
        raise ValueError(f"Prefix length must be in [0, {total_bits}] for ipswen level {level}")
    if count <= 0:
        raise ValueError("Count must be greater than 0")

    max_unique_prefixes = max_ipswen_prefixes_with_nonzero_first_octet(prefix_len)
    if count > max_unique_prefixes:
        raise ValueError(
            f"Cannot generate {count} unique ipswen prefixes of /{prefix_len} with non-zero first octet; "
            f"max is {max_unique_prefixes}"
        )

    step = 1 << (total_bits - prefix_len)
    start_index = 1 if prefix_len < 8 else (1 << (prefix_len - 8))

    for offset in range(count):
        network_index = start_index + offset
        network_int = network_index * step
        yield format_ipswen_prefix(network_int, prefix_len, level)


def generate_prefixes(
    family: str,
    prefix_len: int,
    count: int,
    ipswen_level: Optional[int] = None,
) -> Iterator[str]:
    if family == "ipswen":
        if ipswen_level is None:
            raise ValueError("ipswen_level is required when family is ipswen")
        yield from generate_ipswen_prefixes(prefix_len=prefix_len, count=count, level=ipswen_level)
        return

    bits = 32 if family == "ipv4" else 128
    if not (0 <= prefix_len <= bits):
        raise ValueError(f"Prefix length must be in [0, {bits}] for {family}")
    if count <= 0:
        raise ValueError("Count must be greater than 0")

    max_unique_prefixes = 1 << prefix_len
    if count > max_unique_prefixes:
        raise ValueError(
            f"Cannot generate {count} unique {family} prefixes of /{prefix_len}; "
            f"max is {max_unique_prefixes}"
        )

    step = 1 << (bits - prefix_len)
    for i in range(count):
        network_int = i * step + (1 << (bits-8))
        network = ipaddress.ip_network((network_int, prefix_len))
        yield str(network)


def render_bgp_channel(family: str) -> str:
    if family in {"ipv4", "ipswen"}:
        return (
            "    ipv4 {\n"
            "        import filter {\n"
            "            if (net ~ [192.168.0.0/16+]) then reject;\n"
            "            accept;\n"
            "        };\n"
            "        export filter {\n"
            "            if (net ~ [192.168.0.0/16+]) then reject;\n"
            "            accept;\n"
            "        };\n"
            "        next hop self;\n"
            "    };\n"
        )

    return (
        "    ipv6 {\n"
        "        import all;\n"
        "        export all;\n"
        "        next hop self;\n"
        "    };\n"
    )


def build_config(
    family: str,
    prefix_len: int,
    count: int,
    neighbor_clause: str,
    local_clause: str,
    ipswen_level: Optional[int] = None,
) -> str:
    lines = []
    lines.append('log "bird.log" all;')
    lines.append("router id 192.168.1.254;")
    lines.append("protocol static {")
    lines.append(f"    {family.replace('ipswen', 'ipv4')};")
    for prefix in generate_prefixes(family, prefix_len, count, ipswen_level=ipswen_level):
        lines.append(f"    route {prefix} blackhole;")
    lines.append("}")
    lines.append("")
    lines.append("protocol bgp {")
    lines.append(f"    neighbor {neighbor_clause};")
    lines.append(f"    local {local_clause};")
    lines.append("    multihop;")
    lines.append("    keepalive time 30;")
    lines.append(render_bgp_channel(family).rstrip("\n"))
    lines.append("}")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Generate a BIRD config with one BGP session and generated static prefixes"
    )
    parser.add_argument("--count", "-n", type=int, required=True, help="Number of prefixes to generate")
    parser.add_argument("--prefix-len", "-p", type=int, required=True, help="Prefix length")
    parser.add_argument(
        "--address-family",
        "--af",
        "-a",
        type=parse_family,
        required=True,
        help="Address family: ipv4, ipv6, or ipswen",
    )
    parser.add_argument(
        "--ipswen-level",
        type=int,
        default=None,
        help="IPswen extension level (byte count), required when --address-family=ipswen",
    )
    parser.add_argument(
        "--source-conf",
        "-s",
        default="bird.conf",
        help="Source BIRD config used to read existing neighbor/local settings",
    )
    parser.add_argument(
        "--output-conf",
        "-o",
        default="bird.generated.conf",
        help="Output file path for generated config",
    )

    args = parser.parse_args()

    if args.address_family == "ipswen":
        if args.ipswen_level is None:
            parser.error("--ipswen-level is required when --address-family is ipswen")
        if args.ipswen_level < 0:
            parser.error("--ipswen-level must be >= 0")

    source_path = Path(args.source_conf)
    output_path = Path(args.output_conf)

    try:
        conf_text = source_path.read_text(encoding="utf-8")
        neighbor_clause, local_clause = extract_neighbor_local(conf_text)
        new_conf = build_config(
            family=args.address_family,
            prefix_len=args.prefix_len,
            count=args.count,
            neighbor_clause=neighbor_clause,
            local_clause=local_clause,
            ipswen_level=args.ipswen_level,
        )
        output_path.write_text(new_conf, encoding="ascii")
    except (OSError, ValueError) as exc:
        print(f"Error: {exc}", file=sys.stderr)
        return 1

    ipswen_suffix = ""
    if args.address_family == "ipswen":
        ipswen_suffix = f" (level={args.ipswen_level})"

    print(
        f"Generated {output_path} with {args.count} {args.address_family} prefixes /{args.prefix_len}"
        f"{ipswen_suffix}. BGP neighbor/local copied from {source_path}."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
