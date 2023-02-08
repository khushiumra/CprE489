#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

int main(int argc, char ** argv) {

    //First, we need to create the socket

    int hSock, to_client;

    int outputvalue;

    int PORT_NUM = 42069;

    char* SERVER_IP;

    char* args[] = ("uptime", NULL);

    if(argc == 2) // Make sure that there are enough arguments being passed

    {
        SERVER_IP = argv[1];
    } 

    else {

        SERVER_IP = "127.0.0.1";
    }

    struct sockaddr_in serverAddr, clientAddr;

    int len = sizeof(clientAddr);

  if( (hSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) //To create the socket, which ensuring that it is created correctly

  {
	  perror("Socket error.\n");
	  return -1;
  }
  
  //Here, we are going to add the TCP data to the serverAddr struct

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT_NUM);

  if(inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr) <=0 ) //This checks to make sure that the IP is a valid IP address

  { 
      printf("Invalid address / Address not supported \n"); 
      return -1; 
  }
  
  //bind server to the given address and port

  if(bind(hSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr))<0)//checks server binding errors

  {
	  perror("socket bind failed");
	  return -1;
  }

/* Then the server listens to IP Address : Port */

  if(listen(hSock, 10) < 0 ) //error checking listening

  {
	  perror("Listen failed \n");
	  return -1;
  } 

  while (1) 
  {
	//server accepts client connection

    if((to_client = accept(hSock, (struct sockaddr *) &clientAddr, &len)) < 0 ) //error checking connection acception

	{
		perror("Server failed to accept \n");
		return -1;
	}

	//create child process so we dont close server

	pid_t cpid = fork();

	if(cpid) //parent process
	{
		close(to_client);//Here we will close the socket connection on the parent
		waitpid(cpid, NULL,0);//We will need to wait for child, before we try to do anything else
	}

	else //From here, we will be focusing on the child process

	{
		dup2(to_client,1);//To duplicate the client socket to studio file stream
		close(to_client);//To close the socket
		execvp("uptime",args);//To print uptime to studio, which goes to client socket because of dup2
		exit(0);//quit child process
	}

  }

  close(hSock);

  return 0;
  
}

