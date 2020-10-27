/********************************************************************
 * ******************************************************************
 * This module is responsible for loading a board from a given file / saving a board to a given file.
 * *******************************************************************
 ********************************************************************/

#ifndef FILES_H_
#define FILES_H_

/* Receives file, checks if the file's content describes a sudoku board and stores it as a sudoku board if so.
 * Returns the sudoku board, of NULL if something went wrong. */
Sudoku * loadFile(char * fileName);

/* Saves a file containing a description of sudoku board passed as 'toSave'. */
int saveFile (char * fileName, Sudoku * toSave);

/* Returns a default sudoku with dimensions n=m=3. */
Sudoku * defaultSudoku();

#endif /* FILES_H_ */
