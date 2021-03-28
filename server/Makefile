CFLAGS=-std=gnu++11 -pedantic -ggdb3
EXTRAFLAGS=-lpqxx -lpq -pthread

all: server

server: main.cpp common.h database.h database.cpp parser.h parser.cpp server.h server.cpp
	g++ $(CFLAGS) -o server main.cpp database.cpp parser.cpp server.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o server

clobber:
	rm -f *~ *.o
