/* screen.c
Handles non-map screens.
*/
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "map.h"

#define clrscr() printf("\e[1;1H\e[2J")

/* screen functions */
void GetMonitorResolution(int *horizontal, int *vertical);
void title_screen(void);
void _title(void);
void controls(void);
void blade(void);
void quit(void);

/* Map, tile, and Character functions */

/* title_screen : commands the titlescreen and leads to next screens */
void title_screen(void) {
    int keystr;
    _title();
    set_state_vals(Controller, FALSE, TRUE, FALSE, FALSE, FALSE);
    while ((keystr = getaction(Controller)) != _EOF) {
        switch (keystr) {
            case Help:
                controls();
                _title();
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
                Map1 = initialize_map(MAP1Y, MAX_COLUMNS);  // Make map of Map1 size.
                Map1->player = initialize_player(); // Allocate memory for the player's Character.
                first_item = NULL;
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

/* title : print out the terminal output of the titlescreen */
void _title(void) {
    char resize_cmd[] = "MODE CON: COLS=", buffer[num_digits(WIDTH) + 1];
    strcat(resize_cmd, itoa(WIDTH, buffer, 10)); /* this block forces the terminal to resize */
    system(resize_cmd);

    char title_name[] = "Due Recompense";
    char start_prompt[5][35] = {
        "Input...",
        "\"e\" : Easy start",
        "\"n\" : Normal start",
        "\"h\" : Hard start",
        "\"?\" : Display/close instructions"
    };
    center_screen(WIDTH, "%s\n\n", title_name);
    blade();    /* this block outputs the text-formatted title screen */
    center_screen(WIDTH, "%s\n%s\n%s\n%s\n%s\n", start_prompt[0], start_prompt[1], start_prompt[2], start_prompt[3], start_prompt[4]);

    return;
}

/* blade : prints out a sword in the center of the terminal */
void blade(void) {
    char point[] = "^", tip[] = "/ \\", len[] = "| |", handle[] = "<===>", hilt[] = "[_]";
    center_screen(WIDTH, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n", point, tip, len, len, len, len, len, handle, hilt);
}

/* controls : clears the screen, replaces it with the list of controls */
void controls(void) {
    clrscr();
    int keystr;
    printf("%5s : Move up\n%5s : Move left\n%5s : Move right\n%5s : Move down/Skip dialogue\n%5s : Select/Advance\n%5s : Inventory\n"
           "%5s : Yes\n%5s : No\n%5s : Display/close instructions\n",
        "w", "a", "d", "s", "r", "i", "y", "n", "?");
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
    while ((keystr = getaction(Controller)) != _EOF) {
        switch (keystr) {
            case Help:
                return;
        }
    }
    quit();
}