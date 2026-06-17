def ipstr2int(ipstr):
    return sum([int(x) << (8 * (3 - i)) for i, x in enumerate(ipstr.split('.'))])

def ipint2str(ipint):
    return '.'.join([str((ipint >> (8 * (3 - i))) & 0xff) for i in range(4)])

as_prefix = dict()
with open('as_prefix.txt', 'r') as f:
    asn = None
    for line in f:
        s = line.strip()
        if asn is None:
            asn = int(s)
            as_prefix[asn] = []
        elif s == '':
            asn = None
        else:
            as_prefix[asn].append(s)

prefix_as = dict()
for asn, pl in as_prefix.items():
    for prefix in pl:
        ip, n = prefix.split('/')
        i = ipstr2int(ip)
        prefix_as[i] = (int(n), asn)

prefix_as2 = dict()
for i, (n, asn) in prefix_as.items():
    add = True
    m = n
    while m > 0:
        m -= 1
        j = i & (0xffffffff << (32 - m))
        if j in prefix_as and prefix_as[j][0] <= m:
            add = False
            break
    if add:
        prefix_as2[i] = (n, asn)

def get_ip_asn(ip):
    i = ipstr2int(ip)
    for n in range(32, -1, -1):
        j = i & (0xffffffff << (32 - n))
        if j in prefix_as2 and prefix_as2[j][0] <= n:
            return prefix_as2[j]