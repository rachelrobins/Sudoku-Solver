#include <stdio.h>
#include "SPBufferset.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "Game.h"
#include "Backtracking.h"
#include "MainAux.h"

/* Pushes 'item' to the beginning of stack which starts with 'head'. */
int push(elem ** head, elem * item) {
	if (item == NULL)
		return -1;

	item->next = *head;
	*head = item;
	return 0;
}

/* Pops out the beginning of stack and returns the numSol field of element we popped. */
int pop(elem ** head) {
	elem * item = *head;
	int num = item->numSol;
	*head = (*head)->next;
	free(item);
	return num;
}

/* Calculates and returns the row of cell after <row, col>. */
int nextRow(int m, int n, int row, int col) {
	if (col == m * n - 1)
		return row + 1;
	else
		return row;
}

/* Calculates and returns the col of cell after <row, col>. */
int nextCol(int m, int n, int row, int col) {
	if (col == m * n - 1)
		return 0;
	else
		return col + 1;


	return row;
}

/* Calculates and returns row of cell which is empty after <row, col>.
 * -1 is returned if there is no such cell. */
int nextZeroRow(int ** isZero, int m, int n, int i, int j) {
	int value = 1, row = i, col = j;
	while (value != 0 && (row != m * n - 1 || col != m * n - 1)) {
		row = nextRow(m, n, row, col);
		col = nextCol(m, n, row, col);
		value = isZero[row][col];
	}

	if (value == 0)
		return row;
	else
		return -1;
}

/* Calculates and returns col of cell which is empty after <row, col>.
 * -1 is returned if there is no such cell. */
int nextZeroCol(int ** isZero, int m, int n, int i, int j) {
	int value = 1, row = i, col = j;
	while (value != 0 && (row != m * n - 1 || col != m * n - 1)) {
		row = nextRow(m, n, row, col);
		col = nextCol(m, n, row, col);
		value = isZero[row][col];
	}

	/*printf("\trow = %d, col = %d, nextRow = %d, nextCol = %d.\n", i+1, j+1, row+1, col+1);*/

	if (value == 0)
		return col;
	else
		return -1;
}

/* Frees the stack. */
int freeList(elem * head) {
	elem * temp;
	while (head != NULL) {
		temp = head;
		head = head->next;
		free(temp);
	}
	return 0;
}


int backtracking(Sudoku * toSolve) {
	int m = toSolve->m, n = toSolve->n, i, j, row, col, curValue, rowCopy, colCopy;
	int numSolutions = 0;
	elem * top = (elem *) malloc(sizeof(top));
	elem * temp;
	int ** isZero = (int **) malloc(m * n * sizeof(int *)); /*Stores 0 if cell is empty, and -1 otherwise. */

	/*1. Allocating and filling 'isZero' Matrix. */
	if (top == NULL || isZero == NULL) {
		memoryError();
		free(top);
		free(isZero);
		return EXIT;
	}
	for (i = 0; i < n * m; i++) {
		isZero[i] = (int*) malloc(n * m * sizeof(int));
		if (isZero[i] == NULL) {
			memoryError();
			free(top);
			free(isZero);
			return EXIT;
		}
	}

	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++)
			if (toSolve->values[i][j] == 0)
				isZero[i][j] = 0;
			else
				isZero[i][j] = -1;

	/*2. If there are no empty cells, than the board is already solved. */
	row = nextZeroRow(isZero, m, n, 0, -1);
	col = nextZeroCol(isZero, m, n, 0, -1);
	if (row == -1 || col == -1) {
		free(top);
		free(isZero);
		return 1;
	}

	/*3. Entering the first empty cell to the stack. */
	top->row = row;
	top->col = col;
	top->val = 0;
	top->numSol = 0;
	top->next = NULL;

	/*4. Starting recursion. */
	while (top != NULL) {
		row = top->row;
		col = top->col;
		curValue = top->val;

		/* Calculates the smallest legal value for cell <row, col>.
		 * If there is no legal value, than we skip to the next iteration. */
		while (curValue <= n * m) {
			curValue++;
			toSolve->values[row][col] = curValue;
			if (legalPlaceInPuzzle(toSolve, row, col) == 0)
				break;
		}
		if (curValue == m * n + 1) {
			numSolutions = pop(&top);
			toSolve->values[row][col] = 0;
			if (top != NULL)
				top->numSol+=numSolutions;
			continue;
		}

		/* Finds the next empty cell before continue checking current cell.
		 * If there is no another empty cell, than we skip to the next iteration. */
		rowCopy = row; colCopy = col;
		row = nextZeroRow(isZero, m, n, rowCopy, colCopy);
		col = nextZeroCol(isZero, m, n, rowCopy, colCopy);

		if (row == -1 || col == -1) {
			top->numSol += 1;
			numSolutions = pop(&top);
			toSolve->values[rowCopy][colCopy] = 0;
			if (top != NULL)
				top->numSol+=numSolutions;
			continue;
		}

		/*Insert next empty cell to stack. */
		temp = (elem *) malloc(sizeof(elem));
		if (temp == NULL) {
			memoryError();
			freeList(top);
			free(isZero);
			return EXIT;
		}
		temp->row = row;
		temp->col = col;
		temp->val = 0;
		temp->numSol = 0;
		temp->next = NULL;
		top->val = curValue;

		push(&top, temp);
	}

	/*3. Cleaning the original puzzle from the values we wrote. */
	for (i = 0; i < n * m; i++)
		for (j = 0; j < n * m; j++)
			if (isZero[i][j] == 0)
				toSolve->values[i][j]=0;

	freeList(top);
	free(isZero);
	return numSolutions;
}
