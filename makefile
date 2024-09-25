ifeq ($(OS),Windows_NT)
RM = del /Q /F
FILE = wordle.exe
else
RM = rm -rf
FILE = wordle
endif

CC = gcc
CFLAGS = -c -Wall -Werror -pedantic-errors

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
	-$(RM) *.o
	
# DOES NOT WORK ON WINDOWS - Windows has no command-line zip utility
zip:
	zip -r wordle.zip wordle word_banks