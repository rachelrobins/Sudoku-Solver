/********************************************************************
 * ******************************************************************
 * This module handles undo / redo double linked list.
 * It uses a struct of commandsList, where each command contains a list of changes.
 * Additionally, the module contains auxiliary functions to handle this list.
 * *******************************************************************
 ********************************************************************/

#ifndef COMMANDSLIST_H_
#define COMMANDSLIST_H_

struct listOfChanges {
	int row;
	int col;
	int from;	/* cell value before change*/
	int to;		/* cell value after change*/
	struct listOfChanges * nextChange;
};

typedef struct listOfChanges change;

struct listOfCommands {
	char * commandName;
	change  * list;			/*list of changes made by the command.*/
	struct listOfCommands * prev;
	struct listOfCommands * next;
};

typedef struct listOfCommands commands;

/* Conducts all changes in the list stored in 'toChange' and prints those changes if toPrint = 1;*/
int doChange(change * toChange, int toPrint);

/* Cancels all changes in the list stored in 'toChange' and prints those changes if toPrint = 1;*/
int undoChange(change * toCancel, int toPrint);

/* Adds change stored in 'toAdd' to changeList. */
int addChange(change ** changeList, change * toAdd);

/* Frees all commands after 'command' (included) */
int freeFromCommand(commands * command);

/* Adds a command stored in 'toAdd' to commandsList. */
int addCommand(commands ** commandsList, commands * toAdd);

/* Frees all changes in 'changeList*/
int freeChangeList(change * changeList);

#endif /* COMMANDSLIST_H_ */
