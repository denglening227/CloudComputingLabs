#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "showError.h" 	//print Error msg by errorCode
#include "sudoku.h"		//Need to write! includes Algorithms etc

int main(int argc, char* argv[])
{
	//Counting
	int puzzleNum = 0;
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
