#ifndef COMMUNICATE_H
#define COMMUNICATE_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "client(01.05).h"
#include "ConnectToServer.h"
#include "SSL_Handler.h"

struct node
{
    SSL *client;
    int id;
    int fd; 
    struct node *pNext;
};

void handleSignal(int sig);
struct node* DeleteClient(int ClientId);
void * ClientRead(void * ssl);
void * NodeServer(void * ssl);
void * ClientCommunicate(void * ssl);
void FreeList(void);
void InsertClient(SSL *ssl, int id, int clientFd);
void * NodeListenToClient(void * ssl);
void * NodeListenToServer(void * ssl);


#endif

