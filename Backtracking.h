/********************************************************************
 * ******************************************************************
 * This module finds the number of different solutions to the board using exhaustive backtracking.
 * We define a struct to be user as a stack for recursive behavior.
 * This module contains 'backtracking' function as the main logic and other auxiliary functions.
 * *******************************************************************
 ********************************************************************/
#ifndef BACKTRACKING_H_
#define BACKTRACKING_H_

typedef struct elem {
	int row; /*Between 1 and n*m*/
	int col;
	int val;
	int numSol;
	struct elem * next;
} elem;

/* Solves board without mistakes using exhaustive backtracking.
 * Recursive behavior is implemented using stack. */
int backtracking(Sudoku * toSolve);

#endif /* BACKTRACKING_H_ */
