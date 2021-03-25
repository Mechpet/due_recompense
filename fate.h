/* fate.h
Array of structures that describe what the player's outcome was */
#include <stdio.h>
#define NUM_ENEMIES 17  // Total # of implemented enemies in the game
#define NUM_PUPPETS 15
#define MAX_EXTRA 3 // Maximum extra attributes an enemy can have to describe their fate
#define FALSE 0 
#define TRUE 1

enum fate_numbers {
    NEVER_MET,  // Never talked to the enemy (i.e. never walked into the enemy's representation) 
    SPARED,     // Talked to the enemy but didn't enter a fight with them (i.e. walked into the enemy's representation and answered 'y')
    CONVICTED,  // Talked to the enemy and entered a fight with them and won (i.e. walked into the enemy's representation and answered 'n', player won) 
    VICTORIOUS,  // Talked to the enemy and entered a fight with them but lost (i.e. walked into the enemy's representation and answered 'n', player lost)
    THIRD_PARTY // Didn't talk to the enemy and the enemy lost against a third-party 
};

enum extra_numbers {NONE, MARKED, SURGERY, SYNTHOL, ALERTED, RETURNING, TRAILED, TREATED, GAMBLED, COMPETED, TRAINED, FOUGHT, HALTED, DUEL, WON};
/* 
Assigned extra attributes=
KID:        MARKED
DOC:        SURGERY or one of (SYNTHOL, NONE) and one of (ALERTED, NONE)
QUEEN:      RETURNING
ARSONIST:   TRAILED
LUNCHLADY:  TREATED
FRAUDSTER:  GAMBLED
ASSASSIN:   COMPETED and one of (TRAINED, FOUGHT, HALTED, DUEL, NONE)
*/

// Measure of how "evil" or "just" an enemy is; purposefully vague and subjective
enum karma_ids {
    ASSASSIN = 5,
    HACKER = 5,
    POLITICIAN = 6,
    LORD = 10,
    NAPPER = 14,
    FERAL = 16,
    DOC = 18,
    LUNCHLADY = 20,
    BAT = 21,
    STOWAWAY = 23,
    ARSONIST = 23,
    QUEEN = 29,
    ARTIST = 33,
    KID = 34,
    FRAUDSTER = 39,
    JUDGE = 101,
    COP = 4000
};

// This structure will determine an enemy's ending/fate at the end of the game
struct fates {
    char rep;   
    int extra[MAX_EXTRA];   // Extra attributes obtained in extra_numbers
    int fate;   // General fate decided by player in fate_numbers
} all_fates[NUM_ENEMIES];

void fate_initialize(void);
void fate_store(char rep, int their_fate);
int fate_returner(char rep);
int extra_returner(char rep, int extra_to_locate);
int total_fate(int fate_number);

/* fate_initialize : 
 * Arguments: void 
 * Implementation: Loop through all_fates[] and in each all_fates[index].extra[]
 * Purpose: initializes all fates in all_fates[NUM_ENEMIES] */
void fate_initialize(void) {
    all_fates[0].rep = 'c';
    all_fates[1].rep = '+';
    all_fates[2].rep = 'M';
    all_fates[3].rep = '%';
    all_fates[4].rep = '$';
    all_fates[5].rep = '*';
    all_fates[6].rep = 'L';
    all_fates[7].rep = 'C';
    all_fates[8].rep = '?';
    all_fates[9].rep = '@';
    all_fates[10].rep = '\\';
    all_fates[11].rep = '/';
    all_fates[12].rep = '#';
    all_fates[13].rep = '&';
    all_fates[14].rep = '!';
    all_fates[15].rep = 'V';
    all_fates[16].rep = 'X';
    
    int i, t;
    for (i = 0; i < NUM_ENEMIES; ++i) {
        all_fates[i].fate = NEVER_MET;  // To begin with, player has never met any enemy
        for (t = 0; t < MAX_EXTRA; ++t) {
            all_fates[i].extra[t] = NONE;   // As such, no extra attributes should have been obtained
        }
    }
}

/* fate_store : 
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to store a general fate
 *            their_fate is a general fate in fate_numbers to overwrite
 * Implementation: Loop through all_fates[] until matching representation
 * Purpose: assigns a fate number to the enemy with matching rep */
void fate_store(char rep, int their_fate) {
    int i;
    for (i = 0; i < NUM_ENEMIES && all_fates[i].rep != rep; ++i) {   // Iterate through fates until matching representation
        ;
    }
    all_fates[i].fate = their_fate;
}

/* extra_store : 
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to store an extra number
 *            their_extra is an extra number in extra_numbers to store
 * Implementation: Loop through all_fates[] until matching representation, loop through all extra numbers until an empty space is located
 * Purpose: adds an extra number to the enemy with matching rep */
void extra_store(char rep, int their_extra) {
    int i, t;
    for (i = 0; i < NUM_ENEMIES && all_fates[i].rep != rep; ++i) {   // Iterate through fates until matching representation
        ;
    }
    for (t = 0; all_fates[i].extra[t]; ++t) {   // Iterate through extra numbers 
        ;
    }
    if (t < MAX_EXTRA) { // If there is an empty extra number (== NONE), function decides to store the extra number argument
        all_fates[i].extra[t] = their_extra;
    }
}

/* fate_returner : 
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to select 
 * Implementation: Loop through all_fates[] until matching representation, return the fate of that one
 * Purpose: returns the fate of the enemy with matching rep */
int fate_returner(char rep) {
    int i;
    for (i = 0; i < NUM_ENEMIES && all_fates[i].rep != rep; ++i) {
        ;
    }
    return all_fates[i].fate;   // Should always return a legal and important value
}

/* extra_returner : 
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to select
 *            extra_number is the extra number to check for in a fate of an enemy
 * Implementation: Loop through all_fates[] until matching representation, loop through the extra numbers and if the argument is/isn't found, return TRUE/FALSE
 * Purpose: returns whether or not the enemy with matching rep has the specified extra number assigned */
int extra_returner(char rep, int extra_number) {
    int i, t;
    for (i = 0; i < NUM_ENEMIES && all_fates[i].rep != rep; ++i) {
        ;
    }
    for (t = 0; t < MAX_EXTRA; ++t) {
        if (all_fates[i].extra[t] == extra_number) {
            return TRUE;
        }
    }
    return FALSE;
}

/* total_fate : 
 * Arguments: fate_number is the number in fate_numbers to tally up
 * Implementation: Loop through all_fates[] and if matching fate, increment the tally
 * Purpose: returns the number of enemies with the fate that matches the fate_number */
int total_fate(int fate_number) {
    int i, num_with_fate_no = 0;
    for (i = 0; i < NUM_ENEMIES; ++i) {
        if (all_fates[i].fate == fate_number) {
            ++num_with_fate_no;
        }
    }
    return num_with_fate_no;
}