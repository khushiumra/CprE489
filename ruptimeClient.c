#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc, char ** argv) {

    struct sockaddr_in serverAddr;

    int hSock; //This is the client socket
    int SERVER_PORT = 42069;

    char* SERVER_IP;

    if(argc == 2) // Make sure that there are enough arguments being passed

    {
        SERVER_IP = argv[1];
    } 

    else {
        printf ("Not enough arguments given. Using default IP. \n");
        SERVER_IP = "127.0.0.1";
    }

    if( (hSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) //To create the socket, which ensuring that it is created correctly

    {

        perror("Socket error. \n");
        return -1;
    } 

    //Here, we are going to add the TCP data to the serverAddr struct

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    if(inet.pton(AF_INET, SERVER_IP, &serverAddr, sin_addr, s_addr) <= 0) //This checks to make sure that the IP is a valid IP address

    {

        printf("Invalid address / Address not supported \n");
        return -1;
    }

    int outputvalue;
    outpulvalue = connect(hSock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)); //To try to send the information for serveraddr to the socket

    if(outputvalue < 0) 

    {
        perror("connection failed");
        return -1;
    }

    /* From here onwards, the client side reads from thr server sides */

    char readBuffer[1000];

    // Here we will collect data from the server and we will save it to readBuffer

    read(hSock, readBuffer, sizeof(readBuffer));

    printf("%s%s", SERVER_IP, readBuffer); //To print the server ip and the uptime

    close(hSock); 
    return 0;
}