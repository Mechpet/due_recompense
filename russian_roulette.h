/* russian_roulette.h
Contains all the functions needed for a Russian Roulette shootoff with a computer player 
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "general.h"

/* Whose turn it is */
enum shooters {
    YOU, ENEMY
};

/* The result of a player */
enum turn_result {
    ALIVE, DEAD
};

/* Describes a game of Russian Roulette where each player has one chance to spin the revolver's chamber */
struct shootoff {
    unsigned int chamber[6];    // Describes the sequence of the results of shooting (kth shot will correspond to chamber[k])
    unsigned int chamber_no;    // Describes the current shot number 
    unsigned int your_spin; // The remaining amount of chamber spins you have
    unsigned int their_spin;    // The remaining amount of chamber spins the opponent has
    unsigned int first_shooter; // Identifies the player with the first turn
} *roulette;


struct shootoff *init_roulette(void);
void reset_chamber(struct shootoff *roulette);
int duel_enemy(void);
int their_turn_RR(struct shootoff *roulette);
int your_turn_RR(struct shootoff *roulette);
int shoot_gun(struct shootoff *roulette, int user);
void spin_chamber(struct shootoff *roulette, int user);

/* duel_enemy 
 * Arguments:
 * Implementation: While loop that is terminated when a condition is met (i.e. a player loses/wins)
 * Purpose: Run an instance of this modified Russian Roulette with a computer player */
int duel_enemy(void) {
    roulette = init_roulette();
    while (TRUE) {
        if (roulette->first_shooter == ENEMY) {
            if (their_turn_RR(roulette)) {
                free(roulette);
                roulette = NULL;
                return ALIVE;
            }
            if (your_turn_RR(roulette)) {
                free(roulette);
                roulette = NULL;
                return DEAD;
            }
        }
        else {
            if (your_turn_RR(roulette)) {
                free(roulette);
                roulette = NULL;
                return DEAD;
            }
            if (their_turn_RR(roulette)) {
                free(roulette);
                roulette = NULL;
                return ALIVE;
            }
        }
    }
}

/* their_turn_RR : 
 * Arguments: roulette is the pointer to the current shootoff 
 * Implementation: Spin based on remaining number of chambers and spins (less chamber numbers means more urgent to spin); if a spin is not chosen, shoot the gun
 * Purpose: Plays the opponent's turn (has somewhat pseudo-random behavior) */
int their_turn_RR(struct shootoff *roulette) {
    if (roulette->their_spin) { // Consider the option of spinning chamber
        if ((rand() % 5) < roulette->chamber_no) { // As more bullets are shot w/o spinning, increases chance of spinning
            spin_chamber(roulette, ENEMY);
            return their_turn_RR(roulette);
        }
    }
    return shoot_gun(roulette, ENEMY);
}

/* your_turn_RR : 
 * Arguments: roulette is the pointer to the current shootoff
 * Implementation: Iteratively receive user input until valid input
 * Purpose: Plays the user's turn (has controlled behavior) */
int your_turn_RR(struct shootoff *roulette) {
    int c;
    printf("Press 'k' to shoot yourself, 's' to spin the chamber (%d remaining).\n>", roulette->your_spin);
    while ((c = getchar()) != EOF && c != 'k' && ((!roulette->your_spin && c == 's') || c != 's')) {
        ;
    }
    switch (c) {
        case 'k':
            return shoot_gun(roulette, YOU);
            break;
        case 's':
            spin_chamber(roulette, YOU);
            return your_turn_RR(roulette);
            break;
    }
    return shoot_gun(roulette, YOU);;
}

/* shoot_gun : 
 * Arguments: roulette is the pointer to the current shootoff to be updated
              user identifies the player that is shooting
 * Implementation: Check whether the gun is loaded (means there is a non-zero value at the current chamber number)
 * Purpose: Shoot the gun; consequently, the shootoff must be updated - results in either a loss or the continuation of the shootoff */
int shoot_gun(struct shootoff *roulette, int user) {
    printf("%s pulled the trigger...\n", (user == ENEMY) ? "They" : "You");
    sleep(3 * WAIT);
    if (roulette->chamber[roulette->chamber_no]) {
        play_sound_fx("Music\\Revolver_shot.wav");

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

/* init_roulette :
 * Arguments:
 * Implementation: Dynamic allocation; randomizes the chamber 
 * Purpose: Initialize the structure shootoff for a round of Russian Roulette */
struct shootoff *init_roulette(void) {
    struct shootoff *roulette = NULL;
    if ((roulette = malloc(sizeof(struct shootoff))) == NULL) {
        printf("Error.");
    }
    reset_chamber(roulette);
    roulette->your_spin = 1;
    roulette->their_spin = 1;
    roulette->first_shooter = rand() % 2;
    return roulette;
}

/* reset_chamber : 
 * Arguments: roulette is the pointer to the current shootoff
 * Implementation: Zero the shootoff's chamber array; randomly pick a chamber in the range [0, 5] to be non-zero
 * Purpose: Resets a shootoff's chamber; randomizes the location of the loaded bullet and resets the current chamber number */
void reset_chamber(struct shootoff *roulette) {
    for (int i = 0; i < 6; i++) {
        roulette->chamber[i] = 0;
    }
    roulette->chamber[rand() % 6] = 1;
    roulette->chamber_no = 0;
}

/* spin_chamber : 
 * Arguments: roulette is the pointer to the current shootoff
              user identifies the player that is spinning the chamber
 * Implementation: Update the number of spins remaining corresponding to the passed user; then, randomize and reset the chamber
 * Purpose: Reset a shootoff's chamber while adjusting the number of spins */
void spin_chamber(struct shootoff *roulette, int user) {
    play_sound_fx("Music\\Spin.wav");
    printf("%s spun the chamber!\n", (user == ENEMY) ? "They" : "You");
    sleep(3 * WAIT);
    if (user == ENEMY) {
        roulette->their_spin--;
        reset_chamber(roulette);
    }
    else {
        roulette->your_spin--;
        reset_chamber(roulette);
    }
}