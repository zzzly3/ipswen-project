from scapy.all import *
import sys

a = IP(dst=sys.argv[1])/TCP(options=[('MSS', 1460)])
b = sr1(a)
for i in b['TCP'].options:
    if i[0] == 'MSS':
        print('MSS:', i[1])
        break