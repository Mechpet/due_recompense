/* fate.h
Functions and structures that help describe what each character's fate was
*/

#include <stdio.h>
#include "general.h"

#define NUM_CHARACTERS 18  // Total # of implemented characters in the game
#define MAX_EXTRA 3 // Maximum extra attributes an enemy can have to describe their fate

/* Describes the general fate/outcome of a character */
enum fate_numbers {
    NEVER_MET,  // Never talked to the enemy (i.e. never walked into the enemy's representation) 
    SPARED,     // Talked to the enemy but didn't enter a fight with them (i.e. walked into the enemy's representation and answered 'y')
    CONVICTED,  // Talked to the enemy and entered a fight with them and won (i.e. walked into the enemy's representation and answered 'n', player won) 
    VICTORIOUS,  // Talked to the enemy and entered a fight with them but lost (i.e. walked into the enemy's representation and answered 'n', player lost)
    THIRD_PARTY // Didn't talk to the enemy and the enemy lost against a third-party (i.e. didn't walk into the enemy's representation and the enemy lost to another enemy)
};

/* Describes an extra variable (character-exclusive, in fact) in the fate/outcome of a character */
enum extra_numbers {
    NONE,       // All characters
    MARKED,     // KID
    SURGERY,    // DOC
    ALERTED,    // DOC
    RETURNING,  // QUEEN
    TRAILED,    // ARSONIST
    TREATED,    // LUNCHLADY 
    HUNTED,     // LUNCHLADY | BAT
    KILLED,     // FRAUDSTER | YOU
    COMPETED,   // ASSASSIN
    TRAINED,    // ASSASSIN
    FOUGHT,     // ASSASSIN
    HALTED,     // ASSASSIN
    DUEL,       // ASSASSIN
    RICH,       // ASSASSIN
    WON,        // ASSASSIN
    NOT_ABSORBED    // COP
};

// Measure of how "evil" or "just" an enemy is in this fiction 
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
    COP = 400
};

// This structure will determine a character's ending/fate at the end of the game
struct fates {
    char rep;   // 
    int extra[MAX_EXTRA];   // Extra attributes obtained in extra_numbers
    int fate;   // General fate decided by player in fate_numbers
} all_fates[NUM_CHARACTERS];

void fate_initialize(void);
void fate_store(char rep, int their_fate);
int fate_returner(char rep);
int extra_returner(char rep, int extra_to_locate);
int total_fate(int fate_number);
unsigned int is_alive(char rep);
unsigned int is_active(char rep);
unsigned int is_arrested(char rep);

/* fate_initialize : 
 * Arguments:  
 * Implementation: Nested iteration
 * Purpose: Initializes all fates in all_fates[NUM_CHARACTERS] */
void fate_initialize(void) {
    all_fates[0].rep = 'c';
    all_fates[1].rep = '+';
    all_fates[2].rep = 'M';
    all_fates[3].rep = '%';
    all_fates[4].rep = '$';
    all_fates[5].rep = '*';
    all_fates[6].rep = 'L';
    all_fates[7].rep = '?';
    all_fates[8].rep = 'C';
    all_fates[9].rep = '@';
    all_fates[10].rep = '\\';
    all_fates[11].rep = '/';
    all_fates[12].rep = '!';
    all_fates[13].rep = '#';
    all_fates[14].rep = '&';
    all_fates[15].rep = 'V';
    all_fates[16].rep = 'X';
    all_fates[17].rep = '^';
    
    for (int i = 0; i < NUM_CHARACTERS; ++i) {
        all_fates[i].fate = NEVER_MET;  // To begin with, player has never met any enemy
        for (int t = 0; t < MAX_EXTRA; ++t) {
            all_fates[i].extra[t] = NONE;   // As such, no extra attributes should have been obtained
        }
    }
}

/* fate_store : 
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to store a general fate
 *            their_fate is a general fate in fate_numbers to overwrite
 * Implementation: Iteration 
 * Purpose: Assigns a fate number to the enemy with matching rep */
void fate_store(char rep, int their_fate) {
    int i;
    for (i = 0; i < NUM_CHARACTERS && all_fates[i].rep != rep; ++i) {   // Iterate through fates until matching representation
        ;
    }
    all_fates[i].fate = their_fate;
}

/* extra_store : 
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to store an extra number
 *            their_extra is an extra number in extra_numbers to store
 * Implementation: Nested iteration
 * Purpose: adds an extra number to the enemy with matching rep */
void extra_store(char rep, int their_extra) {
    int i, t;
    for (i = 0; i < NUM_CHARACTERS && all_fates[i].rep != rep; ++i) {   // Iterate through fates until matching representation
        ;
    }
    for (t = 0; t < MAX_EXTRA && all_fates[i].extra[t] && all_fates[i].extra[t] != their_extra; ++t) {   // Iterate through extra numbers until a placeholder is found
        ;
    }
    if (all_fates[i].extra[t] == their_extra) { // In case an extra is already stored, return immediately
        return;
    }
    else if (t < MAX_EXTRA) { // If there is an empty extra number (== NONE), function decides to store the extra number argument
        all_fates[i].extra[t] = their_extra;
    }
}

/* fate_returner : 
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to select 
 * Implementation: Iteration
 * Purpose: Returns the stored fate of the enemy with matching rep */
int fate_returner(char rep) {
    int i;
    for (i = 0; i < NUM_CHARACTERS && all_fates[i].rep != rep; ++i) {
        ;
    }
    return all_fates[i].fate;   // Should always return a legal and important value
}

/* extra_returner : 
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to select
 *            extra_number is the extra number to check for in a fate of an enemy
 * Implementation: Nested iteration
 * Purpose: Returns whether or not the enemy with matching rep has the specified extra number assigned */
int extra_returner(char rep, int extra_number) {
    int i, t;
    for (i = 0; i < NUM_CHARACTERS && all_fates[i].rep != rep; ++i) {
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
 * Implementation: Iteration 
 * Purpose: Returns the number of enemies with the fate that matches the fate_number */
int total_fate(int fate_number) {
    int i, num_with_fate_no = 0;
    for (i = 0; i < NUM_CHARACTERS; ++i) {
        if (all_fates[i].fate == fate_number) {
            ++num_with_fate_no;
        }
    }
    return num_with_fate_no;
}

/* is_active :
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to select
 * Implementation: Calling of helper function
 * Purpose: Return whether a character has a fate of either NEVER_MET or SPARED (frequently used in determining an ending) */
unsigned int is_active(char rep) {
    if (fate_returner(rep) == NEVER_MET || fate_returner(rep) == SPARED) {
        return TRUE;
    }
    return FALSE;
}

/* is_alive :
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to select
 * Implementation: Calling of helper function
 * Purpose: Return whether a character has a fate of NEVER_MET, SPARED, or VICTORIOUS (sometimes used in determining an ending) */
unsigned int is_alive(char rep) {
    if (is_active(rep) || fate_returner(rep) == VICTORIOUS) {
        return TRUE;
    }
    return FALSE;
}

/* is_arrested :
 * Arguments: rep is the representation of the chosen enemy so that the function knows which index in all_fates[] to select
 * Implementation: Calling of helper function
 * Purpose: Return whether a character has a fate of either CONVICTED or THIRD_PARTY (frequently used in determining an ending) */
unsigned int is_arrested(char rep) {
    if (fate_returner(rep) == CONVICTED || fate_returner(rep) == THIRD_PARTY) {
        return TRUE;
    }
    return FALSE;
}