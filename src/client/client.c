#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define SERVER_IP "127.0.0.1"

int client_socket;
char username[USERNAME_MAX];
int running = 1;

void *receive_handler(void *arg) {
    (void) arg;
    char buffer[BUFFER_SIZE];
    int receive_len;
    static int welcome_received = 0;

    while (running) {
        memset(buffer, 0, BUFFER_SIZE);
        receive_len = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (receive_len > 0) {
            if (strncmp(buffer, MSG_TYPE_USERNAME, strlen(MSG_TYPE_USERNAME)) == 0 ) {
                continue;            
            }
            printf("\033[2K\r");
            printf("%s", buffer);

            if (buffer[strlen(buffer) - 1] != '\n') {
                printf("\n");
            }

            if (!welcome_received && strstr(buffer, "Welcome to the Poli Chat") != NULL) {
                welcome_received = 1;
                printf("You can now start chatting. Type 'quit' or 'chao' to exit.\n");
            }
            
            printf("> ");
            fflush(stdout);
        } else if ( receive_len == 0) {
            printf("Server disconnected\n");
            running = 0;
            break;
        } else {
            if (running) {
                perror("recv failed");
                running = 0;
                break;
            }
        }
    }

    return NULL;
}

void handle_signal(int sig) {
    (void)sig;
    running = 0;
    printf("\nDisconnecting from server...\n");
    close(client_socket);
    exit(EXIT_SUCCESS);
}

int connect_to_server(const char *server_ip, int port){
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Could not create socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }

    printf("Connecting to server at %s:%d...\n", server_ip, port);
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Connected to server!\n");
    return 0;
}

void send_username(int socket, const char *username) {
    char buffer[BUFFER_SIZE];
    sprintf(buffer,"%s%s", MSG_TYPE_USERNAME, username);
    send(socket, buffer, strlen(buffer), 0);
}

void client_cleanup(void) {
    printf("Disconnection from server...\n");
    close(client_socket);
}


int main() {
    pthread_t receive_thread;
    char buffer[BUFFER_SIZE];

    signal(SIGINT, handle_signal);

    printf("Enter your username (max %d characters):", USERNAME_MAX - 1);
    fgets(username, USERNAME_MAX, stdin);

    size_t username_len = strlen(username);
    if (username[username_len - 1] == '\n') {
        username[username_len - 1] = '\0';
        username_len--;
    }

    if (username_len < 2) {
        printf("Username too short. Please use at least 2 characters.\n");
        return 1;
    }

    if (connect_to_server(SERVER_IP, PORT) < 0) {
        return 1;
    }

    send_username(client_socket, username);

    if (pthread_create(&receive_thread, NULL, receive_handler, NULL) != 0) {
        perror("Failed to create receive thread");
        close(client_socket);
        return 1;
    }

    pthread_detach(receive_thread);

    sleep(1);
    
    printf("> ");
    fflush(stdout);
    
    while (running) {
        memset(buffer, 0, BUFFER_SIZE);

        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        if (strncmp(buffer, "chao", 4) == 0 || strncmp(buffer, "bye", 3) == 0) {
            running = 0;
            break;
        }

        printf("\033[1A");
        printf("\033[2K");

        send(client_socket, buffer, strlen(buffer), 0);
    }
    client_cleanup();

    return 0;
}
