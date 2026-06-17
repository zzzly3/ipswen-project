import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sock.sendto('a bu g i e hi aabugiehia'.encode(), ('127.0.0.1', 12345))

