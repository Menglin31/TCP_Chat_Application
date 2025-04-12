#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define NICKNAME_SIZE 32

void broadcast(int sender_fd, char *message, int *client_fds, char *nicknames[], int max_fd);

int main() {
    int server_fd, new_socket, client_fds[MAX_CLIENTS] = {0};
    char *nicknames[MAX_CLIENTS] = {0};
    struct sockaddr_in server_addr, client_addr;
    fd_set read_fds;
    char buffer[BUFFER_SIZE];
    int max_fd, addr_len = sizeof(client_addr);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Allow port reuse
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Clear and set FD set
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        max_fd = server_fd;

        // Add client sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] > 0) {
                FD_SET(client_fds[i], &read_fds);
                if (client_fds[i] > max_fd) max_fd = client_fds[i];
            }
        }

        // Select
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Select failed");
            continue;
        }

        // Check for new connection
        if (FD_ISSET(server_fd, &read_fds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) < 0) {
                perror("Accept failed");
                continue;
            }

            // Receive nickname
            char nickname[NICKNAME_SIZE] = {0};
            int bytes_read = read(new_socket, nickname, NICKNAME_SIZE - 1);
            if (bytes_read <= 0) {
                close(new_socket);
                continue;
            }

            // Find free slot
            int i;
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == 0) {
                    client_fds[i] = new_socket;
                    nicknames[i] = strdup(nickname);
                    printf("New connection: %s\n", nicknames[i]);
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                printf("Max clients reached\n");
                close(new_socket);
                continue;
            }

            // Broadcast join message
            char join_msg[BUFFER_SIZE];
            snprintf(join_msg, BUFFER_SIZE, "%s joined the chat!", nickname);
            broadcast(new_socket, join_msg, client_fds, nicknames, MAX_CLIENTS);
        }

        // Check for client messages
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];
            if (fd > 0 && FD_ISSET(fd, &read_fds)) {
                int bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
                if (bytes_read <= 0) {
                    // Client disconnected
                    printf("Disconnected: %s\n", nicknames[i]);
                    char leave_msg[BUFFER_SIZE];
                    snprintf(leave_msg, BUFFER_SIZE, "%s left the chat!", nicknames[i]);
                    broadcast(fd, leave_msg, client_fds, nicknames, MAX_CLIENTS);
                    close(fd);
                    free(nicknames[i]);
                    nicknames[i] = NULL;
                    client_fds[i] = 0;
                } else {
                    // Broadcast message
                    buffer[bytes_read] = '\0';
                    char message[BUFFER_SIZE];
                    snprintf(message, BUFFER_SIZE, "%s: %s", nicknames[i], buffer);
                    broadcast(fd, message, client_fds, nicknames, MAX_CLIENTS);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}

void broadcast(int sender_fd, char *message, int *client_fds, char *nicknames[], int max_fd) {
    for (int i = 0; i < max_fd; i++) {
        if (client_fds[i] > 0 && client_fds[i] != sender_fd) {
            write(client_fds[i], message, strlen(message));
        }
    }
}
