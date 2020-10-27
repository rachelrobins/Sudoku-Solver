#include <stdio.h>
#include "SPBufferset.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "Game.h"
#include "Print.h"
#include "MainAux.h"

int freeSudoku(Sudoku * toDelete) {
	int m, n, i;
	if (toDelete == NULL)
		return 0;

	m = toDelete->m;
	n = toDelete->n;

	/* Free values matrix. */
	if (toDelete->values != NULL)
		for (i = 0; i < n * m; i++)
			free(toDelete->values[i]);
	free(toDelete->values);
	/* Free fixed matrix. */

	if (toDelete->fixed != NULL)
		for (i = 0; i < n * m; i++)
			free(toDelete->fixed[i]);
	free(toDelete->fixed);

	return 0;
}

int countEmptyCells(Sudoku * toCount) {
	int count=0, i, j, n=toCount->n, m=toCount->m;
	for (i=0; i<n*m; i++)
		for (j=0; j<n*m; j++)
			if (toCount->values[i][j]==0)
				count++;
	return count;
}

/* 0 <= row,col <= m * n - 1*/
int legalPlaceInPuzzle(Sudoku * puzzle, int row, int col) {
	int m = puzzle->m, n = puzzle->n;
	int x, y, rowBlock, colBlock;
	int value = puzzle->values[row][col];
	if (value == 0)
		return 0;

	/*1. Checks there is no same value in the same column. */
	for (y = 0; y < m * n; y++)
		if (y != row && puzzle->values[y][col] == value)
			return -1;

	/*2. Checks there is no same value in the same row. */
	for (x = 0; x < m * n; x++)
		if (x != col && puzzle->values[row][x] == value)
			return -1;

	/*3. Checks there is no same value in the same block. */
	colBlock = col / n;
	rowBlock = row / m;
	for (y = m * rowBlock; y < m * rowBlock + m; y++)
		for (x = n * colBlock; x < n * colBlock + n; x++)
			if ((y != row || x != col) && puzzle->values[y][x] == value)
				return -1;

	return 0;
}

int legalFixedPlaceInPuzzle(Sudoku * puzzle, int row, int col) {
	int m = puzzle->m, n = puzzle->n;
	int x, y, rowBlock, colBlock;
	int value = puzzle->values[row][col];
	if (value == 0 || puzzle->fixed[row][col]!=1)
		return 0;

	/*1. Checks there is no same fixed value in the same column. */
	for (y = 0; y < m * n; y++)
		if (y != row && puzzle->fixed[y][col]==1 && puzzle->values[y][col] == value)
			return -1;

	/*2. Checks there is no same fixed value in the same row. */
	for (x = 0; x < m * n; x++)
		if (x != col && puzzle->fixed[row][x]==1 && puzzle->values[row][x] == value)
			return -1;

	/*3. Checks there is no same fixed value in the same block. */
	colBlock = col / n;
	rowBlock = row / m;
	for (y = m * rowBlock; y < m * rowBlock + m; y++)
		for (x = n * colBlock; x < n * colBlock + n; x++)
			if ((y != row || x != col) && puzzle->fixed[y][x]==1 && puzzle->values[y][x] == value)
				return -1;

	return 0;
}

int existsInRow(Sudoku * puzzle, int row, int num) {
	int col;
	for (col = 0; col < puzzle->n * puzzle->m; col++)
		if (puzzle->values[row][col] == num)
			return 0;
	return -1;
}

int existsInCol(Sudoku * puzzle, int col, int num) {
	int row;
	for (row = 0; row < puzzle->n * puzzle->m; col++)
		if (puzzle->values[row][col] == num)
			return 0;
	return -1;
}

int existsInBlock(Sudoku * puzzle, int rowBlock, int colBlock, int num) {
	int n = puzzle->n, m = puzzle->m, x, y;
	for (y = m * rowBlock; y < m * rowBlock + m; y++)
			for (x = n * colBlock; x < n * colBlock + n; x++)
				if (puzzle->values[y][x] == num)
					return 0;
	return -1;
}

int singleLegalValueForCell (Sudoku * puzzle, int i, int j) {
	int m = puzzle->m, n = puzzle->n, value, count=0, singleValue;
	if (puzzle->values[i][j]!=0)
		return -1;

	/*Count legal values for the given cell. */
	for (value=1; value<=n*m; value++) {
		puzzle->values[i][j] = value;
		if (legalPlaceInPuzzle(puzzle, i, j) == 0) {
			count++;
			singleValue=value;
		}
	}

	/*printf("For row = %d and col = %d there are %d legal values.\n", i, j, count);*/

	puzzle->values[i][j]=0;
	if (count == 1) /*Only one legal value was detected. */
		return singleValue;
	else
		return -1;
}

int randomLegalValue (Sudoku * puzzle, int row, int col) {
	int val, count = 0, n = puzzle->n, m = puzzle->m, place;
	int * legalValues;

	count=0;

	/*1. Counts how many legal values exist for out cell and stores them in 'legalValues'. */
	for (val=1; val<=n*m; val++) {
		puzzle->values[row][col] = val;
		if (legalPlaceInPuzzle(puzzle, row,col)==0)
			count++;
		puzzle->values[row][col] = 0;
	}
	if (count==0) /*no legal value for cell. */
		return -1;

	legalValues = (int *) malloc(count * sizeof(int));
	if (legalValues == NULL) {
		memoryError();
		toExit=1;
		return EXIT;
	}
	count=0;
	for (val = 1; val <= n*m; val++) {
		puzzle->values[row][col] = val;
		if (legalPlaceInPuzzle(puzzle, row, col)==0) {
			legalValues[count] = val;
			count++;
		}
		puzzle->values[row][col]=0;
	}

	/*2. Choose random legal value. */

	place = rand()%count;
	val = legalValues[place];
	free(legalValues);
	return val;
}

int chooseRandomValues (Sudoku * puzzle, variable * emptyCells, int emptyCounter, int num) {
	int val, place, count=0;

	/*Goes over each empty cell (in random order). */
	while (count<num) {
		place = rand()%emptyCounter;
		if (emptyCells[place].val!=0)
			continue;
		/*Choose a random legal value for our cell. */
		val = randomLegalValue(puzzle, emptyCells[place].row, emptyCells[place].col);
		if (val == -1)
			return -1;
		else if (val == EXIT)
			return EXIT;
		emptyCells[place].val = val;
		count++;
	}
	return 0;
}

int fillEmptyCells(Sudoku * puzzle, variable * emptyCells, int emptyCounter) {
	int k;
	for (k=0; k<emptyCounter; k++)
		puzzle->values[emptyCells[k].row][emptyCells[k].col] = emptyCells[k].val;
	return 0;
}

int unfillEmptyCells(Sudoku * puzzle, variable * emptyCells, int emptyCounter) {
	int k;
	for (k=0; k<emptyCounter; k++) {
		puzzle->values[emptyCells[k].row][emptyCells[k].col] = 0;
		emptyCells[k].val = 0;
	}
	return 0;
}

int chooseRandomFixedCells (Sudoku * puzzle, int y) {
	int count = 0, row, col, n = puzzle->n, m = puzzle->m;
	for (row=0; row < n*m; row++)
		for(col = 0; col < n*m; col++)
			puzzle->fixed[row][col]=0;

	while (count < y) {
		row = rand()%(m*n);
		col = rand()%(m*n);
		if (puzzle->fixed[row][col]!=0)
			continue;
		puzzle->fixed[row][col] = 1;
		count++;
	}

	return 0;
}

int erroneousPuzzle(Sudoku * puzzle) {
	int i, j, n = puzzle->n, m = puzzle->m;
	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++)
			if (legalPlaceInPuzzle(puzzle, i, j)==-1)
				return 0;
	return 1;
}

int finishedPuzzle(Sudoku * puzzle) {
	int i, j, n = puzzle->n, m = puzzle->m;

	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++)
			if (puzzle->values[i][j] == 0)
				return 0;

	if (erroneousPuzzle(puzzle) == 0)
		return -1;

	return 1;
}
