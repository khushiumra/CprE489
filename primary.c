#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "ccitt16.h"
#include "utilities.h"
// #include "introduceerror.h"

void shiftMessage(char arr[], int size, int amt);

// Reminder : ARQ GB3, with added NAK (& therefore no TO)
void primary(int sockfd, double ber) {
	int read_size;
	int window[3] = {0,1,2};
	int counter[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	char temp[3];
	char msg[100] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	msg[26] = '\0';
	char srv_reply[150], srv_msg[150];
	struct packet_buffer packet_buff[3], sending_pack_buff[3];

	srv_msg[0] = '\0';

	// keep communicating with server
	while(1)
	{
		// TODO : Are these within this loop? Is there a better way to start sending?
		// printf("Enter message : ");
		// fgets(msg, 100 , stdin);
		// fgets(temp, 100 , stdin);
		int i = 0;

		// Send first three (possible) packets
		while ( (i < 3) ) { // TODO : These conditions work? // && (strlen(msg) < (2*i))
			struct packet_buffer pack;
			unsigned char data[DATA_LENGTH] = {msg[2*window[i]], msg[(2*window[i])+1]};
			buildPacket(pack.packet, DATA_PACKET, data, window[i]);
			packet_buff[i] = pack;
			printPacket(pack.packet); // TODO : What about when there is error here?
			// IntroduceError(pack.packet, ber);
			if( send(sockfd , pack.packet, PACKET_SIZE, 0) < 0)
				perror("Send failed");
			i++;
		}

		// First read from the receiver
		if ( (read_size = recv(sockfd , srv_reply , 149 , 0)) < 0)
			perror("recv failed");
		srv_reply[read_size] = '\0';
		
		// int j = 0;
		while (read_size > 0) {
			strcat(srv_msg, srv_reply);
			read_size = strlen(srv_msg);
			// printf("Server's reply: %s\n", srv_msg);
			// Packet make-up: 0 = type; 1 = SN; 2&3 = DATA; 4&5 = CRC;
			while (strlen(srv_msg) > 5) {
				if (window[0] > (strlen(msg)/2)) {
					printf("\nSent entire message\n\n");
					for (i = 0; i < 14; i++) {
						printf("Serial Number %d transmitted : %d\n", i, counter[i]);
					}
					return;
				}
				// printPacket((char *)&srv_msg[(j * 6)]);
				// printf("SN: %d\n\r", srv_msg[(j * 6) + 1]);
				// fgets(temp, 100 , stdin);
				// TODO : I think these should be some sort of incrementing value. Or copy values to new buffer.
				if ( srv_msg[1] >= window[0] && srv_msg[1] <= (window[2] + 1) ) {
					int new_frames = shiftBuf(packet_buff, 3, (srv_msg[1] - window[0]) );
					shiftWindow(window, 3, new_frames);
					// printf("New frames: %d\n", new_frames);
					// Add new packets to packet_buff
					for (i = (3-new_frames); i < 3; i++) {
						struct packet_buffer pack, sending_pack;
						unsigned char data[DATA_LENGTH] = {msg[2*window[i]], msg[(2*window[i])+1]};
						buildPacket(pack.packet, DATA_PACKET, data, window[i]);						
						buildPacket(sending_pack.packet, DATA_PACKET, data, window[i]);
						packet_buff[i] = pack;
						sending_pack_buff[i] = sending_pack;
					}
					if (srv_msg[0] == ACK_PACKET) {
						for (i = (3-new_frames); i < 3; i++) {
							printPacket(packet_buff[i].packet);
							// IntroduceError(sending_pack_buff[i].packet, ber);
							if( send(sockfd , sending_pack_buff[i].packet, PACKET_SIZE, 0) < 0)
								perror("Send failed");
							// j++;
						}
						shiftMessage(srv_msg, strlen(srv_msg), 6);
					} else { // assumed NAK_PACKET
						// Retransmit ALL of the window
						for (i = 0; i < 3; i++) {
							struct packet_buffer sending_pack;
							printPacket(packet_buff[i].packet);
							buildPacket(sending_pack.packet, DATA_PACKET, (char *)&sending_pack.packet[2], sending_pack.packet[1]);
							// IntroduceError(sending_pack.packet, ber);
							counter[packet_buff[i].packet[1]]++;
							if( send(sockfd , packet_buff[i].packet, PACKET_SIZE, 0) < 0)
								perror("Send failed");
							// j++;
						}
						shiftMessage(srv_msg, strlen(srv_msg), 6);
					}
					
				} else {
					// printf("Unexpected SN, retransmitting sender window\n\r");
					for (i = 0; i < 3; i++) {
						counter[packet_buff[i].packet[1]]++;
						printPacket(packet_buff[i].packet);
						// TODO : IntroduceError(packet_buff[i].packet, ber);
						if( send(sockfd , packet_buff[i].packet, PACKET_SIZE, 0) < 0)
							perror("Send failed");
					}
					shiftMessage(srv_msg, strlen(srv_msg), 6);
				}
			}
			
			if( (read_size = recv(sockfd , srv_reply , 149 , 0)) < 0)
				perror("recv failed");
			srv_reply[read_size] = '\0';
			printf("Server's reply: %s\n", srv_reply);
		}

		// TODO : Print every recieved packet? Or every recv?
		// Null termination since we need to print it
		srv_reply[read_size] = '\0';
		printf("Last Server's reply: %s\n", srv_reply);
	}
}

void shiftMessage(char arr[], int size, int amt)
{
	int i;
	for(i = 0; i < size; ++i)
	{
		arr[i] += amt;
	}
}
