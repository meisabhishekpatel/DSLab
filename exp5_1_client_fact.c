#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    int client_fd;
    struct sockaddr_in server_addr;
    int number;
    unsigned long long result;

    // Create socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);  // Server port

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Get input number from user
    printf("Enter an integer to calculate its factorial: ");
    scanf("%d", &number);

    // Send the number to the server
    send(client_fd, &number, sizeof(number), 0);
    printf("Sent number %d to server.\n", number);

    // Receive the factorial result from the server
    recv(client_fd, &result, sizeof(result), 0);
    printf("Received factorial result: %llu\n", result);

    // Close the connection
    close(client_fd);

    return 0;
}

