#include "ConnectToServer.h"


int ConnectServer(char *hostname, int port)
{
    int sd;
    struct sockaddr_in addr;
    socklen_t addrlen;
    
    bzero(&addr, sizeof(addr));
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    addr.sin_addr.s_addr = inet_addr(hostname);
    
    sd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sd < 0)
    {
        perror("Error creating socket!");
    }
    
    if (connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
        puts("Error connecting to server!");
        getsockname(sd, (struct sockaddr *)&addr, &addrlen);
        printf("Bound to port %d\n", ntohs(addr.sin_port));
        perror(hostname);
        close(sd);
        exit(EXIT_FAILURE);
    }
    else
    {
        puts("Succesfully connected to server!");
    }
    
    return sd;
}

int ServerSetup(int port)
{
	int socketD;
	
	struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
	
	
	addr.sin_family = AF_INET; //IPV4 protocol
	addr.sin_port = htons((uint16_t)port); //Port to bind to 
	addr.sin_addr.s_addr = INADDR_ANY;
	
	socketD = socket(AF_INET, SOCK_STREAM, 0); //IPV4, sequenced byte streams.
	
	if (socketD < 0)
	{
		puts("Error creating socket!");
		exit(EXIT_FAILURE);
	}
	
	if (bind(socketD, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		perror("Unable to bind port!");
		exit(EXIT_FAILURE);
	}
    else
    {
        getsockname(socketD, (struct sockaddr *)&addr, &addrlen);
        printf("Bound to port %d\n", ntohs(addr.sin_port));
    }
	
	if (listen(socketD,1) < 0)
	{
		puts("Unable to listen!");
		exit(EXIT_FAILURE);
	}
	
	return socketD;
}
