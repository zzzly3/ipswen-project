#!/usr/bin python
import argparse
import subprocess
import sys
import time
import numpy as np
import json
import traceback
from tqdm import tqdm
from pktest import *

l4 = ['icmp', 'tcp']
opt = list(rr_opt_variants.keys()) + ['rr', 'x1', 'x1_2']
retry = 6

workers = 1

def get_slice(all, i):
    n = len(all)
    return all[i * n // workers: (i + 1) * n // workers]

def main():
    parser = argparse.ArgumentParser(description="A simple argument parser")
    parser.add_argument('--id', type=int, help='Worker ID', default=0)
    workerid = parser.parse_args().id

    ip = {}
    with open('asip.jsonl') as f:
        for line in f:
            a = json.loads(line)
            ip[a[2]] = max(ip.get(a[2], -9999), a[1])
    ip = list(ip.items())
    np.random.shuffle(ip)

    # with open('all_targets.jsonl') as f:
    #     for line in f:
    #         a = json.loads(line)
    #         asip[a[0]] = a[1][:20]
    # with open('asip.jsonl') as f:
    #     for line in f:
    #         a = json.loads(line)
    #         del asip[a[0]]

    # ip = ip[:100]
    runner = TestRunner()
    # runner.add_tests(TestGroupASIP.make_groups(asip, ttl_min=30, ttl_max=30))
    with tqdm(total=len(ip)*len(opt)*len(l4), unit='tests', disable= workerid != 0) as pbar:
        i = 0
        while True:
            if runner.count_remain() < 1000:
                for _ in range(100):
                    if i < len(ip):
                        if ip[i][1] == 0:
                            max_fail = 10
                        else:
                            max_fail = 4
                        runner.add_tests(TestGroupTTL.make_groups(ip[i][0], l4, opt, ttl_min=64, ttl_max=64, max_fail=max_fail, max_retry=retry))
                        i += 1
                    else:
                        break
            pbar.update(runner.run_batch())
            if runner.done():
                break
    
    res = runner.get_results()
    with open(f'worker{workerid}_out.jsonl', 'w') as f:
        for r in res:
            f.write(json.dumps(r) + '\n')

if __name__ == "__main__":
    main()
    exit(0)
    if len(sys.argv) == 1:
        processes = []
        try:
            for i in range(workers):
                p = subprocess.Popen(['python3', 'test_ip.py', '--id', str(i)], stdout=sys.stdout, stderr=sys.stderr)
                processes.append((i, p))
                print(f'Worker {i} started')
            while processes:
                for i, p in processes[:]:
                    if p.poll() is not None:
                        print(f'Worker {i} has exited with code {p.returncode}')
                        processes.remove((i, p))
                time.sleep(1)
        except Exception as e:
            traceback.print_exc()
            print("Interrupted, terminating child processes...")
            for i, p in processes:
                p.kill()
    else:
        main()
