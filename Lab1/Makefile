CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

all: sudoku_solve

sudoku_solve: ./src/sudoku/main.cc ./src/sudoku/neighbor.cc ./src/sudoku/sudoku_dancing_links.cc ./src/sudoku/sudoku_basic.cc
	g++ -O2 -o $@ $^ -lpthread

clean:
	rm -f ./sudoku_solve
