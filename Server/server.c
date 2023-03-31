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
	
	int clientId;
	
	while (1)
	{
		struct sockaddr_in addr;
		unsigned int len = sizeof(addr);
		SSL *ssl;
		const char reply[] = "test\n";
		//thread id
		pthread_t tid; 
		
		
		puts("Waiting for SSL/TSL connection!");
		
		
		int client = accept(socket, (struct sockaddr*)&addr, &len);
		

		
		if (client < 0)
		{
			puts("unable to accept client! exiting");
			exit(EXIT_FAILURE);
		}
		
		ssl = SSL_new(ctx); //create new SSL structure for TLS/SSL connection
		
		SSL_set_fd(ssl, client); //set client as the IO facility for encryption
		
		if (SSL_accept(ssl) <= 0)
		{
			puts("Error accepting TLS/SSL handshake connection!");
		}
		else
		{
			puts("Handshake accepted!");
			clientId++;
			
			InsertClient(ssl, clientId);
			
			
			pthread_create(&tid, NULL, client_handler, ssl);
			
			
			
		}
		
		
		
		//SSL_shutdown(ssl);
		//SSL_free(ssl);
		//close(client);
	}
	
	close(socket);
	SSL_CTX_free(ctx);
}

