#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/stdc++.h>

#define log(fmt, ...) fprintf(stderr, "%lf: " fmt "\n", get_time(), ##__VA_ARGS__)

double get_time()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int get_socketerror(int sock)
{
    int so_error;
    socklen_t len = sizeof so_error;
    getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
    return so_error;
}

int wait_writeable(const std::vector<int> &socks, struct timeval& tv)
{
    fd_set myset;
    FD_ZERO(&myset);
    for (int sock : socks) FD_SET(sock, &myset);
    if (select(*std::max_element(socks.begin(), socks.end()) + 1, NULL, &myset, NULL, &tv) > 0)
    {
        for (int sock : socks) if (FD_ISSET(sock, &myset)) return sock;
    }
    return -1;
}

int select_socket(std::vector<int> &socks, struct timeval& tv)
{
    int sock = wait_writeable(socks, tv);
    if (sock == -1) {
        log("timeout");
        socks.empty();
        for (int sock : socks) close(sock);
        return -1;
    }
    socks.erase(std::remove(socks.begin(), socks.end(), sock), socks.end());
    int soerr = get_socketerror(sock);
    if (soerr == 0) {
        log("fd %d: ready", sock);
        return sock;
    } else {
        log("fd %d: %s", sock, strerror(soerr));
        close(sock);
        if (socks.empty()) return -1;
        return select_socket(socks, tv);
    }
}

int happy_socket(const struct sockaddr * addr, int sock1, int sock2, int timeout, int wait2us)
{
    long arg = fcntl(sock1, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(sock1, F_SETFL, arg);
    arg = fcntl(sock2, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(sock2, F_SETFL, arg);
    log("sock1 = %d, sock2 = %d", sock1, sock2);
    connect(sock1, addr, sizeof(*addr));
    connect(sock2, addr, sizeof(*addr));
    std::vector<int> socks = {sock1, sock2};
    struct timeval tv1 = {timeout, 0}, tv2 = {0, wait2us};
    int sock = select_socket(socks, tv1);
    if (sock == sock1) {
        log("wait for sock2");
        if (select_socket(socks, tv2) == sock2) {
            log("use sock2");
            sock = sock2;
        } else {
            log("use sock1");
        }
    } else if (sock == sock2) {
        log("use sock2");
    } else {
        log("use nothing");
    }
    for (int sock : socks) close(sock);
    return sock;
}

void bind_device(int sock, const char * device)
{
    if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, device, strlen(device)) < 0) {
        perror("SO_BINDTODEVICE failed");
        exit(-1);
    }
}

void add_ip_options(int sock)
{
    unsigned char ip_options[] = {0b10100000, 2, 0b10110000, 2};
    if (setsockopt(sock, IPPROTO_IP, IP_OPTIONS, ip_options, sizeof(ip_options)) < 0) {
        perror("IP_OPTIONS failed");
        exit(-1);
    }
}

int main()
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr("192.168.10.2");
    int sock1 = socket(AF_INET, SOCK_DGRAM, 0);
    int sock2 = socket(AF_INET, SOCK_STREAM, 0);
    // bind_device(sock1, "ipiptun");
    add_ip_options(sock1);
    //std::cout << connect(sock1, (struct sockaddr *)&addr, sizeof(addr)) << get_socketerror(sock1) << std::endl;
    //send(sock1, "hello", 5, 0);
    sendto(sock1, "wogjowigojwe", 12, 0, (struct sockaddr*)&addr, sizeof(addr));
    // bind_device(sock1, "gretun");
    // bind_device(sock2, "eth0");
    // add_ip_options(sock1);
    // add_ip_options(sock2);
    // std::cout << happy_socket((struct sockaddr *)&addr, sock1, sock2, 5, 250e3) << std::endl;
    return 0;
}
