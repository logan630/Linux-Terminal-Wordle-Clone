#include "wordle.h"

// Grabs a random word from words.txt
// Learn the length of each word from the word bank
void initialize(game_state* state, int flag, char* word_bank) {
    int fs_valid = -1; // The first fs_valid words in the file are valid answers; determined by the first line
    FILE* fp;

    fp = fopen(word_bank, "r");

    // Seeds the random number generator
    if(flag == DAILY) {
        srand(time(0) / 86400); // Daily Wordle - uses the same seed for an entire day
    } else {
        srand(time(0)); // Random wordle - seed changes constantly
    }

    char buffer[8];

    // Learn the length of each word by reading the file's first line
    fscanf(fp, "%s", buffer);

    // Allocate memory and initialize attributes
    state->word_length = atoi(buffer);
    state->target = (char*) malloc(state->word_length);
    state->hard_mode = false;

    // Hard mode variant
    if(flag == HARD) {
        state->known_characters = EMPTY;
        state->hard_mode = true;

        // Every green index is "out of bounds" before it is identified
        for(int i = 0; i < 26; i++) {
            state->greens[i] = ABSENT;
        }
    }

    // Learns the number of valid answers by reading the file's second line
    fscanf(fp, "%s", state->target);
    fs_valid = atoi(state->target);

    // Randomness
    int line = rand() % fs_valid; // Selects a line from the valid words in words.txt
    int counter = 0;

    while(fscanf(fp, "%s", state->target) && (counter++) < line);

    fclose(fp);
}

// Given some string, determines if it is a valid wordle guess
validation_message validate_input(char* word, game_state state, char* word_bank) {
    validation_message valid;
    valid.valid = false;
    valid.has_message = false;

    // Only allows five character strings
    if(strlen(word) != state.word_length + 1) {
        return valid;
    }

    FILE* fp;
    fp = fopen(word_bank, "r");

    // Creates a buffer to store a word from words.txt, as well as a lowercase version of word
    char* line = (char*) malloc(state.word_length + 1);
    char* lower_word = (char*) malloc(state.word_length + 1);

    // Creates a copy of the word all in lower case to check against the file
    for(int i = 0; i < state.word_length; i++) {
        lower_word[i] = tolower((unsigned char) word[i]);
    }

    // Checks to see if the word is in words.txt
    while(fgets(line, state.word_length + 1, fp)) {

        // Returns true if it is
        if(strcmp(line, lower_word) == 0) {
            valid.valid = true;
        }
    }

    // Hard mode logic
    // Check to see if every known letter (yellow or green) is used
    if(state.hard_mode && state.known_characters != EMPTY) {
        int map = encode_word(word, state.word_length);
        if((map & state.known_characters) != state.known_characters) {
            valid.valid = false;
            valid.has_message = true;
            sprintf(valid.message, "%sHARD MODE%s: You must use all known letters. Please try again.\n", BOLD, DEFAULT);
        }

        // Iterate over the alphabet
        for(int i = 0; i < 26; i++) {
            // If a letter is green but the corresponding position in the word is not green, return false
            if(state.greens[i] > -1 && VALUE(word[state.greens[i]]) != i) {
                valid.valid = false;
                valid.has_message = true;
                sprintf(valid.message, "%sHARD MODE%s: You must use all green letters in their proper places. Please try again.\n", BOLD, DEFAULT);
                break;
            }
        }
    }

    fclose(fp);
    free(line);
    free(lower_word);
    return valid;
}

// Determines how accurate a guess is relative to the target
// Sets appropriate color values in colors
void analyze_guess(char* guess, game_state* state, color* colors, color* keyboard) {
    // Evaluates the green letters first (if any exist)
    for(int i = 0; i < state->word_length; i++) {
        // If the characters are the same, set the letter to green
        if(tolower((unsigned char) guess[i]) == tolower((unsigned char) state->target[i])) {
            colors[i] = GREEN;
            int index = 2 * VALUE(guess[i]);
            keyboard[index] = GREEN;

            // Update known characters
            if(state->hard_mode) {
                state->known_characters |= encode(guess[i]);
                state->greens[VALUE(guess[i])] = i;
            }
        } 
    }

    // Array to keep track of results
    bool* checked = (bool*) malloc(state->word_length * sizeof(bool));

    for(int i = 0; i < state->word_length; i++) {
        // Keeps track of visited letters
        // Also solves the double letter problem (at the expense of a bit more memory - oh no!)
        bool yellow = false;

        // Checks all letters of the target to see if the given letter is present at all
        for(int j = 0; j < state->word_length; j++) {
            // If the letter is present, set the letter to yellow (don't override greens)
            if(VALUE(guess[i]) == VALUE(state->target[j]) 
                && colors[i] != GREEN && colors[j] != GREEN && !checked[j]) {
                colors[i] = YELLOW;

                // Update known characters
                if(state->hard_mode) {
                    state->known_characters |= encode(guess[i]);
                }

                // Update keyboard color array
                int index = 2 * VALUE(guess[i]);
                if(keyboard[index] == WHITE) {
                    keyboard[index] = YELLOW;
                }

                yellow = true;
                checked[j] = true;
                break;
            }
        } 
        // The default is white
        if(!yellow && colors[i] != GREEN) {
            colors[i] = WHITE;
            int index = 2 * VALUE(guess[i]);
            if(keyboard[index] == WHITE) {
                keyboard[index] = BLACK;
            }
        }
    }

    free(checked);
}

// Self-explanatory
bool is_solved(color* colors, game_state state) {
    for(int i = 0; i < state.word_length; i++) {
        if(colors[i] != GREEN) {
            return false;
        }
    }
    return true;
}

// Free allocated memory on the heap
void terminate(game_state state) {
    free(state.target);
}