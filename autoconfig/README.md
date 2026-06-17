# autoconfig —— 自动化 GRE 隧道配置工具

## 概述

`autoconfig` 是一个轻量级 C/S 架构工具，用于**完全自动化**测试床中 GRE 隧道的建立和拆除。在许多网络研究场景中，需要频繁改变拓扑——手工 `ip tun add/del` 极易出错且不可扩展。此工具通过简单的 JSON-over-TCP 协议让任意数量的 Spoke 节点动态连接到 Hub。

**使用场景：**
- 动态拓扑实验：节点随时加入/离开 overlay 网络
- GRE 承载的 BGP Full-Mesh 测试
- 大规模隧道自动化部署

---

## 目录结构

```
autoconfig/
├── server.py      # 隧道分配服务（Hub 节点运行）
├── client.py      # 隧道客户端（Spoke 节点运行）
└── 1.ipynb        # Jupyter Notebook（实验记录）
```

---

## 架构与协议

```
  Spoke A (client.py)           Hub (server.py)           Spoke B
  ┌──────────────┐    GRE       ┌────────────────┐       ┌──────────┐
  │ connect      │◄────────────►│ listen :1111    │◄─────►│ connect  │
  │ → 得到 .1/31 │   tun-2531   │ 管理 128 个 /31 │       │ → .3/31  │
  └──────────────┘              └────────────────┘       └──────────┘
```

### 协议

1. TCP 连接 `server:1111`
2. Client → Server: `{"type": "connect"}` 或 `"disconnect"}\n`
3. Server → Client: `{"status": "success", "cidr": "A.B.C.D/NN"}` 或 `{"status": "fail", "reason": "..."}`

### 隧道命名

```python
idx = server_addr.split('.')[-1] + server_addr.split('.')[-2]
# server=192.168.1.253 → idx="2531" → tun-2531
```

### 地址池

`192.168.1.0/24` 中 128 个 `/31` 子网，每对点对点隧道使用一个。

---

## 源码全文解析

### server.py

#### Tunnel 类

```python
class Tunnel:
    def __init__(self, ipv4_peer, cidr_peer):
        self.ipv4_peer = ipv4_peer        # (server_ip, client_ip)
        self.cidr_peer = cidr_peer        # (server_cidr, client_cidr)
        self.idx = hex(hash(ipv4_peer[1]) % 0x1000000)[2:]  # hash→hex

    def install(self):
        # ip tun add tun-XXXX mode gre remote <client> local <server>
        assert os.system(f'ip tun add tun-{self.idx} mode gre '
                         f'remote {self.ipv4_peer[1]} local {self.ipv4_peer[0]}') == 0
        # ip addr add <server_cidr> dev tun-XXXX
        assert os.system(f'ip addr add {self.cidr_peer[0]} dev tun-{self.idx}') == 0
        # ip link set tun-XXXX up mtu 1450
        assert os.system(f'ip link set tun-{self.idx} up mtu 1450') == 0
        installed_tunnels[self.ipv4_peer[1]] = self

    def uninstall(self):
        assert os.system(f'ip link del tun-{self.idx}') == 0
        del installed_tunnels[self.ipv4_peer[1]]
```

**`assert` 的作用：** 如果 `ip` 命令返回非零（失败），程序立即崩溃——这是一种"快速失败"策略。比静默忽略错误更安全，因为半建立的隧道会导致后续问题。

**`idx` 的哈希设计：** `hash(client_ip) % 0x1000000` 取 24 位哈希值，转十六进制（最多 6 位）。这样不同客户端 IP 总是得到不同的 idx，且 IP 不变则 idx 不变（幂等性——重复 connect/disconnect 使用同一隧道名）。

#### MyTCPHandler

```python
class MyTCPHandler(socketserver.StreamRequestHandler):
    def _handle(self, req):
        ipv4_peer = self.request.getsockname()[0], self.client_address[0]

        if req['type'] == 'connect':
            if len(cidr_to_alloc) == 0:
                return {'status': 'fail', 'reason': 'No enough resources.'}
            if self.client_address[0] in installed_tunnels:
                return {'status': 'fail', 'reason': 'Already connected'}
            cidr_peer = cidr_to_alloc.pop()         # 取一个 /31
            tunnel = Tunnel(ipv4_peer, cidr_peer)
            tunnel.install()
            return {'status': 'success', 'cidr': tunnel.cidr_peer[1]}

        elif req['type'] == 'disconnect':
            if self.client_address[0] not in installed_tunnels:
                return {'status': 'fail', 'reason': 'Not connected'}
            tunnel = installed_tunnels[self.client_address[0]]
            tunnel.uninstall()
            cidr_to_alloc.append(tunnel.cidr_peer)  # 归还
            return {'status': 'success'}
```

四个边界情况全覆盖：
1. **资源耗尽**：`len(cidr_to_alloc) == 0`
2. **重复连接**：同 IP 已在 `installed_tunnels`
3. **未连接就断开**：IP 不在 `installed_tunnels`
4. **未知请求类型**：else 分支返回错误

#### 优雅退出

```python
if __name__ == '__main__':
    server = socketserver.TCPServer(('0.0.0.0', 1111), MyTCPHandler)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        for tunnel in list(installed_tunnels.values()):
            tunnel.uninstall()
```

`list()` 包裹是因为在迭代时修改 dict——先复制一份所有隧道列表，逐个卸载。

### client.py

```python
action = sys.argv[1]; server_addr = sys.argv[2]
idx = server_addr.split('.')[-1] + server_addr.split('.')[-2]

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((server_addr, 1111))
s.send(json.dumps({'type': action}).encode() + b'\n')
res = json.loads(s.recv(1024).decode())

if action == 'disconnect':
    os.system(f'ip link del tun-{idx}')  # 先删隧道再检查响应

if res['status'] == 'fail':
    print(res['reason']); sys.exit(1)

if action == 'connect':
    os.system(f'ip tun add tun-{idx} mode gre remote {s.getpeername()[0]} local {s.getsockname()[0]}')
    os.system(f'ip addr add {res["cidr"]} dev tun-{idx}')
    os.system(f'ip link set tun-{idx} up mtu 1450')
```

**设计细节：** `disconnect` 时客户端**不管服务端响应如何都先删本地隧道**——这样可以清理即使服务端状态已丢失的残留隧道。

---

## MTU 设计

```
1500 (以太网)
 - 20 (外层 IP)
 -  4 (GRE)
 - 26 (预留 IP 选项)
──────────────
 1450 ← 隧道内层 MTU
```

26 字节预留支持实验性 IP 选项（最大 40 字节）。如果路径涉及多层封装（IP-in-IP over GRE），需要进一步降低。

---

## 实验定制指南

### 扩展地址池

```python
# 512 个 /31（从 /23 中）
for i in range(512):
    cidr_to_alloc.append((f'10.0.0.{i*2}/31', f'10.0.0.{i*2+1}/31'))
```

### 改隧道类型

```python
# GRE → IP-in-IP
os.system(f'ip tun add tun-{idx} mode ipip remote ... local ...')
```

### 加认证

```python
# server 验证
if req.get('token') != 'my_secret':
    return {'status': 'fail', 'reason': 'Auth required'}
```

### 集成到 bash 脚本

```bash
#!/bin/bash
python3 client.py connect 192.168.1.253
sleep 1
ping -c 3 192.168.1.0 && echo "Tunnel OK" || echo "Tunnel FAIL"
```

---

## 依赖

- Python 3.6+（stdlib 仅 `json, socket, os, sys, socketserver`）
- Linux `CONFIG_NET_IPGRE`
- iproute2 + root

---

## 故障排查

| 症状 | 最可能原因 | 验证 | 解决 |
|------|----------|------|------|
| `Network is unreachable` | 防火墙堵 1111 | `nc -zv server 1111` | 放行 TCP 1111 |
| `No enough resources` | 128 个 /31 全占满 | 杀掉无用连接 | 扩大地址池 |
| `File exists` | 隧道名冲突 | `ip link show | grep tun` | 手动 `ip link del` |
| ping 不通 | GRE(proto 47) 被拦 | `tcpdump -i eth0 proto 47` | `iptables -A INPUT -p 47 -j ACCEPT` |
| 服务端崩后残留 | 状态仅内存 | `ip link show | grep tun` | 各客户端手动清理 |

---

## 相关项目

- **bird** —— 隧道上的 BGP 路由
- **mrtdb** —— 连通性测试
- **happyfootball** —— 隧道接口上的地址选择测试

---

## 附录 A: 快速部署脚本

### Hub 节点一键启动
```bash
#!/bin/bash
# hub_start.sh
cd $(dirname $0)
sudo python3 server.py &
echo "Hub server started on :1111"
```

### Spoke 节点一键连接
```bash
#!/bin/bash
# spoke_connect.sh
SERVER=${1:-192.168.1.253}
cd $(dirname $0)
sudo python3 client.py connect $SERVER
# 验证
MYIP=$(python3 -c "import socket;s=socket.socket();s.connect(('$SERVER',1111));print(s.getsockname()[0])")
ping -c 3 -W 2 $(echo $MYIP | awk -F. '{print $1"."$2"."$3"."($4%2==0?$4+1:$4-1)}')
```

### 一键断开所有
```bash
#!/bin/bash
for TUN in $(ip link show | grep -oP 'tun-\w+'); do
    sudo ip link del $TUN
done
```

## 附录 B: 扩展地址池示例

### 大容量 /24 池（256 个 /31）
```python
cidr_to_alloc = []
for i in range(256):
    cidr_to_alloc.append((f'10.99.0.{i*2}/31', f'10.99.0.{i*2+1}/31'))
```

### 多 /24 池（1024 个 /31）
```python
for subnet in range(4):
    for i in range(256):
        cidr_to_alloc.append((f'10.{subnet}.{i//2}.{(i%2)*128}/31', ...))
```

## 附录 C: 隧道状态检查脚本
```bash
#!/bin/bash
echo "Active tunnels:"
ip link show | grep -E "tun-" | awk '{print $2}' | sed 's/:$//'
echo ""
echo "GRE traffic:"
sudo tcpdump -i any proto 47 -c 5 -n 2>/dev/null || echo "No GRE traffic"
```
