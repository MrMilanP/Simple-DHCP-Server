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
