#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/ssl.h>

void* serverCloseHandler(void* vargp);

#endif
