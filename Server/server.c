#include "server.h"
#include "socket_handler.h"
#include "SSL_handler.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <unistd.h>
#include <pthread.h>
#include "client_handler.h"




int main(void)
{
	int socket;
	SSL_CTX *ctx; //Declaration of SSL_CTX object
	
	
	socket = BindAndListenToPort(8888);
	
	ctx = create_context(); //create SSL_CTX object
	
	Configure_context(ctx); //add cert and key to SSL_CTX object
	SSL *ssl;
	
	puts("SSL context configured successfully, server is up and running!");
	
	int connectedClientId = 0;
	
	while (1)
	{
		struct sockaddr_in addr;
		unsigned int len = sizeof(addr);
		
		//thread id
		pthread_t tid; 
		
		
		puts("Ready to accept next connection");
		

		int client = accept(socket, (struct sockaddr*)&addr, &len);
		

		
		if (client < 0)
		{
			puts("unable to accept client!");
		}
		else
		{
			
			ssl = SSL_new(ctx); //create new SSL structure for TLS/SSL connection
			SSL_set_fd(ssl, client); //set client as the IO facility for encryption
			
		}
		

		
		if (SSL_accept(ssl) <= 0)
		{
			puts("Error accepting TLS/SSL handshake!");
		}
		else
		{
			puts("Handshake accepted!");
			connectedClientId++; 
			
			InsertClient(ssl, connectedClientId, client);
			
			pthread_create(&tid, NULL, client_handler, ssl);
		}
		
		
		
		
	}
	//Should never actually reach here, if it does without CTRL + C command,
	//then it's between you and god
	printf("Closing server!\n");
	
	SSL_free(ssl);
	close(socket);
	SSL_CTX_free(ctx);
}




