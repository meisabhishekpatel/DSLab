#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to handle file transfer
void send_file(int client_socket, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("File not found");
        return;
    }

    // Read file and send to client in chunks
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    printf("File transfer completed\n");
    fclose(file);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed");
        exit(1);
    }

    // Listen for client connections
    if (listen(server_socket, 10) == 0) {
        printf("Server listening on port %d\n", PORT);
    } else {
        perror("Listen failed");
        exit(1);
    }

    // Accept a connection from the client
    addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
    if (client_socket < 0) {
        perror("Client connection failed");
        exit(1);
    }

    printf("Connection established with client\n");

    // Receive the filename from the client
    char filename[BUFFER_SIZE];
    recv(client_socket, filename, sizeof(filename), 0);
    printf("Client requested file: %s\n", filename);

    // Call the function to send the file
    send_file(client_socket, filename);

    // Close the client and server sockets
    close(client_socket);
    close(server_socket);

    return 0;
}

