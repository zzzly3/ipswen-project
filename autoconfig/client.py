import json, socket, os, sys

action = sys.argv[1]
server_addr = sys.argv[2]
idx = server_addr.split('.')[-1] + server_addr.split('.')[-2]

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect((server_addr, 1111))
socket.send(json.dumps({'type': action}).encode('utf-8') + b'\n')
res = json.loads(socket.recv(1024).decode('utf-8'))
if action == 'disconnect':
    os.system(f'ip link del tun-{idx}')
if res['status'] == 'fail':
    print(res['reason'])
    sys.exit(1)
if action == 'connect':
    os.system(f'ip tun add tun-{idx} mode gre remote {socket.getpeername()[0]} local {socket.getsockname()[0]}')
    os.system(f'ip addr add {res["cidr"]} dev tun-{idx}')
    os.system(f'ip link set tun-{idx} up mtu 1450')