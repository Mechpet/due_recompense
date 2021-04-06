#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "general.h"

int choose_rps(void);
int choose_rps_random(void);
int shoot(void);

int choose_rps(void) {
    int c;
    while (c != EOF && c != 'r' && c != 'R' && c != 'p' && c != 'P' && c != 's' && c != 'S') {
        printf("What will you play?:\nr = rock, p = paper, s = scissors\n>");
        c = getchar();
        flush_input();
    }
    switch (tolower(c)) {
        case 'r':
            return 0;
            break;
        case 'p':
            return 1;
            break;
        case 's':
            return 2;
            break;
    }
    return 0;
}

int choose_rps_random(void) {
    return (rand() % 3);
}

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
    int player_choice = choose_rps(), opponent_choice = choose_rps_random(), i;
    for (i = 0; i < 7; ++i)
        printf("%s%s", rock_a[i], rock_b[i]);
    sleep(1);
    for (i = 0; i < 7; ++i)
        printf("%s%s", paper_a[i], paper_b[i]);
    sleep(1);
    for (i = 0; i < 7; ++i)
        printf("%s%s", scissors_a[i], scissors_b[i]);
    sleep(1);
    printf("%22s\n", "SHOOT!");
    sleep(1);
    for (i = 0; i < 7; ++i)
        printf("%s%s", artworks[0][player_choice][i], artworks[1][opponent_choice][i]);
    if (player_choice == opponent_choice) {
        printf("Draw.\n");
        return 0;
    }
    else if ((player_choice == 0 && opponent_choice == 1) ||
             (player_choice == 1 && opponent_choice == 2) ||
             (player_choice == 2 && opponent_choice == 0)) {
        printf("Your opponent wins.\n");
        return 2;
    }
    else {
        printf("You win.\n");
        return 1;
    }    
}