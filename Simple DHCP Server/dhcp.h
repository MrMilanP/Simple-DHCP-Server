#pragma once

#include <winsock2.h>  // Promeniti iz <winsock.h> u <winsock2.h>
#include <cstdint>

#define DHCP_UDP_OVERHEAD (20 + 8)   // IP header + UDP header
#define DHCP_SNAME_LEN    64
#define DHCP_FILE_LEN     128
#define DHCP_FIXED_NON_UDP 236
#define DHCP_FIXED_LEN    (DHCP_FIXED_NON_UDP + DHCP_UDP_OVERHEAD) // Everything but options
#define BOOTP_MIN_LEN     300

#define DHCP_MTU_MAX      1500
#define DHCP_MTU_MIN      576

#define DHCP_MAGIC_COOKIE_LEN 4

#define DHCP_MAX_OPTION_LEN (DHCP_MTU_MAX - DHCP_FIXED_LEN - DHCP_MAGIC_COOKIE_LEN)
#define DHCP_MIN_OPTION_LEN (DHCP_MTU_MIN - DHCP_FIXED_LEN)

#define MAGIC_COOKIE 0x63825363  // Uklonjena tačka i zapeta

#define BUFLEN 1500  // Max length of buffer
#define SERVER_PORT 67	
#define CLIENT_PORT 68

// DHCP message types
#define DHCPDISCOVER       0x01
#define DHCPOFFER          0x02
#define DHCPREQUEST        0x03
#define DHCPDECLINE        0x04
#define DHCPACK            0x05
#define DHCPNAK            0x06
#define DHCPRELEASE        0x07
#define DHCPINFORM         0x08

// DHCP options
#define DHCP_PADDING          0x00
#define DHCP_SUBNET           0x01
#define DHCP_TIME_OFFSET      0x02 /* (localtime - UTC_time) in seconds. signed */
#define DHCP_ROUTER           0x03
#define DHCP_TIME_SERVER      0x04 /* RFC 868 time server (32-bit, 0 = 1.1.1900) */
#define DHCP_NAME_SERVER      0x05 /* IEN 116 _really_ ancient kind of NS */
#define DHCP_DNS_SERVER       0x06
#define DHCP_LOG_SERVER       0x07  /* port 704 UDP log (not syslog) */
#define DHCP_COOKIE_SERVER    0x08 /* "quote of the day" server */
#define DHCP_LPR_SERVER       0x09
#define DHCP_HOST_NAME        0x0c /* either client informs server or server gives name to client */
#define DHCP_BOOT_SIZE        0x0d
#define DHCP_DOMAIN_NAME      0x0f /* server gives domain suffix */
#define DHCP_SWAP_SERVER      0x10
#define DHCP_ROOT_PATH        0x11
#define DHCP_IP_TTL           0x17
#define DHCP_MTU              0x1a
#define DHCP_BROADCAST        0x1c
#define DHCP_ROUTES           0x21
#define DHCP_NIS_DOMAIN       0x28
#define DHCP_NIS_SERVER       0x29
#define DHCP_NTP_SERVER       0x2a
#define DHCP_WINS_SERVER      0x2c
#define DHCP_REQUESTED_IP     0x32 /* sent by client if specific IP is wanted */
#define DHCP_LEASE_TIME       0x33
#define DHCP_OPTION_OVERLOAD  0x34
#define DHCP_MESSAGE_TYPE     0x35
#define DHCP_SERVER_ID        0x36 /* by default server's IP */
#define DHCP_PARAM_REQ        0x37 /* list of options client wants */
#define DHCP_ERR_MESSAGE      0x38 /* error message when sending NAK etc */
#define DHCP_MAX_SIZE         0x39
#define DHCP_VENDOR           0x3c /* client's vendor (a string) */
#define DHCP_CLIENT_ID        0x3d /* by default client's MAC addr, but may be arbitrarily long */
#define DHCP_TFTP_SERVER_NAME 0x42 /* same as 'sname' field */
#define DHCP_BOOT_FILE        0x43 /* same as 'file' field */
#define DHCP_USER_CLASS       0x4d /* RFC 3004. set of LASCII strings. "I am a printer" etc */
#define DHCP_FQDN             0x51 /* client asks to update DNS to map its FQDN to its new IP */
#define DHCP_DOMAIN_SEARCH    0x77 /* RFC 3397. set of ASCIZ string, DNS-style compressed */
#define DHCP_SIP_SERVERS      0x78 /* RFC 3361. flag byte, then: 0: domain names, 1: IP addrs */
#define DHCP_STATIC_ROUTES    0x79 /* RFC 3442. (mask,ip,router) tuples */
#define DHCP_VLAN_ID          0x84 /* 802.1P VLAN ID */
#define DHCP_VLAN_PRIORITY    0x85 /* 802.1Q VLAN priority */
#define DHCP_MS_STATIC_ROUTES 0xf9 /* Microsoft's pre-RFC 3442 code for 0x79? */
#define DHCP_WPAD             0xfc /* MSIE's Web Proxy Autodiscovery Protocol */
#define DHCP_END              0xff

class Dhcp
{
private:
    int x;

    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen, recv_len;
    uint8_t buf[BUFLEN]; // Zamenjen byte sa uint8_t
    WSADATA wsa;

    struct Dhcp_packet {
        u_char  op;	//0	
        u_char  htype;	//1 
        u_char  hlen;	//2 
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
        u_char options[DHCP_MIN_OPTION_LEN]; //236			
    };

    void parseOptions(u_char options[]);

public:
    Dhcp_packet dhcp_packet;

    Dhcp();
    void listen();
};
