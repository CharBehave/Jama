#ifndef CONNECT_TO_SERVER_H
#define CONNECT_TO_SERVER_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int ConnectServer(char *hostname, int port);
int ServerSetup(int port);

#endif
