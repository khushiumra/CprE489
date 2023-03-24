#include <stdio.h> 
#include <string.h>    
#include <sys/socket.h>    
#include <stdlib.h>
#include "ccitt16.h"
#include "utilities.h"
#include "introduceerror.h"

#define MAX(a,b) (((a)>(b))? (a) : (b))

void primary(int sockfd, double ber) 
{
	int read_size;

    char msg[100], srv_reply[150];
	unsigned char packet[PACKET_SIZE];

	// To keep a track of the current packet in the window frame
	int current_packet = 0;

	//To keep track of the window frame
	int track_packet = 0;

	//To keep track of the total number of packets sent
	int total_packets = 0;
	
    //keep communicating with server
    while(1)
    {
        printf("Enter your message here : ");
		fgets(msg, 100 , stdin); //Here we will receive the message from the user
        
		//To get the size of the message
		int msg_Size = strlen(msg);

		// Here we will calculate the number of packets that are needed
		int no_packets = (msg_Size+(DATA_LENGTH-1))/DATA_LENGTH;

		// This will adjust the window frame based on the receiver ACK packets
		track_packet += current_packet;
		current_packet = 0;

		//Here we will continue to send packets until we receive ACK for all of them
		
		while(current_packet < no_packets)
		{
			//Beginning with the initial packet of the window frame
			int c = 0;

		// This will ensure that the packets are sent until the end of the frame
			while( c < WINDOW_SIZE && current_packet < no_packets)
			{
				char buffer[DATA_LENGTH+1];

				
				// To set the memory of the packet and send the packet to the receiver
				memset(buffer, '\0', sizeof(buffer));
				strncpy(buffer, msg  + (current_packet*DATA_LENGTH), DATA_LENGTH);

				//To build the packet
				buildPacket(packet, DATA_PACKET, buffer, track_packet + current_packet);

				//To print the packet
				printPacket(packet);
				IntroduceError(packet, ber);
				
				if( send(sockfd , packet, PACKET_SIZE, 0) < 0)
				perror("Send failed! Try again.");


				//To ensure that we increase the count of packets and the total packets sent
				total_packets++;
				c++;
				current_packet++;
			}

			printf("Window is now complete! \n");
			printf("Number of packets sent: %d \n\n",c);
			printf("Server response:\n");
			
			//To store the largest sq number
			int maxSqNum = 0;

			//Boolean - to check if received packet is a NAK packet or not
			int isNAK = 0;

			int d = c;

			//Here we will check for the three expected ACK packets
			for(d; d > 0; d--)
			{

				//To receive the packet from the sender
				if( (read_size = recv(sockfd , srv_reply , PACKET_SIZE , 0)) < 0)
					perror("recv failed");

				//Here we will print the packet information
				printPacket(srv_reply);
				
				//This will allow us to check the type of packet we received
				switch(srv_reply[0])
				{
					case ACK_PACKET:
						maxSqNum = MAX(srv_reply[1] - track_packet, maxSqNum);
						break;
					case NAK_PACKET:
						isNAK = 1;
					default:
						printf("UNDEFINED PACKET TYPE");
				}
			}
			
			//If the packet is NAK, we will return it to the beginning of the window frame
			current_packet -= c;

			//If the packet is not NAK, the window frame will begin at the maximum sq number
			if(!isNAK)
			{
				current_packet = maxSqNum;
			}
		
		}

		//Final step is to print the average number of transmissions per packet received
		printf("Average transmission attempts per packet: %f\n",total_packets/14.0);
    }
  
}