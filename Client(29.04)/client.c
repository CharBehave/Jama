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
    if (argc != 3)
    {
        printf("usage: %s <hostname> <portnum>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
	dataFD = (SSLData*)malloc(sizeof(SSLData));
	dataFD->nodeSSL = NULL;
	dataFD->ssl = NULL;
	
    char c;
    pthread_t nodeServer;
    pthread_t clientCom;
    pthread_t clientComRead;
    
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
    
    puts("Do you wish to be NodeServer? Yes - Y/No - N");
    scanf("%c", &c);    
    

    if (c != 'Y')
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
        if (pthread_create(&clientComRead, NULL, ClientRead, (void *)dataFD) != 0)
        {
            perror("Error creating clientRead thread!");
            exit(EXIT_FAILURE);
        }
        else
        {
            puts("Entering ClientRead...");
        }
    }
    else if (c == 'Y')
    {
        if (pthread_create(&nodeServer, NULL, NodeServer, (void *)dataFD) != 0)
        {
            perror("Error creating node server thread!");
            exit(EXIT_FAILURE);
        }
        
    }
    
    
    if (c != 'Y')
    {
        if (pthread_join(clientCom, NULL) != 0)
        {
            perror("Error joining clientCom thread!");
        }
        if (pthread_join(clientComRead, NULL) != 0)
        {
            perror("Error joining clientComRead thread!");
        }
    }
    else if (c == 'Y')
    {
        if (pthread_join(nodeServer, NULL) != 0)
        {
            perror("Error joining nodeServer thread!");
        }
    }
    
    FreeList();
    printf("Closing server!\n");
    SSL_free(dataFD->nodeSSL);
    SSL_free(dataFD->ssl);        /* release connection state */
    SSL_CTX_free(ctx1);        /* release context */
	printf("Exiting program!\n");
    close(socket);

    return 0;
}

