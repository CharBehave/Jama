#include "server.h"
#include "socket_handler.h"
#include "SSL_handler.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <unistd.h>
#include <pthread.h>
#include "client_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


//global variables
struct node *head = NULL;
struct node *current = NULL;

//Must be done like this at the moment, I would rather gouge my eyes out than
//refactor the whole code
int exitClientId = 0;
int handledClientId = 0;



void* client_handler(void *vargp)
{
	char buf[1024] = {0};
	int bytes;
	
	char* pBuf = buf;
	
	SSL *ssl = ((SSL *)vargp); //get ssl file descriptor from the pthread argument
	
	printf("Thread id: %ld\n", pthread_self());
	
	handledClientId++;
	
	int id = handledClientId;
	
	printf("Handling client with id: %d\n", handledClientId);
	
	
	//While connection is present, wait for message
	while ((bytes = SSL_read(ssl, buf, sizeof(buf))) > 0)
	{
		buf[bytes] = '\0';
		
		
		SendMessageToClients(pBuf);
		
	}
	
	printf("Shutting down thread: %ld\n", pthread_self());
	
	pthread_exit(NULL);
	
}



//Insert client into struct linked list
void InsertClient(SSL *ssl, int id, int clientFd)
{
	
	struct node *link = (struct node*) malloc(sizeof(struct node));
	
	link->client = ssl;
	link->id = id;
	link->fd = clientFd;
	
	
	link->pNext = head;
	
	head = link; 
	
	
}

//Delete given client from linked list, also free memory
struct node* DeleteClient(int ClientId)
{
	
	struct node* current = head;
	struct node* previous = NULL;
	
	if(head == NULL)
	{
		puts("Critical error! Client Delete NULL");
	}
	
	while (current->id != ClientId)
	{
		if(current->pNext == NULL)
		{
			puts("Critical error! Client with this id does not exist!");
			exit(EXIT_FAILURE);
		}
		else
		{
			
			previous = current;
			
			current = current->pNext;
			
		}
		
	}	
	
	
	close(current->fd);
	//SSL_shutdown(current->client);
	SSL_free(current->client);
	
	
	if (current == head)
	{
		head = head->pNext;
	}
	else
	{
		previous->pNext = current->pNext;
	}
			
	exitClientId = ClientId;
	
	free(current);
	current = NULL;
	free(current);
	
	return previous; 
	
}


void SendMessageToClients(char* msg)
{
	struct node* ptr = head;
	
	while(ptr != NULL)
	{
		
		if (SSL_write(ptr->client, msg, strlen(msg)) > 0)
		{
			printf("Sending Client %d : %s", ptr->id, msg);
			
		}
		else
		{
			printf("Error sending Client %d message\n", ptr->id);
			
			//if error writing to certain client, shut the connection
			if (SSL_write(ptr->client, msg, strlen(msg)) <= 0)
			{
				printf("Shutting down connection to Client %d\n", ptr->id);
				ptr = DeleteClient(ptr->id);
				printf("Connection shut down\n");
			}
			
			
		}
		
		
		if (ptr != NULL)
		{
			ptr = ptr->pNext;
		}
		
	}
	
	
	
}
