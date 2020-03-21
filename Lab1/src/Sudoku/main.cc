#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "sudoku.h"

int64_t now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main(int argc, char* argv[])
{
  init_neighbors();

  FILE* fp = fopen(argv[1], "r");	//  ./sudoku test1 a  test1是输入文件名
  char puzzle[128];
  int total_solved = 0;
  int total = 0;
  bool (*solve)(int) = solve_sudoku_basic;	//默认用basic算法
  if (argv[2] != NULL)	
  //第二个参数  a-min_arity c-min_arity_cache d-dancing_links   用于指定使用的算法 如果不指定 就用basic
    if (argv[2][0] == 'a')
      solve = solve_sudoku_min_arity;
    else if (argv[2][0] == 'c')
      solve = solve_sudoku_min_arity_cache;
    else if (argv[2][0] == 'd')
      solve = solve_sudoku_dancing_links;
  int64_t start = now();
  while (fgets(puzzle, sizeof puzzle, fp) != NULL) {	//IO操作 从输入文件中按行读入puzzle
  printf(puzzle);		//打印一下读入的数独字符串
    if (strlen(puzzle) >= N) {		//N定义在sudoku.h中 值为81
      ++total;			//解题数量
      input(puzzle);	//这个方法在sudoku_basic.cc中实现  所有解题算法共用这一个input方法
      init_cache();		//定义在sudoku_min_arity_cache中 暂时不知道为什么任何解法都调用这个函数
      //if (solve_sudoku_min_arity_cache(0)) {
      //if (solve_sudoku_min_arity(0))
      //if (solve_sudoku_basic(0)) {
      
       //*solve是个函数指针  solve()相当于调用这个函数 也就是说这里的solve对应者以上不同算法的解题主函数
       //如果求解成功返回了true
      if (solve(0)) {		
      //成功求解计数增加
        ++total_solved;
        if (!solved())	//即使solve返回了true 也要对整个解进行横列、九宫格的校验
          assert(0);
      }
      else {//solve返回了false 表示无解
        printf("No: %s", puzzle);	//输出：NO： puzzle  表示该题目无解
      }
    }
  }
  int64_t end = now();	//获取结束执行时间
  double sec = (end-start)/1000000.0;
  printf("%f sec %f ms each %d\n", sec, 1000*sec/total, total_solved);

  return 0;
}

