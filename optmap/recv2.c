#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "optmap.h"

#define BUF_SIZE 2048
#define CTRL_BUF_SIZE 1024

int main(void) {
    int sockfd;
    struct sockaddr_in addr;
    int opt = 1;
    /* Create TCP socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
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

    if (listen(sockfd, 5) < 0) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Listening on TCP port 12345...\n");

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_sock < 0) {
        perror("accept");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    
    char data_buf[BUF_SIZE];
    if (recv(client_sock, data_buf, sizeof(data_buf), 0) < 0) {
        perror("recv");
        close(client_sock);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Received data: %s\n", data_buf);
    close(client_sock);
    close(sockfd);
    return 0;
}