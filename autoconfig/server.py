import json, socketserver, os

cidr_to_alloc = []
for i in range(128):
    cidr_to_alloc.append((f'192.168.1.{i*2}/31', f'192.168.1.{i*2+1}/31'))

installed_tunnels = {}
class Tunnel:
    def __init__(self, ipv4_peer, cidr_peer):
        self.ipv4_peer = ipv4_peer
        self.cidr_peer = cidr_peer
        self.idx = hex(hash(ipv4_peer[1]) % 0x1000000)[2:]
    def install(self):
        assert os.system(f'ip tun add tun-{self.idx} mode gre remote {self.ipv4_peer[1]} local {self.ipv4_peer[0]}') == 0
        assert os.system(f'ip addr add {self.cidr_peer[0]} dev tun-{self.idx}') == 0
        assert os.system(f'ip link set tun-{self.idx} up mtu 1450') == 0
        installed_tunnels[self.ipv4_peer[1]] = self
    def uninstall(self):
        assert os.system(f'ip link del tun-{self.idx}') == 0
        del installed_tunnels[self.ipv4_peer[1]]

class MyTCPHandler(socketserver.StreamRequestHandler):
    def _handle(self, req):
        ipv4_peer = self.request.getsockname()[0], self.client_address[0]
        print(req['type'], ipv4_peer)
        if req['type'] == 'connect':
            if len(cidr_to_alloc) == 0:
                return {'status': 'fail', 'reason': 'No enough resources.'}
            if self.client_address[0] in installed_tunnels:
                return {'status': 'fail', 'reason': 'Already connected'}
            cidr_peer = cidr_to_alloc.pop()
            tunnel = Tunnel(ipv4_peer, cidr_peer)
            tunnel.install()
            return {'status': 'success', 'cidr': tunnel.cidr_peer[1]}
        elif req['type'] == 'disconnect':
            if self.client_address[0] not in installed_tunnels:
                return {'status': 'fail', 'reason': 'Not connected'}
            tunnel = installed_tunnels[self.client_address[0]]
            tunnel.uninstall()
            cidr_to_alloc.append(tunnel.cidr_peer)
            return {'status': 'success'}
        else:
            return {'status': 'Unknown type ' + req['type']}
    def handle(self):
        self.data = self.rfile.readline().strip()
        req = json.loads(self.data.decode('utf-8'))
        res = self._handle(req)
        self.wfile.write(json.dumps(res).encode('utf-8'))

if __name__ == '__main__':
    server = socketserver.TCPServer(('0.0.0.0', 1111), MyTCPHandler)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        for tunnel in list(installed_tunnels.values()):
            tunnel.uninstall()
