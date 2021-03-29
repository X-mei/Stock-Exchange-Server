CFLAGS=-std=gnu++11 -pedantic -ggdb3
EXTRAFLAGS=-lpqxx -lpq -pthread

all: client

client: client.cpp common.h
	g++ $(CFLAGS) -o client client.cpp $(EXTRAFLAGS)

clean:
	rm -f *~ *.o server

clobber:
	rm -f *~ *.o