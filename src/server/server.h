#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#define PORT 5555
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct{
  int socket;
  struct sockaddr_in  address;
  int id;
  char username[32];
} client_t;

void *handle_client(void *arg);
void add_client(client_t *client);
void remove_client(int id);
int start_server(void);

#endif /* SERVER_H */
