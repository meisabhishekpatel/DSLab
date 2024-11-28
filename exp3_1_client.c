#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"  // Server IP address (localhost for testing)
#define SERVER_PORT 8080       // Server port number

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char message[] = "Hello, Server!";  // Message to be sent to the server

    // Create a socket (UDP, SOCK_DGRAM)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);  // Convert port number to network byte order
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);  // Convert IP address to network byte order

    // Send message to server using sendto()
    ssize_t sent_len = sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sent_len < 0) {
        perror("Error sending message");
        close(sockfd);
        exit(1);
    }

    printf("Message sent to server: %s\n", message);

    // Close the socket
    close(sockfd);

    return 0;
}

