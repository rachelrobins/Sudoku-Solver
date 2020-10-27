#include <stdio.h>
#include "SPBufferset.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "Game.h"
#include "MainAux.h"
#include "CommandsList.h"
#include "Parser.h"


int collides(Sudoku * puzzle, variable * cell) {
	int i, j, count=0, n=puzzle->n, m=puzzle->m;
	int row = cell->row, col = cell->col, val = cell->val;
	int colBlock = col / n, rowBlock = row / m;

	/*collides in row*/
	for (j = 0; j < n * m; j++) {
		if (puzzle->values[row][j] == 0) {
			puzzle->values[row][j] = val;
			if (j != col && legalPlaceInPuzzle(puzzle, row, j) == 0)
				count++;
			puzzle->values[row][j] = 0;
		}
	}

	/*collides in column*/
	for (i = 0; i < n * m; i++) {
		if (puzzle->values[i][col] == 0) {
			puzzle->values[i][col] = val;
			if (i != row && legalPlaceInPuzzle(puzzle, i, col) == 0)
				count++;
			puzzle->values[i][col] = 0;
		}
	}

	/*collides in block*/
	for (i = m * rowBlock; i < m * rowBlock + m; i++)
		for (j = n * colBlock; j < n * colBlock + n; j++) {
			if (puzzle->values[i][j] == 0) {
				puzzle->values[i][j] = val;
				if ((i != row || j != col) && legalPlaceInPuzzle(puzzle, i, j) == 0)
					count++;
				puzzle->values[i][j] = 0;
			}
		}

	return count;
}


/* Return a guessed legal value for cell in <row,col> with score greater or equal to threshold.
 * The legal values for the cell are scored in cells array, and their scores in sol array accordingly.
 * Cumulative distribution function is used to randomly choose a value according to score if several values hold for the same cell.*/
int fillCellWithGuess(Sudoku * puzzle, int row, int col, variable * cells, double * sol, int indBegin, int indEnd, double threshold) {
	int k;
	double sum = 0.0, random;

	for (k=indBegin; k<=indEnd; k++)
		if (sol[k]<threshold)
			sol[k]=0;
		else {
			puzzle->values[row][col] = cells[k].val;
			if (sol[k]!=0 && legalPlaceInPuzzle(puzzle, row, col)!=0)
				sol[k]=0;
			puzzle->values[row][col] = 0;
		}
	for (k = indBegin; k<=indEnd; k++)
		sum+=sol[k];
	if (sum == 0.0)
		return -1;

	/*Normalizing scores so they all add up to 1.*/
	for (k = indBegin; k<=indEnd; k++)
		sol[k] = sol[k] / sum;

	sum = 0.0;

	/*Adding up score for Cumulative distribution function.*/
	for (k = indBegin; k<=indEnd; k++) {
		sum+=sol[k];
		sol[k] = sum;
	}

	k = indBegin;
	/*Skipping on zero scores.*/
	while(sol[k] == 0.0)
		k++;

	random = (double)rand()/RAND_MAX;

	while (sol[k] < random)
		k++;

	return cells[k].val;
}

int fillGuess (Sudoku * puzzle, variable * cells, int lenList, double * sol, double threshold) {
	int k=0, row, col, indBegin=0, indEnd=0, value;
	commands * guess;
	change * toChange;

	guess = (commands *) malloc(sizeof(commands));
	if (guess == NULL) {
		memoryError();
		handleExit();
		toExit=1;
		return -1;
	}
	guess->commandName = "guess";
	guess->list = NULL;
	guess->next = NULL;
	guess->prev = NULL;

	while (k < lenList) {
		row = cells[k].row;
		col = cells[k].col;
		if (k == lenList - 1 || cells[k+1].row != row || cells[k+1].col != col) {
			/*Going over each section (representing each cell). */
			indEnd=k;
			value = fillCellWithGuess(puzzle, row, col, cells, sol, indBegin, indEnd, threshold);
			if (value != -1) {
				puzzle->values[row][col] = value;
				toChange = (change *) malloc(sizeof(change));
				if (toChange == NULL) {
					memoryError();
					freeFromCommand(guess);
					handleExit();
					toExit = 1;
					return -1;
				}
				/*Adding change made to list stored in command guess.*/
				toChange->row = row;
				toChange->col = col;
				toChange->from = 0;
				toChange->to = value;
				toChange->nextChange = NULL;
				addChange(&(guess->list), toChange);
			}
			indBegin = k + 1;
		}
		k++;
	}


	addCommand(&currentMove, guess);
	freeFromCommand(currentMove->next);
	currentMove->next = NULL;

	return 0;
}

int guessHint(variable * cells, int lenList, double * sol, int x, int y) {
	int k;
	for (k=0; k<lenList; k++)
		if (cells[k].row == y-1 && cells[k].col == x-1 && sol[k]>0)
			giveHintWithScore(x,y, cells[k].val, sol[k]);

	return 0;
}
