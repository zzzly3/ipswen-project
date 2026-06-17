from scapy.all import *
from typing import List, Tuple, Iterable, Union, Optional, Type
from abc import ABC, abstractmethod
import re, traceback

conf.check_TCPerror_seqack = True

none_opt = b''
rr_opt = bytes([7, 39, 4] + [0] * 36)
nop_opt = b'\1'
ts_opt = bytes([68, 40, 5, 0] + [0] * 36)
x1_opt = bytes([28, 3, 0])
x2_opt = bytes([156, 3, 0])
x3_opt = bytes([92, 3, 0])
x4_opt = bytes([220, 3, 0])
x5_opt = bytes([124, 3, 0])
x6_opt = bytes([252, 3, 0])

x1_opt_variants = {}
for i in range(2, 41):
    if i != 3:
        x1_opt_variants['x1_' + str(i)] = bytes([28, i] + [0] * (i - 2))

rr_opt_variants = {}
for i in range(1, 9):
    rr_opt_variants['rr_' + str(i)] = bytes([7, 3 + i * 4, 4] + [0] * i * 4)

exp_options = ['none', 'rr', 'nop', 'ts', 'x1']

options = {
    'none': none_opt,
    'rr': rr_opt,
    'nop': nop_opt,
    'ts': ts_opt,
    'x1': x1_opt,
    'x2': x2_opt,
    'x3': x3_opt,
    'x4': x4_opt,
    'x5': x5_opt,
    'x6': x6_opt,
}

for i in x1_opt_variants:
    options[i] = x1_opt_variants[i]
for i in rr_opt_variants:
    options[i] = rr_opt_variants[i]

tier1 = [6762,
12956,
2914,
3356,
6453,
701,
6461,
3257,
1299,
3491,
7018,
3320,
5511,
6830,
7922,
174]

def make_packet(dst, l4, opt, ttl) -> Packet:
    try:
        flag = list(options.keys()).index(opt) + ttl * len(options)
        if l4 == 'icmp':
            pkt = IP(dst=dst, options=options[opt], ttl=ttl) / ICMP(id=flag, seq=10000-flag)
        elif l4 == 'tcp':
            pkt = IP(dst=dst, options=options[opt], ttl=ttl) / TCP(seq=flag)
        else:
            raise ValueError(f'Unsupported L4 protocol: {l4}')
        return pkt
    except:
        traceback.print_exc()
        print(f'Error: {dst} {l4} {opt} {ttl}')
        exit(1)

def get_packet_l4(pkt: Packet):
    if pkt.haslayer(ICMP):
        return 'icmp'
    elif pkt.haslayer(UDP):
        return 'udp'
    elif pkt.haslayer(TCP):
        return 'tcp'
    else:
        return None

def get_packet_opt(pkt: Packet):
    for i in options:
        if pkt[IP].options[0] == options[i]:
            return i
    return None

class TestGroupBase(ABC):
    def __init__(self, max_retry=4, max_fail=4):
        self.max_retry = max_retry
        self.max_fail = max_fail
        self.retry = 0
        self.fail = 0
        self.complete = False

    def update(self, rev: Packet):
        if rev is not None:
            self.fail = 0
            self.retry = 0
            self.update_result(rev)
            self.next_packet()
        else:
            self.retry += 1
            if self.retry > self.max_retry:
                self.retry = 0
                self.fail += 1
                if self.fail > self.max_fail:
                    self.complete = True
                self.next_packet()

    def done(self):
        return self.complete
    
    @abstractmethod
    def next_packet(self):
        pass

    @abstractmethod
    def get_packet(self) -> Optional[Packet]:
        pass
    
    @abstractmethod
    def update_result(self, rev: packet):
        pass

    @abstractmethod
    def get_result(self):
        pass

    @abstractmethod
    def get_key(self):
        pass

    @staticmethod
    @abstractmethod
    def get_packet_key(pkt: Packet):
        pass

    @staticmethod
    @abstractmethod
    def make_groups(self, *args, **kwargs):
        pass

class TestRunner:
    def __init__(self):
        self.type = None
        self.todo = set()
        self.tests = dict()

    def add_tests(self, tests: Iterable[TestGroupBase]):
        for t in tests:
            if self.type is None:
                self.type = type(tests[0])
            assert type(t) == self.type
            self.todo.add(t.get_key())
            self.tests[t.get_key()] = t

    def send_packets(self, pkts: List[Packet], *args, **kwargs):
        if len(pkts) == 0:
            return
        ans, unans = sr(pkts, filter='ip', *args, **kwargs)
        for i in unans:
            k = self.type.get_packet_key(i)
            self.tests[k].update(None)
            if self.tests[k].done():
                self.todo.remove(k)
        for snd, rev in ans:
            k = self.type.get_packet_key(snd)
            self.tests[k].update(rev)
            if self.tests[k].done():
                self.todo.remove(k)
    
    def prepare_packets(self, batch):
        packets = []
        for i in self.todo:
            if len(packets) >= batch:
                break
            pkt = self.tests[i].get_packet()
            assert pkt is not None
            packets.append(pkt)
        return packets
    
    def run_batch(self, batch=500, timeout=2, verbose=0, *args, **kwargs):
        r0 = self.count_remain()
        packets = self.prepare_packets(batch)
        self.send_packets(packets, timeout=timeout, verbose=verbose, *args, **kwargs)
        return r0 - self.count_remain()
    
    def done(self):
        return not self.todo

    def count_remain(self):
        return len(self.todo)
    
    def get_results(self):
        return [(i, self.tests[i].get_result()) for i in self.tests]

class TestGroupTTL(TestGroupBase):
    def __init__(self, dst, l4, opt, ttl_min=1, ttl_max=30, *args, **kwargs):
        TestGroupBase.__init__(self, *args, **kwargs)
        self.dst = dst
        self.l4 = l4
        self.opt = opt
        self.ttl_min = ttl_min
        self.ttl_max = ttl_max
        self.ttl = ttl_min
        self.result = []
    
    def next_packet(self):
        self.ttl += 1
        if self.ttl > self.ttl_max:
            self.complete = True

    def get_packet(self):
        if self.complete:
            return None
        return make_packet(self.dst, self.l4, self.opt, self.ttl)
    
    def update_result(self, rev: Packet):
        if rev[IP].src == self.dst:
            self.complete = True
        self.result.append((self.ttl, rev[IP].src))
    
    def get_result(self):
        if len(self.result) == 0:
            return []
        self.result.sort()
        path = ['*'] * (self.result[-1][0] - self.ttl_min + 1)
        for i, p in self.result:
            path[i - self.ttl_min] = p
        return path
    
    def get_key(self):
        return (self.dst, self.l4, self.opt)
    
    @staticmethod
    def get_packet_key(pkt: Packet):
        return (pkt[IP].dst, get_packet_l4(pkt), get_packet_opt(pkt))
    
    @staticmethod
    def make_groups(dst, l4, opt, *args, **kwargs):
        r = []
        if opt == 'all':
            opt = options.keys()
        if isinstance(dst, str):
            dst = [dst]
        if isinstance(l4, str):
            l4 = [l4]
        if isinstance(opt, str):
            opt = [opt]
        for d in dst:
            for l in l4:
                for o in opt:
                    r.append(TestGroupTTL(d, l, o, *args, **kwargs))
        return r
    
class TestGroupASIP(TestGroupBase):
    def __init__(self, asn: int, iplist: List[str], demand=1, l4='icmp', opt='none', *args, **kwargs):
        TestGroupBase.__init__(self)
        self.asn = asn
        self.iplist = iplist
        self.result = []
        self.pos = 0
        self.demand = demand
        self.l4 = l4
        self.opt = opt
        self.args = args
        self.kwargs = kwargs
        self.test = TestGroupTTL(iplist[0], l4, opt, *args, **kwargs)

    def update(self, rev: Packet):
        self.test.update(rev)
        if self.test.done():
            r = self.test.get_result()
            if len(r) > 0 and r[-1] == self.iplist[self.pos]:
                self.result.append(r[-1])
            self.pos += 1
            if self.pos >= len(self.iplist) or len(self.result) >= self.demand:
                self.complete = True
                return
            self.test = TestGroupTTL(self.iplist[self.pos], self.l4, self.opt, *self.args, **self.kwargs)

    def next_packet(self):
        pass

    def get_packet(self):
        if self.complete:
            return None
        pkt = self.test.get_packet()
        pkt[ICMP].id = self.asn // 4096
        pkt[ICMP].seq = self.asn % 4096
        return pkt
    
    def update_result(self, rev: packet):
        pass

    def get_result(self):
        return self.result
    
    def get_key(self):
        return self.asn
    
    @staticmethod
    def get_packet_key(pkt: Packet):
        return pkt[ICMP].id * 4096 + pkt[ICMP].seq
    
    @staticmethod
    def make_groups(aslist, *args, **kwargs):
        r = []
        for asn in aslist:
            r.append(TestGroupASIP(asn, aslist[asn], *args, **kwargs))
        return r
    
