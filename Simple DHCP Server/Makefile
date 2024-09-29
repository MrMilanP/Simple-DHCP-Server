# Makefile za Simple DHCP Server

# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -g

# Izvorni fajlovi
SRCS = dhcp.cpp main.cpp
# Ciljni fajl
TARGET = simple_dhcp_server

# Pravilo za pravljenje cilja
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Čisti projekat
clean:
	rm -f $(TARGET)
