CFLAGS=-std=gnu++11 -pedantic -ggdb3
EXTRAFLAGS=-lpqxx -lpq -pthread

all: server

server: main.cpp  database.h database.cpp 
	g++ $(CFLAGS) -o server main.cpp database.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o server

clobber:
	rm -f *~ *.o
