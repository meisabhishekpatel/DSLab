#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// gcc -pthread -o server server.c
// gcc -o client client.c

// Function to handle communication with the client
void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    char buffer[BUFFER_SIZE];
    int bytes_received;

    // Receive message from the client
    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0'; // Null-terminate the received string
        printf("Received from client: %s\n", buffer);

        // Echo the message back to the client
        send(sock, buffer, bytes_received, 0);
    }

    if (bytes_received == 0) {
        printf("Client disconnected.\n");
    } else {
        perror("recv failed");
    }

    // Close the socket and free the thread resources
    close(sock);
    free(client_socket);
    return NULL;
}

int main() {
    int server_fd, client_fd, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

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

    // Accept and handle clients concurrently
    while ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) > 0) {
        printf("New connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Allocate memory for the client socket and pass it to the thread
        new_sock = malloc(sizeof(int));
        *new_sock = client_fd;

        // Create a new thread to handle the client
        if (pthread_create(&thread_id, NULL, handle_client, (void *)new_sock) != 0) {
            perror("Thread creation failed");
            free(new_sock);
        }

        // Detach the thread so that resources are freed automatically
        pthread_detach(thread_id);
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

