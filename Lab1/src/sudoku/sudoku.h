#ifndef SUDOKU_H

#define SUDOKU_H

const bool DEBUG_MODE = false;//是否输出中间结果 

enum { ROW=9, COL=9, N = 81, NEIGHBOR = 20 };

const int NUM = 9;

extern int neighbors[N][NEIGHBOR];//neighbors[i][j]表示第i个方格的第j个邻居的下标

extern int (*chess)[COL];//表示棋盘的二维数组 

const int num_puzzle = 100000;  

extern char puzzle[num_puzzle][128];

extern char solution[num_puzzle][N+1];

extern int board[num_puzzle][N];

void init_neighbors();

bool solve_sudoku_dancing_links(int unused,int chessb[N]);

bool solved(int chessb[N]);

void input(const char in[N],int board[N],int spaces[N]);

#endif
