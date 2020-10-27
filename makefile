CC = gcc
OBJS = main.o MainAux.o Parser.o Game.o Files.o CommandsList.o LP.o ILP.o Backtracking.o Print.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

.PHONY : all
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@
main.o: main.c MainAux.h Parser.h Game.h Files.h Print.h ILP.h SPBufferset.h
	$(CC) $(COMP_FLAG) -c $*.c
MainAux.o: MainAux.c MainAux.h
	$(CC) $(COMP_FLAG) -c $*.c
Parser.o: Parser.c Parser.h
	$(CC) $(COMP_FLAG) -c $*.c
Game.o: Game.c Game.h
	$(CC) $(COMP_FLAG) -c $*.c
Files.o: Files.c Files.h
	$(CC) $(COMP_FLAG) -c $*.c
CommandsList.o: CommandsList.c CommandsList.h
	$(CC) $(COMP_FLAG) -c $*.c
LP.o: LP.c LP.h
	$(CC) $(COMP_FLAG) -c $*.c
ILP.o: ILP.c ILP.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
Backtracking.o: Backtracking.c Backtracking.h
	$(CC) $(COMP_FLAG) -c $*.c
Print.o: Print.c Print.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
