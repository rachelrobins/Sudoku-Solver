#include <stdio.h>
#include "SPBufferset.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "Game.h"
#include "Parser.h"
#include "MainAux.h"
#include "Parser.h"
#include "Files.h"
#include "Print.h"



int main() {
	int temp; /*Command's output will be stored here.*/
	updateModesToCommands();
	mode = 0; /*INIT mode.*/
	toExit=0;
	srand(time(0));
	markErrors=1;
	currentPuzzle= NULL;
	solvedPuzzle=NULL;

	printf("-------------- Welcome To Yarin & Rachel's Sudoku Game! --------------\n");

	while (toExit == 0) { /*Game flow - as long as we didn't exit the game.*/
		temp = selectCommand();
		if (temp == EXIT)
			toExit = 1;
	}

	return 0;
}
