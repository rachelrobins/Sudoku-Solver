/********************************************************************
 * ******************************************************************
 * This module is for user interaction, mainly used for printing messages to the user.
 * *******************************************************************
 ********************************************************************/
#ifndef MAINAUX_H_
#define MAINAUX_H_
#define NUMBER_OF_LEGAL_COMMANDS 17
#define NUMBER_OF_MODES 3

/*This is a matrix which stores for each command, which modes are relevant. */
int modesToCommands[NUMBER_OF_LEGAL_COMMANDS][NUMBER_OF_MODES];

int ilegalCommandInCurrentMode(int comNum, char * command);
int validationFails();
int updateModesToCommands();
int ilegalCommandMessage();
int toLongCommandMessage();
int errorWhileLoading();
int memoryError();
int notEnoughParameters(int parNum, char * command);
int toManyParameters(int parNum, char * command);
int iLegalRange(int parNum, char * command, int lowRange, int highRange);
int updateFixedCellsError();
int erroneousFinishedBoard();
int successfullyFinishedBoard();
int autofillErroneousPuzzle();
int numsolErroneousPuzzle();
int cannotLoadFile();
int notaNumber();
int notEnoughCharacters();
int noMovesToUndo();
int noMovesToRedo();
int unsolvableBoard();
int solvableBoard();
int validateErroneousPuzzle();
int saveUnsolvableBoard();
int saveErroneousBoard();
int hintErroneousBoard();
int hintUnsolvableBoard();
int generateErroneousBoard();
int generateUnsolvableBoard();
int hintFixedCell();
int hintValuedCell();
int giveHint(int column, int row, int value);
int giveHintWithScore(int column, int row, int value, double score);
int toBigPuzzle();
int tooManyCharacters();
int fixedErroneousCell();
int notEnoughCells(int x, int emptyCells);
int generateFails();
int LPFails();
int zeroFixed();
int errorWhileSaving();
#endif /* MAINAUX_H_ */
