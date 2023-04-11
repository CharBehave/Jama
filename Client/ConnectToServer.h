#ifndef CONNECT_TO_SERVER_H
#define CONNECT_TO_SERVER_H

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

int ConnectServer(char *hostname, int port);
int ServerSetup(int port);

#endif
