all: send recv optmap.o send2 recv2 mytcp.so

WAN_DEV ?= eth1
LAN_DEV ?= eth2

send: send.c optmap.h
	gcc -o send send.c

recv: recv.c optmap.h
	gcc -o recv recv.c

send2: send2.c optmap.h
	gcc -o send2 send2.c

recv2: recv2.c optmap.h
	gcc -o recv2 recv2.c

mytcp.so: mytcp.c optmap.h
	gcc -shared -fPIC -o mytcp.so mytcp.c -ldl

ebpf.o: ebpf.c
	clang -O2 -target bpf -c ebpf.c -o ebpf.o

optmap.o: optmap.c optmap.h
	clang -O2 -target bpf -c optmap.c -o optmap.o

install:
	echo 1 > /proc/sys/net/ipv4/conf/all/forwarding
	echo 1 > /proc/sys/net/ipv4/conf/all/accept_local
	ip link set dev $(WAN_DEV) xdp obj optmap.o sec xdp_router
	ip link set dev $(LAN_DEV) xdp obj optmap.o sec xdp_router

uninstall:
	ip link set dev $(WAN_DEV) xdp off
	ip link set dev $(LAN_DEV) xdp off