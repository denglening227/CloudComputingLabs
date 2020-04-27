CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

all: httpserver

httpserver: ./src/httpserver.cpp
	g++ -O2 -o $@ $^ #-lpthread

client: ./src/client.cpp
	g++ -O2 -o $@ $^ #-lpthread

clean:
	rm -f ./httpserver
