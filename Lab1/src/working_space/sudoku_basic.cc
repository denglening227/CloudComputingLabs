#include <assert.h>
#include <stdio.h>

#include <algorithm>

#include "sudoku.h"

int board[N];
int spaces[N];
int nspaces;
int (*chess)[COL] = (int (*)[COL])board;

//在main.cc中调用  作用是记录输入题中空格的位置情况
static void find_spaces()
{
  nspaces = 0;
  for (int cell = 0; cell < N; ++cell) {
    if (board[cell] == 0)		//board[]存的是初始题 但会随着递归的过程慢慢变化 也就是为0的位置慢慢填上正确值
      spaces[nspaces++] = cell; //nspaces是初始题目的空格数 spaces[n] 存的是第n个空格对应原题目中的格子索引
  }
}

//全局通过这个函数进行数据读入 （不涉及I/O 只是把I/O读入的字符串转化成int数组）
void input(const char in[N])
{
  for (int cell = 0; cell < N; ++cell) {
    board[cell] = in[cell] - '0';
    assert(0 <= board[cell] && board[cell] <= NUM);
    //printf("%d",board[cell]);	//打印读入的题
  }
  find_spaces();
}

//检查某一个单元格 当选填guess这个数字时 是否与邻居单元格冲突 sudoku.h中定义neighbor为20个 即横纵16+四个角=20
bool available(int guess, int cell)
{
  for (int i = 0; i < NEIGHBOR; ++i) {
    int neighbor = neighbors[cell][i];
    if (board[neighbor] == guess) {
      return false;
    }
  }
  return true;
}

//递归求解
bool solve_sudoku_basic(int which_space)
{
	
  //这里返回true说明找到了可行解 
  if (which_space >= nspaces) {
  
  //打印可行解
/*  printf("\nsolution: \n");
  for(int cell=0;cell<N;cell++){
    printf("%d",board[cell]);
  }
  printf("\n"); */
  
    return true;
  }

  // find_min_arity(which_space);
  int cell = spaces[which_space];	//which_space表示的是本次递归讨论的是第几个空格子 （spaces[wich_space]表示这个空格子在原题目数组中的索引）

  //依次猜测本空格可填的数字 1到9
  for (int guess = 1; guess <= NUM; ++guess) {
  //校验这个猜测数字是否可行
    if (available(guess, cell)) {
      // hold
      assert(board[cell] == 0);	//断言  如果此时的cell位置不为0 那就终止程序 （这个情况理论上不会出现）
      board[cell] = guess;  //当前空填上guess 

      // try				
      //递归下一空  如果整个后续都返回了true 说明这一轮的guess是有可以满足可行解的 那么这一空也返回true
      if (solve_sudoku_basic(which_space+1)) {
        return true;
      }

	  //如果刚刚下一空之后的递归失败了 说明本轮的guess不满足可行解
      // unhold
      //断言  确保刚刚的递归没有修改board的值 也就是每一次失败的递归 都会不会导致参数发生改变
      assert(board[cell] == guess);	
      //本次尝试归0 进行下一个guess的验证
      board[cell] = 0;
    }
  }
  //如果当前空的9个数都构成不了可行解 说明此题无解 返回false
  return false;
}
