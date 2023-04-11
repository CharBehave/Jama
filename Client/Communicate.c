// TODO
// Function for node server to listen to main server and forward message to 
// clients
// Function for node server to listen to clients and forward message to main
// server


#include "Communicate.h"

struct node *head = NULL;

void FreeList(void) 
{
    struct node *temp;
    while (head != NULL) 
    {
        temp = head;
        head = head->pNext;
        free(temp);
    }
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

void * ClientCommunicate(void * ssl)
{
    SSLData *SSL;
    SSL = (SSLData*)ssl;
    char strCheck[5] = "exit\n";
    int check;
    scanf("%d", &check);
    
    while (check == 2)
    {
        scanf("%d", &check);
        char *message = NULL;
        message = (char*) malloc(1024);

        if (message == NULL)
        {
            perror("memory allocation failed!");
            exit(EXIT_FAILURE);
        }
        
        strcpy(message, "message to server");
        
        if (strcmp(message, strCheck) == 0)
        {
            free(message);
            return NULL;
        }
        
        //printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));
        //~ ShowCerts(ssl1);        /* get any certs */
        
        int amountOfBytes = SSL_write(SSL->nodeSSL, message, (int)strlen(message)) ;
        
        if (amountOfBytes <= 0)   /* encrypt & send message */
        {
            perror("Could not send message!");
        }
        
        free(message);
    }
    return NULL;
}
 
void * NodeListenToClient(void * ssl)
{
    SSLData *SSL;
    SSL = (SSLData*)ssl;
    int bytes;
    char buf[1024];
    struct node* ptr = head;
    
    //~ while (1)
    {
        if ((bytes = SSL_read(ptr->client, buf, sizeof(buf))) > 0) /* get reply & decrypt */
        {
            if (bytes >= 1024)
            {
                printf("Client message is too long!\n");
          //      break;
            }
            buf[bytes] = 0;
            printf("Received from client: %s\n", buf);
        }
        
        if (SSL_write(SSL->ssl, buf, (int)strlen(buf)) > 0)
        {
            printf("Sending server %d: %s\n", ptr->id, buf);
        }
        else
        {
            printf("Error sending server %d message\n", ptr->id);
        
            if (SSL_write(SSL->ssl, buf, (int)strlen(buf)) <= 0)
            {
                printf("Freeing whole list of clients\n");
                FreeList();
            }
        }
            
            if (ptr != NULL)
            {
                ptr = ptr->pNext;
            }
        
        memset(buf, 0, sizeof(buf));
    }
    return NULL;
}

void * NodeListenToServer(void * ssl)
{
    SSLData *SSL;
    SSL = (SSLData*)ssl;
    int bytes;
    char buf[1024];
    struct node* ptr = head;
    
    //~ while (1)
    {
        while ((bytes = SSL_read(SSL->ssl, buf, sizeof(buf))) > 0) /* get reply & decrypt */
        {
            if (bytes >= 1024)
            {
                printf("Server message is too long!\n");
                break;
            }
            buf[bytes] = 0;
            printf("Received from server: %s\n", buf);
        }
        
        while (ptr != NULL)
        {
            if (SSL_write(ptr->client, buf, (int)strlen(buf)) > 0)
            {
                printf("Sending client %d: %s\n", ptr->id, buf);
            }
            else
            {
                printf("Error sending client %d message\n", ptr->id);
			
                if (SSL_write(ptr->client, buf, (int)strlen(buf)) <= 0)
                {
                    printf("Freeing whole list of clients\n");
                    FreeList();
                }
            }
            
            if (ptr != NULL)
            {
                ptr = ptr->pNext;
            }
        }
        memset(buf, 0, sizeof(buf));
    }
    return NULL;
}