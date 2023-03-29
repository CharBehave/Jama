#include "client.h"
#include "ConnectToServer.h"
#include "SSL_Handler.h"

int main(int argc, char *argv[])
{
    //func(socket);
    SSL_CTX *ctx;
    int socket;
    SSL *ssl;
    char *hostname, *portnum;
    pthread_t read;
    pthread_t write;
    
    if (argc != 3)
    {
        printf("usage: %s <hostname> <portnum>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    SSL_library_init();
    hostname = argv[1];
    portnum = argv[2];
    ctx = InitCTX();
    socket = ConnectServer(hostname, atoi(portnum));
    ssl = SSL_new(ctx);      /* create new SSL connection state */
    SSL_set_fd(ssl, socket);    /* attach the socket descriptor */
    
    if ( SSL_connect(ssl) == FAIL )   /* perform the connection */
        ERR_print_errors_fp(stderr);
    else
    {
        if (pthread_create(&write, NULL, WriteFunc, (void *)ssl) != 0)
        {
            perror("Error creating write thread!");
            exit(EXIT_FAILURE);
        }
        
        puts("You can start messaging now...");
        
        if (pthread_create(&read, NULL, ReadFunc, (void *)ssl) != 0)
        {
            perror("Error creating read thread!");
            exit(EXIT_FAILURE);
        }
        
        if (pthread_join(write, NULL) != 0)
        {
            perror("pthread_join write error!");
            exit(EXIT_FAILURE);
        }

        if (pthread_join(read, NULL) != 0)
        {
            perror("pthread_join read error!");
            exit(EXIT_FAILURE);
        }
        
        SSL_free(ssl);        /* release connection state */
    }
    
    SSL_CTX_free(ctx);        /* release context */
	printf("Exiting program!\n");
    close(socket);
    
    return 0;
}

void * WriteFunc(void * ssl)
{
    SSL *ssl1;
    ssl1 = (SSL*)ssl;
    char strCheck[5] = "exit\n";
    
    while (1)
    {
        
        char *message = NULL;
        message = (char*) malloc(1024);
        if (message == NULL)
        {
            perror("memory allocation failed!");
            exit(EXIT_FAILURE);
        }
        fgets(message, 1024, stdin);
        
        if (strcmp(message, strCheck) == 0)
        {
            return NULL;
        }
        
        printf("\n\nConnected with %s encryption\n", SSL_get_cipher(ssl));
        //~ ShowCerts(ssl1);        /* get any certs */
        
        int amountOfBytes = SSL_write(ssl1, message, strlen(message)) ;
        
        if (amountOfBytes <= 0)   /* encrypt & send message */
        {
            puts("Could not send message!");
            exit(EXIT_FAILURE);
        }
        free(message);
    }
    
    return NULL;
}

void * ReadFunc(void * ssl)
{
    SSL *ssl1;
    ssl1 = (SSL*)ssl;
    int bytes;
    char buf[1024];
    
    while (1)
    {
        while ((bytes = SSL_read(ssl1, buf, sizeof(buf))) > 0) /* get reply & decrypt */
        {
            buf[bytes] = 0;
            printf("Received: %s\n", buf);
            memset(buf, 0, sizeof(buf));
        }
    }
    return NULL;
}
