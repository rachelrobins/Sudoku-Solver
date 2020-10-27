#include <stdio.h>
#include "SPBufferset.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "Game.h"
#include "MainAux.h"


/*Prints separate row in the format requested. */
int separateRow(int m, int n) {
	int i;
	for (i = 0; i < 4 * n * m + m + 1; i++)
		printf("-");
	printf("\n");
	fflush(stdout);
	return 0;
}

/*Prints a row containing cells, and '.' to each fixed cell in SOLVE mode. */
int cellsRow(Sudoku * toPrint, int row, int * rowFixed, int m, int n) {
	int i, j;
	for (i = 0; i < m; i++) {
		printf("|");
		for (j = 0; j < n; j++) {
			printf(" ");

			if (toPrint->values[row][n * i + j] == 0)
				printf("  ");
			else
				printf("%2d", toPrint->values[row][n * i + j]);

			/*In SOLVE mode, we print '.' to each fixed cell.
			 *IN EDIT mode, or if markErrors=1, we print '*' to each erroneous cell*/
			if (mode == 1 && rowFixed[n * i + j] == 1)
				printf(".");
			else if ((mode == 2 || markErrors == 1)
					&& legalPlaceInPuzzle(toPrint, row, n*i+j)==-1)
				printf("*");
			else
				printf(" ");
			fflush(stdout);
		}
	}
	printf("|\n");
	return 0;
}

int printSudoku(Sudoku *toPrint) {
	int m = toPrint->m, n = toPrint->n, i, j;
	separateRow(m, n);
	for (i = 0; i < n; i++) {
		for (j = 0; j < m; j++)
			cellsRow(toPrint, m*i+j, toPrint->fixed[m * i + j], m, n);
		separateRow(m, n);
	}

	return 0;
}
