/* rockpaperscissors.h
Contains all the functions needed for a rock-paper-scissors game with a computer player 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "general.h"

int choose_rps(void);
int choose_rps_random(void);
int shoot(void);

enum choice {
    ROCK,
    PAPER,
    SCISSORS
};

/* choose_rps
 * Arguments:
 * Implementation: While loop is terminated when a condition is met 
 * Purpose: Returns a way to identify the of choice of rock, paper, or scissors the player decides */
int choose_rps(void) {
    int c;
    printf("What will you play?:\nr = rock, p = paper, s = scissors\n");
    while (c != EOF && c != 'r' && c != 'R' && c != 'p' && c != 'P' && c != 's' && c != 'S') {
        printf(">");
        c = getchar();
        flush_input();
    }
    switch (tolower(c)) {
        case 'r':
            return ROCK;
            break;
        case 'p':
            return PAPER;
            break;
        case 's':
            return SCISSORS;
            break;
    }
    return 0;
}

/* choose_rps_random :
 * Arguments:
 * Implementation: Random number generator
 * Purpose: Return a random integer in the range [0, 2] */
int choose_rps_random(void) {
    return (rand() % 3);
}

/* shoot :
 * Arguments:
 * Implementation: Call all helper functions to play the game
 * Purpose: Play a single round of rock-paper-scissors */
int shoot(void) {
    char rock_a[7][25] = {
        "                    ",
        "   ______           ",
        "--'   _  |_         ",
        "     (_| |_)        ",
        "     (_\\_|_)        ",
        "     (_____)        ",
        "--.___(___)         ",
    };
    char rock_b[7][25] = {
        "\n",
        "         ______   \n",
        "       _|  _   '--\n",
        "      (_| |_)   \n",
        "      (_|_/_)\n",
        "      (_____)\n",
        "       (___)___.--\n",
    };
    char scissors_a[7][25] = {
        "               __   ",
        "   ______ _--\"\"__)  ",
        "--'  __  |.--\"\"     ",
        "       | |'--..__   ",
        "      _\\_|_--..__)  ",
        "     (_____)        ",
        "--.___(___)         ",
    };
    char scissors_b[7][25] = {
        " __\n",
        "(__\"\"--_ ______\n",
        "   \"\"--.|  __  '--\n",
        " __..--'| |\n",
        "(__..--_|_/_\n",
        "      (_____)\n",
        "       (___)___.--\n",
    };
    char paper_a[7][25] = {
        "                    ",
        "   ________         ",
        "--'   _____)____    ",
        "         _______)   ",
        "         ________)  ",
        "        ________)   ",
        "--.___________)     ",
    };
    char paper_b[7][25] = {
        "\n",
        "       ________\n",
        "  ____(_____   '--\n",
        " (_______\n",
        "(________\n",
        " (________\n",
        "   (___________.--\n",
    };
    
    char (*artworks[2][3])[25] = {
        {rock_a, paper_a, scissors_a},
        {rock_b, paper_b, scissors_b},
    };
    int i, player_choice = choose_rps(), opponent_choice = choose_rps_random();
    for (i = 0; i < 7; ++i)
        printf("%s%s", rock_a[i], rock_b[i]);
    sleep(WAIT);
    for (i = 0; i < 7; ++i)
        printf("%s%s", paper_a[i], paper_b[i]);
    sleep(WAIT);
    for (i = 0; i < 7; ++i)
        printf("%s%s", scissors_a[i], scissors_b[i]);
    sleep(WAIT);
    printf("%22s\n", "SHOOT!");
    sleep(WAIT);
    for (i = 0; i < 7; ++i)
        printf("%s%s", artworks[0][player_choice][i], artworks[1][opponent_choice][i]);
    if (player_choice == opponent_choice) {
        printf("Draw.\n");
        return 0;
    }
    else if ((player_choice == ROCK && opponent_choice == PAPER) ||
             (player_choice == PAPER && opponent_choice == SCISSORS) ||
             (player_choice == SCISSORS && opponent_choice == ROCK)) {
        printf("Your opponent wins.\n");
        return 2;
    }
    else {
        printf("You win.\n");
        return 1;
    }    
}