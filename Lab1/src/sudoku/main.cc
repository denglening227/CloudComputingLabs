#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>
#include "showError.h" 	         //print Error msg by errorCode
#include "sudoku.h"		//Need to write! includes Algorithms etc



char puzzle[num_puzzle][128];

char solution[num_puzzle][N+1];

int board[num_puzzle][N];

int spaces[num_puzzle][N];



int puzzleNum=0;//数独总数量

int solutionNum = 0;//解决数独的数目

bool (*solve)(int, int*) = solve_sudoku_dancing_links;//求解数独的算法

int thread_count=2;//默认双线程

pthread_t* thread_solve;

long int t;

int64_t now() {

	struct timeval tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec * 1000000 + tv.tv_usec;

}



void inputdata()    //输入部分
{


//修改一下，与我下面的定义相适应
/*
        //Counting
	
	int solutionNum = 0;
	//Recording Puzzle
	char puzzle[82];	
	
	//Read inputs
	FILE* testFile = fopen(argv[1],"r");
	if(testFile==NULL) 
	{
		showError(100);
		exit(1);
	}
	while (fgets(puzzle, 82, testFile))
	{
		++puzzleNum;
		// Solution Function
		++solutionNum;
		puts(puzzle);
	}

*/


}

void* sudoku_solve(void* x) {

	long int cnt = (long int) x;

	int K= puzzleNum/thread_count;

	int first = cnt*K;

	int last = (cnt==thread_count-1)?puzzleNum:(cnt+1)*K;


	for(int i=first; i<last; i++) {


		input(puzzle[i],board[i],spaces[i]);

		if(solve(0 ,board[i])) {

			++solutionNum;
			if (!solved(board[i]));
			//assert(0);

		} else {
			printf("i:%d;No: %s", i,puzzle[i]);
		}

		
		for(int j=0; j<N; j++) {

			solution[i][j]=board[i][j]+'0';  //将求解出的数字转换为字符
		}
		solution[i][N]='\0';
	}

	return NULL;
}

void outputdata()      //输出部分
{ 

	

}




int main(int argc, char* argv[]) {


	if(argv[1]!=NULL) thread_count = strtol(argv[1],NULL,10);//线程数作为参数输入

	init_neighbors();

	inputdata();

	int64_t start = now();

	thread_solve = (pthread_t *)malloc(thread_count*sizeof(pthread_t));

	for(t=0; t<thread_count; t++) {

		pthread_create(&thread_solve[t],NULL,sudoku_solve,(void*)t);

	}

	for(t=0; t<thread_count; t++) {

		pthread_join(thread_solve[t],NULL);

	}
	
	int64_t end = now();

	double sec = (end-start)/1000000.0;

	printf("%f sec %f ms each %d\n", sec, 1000*sec/puzzleNum, solutionNum);

	free(thread_solve);

	outputdata();

        return 0;
}
