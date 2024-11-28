#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090  // Load balancer port
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    
    // Create socket for client
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address (load balancer)
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);  // Load balancer's port
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Localhost IP

    // Connect to the load balancer server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Enter a lowercase string: ");
    fgets(buffer, sizeof(buffer), stdin);

    // Remove the newline character if present
    buffer[strcspn(buffer, "\n")] = '\0';

    // Send the lowercase string to the load balancer
    send(sock, buffer, strlen(buffer), 0);

    // Receive the uppercase string from the load balancer
    int len = recv(sock, buffer, sizeof(buffer), 0);
    buffer[len] = '\0';  // Null-terminate the string

    printf("Received uppercase string: %s\n", buffer);

    // Close the socket
    close(sock);
    return 0;
}



/*************************************************************/
// Q1 : implementing the TCP client-server system with a load balancer (Client)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Enter a string: ");
    fgets(input, BUFFER_SIZE, stdin);
    input[strcspn(input, "\n")] = '\0';
    send(sock, input, strlen(input), 0);

    read(sock, buffer, BUFFER_SIZE);
    printf("Uppercase from server: %s\n", buffer);

    close(sock);
    return 0;
}



/************************* Server 1********************/

// Q1 : implementing the TCP client-server system with a load balancer (Server)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    if (port <= 0) {
        fprintf(stderr, "Invalid port number.\n");
        exit(EXIT_FAILURE);
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE] = {0};
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", port);
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }

        read(new_socket, buffer, BUFFER_SIZE);
        printf("Received: %s\n", buffer);

        to_uppercase(buffer);

        send(new_socket, buffer, strlen(buffer), 0);
        printf("Sent: %s\n", buffer);

        close(new_socket);
    }

    close(server_fd);
    return 0;
}



/************************* Server 2********************/
// Q1 : implementing the TCP client-server system with a load balancer (Load balancer)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int get_cpu_load(const char *ip, int port) {
    return rand() % 100;
}

void forward_message(const char *ip, int port, const char *message, char *response) {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_address.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        perror("Connection to server failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    send(sock, message, strlen(message), 0);

    read(sock, buffer, BUFFER_SIZE);
    strcpy(response, buffer);
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server1_port> <server2_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server1_port = atoi(argv[1]);
    int server2_port = atoi(argv[2]);

    if (server1_port <= 0 || server2_port <= 0) {
        fprintf(stderr, "Invalid port numbers.\n");
        exit(EXIT_FAILURE);
    }

    int load_balancer_sock, client_sock;
    struct sockaddr_in load_balancer_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[BUFFER_SIZE] = {0};
    const char *server_ip = "127.0.0.1";

    if ((load_balancer_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    load_balancer_address.sin_family = AF_INET;
    load_balancer_address.sin_addr.s_addr = INADDR_ANY;
    load_balancer_address.sin_port = htons(8080);

    if (bind(load_balancer_sock, (struct sockaddr *) &load_balancer_address, sizeof(load_balancer_address)) < 0) {
        perror("Bind failed");
        close(load_balancer_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(load_balancer_sock, 5) < 0) {
        perror("Listen failed");
        close(load_balancer_sock);
        exit(EXIT_FAILURE);
    }

    printf("Load balancer listening on port 8080...\n");

    while (1) {
        client_sock = accept(load_balancer_sock, (struct sockaddr *) &client_address, &client_address_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        read(client_sock, buffer, BUFFER_SIZE);
        printf("Received message from client: %s\n", buffer);

        int cpu_load1 = get_cpu_load(server_ip, server1_port);
        int cpu_load2 = get_cpu_load(server_ip, server2_port);
        printf("CPU Load - Server 1: %d%%, Server 2: %d%%\n", cpu_load1, cpu_load2);

        int selected_port = (cpu_load1 <= cpu_load2) ? server1_port : server2_port;
        printf("Forwarding message to server on port %d...\n", selected_port);

        char response[BUFFER_SIZE] = {0};
        forward_message(server_ip, selected_port, buffer, response);
        send(client_sock, response, strlen(response), 0);
        close(client_sock);
    }

    close(load_balancer_sock);
    return 0;
}
