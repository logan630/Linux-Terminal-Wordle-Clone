CC = gcc
CFLAGS = -c -Wall -Werror -pedantic-errors
FILE = wordle

all: wordle
all: clean
wordle: game.o puzzle.o utils.o
	$(CC) game.o puzzle.o utils.o -o $(FILE)
game.o: game.c
	$(CC) $(CFLAGS) game.c	
puzzle.o: puzzle.c
	$(CC) $(CFLAGS) puzzle.c
wordle_utils.o: utils.c
	$(CC) $(CFLAGS) utils.c
clean:
	rm -rf *.o
zip:
	zip wordle.zip wordle word_banks