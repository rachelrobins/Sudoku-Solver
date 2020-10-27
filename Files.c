#include <stdio.h>
#include "SPBufferset.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "string.h"
#include "Game.h"
#include "MainAux.h"
#include "Parser.h"
#define BLOCK 3

Sudoku * loadFile(char * fileName) {
	FILE * input;
	Sudoku * puzzle = (Sudoku *) malloc(sizeof(Sudoku));
	const char delim[8] = " \t\r\n";
	char line[128] = "";
	char * num;
	int len, value;
	int m, n, i, j;

	/*1. Opening the file and allocating memory for a puzzle. */
	if (puzzle == NULL) {
		memoryError();
		handleExit();
		toExit=1;
		return NULL;
	}

	input = fopen(fileName, "r");
	if (input == NULL) {
		cannotLoadFile();
		free(puzzle);
		return NULL;
	}

	/*2. Scanning dimensions from the file and allocating memory accordingly. */

	if (fscanf(input, "%d", &m) != 1 || m<1) {
		errorWhileLoading();
		fclose(input);
		free(puzzle);
		return NULL;
	}

	if (fscanf(input, "%d", &n) != 1 || n<1) {
		errorWhileLoading();
		fclose(input);
		free(puzzle);
		return NULL;
	}

	if (m*n>99) { /*puzzle is too big. */
		toBigPuzzle();
		free(input);
		free(puzzle);
		return NULL;
	}

	puzzle->m = m;
	puzzle->n = n;
	puzzle->values = (int**) malloc(n * m * sizeof(int*));
	puzzle->fixed = (int**) malloc(n * m * sizeof(int*));

	if (puzzle->values == NULL || puzzle->fixed == NULL) {
		memoryError();
		fclose(input);
		free(puzzle->values);
		free(puzzle->fixed);
		free(puzzle);
		handleExit();
		toExit=1;
		return NULL;
	}

	for (i = 0; i < n * m; i++) {
		puzzle->values[i] = (int*) malloc(n * m * sizeof(int));
		if (puzzle->values[i] == NULL) {
			memoryError();
			fclose(input);
			handleExit();
			toExit=1;
			return NULL;
		}
	}
	for (i = 0; i < n * m; i++) {
		puzzle->fixed[i] = (int*) malloc(n * m * sizeof(int));
		if (puzzle->fixed[i] == NULL) {
			memoryError();
			free(puzzle);
			handleExit();
			toExit=1;
			fclose(input);
			return NULL;
		}
	}


	/*3. Starting to read from file, line by line. */
	i = 0; /*line*/
	j = 0; /*word in line*/

	while (i < n*m && fgets(line, sizeof line , input) != NULL) {
		num = strtok(line, delim);

		while (i < n*m && num != NULL) {
			len = strlen(num);

			/*Is this a fixed value? */
			if (num[len - 1] == '.') {
				puzzle->fixed[i][j] = 1;
				num[len - 1] = '\0';
			} else
				puzzle->fixed[i][j] = 0;

			if (num == NULL || strcmp(num, "") == 0) {
				notaNumber();
				fclose(input);
				freeSudoku(puzzle);
				free(puzzle);
				return NULL;
			}

			/*What is the value? */
			if (strcmp(num, "0") == 0)
				puzzle->values[i][j] = 0;
			else {
				value = atoi(num);
				if (value <= 0) {
					notaNumber();
					fclose(input);
					freeSudoku(puzzle);
					free(puzzle);
					return NULL;
				} else if (value > n * m) {
					notaNumber();
					freeSudoku(puzzle);
					free(puzzle);
					fclose(input);
					return NULL;
				} else
					puzzle->values[i][j] = value;
			}

			/*What is the next cell? */
			if (j == n * m - 1) {
				j = 0;
				i = i + 1;
			} else
				j = j + 1;

			num = strtok(NULL, delim);
		}
	}

	/*4. Loop ended */
	if (i < n * m) { /*not enough characters. */
		notEnoughCharacters();
		freeSudoku(puzzle);
		free(puzzle);
		fclose(input);
		return NULL;
	} else if (num != NULL) { /*too many characters. */
		tooManyCharacters();
		freeSudoku(puzzle);
		free(puzzle);
		fclose(input);
		return NULL;
	}
	while (fgets(line, sizeof line, input) != NULL) { /*too many characters. */
		num = strtok(line, delim);
		if (num != NULL) {
			tooManyCharacters();
			freeSudoku(puzzle);
			free(puzzle);
			fclose(input);
			return NULL;
		}
	}
	fclose(input);

	return puzzle;
}

Sudoku * defaultSudoku() {
	Sudoku * puzzle = (Sudoku *) malloc(sizeof(Sudoku));
	int m = BLOCK, n = BLOCK, i, j;
	puzzle->m = m;
	puzzle->n = n;

	/*1. Allocating memory for the puzzle. */
	if (puzzle == NULL) {
		memoryError();
		handleExit();
		toExit=1;
		return NULL;
	}

	puzzle->values = (int**) malloc(n * m * sizeof(int*));
	puzzle->fixed = (int**) malloc(n * m * sizeof(int*));

	if (puzzle->values == NULL || puzzle->fixed == NULL) {
		memoryError();
		handleExit();
		toExit=1;
		return NULL;
	}

	for (i = 0; i < n * m; i++) {
		puzzle->values[i] = (int*) malloc(n * m * sizeof(int));
		if (puzzle->values[i] == NULL) {
			memoryError();
			toExit=1;
			return NULL;
		}
	}
	for (i = 0; i < n * m; i++) {
		puzzle->fixed[i] = (int*) malloc(n * m * sizeof(int));
		if (puzzle->fixed[i] == NULL) {
			memoryError();
			toExit=1;
			return NULL;
		}
	}

	/*2. Initializing cells in puzzle to 0. */
	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++) {
			puzzle->values[i][j] = 0;
			puzzle->fixed[i][j] = 0;
		}

	return puzzle;
}

int saveFile(char * fileName, Sudoku * toSave) {
	FILE * output = fopen(fileName, "w");
	int m = toSave->m, n = toSave->n, i, j;

	/*1. Opening the file and writing the puzzle's dimensions. */
	if (output == NULL) {
		cannotLoadFile();
		return -1;
	}

	if (fprintf(output, "%d ", m) < 0) {
		errorWhileSaving();
		fclose(output);
		return -1;
	}
	if (fprintf(output, "%d", n) < 0) {
		errorWhileSaving();
		fclose(output);
		return -1;
	}

	/*2. Writing the cells' values to the file. */
	for (i = 0; i < m * n; i++) {
		if (fprintf(output, "\n") < 0) {
			errorWhileSaving();
			fclose(output);
			return -1;
		}
		for (j = 0; j < m * n; j++) {
			fprintf(output, "%d", toSave->values[i][j]);
			if (toSave->fixed[i][j] == 1 || (mode==2 && toSave->values[i][j]!=0))
				if (fprintf(output, ".") < 0)  { /*Writing "." for fixed cells in SOLVE mode and for valued cells in EDIT mode. */
					errorWhileSaving();
					fclose(output);
					return -1;
				}
			if (j < m * n - 1)				/*Next line. */
				if (fprintf(output, " ") < 0) {
					errorWhileSaving();
					fclose(output);
					return -1;
				}
		}
	}

	printf("File %s saved successfully!\n", fileName);
	fclose(output);
	return 0;
}

