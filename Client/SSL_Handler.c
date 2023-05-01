#include "SSL_Handler.h"

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
	
	SSL_CTX *ctx2; // SSL_CTX object
	
	method = TLS_server_method(); //Transport layer security
	
	ctx2 = SSL_CTX_new(method); //create new SSC_CTX object with given method
	
	if (!ctx2)
	{
		puts("Unable to create SSL context");
		exit(EXIT_FAILURE);
	}
	
	return ctx2; //returns pointer to SSL_CTX object
}

SSL_CTX* InitCTX(void)
{
    const SSL_METHOD *method;
    SSL_CTX *ctx1;
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLS_client_method();  /* Create new client-method instance */
    ctx1 = SSL_CTX_new(method);   /* Create new context */
    if (ctx1 == NULL)
    {
        perror("Error creating context!");
    }
    return ctx1;
}

void ShowCerts(SSL* ssl)
{
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}
