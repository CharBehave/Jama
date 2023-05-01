
#include "Communicate(01.05).h"

struct node *head = NULL;

int handledClientId = 0;
int exitClientId = 0;
int cancelFlag = 0;
pthread_t threadID1;
pthread_t threadID2;

void handleSignal(int sig) 
{
    if (sig == SIGINT) 
    {
        printf("Signal %d received, cancelling threads...\n", sig);
        
        pthread_cancel(threadID1);
        pthread_cancel(threadID2);
        cancelFlag = 1;
        return;
    }
}

void * NodeServer(void * ssl)
{
    SSL_CTX *ctx2; //SSL_CTX object
    int connectedClient = 0;
    int socketLocal;
    int flag = 0;

    socketLocal = ServerSetup(6221);
    ctx2 = create_context(); //Creating SSL_CTX object
    Configure_context(ctx2);
    puts("SSL_context created!");
    
    SSLData *SSL = NULL;
    SSL = (SSLData*)ssl;
    
    while (1)
    {
        struct sockaddr_in addr;
        unsigned int len = sizeof(addr);

        int client;
            
        client = accept(socketLocal, (struct sockaddr*)&addr, &len);
        
        if (client < 0)
        {
            puts("Accepting client failed!");
        }
        else
        {
            SSL->nodeSSL = SSL_new(ctx2); //Creating new SSL connectin state
            SSL_set_fd(SSL->nodeSSL, client); //set client as the IO facility for encryption
        }
        
        if (SSL_accept(SSL->nodeSSL) <= 0)
        {
            puts("Error accepting TLS/SSL handshake!");
        }
        else
        {
            puts("Handshake accepted!");
            connectedClient++; 
            
            InsertClient(SSL->nodeSSL, connectedClient, client);
        }
 
        puts("Creating read and write threads...");
        
        if (pthread_create(&threadID1, NULL, NodeListenToClient, (void *)SSL) != 0)
        {
            perror("Error creating thread 2!");
            exit(EXIT_FAILURE);
        }
        
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
        
        if (flag != 1)
		{
			flag = 1;
			if (pthread_create(&threadID2, NULL, NodeListenToServer, (void *)SSL) != 0)
			{
				perror("Error creating thread 3!");
				exit(EXIT_FAILURE);
			}
            
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
		}
        
    }

    if (pthread_join(threadID1, NULL) != 0)
    {
        perror("Error joining subsidiary thread 1!");
    }
    if (pthread_join(threadID2, NULL) != 0)
    {
        perror("Error joining subsidiary thread 2!");
    }
    
    
    SSL_CTX_free(ctx2);
    connectedClient = 0;
    close(socketLocal);
    
    return NULL;
}

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

void FreeList(void) 
{
    struct node *temp;
    while (head != NULL) 
    {
        temp = head;
        head = head->pNext;
        if (temp != NULL)
		{
			free(temp);
		}
    }
    head = NULL;
}

void InsertClient(SSL *ssl, int id, int clientFd)
{
	struct node *link = (struct node*) malloc(sizeof(struct node));
	
	link->client = ssl;
	link->id = id;
	link->fd = clientFd;
	
	link->pNext = head;
	
	head = link; 
}

void * ClientRead(void * ssl)
{
    SSLData *SSL = NULL;
    SSL = (SSLData*)ssl;
    int amountOfBytes;
    char message[1024] = {0};
    
    while ((amountOfBytes = SSL_read(SSL->ssl, message, sizeof(message))) > 0)
    {
        //printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));
        //~ //ShowCerts(ssl1);        /* get any certs */
        
        message[amountOfBytes] = '\0';
        
        printf("Recieved in clientRead: %s\n", message);
        
        memset(message, 0, sizeof(message));
        
    }

    amountOfBytes = 0;
    
    if ((amountOfBytes = SSL_read(SSL->ssl, message, sizeof(message))) <= 0)
    {
        puts("Error with SSL_read in clientRead!");
        exit(EXIT_FAILURE);
    }
    
    amountOfBytes = 0;
    
    return NULL;
}


void * ClientCommunicate(void * ssl)
{
    SSLData *SSL = NULL;
    SSL = (SSLData*)ssl;
    char strCheck[6] = "exit\n\0";
    char message[1024] = {0};
    
    while (1)
    {
        //~ //printf("\nEnter message: \n");
        fgets(message, 1024, stdin);

        message[strlen(message) - 1] = '\n';
        message[strlen(message)] = '\0';
        
        if (strcmp(message, strCheck) == 0)
        {
            pthread_exit("Client exited program!");
        }
        
        //printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));
        //~ //ShowCerts(ssl1);        /* get any certs */
        
        int amountOfBytes = SSL_write(SSL->ssl, message, (int)strlen(message));
        
        if (amountOfBytes <= 0)   /* encrypt & send message */
        {
            puts("Could not send message!");
        }
        
        memset(message, 0, sizeof(message));
    }
    
    return NULL;
}
 
 
void * NodeListenToClient(void * ssl)
{
    SSLData *SSL = NULL;
    SSL = (SSLData*)ssl;
    int bytes;
    char buf[1024] = {0};
    int err;
    
	while ((bytes = SSL_read(SSL->nodeSSL, buf, sizeof(buf))) > 0)
	{
		if (bytes >= 1024)
		{
			printf("Client message is too long!\n");
			//break;
		}
		
        buf[bytes] = '\0';
		printf("Received from client: %s\n", buf);
		
        if ((err = SSL_write(SSL->ssl, buf, (int)strlen(buf))) > 0)
		{
			printf("Sending server: %s\n", buf);
		}
		else
		{

			printf("Error sending server message!\n");
			printf("Freeing whole list of clients\n");
			FreeList();
            break;
		}
		
		memset(buf, 0, sizeof(buf));
	}
    
    printf("Shutting down client %ld\n", pthread_self());
    pthread_exit(NULL);
    
    return NULL;
}

void * NodeListenToServer(void * ssl)
{
    SSLData *SSL;
    SSL = (SSLData*)ssl;
    int bytes;
    char buf[1024];
    struct node* ptr = head;
    int write;
    
            
    while ((bytes = SSL_read(SSL->ssl, buf, sizeof(buf))) > 0)
    {
        if (bytes >= 1024)
        {
            printf("Server message is too long!\n");
            //break;
        }
        buf[bytes] = '\0';
        printf("Received from server: %s\n", buf);
        
        while (ptr != NULL)
		{
			if ((write = SSL_write(ptr->client, buf, (int)strlen(buf))) > 0)
	        {
	            printf("Sending client %d: %s\n", ptr->id, buf);
	        }
	        else
	        {
	            printf("Shutting down connection to Client %d\n", ptr->id);
				ptr = DeleteClient(ptr->id);
				printf("Connection shut down\n"); 
	        }
	        
	        ptr = ptr->pNext; 
		}
		
		ptr = head;
        memset(buf, 0, sizeof(buf));
    }
    
    pthread_exit(NULL);
    
    return NULL;
}

