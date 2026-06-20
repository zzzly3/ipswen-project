//wrapper socket

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "optmap.h"

int socket(int domain, int type, int protocol) {
    static int (*__socket)(int, int, int) = NULL;
    if (__socket == NULL) {
        // printf("socket wrapper\n");
        __socket = (int (*)(int, int, int)) dlsym(RTLD_NEXT, "socket");
        if (__socket == NULL) {
            return -1;
        }
    }
    int sockfd = __socket(domain, type, protocol);
    if (sockfd < 0) {
        return sockfd;
    }

    // set the socket to send IP options
    const char *opt = getenv("DEST_EXT");
    int daddr = 0;
    if (opt) {
        daddr = atoi(opt);
    }
    if (daddr > 0) {
        struct opt_ext ext;
        init_opt_ext(&ext);
        opt_ext_set_daddr(&ext, (__u32)daddr);
        if (setsockopt(sockfd, IPPROTO_IP, IP_OPTIONS, &ext, sizeof(ext)) < 0) {
            perror("setsockopt");
            close(sockfd);
            return -1;
        }
        printf("socket %d, daddr %d\n", sockfd, daddr);
    }

    return sockfd;
}