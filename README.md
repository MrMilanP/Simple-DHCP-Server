An example of a simple DHCP server
# Simple DHCP Server

## Opis

Simple DHCP Server je jednostavan DHCP server napisan u C++. Ovaj server podržava osnovne funkcionalnosti DHCP protokola, omogućavajući dinamičko dodeljivanje IP adresa klijentima na mreži.

## Funkcionalnosti

- **DHCP Discover**: Prepoznaje klijente koji traže IP adresu.
- **DHCP Offer**: Dodeljuje IP adresu klijentu.
- **DHCP Request**: Klijent traži dodeljenu IP adresu.
- **DHCP Acknowledge**: Potvrđuje dodeljivanje IP adrese klijentu.

## Sistem zahteva

- C++ kompajler (npr. g++)
- Make alat (za Linux)
- Winsock biblioteka (za Windows)

## Instalacija

### Windows

1. Klonirajte repo:
   ```bash
   git clone https://your-repo-url.git
   cd Simple-DHCP-Server
   ```
2. Otvorite `Simple DHCP Server.vcxproj` u Visual Studio-u.
3. Izvršite build projekta.

### Linux

1. Klonirajte repo:
   ```bash
   git clone https://your-repo-url.git
   cd Simple-DHCP-Server
   ```
2. U root direktorijumu projekta kreirajte Makefile:
   ```Makefile
   CXX = g++
   CXXFLAGS = -Wall -Wextra -std=c++11

   all: dhcp_server

   dhcp_server: main.o dhcp.o
       $(CXX) -o dhcp_server main.o dhcp.o

   %.o: %.cpp
       $(CXX) $(CXXFLAGS) -c $< -o $@

   clean:
       rm -f *.o dhcp_server
   ```
3. Pokrenite `make` da biste izgradili projekat:
   ```bash
   make
   ```

## Korišćenje

Pokrenite server iz komandne linije:

```bash
./dhcp_server
```

Server će slušati na portu 67 za dolazne DHCP zahteve.
