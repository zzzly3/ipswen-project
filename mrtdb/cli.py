#!/usr/bin/env python3
import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_OPTIONS, bytes([136, 4, 222, 222]))
sock.sendto(b'Hello, world!', ('opsiano.tech', 4489))
