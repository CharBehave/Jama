#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "client.h"
#include "ConnectToServer.h"
#include "SSL_Handler.h"

struct node
{
    SSL *client;
    int id;
    int fd; 
    struct node *pNext;
};

void * ClientRead(void * ssl);
void * NodeServer(void * ssl);
void * ClientCommunicate(void * ssl);
void FreeList(void);
void InsertClient(SSL *ssl, int id, int clientFd);
void * NodeListenToClient(void * ssl);
void * NodeListenToServer(void * ssl);


#endif
