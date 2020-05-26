CXXFLAGS+=-std=c++11 -Wno-literal-suffix
CXXFLAGS+=-O2
CXXFLAGS+=-Wall -Wextra

all: kvstore2pcsystem

kvstore2pcsystem: ./src/kvstore2pcsystem.cpp
	g++ -O2 -o $@ $^ -lpthread

clean:
	rm -f ./kvstore2pcsystem
run:
	./kvstoresystem
