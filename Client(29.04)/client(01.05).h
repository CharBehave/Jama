#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <pthread.h>
#include "Communicate.h"

#define HOSTNAME argv[1]

typedef struct
{
    SSL* nodeSSL;
    SSL* ssl;
} SSLData;

void handleSignalMain(int sig); 

#endif
