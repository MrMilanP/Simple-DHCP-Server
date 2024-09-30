#include <iostream>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cstring>

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

#include "dhcp.h"

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
    recv_len = recvfrom(s, reinterpret_cast<char*>(buf), BUFLEN, 0, (struct sockaddr*)&si_other, &slen);// Prima podatke sa mreže
    if (recv_len == SOCKET_ERROR) {
        throw std::runtime_error("recvfrom() nije uspeo");
    }
    std::cout << "Paket primljen od " << ipToString(si_other.sin_addr) << ":" << ntohs(si_other.sin_port) << std::endl;

}

bool Dhcp::isValidDhcpPacket(byte* buffer) {
    Dhcp_packet dhcp_packet;
    memcpy(&dhcp_packet, buffer, sizeof(Dhcp_packet)); // Kopira podatke u DHCP strukturu

    // Konvertuje prva četiri bajta opcija u jedan 32-bitni broj
    uint32_t cookie = (dhcp_packet.options[0] << 24) |
        (dhcp_packet.options[1] << 16) |
        (dhcp_packet.options[2] << 8) |
        dhcp_packet.options[3];

    // Poredi sa definisanom MAGIC_COOKIE vrednošću
    return cookie == MAGIC_COOKIE;
}

void Dhcp::handleDhcpPacket() {
    //if (recv_len < sizeof(Dhcp_packet)) {
    //    std::cerr << "Primljen paket je previše mali za obradu." << std::endl;
    //    return; // Napusti metodu ako je paket manji
    //}
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
    parseOptions(dhcp_packet.options, dhcp_packet);
}

void Dhcp::sendDhcpPacketData(const Dhcp_packet* packet) {
    sendto(s, reinterpret_cast<const char*>(packet), sizeof(Dhcp_packet), 0, (struct sockaddr*)&si_other, slen);
}

void Dhcp::sendDhcpOffer(Dhcp_packet* request) {
    Dhcp_packet offer_packet;
    memset(&offer_packet, 0, sizeof(offer_packet));

    offer_packet.op = 2; // Odgovor (BOOTREPLY)
    offer_packet.xid = request->xid; // Koristi isti xid
    memcpy(offer_packet.chaddr, request->chaddr, 16); // Postavi MAC adresu klijenta


    if (inet_pton(AF_INET, "192.168.1.100", &offer_packet.yiaddr) <= 0) {
        std::cerr << "Greška u konverziji IP adrese." << std::endl;
        return;
    } // Dodeljena IP adresa

    strcpy_s(reinterpret_cast<char*>(offer_packet.sname), sizeof(offer_packet.sname), "Simple DHCP Server");
    strcpy_s(reinterpret_cast<char*>(offer_packet.file), sizeof(offer_packet.file), "Simple DHCP Server.exe");



    offer_packet.options[0] = 53; // Opcija tipa DHCP
    offer_packet.options[1] = 1;  // Dužina opcije
    offer_packet.options[2] = DHCPOFFER; // Tip paketa: DHCPOFFER

    // Dodavanje drugih opcija kao što su subnet maska, router itd.
    // Primer za subnet masku (255.255.255.0)
    offer_packet.options[3] = 1; // Opcija subnet maske
    offer_packet.options[4] = 4; // Dužina opcije
    offer_packet.options[5] = 255; // subnet maska 255
    offer_packet.options[6] = 255; // subnet maska 255
    offer_packet.options[7] = 255; // subnet maska 255
    offer_packet.options[8] = 0;   // subnet maska 0

    // Signaliziranje kraja opcija
    offer_packet.options[9] = 255; // Opcija za kraj

    // Slanje DHCPOFFER paketa
    //sendto(s, (char*)&offer_packet, sizeof(offer_packet), 0, (struct sockaddr*)&si_other, slen);

    sendDhcpPacketData(&offer_packet);
}

void Dhcp::parseOptions(u_char options[], Dhcp_packet& dhcp_packet) {
    u_char packet_type = options[6]; // Tip DHCP paketa (DISCOVER, REQUEST, itd.)

    switch (packet_type) {
    case DHCPDISCOVER:
        std::cout << "DHCPDISCOVER" << std::endl;
        sendDhcpOffer(&dhcp_packet); // Funkcija koja šalje DHCPOFFER
        break;

    case DHCPREQUEST:
        std::cout << "DHCPREQUEST" << std::endl;
        // Implementirati logiku za obradu DHCPREQUEST paketa
        break;

    case DHCPDECLINE:
        std::cout << "DHCPDECLINE" << std::endl;
        // Implementirati logiku za obradu DHCPDECLINE paketa
        break;

    case DHCPRELEASE:
        std::cout << "DHCPRELEASE" << std::endl;
        // Implementirati logiku za obradu DHCPRELEASE paketa
        break;

    case DHCPINFORM:
        std::cout << "DHCPINFORM" << std::endl;
        // Implementirati logiku za obradu DHCPINFORM paketa
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
