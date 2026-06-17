#!/usr/bin/env python3
import socket
from scapy.all import *
from mrtdb.pktest import *

conf.check_TCPerror_seqack = True

none_opt = b''
rr_opt = bytes([7, 3+9*4, 4] + [0] * 9*4)
nop_opt = b'\1'
unkctl3_opt = bytes([156, 3, 233])
unkres3_opt = bytes([252, 3, 233])
unkctl40_opt = bytes([157, 40] + [233] * 38)
unkres40_opt = bytes([253, 40] + [233] * 38)

dst="43.131.31.97"
ip_options = unkctl3_opt

# Construct a TCP traceroute packet
# packet = IP(dst=dst, options=ip_options)/TCP(dport=80, flags='S')
# packet = IP(dst=dst, options=ip_options, ttl=(1,30))/TCP()
# packet = IP(dst=dst, options=ip_options, ttl=(1,30))/UDP()
packet = IP(dst=dst, options=ip_options, ttl=(1,30))/ICMP()
#packet = [IP(dst=dst, options=ip_options, ttl=i)/GRE()/IP(dst='192.168.1.2', src='192.168.1.1')/ICMP(id=i, seq=1000-i) for i in range(29, 30)]
#packet += [IP(dst=dst, options=ip_options, ttl=i)/ICMP(id=i, seq=1000-i) for i in range(29,30)]
#packet += [IP(#dst=dst, options=ip_options, ttl=i)/TCP(seq=i,flags="S") for i in range(29,30)]
# r1 = TestRunner()
# r1.add_tests(TestGroupTTL.make_groups(['157.130.228.150', '12.55.112.242'], 'icmp', 'all', ttl_min=5, ttl_max=30))
# packet = r1.prepare_packets(500)

# Send the packet
a,b = sr(packet, filter="ip", timeout=2)

for snd, rev in a:
    print(snd[IP].ttl, rev[IP].src)
