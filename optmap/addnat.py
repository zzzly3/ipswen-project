import os

for i in range(1000, 65535):
    assert os.system(f'iptables -t nat -A PREROUTING -i eth2 -p tcp --dport {i} -j DNAT --to-destination 10.1.3.2:888')