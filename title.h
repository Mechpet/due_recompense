/* title.h
Functions that handle the main title screens
*/
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "map.h"

/* TITLE = functions that handle the title screens */
void title_screen(void);
void print_title(void);
void controls(void);
void options(void);
void blade(void);
void quit(void);

/* title_screen : 
 * Arguments:
 * Implementation: Iterate until input is accepted
 * Purpose: Commands the main title screen and call functions to the next screens */
void title_screen(void) {
    int keystr;
    print_title();
    set_state_vals(Controller, FALSE, TRUE, FALSE, FALSE, FALSE);
    while ((keystr = getaction(Controller)) != _EOF) {
        switch (keystr) {
            case Help:
                controls();
                print_title();
                break;
            case Options:
                options();
                print_title();
                break;
            case Easy: case Normal: case Hard:
                if (keystr == Easy) {
                    MULTIPLIER = 0.66;
                }
                else if (keystr == Normal) {
                    MULTIPLIER = 1.0;
                }
                else {
                    MULTIPLIER = 1.33;
                }
                determine_stats();
                Map1 = initialize_map(MAP1Y, MAX_COLUMNS);  // Initialize Map1 (static map design)
                Map1->player = initialize_player(); // Allocate memory for the user
                add_item(&first_item, DAMAGE);
                add_item(&first_item, COINS);
                add_item(&first_enemy_item, DAMAGE);
                COP_tracker = initialize_COP();
                move_char(Map1, Map1->all_tiles[MAP1Y - 2][MAX_COLUMNS / 2]); // Sets player to the middle bottom tile.
                allocate_enemy(Map1, Map1->all_tiles[KID_y][KID_x], "Cue", KID_hp, KID_hp, KID_attack, KID, 'c', RUSH);   // Allocates memory for the 1st "enemy".
                set_state_vals(Controller, FALSE, FALSE, TRUE, FALSE, FALSE);
                fmt_map(Map1);
                break;
        }
    }
    quit();
}

/* title : 
 * Arguments:
 * Implementation: Automatic strings are initialized
 * Purpose: Print out the static title screen */
void print_title(void) {
    char resize_cmd[] = "MODE CON: COLS=", buffer[num_digits(WIDTH) + 1];
    strcat(resize_cmd, itoa(WIDTH, buffer, 10)); /* this block forces the terminal to resize */
    system(resize_cmd);

    char title_name[] = "Due Recompense";
    char start_prompt[6][35] = {
        "Input...",
        "\"e\" : Easy start",
        "\"n\" : Normal start",
        "\"h\" : Hard start",
        "\"?\" : Display/close instructions",
        "\"o\" : Change options"
    };
    center_screen(WIDTH, "%s\n\n", title_name);
    blade();    /* this block outputs the text-formatted title screen */
    center_screen(WIDTH, "%s\n%s\n%s\n%s\n%s\n%s\n", start_prompt[0], start_prompt[1], start_prompt[2], start_prompt[3], start_prompt[4], start_prompt[5]);

    return;
}

/* blade : 
 * Arguments:
 * Implementation: Automatic strings are initialized 
 * Purpose: Prints out a sword in the center of the terminal */
void blade(void) {
    char point[] = "^", tip[] = "/ \\", len[] = "| |", handle[] = "<===>", hilt[] = "[_]";
    center_screen(WIDTH, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", point, tip, len, len, len, len, len, handle, hilt);
}

/* controls : 
 * Arguments:
 * Implementation: Automatic strings are initialized; recursively call function until a specific key is pressed
 * Purpose: Replaces title screen with the list of controls and explanations of mechanics */
void controls(void) {
    clrscr();
    int keystr;
    printf("%5s : Move up\n%5s : Move left\n%5s : Move right\n%5s : Move down/Skip dialogue\n%5s : Select/Advance\n%5s : Inventory\n"
           "%5s : Yes\n%5s : No\n%5s : Display/close instructions\n",
        "w", "a", "s", "d", "r", "i", "y", "n", "?");
    printf("Attributes:\nHealth = Measure of how close to death a character is; the lower, the worse (restored per exiting of a map)\n"
           "Attack = Measure of how powerful a character is; the higher, the more threatening\n"
           "Karma  = Measure of how good the character is; can be exchanged to use items as the player (restored per exiting of a map)\n"
           "Sword  = Charged up when there are three ticks; charged up by using normal attacks; once filled, can access sword skills\n"
           "Mechanics:\nBuffs are temporary boosts to a character's abilities. Debuffs are temporary drops to a character's abilities.\n"
           "Flat damage is damage that can't be modified through buffs or debuffs.\n"
           "Non-flat damage can be modified by buffs and debuffs.\n"
           "The *type* of damage for each attack will be specified in the game.\n"
           "Buffs and debuffs that are preceded by a 's:' are \"sword buffs/debuffs\" that are treated differently from \"normal buffs/debuffs\".\n"
           "A character can have a buff and sword buff active at the same time.\n"
           "An 'S' symbol denotes a shop where coins can be exchanged for rewards.\n");
    if ((keystr = getaction(Controller)) != _EOF) {
        switch (keystr) {
            case Help:
                return;
                break;
            default:
                controls();
                return;
                break;
        }
    }
    quit();
}

/* options : 
 * Arguments:
 * Implementation: Automatic strings are initialized; recursively call function until a specific key is pressed
 * Purpose: Replaces title screen with the list of options (i.e. "fading" text speed, where messages should be centered) */
void options(void) {
    clrscr();
    int keystr;
    printf("\"w\" : Change display width\n"
           "\"s\" : Change uncontrolled message speed\n"
           "\"o\" : Close options\n");
    if ((keystr = getaction(Controller)) != _EOF) {
        switch (keystr) {
            case Options:
                return;
                break;
            case Speed:
                printf("Current speed = %d\n", WAIT);
                printf("Input the desired speed:\n");
                while ((WAIT = get_pos_int()) == _EOF) {
                    ;
                }
                printf("Example message sequence:\n");
                printf("WOWZER!\n");
                sleep(WAIT);
                printf("WOWZER indeed!\n");
                sleep(WAIT);
                options();
                return;
                break;
            case Display:
                printf("Current width = %d\n", WIDTH);
                printf("Input the desired width:\n");
                while ((WIDTH = get_pos_int()) == _EOF) {
                    ;
                }
                options();
                return;
                break;
            default:
                options();
                return;
                break;
        }
    }
    quit();
}