/* controller.h
Functions that implement the input system.
The result is the player/user types in their input and submits via 'enter' key. */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <conio.h>
#include <string.h>
#include "general.h"

// Shields from stacking this header file in other files
#ifndef CONTROLLER_H
#define CONTROLLER_H
#define BUFFER_SIZE 25  // Maximum characters of input the functions are willing to receive
#define MAX_NUM 5   // Maximum digits of input the functions are willing to receive

// Rename all ASCII values to meaningful definitions/utility
enum bindings {
    _EOF        = -1,
    Placeholder = 0,
    Flush       = 10, /* '\n' */
    Start       = 32, /* ' ' */
    Help        = 63, /* '?' */
    RR          = 82, /* 'R' */
    Shop        = 83, /* 'S' */
    Left        = 97, /* 'a' */
    Cancel      = 99, /* 'c' */
    Defense     = 99, /* 'c' */
    Right       = 100, /* 'd' */
    Easy        = 101, /* 'e' */
    Fight       = 102, /* 'f' */
    Hard        = 104, /* 'h' */
    Inventory   = 105, /* 'i' */
    Lessons     = 108, /* 'l' */
    Monies      = 109, /* 'm' */
    Normal      = 110, /* 'n' */
    No          = 110, /* 'n' */
    Options     = 111, /* 'o' */
    Select      = 114, /* 'r' */
    Speed       = 115, /* 's' */
    Down        = 115, /* 's' */
    Sword       = 115, /* 's' */
    Sibling     = 115, /* 's' */
    TicTacToe   = 116, /* 't' */
    Display     = 119, /* 'w' */
    Up          = 119, /* 'w' */
    Signature   = 120, /* 'x' */
    Yes         = 121, /* 'y' */
    Attack      = 122  /* 'z' */
};

// Describes the game's current state - what is the player's screen in right now?
struct state_vals {
    unsigned int is_waiting : 1;    // Waiting for something to happen/pass until input is accepted again
    unsigned int is_title   : 1;
    unsigned int is_map     : 1;
    unsigned int is_battle  : 1;
    unsigned int is_fate  : 1;
};

// Records the player's given input and game's current state to see what actions are legal
struct controller {
    char input; // Character that was parsed to be the input from the player
    struct state_vals player_state; // Describes what the game state is (useless)
} *Controller;

/* getaction : 
 * Arguments: in is of the controller structure (will be a global variable)
 * Implementation: Attempt to get defined inputs through checks; returns meaningful value if all tests are passed (line-based input)
 * Purpose: Receive standard input if in the position to do so (based on player_state), then return the decided input */
int getaction(struct controller *in) {
    char line[BUFFER_SIZE];
    Mix_Chunk *map_wav = NULL;
    if (!Mix_Playing(-1)) {
        if (in->player_state.is_map && in->player_state.is_battle) {
            if ((map_wav = Mix_LoadWAV("Music\\Map_island.wav")) == NULL) {
                fprintf(stderr, "Could not load map wav file.\n");
            }
        }
        else if (in->player_state.is_map && in->player_state.is_title) {
            if ((map_wav = Mix_LoadWAV("Music\\Charging.wav")) == NULL) {
                fprintf(stderr, "Could not load map wav file.\n");
            }
        }
        else {
            if ((map_wav = Mix_LoadWAV("Music\\Map_normal.wav")) == NULL) {
                fprintf(stderr, "Could not load map wav file.\n");
            }
        }
        if (Mix_FadeInChannel(-1, map_wav, -1, 1000) == -1) {
            fprintf(stderr, "Could not play map wav file.\n");
        }
    }
    if (in->player_state.is_waiting || _kbhit()) {  // If player is supposed to be waiting or the program is in sleep() && input to the console has been detected
        _getch();   // Discards input typed while in sleep()
        return Placeholder;
    }
    else {
        printf(">");    // Prompt the player for input with this character
        fflush(stdin);
        if (fgets(line, BUFFER_SIZE, stdin) == NULL) {   // Gets input up to '\n' if of defined behavior
            return _EOF;    // If the behavior is undefined, return -1
        }
        if (sscanf(line, "%c", &in->input) != 1) {   // Stores 1 character of input in controller.input
            return _EOF;
        }
    }
    return in->input;   // All tests passed and a character was inputted; just return that character of input
}

/* get_pos_int:
 * Arguments: 
 * Implementation: Attempts to get defined inputs through checks; returns integer that user input if all tests are passed (line-based input)
 * Purpose: Receive standard input when called (meant for receiving a number), then return the positive integer typed if it's an int */
int get_pos_int(void) {
    char number[MAX_NUM];
    int i, num = 0;
    printf(">");    // Prompt the player for input with this character
    if (fgets(number, MAX_NUM, stdin) == NULL) { // Gets input up to '\n' if of defined behavior
        return _EOF;    // If the behavior is undefined, return -1
    }
    for (i = 0; i < (int) strlen(number); ++i) {  // Iterate through the array of characters, checking that all of them are digits
        if (!isdigit(number[i]) && number[i] != '\n') { // Only digits and '\n' should be accepted, so return -1 if another character was found
            printf("Invalid number.\n");
            return _EOF;
        }
        else if (isdigit(number[i])) {   // If the character at number[i] was a digit, sum up in the integer value num
            num = (num * 10 + number[i] - '0'); // Multiply by 10 for every extra number (weight the order of numbers) and add the extra number
        }
    }
    return num;
}

/* initialize_controller : 
 * Arguments: in is of the controller structure (will be a global variable)
 * Implementation: Dynamic memory allocation
 * Purpose: Allocates memory for the controller structure */
struct controller *initialize_controller(struct controller *in) {
    if ((in = malloc(sizeof(*in))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate controller");
    }
    return in;
}

/* set_state_vals : 
 * Arguments: in is of the controller structure (will be a global variable)
 *            waiting, title, map, battle, invent - are the unsigned integers to set to the player state
 * Implementation: Assign the values
 * Purpose: Sets the state values in the controller */
void set_state_vals(struct controller *in, unsigned int waiting, unsigned int title, unsigned int map, unsigned int battle, unsigned int fate) {
    in->player_state.is_waiting = waiting, in->player_state.is_title = title, in->player_state.is_map = map, in->player_state.is_battle = battle, in->player_state.is_fate = fate;
}

/* quit : 
 * Arguments:
 * Implementation: Automatic strings are initialized
 * Purpose: Replaces current screen with the "FIN" screen */
void quit(void) {
    clrscr();
    char end_buffer[] =
    " _______________________________________\n"
    "|  ________    ______     ____     ___  |\n"
    "| (   ____/ . /_   _/  _ |    \\ ` /   | |\n"
    "|  \\  \\___     |  /_  __ !  |\\ \\__|  !  |\n"
    "| _ |  ___\\   [____/ ___ |  | \\      |  |\n"
    "| __ \\_\\         .       /  / `\\____/   |\n"
    "|        .              |__/      .     |\n"
    "|______________________________________/\n"
    "                  \"FIN\"";
    puts(end_buffer);
    sleep(5);
    exit(EXIT_SUCCESS);
}

/* player_death : 
 * Arguments:
 * Implementation: Automatic strings are initialized
 * Purpose: Replaces current screen with the "DEATH" screen */
void player_death(void) {
    clrscr();
    char die_buffer[] =
    ":::::::::::::::::::::::::::::::::::::::::\n"
    ":::::::::::::::::::::::::::::::::::::::::\n"
    "::::::::::::::::::_:_:_::::::::::::::::::\n"
    "::::::::::::::_:_|     |_:_::::::::::::::\n"
    "::::::::::::|____  /|\\  ____|::::::::::::\n"
    "::::::::::::::::|       |::::::::::::::::\n"
    "::::::::::::::::|   *   |::::::::::::::::\n"
    "________________|__\\|/__|________________\n"
    "             \"Here lies you\"";
    puts(die_buffer);
    sleep(5);

}
#endif