#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/stdc++.h>
#include <netinet/tcp.h>

double get_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

#define log(fmt, ...) fprintf(stderr, "%lf: " fmt "\n", get_time(), ##__VA_ARGS__)

int get_socketerror(int sock)
{
    int so_error;
    socklen_t len = sizeof so_error;
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
    return so_error;
}

void bind_device(int sock, const char * device)
{
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, device, strlen(device)) < 0) {
        perror("SO_BINDTODEVICE failed");
        exit(-1);
    }
}

void add_ip_options(int sock, int size)
{
    if (size == 0)
        return;
    unsigned char ip_options[64] = {0};
    ip_options[0] = 0b10100000;
    ip_options[1] = size + 1;
    // unsigned char ip_options[] = {0b10100000, 2, 0b10110000, 2};
    if (setsockopt(sock, IPPROTO_IP, IP_OPTIONS, ip_options, size + 1) < 0) {
        perror("IP_OPTIONS failed");
        exit(-1);
    }
}

int get_mss(int sock)
{
    int mss;
    socklen_t len = sizeof mss;
    getsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, &mss, &len);
    return mss;
}

int main(int argc, char ** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip>[:port] <size>\n", argv[0]);
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (strchr(argv[1], ':')) {
        char * port = strchr(argv[1], ':');
        *port = 0;
        port++;
        addr.sin_port = htons(atoi(port));
    } else {
        addr.sin_port = htons(1110);
    }
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    std::cout << "addr: " << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port) << std::endl;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // bind_device(sock1, "ipiptun");
    int size = atoi(argv[2]);
    add_ip_options(sock, size);
    double t1 = get_time();
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr))) {
        perror("connect failed");
        return -1;
    }
    // static char buf[4096];
    // memset(buf, 0xcd, 4096);
    // for (int i = 0; i < 1024; i++) {
    //     send(sock, buf, 4096, 0);
    // }
    std::cout << "time: " << get_time() - t1 << std::endl;
    std::cout << "mss: " << get_mss(sock) << std::endl;
    close(sock);
    // sendto(sock1, "wogjowigojwe", 12, 0, (struct sockaddr*)&addr, sizeof(addr));
    return 0;
}
