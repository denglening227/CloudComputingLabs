CXXFLAGS+=-std=c++11 -Wno-literal-suffix
CXXFLAGS+=-O2
CXXFLAGS+=-Wall -Wextra

all: httpserver

httpserver: ./src/httpserver.cpp
	g++ -O2 -o $@ $^ -lpthread

client: ./src/client.cpp
	g++ -O2 -o $@ $^ #-lpthread

clean:
	rm -f ./httpserver
	rm -f ./client
