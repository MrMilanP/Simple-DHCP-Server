#include <iostream>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sstream>

#ifdef _WIN32
    #include <ws2tcpip.h>
    #include <winsock2.h>
    #include <cstdint>
    #pragma comment(lib, "ws2_32.lib") // Linkovanje Winsock biblioteke na Windows-u
    typedef int socklen_t;  // Definisanje socklen_t za kompatibilnost sa Windows-om
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define INVALID_SOCKET (-1) // Definisanje INVALID_SOCKET za kompatibilnost
    #define SOCKET_ERROR   (-1) // Definisanje SOCKET_ERROR za kompatibilnost
    typedef int SOCKET; // Na Linux-u socket je samo int, pa definisemo SOCKET kao int
#endif

#define SERVER_PORT 67 // DHCP port na kojem server sluša
#define BUFLEN 512 // Maksimalna dužina bafera za primanje podataka
#define DHCPDISCOVER 1 // Tip DHCP DISCOVER paketa
#define DHCPREQUEST 3 // Tip DHCP REQUEST paketa

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

class Dhcp {
public:
    Dhcp(); // Konstruktor
    ~Dhcp(); // Destruktor
    void listen(); // Metoda koja osluškuje DHCP pakete

private:
    SOCKET s; // Socket deskriptor
    struct sockaddr_in server, si_other; // Strukture za adresiranje servera i klijenta
    char buf[BUFLEN]; // Bafer za primanje paketa
    socklen_t slen; // Dužina adrese
    int recv_len; // Dužina primljenog paketa

    void receivePacket(); // Metoda za primanje DHCP paketa
    bool isValidDhcpPacket(byte* buffer); // Proverava da li je validan DHCP paket
    void handleDhcpPacket(); // Obradjuje primljeni DHCP paket
    void parseOptions(u_char options[]); // Parsira opcije iz DHCP paketa

    void initializeSockets(); // Inicijalizuje socket (specifično za Windows)
    void cleanupSockets(); // Čisti socket resurse (specifično za Windows)
};

Dhcp::Dhcp() {
    slen = sizeof(si_other); // Postavlja dužinu strukture adrese klijenta
    initializeSockets(); // Inicijalizuje socket (ako je na Windows-u)

    // Kreira UDP socket
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == INVALID_SOCKET) {
        throw std::runtime_error("Nije moguće kreirati socket");
    }

    // Priprema sockaddr_in strukturu za server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; // Server sluša na svim dostupnim interfejsima
    server.sin_port = htons(SERVER_PORT); // Port na kojem server sluša

    // Bind-uje socket na odgovarajuću adresu i port
    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        cleanupSockets(); // Ako dođe do greške, čisti resurse
        throw std::runtime_error("Bind nije uspeo");
    }

    std::cout << "Bind uspešan." << std::endl;
}

void Dhcp::listen() {
    while (true) {
        try {
            receivePacket(); // Prima paket
            if (isValidDhcpPacket((byte*)buf)) { // Proverava da li je validan DHCP paket
                handleDhcpPacket(); // Obradjuje paket
            } else {
                std::cout << "Nevalidan DHCP paket primljen." << std::endl;
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "Greška: " << e.what() << std::endl; // Ispisuje grešku
        }
    }
}

std::string ipToString(struct in_addr ipAddr) {
    char buffer[INET_ADDRSTRLEN];  // Buffer za string IP adrese
    if (inet_ntop(AF_INET, &ipAddr, buffer, sizeof(buffer))) {
        return std::string(buffer);  // Vraća string IP adrese
    }
    else {
        return "Invalid IP";  // U slučaju greške
    }
}

void Dhcp::receivePacket() {
    memset(buf, '\0', BUFLEN); // Briše prethodni sadržaj bafera
    recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen); // Prima podatke sa mreže
    if (recv_len == SOCKET_ERROR) {
        throw std::runtime_error("recvfrom() nije uspeo");
    }
    std::cout << "Paket primljen od " << ipToString(si_other.sin_addr) << ":" << ntohs(si_other.sin_port) << std::endl;

}

bool Dhcp::isValidDhcpPacket(byte* buffer) {
    Dhcp_packet dhcp_packet;
    memcpy(&dhcp_packet, buffer, sizeof(Dhcp_packet)); // Kopira podatke u DHCP strukturu
    // Proverava da li opcije sadrže magični broj DHCP protokola
    return (dhcp_packet.options[0] == 0x63 && dhcp_packet.options[1] == 0x82 && 
            dhcp_packet.options[2] == 0x53 && dhcp_packet.options[3] == 0x63);
}

void Dhcp::handleDhcpPacket() {
    Dhcp_packet dhcp_packet;
    memcpy(&dhcp_packet, buf, sizeof(Dhcp_packet)); // Kopira primljene podatke u DHCP strukturu

    // Ispisuje MAC adresu klijenta iz DHCP paketa
    std::cout << "Obrada DHCP paketa, MAC: "
              << std::hex << (int)dhcp_packet.chaddr[0] << ":" 
              << std::hex << (int)dhcp_packet.chaddr[1] << ":" 
              << std::hex << (int)dhcp_packet.chaddr[2] << ":" 
              << std::hex << (int)dhcp_packet.chaddr[3] << ":"
              << std::hex << (int)dhcp_packet.chaddr[4] << ":"
              << std::hex << (int)dhcp_packet.chaddr[5] << std::endl;

    // Obradjuje opcije DHCP paketa
    parseOptions(dhcp_packet.options);
}

void Dhcp::parseOptions(u_char options[]) {
    u_char packet_type = options[6]; // Tip DHCP paketa (DISCOVER, REQUEST, itd.)

    switch (packet_type) {
        case DHCPDISCOVER:
            std::cout << "DHCPDISCOVER" << std::endl;
            // Implementirati logiku za obradu DHCPDISCOVER paketa
            break;

        case DHCPREQUEST:
            std::cout << "DHCPREQUEST" << std::endl;
            // Implementirati logiku za obradu DHCPREQUEST paketa
            break;

        default:
            std::cout << "Nepoznat tip DHCP paketa: " << std::to_string(packet_type) << std::endl;
            break;
    }
}

void Dhcp::initializeSockets() {
#ifdef _WIN32
    WSADATA wsa;
    std::cout << "Inicijalizacija Winsock-a..." << std::endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { // Inicijalizuje Winsock na Windows-u
        throw std::runtime_error("Winsock inicijalizacija nije uspela");
    }
#endif
}

void Dhcp::cleanupSockets() {
#ifdef _WIN32
    WSACleanup(); // Oslobađa Winsock resurse na Windows-u
#endif
}

Dhcp::~Dhcp() {
#ifdef _WIN32
    cleanupSockets(); // Čisti socket resurse prilikom destrukcije objekta
#else
    close(s); // Zatvara socket
#endif
}
