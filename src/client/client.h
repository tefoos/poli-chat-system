#ifndef CLIENT_H
#define CLIENT_H

#include "../common/chat_common.h"

void *receive_handler(void *arg);
void handle_signal(int sig);
int connect_to_server(const char *server_ip, int port);
void send_username(int socket, const char *username);
void client_cleanup(void);

#endif
