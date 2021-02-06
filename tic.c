#include "tictactoe.h"

int main(void) {
    int i, result;
    if ((tictac = malloc(sizeof(*tictac))) == NULL)
        fprintf(stderr, "FATAL: No more memory for Tic-tac-toe.\n");
    if ((tictac->board = malloc(sizeof(int [9]))) == NULL)
        fprintf(stderr, "FATAL: No more memory for Tic-tac-toe board.\n");
    tictac->turn_no = 1, tictac->scenario = 0.0000;
    for (i = 0; i < 9; ++i)
        *tictac->board[i] = ' ';
    while (1) {
        layout(tictac);
        your_turn(tictac);
        if ((result = win(tictac, (tictac->turn_no % 2) ? 'X' : 'O')) != INACTIVE)
            break; 
        layout(tictac);

        if (tictac->turn_no == TERMINATING_MOVE)
            break;

        your_turn(tictac);
        if ((result = win(tictac, (tictac->turn_no % 2) ? 'X' : 'O')) != INACTIVE)
            break;
    }
    printf("FINAL BOARD:\n");
    layout(tictac);
    free(tictac->board);
    free(tictac);
    if (result == 'O') {
        printf("O wins!\n");
    }
    else if (result == 'X') {
        printf("X wins!\n");
    }
    else {
        printf("Draw.\n");
        sleep(3);
        return DRAW;
    }
}

/*
X: 4, 7, 2
O: 3, 1, 6
*/