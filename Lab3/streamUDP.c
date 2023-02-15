#include <netinet/in.h>
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>

int drop_packet(double rate)
{
    if (rand() % 100 < rate)
        return 1;
    else
        return 0;
}

int main (int argc, char *argv[]) {
	// in order : source IP, source port, dest IP, dest port, loss rate
	struct sockaddr_in recv_addr, send_addr;
	int hSock, to_client;
	socklen_t recv_len = sizeof(recv_addr);
	socklen_t send_len = sizeof(send_addr);
	char buffer[2048];

	if (argc < 6) {
		printf("Not enough inputs.");
		exit(1);
	}

	int loss_rate = atoi(argv[5]);
	// uses sendto()
	hSock = socket(PF_INET, SOCK_DGRAM, 0);
	// uses recvfrom()
	to_client = socket(PF_INET, SOCK_DGRAM, 0);

	// Send to this VLC
	recv_addr.sin_family = PF_INET;
	recv_addr.sin_port = htons(atoi(argv[2]));
	recv_addr.sin_addr.s_addr = inet_addr(argv[1]);

	// Recieve from this VLC
	send_addr.sin_family = PF_INET;
	send_addr.sin_port = htons(atoi(argv[4]));
	send_addr.sin_addr.s_addr = inet_addr(argv[3]);


	// Bind to the VLC/client that is sending the stream
	if ( (bind(to_client, (struct sockaddr *) &send_addr, sizeof(send_addr))) < 0) { // was send_addr
		close(to_client);
		perror("Client bind() error\n");
		exit(1);
	}

	while(1){
		recvfrom(to_client, buffer, 2048, 0, (struct sockaddr *) &recv_addr, &recv_len); // was recv_addr
		 
		if (drop_packet(loss_rate) == 1) {
			// drop the packet
		} else {
			sendto(hSock, buffer, 2048, 0, (struct sockaddr *) &send_addr, send_len); // was send_addr
		}
	}
}
