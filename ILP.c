#include <stdio.h>
#include "SPBufferset.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "Game.h"
#include "MainAux.h"
#include "gurobi_c.h"
#include "Print.h"
#include "ILP.h"
#include "Game.h"
#include "LP.h"
#include "Parser.h"


int solveSudoku(Sudoku * toSolve, int index, double thresholdForGuess,
		int XForGuessHint, int YForGuessHint) {
	int m = toSolve->m, n = toSolve->n;
	variable * legalValues;
	GRBenv *env = NULL;
	GRBmodel *model = NULL;
	double * sol;
	int * ind;
	double * obj;
	char * vtype;
	double * con;
	double objval;
	int lenOfArray = 0, count = 0, i, j, value, error, optimstatus, k;
	int curPlace, curRow, curCol, indBegin, indEnd, curRowBlock, curColBlock;

	/********************************************************************
	 * ******************************************************************
	 * PART 1
	 * 		Count how many legal values for empty cells there are in board, and store it in 'lenOfArray'.
	 * 		If there is a cell with no legal value, than the sudoku is unsolvable, and we return 0.
	 * 		If lenOfArray = 0, than the sudoku is finished and solvable, and we return 1.
	 * ******************************************************************
	 ********************************************************************/

	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++)
			if (toSolve->values[i][j] == 0) {
				count = 0;
				for (value = 1; value <= n * m; value++) {
					toSolve->values[i][j] = value;
					if (legalPlaceInPuzzle(toSolve, i, j) == 0)
						count++;
					toSolve->values[i][j] = 0;
				}
				if (count == 0)
					return 0;
				else
					lenOfArray += count;
			}

	if (lenOfArray == 0) {
		freeSudoku(solvedPuzzle);
		free(solvedPuzzle);
		solvedPuzzle = (Sudoku *) malloc(sizeof(Sudoku));
		if (solvedPuzzle == NULL) {
			memoryError();
			handleExit();
			toExit = 1;
			return -1;
		}
		solvedPuzzle->n = toSolve->n;
		solvedPuzzle->m = toSolve->m;
		solvedPuzzle->values = (int **) malloc(n * m * sizeof(int*));
		solvedPuzzle->fixed = (int **) malloc(n * m * sizeof(int*));
		if (solvedPuzzle->values == NULL || solvedPuzzle->fixed == NULL) {
			memoryError();
			free(solvedPuzzle);
			solvedPuzzle = NULL;
			handleExit();
			toExit = 1;
			return -1;
		}
		for (i = 0; i < n * m; i++) {
			solvedPuzzle->values[i] = (int *) malloc(n * m * sizeof(int));
			solvedPuzzle->fixed[i] = (int *) malloc(n * m * sizeof(int));
			if (solvedPuzzle->values[i] == NULL
					|| solvedPuzzle->values[i] == NULL) {
				memoryError();
				free(solvedPuzzle);
				free(solvedPuzzle->values);
				free(solvedPuzzle->fixed);
				solvedPuzzle = NULL;
				handleExit();
				toExit = 1;
				return -1;
			}
			for (j = 0; j < n * m; j++) {
				solvedPuzzle->values[i][j] = toSolve->values[i][j];
				solvedPuzzle->fixed[i][j] = toSolve->fixed[i][j];
			}
		}
		return 1;
	}

	/********************************************************************
	 * ******************************************************************
	 * PART 2
	 * 		Fill the 'legalValues' array with all legal values for all empty cells in board.
	 * 		Each of those will be a 'variable' in the linear programming.
	 * ******************************************************************
	 ********************************************************************/

	k = 0;
	legalValues = (variable *) malloc(lenOfArray * sizeof(variable));
	if (legalValues == NULL) {
		memoryError();
		handleExit();
		toExit = 1;
		return -1;
	}

	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++)
			if (toSolve->values[i][j] == 0)
				for (value = 1; value <= n * m; value++) {
					toSolve->values[i][j] = value;
					if (legalPlaceInPuzzle(toSolve, i, j) == 0) {
						legalValues[k].row = i;
						legalValues[k].col = j;
						legalValues[k].val = value;
						k++;
					}
					toSolve->values[i][j] = 0;
				}

	/*printf("Number of variables = %d:\t", lenOfArray);
	for (k = 0; k < lenOfArray; k++)
		printf("x(%d,%d,%d)\t", legalValues[k].row, legalValues[k].col,
				legalValues[k].val);
	printf("\n");*/

	/********************************************************************
	 * ******************************************************************
	 * PART 3
	 * 		Create gurobi environment & model.
	 * 		If we are in ILP, than we create 'lenOfArray' BINARY variables with 0 as coefficients.
	 * 		If we are in LP, than we create 'lenOfArray' CONTINUOUS variable with coefficients calculated by 'collides'.
	 *		Set the target function to minimum.
	 * ******************************************************************
	 ********************************************************************/

	/*ILP*/
	error = GRBloadenv(&env, "mip1.log");
	if (error) {
		printf("ERROR %d GRBloadenv(): %s\n", error, GRBgeterrormsg(env));
		free(legalValues);
		return -1;
	}

	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		printf("ERROR %d GRBsetintparam(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}

	/*empty model*/
	error = GRBnewmodel(env, &model, "mip1", 0, NULL, NULL, NULL, NULL, NULL);
	if (error) {
		printf("ERROR %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(env));
		free(legalValues);
		GRBfreeenv(env);
		return -1;
	}

	/*variables*/
	obj = (double *) malloc(lenOfArray * sizeof(double));
	vtype = (char *) malloc(lenOfArray * sizeof(char));

	if (obj == NULL || vtype == NULL) {
		memoryError();
		free(legalValues);
		free(obj);
		free(vtype);
		GRBfreemodel(model);
		GRBfreeenv(env);
		handleExit();
		toExit = 1;
		return -1;
	}

	if (index == 0) /* ILP */
		for (k = 0; k < lenOfArray; k++) {
			obj[k] = 0;
			vtype[k] = GRB_BINARY;
		}
	else /* LP */
		for (k = 0; k < lenOfArray; k++) {
			obj[k] = collides(toSolve, &legalValues[k]);
			vtype[k] = GRB_CONTINUOUS;
		}
	error = GRBaddvars(model, lenOfArray, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	if (error) {
		printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		free(legalValues);
		free(obj);
		free(vtype);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}

	/*target function*/
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
	if (error) {
		printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
		free(legalValues);
		free(obj);
		free(vtype);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}

	error = GRBupdatemodel(model);
	if (error) {
		printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
		free(legalValues);
		free(obj);
		free(vtype);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}

	/********************************************************************
	 * ******************************************************************
	 * PART 4
	 * 		Create constrains.
	 * ******************************************************************
	 ********************************************************************/

	ind = (int *) malloc(n * m * sizeof(int));
	con = (double *) malloc(n * m * sizeof(double));

	if (ind == NULL || con == NULL) {
		memoryError();
		handleExit();
		free(legalValues);
		free(obj);
		free(vtype);
		free(ind);
		free(con);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}

	/*Range constraints for each variables. */
	if (index != 0)
		for (k = 0; k < lenOfArray; k++) {
			ind[0] = k;
			con[0] = 1;
			error = GRBaddconstr(model, 1, ind, con, GRB_GREATER_EQUAL, 0,
					"range constraint: ");
			if (error) {
				printf("ERROR %d 1st GRBaddconsr(): %s\n", error,GRBgeterrormsg(env));
				free(legalValues);
				free(obj);
				free(vtype);
				free(ind);
				free(con);
				GRBfreemodel(model);
				GRBfreeenv(env);
				return -1;
			}
		}

	/*Cell constraints for each empty cell*/
	curPlace = 0;
	count = 0;
	while (curPlace < lenOfArray) {
		curRow = legalValues[curPlace].row;
		curCol = legalValues[curPlace].col;
		ind[count] = curPlace;
		con[count] = 1;
		count++;
		if (curPlace == lenOfArray - 1 || legalValues[curPlace + 1].row != curRow || legalValues[curPlace + 1].col != curCol) {
			error = GRBaddconstr(model, count, ind, con, GRB_EQUAL, 1, "cell constraint: ");
			if (error) {
				printf("ERROR %d 1st GRBaddconsr(): %s\n", error, GRBgeterrormsg(env));
				free(legalValues);
				free(obj);
				free(vtype);
				free(ind);
				free(con);
				GRBfreemodel(model);
				GRBfreeenv(env);
				return -1;
			}
			count = 0;
		}
		curPlace++;
	}

	/*Row constraints for each missing value in each row. */
	curPlace = 0;
	for (curRow = 0; curRow < n * m; curRow++) {
		/*finds the relevant variables on 'legalValue' array*/
		if (curPlace == lenOfArray)
			break;
		if (legalValues[curPlace].row != curRow)
			continue;
		indBegin = curPlace;
		while (curPlace < lenOfArray && legalValues[curPlace].row == curRow)
			curPlace++;
		indEnd = curPlace - 1;

		for (value = 1; value <= n * m; value++) {
			count = 0;
			for (curPlace = indBegin; curPlace <= indEnd; curPlace++) {
				if (legalValues[curPlace].val == value) {
					ind[count] = curPlace;
					con[count] = 1;
					count++;
				}
			}
			if (count == 0 && existsInRow(toSolve, curRow, value) == -1) { /*'value' is no legal in any of 'row' cells. */
				free(legalValues);
				free(obj);
				free(vtype);
				free(ind);
				free(con);
				GRBfreemodel(model);
				GRBfreeenv(env);
				return 0;
			} else if (count == 0)
				continue;

			error = GRBaddconstr(model, count, ind, con, GRB_EQUAL, 1, "row constraint: ");
			if (error) {
				printf("ERROR %d 1st GRBaddconsr(): %s\n", error, GRBgeterrormsg(env));
				free(legalValues);
				free(obj);
				free(vtype);
				free(ind);
				free(con);
				GRBfreemodel(model);
				GRBfreeenv(env);
				return -1;
			}
		}
	}

	/*Col constraints for each missing value in each col. */
	for (curCol = 0; curCol < n * m; curCol++)
		for (value = 1; value <= n * m; value++) {
			count = 0;
			for (curPlace = 0; curPlace < lenOfArray; curPlace++)
				if (legalValues[curPlace].col == curCol
						&& legalValues[curPlace].val == value) {
					ind[count] = curPlace;
					con[count] = 1;
					count++;
				}
			if (count == 0 && existsInCol(toSolve, curCol, value) == -1) {
				free(legalValues);
				free(obj);
				free(vtype);
				free(ind);
				free(con);
				GRBfreemodel(model);
				GRBfreeenv(env);
				return 0;
			} else if (count == 0)
				continue;

			error = GRBaddconstr(model, count, ind, con, GRB_EQUAL, 1, "col constraint: ");
			if (error) {
				printf("ERROR %d 1st GRBaddconsr(): %s\n", error, GRBgeterrormsg(env));
				free(legalValues);
				free(obj);
				free(vtype);
				free(ind);
				free(con);
				GRBfreemodel(model);
				GRBfreeenv(env);
				return -1;
			}

		}

	/*Block constrains for each missing value on each block*/
	for (curRowBlock = 0; curRowBlock < n; curRowBlock++)
		for (curColBlock = 0; curColBlock < m; curColBlock++)
			for (value = 1; value <= n * m; value++) {
				count = 0;
				for (curPlace = 0; curPlace < lenOfArray; curPlace++)
					if (legalValues[curPlace].row / m == curRowBlock
							&& legalValues[curPlace].col / n == curColBlock
							&& legalValues[curPlace].val == value) {
						ind[count] = curPlace;
						con[count] = 1;
						count++;
					}
				if (count == 0 && existsInBlock(toSolve, curRowBlock, curColBlock, value) == -1) {
					free(legalValues);
					free(obj);
					free(vtype);
					free(ind);
					free(con);
					GRBfreemodel(model);
					GRBfreeenv(env);
					return 0;
				} else if (count == 0)
					continue;

				error = GRBaddconstr(model, count, ind, con, GRB_EQUAL, 1, "block constraint: ");
				if (error) {
					printf("ERROR %d 1st GRBaddconsr(): %s\n", error, GRBgeterrormsg(env));
					free(legalValues);
					free(obj);
					free(vtype);
					free(ind);
					free(con);
					GRBfreemodel(model);
					GRBfreeenv(env);
					return -1;
				}
			}

	/********************************************************************
	 * ******************************************************************
	 * PART 5
	 * 		Optimize the LP problem, and get the 'optimstatus'.
	 * 	 	If optimstatus!=GRB_OPTIMAL, than the sudoku is unsolvable, and we return 0.
	 * 	 	Get the soluions into 'sol' array (which will store a score for each legal value on each empty cell).
	 * ******************************************************************
	 ********************************************************************/

	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		free(legalValues);
		free(obj);
		free(vtype);
		free(ind);
		free(con);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		free(legalValues);
		free(vtype);
		free(ind);
		free(con);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}

	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		free(legalValues);
		free(obj);
		free(vtype);
		free(ind);
		free(con);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}

	if (optimstatus != GRB_OPTIMAL) {
		free(legalValues);
		free(obj);
		free(vtype);
		free(ind);
		free(con);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return 0;
	}

	error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
	if (error) {
		printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
		free(legalValues);
		free(obj);
		free(vtype);
		free(ind);
		free(con);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}
	sol = (double *) malloc(lenOfArray * sizeof(double));
	if (sol == NULL) {
		memoryError();
		handleExit();
		toExit = 1;
		free(legalValues);
		free(obj);
		free(vtype);
		free(ind);
		free(con);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}

	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, lenOfArray, sol);
	if (error) {
		printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
		free(legalValues);
		free(obj);
		free(vtype);
		free(ind);
		free(con);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}

	/********************************************************************
	 * ******************************************************************
	 * PART 6
	 * 		If index = 0, than we are on ILP and we only need to save the solutions and to return 1;
	 * 		if index = 1, than we are on guess, and we will use fillGuess to complete the command.
	 * 		If index = 2, than we are on guess_hint, and we will use guessHint to complete the command.
	 * ******************************************************************
	 ********************************************************************/

	if (optimstatus == GRB_OPTIMAL) {
		if (index == 0) {
			freeSudoku(solvedPuzzle);
			free(solvedPuzzle);
			solvedPuzzle = (Sudoku *) malloc(sizeof(Sudoku));
			if (solvedPuzzle == NULL) {
				memoryError();
				handleExit();
				toExit = 1;
				free(legalValues);
				free(obj);
				free(vtype);
				free(ind);
				free(con);
				free(sol);
				GRBfreemodel(model);
				GRBfreeenv(env);
				return -1;
			}
			solvedPuzzle->n = toSolve->n;
			solvedPuzzle->m = toSolve->m;
			solvedPuzzle->values = (int **) malloc(n * m * sizeof(int*));
			solvedPuzzle->fixed = (int **) malloc(n * m * sizeof(int*));
			if (solvedPuzzle->values == NULL || solvedPuzzle->fixed == NULL) {
				memoryError();
				free(solvedPuzzle);
				solvedPuzzle = NULL;
				handleExit();
				toExit = 1;
				free(legalValues);
				free(obj);
				free(vtype);
				free(ind);
				free(con);
				free(sol);
				GRBfreemodel(model);
				GRBfreeenv(env);
				return -1;
			}
			for (i = 0; i < n * m; i++) {
				solvedPuzzle->values[i] = (int *) malloc(n * m * sizeof(int));
				solvedPuzzle->fixed[i] = (int *) malloc(n * m * sizeof(int));
				if (solvedPuzzle->values[i] == NULL || solvedPuzzle->values[i] == NULL) {
					memoryError();
					free(solvedPuzzle);
					free(solvedPuzzle->values);
					free(solvedPuzzle->fixed);
					solvedPuzzle = NULL;
					handleExit();
					toExit = 1;
					free(legalValues);
					free(obj);
					free(vtype);
					free(ind);
					free(con);
					free(sol);
					GRBfreemodel(model);
					GRBfreeenv(env);
					return -1;
				}
			}
			for (i=0; i < n * m; i++)
				for (j = 0; j < n * m; j++) {
					solvedPuzzle->values[i][j] = toSolve->values[i][j];
					solvedPuzzle->fixed[i][j] = toSolve->fixed[i][j];
				}
			for (k = 0; k < lenOfArray; k++) {
				if (sol[k] == 1)
					solvedPuzzle->values[legalValues[k].row][legalValues[k].col] = legalValues[k].val;
			}
		} else if (index == 1)
			fillGuess(toSolve, legalValues, lenOfArray, sol, thresholdForGuess);
		else
			guessHint(legalValues, lenOfArray, sol, XForGuessHint, YForGuessHint);
	} else if (optimstatus != GRB_INF_OR_UNBD)
		printf("Optimizations was stopped early\n");

	free(legalValues);
	free(obj);
	free(vtype);
	free(ind);
	free(con);
	free(sol);
	GRBfreemodel(model);
	GRBfreeenv(env);

	/*printf("Validation Over!\n");*/

	if (optimstatus == GRB_OPTIMAL)
		return 1;
	else if (optimstatus == GRB_INF_OR_UNBD)
		return 0;
	return -1;
}
