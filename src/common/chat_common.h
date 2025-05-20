#ifndef CHAT_COMMON_H
#define CHAT_COMMON_H

#include <netinet/in.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10
#define USERNAME_MAX 32

typedef struct {
  int socket;
  struct sockaddr_in address;
  int id;
  char username[USERNAME_MAX];
} client_t;

#define MSG_TYPE_USERNAME "USERNAME:"
#define MSG_TYPE_PRIVATE "PRIVATE:"
#define MSG_TYPE_GROUP "GROUP:"

#endif /* CHAT_COMMON_H */
