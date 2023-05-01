#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>
#include "Communicate(01.05).h"

#define HOSTNAME argv[1]

typedef struct
{
    SSL* nodeSSL;
    SSL* ssl;
} SSLData;

void * ClientRead(void * ssl);
void * ClientCommunicate(void * ssl);
void handleSignalMain(int sig); 

#endif
