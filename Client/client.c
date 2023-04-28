#include "client.h"
#include "Communicate.h"
#include "ConnectToServer.h"
#include "SSL_Handler.h"

/*
 * 1.Connect to server
 * 2."Do you wish to be NodeServer"
 * 3.Create thread clientCommunicate
 * 4.Create node server, if wanted
 */




    SSLData *dataFD = NULL;

int main(int argc, char *argv[])
{
	dataFD = (SSLData*)malloc(sizeof(SSLData));
	dataFD->nodeSSL = NULL;
	dataFD->ssl = NULL;
	
	
    int nodeServerInitialized = 0;
    
    if (argc != 3)
    {
        printf("usage: %s <hostname> <portnum>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    char c;
    
    puts("Do you wish to be NodeServer? Yes - Y/No - N");
    scanf("%c", &c);
    
    pthread_t threadID1;
    pthread_t threadID2;
    pthread_t clientCom;
    int connectedClient = 0;
    int socketLocal;
    int client;
    SSL_CTX *ctx2; //SSL_CTX object

    if (c == 'Y')
    {
        socketLocal = ServerSetup(6221);
        
        nodeServerInitialized = 1;
        
        ctx2 = create_context(); //Creating SSL_CTX object
        Configure_context(ctx2);
        
        puts("SSL_context created!");
        
        struct sockaddr_in addr;
        unsigned int len = sizeof(addr);
        
        puts("Waiting to accept client");
        
        client = accept(socketLocal, (struct sockaddr*)&addr, &len);
        
        if (client < 0)
        {
            puts("Accepting client failed!");
        }
        else
        {
            dataFD->nodeSSL = SSL_new(ctx2); //Creating new SSL connectin state
            SSL_set_fd(dataFD->nodeSSL, client); //set client as the IO facility for encryption
        }
        
        if (SSL_accept(dataFD->nodeSSL) <= 0)
		{
			puts("Error accepting TLS/SSL handshake!");
		}
		else
		{
			puts("Handshake accepted!");
			connectedClient++; 
            
            InsertClient(dataFD->nodeSSL, connectedClient, client);
            
            puts("Creating subsidiary threads...");
        }
    }
    
    SSL_CTX *ctx1;
    int socket;
    char *hostname, *portnum;
    
    SSL_library_init();
    hostname = argv[1];
    portnum = argv[2];
    ctx1 = InitCTX();
    socket = ConnectServer(hostname, atoi(portnum));
    dataFD->ssl = SSL_new(ctx1);      /* create new SSL connection state */
    SSL_set_fd(dataFD->ssl, socket);    /* attach the socket descriptor */
    
    int ret = SSL_connect(dataFD->ssl);
    
    if (ret == 0)   /* perform the connection */
    {
        perror("SSL_connect SSL/TLS handshake failed!");
        SSL_free(dataFD->ssl);        /* release connection state */
        SSL_CTX_free(ctx1);        /* release context */
        printf("Exiting program!\n");
        close(socket);
    }    
    else if (ret == -1)
    {
        int err = SSL_get_error(dataFD->ssl, ret);
        switch (err) 
        {
            case SSL_ERROR_NONE:
                printf("No error\n");
                break;
            case SSL_ERROR_SSL:
                printf("SSL library error\n");
                ERR_print_errors_fp(stderr);
                break;
            case SSL_ERROR_WANT_READ:
                printf("SSL wants to read more data\n");
                break;
            case SSL_ERROR_WANT_WRITE:
                printf("SSL wants to write more data\n");
                break;
            case SSL_ERROR_SYSCALL:
                printf("System call error\n");
                break;
            case SSL_ERROR_ZERO_RETURN:
                printf("SSL connection closed\n");
                break;
            default:
                printf("Unknown SSL error\n");
                break;
        }
        SSL_free(dataFD->ssl);        /* release connection state */
        SSL_CTX_free(ctx1);        /* release context */
        printf("Exiting program!\n");
        close(socket);
        exit(EXIT_FAILURE);
    }
    else
    {
        if (nodeServerInitialized != 1)
        {
            if (pthread_create(&clientCom, NULL, ClientCommunicate, (void *)dataFD) != 0)
            {
                perror("Error creating clientCom thread!");
                exit(EXIT_FAILURE);
            }
            else
            {
                puts("Entering ClientCommunicate...");
            }
            
            if (pthread_join(clientCom, NULL) != 0)
            {
                perror("Error joining clientCom thread!");
            }
        }
        else
        {
            if (pthread_create(&threadID1, NULL, NodeListenToClient, (void *)dataFD) != 0)
            {
                perror("Error creating thread 2!");
                exit(EXIT_FAILURE);
            }
            if (pthread_create(&threadID2, NULL, NodeListenToServer, (void *)dataFD) != 0)
            {
                perror("Error creating thread 3!");
                exit(EXIT_FAILURE);
            }
            
            if (pthread_join(threadID1, NULL) != 0)
            {
                perror("Error joining subsidiary thread 1!");
            }
            if (pthread_join(threadID2, NULL) != 0)
            {
                perror("Error joining subsidiary thread 2!");
            }
        }
    }
    
    FreeList();
    printf("Closing server!\n");
    SSL_free(dataFD->nodeSSL);
    close(socketLocal);
    SSL_CTX_free(ctx2);
    
    SSL_free(dataFD->ssl);        /* release connection state */
    SSL_CTX_free(ctx1);        /* release context */
	printf("Exiting program!\n");
    close(socket);

    return 0;
}

