#include "dhcp.h"
#include <wchar.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

void Dhcp::setX(int i)
{
	x = i;
}

int Dhcp::getX()
{
	return x;
}

void Dhcp::parseOptions(u_char options[])
{
	u_char packet_type = options[6];

	switch (packet_type)
	{

	case DHCPDISCOVER:
		puts("DHCPDISCOVER");
		break;

	case DHCPREQUEST:
		puts("DHCPREQUEST");
		break;

	case DHCPRELEASE:

		break;

	default:
		break;
	}

}

Dhcp::Dhcp()
{
	//Dhcp_packet d;

	slen = sizeof(si_other);

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(SERVER_PORT);

	//Bind
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	//keep listening for data
	while (1)
	{
		printf("Waiting for data...");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, (char*)buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		printf("recv_len: %d\n", recv_len);

		//print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Data: %p\n", buf);

		//printf("Data Len: %d\n", strlen( buf));
		//int len = strlen(buf);
		for (int i = 0; i < recv_len; i++) {
			printf("Data[%d] : %02X\n",i,  buf[i]);
		};

		struct Dhcp_packet dhcp_packet ;
		//dhcp_packet = (struct Dhcp_packet*)malloc(sizeof(struct Dhcp_packet));

		//memset(dhcp_packet, '\0', sizeof(struct Dhcp_packet));

		memcpy(&dhcp_packet, buf, sizeof( Dhcp_packet));


		printf("dhcp_packet.hlen: %d\n", dhcp_packet.hlen);

		printf("%02X:%02X:%02X:%02X:%02X:%02X\n", dhcp_packet.chaddr[0], dhcp_packet.chaddr[1], dhcp_packet.chaddr[2], dhcp_packet.chaddr[3], dhcp_packet.chaddr[4], dhcp_packet.chaddr[5]);

		printf("%02X %02X %02X %02X\n", dhcp_packet.options[0], dhcp_packet.options[1], dhcp_packet.options[2], dhcp_packet.options[3]);

		if (recv_len > 240) {
			//Check did magic cookie exist
			if ((dhcp_packet.options[0] == 0x63) && (dhcp_packet.options[1] == 0x82) && (dhcp_packet.options[2] == 0x53) && (dhcp_packet.options[3] == 0x63)){
				puts("Magic cookie!!!");

				parseOptions(dhcp_packet.options);
			}
		}

		//if (dhcp_packet) {
		//	free(dhcp_packet);
		//}

		//now reply the client with the same data
		
		
		/*if (sendto(s, buf, recv_len, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}*/
	}

}
