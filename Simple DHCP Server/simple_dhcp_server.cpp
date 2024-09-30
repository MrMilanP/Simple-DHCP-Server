#include <iostream>
#include <stdexcept>
#include "dhcp.h"

int main() {
    try {
        Dhcp dhcpServer; // Kreira DHCP server
        std::cout << "Pokreće DHCP server..." << std::endl;
        dhcpServer.listen(); // Pokreće osluškivanje DHCP paketa
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Greška: " << e.what() << std::endl;
        return EXIT_FAILURE; // Izlaz sa greškom
    }

    return EXIT_SUCCESS; // Uspešan završetak
}
