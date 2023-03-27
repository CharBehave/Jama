#ifndef SSL_HANDLER_H
#define SSL_HANDLER_H

#define CERT "cert.pem"
#define PRIVKEY "key.pem"

#include <openssl/ssl.h>

void Configure_context(SSL_CTX *ctx);

SSL_CTX *create_context();

#endif
