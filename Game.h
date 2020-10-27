/********************************************************************
 * ******************************************************************
 * This module is responsible for game logic, contains two structs representing boards and cells.
 * Additionally, the module contains the auxiliary functions for getting information from sudoku boards.
 * *******************************************************************
 ********************************************************************/

#ifndef GAME_H_
#define GAME_H_
#include "CommandsList.h"
#define EXIT 100

int markErrors;
int mode;
/*0 for INIT, 1 for SOLVE, 2 for EDIT*/
int toExit;

/*This struct represents a cell. */
typedef struct {
	int row;
	int col;
	int val;
} variable;

/*This strict represents a board. */
typedef struct {
	int m;
	int n;
	int ** values;
	int ** fixed; /*matrix with 1 in fixed cells and 0 for unfixed cells.*/
} Sudoku;

Sudoku * currentPuzzle;
Sudoku * solvedPuzzle;
commands * currentMove; /*The last move we did*/

/* Gets an array of 'emptyCounter' empty Cells, and chooses 'num' of them randomly.
 * -1 is returned if one of the chosen cells has no legal value. */
int chooseRandomValues(Sudoku * puzzle, variable * emptyCells, int emptyCounter, int num);

/* Frees all memory allocated in the sudoku fields.*/
int freeSudoku(Sudoku * toDelete);

/* Returns 0 for unfinished puzzle, 1 for correct finished puzzle and -1 for erroneous finished puzzle. */
int finishedPuzzle(Sudoku * puzzle);

/* Returns 0 for erroneous Puzzle and 1 else. */
int erroneousPuzzle(Sudoku * puzzle);

/*If cell in <row=i, col=j> has only 1 legal value, return it. Otherwise, we return -1. */
int singleLegalValueForCell(Sudoku * puzzle, int i, int j);

/* Returns 0 if value in cell placed in <row, col> is legal in the current board, and -1 otherwise. */
int legalPlaceInPuzzle(Sudoku * puzzle, int row, int col);

/* Returns 0 if values stored in 'num' exists in 'row', and -1 otherwise. */
int existsInRow(Sudoku * puzzle, int row, int num);

/* Returns 0 if values stored in 'num' exists in 'col', and -1 otherwise. */
int existsInCol(Sudoku * puzzle, int col, int num);

/* Returns 0 if values stored in 'num' exists in block, and -1 otherwise. */
int existsInBlock(Sudoku * puzzle, int row, int col, int num);

/* Return 0 if value in cell placed in <row, col> is legal according to fixed cell board, and -1 otherwise. */
int legalFixedPlaceInPuzzle(Sudoku * puzzle, int row, int col);

/* Calculates and returns how many empty cells there are in puzzle. */
int countEmptyCells(Sudoku * puzzle);

/* Fills values in 'puzzle' according to information stored in 'emptyCells' array. */
int fillEmptyCells(Sudoku * puzzle, variable * emptyCells, int emptyCoutner);

/* Sets 0 in 'puzzle according to information stored in 'emptyCells' array. */
int unfillEmptyCells(Sudoku * puzzle, variable * emptyCells, int emptyCounter);

/* Mark y cells in 'puzzle' as fixed, randomly. */
int chooseRandomFixedCells(Sudoku * puzzle, int y);

#endif /* GAME_H_ */
