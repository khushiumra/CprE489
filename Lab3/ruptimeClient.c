#include <netinet/in.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])

{
	struct sockaddr_in serverAddr;
    int SERVER_PORT = 1025;

	int hSock, i; //hSock is the client socket

	if (argc < 3) // Make sure that there are enough arguments being passed
    
    {
		perror("Not enough inputs given \n");
		exit(1);
	}


	for (i = 1; i < argc; i++) 
    
    {
		if ( (hSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) // To create the socket, while ensuring that it is created correctly
        
        {
			perror("Socket() error \n");
			exit(1);
		}

    // Here weare going to add the data to the serverAddr struct
		
		serverAddr.sin_port = htons(SERVER_PORT);
		serverAddr.sin_family = PF_INET;
		serverAddr.sin_addr.s_addr = inet_addr(argv[i]);

		if ( (connect(hSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) < 0) // This checks to make sure that the IP is valid
        
        {
			perror("Connect() error \n");
			exit(1);
		}

        char readBuffer[1000];
	
		read(hSock, readBuffer, sizeof(readBuffer));

		printf("%s: Uptime: %s", argv[i], readBuffer);

		close(hSock);
	}
}
