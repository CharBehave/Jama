#include "server.h"
#include "socket_handler.h"
#include "SSL_handler.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <unistd.h>
#include <pthread.h>
#include "client_handler.h"






void* client_handler(void *vargp)
{
	char buf[1024] = {0};
	int bytes;
	
	SSL *ssl = ((SSL *)vargp);
	
	
	
	while ((bytes = SSL_read(ssl, buf, sizeof(buf))) > 0)
	{
		buf[bytes] = '\0';
		
		printf("Client message: %s\n", buf);
		
		if (SSL_write(ssl, buf, strlen(buf)) > 0)
		{
			printf("Sent client message: %s\n", buf);
		}
		
		SSL_write(ssl, buf, strlen(buf));
	}
	
	return NULL;
}
