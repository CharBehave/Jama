#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include "socket_handler.h"
#include "SSL_handler.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <unistd.h>
#include <pthread.h>
#include "client_handler.h"
#include "server.h"


void* client_handler(void *vargp);

typedef struct node {
	SSL *client;
	int id;
	int fd;
	struct node *pNext;
	
} clients_t;


void InsertClient(SSL *ssl, int id, int clientFd);

struct node* DeleteClient(int id);

void SendMessageToClients(char* msg);




#endif
