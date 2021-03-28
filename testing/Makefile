all: client

test: client.cpp
	g++ -O3 -std=c++11 -pthread -o client client.cpp

.PHONY:
clean:
	rm -f test *.o *~