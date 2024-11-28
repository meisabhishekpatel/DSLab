#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8080  // Server port number

// gcc -o server exp3_1_server.c
// gcc -o client exp3_1_client.c

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[1024];
    socklen_t client_addr_len = sizeof(client_addr);

    // Create a socket (UDP, SOCK_DGRAM)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);  // Convert port number to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY;   // Accept connections from any IP address

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        close(sockfd);
        exit(1);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    // Receive message from client
    ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len);
    if (recv_len < 0) {
        perror("Error receiving message");
        close(sockfd);
        exit(1);
    }

    // Null-terminate the received message and print it
    buffer[recv_len] = '\0';
    printf("Received message from client: %s\n", buffer);

    // Close the socket
    close(sockfd);

    return 0;
}

