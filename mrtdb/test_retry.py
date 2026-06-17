from pktest import *
import json
import numpy as np
from tqdm import tqdm

l4 = 'tcp'
opt = 'all'
retry = 1

print(l4, opt, retry)

ip = {}
with open('asip.jsonl') as f:
    for line in f:
        a = json.loads(line)
        ip[a[2]] = max(ip.get(a[2], -9999), a[1])
ip = list(ip.items())
np.random.shuffle(ip)
ip = ip[:100]

runner = TestRunner()
runner.add_tests(TestGroupTTL.make_groups([i[0] for i in ip], l4, opt, ttl_min=5, ttl_max=30, max_retry=retry))
with tqdm(total=runner.count_remain(), unit='tests') as pbar:
    while not runner.done():
        pbar.update(runner.run_batch())

stats = {}
for k, p in runner.get_results():
    stats.setdefault(k[2], 0)
    stats[k[2]] += len([i for i in p if i != '*'])
print(stats)
print(np.sum(list(stats.values())))