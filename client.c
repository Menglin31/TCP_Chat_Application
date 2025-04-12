#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024
#define NICKNAME_SIZE 32

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char nickname[NICKNAME_SIZE], buffer[BUFFER_SIZE];
    fd_set read_fds;

    // Get nickname
    printf("Enter your nickname: ");
    fgets(nickname, NICKNAME_SIZE, stdin);
    nickname[strcspn(nickname, "\n")] = 0; // Remove newline

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }

    // Send nickname
    write(sock, nickname, strlen(nickname));

    printf("Connected to server. Type your messages:\n");

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(sock, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        // Select
        if (select(sock + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Select failed");
            break;
        }

        // Check for server messages
        if (FD_ISSET(sock, &read_fds)) {
            int bytes_read = read(sock, buffer, BUFFER_SIZE - 1);
            if (bytes_read <= 0) {
                printf("Server disconnected\n");
                break;
            }
            buffer[bytes_read] = '\0';
            printf("%s\n", buffer);
        }

        // Check for user input
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
                break;
            }
            buffer[strcspn(buffer, "\n")] = 0; // Remove newline
            if (strlen(buffer) > 0) {
                write(sock, buffer, strlen(buffer));
            }
        }
    }

    close(sock);
    return 0;
}