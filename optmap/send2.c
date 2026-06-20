#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "optmap.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <destination IP> <destination opt>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *dest_ip = argv[1];
    int dest_opt = atoi(argv[2]);

    int sock;
    struct sockaddr_in dest;
    char message[] = "Hello, TCP with IP options!";

    // Create a TCP socket.
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct opt_ext ext;
    init_opt_ext(&ext);
    opt_ext_set_daddr(&ext, (__u32)dest_opt);

    if (setsockopt(sock, IPPROTO_IP, IP_OPTIONS, &ext, sizeof(ext)) < 0) {
        perror("setsockopt");
        close(sock);
        return EXIT_FAILURE;
    }

    // Configure destination address.
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(12345); // Change port as needed.
    if (inet_aton(dest_ip, &dest.sin_addr) == 0) {
        fprintf(stderr, "Invalid destination IP address\n");
        close(sock);
        return EXIT_FAILURE;
    }

    // Connect to the server.
    if (connect(sock, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("connect");
        close(sock);
        return EXIT_FAILURE;
    }
    // Send the TCP packet.
    if (send(sock, message, sizeof(message), 0) < 0) {
        perror("send");
        close(sock);
        return EXIT_FAILURE;
    }
    printf("TCP packet sent successfully.\n");

    close(sock);
    return EXIT_SUCCESS;
}