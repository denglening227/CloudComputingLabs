#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "showError.h" 	         //print Error msg by errorCode
#include "sudoku.h"		//Need to write! includes Algorithms etc



int nextpuzzleToBeDone=0;
int puzzleNum = 0;  //数独总数量

pthread_mutex_t puzzleQueueMutex=PTHREAD_MUTEX_INITIALIZER;

int recvApuzzle()        //动态分配任务
{

  int currentpuzzleID=0;
  pthread_mutex_lock(&puzzleQueueMutex);
  if(nextpuzzleToBeDone>=puzzleNum) 
  {
    pthread_mutex_unlock(&puzzleQueueMutex);
    return -1;
  }

  currentpuzzleID=nextpuzzleToBeDone;
  nextpuzzleToBeDone++;
  pthread_mutex_unlock(&puzzleQueueMutex);
  return currentpuzzleID;
}



int main(int argc, char* argv[])
{
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
	return 0;
}
