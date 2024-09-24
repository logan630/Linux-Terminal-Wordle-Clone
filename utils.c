#include "wordle.h"

// Makes single characters colorful
void print_color(char c, color col) {
    printf("\033[;%dm%c\033[0m", col, c);
}

// Makes entire STRINGS colorful
void print_color_str(char* word, color* colors, int str_length) {
    for(int i = 0; i < str_length; i++) {
        print_color(toupper((unsigned char) word[i]), colors[i]);
    }
    printf("\n");
}

// Returns a color array to all white
void clean(color* colors, int length) {
    for(int i = 0; i < length; i++) {
        colors[i] = WHITE;
    }
}

// Capitalizes every character in a string
void to_caps(char* string) {
    for(int i = 0; i < strlen(string); i++) {
        string[i] = (char) toupper(string[i]);
    }
}

// Converts a character to a bitmask
int encode(char c) {
    int offset = VALUE(c);
    return 1 << offset;
}

// Converts an entire word to a bitmask
// State provided for word_length attribute
int encode_word(char* word, int length) {
    int map = 0;
    for(int i = 0; i < length; i++) {
        map |= encode(word[i]);
    }

    return map;
}