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
   git clone https://github.com/MrMilanP/Simple-DHCP-Server.git
   cd Simple-DHCP-Server
   ```
2. Otvorite `Simple DHCP Server.vcxproj` u Visual Studio-u.
3. Izvršite build projekta.

### Linux

1. Klonirajte repo:
   ```bash
   git clone https://github.com/MrMilanP/Simple-DHCP-Server.git
   cd Simple-DHCP-Server
   ```
2. U root direktorijumu projekta kreirajte Makefile:
   ```Makefile
   OBJS	= simple_dhcp_server.o dhcp.o
   SOURCE	= simple_dhcp_server.cpp dhcp.cpp
   HEADER	= 
   OUT	= simple_dhcp_server
   FLAGS	= -g -c -Wall
   LFLAGS	= 
   CC	= g++

   all:	simple_dhcp_server

   simple_dhcp_server: $(OBJS)
	   $(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

   simple_dhcp_server.o: simple_dhcp_server.cpp
	   $(CC) $(FLAGS) simple_dhcp_server.cpp 

   dhcp.o: dhcp.cpp
	   $(CC) $(FLAGS) dhcp.cpp 

   clean:
	   rm -f $(OBJS) $(OUT)
   ```
3. Pokrenite `make` da biste izgradili projekat:
   ```bash
   make
   ```

## Korišćenje

Pokrenite server iz komandne linije:

```bash
sudo ./simple_dhcp_server
```

Server će slušati na portu 67 za dolazne DHCP zahteve.
