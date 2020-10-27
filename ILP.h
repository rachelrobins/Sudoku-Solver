/********************************************************************
 * ******************************************************************
 * This module is responsible performing linear programming on sudoku board using gurobi optimizer.
 * Each variable represents a cell and a value.
 * On out target function when index!=0, each variable's coefficient is its number of collides calculated in 'collides' function in LP module.
 * This coefficient represents number of empty cells in same row, column or block which can have the same value of the variable.
 * The logic is that the more collisions the variable has - we will be less likely to choos its value.
 * We would prefer values with less collisions because the meaning is we have less 'options in the environment of the variable' for this value.
 * To make sure this is done, we set the target function as minimum.
 * *******************************************************************
 ********************************************************************/

#ifndef ILP_H_
#define ILP_H_


/* Validates a non-erroneous board using LP with gurobi optimizer.
 * Returns -1 for failure, 0 for unsolvable, 1 for solvable (and if so, saves the solution if solvedPuzzle).
 * index = 0 is for ILP. index = 1 for guess. index = 2 for guess_hint.*/
int solveSudoku(Sudoku * toSolve, int index, double thresholdForGuess, int XForGuessHint, int YForGuessHint);


#endif /* ILP_H_ */
