#include "socket_handler.h"
#include <stdlib.h>
#include <stdio.h>


int BindAndListenToPort(int port)
{
	int socketDescriptor;
	
	struct sockaddr_in addr;
	
	addr.sin_family = AF_INET; //IPV4 protocol
	addr.sin_port = htons(port); //Port to bind to 
	addr.sin_addr.s_addr = INADDR_ANY;
	
	socketDescriptor = socket(AF_INET, SOCK_STREAM, 0); //IPV4, sequenced byte streams.
	
	if (socketDescriptor < 0)
	{
		puts("Error creating socket! Exiting");
		exit(EXIT_FAILURE);
	}
	
	if (bind(socketDescriptor, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		puts("Unable to bind port! Exiting");
		exit(EXIT_FAILURE);
	}
	
	if (listen(socketDescriptor,1) < 0)
	{
		puts("Unable to listen! Exiting");
		exit(EXIT_FAILURE);
	}
	
	return socketDescriptor;
}
