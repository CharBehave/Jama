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
	
	char* pBuf = buf;
	
	SSL *ssl = ((SSL *)vargp);
	
	
	
	while ((bytes = SSL_read(ssl, buf, sizeof(buf))) > 0)
	{
		buf[bytes] = '\0';
		
		printf("Client message: %s\n", buf);
		
		SendMessageToClients(pBuf);
		
		if (SSL_write(ssl, buf, strlen(buf)) > 0)
		{
			printf("Sent client message: %s\n", buf);
		}
		
		SSL_write(ssl, buf, strlen(buf));
	}
	
	return NULL;
}


void InsertClient(SSL *ssl, int id)
{
	
	struct node *link = (struct node*) malloc(sizeof(struct node));
	
	link->client = ssl;
	link->id = id;
	
	
	link->pNext = head;
	
	head = link; 
	
	
}



struct node* DeleteClient(int ClientId)
{
	
	struct node* current = head;
	struct node* previous = NULL;
	
	if(head == NULL)
	{
		puts("Critical error! Client Delete NULL");
		return NULL;
	}
	
	while (current->id != ClientId)
	{
		if(current->pNext == NULL)
		{
			puts("Critical error! Client with this id does not exist!");
			return NULL;
		}
		else
		{
			
			previous = current;
			
			current = current->pNext;
			
		}
		
		
		if (current == head)
		{
			head = head->pNext;
		}
		else
		{
			previous->pNext = current->pNext;
		}
		
	}
	
	return current;
}


void SendMessageToClients(char* msg)
{
	struct node* ptr = head;
	
	while(ptr != NULL)
	{
		printf("Sending Client %d : %s", ptr->id, msg);
		
		SSL_write(ptr->client, msg, strlen(msg));
		ptr = ptr->pNext;
	}
	
	printf("Message '%s' sent to all clients\n", msg);
	
	
}
