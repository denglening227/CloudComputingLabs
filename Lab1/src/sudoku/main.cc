#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>

#include "showError.h"
#include "sudoku.h"

//Multi-thread
int thread_count=2;		//默认双线程
pthread_t* thread_solve;

//Record Data
char puzzle[num_puzzle][128];//保存输入的数独
char solution[num_puzzle][N+1];//保存结果数独,值为数字
int board[num_puzzle][N];//数独棋盘一维数组
int spaces[num_puzzle][N];//待填的空格，值表示在棋盘中的下标

//Counting
int puzzleNum=0;	 	  //数独总数量
int solutionNum = 0;	//解决数独的数目
//Choose a Solution
bool (*solve)(int, int*) = solve_sudoku_dancing_links;//解数独算法
//Time Counting
long int t;
int64_t now() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec; //微秒级
}

void inputData(char* fileName)    //输入部分
{
	//Read inputs
	FILE* testFile = fopen(fileName,"r");
	if(testFile==NULL)
	{
		showError(100);
		exit(1);
	}
	while (fgets(puzzle[puzzleNum], 82, testFile))
	{
		printf("%d ",puzzleNum);
		puts(puzzle[puzzleNum]);
		++puzzleNum;
	}
}

void* sudoku_solve(void* x)
{
	long int cnt = (long int) x;
	int K= puzzleNum/thread_count;
	int first = cnt*K;
	int last = (cnt==thread_count-1)?puzzleNum:(cnt+1)*K;
	for(int i=first; i<last; i++) {


		input(puzzle[i],board[i],spaces[i]);//根据输入的字符串转为一维棋盘中的数字

		if(solve(0 ,board[i])) {
			++solutionNum;
			if (!solved(board[i]));
			//assert(0);
		} else {
			printf("i:%d;No: %s", i,puzzle[i]);
		}
		for(int j=0; j<N; j++)
		{
			solution[i][j]=board[i][j]+'0';  //将求解出的数字转换为字符
		}
		solution[i][N]='\0';
	}

	return NULL;
}

void outputData()      //输出部分
{
	printf("Data Outputing!");
}




int main(int argc, char* argv[]) 
{
	//初始化
	if(argv[1]!=NULL) thread_count = strtol(argv[1],NULL,10);//线程数作为参数输入
	init_neighbors();
	inputData(argv[1]);//输入函数

	int64_t start = now();//计算时间开销

	thread_solve = (pthread_t *)malloc(thread_count*sizeof(pthread_t));
	for(t=0; t<thread_count; t++) 
	{
		pthread_create(&thread_solve[t],NULL,sudoku_solve,(void*)t);
	}
	for(t=0; t<thread_count; t++) 
	{
		pthread_join(thread_solve[t],NULL);
	}

	int64_t end = now();
	double sec = (end-start)/1000000.0;//时间开销
	printf("%f sec %f ms each %d\n", sec, 1000*sec/puzzleNum, solutionNum);

	free(thread_solve);

	outputData();//输出函数
    return 0;
}
