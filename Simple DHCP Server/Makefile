# Makefile za Simple DHCP Server

# Definiši varijable za kompajler i opcije
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Izvorni fajlovi
SRC = simple_dhcp_server.cpp dhcp.cpp
# Ciljni fajl
TARGET = dhcp_server

# Pravilo za pravljenje cilja
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Čisti projekat
clean:
	rm -f $(TARGET)
