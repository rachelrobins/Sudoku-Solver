#include <stdio.h>
#include "SPBufferset.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "Game.h"
#include "MainAux.h"



int updateModesToCommands() {
	/*INIT mode = 0, SOLVE mode = 1, EDIT mode = 2*/
	modesToCommands[0][0] = 1;
	modesToCommands[0][1] = 1;
	modesToCommands[0][2] = 1;
	modesToCommands[1][0] = 1;
	modesToCommands[1][1] = 1;
	modesToCommands[1][2] = 1;
	modesToCommands[2][0] = 0;
	modesToCommands[2][1] = 1;
	modesToCommands[2][2] = 0;
	modesToCommands[3][0] = 0;
	modesToCommands[3][1] = 1;
	modesToCommands[3][2] = 1;
	modesToCommands[4][0] = 0;
	modesToCommands[4][1] = 1;
	modesToCommands[4][2] = 1;
	modesToCommands[5][0] = 0;
	modesToCommands[5][1] = 1;
	modesToCommands[5][2] = 1;
	modesToCommands[6][0] = 0;
	modesToCommands[6][1] = 1;
	modesToCommands[6][2] = 0;
	modesToCommands[7][0] = 0;
	modesToCommands[7][1] = 0;
	modesToCommands[7][2] = 1;
	modesToCommands[8][0] = 0;
	modesToCommands[8][1] = 1;
	modesToCommands[8][2] = 1;
	modesToCommands[9][0] = 0;
	modesToCommands[9][1] = 1;
	modesToCommands[9][2] = 1;
	modesToCommands[10][0] = 0;
	modesToCommands[10][1] = 1;
	modesToCommands[10][2] = 1;
	modesToCommands[11][0] = 0;
	modesToCommands[11][1] = 1;
	modesToCommands[11][2] = 0;
	modesToCommands[12][0] = 0;
	modesToCommands[12][1] = 1;
	modesToCommands[12][2] = 0;
	modesToCommands[13][0] = 0;
	modesToCommands[13][1] = 1;
	modesToCommands[13][2] = 1;
	modesToCommands[14][0] = 0;
	modesToCommands[14][1] = 1;
	modesToCommands[14][2] = 0;
	modesToCommands[15][0] = 0;
	modesToCommands[15][1] = 1;
	modesToCommands[15][2] = 1;
	modesToCommands[16][0] = 1;
	modesToCommands[16][1] = 1;
	modesToCommands[16][2] = 1;

	return 0;
}

int zeroFixed() {
	printf("Fixed cell cannot be empty!\n");
	fflush(stdout);
	return 0;
}

int validationFails() {
	printf("The command generate failed validating the board.\n");
	fflush(stdout);
	return 0;
}

int generateFails() {
	printf("The command generate has failed after 1000 iterations.\n");
	fflush(stdout);
	return 0;
}

int notEnoughCells(int x, int emptyCells) {
	printf("You entered x = %d, but there are only %d empty cells.\n", x, emptyCells);
	fflush(stdout);
	return 0;
}

int toBigPuzzle() {
	printf("The dimensions of the puzzle are too big!\n");
	fflush(stdout);
	return 0;
}

int giveHint(int column, int row, int value) {
	printf("In column=%d row=%d I recommend the value: %d.\n", column, row, value);
	fflush(stdout);
	return 0;
}

int giveHintWithScore(int column, int row, int value, double score) {
	printf("In column=%d row=%d I recommend the value: %d with score: %f.\n", column, row, value, score);
	fflush(stdout);
	return 0;
}

int LPFails() {
	printf("Linear programming failed.\n");
	fflush(stdout);
	return 0;
}

int hintUnsolvableBoard() {
	printf("You can't ask for a hint to unsolvable board.\n");
	fflush(stdout);
	return 0;
}

int hintErroneousBoard() {
	printf("You can't ask for a hint to an erroneous board.\n");
	fflush(stdout);
	return 0;
}

int generateUnsolvableBoard() {
	printf("You can't use generate on unsolvable board.\n");
	fflush(stdout);
	return 0;
}

int generateErroneousBoard() {
	printf("You can't use generate on erroneous board.\n");
	fflush(stdout);
	return 0;
}

int hintFixedCell() {
	printf("You can't use hint on a fixed cell.\n");
	fflush(stdout);
	return 0;
}

int hintValuedCell() {
	printf("You can't use hint on a cell with a value.\n");
	fflush(stdout);
	return 0;
}

int saveUnsolvableBoard() {
	printf("In EDIT mode, you can't save an unsolvable board.\n");
	fflush(stdout);
	return 0;
}

int saveErroneousBoard() {
	printf("In EDIT mode, you can't save an erroneous board.\n");
	fflush(stdout);
	return 0;
}

int unsolvableBoard() {
	printf("Your Board Is Unsolvable!\n");
	return 0;
}

int solvableBoard() {
	printf("Your Board Is Solvable!\n");
	return 0;
}

int toManyParameters(int parNum, char * command) {
	printf("To many parameters were entered.\n");
	printf("The command %s should receive exactly %d parameters.\n", command, parNum);
	return 0;
}

int validateErroneousPuzzle() {
	printf("You can't validate an erroneous board!\n");
	fflush(stdout);
	return 0;
}

int autofillErroneousPuzzle() {
	printf("You can't autofill an erroneous board!\n");
	fflush(stdout);
	return 0;
}

int numsolErroneousPuzzle() {
	printf("You can't Use num_solutions on an erroneous board!\n");
	fflush(stdout);
	return 0;
}

int notEnoughParameters(int parNum, char * command) {
	printf("Not enough parameters were entered.\n");
	printf("The command %s should receive exactly %d parameters.\n", command, parNum);
	return 0;
}

int iLegalRange(int parNum, char * command, int lowRange, int highRange) {
	printf(
			"Parameter Number %d of the command %s should be an integer between %d and %d.\n",
			parNum, command, lowRange, highRange);
	return 0;
}

int memoryError() {
	printf("Memory Error\n");
	fflush(stdout);
	return 0;
}

int cannotLoadFile() {
	printf("This file cannot be loaded!\n");
	fflush(stdout);
	return 0;
}

int notaNumber() {
	printf("The file includes not-number characters or ilegal numbers.\n");
	fflush(stdout);
	return 0;
}

int notEnoughCharacters() {
	printf("The file is too small to make a full puzzle.\n");
	fflush(stdout);
	return 0;
}

int tooManyCharacters() {
	printf("The file is too big.\n");
	fflush(stdout);
	return 0;
}

int fixedErroneousCell() {
	printf("In your file, there are erroneous fixed cells.\n This file cannot be loaded.\n");
	fflush(stdout);
	return 0;
}

int noMovesToUndo() {
	printf("There are no moves to undo yet.\n");
	fflush(stdout);
	return 0;
}

int noMovesToRedo() {
	printf("There are no moves to redo yet.\n");
	fflush(stdout);
	return 0;
}

int updateFixedCellsError() {
	printf("In solve mode, fixed cells may not be updated.\n");
	fflush(stdout);
	return 0;
}

int erroneousFinishedBoard() {
	printf("You have filled all cells, by your board is erroneous...\n");
	fflush(stdout);
	return 0;
}

int successfullyFinishedBoard() {
	printf("Congratulations! You have solved the board successfully!\n");
	fflush(stdout);
	return 0;
}

int errorWhileLoading() {
	printf("Error was accrued while loading from file.\n");
	return 0;
}

int errorWhileSaving() {
	printf("Error was accrued while saving to file.\n");
	return 0;
}

int toLongCommandMessage() {
	printf("To many characters in a single line!\n");
	return 0;
}

int ilegalCommandMessage() {
	printf("This is not a Command!\n");
	return 0;
}

int ilegalCommandInCurrentMode(int comNum, char * command) {
	printf("The Command %s is not available in current mode.\n", command);
	printf("You can use it only on: ");
	if (modesToCommands[comNum][0] == 1)
		printf("INIT ");
	if (modesToCommands[comNum][1] == 1)
		printf("SOLVE ");
	if (modesToCommands[comNum][2] == 1)
		printf("EDIT ");
	printf("\n");
	return 0;
}
