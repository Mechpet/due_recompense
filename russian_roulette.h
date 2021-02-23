#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "general.h"

enum shooters {
    YOU, ENEMY
};

enum turn_result {
    ALIVE, DEAD
};

struct shootoff {
    unsigned int chamber[6];
    unsigned int chamber_no;
    unsigned int your_spin;
    unsigned int their_spin;
    unsigned int first_shooter;
} *roulette;

int duel_enemy();
int their_turn_RR(struct shootoff *roulette);
int your_turn_RR(struct shootoff *roulette);
int shoot_gun(struct shootoff *roulette, int user);
struct shootoff *init_roulette(void);
void reset_chamber(struct shootoff *roulette);
void spin_chamber(struct shootoff *roulette, int user);

int duel_enemy() {
    roulette = init_roulette();
    int result;
    while (1) {
        if (roulette->first_shooter == ENEMY) {
            if (their_turn_RR(roulette)) {
                return ALIVE;
            }
            if (your_turn_RR(roulette)) {
                return DEAD;
            }
        }
        else {
            if (your_turn_RR(roulette)) {
                return DEAD;
            }
            if (their_turn_RR(roulette)) {
                return ALIVE;
            }
        }
    }
}

int their_turn_RR(struct shootoff *roulette) {
    if (roulette->their_spin) { // Consider spinning chamber
        if ((rand() % 6) <= roulette->chamber_no) {
            spin_chamber(roulette, 0);
            return their_turn_RR(roulette);
        }
    }
    return shoot_gun(roulette, ENEMY);
}

int your_turn_RR(struct shootoff *roulette) {
    int c;
    printf("Press 'k' to shoot yourself, 's' to spin the chamber (%d remaining).\n>", roulette->your_spin);
    while ((c = getchar()) != EOF && c != 'k' && ((!roulette->your_spin && c == 's') || c != 's'))
        ;
    switch (c) {
        case 'k':
            return shoot_gun(roulette, YOU);
            break;
        case 's':
            spin_chamber(roulette, YOU);
            return your_turn_RR(roulette);
            break;
    }
}

int shoot_gun(struct shootoff *roulette, int user) {
    printf("%s pulled the trigger...\n", (user == ENEMY) ? "They" : "You");
    sleep(3);
    //printf("Chamber %d: %d\n", roulette->chamber_no, roulette->chamber[roulette->chamber_no]);
    if (roulette->chamber[roulette->chamber_no]) {
        if (user == ENEMY) {
            printf("They died.\n");
        }
        else {
            printf("You died.\n");
        }
        return DEAD;
    }
    else {
        printf("Nothing happened.\n");
        roulette->chamber_no++;
        return ALIVE;
    }
}

struct shootoff *init_roulette(void) {
    struct shootoff *roulette = NULL;
    if ((roulette = malloc(sizeof(struct shootoff))) == NULL)
        printf("Error.");
    reset_chamber(roulette);
    roulette->your_spin = 1;
    roulette->their_spin = 1;
    roulette->first_shooter = rand() % 2;
    return roulette;
}

void reset_chamber(struct shootoff *roulette) {
    for (int i = 0; i < 6; i++) {
        roulette->chamber[i] = 0;
    }
    roulette->chamber[rand() % 6] = 1;
    roulette->chamber_no = 0;
}

void spin_chamber(struct shootoff *roulette, int user) {
    printf("%s spun the chamber!\n", (user == ENEMY) ? "They" : "You");
    sleep(3);
    if (user == ENEMY) {
        roulette->their_spin--;
        reset_chamber(roulette);
    }
    else {
        roulette->your_spin--;
        reset_chamber(roulette);
    }
}