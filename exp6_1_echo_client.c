#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Type your message:\n");

    while (1) {
        printf("Enter message: ");
        fgets(message, sizeof(message), stdin);

        // Remove newline character if present
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        // Send message to the server
        send(sock, message, strlen(message), 0);

        // Receive response from the server
        int bytes_received = recv(sock, response, sizeof(response) - 1, 0);
        if (bytes_received > 0) {
            response[bytes_received] = '\0';
            printf("Server echoed: %s\n", response);
        } else {
            perror("Receive failed");
            break;
        }
    }

    // Close the socket
    close(sock);
    return 0;
}

