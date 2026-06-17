import time, datetime, os, sys

ping_points = [
    0, 10,
    11, 20,
    21, 29,
    30, 37,
    38, 44,
    45, 50,
    51, 55,
    56, 59,
    60, 62,
    63, 64
]
if sys.argv[1] == 'wait':
    ping_points = ping_points[::2]
elif sys.argv[1] == 'send':
    pass
elif sys.argv[1] == 'init':
    local, remote, subnet = sys.argv[2], sys.argv[3], sys.argv[4]
    idx = 'tun' + str(hash(local + remote + subnet))[0:4]
    os.system(f'ip tun add {idx} mode gre local {local} remote {remote}')
    os.system(f'ip addr add {subnet} dev {idx}')
    os.system(f'ip link set {idx} up')
    exit(0)

peer = sys.argv[2]
start_time = sys.argv[3], sys.argv[4]

def wait_time(hour, minute):
    while True:
        if datetime.datetime.now().hour == hour and datetime.datetime.now().minute == minute:
            break
        time.sleep(0.2)

def ping_at(ip, hour, minute):
    wait_time(hour, minute)
    print(datetime.datetime.now())
    os.system(f'ping {ip} -c 5 -w 5')

for i in ping_points:
    hour = int(start_time[0])
    minute = int(start_time[1]) + i
    while minute >= 60:
        hour += 1
        minute -= 60
    ping_at(peer, hour, minute)
