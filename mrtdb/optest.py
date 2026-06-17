from scapy.all import *
from typing import List, Tuple, Iterable

none_opt = b''
rr_opt = bytes([7, 11, 4, 0, 0, 0, 0, 0, 0, 0, 0])
nop_opt = b'\1'
unkctl3_opt = bytes([156, 3, 233])
unkres3_opt = bytes([252, 3, 233])
unkctl40_opt = bytes([157, 40] + [233] * 38)
unkres40_opt = bytes([253, 40] + [233] * 38)

options = {
    'none': none_opt,
    'rr': rr_opt,
    'nop': nop_opt,
    'unkctl3': unkctl3_opt,
    'unkres3': unkres3_opt,
    'unkctl40': unkctl40_opt,
    'unkres40': unkres40_opt
}

def make_packet(dst, l4, opt, ttl) -> Packet:
    flag = list(options.keys()).index(opt) + ttl * len(options)
    if l4 == 'icmp':
        pkt = IP(dst=dst, options=options[opt], ttl=ttl) / ICMP(id=flag, seq=1000-flag)
    else:
        raise ValueError(f'Unsupported L4 protocol: {l4}')
    return pkt

def get_packet_group(pkt):
    l4 = '*'
    opt = '*'
    if pkt.haslayer(ICMP):
        l4 = 'icmp'
    elif pkt.haslayer(UDP):
        l4 = 'udp'
    elif pkt.haslayer(TCP):
        l4 = 'tcp'
    for i in options:
        if pkt[IP].options[0] == options[i]:
            opt = i
            break
    assert pkt[IP].ttl > 0
    return (pkt[IP].dst, l4, opt), pkt[IP].ttl

class TestGroup:
    def __init__(self, key, ttl_min=1, ttl_max=32, max_retry=4, max_fail=4):
        self.key = key
        self.ttl_min = ttl_min
        self.ttl_max = ttl_max
        self.max_retry = max_retry
        self.max_fail = max_fail
        self.ttl = ttl_min
        self.retry = 0
        self.fail = 0
        self.complete = False
        self.path = []
        self.log = []

    def update(self, ttl: int, src: str):
        assert ttl == self.ttl
        if ttl != self.ttl:
            return
        if src is not None:
            self.fail = 0
            self.retry = 0
            self.ttl += 1
            self.path.append((ttl, src))
            if src == self.key[0]:
                self.complete = True
        else:
            self.retry += 1
            if self.retry > self.max_retry:
                self.retry = 0
                self.fail += 1
                self.ttl += 1
                if self.fail > self.max_fail:
                    self.complete = True
        if self.ttl > self.ttl_max:
            self.complete = True
    
    def done(self) -> bool:
        return self.complete

    def get(self) -> Packet:
        if self.done():
            return None
        return make_packet(*self.key, ttl=self.ttl)

    def get_path(self) -> List[str]:
        if len(self.path) == 0:
            return []
        self.path.sort()
        path = ['*'] * (self.path[-1][0] - self.ttl_min + 1)
        for i, p in self.path:
            path[i - self.ttl_min] = p
        return path

class TestRunner:
    def __init__(self) -> None:
        self.todo = set()
        self.tests = {}

    def add_tests(self, dst: List[str], l4: List[str], opt: List[str], *args, **kwargs):
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
                    self.todo.add((d, l, o))
                    self.tests[(d, l, o)] = TestGroup((d, l, o), *args, **kwargs)

    def send_packets(self, packets, timeout=2, verbose=0, *args, **kwargs):
        if not packets:
            return 0
        ans, unans = sr(packets, filter='ip', timeout=timeout, verbose=verbose)
        for i in unans:
            k, v = get_packet_group(i)
            # self.tests[k].log.append(i)
            self.tests[k].update(v, None)
            if self.tests[k].done():
                self.todo.remove(k)
        for snd, rev in ans:
            k, v = get_packet_group(snd)
            # self.tests[k].log.append((snd, rev))
            self.tests[k].update(v, rev[IP].src)
            if self.tests[k].done():
                self.todo.remove(k)

    def prepare_packets(self, batch=500, *args, **kwargs):
        packets = []
        for i in self.todo:
            if len(packets) >= batch:
                break
            pkt = self.tests[i].get()
            assert pkt is not None
            packets.append(pkt)
        return packets
    
    def send_batch(self, *args, **kwargs):
        r0 = self.count_remain()
        self.send_packets(self.prepare_packets(*args, **kwargs), *args, **kwargs)
        return r0 - self.count_remain()
    
    def done(self):
        return not self.todo

    def count_remain(self):
        return len(self.todo)

    def get_paths(self) -> List[Tuple[str, str, str, List[str]]]:
        return [(i, self.tests[i].get_path()) for i in self.tests]
