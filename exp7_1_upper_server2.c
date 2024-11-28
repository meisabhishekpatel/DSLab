#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 8080  // Change this for the second server

// Function to convert a lowercase string to uppercase
void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);  // Server's port

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 10) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client connections and process them
    while ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) > 0) {
        printf("Connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Receive the lowercase string from the client
        int len = recv(client_fd, buffer, sizeof(buffer), 0);
        if (len > 0) {
            buffer[len] = '\0';  // Null-terminate the string
            printf("Received string: %s\n", buffer);

            // Convert the string to uppercase
            to_uppercase(buffer);

            // Send the uppercase string back to the client
            send(client_fd, buffer, strlen(buffer), 0);
        }

        // Close the connection with the client
        close(client_fd);
    }

    if (client_fd < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Close the server socket
    close(server_fd);
    return 0;
}

