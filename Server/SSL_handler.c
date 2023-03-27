#include <stdio.h>
#include <stdlib.h>
#include "SSL_handler.h"
#include <openssl/ssl.h>

void Configure_context(SSL_CTX *ctx)
{
	//load certificate and private key into SSL_CTX object
	if (SSL_CTX_use_certificate_file(ctx, CERT, SSL_FILETYPE_PEM) <= 0)
	{
		puts("Error using certificate file! exiting");
		exit(EXIT_FAILURE);
	}
	
	if (SSL_CTX_use_PrivateKey_file(ctx, PRIVKEY, SSL_FILETYPE_PEM) <= 0)
	{
		puts("Error using private key file! exiting");
		exit(EXIT_FAILURE);
	}
}


SSL_CTX *create_context()
{
	const SSL_METHOD *method; //constant pointer for sever method
	
	SSL_CTX *ctx; // SSL_CTX object
	
	method = TLS_server_method(); //Transport layer security
	
	ctx = SSL_CTX_new(method); //create new SSC_CTX object with given method
	
	if (!ctx)
	{
		puts("Unable to create SSL context");
		exit(EXIT_FAILURE);
	}
	
	return ctx; //returns pointer to SSL_CTX object
}
