#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t; // Definiši socklen_t na Windows-u
#else
#include <sys/socket.h> // Uključi zaglavlja za Unix/Linux
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#endif

#include <cstdint>

class Dhcp {
public:
    Dhcp(); // Konstruktor
    ~Dhcp(); // Destruktor
    void listen(); // Metoda koja osluškuje DHCP pakete

private:
    // Definicije za DHCP
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

#define BUFLEN 512 // Maksimalna dužina bafera za primanje podataka

    typedef unsigned char u_char; // Definišemo u_char kao skraćenicu za unsigned char

    // Struktura za DHCP paket (uključuje standardna polja DHCP protokola)
    struct Dhcp_packet {
        u_char op;                // Operacioni kod poruke / tip poruke
        u_char htype;             // Tip hardverske adrese
        u_char hlen;              // Dužina hardverske adrese
        u_char hops;              // Hops (broj "skokova")

        uint32_t xid;             // Transaction ID (identifikacija transakcije)
        uint16_t secs;            // Sekunde protekle od početka
        uint16_t flags;           // Zastavice (flags)
        uint32_t ciaddr;          // Klijentova IP adresa (Client IP Address)
        uint32_t yiaddr;          // "Tvoja" IP adresa (Your IP Address)
        uint32_t siaddr;          // Sledeća serverska IP adresa (Next Server IP Address)
        uint32_t giaddr;          // IP adresa relay agenta (Gateway IP Address)

        u_char chaddr[16];        // Hardverska adresa klijenta (Client Hardware Address)
        u_char sname[64];         // Ime servera (Server Name)
        u_char file[128];         // Naziv boot fajla (Boot File Name)
        u_char options[312];      // Polje za opcionalne parametre (Options)
    };

    SOCKET s; // Socket deskriptor
    struct sockaddr_in server, si_other; // Strukture za adresiranje servera i klijenta
    socklen_t slen; // Dužina adrese
    int recv_len; // Dužina primljenog paketa
    uint8_t buf[BUFLEN]; // Bafer za primanje paketa
    WSADATA wsa;

    void safeStrncpy(char* dest, const char* src, size_t dest_size);

    void sendDhcpOffer(Dhcp_packet* request);
    void sendDhcpPacketData(const Dhcp_packet* packet);

    void receivePacket(); // Metoda za primanje DHCP paketa
    bool isValidDhcpPacket(byte* buffer); // Proverava da li je validan DHCP paket
    void handleDhcpPacket(byte* buffer); // Obradjuje primljeni DHCP paket
    void parseOptions(u_char options[], Dhcp_packet& dhcp_packet); // Parsira opcije iz DHCP paketa

    void initializeSockets(); // Inicijalizuje socket (specifično za Windows)
    void cleanupSockets(); // Čisti socket resurse (specifično za Windows)
};
