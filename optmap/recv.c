#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF_SIZE 2048
#define CTRL_BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in addr;
    int opt = 1;
    /* Create UDP socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Enable receiving IP options */
    if (setsockopt(sockfd, IPPROTO_IP, IP_RECVOPTS, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(IP_RECVOPTS)");
        /* Note: This option might not be supported on all systems */
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on UDP port 12345...\n");

    while (1) {
        char data_buf[BUF_SIZE];
        char ctrl_buf[CTRL_BUF_SIZE];
        struct iovec iov;
        struct msghdr msg;
        struct sockaddr_in src_addr;
        ssize_t num_bytes;

        iov.iov_base = data_buf;
        iov.iov_len = sizeof(data_buf);

        memset(&msg, 0, sizeof(msg));
        msg.msg_name = &src_addr;
        msg.msg_namelen = sizeof(src_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = ctrl_buf;
        msg.msg_controllen = sizeof(ctrl_buf);

        num_bytes = recvmsg(sockfd, &msg, 0);
        if (num_bytes < 0) {
            perror("recvmsg");
            continue;
        }

        printf("Received %zd bytes from %s:%d\n",
               num_bytes,
               inet_ntoa(src_addr.sin_addr),
               ntohs(src_addr.sin_port));
        
        printf("Data: %s\n", data_buf);

        /* Process ancillary data to extract IP options */
        struct cmsghdr *cmsg;
        for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL;
             cmsg = CMSG_NXTHDR(&msg, cmsg)) {
            if (cmsg->cmsg_level == IPPROTO_IP &&
                cmsg->cmsg_type == IP_RECVOPTS) {

                int opt_len = cmsg->cmsg_len - CMSG_LEN(0);
                unsigned char *ip_options = (unsigned char *) CMSG_DATA(cmsg);
                
                printf("IP Options (%d bytes): ", opt_len);
                for (int i = 0; i < opt_len; i++) {
                    printf("%02x ", ip_options[i]);
                }
                printf("\n");
            }
        }
    }

    close(sockfd);
    return 0;
}