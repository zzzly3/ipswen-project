import paramiko
import time
import subprocess
import json
import sys
import numpy as np
from tqdm import tqdm

'''
nodes = []
with open('nodes.txt', 'r') as f:
    for i in f.readlines():
        ls = i.split()
        if len(ls) == 5:
            nodes.append({'id':int(ls[0]), 'hostname':ls[1], 'port':ls[2], 'username':ls[3], 'password':ls[4]})
'''
ip = {}
with open('asip.jsonl') as f:
    for line in f:
        a = json.loads(line)
        ip[a[2]] = max(ip.get(a[2], -9999), a[1])
ip = list(ip.items())
np.random.shuffle(ip)

local = 0
#ex = {1:[13], 2:[13, 3, 22], 3:[12, 13, 14], 4:[12, 13, 14, 2, 3, 4, 21, 22, 23]}
ex = {1:[13]}
if local == 0:
    def scp_upload():
        '''
        for ex_id in [0, 1, 2, 3, 4]:
            for node in nodes:
                subprocess.check_call('mkdir ../ex%d/node%d' % (ex_id, node['id']), shell=True)

        for node in nodes:
            print('node', node['id'])
            print('  SCP node%d.conf' % (node['id']))
            subprocess.check_call('sshpass -p %s scp -o StrictHostKeychecking=no -P %s ../ex/node%d.conf %s@%s:' % (node['password'], node['port'], node['id'], node['username'], node['hostname']), shell=True)
        '''

        for ex_id in ex:
            ex_nodes = [d for d in nodes if d['id'] in ex[ex_id]]
            for node in ex_nodes:
                print('node', node['id'])
                print('  SCP data')
                subprocess.check_call('sshpass -p %s scp -o StrictHostKeychecking=no -P %s ../ex/ex%d_node%d_IPv4.conf ../ex/ex%d_node%d_IPv6.conf ../ex/ex%d_node%d_IPswen.conf %s@%s:' % (node['password'], node['port'], ex_id, node['id'], ex_id, node['id'], ex_id, node['id'], node['username'], node['hostname']), shell=True)

    def mrtdb_control():
        '''
        for ex_id in [1, 2, 3, 4]:
            for node in nodes:
                print('Local')
                print('  pypy3 ../mrtdb_builder/mrtdb_builder.py ../ex%d/node%d/ex%d_node%d_master4.mrt -m -O ../ex%d/node%d/ex%d_node%d_master4_builder.out' % (ex_id, node['id'], ex_id, node['id'], ex_id, node['id'], ex_id, node['id']))
                subprocess.check_call('  pypy3 ../mrtdb_builder/mrtdb_builder.py ../ex%d/node%d/ex%d_node%d_master4.mrt -m -O ../ex%d/node%d/ex%d_node%d_master4_builder.out' % (ex_id, node['id'], ex_id, node['id'], ex_id, node['id'], ex_id, node['id']), shell=True)
                print('  ../mrtdb_user/mrtdb_user -n ../ex%d/node%d/' % (ex_id, node['id']))
                subprocess.check_call('  ../mrtdb_user/mrtdb_user -n ../ex%d/node%d/' % (ex_id, node['id']), shell=True)
                print('  ../mrtdb_user/mrtdb_user -d ../ex%d/node%d/' % (ex_id, node['id']))
                subprocess.check_call('  ../mrtdb_user/mrtdb_user -d ../ex%d/node%d/' % (ex_id, node['id']), shell=True)
        print('  python3 ../mrtdb_stat/mrtdb_stat.py')
        subprocess.check_call('  python3 ../mrtdb_stat/mrtdb_stat.py', shell=True)
        '''

        for i in tqdm(ip):
            subprocess.check_call('echo %s | sudo -S ./%s %s' % ('PKUnet1440', sys.argv[-1], i[0]), shell=True)
        '''
        res = {}
        with open('res.txt', 'r') as f:
            for line in f:
                ls = line.strip().split()
                if ls[0] in res:
                    res[ls[0]].append(float(ls[-1]))
                else:
                    res[ls[0]] = [float(ls[-1])]
        res_opt = {}
        with open('res_opt.txt', 'r') as f:
            for line in f:
                ls = line.strip().split()
                if ls[0] in res_opt:
                    res_opt[ls[0]].append(float(ls[-1]))
                else:
                    res_opt[ls[0]] = [float(ls[-1])]
        cnt = [0] * 21
        for ip in res_opt:
            if ip not in res:
                continue
            if len(res[ip]) < 3 or len(res_opt[ip]) < 3:
                continue
            a = 0
            for x in res[ip]:
                a += x
            a = a / len(res[ip])
            b = 0
            for x in res_opt[ip]:
                b += x
            b = b / len(res_opt[ip])
            c = (b - a) / a
            if int((c + 1.05) * 10) < 0 or int((c + 1.05) * 10) >20:
                continue
            cnt[int((c + 1.05) * 10)] += 1
        s = 0
        for i in range(8, 16):
            s += cnt[i]
            print(0.1 * i - 1, cnt[i])
        for i in range(8, 16):
            print(0.1 * i - 1, cnt[i] / s)
        print(s, len(res_opt))
        '''
    mrtdb_control()
else:
    ex_id = 1
    sessions = {}
    outputs = {}
    errors = {}

    def command(node):
        #cmds = ['sshpass -p 100200 scp -o StrictHostKeychecking=no -P 22 bird.log net@10.10.10.23:/home/net/log/bird_exception_IPv6_node%d.log' % (node['id'])] if node['id'] != 1 else ['sshpass -p 100200 scp -o StrictHostKeychecking=no -P 22 ./ext2/linux-app-layers/bird-2.13.1/bird.log net@10.10.10.23:/home/net/log/bird_exception_IPv6_node%d.log' % (node['id'])]
        cmds = ['echo %s | sudo -S ./birdc -l show protocols all' % (node['password'])] if node['id'] != 1 else ['echo %s | sudo -S ./ext2/linux-app-layers/bird-2.13.1/birdc -s ./ext2/linux-app-layers/bird-2.13.1/bird.ctl show protocols all' % (node['password'])]
        #cmds = ['echo %s | sudo -S apt install sshpass' % (node['password'])] if node['id'] >= 10 else ['ls']
        return ' && '.join(cmds)

    def ssh_open(node):
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(hostname=node['hostname'], port=node['port'], username=node['username'], password=node['password'])
        sessions[node['id']] = ssh
        print('node', node['id'], '(connected)')

    def ssh_exec(node, command):
        _, outputs[node['id']], errors[node['id']] = sessions[node['id']].exec_command(command)
        print('node', node['id'], command)

    for i in nodes:
        ssh_open(i)

    for i in nodes:
        ssh_exec(i, command(i))

    try:
        while len(errors):
            done = set()
            for i in sessions:
                if i in outputs and outputs[i].readable():
                    r = str(outputs[i].read(65536), 'ascii', errors='ignore')
                    if len(r) > 0:
                        print('node', i, r)
                    elif i not in done:
                        print('node', i, '(done)')
                        done.add(i)
                if i in errors and errors[i].readable():
                    r = str(errors[i].read(65536), 'ascii', errors='ignore')
                    if len(r) > 0:
                        print('node', i, r)
                    elif i not in done:
                        print('node', i, '(done)')
                        done.add(i)
            for i in done:
                outputs.pop(i)
                errors.pop(i)
            time.sleep(1)
    except KeyboardInterrupt:
        pass
    for i in sessions:
        sessions[i].close()