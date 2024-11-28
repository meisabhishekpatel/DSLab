#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

// Structure to hold client information (Node Number, IP Address, Port Number)
typedef struct {
    int node_number;
    char ip_address[INET_ADDRSTRLEN];
    int port;
} NodeInfo;

// Function to receive and display the master table
void receive_and_display_master_table(int sock) {
    int node_count;
    recv(sock, &node_count, sizeof(int), 0); // Receive the number of nodes

    NodeInfo node;
    printf("Updated Master Table:\n");
    printf("Node Number | IP Address | Port Number\n");

    // Receive and print each node's information
    for (int i = 0; i < node_count; i++) {
        recv(sock, &node, sizeof(NodeInfo), 0);
        printf("%d          | %s      | %d\n", node.node_number, node.ip_address, node.port);
    }
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int node_number = 1;  // Example node number, can be dynamic
    int port = 8081;      // Example port for the client
    char *ip_address = "127.0.0.1"; // Example IP address

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);  // Port should be the same as the server
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Send client information to the server (Node Number, IP Address, Port Number)
    snprintf(buffer, sizeof(buffer), "%d %s %d", node_number, ip_address, port);
    send(sock, buffer, strlen(buffer), 0);

    // Receive and display the updated master table from the server
    receive_and_display_master_table(sock);

    // Close the socket
    close(sock);
    return 0;
}

