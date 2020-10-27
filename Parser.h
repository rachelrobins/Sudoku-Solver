/********************************************************************
 * ******************************************************************
 * This module is responsible for interpreting commands, checking their propriety and handling them.
 * *******************************************************************
 ********************************************************************/

#ifndef PARSER_H_
#define PARSER_H_

/* Takes input and beaks it into a command and into lal its parameters.
 * Return the output of illegal command, -1 for illegal command or EXIT for exit. */
int selectCommand();

/* Handles 'exit' command, frees all memory allocated and return EXIT constant. */
int handleExit();


#endif /* PARSER_H_ */
