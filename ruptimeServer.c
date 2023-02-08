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

  if( (hSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) //Creating the socket, making sure it creates correctly

  {
	  perror("Socket error.\n");
	  return -1;
  }
  
  //Add tcp data to server socket

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(PORT_NUM);

  if(inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr.s_addr) <=0 ) //Checks to make sure that IP is a valid address  

  { 
      printf("Invalid address/ Address not supported\n"); 
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
	  perror("listen failure");
	  return -1;
  } 

  while (1) 
  {
	//server accepts client connection

    if((to_client = accept(hSock, (struct sockaddr *) &clientAddr, &len)) < 0 ) //error checking connection acception

	{
		perror("accept connection failure");
		return -1;
	}

	//create child process so we dont close server

	pid_t cpid = fork();

	if(cpid) //parent process
	{
		close(to_client);//close socket connection on parent
		waitpid(cpid, NULL,0);//wait for child before doing anything else
	}

	else //child process

	{
		dup2(to_client,1);//duplicate client socket to stdio file stream
		close(to_client);//close socket
		execvp("uptime",args);//print uptime to stdio, which goes to client socket because of dup2
		exit(0);//quit child process
	}

  }

  close(hSock);

  return 0;
  
}

