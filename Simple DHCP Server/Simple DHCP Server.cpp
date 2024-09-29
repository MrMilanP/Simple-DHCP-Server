#include <cstdint>
#include <cstdio>
#include <cstring>
#include <stdexcept>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib") // Linkovanje Winsock biblioteke na Windows-u
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

int main() {
    try {
        Dhcp dhcpServer; // Kreira DHCP server
        std::cout << "Pokreće DHCP server..." << std::endl;
        dhcpServer.listen(); // Pokreće osluškivanje DHCP paketa
    } catch (const std::runtime_error& e) {
        std::cerr << "Greška: " << e.what() << std::endl;
        return EXIT_FAILURE; // Izlaz sa greškom
    }

    return EXIT_SUCCESS; // Uspešan završetak
}
