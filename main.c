#define clrscr() printf("\e[1;1H\e[2J") // This is the print format for clearing a screen without other header files like curses.h
#include <stdio.h>
#include <time.h>
#include "screen.h"

int main(void) {
    srand(time(NULL));  // This sets the seed to a pseudo-random seed determined by the current time (works across all files)
    fate_initialize();  // This initializes the fate structure before the first Character is encountered
    //extra_store('+', ALERTED); 
    //fate_store('c', CONVICTED);
    Controller = initialize_controller(Controller); // This initializes the controller structure before input is accepted
    printf("This is a work of fiction. Behaviors/actions in this game may be offensive/controversial. Thank you for understanding.");
    //sleep(3);   // This pauses the program for 3 seconds before advancing to the title screen
    title_screen(); // This line starts the game up by popping open the title screen.
    return EXIT_SUCCESS;
}

/*
CURRENT GOALS:
P. Make sure that the stack isn't very full                                                         <<<
1. Finish the "Battle Royale Island" event.                                                         <<
    a. Designing and implementing POLITICIAN's drop item.                                           <<<
        - Pot of milk from the cosmic Ocean of Milk (from Vishnu's myth)
        - Use #1: RIVALED (versatility makes this item have less uses and an additional karma cost)
        - Use #2: SPIKES
    b. Verifying that moving through the LOBBY, JUNGLE, and CLIFF works (loops through them).       <<<
    c. Implement the prizes for winning the event.                                                  <<<
    d. Make it so that after claiming your prize, return to the initial MAP.                        <<<
    e. Add descriptors to the dialogue (what is going on/setting)                                   <<
1.5. Convenient fixes
    a. Implement PRIORITY for more convenient nested move_char()                                    <<<
    b. Easier/more efficient way to set tile symbols                                                <<
2. Design the next map.                                                                             <
    a. Tiles.                                                                                       <
    b. POLICE character.                                                                            <
        i. Storyline & dialogue                                                                     <
        ii. Behavior                                                                                <
        iii. Drop item                                                                              <<<
3. Rework the JUDGE character                                                                       <
4. Brainstorm the fates of each character depending on options chosen                               <
5. Implement ending screen (iterate through each fate)                                              <
*/