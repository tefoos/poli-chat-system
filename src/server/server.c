#include "server.h"
#include <bits/pthreadtypes.h>
#include <bits/types/struct_iovec.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int start_server(void) {
  int server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  pthread_t tid;
  socklen_t client_size;

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("Failed to set socket options");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("Failed to bind socket");
    exit(EXIT_FAILURE);
  }

  if (listen(server_socket, 5) < 0) {
    perror("Failed to listen for connections");
    exit(EXIT_FAILURE);
  }

  printf("Server started on port %d\n", PORT);

  for (int i = 0; i < MAX_CLIENTS; i++) {
    clients[i] = NULL;
  }

  while (1) {
    client_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_size);

    if (client_socket < 0) {
      perror("Failed to accept connection");
      continue;
    }

    client_t *client = (client_t *)malloc(sizeof(client_t));
    client->socket = client_socket;
    client->address = client_addr;
    client->id = client_socket;
    strncpy(client->username, "PoliAnonimoTemp", sizeof(client->username) - 1);
    client->username[sizeof(client->username) - 1] = '\0';

    add_client(client);




    int thread_result =  pthread_create(&tid, NULL, &handle_client, (void*)client);
    if(thread_result != 0) {
      printf("ERROR: Failed to creat thread:%s\n", strerror(thread_result));
      remove_client(client->id);
      free(client);
      close(client_socket);
      continue;
    }
    pthread_detach(tid);
  }
  close(server_socket);
  return 0;
} 

void *handle_client(void *arg) {
  client_t *client = (client_t*)arg;
  char buffer[BUFFER_SIZE];
  char temp_buffer[BUFFER_SIZE];
  int bytes_received;
  int username_set = 0;

  while ((bytes_received = recv(client->socket,buffer, BUFFER_SIZE, 0)) > 0) {
    buffer[bytes_received] = '\0';

    if (strncmp(buffer, MSG_TYPE_USERNAME, strlen(MSG_TYPE_USERNAME)) == 0 && !username_set) {
      char *new_username = buffer + strlen(MSG_TYPE_USERNAME);

      size_t len = strlen(new_username);
      if (len > 0 && new_username[len - 1] == '\n') {
        new_username[len - 1] = '\0';
        len--;
      }

      if (len > 0) {
        strncpy(client->username, new_username, sizeof(client->username)- 1);
        client->username[sizeof(client->username) - 1] = '\0';
        username_set = 1;

        snprintf(temp_buffer, BUFFER_SIZE, "Hi %s!! Welcome to the Poli Chat!!\n", client->username);
        send(client->socket, buffer, strlen(buffer), 0);

        printf("User \"%s\" connected (ID: %d)\n", client->username, client->id);

        snprintf(temp_buffer, BUFFER_SIZE, "User \"%s\" has joined the chat\n", client->username);
        broadcast_message(buffer, client->id, 0);

        continue;
      }
    }

    printf("Message from client %d (%s): %s", client->id, client->username, buffer);

    //TODO: Process command and send message to other clients

    char temp_buffer[BUFFER_SIZE];
    snprintf(temp_buffer, BUFFER_SIZE, "[Server]: %.*s", BUFFER_SIZE - 11 ,buffer);
    send(client->socket, temp_buffer, strlen(temp_buffer), 0);
  }

  printf("User \"%s\" (ID: %d) disconnected\n", client->username, client->id);
  snprintf(temp_buffer, BUFFER_SIZE, "User \"%s\" has left the chat\n", client->username);
  broadcast_message(temp_buffer, client->id, 1);

  close(client->socket);
  remove_client(client->id);
  free(client);

  return NULL;
}

void add_client(client_t *client) {
  pthread_mutex_lock(&clients_mutex);

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i] == NULL) {
      clients[i] = client;
      break;
    }

  }

  pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int id) {
  pthread_mutex_lock(&clients_mutex);

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i] && clients[i]->id == id) {
      clients[i] = NULL;
      break;
    }
  }

  pthread_mutex_unlock(&clients_mutex);
}


void broadcast_message(const char *message, int sender_id, int exclude_sender) {
  pthread_mutex_lock(&clients_mutex);

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i]) {
      if (exclude_sender && clients[i]->id == sender_id) {
        continue;
      }
      send(clients[i]->socket, message, strlen(message), 0);
    }
  }
  pthread_mutex_unlock(&clients_mutex);
}
