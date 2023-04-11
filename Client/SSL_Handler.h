#ifndef SSL_HANDLER_H
#define SSL_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define CERT "cert.pem"
#define PRIVKEY "key.pem"

#define FAIL -1

SSL_CTX* InitCTX(void);
void ShowCerts(SSL* ssl);
void Configure_context(SSL_CTX *ctx);
SSL_CTX *create_context();

#endif
