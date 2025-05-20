#ifndef SERVER_H
#define SERVER_H

#include "../common/chat_common.h"


void *handle_client(void *arg);
void add_client(client_t *client);
void remove_client(int id);
void broadcast_message(const char *message, int sender_id, int exclude_sender);
int start_server(void);

#endif /* SERVER_H */
