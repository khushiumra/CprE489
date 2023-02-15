#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netinet/udp.h> 
#include <time.h>

int main(int argc, char ** argv)
{
	/*Server and client input info*/
	int IN_PORT;
	int OUT_PORT;
	char* OUT_IP;
	char* IN_IP;
	int lossRate;
	srand(time(NULL));//seed random var
	
	if(argc==6)//Collect network information from arguments
	{
		IN_IP = argv[1];
		IN_PORT = atoi(argv[2]);
		OUT_IP = argv[3];
		OUT_PORT = atoi(argv[4]);
		lossRate = atoi(argv[5]);
	}
	else//if not enough args, complain and quit
	{
		printf("Not enough arguments given. Format as follows: source_ip source_port dest_ip dest_port lossRate");
		return -1;
	}
	
	char serverBuffer[4000];//holds socket message fom server, sends to client
	int sockServ, sockCli;//socket file descriptors
	struct sockaddr_in inAddr, outAddr, in_stream;//socket addresses
	
	if( (sockServ = socket(PF_INET, SOCK_DGRAM, 0))< 0 )//Create socket of type UDP
	{
		perror("Server socket creation failure.");
		return -1;
	}
	
	memset(&inAddr, 0, sizeof(inAddr)); //creating memory pointers
    memset(&in_stream, 0, sizeof(in_stream)); 
	
	//Assign network information to appropiate structs
	inAddr.sin_family = PF_INET;
	inAddr.sin_port = htons(IN_PORT);
	inAddr.sin_addr.s_addr = inet_addr(IN_IP);

	if( bind(sockServ, (const struct sockaddr *)&inAddr, sizeof(inAddr)) <0 )//bind the listening server socket to the given ip and port
	{
		perror("Server Binding failed:");
		return -1;
	}
	//Server socket done
	
	if( (sockCli = socket(PF_INET, SOCK_DGRAM, 0)) < 0)//create client socket to send data
	{
		perror("Client socket creating failed: ");
		return -1;
	}
	memset(&outAddr, 0, sizeof(outAddr));//create memory pointer
	
	//assign network information to client socket
	outAddr.sin_family = PF_INET;
	outAddr.sin_port = htons(OUT_PORT);
	outAddr.sin_addr.s_addr = inet_addr(OUT_IP);
	
	//client socket done

	int lenCli, lenIn;//create length vars
	lenCli = sizeof(outAddr);
	int ranLoss = 0;//variable to store random values
	
	while(1)
	{
		ranLoss = rand()%1000+1;//generate random var between 1 and 1000
		if( (recvfrom(sockServ, serverBuffer, sizeof(serverBuffer), 0, (struct sockaddr *)&in_stream, &lenIn)) < 0)//recieve data thats being sent to our server ip, store sending address to dummy struct
		{
			perror("Receive: ");
		}

		if(ranLoss < (1001-lossRate))//if ranLoss is between 1001 and 1001-lossrate, we dont send, othewise send. Simulates packet loss
		{
			if ( (sendto(sockCli, serverBuffer, sizeof(serverBuffer), 0, (struct sockaddr *)&outAddr, lenCli)) < 0 )//using the client socket, send it to the given destination port and ip
			{
				perror("Send: ");
			}
		}
	}
	
	/*Close socket fd's*/
	close(sockCli);
	close(sockServ);
	
	return 1;
}