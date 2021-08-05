// Simple DHCP Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstdint>
#include <winsock.h>
#include "dhcp.h"


int main()
{
    std::cout << "Hello World!\n";

    printf("u_char: %d\n", sizeof(u_char));
    printf("uint16_t: %d\n", sizeof(uint16_t));
    printf("uint32_t: %d\n", sizeof(uint32_t));
    printf("in_addr: %d\n", sizeof(in_addr));

    Dhcp dhcp;

   // printf("dhcp_packet: %d\n", sizeof(dhcp.dhcp_packet));
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
