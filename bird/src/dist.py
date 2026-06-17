#!/usr/bin/python3

import os, glob, subprocess, shutil, sys, traceback
from conf import encode_objstr
from control import get_nodes, get_targets, get_this

nodes = get_nodes()
hosts = get_targets()
this = get_this()
print(f'TARGET {hosts}')
print(f'THIS {this}')

files = sys.argv[1:]
if len(files) == 0:
    files = ['bird', 'birdc', 'birdcl'] + glob.glob('*.template')
target_path = ''

try:
    for host in hosts:
        print(f'NODE {host}')
        for f in files:
            tmp = f
            output = f
            if output[-9:] == '.template':
                print(f'\tCONF {f}')
                output = output[:-9]
                subprocess.check_call(f'id={host} ./conf.py {tmp} {tmp}.1', shell=True)
                tmp += '.1'
            if output[-3:] == '.py' or output[-3:] == '.sh':
                print(f'\tCHMOD {f}')
                subprocess.check_call(f'chmod a+x {tmp}', shell=True)
            if host != this:
                print(f'\tSCP {f}')
                subprocess.check_call(f'sshpass -p {nodes[host]["password"]} scp -o "StrictHostKeyChecking=no" -P {nodes[host]["port"]} {tmp} {nodes[host]["user"]}@{nodes[host]["ip"]}:{target_path}{output}', shell=True)
            elif tmp != f:
                shutil.copy(tmp, output)
            if tmp != f:
                os.remove(tmp)
except Exception:
    traceback.print_exc(limit=0)
    exit(1)