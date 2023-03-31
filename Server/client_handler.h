#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "server.h"
#include "socket_handler.h"
#include "SSL_handler.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <unistd.h>
#include <pthread.h>
#include "client_handler.h"

void* client_handler(void *vargp);


typedef struct node {
	SSL *client;
	int id;
	struct node *pNext;
	
} clients_t;


void InsertClient(SSL *ssl, int id);

struct node* DeleteClient(int id);

void SendMessageToClients(char* msg);


#endif
