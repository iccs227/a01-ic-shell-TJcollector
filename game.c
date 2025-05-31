
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

void wordle_game() {
    const char *answer = "fades";  //no time to make it as a list/array instead sobbing...
    char guess[100]; // allow room for extra input, we'll trim to 5 chars
    int max_attempts = 6;

    printf("Welcome to Wordle!\n");
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        printf("Guess #%d: ", attempt + 1);
        fflush(stdout);

        if (fgets(guess, sizeof(guess), stdin) == NULL) {
            printf("\n");
            return;
        }

        // Remove trailing newline
        guess[strcspn(guess, "\n")] = '\0';

        if (strlen(guess) != 5) {
            printf("Please enter exactly 5 letters.\n");
            attempt--;
            continue;
        }

        int valid = 1;
        for (int i = 0; i < 5; i++) {
            if (!isalpha(guess[i])) {
                valid = 0;
                break;
            }
        }

        if (!valid) {
            printf("Only alphabetic letters allowed.\n");
            attempt--;
            continue;
        }

        int correct = 1;
        for (int i = 0; i < 5; i++) {
            if (guess[i] == answer[i]) {
                printf("\033[1;32m%c\033[0m", guess[i]); // Green for correct position
            } else if (strchr(answer, guess[i]) != NULL) {
                printf("\033[1;33m%c\033[0m", guess[i]); // Yellow for wrong position
                correct = 0;
            } else {
                printf("%c", guess[i]); // Default for incorrect letter
                correct = 0;
            }
        }
        printf("\n");

        if (correct) {
            printf("Yay! You guessed the word!\n");
            return;
        }
    }

    printf("Better luck next time! The word was '%s'.\n", answer);
}
