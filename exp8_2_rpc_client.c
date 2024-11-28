#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to receive the file from the server
void receive_file(int server_socket, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("File creation failed");
        return;
    }

    // Receive the file in chunks and write to disk
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    while ((bytes_received = recv(server_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    printf("File received successfully\n");
    fclose(file);
}

// Function to print the contents of the received file
void print_file_contents(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("File opening failed");
        return;
    }

    char ch;
    printf("\nContents of the received file:\n");
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    fclose(file);
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to server\n");

    // Send the filename to the server
    char filename[] = "test.txt";  // File to be transferred
    send(client_socket, filename, strlen(filename) + 1, 0);

    // Receive the file from the server
    receive_file(client_socket, "received_test.txt");

    // Print the contents of the received file
    print_file_contents("received_test.txt");

    // Close the socket
    close(client_socket);

    return 0;
}

