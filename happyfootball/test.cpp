#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/stdc++.h>

int main()
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr("43.131.31.97");
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    unsigned char ip_options[] = {0b10100000, 2, 0b10110000, 2};
    if (setsockopt(sock, IPPROTO_IP, IP_OPTIONS, ip_options, sizeof(ip_options)) < 0) {
        std::cerr << "setsockopt() failed: " << strerror(errno) << std::endl;
        return -1;
    }
    long arg = fcntl(sock, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(sock, F_SETFL, arg);
    int res = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    fd_set myset;
    struct timeval tv;
    tv.tv_sec = 5; // 5 seconds timeout
    tv.tv_usec = 0;
    FD_ZERO(&myset);
    FD_SET(sock, &myset);
    if (select(sock+1, NULL, &myset, NULL, &tv) > 0)
    {
       int so_error;
       socklen_t len = sizeof so_error;
       getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);
       if (so_error == 0) std::cout << "Connected.\n";
       else std::cout << strerror(so_error) << std::endl;
    }
    else std::cout << "Timeout or error.\n";
    arg = fcntl(sock, F_GETFL, NULL);
    arg &= (~O_NONBLOCK);
    fcntl(sock, F_SETFL, arg);
    close(sock);
    return 0;
}