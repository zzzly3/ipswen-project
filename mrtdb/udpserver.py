#!/usr/bin/env python

import socket, threading, sys, os, datetime

def log(s):
    s = str(datetime.datetime.now()) + '\t' + str(s)
    print(s)
    
def handle_data(data):
    data = str(data, 'utf-8').strip().split('\n')
    key = data[0]
    items = data[1:]
    print(key, items)


s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(("0.0.0.0", 53))

log('Orca\'s RAM daemon started.')

while True:
    try:
        data, addr = s.recvfrom(4096)
    except KeyboardInterrupt:
        break
    # handle_data(data)
    print(data)
    
