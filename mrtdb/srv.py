#!/usr/bin/env python3
import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('', 179))
while True:
    data, addr = sock.recvfrom(1024)
    print(data.decode('utf-8'))
    # send a reply
    # sock.sendto(b'Hello, ' + data, addr)
