import socket
import time

# AF_INET 表示使用IPv4, SOCK_DGRAM 则表明数据将是数据报(datagrams)
udp_client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

#ip_options[] = {0b10100000, 2, 0b10110000, 2};
ip_options = b'\xa0\x09\xff\xff\xff\xff\xff\xff\xff\xb0\x09\xff\xff\xff\xff\xff\xff\xff'
udp_client.setsockopt(socket.IPPROTO_IP, socket.IP_OPTIONS, ip_options)

client_msg = 'a' * 1024 * 50

st = time.time()
udp_client.sendto(client_msg.encode('utf8'), ('192.168.1.253', 80))
rec_msg, addr = udp_client.recvfrom(1024 * 50)
print('msg form server:', len(rec_msg.decode('utf8')))
et = time.time()
print('time :%s' % (et - st))

udp_client.close()
