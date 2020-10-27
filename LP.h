/********************************************************************
 * ******************************************************************
 * This module is responsible for all commands which use LP: guess and guess_hint.
 * Additionally, the module contains the auxiliary function for LP - 'collides' (used for calculation of coefficients).
 * We used 'struct variable' for an array of cells and an array of doubles called 'sol' representing their scores from LP.
 * *******************************************************************
 ********************************************************************/

#ifndef LP_H_
#define LP_H_


/* Calculates the number of empty cells in same row, column ot block which can have the same value stored in cell.
 * In the actual board, the cell is empty, and cell.val is legal value for this cell in the board.*/
int collides(Sudoku * puzzle, variable * cell);

/* Fills the board with legal values which have scores bigger than threshold.
 * 'cells' is an array which stores values for cells, in sections - all values for certain cells are one after another.
 * 'sol' is an array which stores all scores for cell, in the same order accordingly to 'cells' array. */
int fillGuess (Sudoku * puzzle, variable * cells, int lenList, double * sol, double threshold);

/* Shows to user all legal values for cell <x, y> and their scores from 'cells' and 'sol' arrays. */
int guessHint(variable * cells, int lenList, double * sol, int x, int y);


#endif /* LP_H_ */
