#include <stdio.h>
#include "SPBufferset.h"
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "Game.h"
#include "CommandsList.h"


int doChange(change * toChange, int toPrint) {
	change * temp = toChange;
	int row, col, from, to;
	/*Go over a list of changes on conduct each change. */
	while (temp != NULL) {
		row = temp->row;
		col = temp->col;
		from = temp->from;
		to = temp->to;
		if (toPrint == 1)
			printf("Change row = %d, col = %d from %d to %d.\n", row + 1, col + 1,from, to);
		currentPuzzle->values[row][col] = to;
		temp = temp->nextChange;
	}
	return 0;
}

int undoChange(change * toCancel, int toPrint) {
	change * temp = toCancel;
	int row, col, from, to;
	/*Go over a list of changes and cancel each change. */
	while (temp != NULL) {
		row = temp->row;
		col = temp->col;
		from = temp->to;
		to = temp->from;
		if (toPrint == 1)
			printf("Change row = %d, col = %d from %d to %d.\n", row + 1, col + 1, from, to);
		currentPuzzle->values[row][col] = to;
		temp = temp->nextChange;
	}
	return 0;
}

int addChange(change ** changeList, change * toAdd) {
	/*printf("Change of row = %d, col = %d was inserted!\n", toAdd->row, toAdd->col);*/
	toAdd->nextChange = (*changeList);
	(*changeList) = toAdd;
	return 0;
}

int addCommand(commands ** commandsList, commands * toAdd) {
	if (*commandsList != NULL) {
		toAdd->next = (*commandsList)->next;
		if (toAdd->next != NULL)
			(toAdd->next->prev = toAdd);
		(*commandsList)->next = toAdd;
		toAdd->prev = (*commandsList);
	}
	currentMove = toAdd;

	return 0;
}

int freeChangeList(change * changeList) {
	change * temp;
	while (changeList != NULL) {
		temp = changeList;
		changeList = changeList->nextChange;
		free(temp);
	}
	return 0;
}

int freeFromCommand(commands * command) {
	commands * temp;
	while (command != NULL) {
		freeChangeList(command->list);
		temp = command;
		command = command->next;
		free(temp);
	}
	return 0;
}
