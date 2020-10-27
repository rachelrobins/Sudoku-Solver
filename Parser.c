#include <stdio.h>
#include "SPBufferset.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "Game.h"
#include "string.h"
#include "Parser.h"
#include "MainAux.h"
#include "Files.h"
#include "Print.h"
#include "CommandsList.h"
#include "Backtracking.h"
#include "ILP.h"
#define NUMBER_OF_ITERATIONS 1000

/*Handles 'solve' command, checks propriety, enters SOLVE mode and cleans commandsList. */
int handleSolve(char * parameter1, char * parameter2) {
	Sudoku * puzzle;
	int isFinish, i, j, n, m;

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 == NULL) {
		notEnoughParameters(1, "solve");
		return -1;
	}

	if (parameter2 != NULL) {
		toManyParameters(1, "solve");
		return -1;
	}

	/*2. Loading the puzzle from file. */
	puzzle = loadFile(parameter1);
	if (puzzle == NULL) /*the load failed*/
		return -1;
	n = puzzle->n;
	m = puzzle->m;

	/*3. Checking if there is a fixed cell which if erroneous or zero. */
	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++)
			if (legalFixedPlaceInPuzzle(puzzle, i, j) == -1) {
				fixedErroneousCell();
				freeSudoku(puzzle);
				free(puzzle);
				return -1;
			} else if (puzzle->fixed[i][j] == 1 && puzzle->values[i][j] == 0) {
				zeroFixed();
				freeSudoku(puzzle);
				free(puzzle);
				return -1;
			}

	/*4. Saving the puzzle, cleaning commandsList and freeing allocated memory.*/
	freeSudoku(currentPuzzle);
	freeSudoku(solvedPuzzle);
	while (currentMove != NULL && currentMove->prev != NULL)
		currentMove = currentMove->prev;
	freeFromCommand(currentMove);
	free(currentPuzzle);
	free(solvedPuzzle);
	currentPuzzle = puzzle;
	solvedPuzzle = NULL;

	/*5. Checking if board is finished*/
	isFinish = finishedPuzzle(currentPuzzle);
	if (isFinish == -1) {
		erroneousFinishedBoard();
	} else if (isFinish == 1) {
		printSudoku(currentPuzzle);
		successfullyFinishedBoard();
		mode = 0;
		return 0;
	}

	/*6. Updating current Move. */
	currentMove = (commands *) malloc(sizeof(commands));
	if (currentMove == NULL) {
		memoryError();
		free(currentPuzzle);
		return -1;
	}
	currentMove->commandName = "solve";
	currentMove->list = NULL;
	currentMove->next = NULL;
	currentMove->prev = NULL;

	/*7. Entering SOLVE mode and printing the puzzle. */
	mode = 1;
	printSudoku(currentPuzzle);
	return 0;
}

/*Handles 'edit' command, checks propriety, enters EDIT mode and cleans commandsList. */
int handleEdit(char * parameter1, char * parameter2) {
	Sudoku * puzzle;

	/*1. Making sure we have accurate number of parameters. */
	if (parameter2 != NULL) {
		printf("To many parameters were entered.\n");
		printf("The command edit should receive 0-1 parameters.\n");
		return 0;
	}

	/*2. Loading the puzzle from file (OR 3X3 default sudoku). */
	if (parameter1 == NULL)
		puzzle = defaultSudoku();
	else
		puzzle = loadFile(parameter1);

	if (puzzle == NULL)
		return -1; /*load failed.*/

	/*3. Saving the puzzle, cleaning commandsList and freeing allocated memory.*/
	freeSudoku(currentPuzzle);
	freeSudoku(solvedPuzzle);
	while (currentMove != NULL && currentMove->prev != NULL)
		currentMove = currentMove->prev;
	freeFromCommand(currentMove);
	free(currentPuzzle);
	free(solvedPuzzle);
	currentPuzzle = puzzle;
	solvedPuzzle = NULL;

	/*4. Updating current Move. */
	currentMove = (commands *) malloc(sizeof(commands));
	if (currentMove == NULL) {
		memoryError();
		free(currentPuzzle);
		handleExit();
		return EXIT;
	}
	currentMove->commandName = "edit";
	currentMove->list = NULL;
	currentMove->next = NULL;
	currentMove->prev = NULL;


	/*5. Entering SOLVE mode and printing the puzzle. */
	mode = 2;
	printSudoku(currentPuzzle);
	return 0;
}

/*Handles 'mark_errors' command, checks propriety and sets mark_errors to desired, if parameter is in range. */
int handleMarkErrors(char * parameter1, char * parameter2) {

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 == NULL) {
		notEnoughParameters(1, "mark_errors");
		return -1;
	}

	if (parameter2 != NULL) {
		toManyParameters(1, "mark_errors");
		return -1;
	}

	/*2. Sets mark_errors according to input. */
	if (strcmp(parameter1, "0") == 0)
		markErrors = 0;
	else if (strcmp(parameter1, "1") == 0)
		markErrors = 1;
	else {
		iLegalRange(1, "mark_errors", 0, 1);
		return -1;
	}

	return 0;
}

/*Handles 'print_board' command, checks propriety.*/
int handlePrintBoard(char * parameter1) {

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 != NULL) {
		toManyParameters(0, "print_board");
		return -1;
	}

	/*2. Printing the board. */
	printSudoku(currentPuzzle);
	return 0;

}

/*Handles 'set' command, checks propriety, conducts set command and updates commandsList. */
int handleSet(char * parameter1, char * parameter2, char * parameter3,
		char * parameter4) {
	int x, y, z, m = currentPuzzle->m, n = currentPuzzle->n, isFinish;
	int oldValue;
	commands * set;
	change * toChange;

	/*1. Making sure we have accurate number of parameters in range. */
	if (parameter1 == NULL || parameter2 == NULL || parameter3 == NULL) {
		notEnoughParameters(3, "set");
		return -1;
	}
	if (parameter4 != NULL) {
		toManyParameters(3, "set");
		return -1;
	}
	x = atoi(parameter1);
	if (x < 1 || x > n * m) {
		iLegalRange(1, "set", 1, n * m);
		return -1;
	}
	y = atoi(parameter2);
	if (y < 1 || y > n * m) {
		iLegalRange(2, "set", 1, n * m);
		return -1;
	}
	if (strcmp(parameter3, "0") == 0)
		z = 0;
	else {
		z = atoi(parameter3);
		if (z <= 0 || z > n * m) {
			iLegalRange(3, "set", 0, n * m);
			return -1;
		}
	}

	/*2. Making sure that the user isn't trying to use 'set' for fixed cell in SOLVE mode. */
	if (mode == 1 && currentPuzzle->fixed[y - 1][x - 1] == 1) {
		updateFixedCellsError();
		return -1;
	}

	/*3. Conducting 'set' command and updating commandsList. */
	oldValue = currentPuzzle->values[y - 1][x - 1];

	set = (commands *) malloc(sizeof(commands));
	if (set == NULL) {
		memoryError();
		handleExit();
		return EXIT;
	}
	set->commandName = "set";
	set->list = NULL;
	set->next = NULL;
	set->prev = NULL;
	toChange = (change *) malloc(sizeof(change));
	if (toChange == NULL) {
		memoryError();
		freeFromCommand(set);
		handleExit();
		return EXIT;
	}
	toChange->row = y - 1;
	toChange->col = x - 1;
	toChange->from = oldValue;
	toChange->to = z;
	toChange->nextChange = NULL;
	addChange(&(set->list), toChange);
	addCommand(&currentMove, set);
	freeFromCommand(currentMove->next);
	currentMove->next = NULL;
	currentPuzzle->values[y - 1][x - 1] = z;

	if (z == 0 && currentPuzzle->fixed[y - 1][x - 1] == 1)
		currentPuzzle->fixed[y - 1][x - 1] = 0;
	printSudoku(currentPuzzle);

	/*4. Checking if board is finished. */
	if (mode == 1) {
		isFinish = finishedPuzzle(currentPuzzle);
		if (isFinish == -1)
			erroneousFinishedBoard();
		else if (isFinish == 1) {
			successfullyFinishedBoard();
			mode = 0;
		}
	}

	return 0;
}

/*Handles 'validate' command, checks propriety and solves board with ILP using solveSudoku function. */
int handleValidate(char * parameter1) {
	int check;

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 != NULL) {
		toManyParameters(0, "validate");
		return -1;
	}
	if (erroneousPuzzle(currentPuzzle) == 0) {
		validateErroneousPuzzle();
		return -1;
	}

	/*2. Validating the board using ILP*/
	check = solveSudoku(currentPuzzle, 0, -1.0, -1, -1);
	if (check == -1)
		return -1;
	else if (check == 0)
		unsolvableBoard();
	else if (check == 1)
		solvableBoard();

	return 0;
}

/*Handles 'guess' command, checks propriety and guesses solution with LP using solveSudoku function. */
int handleGuess(char * parameter1, char * parameter2) {
	double threshold;
	int check, isFinish;

	/*1. Making sure we have accurate number of parameters in range. */
	if (parameter1 == NULL) {
		notEnoughParameters(1, "guess");
		return -1;
	}
	if (parameter2 != NULL) {
		toManyParameters(1, "guess");
		return -1;
	}
	if (strcmp(parameter1, "0") == 0 || strcmp(parameter1, "0.0") == 0)
		threshold = 0.0;
	else {
		threshold = atof(parameter1);
		if (threshold <= 0.0 || threshold > 1.0) {
			printf("The command guess should receive a double between 0-1.\n");
			return -1;
		}
	}

	/*2. Making sure that the user isn't trying to use 'guess' on an erroneous board. */
	if (erroneousPuzzle(currentPuzzle) == 0) {
		generateErroneousBoard();
		return -1;
	}

	/*3. Running LP on the board. */
	check = solveSudoku(currentPuzzle, 1, threshold, -1, -1);
	if (check != 1) {
		LPFails();
	}
	printSudoku(currentPuzzle);

	/*4. Checking if board is finished. */
	if (mode == 1) {
		isFinish = finishedPuzzle(currentPuzzle);
		if (isFinish == -1)
			erroneousFinishedBoard();
		else if (isFinish == 1) {
			successfullyFinishedBoard();
			mode = 0;
		}
	}

	return 0;
}

/*Handles 'generate' command, checks propriety and randomly generating a puzzle. */
int handleGenerate(char * parameter1, char * parameter2, char * parameter3) {
	int x, y, n = currentPuzzle->n, m = currentPuzzle->m;
	int emptyCellsCounter, k = 0, i, j, iter, check;
	variable * emptyCells;
	commands * generate;
	change * toChange;
	int oldVal, newVal;

	/*1. Making sure we have accurate number of parameters in range. */
	if (parameter1 == NULL || parameter2 == NULL) {
		notEnoughParameters(2, "generate");
		return -1;
	}
	if (parameter3 != NULL) {
		toManyParameters(2, "generate");
		return -1;
	}
	x = atoi(parameter1);
	if (x < 1 || x > n * m * n * m) {
		iLegalRange(1, "generate", 1, n * m * n * m);
		return -1;
	}
	y = atoi(parameter2);
	if (y < 1 || y > n * m * n * m) {
		iLegalRange(2, "generate", 1, n * m * n * m);
		return -1;
	}

	/*2. Making sure that the board is not erroneous, solvable and contains enought empty cells. */
	if (erroneousPuzzle(currentPuzzle) == 0) {
		generateErroneousBoard();
		return -1;
	}
	check = solveSudoku(currentPuzzle, 0, -1.0, -1, -1);
	if (check == -1) {
		validationFails();
		return -1;
	} else if (check == 0) {
		generateUnsolvableBoard();
		return -1;
	}

	emptyCellsCounter = countEmptyCells(currentPuzzle);
	if (emptyCellsCounter < x) {
		notEnoughCells(x, emptyCellsCounter);
		return -1;
	}

	/*3. Creating an array of all empty cells in board. */
	emptyCells = (variable *) malloc(emptyCellsCounter * sizeof(variable));
	if (emptyCells == NULL) {
		memoryError();
		handleExit();
		return EXIT;
	}
	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++)
			if (currentPuzzle->values[i][j] == 0) {
				emptyCells[k].row = i;
				emptyCells[k].col = j;
				emptyCells[k].val = 0;
				k++;
			}

	/*printf("Number of empty cells = %d:\t", emptyCellsCounter);
	 for (k = 0; k < emptyCellsCounter; k++)
	 printf("(%d,%d,%d)\t", emptyCells[k].row, emptyCells[k].col,
	 emptyCells[k].val);
	 printf("\n");*/


	/*4. Beginning 1000 iterations. */
	for (iter = 1; iter <= NUMBER_OF_ITERATIONS; iter++) {
		/*choose random values for empty cells. */
		check = chooseRandomValues(currentPuzzle, emptyCells, emptyCellsCounter, x);
		if (check == -1) {
			unfillEmptyCells(currentPuzzle, emptyCells, emptyCellsCounter);
			continue;
		} else if (check == EXIT) {
			free(emptyCells);
			handleExit();
			return EXIT;
		}

		/*check solutions*/
		fillEmptyCells(currentPuzzle, emptyCells, emptyCellsCounter);
		if (erroneousPuzzle(currentPuzzle) == 0) {
			unfillEmptyCells(currentPuzzle, emptyCells, emptyCellsCounter);
			continue;
		}
		check = solveSudoku(currentPuzzle, 0, -1.0, -1, -1);
		if (check == -1) {
			unfillEmptyCells(currentPuzzle, emptyCells, emptyCellsCounter);
			validationFails();
			free(emptyCells);
			return -1;
		} else if (check == 0) {
			unfillEmptyCells(currentPuzzle, emptyCells, emptyCellsCounter);
			continue;
		} else if (check == 1)
			break;
	}

	/*5. Loop ended. */
	if (iter > 1000) {
		generateFails();
		free(emptyCells);
		return -1;
	}

	unfillEmptyCells(currentPuzzle, emptyCells, emptyCellsCounter);
	free(emptyCells);

	/*6. Conducting 'generate' command and updating commandsList. */
	generate = (commands *) malloc(sizeof(commands));
	if (generate == NULL) {
		memoryError();
		handleExit();
		return EXIT;
	}
	generate->commandName = "generate";
	generate->list = NULL;
	generate->next = NULL;
	generate->prev = NULL;

	chooseRandomFixedCells(currentPuzzle, y);
	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++) {
			oldVal = currentPuzzle->values[i][j];
			if (currentPuzzle->fixed[i][j] == 1)
				newVal = solvedPuzzle->values[i][j];
			else
				newVal = 0;
			currentPuzzle->values[i][j] = newVal;
			if (oldVal != newVal) {
				toChange = (change *) malloc(sizeof(change));
				if (toChange == NULL) {
					memoryError();
					freeFromCommand(generate);
					handleExit();
					return EXIT;
				}
				toChange->row = i;
				toChange->col = j;
				toChange->from = oldVal;
				toChange->to = newVal;
				toChange->nextChange = NULL;
				addChange(&(generate->list), toChange);
			}
		}

	addCommand(&currentMove, generate);
	freeFromCommand(currentMove->next);
	currentMove->next = NULL;

	printf("Finished Generating after %d attempts. Now The Board Is:\n", iter);
	printSudoku(currentPuzzle);

	return 0;
}

/*Handles 'undo' command, checks propriety and undo change using undoChange function. */
int handleUndo(char * parameter1) {

	int isFinish;

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 != NULL) {
		toManyParameters(0, "undo");
		return -1;
	}

	/*2. Checking that there are moves to undo. */
	if (currentMove == NULL || strcmp(currentMove->commandName, "solve") == 0
			|| strcmp(currentMove->commandName, "edit") == 0) {
		noMovesToUndo();
		return -1;
	}

	/*3. Undoing change and updating current move. */
	undoChange(currentMove->list, 1);
	currentMove = currentMove->prev;
	printSudoku(currentPuzzle);

	/*4. Checking if board is finished. */
	if (mode == 1) {
		isFinish = finishedPuzzle(currentPuzzle);
		if (isFinish == -1)
			erroneousFinishedBoard();
		else if (isFinish == 1) {
			successfullyFinishedBoard();
			mode = 0;
		}
	}

	return 0;
}

/*Handles 'redo' command, checks propriety and redo change using redoChange function. */
int handleRedo(char * parameter1) {
	int isFinish;

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 != NULL) {
		toManyParameters(0, "redo");
		return -1;
	}

	/*2. Checking that there are moves to redo. */
	if (currentMove == NULL || currentMove->next == NULL) {
		noMovesToRedo();
		return -1;
	}

	/*3. Redoing change and updating current move. */
	currentMove = currentMove->next;
	doChange(currentMove->list, 1);
	printSudoku(currentPuzzle);

	/*4. Chaking if board is finished. */
	if (mode == 1) {
		isFinish = finishedPuzzle(currentPuzzle);
		if (isFinish == -1)
			erroneousFinishedBoard();
		else if (isFinish == 1) {
			successfullyFinishedBoard();
			mode = 0;
		}
	}

	return 0;
}

/*Handles 'save' command, checks propriety and saved the current board using saveFile function. */
int handleSave(char * parameter1, char * parameter2) {

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 == NULL) {
		notEnoughParameters(1, "save");
		return -1;
	}

	if (parameter2 != NULL) {
		toManyParameters(1, "save");
		return -1;
	}

	/*2. In EDIT mode, erroneous and unsolvable puzzles may not be saved. */
	if (mode == 2) {
		if (erroneousPuzzle(currentPuzzle) == 0) {
			saveErroneousBoard();
			return -1;
		} else if (solveSudoku(currentPuzzle, 0, -1.0, -1, -1) != 1) {
			saveUnsolvableBoard();
			return -1;
		}
	}

	return saveFile(parameter1, currentPuzzle);

}

/*Handles 'hint' command, checks propriety and gives hint to cell in <row=param2, col=param1> using ILP. */
int handleHint(char * parameter1, char * parameter2, char * parameter3) {
	int x, y, n = currentPuzzle->n, m = currentPuzzle->m, check;

	/*1. Making sure we have accurate number of parametersin range. */
	if (parameter1 == NULL || parameter2 == NULL) {
		notEnoughParameters(2, "hint");
		return -1;
	}
	if (parameter3 != NULL) {
		toManyParameters(2, "hint");
		return -1;
	}
	x = atoi(parameter1);
	if (x < 1 || x > n * m) {
		iLegalRange(1, "hint", 1, n * m);
		return -1;
	}
	y = atoi(parameter2);
	if (y < 1 || y > n * m) {
		iLegalRange(2, "hint", 1, n * m);
		return -1;
	}

	/*2. Making sure that the user isn't trying to use 'hint' on an erroneous board or on a fixed / valued cell. */
	if (erroneousPuzzle(currentPuzzle) == 0) {
		hintErroneousBoard();
		return -1;
	} else if (currentPuzzle->fixed[y - 1][x - 1] == 1) {
		hintFixedCell();
		return -1;
	} else if (currentPuzzle->values[y - 1][x - 1] != 0) {
		hintValuedCell();
		return -1;
	}

	/*3. Running ILP and giving a hint to the user. */
	check = solveSudoku(currentPuzzle, 0, -1.0, -1, -1);
	if (check == -1)
		return -1;
	if (check == 1)
		giveHint(x, y, solvedPuzzle->values[y - 1][x - 1]);
	else if (check == 0)
		hintUnsolvableBoard();

	return 0;
}

/*Handles 'guess_hint' command, checks propriety and uses LP to show user all legal values with score > 0. */
int handleGuessHint(char * parameter1, char * parameter2, char * parameter3) {
	int x, y, n = currentPuzzle->n, m = currentPuzzle->m, check;

	/*1. Making sure we have accurate number of parameters in range. */
	if (parameter1 == NULL || parameter2 == NULL) {
		notEnoughParameters(2, "guess_hint");
		return -1;
	}
	if (parameter3 != NULL) {
		toManyParameters(2, "guess_hint");
		return -1;
	}
	x = atoi(parameter1);
	if (x < 1 || x > n * m) {
		iLegalRange(1, "guess_hint", 1, n * m);
		return -1;
	}
	y = atoi(parameter2);
	if (y < 1 || y > n * m) {
		iLegalRange(2, "guess_hint", 1, n * m);
		return -1;
	}

	/*2. Making sure that the user isn't trying to use 'guess_hint' on an erroneous board, or on a fixed / valued cell. */
	if (erroneousPuzzle(currentPuzzle) == 0) {
		hintErroneousBoard();
		return -1;
	} else if (currentPuzzle->fixed[y - 1][x - 1] == 1) {
		hintFixedCell();
		return -1;
	} else if (currentPuzzle->values[y - 1][x - 1] != 0) {
		hintValuedCell();
		return -1;
	}

	/*3. Running LP and giving the scores to the user. */
	check = solveSudoku(currentPuzzle, 2, -1.0, x, y);
	if (check == -1)
		return -1;
	else if (check == 0)
		hintUnsolvableBoard();

	return 0;
}

/*Handles 'num_solutions command, checks propriety and uses backtracking to find the number of solutions to the current board. */
int handleNumSolutions(char * parameter1) {
	int numSol;

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 != NULL) {
		toManyParameters(0, "num_solutions");
		return -1;
	}

	/*2. Making sure that the user isn't trying to use 'num_solutions' on an erroneous board. */
	if (erroneousPuzzle(currentPuzzle) == 0) {
		numsolErroneousPuzzle();
		return -1;
	}

	/*3. Finding the number of solutions using backtracking algorithm. */
	numSol = backtracking(currentPuzzle);
	if (numSol == EXIT) {
		handleExit();
		return EXIT;
	} else if (numSol != -1)
		printf("The Number Of Different Solutions is: %d.\n", numSol);
	return 0;
}

/*Handles 'autofill' command, checks propriety, fills board with 'obvious' cells and updates commandsList.*/
int handleAutofill(char * parameter1) {
	int m = currentPuzzle->m, n = currentPuzzle->n;
	int i, j, value, isFinish;
	commands * autofill;
	change * toChange;

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 != NULL) {
		toManyParameters(0, "autofill");
		return -1;
	}

	/*2. Making sure that the user isn't trying to use 'autofill' on an erroneous puzzle. */
	if (erroneousPuzzle(currentPuzzle) == 0) {
		autofillErroneousPuzzle();
		return -1;
	}

	autofill = (commands *) malloc(sizeof(commands));
	if (autofill == NULL) {
		memoryError();
		handleExit();
		return EXIT;
	}
	autofill->commandName = "autofill";
	autofill->list = NULL;
	autofill->next = NULL;
	autofill->prev = NULL;

	/*3. Going through all cells to check for 'obvious values' to fill and adding each fill as a change. */
	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++) {
			value = singleLegalValueForCell(currentPuzzle, i, j);
			if (value != -1) {
				toChange = (change *) malloc(sizeof(change));
				if (toChange == NULL) {
					memoryError();
					freeFromCommand(autofill);
					handleExit();
					return EXIT;
				}
				toChange->row = i;
				toChange->col = j;
				toChange->from = 0;
				toChange->to = value;
				toChange->nextChange = NULL;
				addChange(&(autofill->list), toChange);
			}
		}

	/*4. Updating current move and printing the board. */
	addCommand(&currentMove, autofill);
	freeFromCommand(currentMove->next);
	currentMove->next = NULL;
	doChange(autofill->list, 1);

	printSudoku(currentPuzzle);

	/*5. Checking if board is finished. */
	if (mode == 1) {
		isFinish = finishedPuzzle(currentPuzzle);
		if (isFinish == -1)
			erroneousFinishedBoard();
		else if (isFinish == 1) {
			successfullyFinishedBoard();
			mode = 0;
		}
	}

	return 0;
}

/*Handles 'reset' command, checks propriety and undo all moves from commandsList. */
int handleReset(char * parameter1) {

	int isFinish;

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 != NULL) {
		toManyParameters(0, "reset");
		return -1;
	}

	/*2. Reseting the board and printing it. */
	while (currentMove != NULL && currentMove->prev != NULL) {
		undoChange(currentMove->list, 0);
		currentMove = currentMove->prev;
	}

	printSudoku(currentPuzzle);

	/*3. Checking if board is finished. */
	if (mode == 1) {
		isFinish = finishedPuzzle(currentPuzzle);
		if (isFinish == -1)
			erroneousFinishedBoard();
		else if (isFinish == 1) {
			successfullyFinishedBoard();
			mode = 0;
		}
	}

	return 0;
}

int handleExit() {
	printf("Exiting...\n");
	freeSudoku(currentPuzzle);
	free(currentPuzzle);
	freeSudoku(solvedPuzzle);
	free(solvedPuzzle);
	while (currentMove != NULL && currentMove->prev != NULL)
		currentMove = currentMove->prev;
	freeFromCommand(currentMove);
	return EXIT;
}

/*Handles 'exit' command, checks propriety and calls handleExit function. */
int handleMyExit(char * parameter1) {

	/*1. Making sure we have accurate number of parameters. */
	if (parameter1 != NULL) {
		toManyParameters(0, "exit");
		return -1;
	}

	/*2. Exiting game. */
	handleExit();

	return EXIT;
}

/*Returns 0-16 for each legal command, or -1 for illegal command. */
int getCommandNum(char * command) {
	if (command == NULL)
		return -2;
	else if (strcmp(command, "solve") == 0)
		return 0;
	else if (strcmp(command, "edit") == 0)
		return 1;
	else if (strcmp(command, "mark_errors") == 0)
		return 2;
	else if (strcmp(command, "print_board") == 0)
		return 3;
	else if (strcmp(command, "set") == 0)
		return 4;
	else if (strcmp(command, "validate") == 0)
		return 5;
	else if (strcmp(command, "guess") == 0)
		return 6;
	else if (strcmp(command, "generate") == 0)
		return 7;
	else if (strcmp(command, "undo") == 0)
		return 8;
	else if (strcmp(command, "redo") == 0)
		return 9;
	else if (strcmp(command, "save") == 0)
		return 10;
	else if (strcmp(command, "hint") == 0)
		return 11;
	else if (strcmp(command, "guess_hint") == 0)
		return 12;
	else if (strcmp(command, "num_solutions") == 0)
		return 13;
	else if (strcmp(command, "autofill") == 0)
		return 14;
	else if (strcmp(command, "reset") == 0)
		return 15;
	else if (strcmp(command, "exit") == 0)
		return 16;
	else
		return -1;
}

/*-1 for iLegalCommand, 0 for legalCommand, 100 for exit*/
int selectCommand() {
	char inputString[258] = "";
	int comNum;
	const char delim[8] = " \t\r\n";
	char *command, *parameter1, *parameter2, *parameter3, *parameter4, in;
	SP_BUFF_SET();

	printf("Please Enter a Command:\n");

	if (feof(stdin)) {
		return EXIT;
	}

	/*1. Making sure the command is not too long. */
	fgets(inputString, 258, stdin);
	if (inputString[256] != '\n' && inputString[256] != '\0') {
		toLongCommandMessage();
		do {
			in = fgetc(stdin);
		} while (in != EOF && in != '\n');
		return -1;
	}
	command = strtok(inputString, delim);
	parameter1 = strtok(NULL, delim);
	parameter2 = strtok(NULL, delim);
	parameter3 = strtok(NULL, delim);
	parameter4 = strtok(NULL, delim);

	/*printf(
	 "command = %s, parameter1=%s, parameter2=%s, parameter3=%s, parameter4=%s \n",
	 command, parameter1, parameter2, parameter3, parameter4);*/

	/*2. Making sure that a legal command was entered. */
	comNum = getCommandNum(command);
	if (comNum == -1) {
		ilegalCommandMessage();
		return -1;
	} else if (comNum == -2)
		return -1;
	if (modesToCommands[comNum][mode] == 0) {
		ilegalCommandInCurrentMode(comNum, command);
		return -1;
	}

	/*3. Handles the command. */
	switch (comNum) {
	case 0:
		return handleSolve(parameter1, parameter2);
	case 1:
		return handleEdit(parameter1, parameter2);
	case 2:
		return handleMarkErrors(parameter1, parameter2);
	case 3:
		return handlePrintBoard(parameter1);
	case 4:
		return handleSet(parameter1, parameter2, parameter3, parameter4);
	case 5:
		return handleValidate(parameter1);
	case 6:
		return handleGuess(parameter1, parameter2);
	case 7:
		return handleGenerate(parameter1, parameter2, parameter3);
	case 8:
		return handleUndo(parameter1);
	case 9:
		return handleRedo(parameter1);
	case 10:
		return handleSave(parameter1, parameter2);
	case 11:
		return handleHint(parameter1, parameter2, parameter3);
	case 12:
		return handleGuessHint(parameter1, parameter2, parameter3);
	case 13:
		return handleNumSolutions(parameter1);
	case 14:
		return handleAutofill(parameter1);
	case 15:
		return handleReset(parameter1);
	case 16:
		return handleMyExit(parameter1);
	default:
		return -1;
	}
	return 0;
}
