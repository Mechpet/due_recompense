/* tictactoe.h
Contains all the functions needed for a tic-tac-toe game with a computer player 
*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "general.h"

#define INACTIVE -1
#define TERMINATING_MOVE 10

/* Describes a game of tic-tac-toe */
struct Game {
    int board[9];    // Pointer to the array of 3x3 array (contains the X and O)
    int turn_no;    // Count of the current turn #; decides who gets to act and when the game ends (last possible turn reached)
    float scenario; // Used only for games against INTELLIGENT computer (is a way to track the order of actions from both players)
} *tictac = NULL;

enum game_results {
    DEFEAT, 
    DRAW, 
    WIN
};

enum difficulties {
    RECKLESS, 
    STANDARD, 
    INTELLIGENT
};

void layout(struct Game *tictac);
void your_turn_TTT(struct Game *tictac);

int row_within(int opposing[5], int allied[5]);
int column_within(int opposing[5], int allied[5]);
int diagonal_within(int opposing[5], int allied[5]);

int counter_ally(int target, int check, int allied[5]);

int target_recognition(struct Game *tictac, int opposing[5], int allied[5]);
int win(struct Game *tictac, char rep);
int is_in(int a, int arr[5]);
void opponent_behavior_intelligent(struct Game *tictac);
void opponent_behavior_standard(struct Game *tictac);
void opponent_behavior_reckless(struct Game *tictac);
int play_opponent(int difficulty, char rep);

/* play_opponent :
 * Arguments: difficulty is used to identify the behavior of the opponent
 *            rep is the character that the player is playing as ('X' or 'O')
 * Implementation: While loop that is terminated when a condition is met - use all helper functions to play
 * Purpose: Initialize the tic-tac-toe game and call helper functions to advance the game */
int play_opponent(int difficulty, char rep) {
    int i, result;
    struct Game *tictac_copy = NULL;
    if ((tictac_copy = tictac = malloc(sizeof(*tictac))) == NULL) {
        fprintf(stderr, "FATAL: No more memory for Tic-tac-toe.\n");
    }
    tictac->turn_no = 1, tictac->scenario = 0.0000; // Initialize the game values
    void (*difficulty_behavior) (struct Game *);
    for (i = 0; i < 9; ++i) { // Initialize the 3x3 board to be empty
        tictac->board[i] = ' ';
    }
    switch (difficulty) {   // Assign the proper function to the computer player
        case RECKLESS:
            difficulty_behavior = opponent_behavior_reckless;
            break;
        case STANDARD:
            difficulty_behavior = opponent_behavior_standard;
            break;
        case INTELLIGENT:
            difficulty_behavior = opponent_behavior_intelligent;
            break;
    }
    while (TRUE) {
        layout(tictac); // Display the current state of the tic-tac-toe board game
        if (rep == 'X') { // The player that has 'X' acts on odd-numbered turns
            your_turn_TTT(tictac);
        }
        else {
            difficulty_behavior(tictac);
        }
        if ((result = win(tictac, (tictac->turn_no % 2) ? 'X' : 'O')) != INACTIVE) { // If a player won the game after the previous turn, the terminating condition has been met
            break; 
        }
        layout(tictac); // Display the state of the tic-tac-toe board game after the previous player acted

        if (tictac->turn_no == TERMINATING_MOVE) {   // On turn #10, there are no more possible actions (if it happens, always happens here)
            break;
        }

        if (rep == 'O') { // The player that has 'O' acts on even-numbered turns
            your_turn_TTT(tictac);
        }
        else {
            difficulty_behavior(tictac);
        }
        if ((result = win(tictac, (tictac->turn_no % 2) ? 'X' : 'O')) != INACTIVE) { // Check for a win after the previous turn for who just acted (terminating condition)
            break;
        }
    }
    // Terminating condition has been met; game has ended and results are to be displayed
    printf("FINAL BOARD:\n");
    layout(tictac); // Display the state of the tic-tac-toe board game after the last turn
    free(tictac_copy);
    tictac = NULL;
    if (result == 'O') {    // Check whether the player won by comparing the result to the player's representation
        printf("O wins!\n");
        sleep(3 * WAIT);
        if (rep == result) {
            return WIN;
        }
        else {
            return DEFEAT;
        }
    }
    else if (result == 'X') {
        printf("X wins!\n");
        sleep(3 * WAIT);
        if (rep == result) {
            return WIN;
        }
        else {
            return DEFEAT;
        }
    }
    else {
        printf("Draw.\n");
        sleep(3 * WAIT);
        return DRAW;
    }
}

/* layout :
 * Arguments: tictac is the game structure used to play the game
 * Implementation: Iteration and printing of automatically initialized strings
 * Purpose: Prints the current tic-tac-toe board (formatted) */
void layout(struct Game *tictac) {
    char horizontal_axis[] = "    0   1   2 ";  // Axis to help the player with input
    char first_row[]       = "      |   |   ";  // Rows with no horizontal separator (printed first)
    char third_row[]       = "   ___|___|___";  // Rows with horizontal separator (printed third)
    char row[15];   // Fits the entire row with the X's and O's and vertical axis
    int i;
    puts(horizontal_axis);
    for (i = 0; i < 9; i += 3) {
        puts(first_row);
        sprintf(row, "%d   %c | %c | %c ", i / 3, tictac->board[i], tictac->board[i + 1], tictac->board[i + 2]); // Print the column # and characters in each cell
        puts(row);
        puts((i != 6) ? third_row : first_row); // Print out the row with a horizontal separator except if it's the last row in the board
    }
}

/* your_turn_TTT:
 * Arguments: tictac is the pointer to the game structure used to play the game
 * Implementation: Receive input and pass it through conditionals until a valid input is received
 * Purpose: Play through a controllable turn of tic-tac-toe */
void your_turn_TTT(struct Game *tictac) {
    char xo = (tictac->turn_no % 2) ? 'X' : 'O';
    char buffer[10];
    int x, y, num_read, square, eff_move;
    unsigned int legal_move = 0;
    while (!legal_move) {
        printf("Enter position (\"Horizontal,Vertical\", such as \"1,1\") to fill in:\n>");
        // Input failure
        if (!fgets(buffer, 10, stdin)) {
            printf("Failed to read input.\n");
        }
        // Input success
        else {
            // Failed to get any of the two expected inputs
            if ((num_read = sscanf(buffer, "%d,%d", &x, &y)) == 0) {
                printf("Expected 2 inputs not read.\n");
            }
            // Failed to get one of the two expected inputs
            else if (num_read == 1) {
                printf("Expected 2 inputs, got 1.\n");
            }
            // Got two inputs that have been assigned to x and y
            else {
                // Inputs are out of bounds
                if (y < 0 || y > 2 || x < 0 || x > 2) {
                    printf("Given space is out of range.\n");
                }
                // Inputs correspond to a position on the board that is not occupied
                else if (tictac->board[square = 3 * y + x] == ' ') {
                    tictac->board[square] = xo;
                    eff_move = (xo == 'O') ? tictac->turn_no / 2 : (tictac->turn_no + 1) / 2;
                    square = (!square) ? 9 : square;
                    tictac->scenario += square / pow(10, eff_move - 1);
                    legal_move = 1;
                    ++tictac->turn_no;
                    break;
                }
                // Inputs correspond to a position on the board that is already occupied
                else {
                    printf("Given space is occupied already.\n");
                }
            }
        }
    }
}

/* win : 
 * Arguments: tictac is the pointer to the game structure used to play the game
 * Implementation: Check whether a target is already occupied (implies it's occupied by an opponent) - if so, you already lost
 * Purpose: Checks if the side allied with rep will lose (returns rep that wins) */
int win(struct Game *tictac, char rep) {
    int opposing[5] = {INACTIVE, INACTIVE, INACTIVE, INACTIVE, INACTIVE};
    int allied[5] = {INACTIVE, INACTIVE, INACTIVE, INACTIVE, INACTIVE};
    int i, t, v, target;
    for (i = 0, t = 0, v = 0; i < 9; ++i) {
        if (tictac->board[i] != ' ' && tictac->board[i] != rep) {
            opposing[t++] = i;
        }
        else if (tictac->board[i] == rep) {
            allied[v++] = i;
        }
    }
    if (t < 2) {
        return INACTIVE;
    }
    else {
        if (tictac->board[target = target_recognition(tictac, opposing, allied)] != ' ' && target != INACTIVE) {
            return (rep == 'X') ? 'O' : 'X';
        }
        else  {
            return INACTIVE;
        }
    }
}

/* target_recognition :
 * Arguments: tictac is the pointer to the game struct used to play the game
              opposing is the array containing all of the positions of the opposing team
              allied is the array containing all of the positions of the allied team
 * Implementation: Call all helper functions (returns positions that need to be blocked) and then check their return values 
 * Purpose: Return a target position that the allied team can take to prevent a loss due to the opposing team */
int target_recognition(struct Game *tictac, int opposing[5], int allied[5]) {
    int row, column, diagonal, rep = tictac->board[allied[0]], foe = (rep == 'X') ? 'O' : 'X';
    row = row_within(opposing, allied), column = column_within(opposing, allied), diagonal = diagonal_within(opposing, allied);
    if (row != INACTIVE && tictac->board[row] == foe) { // Checks for loss first
        return row; 
    }
    else if (column != INACTIVE && tictac->board[column] == foe) {
        return column;
    }
    else if (diagonal != INACTIVE && tictac->board[diagonal] == foe) {
        return diagonal;
    }
    else {  // Checks for loss prevention
        if (row != INACTIVE) {
            return row;
        }
        else if (column != INACTIVE) {
            return column;
        }
        else if (diagonal != INACTIVE) {
            return diagonal;
        }
        else {
            return INACTIVE;
        }
    }
}

/* row_within : 
 * Arguments: opposing is the array containing all of the positions of the opposing team
              allied is the array containing all of the positions of the allied team
 * Implementation: Loop through the ordered opposing array; check for cases where a target can be occupied to prevent a row loss
 * Purpose: Find the space that needs to be filled to not lose to a horizontal attack */
int row_within(int opposing[5], int allied[5]) {
    int i, t, target, prior, later;
    for (i = 0; i < 5 && opposing[i + 1] != INACTIVE; ++i) {
        if ((later = opposing[i + 1]) - (prior = opposing[i]) > 2) { /* since the list is ordered, must be <= 2 spaces apart */
            continue;
        }
        else if (later - prior == 2) {
            if (prior % 3 != 0) { /* 2 spaces away must mean the first one is on the first column */
                continue;
            }
        }
        if (prior % 3 == 2 || later % 3 == 0) { /* wrong row */
            continue;
        }
        if (later % 3 == 1) { /* first column and second column of a row occupied */
            target = later + 1;
        }
        else {   /* first/second row and third column of a row occupied */
            target = (prior % 3 == 0) ? later - 1 : prior - 1;
        }
        for (t = 0; t < 5 && allied[t] != INACTIVE; ++t) {
            if (allied[t] == target) {
                target = INACTIVE;
            }
        }
        if (target != INACTIVE) {
            return target;
        }
    }
    return INACTIVE;
}

/* column_within : 
 * Arguments: opposing is the array containing all of the positions of the opposing team
              allied is the array containing all of the positions of the allied team
 * Implementation: Form 2-D array of arrays corresponding to each column - check for cases where a target can be occupied to prevent a column loss
 * Purpose: Find the space that needs to be filled to not lose to a vertical attack */
int column_within(int opposing[5], int allied[5]) {
    int columns[3][2] = {{INACTIVE, INACTIVE}, {INACTIVE, INACTIVE}, {INACTIVE, INACTIVE}}, i, target = INACTIVE, vertical;
    for (i = 0; i < 5 && opposing[i] != INACTIVE; ++i) {    /* columns[row][column] filled */
        vertical = opposing[i] % 3;
        columns[vertical][(columns[vertical][0] == INACTIVE) ? 0 : 1] = opposing[i];
    }
    for (i = 0; i < 3 && target == INACTIVE; ++i) {
        if (columns[i][1] != INACTIVE) {    /* if two are in the same column */
            if (columns[i][1] < 6) { /* first row and second row of column occupied */
                target = columns[i][1] + 3;
            }
            else { /* first/second row and third row of column occupied */
                target = (columns[i][0] > 2) ? columns[i][0] - 3 : columns[i][0] + 3;
            }
            if (is_in(target, allied)) {
                return INACTIVE;
            }
            /*
            for (int z = 0; z < 5 && allied[z] != INACTIVE; z++) {
                if (allied[z] == target) {
                   return INACTIVE;
                }
            }*/
        }
    }
    return target;
}

/* diagonal_within : 
 * Arguments: opposing is the array containing all of the positions of the opposing team
              allied is the array containing all of the positions of the allied team
 * Implementation: Form 2-D array of arrays corresponding to each diagonal - check for cases where a target can be occupied to prevent a diagonal loss
 * Purpose: Find the space that needs to be filled to not lose to a diagonal attack */
int diagonal_within(int opposing[5], int allied[5]) {
    int i, arr, center = 0, target = INACTIVE;
    int diagonals[2][2] = {{INACTIVE, INACTIVE}, {INACTIVE, INACTIVE}};
    for (i = 0; i < 5; ++i) {
        if (opposing[i] == 4) {
            center = 1;
            continue;
        }
        if (opposing[i] % 2 == 0) {
            arr = (opposing[i] % 4) ? 1 : 0;
            diagonals[arr][diagonals[arr][0] == INACTIVE ? 0 : 1] = opposing[i]; 
        }
    }
    if (center || diagonals[0][1] != INACTIVE || diagonals[1][1] != INACTIVE) {
        if (center) {
            if (diagonals[0][0] == 0) {
                target = counter_ally(target, 8, allied);
            }
            if (diagonals[0][0] == 8) {
                target = counter_ally(target, 0, allied);
            }
            if (diagonals[1][0] == 2) {
                target = counter_ally(target, 6, allied); 
            }
            if (diagonals[1][0] == 6) {
                target = counter_ally(target, 2, allied);
            }
        }
        if (diagonals[0][1] != INACTIVE || diagonals[1][1] != INACTIVE) {
            target = counter_ally(target, 4, allied);
        }
    }
    return target;
}

/* is_in : 
 * Arguments: a is the integer to find 
              arr is the array of integers to check for a
 * Implementation: Linear search
 * Purpose: Returns a non-zero integer when the argument int a is located in the given array */
int is_in(int a, int arr[5]) {
    int i;
    for (i = 0; i < 5; ++i) {
        if (arr[i] == a) {
            return TRUE;
        }
    }
    return FALSE;
}

/* counter_ally :
 * Arguments: target is the current position the computer player wants to move to
              check is the position to check for
              allied is the array containing all of the positions of the allied team
 * Implementation: Helper function call
 * Purpose: Return whether an ally is already occupying the target position */
int counter_ally(int target, int check, int allied[5]) {
    if (target != INACTIVE) {
        return target;
    }
    else {
        return (is_in(check, allied)) ? INACTIVE : check;
    }
}

/* opponent_behavior_intelligent : 
 * Arguments: tictac is the pointer to the game struct used to play the game
 * Implementation: Hard-coded
 * Purpose: Performs the best move according to xkcd */
void opponent_behavior_intelligent(struct Game *tictac) {
    int enemy = (tictac->turn_no % 2) ? 'O' : 'X';
    int ally = (enemy == 'O') ? 'X' : 'O';
    int switched_scenario = (int) (tictac->scenario * 10000);
    while (switched_scenario % 10) {
        ++switched_scenario;
    }
    printf("Your opponent is playing...\n");
    sleep(3 * WAIT);
    if (enemy == 'O') {
        switch ((int) tictac->scenario) {
            case 0:
                tictac->board[0] = ally;
                break;
            case 1:
                switch (switched_scenario) {
                    case 10000:
                        tictac->board[4] = ally;
                        break;
                    case 12000: case 13000: case 15000: case 16000: case 17000:
                        tictac->board[8] = ally;
                        break;
                    case 18000:
                        tictac->board[6] = ally;
                        break;
                    case 18200: case 18500: case 18700:
                        tictac->board[3] = ally;
                        break;
                    case 18300:
                        tictac->board[2] = ally;
                        break;
                }
                break;
            case 2:
                switch (switched_scenario) {
                    case 20000:
                        tictac->board[6] = ally;
                        break;
                    case 21000: case 24000: case 25000: case 27000: case 28000:
                        tictac->board[3] = ally;
                        break;
                    case 23000:
                        tictac->board[8] = ally;
                        break;
                    case 23100: case 23400: case 23500:
                        tictac->board[7] = ally;
                        break;
                    case 23700:
                        tictac->board[4] = ally;
                        break;
                }
                break;
            case 3:
                switch (switched_scenario) {
                    case 30000:
                        tictac->board[1] = ally;
                        break;
                    case 32000:
                        tictac->board[4] = ally;
                        break;
                    case 34000: case 35000: case 36000: case 37000: case 38000:
                        tictac->board[2] = ally;
                        break;
                    case 32500: case 32600: case 32800:
                        tictac->board[7] = ally;
                        break;
                    case 32700:
                        tictac->board[8] = ally;
                        break;
                }
                break;
            case 4:
                switch (switched_scenario) {
                    case 40000:
                        tictac->board[8] = ally;
                        break;
                    case 41000: case 42300: case 42500: case 43210: case 45610:
                        tictac->board[7] = ally;
                        break;
                    case 42000: case 41200: case 41300: case 41500: case 43200: case 45100: case 45200: case 45700: case 47200:
                        tictac->board[6] = ally;
                        break;
                    case 43000: case 41630: case 42630: case 43100: case 43600: case 43700: case 46100: case 46700: case 47230:
                        tictac->board[5] = ally;
                        break;
                    case 45000: case 41650: case 42100: case 42650: case 42700: case 47250:
                        tictac->board[3] = ally;
                        break;
                    case 46000: case 41600: case 42600: case 45600: case 45670: case 47300: case 47500: case 47600:
                        tictac->board[2] = ally;
                        break;
                    case 47000: case 43270: case 46300: case 46500:
                        tictac->board[1] = ally;
                        break;
                }
                break;
            case 5:
                switch (switched_scenario) {
                    case 50000:
                        tictac->board[4] = ally;
                        break;
                    case 51000: case 52000: case 53000: case 56000: case 57000:
                        tictac->board[8] = ally;
                        break;
                    case 58000:
                        tictac->board[2] = ally;
                        break;
                    case 58100:
                        tictac->board[6] = ally;
                        break;
                    case 58300: case 58600: case 58700:
                        tictac->board[1] = ally;
                        break;
                }
                break;
            case 6:
                switch (switched_scenario) {
                    case 60000:
                        tictac->board[2] = ally;
                        break;
                    case 61000:
                        tictac->board[8] = ally;
                        break;
                    case 61300: case 61400: case 61700:
                        tictac->board[5] = ally;
                        break;
                    case 61500:
                        tictac->board[4] = ally;
                        break;
                    case 63000: case 64000: case 65000: case 67000: case 68000:
                        tictac->board[1] = ally;
                        break;
                }
                break;
            case 7:
                switch (switched_scenario) {
                    case 71000: case 72000: case 73000: case 75000: case 76000:
                        tictac->board[8] = ally;
                        break;
                    case 78000:
                        tictac->board[6] = ally;
                        break;
                    case 78100: case 78200: case 78500:
                        tictac->board[3] = ally;
                        break;
                    case 78300:
                        tictac->board[2] = ally;
                        break;
                }
                break;
            case 8:
                switch (switched_scenario) {
                    case 80000:
                        tictac->board[2] = ally;
                        break;
                    case 81000:
                        tictac->board[6] = ally;
                        break;
                    case 81300:
                        tictac->board[4] = ally;
                        break;
                    case 81400: case 81500: case 81700:
                        tictac->board[3] = ally;
                        break;
                    case 83000: case 84000: case 85000: case 86000: case 87000:
                        tictac->board[1] = ally;
                        break;
                }
                break;
        }
    }
    else {
        switch((int) tictac->scenario) {
            case 1:
                switch (switched_scenario) {
                    case 10000:
                        tictac->board[4] = ally;
                        break;
                    case 19000: case 13000: case 15800: case 17830: case 17850: case 18500:
                        tictac->board[2] = ally;
                        break;
                    case 19300: case 19500: case 19700: case 19800: case 12830: case 13900: case 13500: case 13700: case 13800: case 15830: 
                    case 15870: case 17800: case 18590: case 18570:
                        tictac->board[6] = ally;
                        break;
                    case 19600: case 12860: case 12870: case 16900: case 16200: case 16700: case 16800: case 17820: case 18000: 
                        tictac->board[3] = ally;
                        break;
                    case 19650: case 13680: case 15860: case 16380: case 18560:
                        tictac->board[7] = ally;
                        break;
                    case 19670: case 19680: case 12800: case 16000: case 18900: case 18200: case 18600: case 18700:
                        tictac->board[5] = ally;
                        break;
                    case 12000: case 13600: case 15000: case 16300: case 17000:
                        tictac->board[0] = ally;
                        break;
                    case 12300: case 12500: case 12600: case 12700: case 13650: case 13670: case 15200: case 15300: case 15600: case 15700: 
                    case 16320: case 16370: case 17200: case 17300: case 17500: case 17600:
                        tictac->board[8] = ally;
                        break;
                }
                break;
            case 2:
                switch (switched_scenario) {
                    case 20000:
                        tictac->board[4] = ally;
                        break;
                    case 29000: case 23900: case 23500: case 23600: case 23800: case 25900: case 26000: case 27590: case 28300:
                        tictac->board[1] = ally;
                        break;
                    case 29300: case 29500: case 29600: case 29800: case 23000: case 25930: case 25960: case 26900: case 26300: case 26500: 
                    case 26800: case 28390: case 28360:
                        tictac->board[7] = ally;
                        break;
                    case 29700: case 21860: case 21870: case 25970: case 26790: case 27000: case 28900: case 28100: case 28600: case 28700:
                        tictac->board[3] = ally;
                        break;
                    case 29750: case 21300: case 21500: case 21600: case 21700: case 23150: case 23160: case 25000: case 26700: case 27500:
                        tictac->board[8] = ally;
                        break;
                    case 29760: case 29780: case 21800: case 23180: case 27900: case 27100: case 27600: case 27800: case 28000:
                        tictac->board[5] = ally;
                        break;
                    case 21000: case 23100: case 25100: case 25300: case 25600: case 25700: case 26730: case 26750: case 27510: case 27560:
                        tictac->board[0] = ally;
                        break;
                    case 21830: case 28370:
                        tictac->board[6] = ally;
                        break;
                }
                break;
            case 3:
                switch (switched_scenario) {
                    case 30000:
                        tictac->board[4] = ally;
                        break;
                    case 39000: case 31900: case 31500: case 31700: case 31800: case 35810: case 37800: case 38700:
                        tictac->board[6] = ally;
                        break;
                    case 39100: case 39500: case 39700: case 39800: case 31000: case 35800: case 36810: case 37810: case 37850: case 38790: 
                    case 38750:
                        tictac->board[2] = ally;
                        break;
                    case 39200: case 32900: case 32500: case 32600: case 32800: case 35860: case 35870: case 36820: case 36850: case 38000:
                        tictac->board[1] = ally;
                        break;
                    case 39250: case 39280: case 31680: case 32000: case 36800: case 38900: case 38200: case 38500: case 38600:
                        tictac->board[7] = ally;
                        break;
                    case 39270: case 32180: case 37820: case 38720:
                        tictac->board[5] = ally;
                        break;
                    case 31600: case 32100: case 35000: case 36000: case 37000:
                        tictac->board[0] = ally;
                        break;
                    case 31650: case 31670: case 32150: case 32160: case 35100: case 35200: case 35600: case 35700: case 36100: case 36200: 
                    case 36500: case 36700: case 37100: case 37200: case 37500: case 37600:
                        tictac->board[8] = ally;
                        break;
                }
                break;
            case 4:
                switch (switched_scenario) {
                    case 40000:
                        tictac->board[0] = ally;
                        break;
                    case 41000: case 42310: case 43100: case 43210: case 45610: case 46100: case 48100:
                        tictac->board[7] = ally;
                        break;
                    case 41200: case 41320: case 41520: case 41580: case 41820: case 41850: case 42000: case 43120: case 43200: case 43720: 
                    case 43820: case 45100: case 45200: case 45700: case 45800: case 47200:
                        tictac->board[6] = ally;
                        break;
                    case 41230:
                        tictac->board[8] = ally;
                        break;
                    case 41250: case 41280: case 41500: case 41650: case 41680: case 41800: case 42100: case 42500: case 42700: case 42800: 
                    case 45000: case 46150: case 46180: case 47250: case 47280: case 48150: case 48160:
                        tictac->board[3] = ally;
                        break;
                    case 41300: case 41630: case 42300: case 43000: case 46130: case 47230: case 48130:
                        tictac->board[5] = ally;
                        break;
                    case 41360: case 41380: case 41560: case 41600: case 41860: case 43160: case 43180: case 43600: case 43760: case 43780:
                    case 43860: case 43870: case 45600: case 46000: case 47300: case 47500: case 47600: case 47800: case 48000:
                        tictac->board[2] = ally;
                        break;
                    case 42370: case 42380: case 43270: case 43280: case 43670: case 43680: case 43700: case 43800: case 45670: case 45680: 
                    case 46300: case 46500: case 46700: case 46800: case 47000: case 48300: case 48500: case 48600: case 48700:
                        tictac->board[1] = ally;
                        break;
                }
                break;
            case 5:
                switch (switched_scenario) {
                    case 50000:
                        tictac->board[4] = ally;
                        break;
                    case 59000: case 51860: case 52930: case 52960: case 56900: case 56200: case 56300: case 56800: case 58600:
                        tictac->board[7] = ally;
                        break;
                    case 59100: case 51800: case 53800: case 57000: case 58000:
                        tictac->board[2] = ally;
                        break;
                    case 59130: case 59180: case 51830: case 51870: case 53810: case 57900: case 57100: case 57300: case 57800: case 58900:
                    case 58100: case 58300: case 58700:
                        tictac->board[6] = ally;
                        break;
                    case 59160: case 52970: case 56790: case 57690:
                        tictac->board[3] = ally;
                        break;
                    case 59200: case 59300: case 59600: case 59800: case 52900: case 53860: case 53870: case 56000: case 58690: case 58630:
                        tictac->board[1] = ally;
                        break;
                    case 51000: case 52100: case 52300: case 52600: case 52700: case 53000: case 56720: case 56730: case 57610: case 47630:
                    case 58620:
                        tictac->board[0] = ally;
                        break;
                    case 51200: case 51300: case 51600: case 51700: case 52000: case 53100: case 53200: case 53600: case 53700: case 56700:
                    case 57600:
                        tictac->board[8] = ally;
                        break;
                }
                break;
            case 6:
                switch (switched_scenario) {
                    case 60000:
                        tictac->board[4] = ally;
                        break;
                    case 69000: case 61900: case 61200: case 61700: case 61800: case 62790: case 65790: case 67900: case 68100:
                        tictac->board[3] = ally;
                        break;
                    case 69100: case 69200: case 69700: case 69800: case 61000: case 67910: case 67920: case 68190: case 68120:
                        tictac->board[5] = ally;
                        break;
                    case 69500: case 62000: case 63820: case 63850: case 65000: case 67950: case 68900: case 68200: case 68300: case 68500:
                        tictac->board[1] = ally;
                        break;
                    case 69520: case 69580: case 61380: case 62900: case 62300: case 62500: case 62800: case 63800: case 65900: case 65200:
                    case 65300: case 65800: case 68000:
                        tictac->board[7] = ally;
                        break;
                    case 69570: case 61320: case 61370: case 62700: case 63100: case 63200: case 63500: case 63700: case 65700: case 67000:
                        tictac->board[8] = ally;
                        break;
                    case 61300: case 62730: case 62750: case 63000: case 65720: case 65730: case 67100: case 67200: case 67300: case 67500:
                        tictac->board[0] = ally;
                        break;
                    case 64810: case 68150:
                        tictac->board[2] = ally;
                        break;
                }
                break;
            case 7:
                switch (switched_scenario) {
                    case 70000:
                        tictac->board[4] = ally;
                        break;
                    case 79000: case 72900: case 72100: case 72600: case 72800: case 73820: case 76910: case 76920: case 78200:
                        tictac->board[5] = ally;
                        break;
                    case 79100: case 79200: case 79600: case 79800: case 71820: case 72000: case 75690: case 76900: case 78290: case 78210:
                        tictac->board[3] = ally;
                        break;
                    case 79300: case 71800: case 73800: case 75900: case 75100: case 75300: case 75800: case 78000:
                        tictac->board[6] = ally;
                        break;
                    case 79310: case 79380: case 71830: case 71850: case 73810: case 73850: case 75000: case 78900: case 78100: case 78300:
                    case 78500:
                        tictac->board[2] = ally;
                        break;
                    case 79320: case 72590: case 76950:
                        tictac->board[1] = ally;
                        break;
                    case 71000: case 72510: case 72560: case 73000: case 75610: case 75630: case 76100: case 76200: case 76300: case 76500:
                    case 78230:
                        tictac->board[0] = ally;
                        break;
                    case 71200: case 71300: case 71500: case 71600: case 72500: case 73100: case 73200: case 73500: case 73600: case 75600:
                    case 76000:
                        tictac->board[8] = ally;
                        break;
                }
                break;
            case 8:
                switch (switched_scenario) {
                    case 80000:
                        tictac->board[4] = ally;
                        break;
                    case 89000: case 82300: case 83000: case 85690: case 85630: case 86900: case 86200: case 86300: case 86500:
                        tictac->board[1] = ally;
                        break;
                    case 89200: case 89300: case 89500: case 89600: case 81560: case 82390: case 82360: case 83900: case 83200: case 83500:
                    case 83600: case 85600: case 86000:
                        tictac->board[7] = ally;
                        break;
                    case 89700: case 81590: case 81570: case 82370: case 83700: case 85900: case 85100: case 85300: case 85700: case 87000:
                        tictac->board[6] = ally;
                        break;
                    case 89720: case 81900: case 81200: case 81600: case 81700: case 82000: case 83720: case 86190: case 86120: case 87200:
                        tictac->board[5] = ally;
                        break;
                    case 89730: case 89750: case 81500: case 83790: case 83750: case 85000: case 86150: case 87900: case 87100: case 87300:
                    case 87500:
                        tictac->board[2] = ally;
                        break;
                    case 81000: case 82900: case 82100: case 82600: case 82700: case 86100: case 87290: case 87210:
                        tictac->board[3] = ally;
                        break;
                    case 85610: case 87230:
                        tictac->board[0] = ally;
                        break;
                }
                break;
            case 9:
                switch (switched_scenario) {
                    case 90000:
                        tictac->board[4] = ally;
                        break;
                    case 91000: case 93100: case 93500: case 93700: case 93800: case 95100: case 97310: case 97380: case 98730: case 98750:
                        tictac->board[2] = ally;
                        break;
                    case 91300: case 91500: case 91700: case 91800: case 93000: case 95130: case 95180: case 97300: case 98700:
                        tictac->board[6] = ally;
                        break;
                    case 91600: case 92700: case 95160: case 96000: case 97100: case 97200: case 97600: case 97800:
                        tictac->board[3] = ally;
                        break;
                    case 91650: case 91670: case 92750: case 96570:
                        tictac->board[8] = ally;
                        break;
                    case 91680: case 92300: case 92500: case 92600: case 92800: case 93250: case 93280: case 95000: case 96520: case 96580: case 98200: case 98300: case 98500: case 98600:
                        tictac->board[7] = ally;
                        break;
                    case 92000: case 93200: case 95200: case 95300: case 95600: case 95800: case 96500: case 97320: case 98000:
                        tictac->board[1] = ally;
                        break;
                    case 92760: case 92780: case 93270: case 96100: case 96200: case 96700: case 96800: case 97000: case 98720: 
                        tictac->board[5] = ally;
                        break;
                }
                break;
        }
    }
    ++tictac->turn_no;
}

/* opponent_behavior_standard : 
 * Arguments: tictac is the pointer to the game struct used to play the game
 * Implementation: Recognize targets on both sides first; if nothing is recognized, occupy middle tile if possible; if not possible, occupy a random tile
 * Purpose: Plays based on priority: win game > prevent loss > occupy middle tile > occupy random tile */
void opponent_behavior_standard(struct Game *tictac) {
    int i, t, v, target, spot, rep = (tictac->turn_no % 2) ? 'X' : 'O';
    int foe = (rep == 'X') ? 'O' : 'X';
    int opposing[5] = {INACTIVE, INACTIVE, INACTIVE, INACTIVE, INACTIVE};
    int allied[5] = {INACTIVE, INACTIVE, INACTIVE, INACTIVE, INACTIVE};
    printf("Your opponent is playing...\n");
    sleep(3 * WAIT);
    for (i = 0, t = 0, v = 0; i < 9; ++i) {
        if (tictac->board[i] != ' ' && tictac->board[i] != rep) {
            opposing[t++] = i;
        }
        else if (tictac->board[i] == rep) {
            allied[v++] = i;
        }
    }
    if ((target = target_recognition(tictac, allied, opposing)) != INACTIVE) {
        tictac->board[target] = rep;
    }
    else if ((target = target_recognition(tictac, opposing, allied)) != INACTIVE) {
        tictac->board[target] = rep;
    }
    else {
        if (tictac->board[4] == ' ') {
            target = 4;
        }
        else {
            while ((spot = tictac->board[target = rand() % 9]) == foe || spot == rep) {
                ;
            }
        }
        tictac->board[target] = rep; 
    }
    ++tictac->turn_no;
}

/* opponent_behavior_reckless : 
 * Arguments: tictac is the pointer to the game struct used to play the game
 * Implementation: Randomize numbers in the range [0, 8] until a number comes up s.t. it's not occupied in the game
 * Purpose: Play completely random (pick a random position and occupy it) */
void opponent_behavior_reckless(struct Game *tictac) {
    int target, spot, rep = (tictac->turn_no % 2) ? 'X' : 'O';
    int foe = (rep == 'X') ? 'O' : 'X';
    printf("Your opponent is playing...\n");
    sleep(3 * WAIT);
    while ((spot = tictac->board[target = rand() % 9]) == foe || spot == rep) {
        ;
    }
    tictac->board[target] = rep; 
    ++tictac->turn_no;
}