
#include "Communicate.h"

struct node *head = NULL;

void * NodeServer(void * ssl)
{
    SSL_CTX *ctx2; //SSL_CTX object
    int connectedClient = 0;
    int socketLocal;

    socketLocal = ServerSetup(6221);
    ctx2 = create_context(); //Creating SSL_CTX object
    Configure_context(ctx2);
    puts("SSL_context created!");
    
    SSLData *SSL = NULL;
    SSL = (SSLData*)ssl;

    pthread_t threadID1;
    pthread_t threadID2;
    
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
        if (pthread_create(&threadID2, NULL, NodeListenToServer, (void *)SSL) != 0)
        {
            perror("Error creating thread 3!");
            exit(EXIT_FAILURE);
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
}

void FreeList(void) 
{
    struct node *temp;
    while (head != NULL) 
    {
        temp = head;
        head = head->pNext;
        free(temp);
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
    //~ char strCheck[5] = "exit\0";
    int amountOfBytes;
    char message[1024] = {0};
    
    while ((amountOfBytes = SSL_read(SSL->ssl, message, sizeof(message))) > 0)
    {
        //printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));
        //~ ShowCerts(ssl1);        /* get any certs */
        
        message[amountOfBytes] = '\0';
        
        printf("Recieved in clientRead: %s\n", message);
        
        //~ if (strcmp(message, strCheck) == 0)
        //~ {
            //~ break;
        //~ }
        
        memset(message, 0, sizeof(message));
        
    }

    if ((amountOfBytes = SSL_read(SSL->ssl, message, sizeof(message))) <= 0)
    {
        puts("Error with SSL_read in clientRead!");
        exit(EXIT_FAILURE);
    }
    
    return NULL;
}


void * ClientCommunicate(void * ssl)
{
    SSLData *SSL = NULL;
    SSL = (SSLData*)ssl;
    //~ char strCheck[5] = "exit\0";
    char message[1024] = {0};
    
    while (1)
    {
        
        //~ printf("\nEnter message: \n");
        scanf("%s", message);
        
        message[strlen(message)] = '\0';
        
        //~ if (strcmp(message, strCheck) == 0)
        //~ {
            //~ break;
        //~ }
        
        //printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));
        //~ ShowCerts(ssl1);        /* get any certs */
        
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
    struct node* ptr = head;
    
    while (ptr != NULL)
    {
            
        while ((bytes = SSL_read(ptr->client, buf, sizeof(buf))) > 0)
        {
            if (bytes >= 1024)
            {
                printf("Client message is too long!\n");
                //break;
            }
            buf[bytes] = '\0';
            printf("Received from client: %s\n", buf);
        
            if (SSL_write(SSL->ssl, buf, (int)strlen(buf)) > 0)
            {
                printf("Sending server: %s\n", buf);
            }
            else
            {
                printf("Error sending server %d message\n", ptr->id);
                printf("Freeing whole list of clients\n");
                FreeList();
            }
            
            memset(buf, 0, sizeof(buf));
        }
        
        if (ptr != NULL)
        {
            ptr = ptr->pNext;
        }
        else
        {
            break;
        }
    }
    
    ptr = head;
    
    return NULL;
}

void * NodeListenToServer(void * ssl)
{
    SSLData *SSL;
    SSL = (SSLData*)ssl;
    int bytes;
    char buf[1024];
    struct node* ptr = head;
    
            
    while ((bytes = SSL_read(SSL->ssl, buf, sizeof(buf))) > 0)
    {
        if (bytes >= 1024)
        {
            printf("Server message is too long!\n");
            //break;
        }
        buf[bytes] = '\0';
        printf("Received from server: %s\n", buf);
        
        if (SSL_write(ptr->client, buf, (int)strlen(buf)) > 0)
        {
            printf("Sending client %d: %s\n", ptr->id, buf);
        }
        else
        {
            printf("Error sending client %d message\n", ptr->id);
            printf("Freeing whole list of clients\n");
            FreeList();
        }
    
        if (ptr != NULL)
        {
            ptr = ptr->pNext;
        }
        
        memset(buf, 0, sizeof(buf));
    }
        
    
    ptr = head;
    
    return NULL;
}
