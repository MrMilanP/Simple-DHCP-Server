#pragma once

#include <winsock.h>
#include <cstdint>

#define DHCP_UDP_OVERHEAD	(20 + /* IP header */			8)   /* UDP header */
#define DHCP_SNAME_LEN		    64
#define DHCP_FILE_LEN	        128
#define DHCP_FIXED_NON_UDP	  236
#define DHCP_FIXED_LEN		  (DHCP_FIXED_NON_UDP + DHCP_UDP_OVERHEAD)
/* Everything but options. */
#define BOOTP_MIN_LEN		300

#define DHCP_MTU_MAX		1500
#define DHCP_MTU_MIN    576

#define DHCP_MAGIC_COOKIE_LEN 4

#define DHCP_MAX_OPTION_LEN	(DHCP_MTU_MAX - DHCP_FIXED_LEN - DHCP_MAGIC_COOKIE_LEN)
#define DHCP_MIN_OPTION_LEN (DHCP_MTU_MIN - DHCP_FIXED_LEN)

#define MAGIC_COOKIE 0x63825363;

#define BUFLEN 1500	//Max length of buffer
#define SERVER_PORT 67	
#define CLIENT_PORT 68




class Dhcp
{
	private:
		int x;

		SOCKET s;
		struct sockaddr_in server, si_other;
		int slen, recv_len;
		byte buf[BUFLEN];
		WSADATA wsa;

		struct  Dhcp_packet {
			u_char  op;	//0	
			u_char  htype;	//1 
			u_char  hlen;		//2 
			u_char  hops;	//3	
			uint32_t xid;	//4	
			uint16_t secs;	//8	
			uint16_t flags;	//10
			struct in_addr ciaddr; //12
			struct in_addr yiaddr;	//16
			struct in_addr siaddr;	//20
			struct in_addr giaddr;	//24
			u_char chaddr[16];	//28
			u_char sname[64];	//44
			u_char file[128];	//108
			u_char options[DHCP_MIN_OPTION_LEN];//216			
		} ;

		struct Dhcp_options
		{
			BYTE pbMagicCookie[4];
			BYTE pbMessageType[3];
			BYTE pbLeaseTime[6];
			BYTE pbSubnetMask[6];
			BYTE pbServerID[6];
			BYTE bEND;
		};

	public:

		void setX(int i);

		int getX();

		Dhcp_packet dhcp_packet;

		Dhcp();
};

