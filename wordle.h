#include <stdio.h> // printf, fprintf, fscanf, fgets, sprintf, fopen, fclose
#include <stdlib.h> // malloc, rand, srand, free
#include <time.h> // time
#include <string.h> // strlen, atoi, strcmp, strcpy
#include <stdbool.h> // bool
#include <ctype.h> // tolower, toupper

// In Windows 10 onwards, ANSI escape sequences work, but must be manually enabled
// Below are the necessary definitions and required header files
// CLEAR just accounts for the difference in syscall names between UNIX-based systems and Windows systems
#ifdef _WIN32
    #include <wchar.h>
    #include <windows.h>
    #define CLEAR "cls"
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #define CLEAR "clear"
#endif

#define FILE_NAME "./word_banks/words.txt" // Default word bank

// Special values of the color type
#define YELLOW 33 
#define GREEN 32 
#define WHITE 37 
#define BLACK 30 

// Special characters used in text formatting
#define BOLD "\033[1m" 
#define ITALICS "\033[3m" 
#define DEFAULT "\033[0m" 
#define RED_TEXT "\033[;31m" 

// Game mode settings
#define NORMAL 0
#define DAILY 1 
#define HARD 2 

// Special values for hard mode data
// Included for readability
#define EMPTY 0 // For comparisons with the known_characters bitmask
#define ABSENT -1 // Index for any character not green in a previous guess

// Common, easily inlined functions
#define CLEAR_LINE(n) for(int ii=0;ii<n;ii++){printf("\033[A\r\33[2K\r");} // Erases the previous n lines in the console
#define VALUE(x) ((int) (tolower((unsigned char) x) - tolower((unsigned char) 'a'))) // Convert char to a number 0 to 25

// Defines length of each word and target word for the game
// Word length is 5 by default but can change in custom wordle
struct game_state {
    int word_length;
    char* target;
    bool hard_mode;

    // A bitmask to keep track of known letters
    // This is done in an effort to limit the amount of heap memory allocations performed
    int known_characters;

    // To keep track of the position of each known letter in the word
    // Cannot be done as a bitmask; indices are never one bit, and exact size can vary (custom wordle)
    // NOTE: custom wordle hard mode not yet implemented. This requires an overhaul of how command line flags are handled.
    int greens[26];
};

// Provides a more descriptive output from the validate_input function than just "true" or "false"
struct validation_message {
    bool valid;
    bool has_message; // If false, print the default error message; otherwise, print the contents of message
    char message[128];
};

typedef struct game_state game_state;
typedef struct validation_message validation_message;
typedef unsigned int color;


/* puzzle.c */

// Chooses a random word from the set of valid words in words.txt
void initialize(game_state*, int, char*);

// Checks if word is 5 characters long and consists only of letters
// Then, if it is, scrolls through words.txt and returns false if word is never present
validation_message validate_input(char*, game_state, char*);

// Compares a guess to some target and returns the appropriate color array
// Assumes the guess has already been validated
void analyze_guess(char*, game_state*, color*, color*);

// Given a color array, determine if the puzzle has been solved
bool is_solved(color*, game_state);

// Frees any memory allocated on the heap within a given state, thus ending the game
void terminate(game_state);


/* utils.c */

// Prints a provided character as a given color
void print_color(char, color);
void print_color_str(char*, color*, int);

// Sets every color to white
void clean(color*, int);

// Capitalizes every character in a string
void to_caps(char*);

// Creates a bitmask for a given character
int encode(char);

// Summarizes an entire word in a single bitmask
int encode_word(char*, int);