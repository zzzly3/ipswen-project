#!/usr/bin/python3

from paramiko import SSHClient, AutoAddPolicy
import time, sys, re, subprocess

def get_targets():
    t = [1, 2, 3, 4]
    t += [10, 11, 12, 13, 14]
    t += [20, 21, 22, 23]
    return t

def get_nodes():
    with open('nodes.txt', 'r') as f:
        nodes = f.read().splitlines()
    nodes = [re.split(r'\s+', i) for i in nodes]
    net = None
    res = {}
    for i in nodes:
        i = [j for j in i if len(j) > 0]
        if len(i) == 0:
            net = None
            continue
        if len(i) == 1:
            net = i[0]
            res[net] = []
            continue
        if net is not None:
            if len(i) == 2:
                res[net].append({'id': int(i[0]), 'ip': i[1]})
            continue
        if len(i) != 5:
            continue
        res[int(i[0])] = {
            'ip': i[1],
            'port': int(i[2]),
            'user': i[3],
            'password': i[4]
        }
    return res

def get_this():
    thisip = subprocess.check_output('ip -brief addr', shell=True).decode('ascii')
    for k, v in get_nodes().items():
        if isinstance(k, int) and v['ip'] in thisip:
            return k
    assert False

if __name__ == '__main__':
    sessions = {}
    outputs = {}
    errors = {}
    nodes = get_nodes()
    this = get_this()

    def ssh_open(host):
        if host == this:
            return
        ssh = SSHClient()
        ssh.set_missing_host_key_policy(AutoAddPolicy())
        ssh.connect(nodes[i]['ip'], nodes[i]['port'], nodes[i]['user'], nodes[i]['password'])
        sessions[host] = ssh
        print('NODE', host, '(connected)')

    def ssh_exec(host, command):
        if host == this:
            process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            outputs[host], errors[host] = process.stdout, process.stderr
        else:
            _, outputs[host], errors[host] = sessions[host].exec_command(command)
        print('NODE', host, '(exec)', command)

    hosts = get_targets()
    for i in hosts:
        ssh_open(i)

    command = ' '.join(sys.argv[1:]) if len(sys.argv) > 1 else 'echo hello world'
    for i in hosts:
        ssh_exec(i, command)

    try:
        while len(errors):
            done = set()
            for i in [this] + list(sessions.keys()):
                if i in outputs and outputs[i].readable():
                    r = str(outputs[i].read(65536), 'ascii', errors='ignore')
                    if len(r) > 0:
                        print('NODE', i, r)
                    elif i not in done:
                        print('NODE', i, '(done)')
                        done.add(i)
                if i in errors and errors[i].readable():
                    r = str(errors[i].read(65536), 'ascii', errors='ignore')
                    if len(r) > 0:
                        print('NODE', i, r)
                    elif i not in done:
                        print('NODE', i, '(done)')
                        done.add(i)
            for i in done:
                outputs.pop(i)
                errors.pop(i)
            time.sleep(1)
    except KeyboardInterrupt:
        pass
    for i in sessions:
        sessions[i].close()