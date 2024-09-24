#include "wordle.h"

int main(int argc, char **argv) {

    // Enable Virtual Terminal Processing for Windows systems
    #ifdef _WIN32
        // Code adapted directly from Microsoft documentation
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if(hOut == INVALID_HANDLE_VALUE) {
            return GetLastError();
        }

        DWORD dwMode = 0;
        if(!GetConsoleMode(hOut, &dwMode)) {
            return GetLastError();
        }

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if(!SetConsoleMode(hOut, dwMode)) {
            return GetLastError();
        }
    #endif

    game_state state;
    bool custom = false; // for input validation

    // Checks for command line arguments; such arguments will be used to enable the daily option.
    switch(argc) {
        case 1: // "Free-play" / random wordle
            initialize(&state, NORMAL, FILE_NAME);
            break;
        case 2: // Checks any flags
            if(strlen(argv[1]) != 2 || argv[1][0] != '-') { // Bad input
                fprintf(stderr, "Error: \"%s\" is not a valid flag.\n", argv[1]);
                exit(EXIT_FAILURE);
            }
            
            switch(argv[1][1]) {
                case 'd': // Daily wordle
                    initialize(&state, DAILY, FILE_NAME);
                    break;
                case 'h': // Help
                    printf("\nUsage: wordle <-d|-h|-r|-c|-H>\n");
                    printf("-d : Changes the mode to Daily Wordle.\n");
                    printf("-h : Displays this menu.\n");
                    printf("-r : Displays the rules.\n");
                    printf("-c <FILE> : Changes the mode to Custom Wordle, with word bank determined in <FILE>.\n");
                    printf("-H : Starts a new game of classic Wordle in %sHARD MODE%s.\n\n", BOLD, DEFAULT);
                    exit(EXIT_SUCCESS);
                case 'r': // Game rules
                    system("clear");
                    printf("\nWelcome to %sWORDLE%s!\n", BOLD, DEFAULT);
                    printf("You are given a random five-letter word.\n");
                    printf("Your goal is to guess this word within six attempts.\n\n");
                    printf("Your incorrect guesses serve as your clues.\n");
                    printf("After each guess, each letter with be colored \033[;%dmgreen\033[0m, \033[;%dmyellow\033[0m, or white.\n\n", GREEN, YELLOW);
                    printf("Green letters represent letters that are both present in the correct answer and are in the right position in your guess.\n");
                    printf("Yellow letters are present in the correct answer but not in the correct position.\n");
                    printf("White letters are not present at all.\n\n");
                    printf("In %sHARD MODE%s, all yellow and green letters must be used in subsequent guesses.\n", BOLD, DEFAULT);
                    printf("Green letters must be used in their proper places.\n\n");
                    exit(EXIT_SUCCESS);
                case 'c': // Custom mode (insufficient arguments)
                    fprintf(stderr, "Error: insufficient arguments. Provide the name of a .txt file.\n");
                    exit(EXIT_FAILURE);
                case 'H': // Hard mode
                    initialize(&state, HARD, FILE_NAME);
                    break;
                default: // Bad input
                    fprintf(stderr, "Error: \"%s\" is not a valid flag.\n", argv[1]);
                    exit(EXIT_FAILURE);
            }
            break;
        case 3:
            if(strlen(argv[1]) != 2 || argv[1][0] != '-' || argv[1][1] != 'c') { // Bad input
                fprintf(stderr, "Error: \"%s\" is not a valid flag.\n", argv[1]);
                exit(EXIT_FAILURE);
            }

            FILE* fp = fopen(argv[2], "r");
            if(fp == NULL) {
                fprintf(stderr, "Error: \"%s\" does not exist.\n", argv[2]);
                exit(EXIT_FAILURE);
            } else {
                custom = true;
            }
            fclose(fp);

            initialize(&state, NORMAL, argv[2]);

            break;
        default:
            break;
    }

    system(CLEAR); // Clears the screen for the game
    
    if(argc == 1) { // Only possible if no arguments are provided
        printf("Welcome to %sWORDLE!%s\n\n", BOLD, DEFAULT);
    } else if (argc == 3) { // Only possible when trying to play daily wordle
        printf("Welcome to %sCUSTOM WORDLE!%s\n", BOLD, DEFAULT);
        // Prints the name of the word bank in italics
        printf("Word Bank: %s%s%s\n\n", ITALICS, argv[2], DEFAULT);
    } else {
        if(state.hard_mode) {
            printf("Welcome to %sWORDLE: HARD MODE!%s\n\n", BOLD, DEFAULT);
        } else {
            printf("Welcome to %sDAILY WORDLE!%s\n\n", BOLD, DEFAULT);
        }
    }
    printf("Your goal is to guess a random %s%d%s letter word.\n", BOLD, state.word_length, DEFAULT);
    printf("You have %s%d%s guesses. Good luck!\n\n", BOLD, state.word_length + 1, DEFAULT);

    if(state.hard_mode) {
        printf("You must use %sall%s information available to you.\n", BOLD, DEFAULT);
        printf("This means:\n");
        printf(" - You must use all yellow and green letters you find.\n");
        printf(" - All green letters must be used in their proper places.\n\n");
    }

    printf("Run the command \"wordle -h\" for help.\n\n");
    // printf("%s\n\n", state.target);

    // Tries to allow the user to provide invalid guesses without crashing wordle
    char* guess = (char*) malloc(100 * (state.word_length + 1) + 1);
    color* colors = (color*) malloc(state.word_length * sizeof(color));
    color keyboard[51];

    for(int i = 0; i < 51; i++) {
        keyboard[i] = WHITE;
    }

    for(int i = 0; i < state.word_length + 1; i++) {
        printf("\nAvailable letters: ");
        print_color_str("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z", keyboard, 51);
        printf("Guess %d/%d: ", i+1, state.word_length + 1);
        fgets(guess, 100 * (state.word_length + 1), stdin);

        // Processes invalid input
        validation_message success;
        success.valid = false;
        success.has_message = false;
        int errors = 0;

        while(!success.valid) {
            if(custom) {
                success = validate_input(guess, state, argv[2]);
            } else {
                success = validate_input(guess, state, FILE_NAME);
            }

            if(!success.valid) {
                guess[strlen(guess) - 1] = 0;
                CLEAR_LINE(2);
                to_caps(guess);
                if(!success.has_message) {
                    printf("The string \"%s%s%s\" is not a valid guess. Please try again.\n", RED_TEXT, guess, DEFAULT);
                } else {
                    printf("%s%s%s - %s", RED_TEXT, guess, DEFAULT, success.message);
                }
                errors++; // Counts the error messages so far
                printf("\nAvailable letters: ");
                print_color_str("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z", keyboard, 51);
                printf("Guess %d/%d: ", i+1, state.word_length + 1); // Does not increment guess counter
                fgets(guess, 100 * (state.word_length + 1), stdin); // Takes input again
            }
        }

        // Determines the correctness of the guess and prints the colored version back to the user
        analyze_guess(guess, &state, colors, keyboard);

        // Erases any error messages that have accumulated
        while(errors--) {
            CLEAR_LINE(2);
        }

        // Erases the input prompt
        CLEAR_LINE(3);
        printf("\t");
        print_color_str(guess, colors, state.word_length);

        // Checks to see if the puzzle is solved by seeing if every color is green
        if(is_solved(colors, state)) {
            if(i+1 > 1) {
                printf("\nCongratulations! You found the secret word in %d guesses!\n\n", i+1);
            } else {
                printf("\nCongratulations! You found the secret word in %d guess!\n\n", i+1);
            }
            free(guess);
            free(colors);
            terminate(state);
            exit(EXIT_SUCCESS);
        }

        clean(colors, state.word_length);
    }

    printf("\nUnfortunately, you did not find the secret word within %d guesses.\n", state.word_length + 1);
    printf("The secret word was: ");
    print_color_str(state.target, colors, state.word_length);
    printf("Better luck next time!\n\n");

    free(guess);
    free(colors);
    terminate(state);
    exit(EXIT_SUCCESS);
}