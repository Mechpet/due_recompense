/* map.h
Functions that primarily implement the map.
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include "inventory.h"
#include "character.h"

#define DEBUG 0

#define NEXT_BONUS 6

#define MAX_COLUMNS 13

// Map height constants
#define MAP1Y 7
#define MAP2Y 10
#define MAP3Y 9
#define MAP4Y 10
#define MAP5Y 8
#define MAP6Y 11
#define MAP7Y 13
#define LOBBY_Y 12
#define CLIFF_Y 12
#define JUNGLE_Y 12
#define MAP8Y 8
#define MAP9Y 10

// Character spawn position constants
#define KID_x MAX_COLUMNS / 2
#define KID_y MAP1Y / 2
#define DOC_x 4
#define DOC_y 3
#define QUEEN_x 9
#define QUEEN_y 6
#define LORD_x MAX_COLUMNS / 2
#define LORD_y MAP3Y / 2
#define STOWAWAY_x MAX_COLUMNS / 2
#define STOWAWAY_y MAP3Y / 2 - 1
#define ARSONIST_x 10
#define ARSONIST_y MAP4Y - 2
#define LUNCHLADY_x MAX_COLUMNS - 2
#define LUNCHLADY_y 5
#define FRAUDSTER_x 10
#define FRAUDSTER_y 3
#define NAPPER_x 3
#define NAPPER_y 5
#define BAT_x 4
#define BAT_y 5
#define ASSASSIN_x 1
#define ASSASSIN_y 6
#define POLITICIAN_x 1
#define POLITICIAN_y 4
#define FERAL_x MAX_COLUMNS / 2
#define FERAL_y JUNGLE_Y / 2
#define ARTIST_x 10
#define ARTIST_y 5
#define HACKER_x MAX_COLUMNS / 2
#define HACKER_y LOBBY_Y - 2
#define COP_x MAX_COLUMNS / 2
#define COP_y 1

#define MAX_ENEMY_INVENTORY 15

struct Map {
    struct tile *(*all_tiles)[MAX_COLUMNS];
    int rows;
    int columns;  
    struct Enforcer *player;
    struct Character *enemy[MAX_ENEMIES];
} *Map1 = NULL, *Map2 = NULL, *Map3 = NULL, *Map4 = NULL, *Map5 = NULL, *Map6 = NULL, *Map7 = NULL, *Lobby = NULL, *Cliff = NULL, *Jungle = NULL, *Map8 = NULL;

struct Map *initialize_map(int rows_of_tiles, int columns_of_tiles);
struct Map *load_map(struct Map *old_map);
void swap_enemy(struct Map *map_ptr, char rep1, char rep2);
void free_enemy(struct Map *old_map, struct Character *enemy);
void free_enemies(struct Map *old_map);
void free_map(struct Map *old_map);
void allocate_enemy(struct Map *Map_n_ptr, struct tile *tile_ptr, char *name, int hp, int max_hp, unsigned int atk, int karma_id, char representation, unsigned int sword_buff);
struct tile *identify_tile(struct Map *curr_map, unsigned int ver, unsigned int hor);
void set_common_symbols(struct Map *curr_map, int *indices, int num_length, char common_symbol, int set_traversable, int set_entry, int set_occupied);
struct tile *decline_event(struct Map *curr_map, char rep);
struct tile *trigger_battle_event(struct Map *curr_map, char rep);
void item_turn(struct item *item_used, struct Enforcer *user_ptr, struct Character *enemy_ptr, int turn_no);
struct Map *find_next_map(struct Map *Map_n_ptr);
void move_char(struct Map *Map_n_ptr, struct tile *next_tile_to_move_to);
int process_movement(struct Map *old_map, int key);
void fmt_map(struct Map *map_ptr);
void trigger_battle(struct Map *map_ptr, struct Character *enemy_ptr, int turn_no);
struct Map *initialize_Map2(void);
struct Map *initialize_Map3(void);
struct Map *initialize_Map4(void);
struct Map *initialize_Map5(void);
struct Map *initialize_Map6(void);
struct Map *initialize_Map7(void);
struct Map *initialize_lobby(void);
struct Map *initialize_cliff(void);
struct Map *initialize_jungle(void);
struct Map *initialize_Map8(void);

void update_cop(struct Enforcer *cop_ptr, struct Map *old_map);
int go_to_tile(int curr_x, int curr_y, struct Map *map, int desired_x, int desired_y);
void reset_prop(struct Map *map);
int return_item_attempt(struct item *item_used, struct Enforcer *user_ptr);

void trigger_battle(struct Map *map_ptr, struct Character *enemy_ptr, int turn_number);

void alignment_meter(int alignment);
struct item *inventory_screen(struct item *first_item_slot, struct Enforcer *user_ptr, struct Character *enemy_ptr, int turn_no);
int sword_screen(struct Map *map_ptr, struct Character *enemy_ptr, int turn_no);

enum sword_ids {ATTACK_ID = 1, SIGNATURE_ID, DEFENSE_ID};

/* move_char : moves user to a new position, triggering encounters with enemies */

struct tile *decline_event(struct Map *curr_map, char enemy_rep) {
    int i, t, *common_tiles = NULL;
    struct tile *next_tile = NULL;
    switch (enemy_rep) { // Below is for events that trigger after the decline scene
        case '+': 
        // 1. Accepted surgery from DOC results in getting kicked out of the house, which is now locked
            if (extra_returner('+', SURGERY)) {
                next_tile = curr_map->all_tiles[3][2];    /* move player to tile outside the barbershop*/
                for (i = 6; i < 8; ++i) {   /* fill in the door, which is impassable*/
                    curr_map->all_tiles[5][i]->symbol = '-';
                    set_prop_vals(curr_map->all_tiles[5][i], FALSE, FALSE, FALSE);
                }
            }
            break;
        // 2. After chatting with LUNCHLADY with ARSONIST chasing you, the homeless shelter will burn down
        case 'L': 
            if (extra_returner('*', TRAILED)) {
                for (t = 6; t < 9; ++t) {   /* get rid of the integrity of the building (remove lines) */
                    curr_map->all_tiles[1][t]->symbol = ' ';
                    set_prop_vals(curr_map->all_tiles[1][t], TRUE, FALSE, FALSE);
                }
                for (i = 3; i < 6; ++i) {
                    curr_map->all_tiles[i][10]->symbol = ' ';
                    set_prop_vals(curr_map->all_tiles[i][10], TRUE, FALSE, FALSE);
                }
                allocate_enemy(curr_map, curr_map->all_tiles[4][MAX_COLUMNS - 4], "Inigo", ARSONIST_hp, ARSONIST_hp, ARSONIST_attack, ARSONIST, '*', FIRE);
                next_tile = curr_map->all_tiles[4][MAX_COLUMNS - 4];
            }
            else {
                next_tile = curr_map->all_tiles[4][3];
                curr_map->all_tiles[4][4]->symbol = '|';
                set_prop_vals(curr_map->all_tiles[4][4], FALSE, FALSE, FALSE);
            }
            break;
        // 3. After chatting with BAT with ARSONIST chasing you, the van will burn down
        case '@':
            if (extra_returner('*', TRAILED) && fate_returner('*') != CONVICTED) {                      
                curr_map->all_tiles[2][4]->symbol = ' ';
                set_prop_vals(curr_map->all_tiles[2][4], TRUE, FALSE, FALSE);
                allocate_enemy(curr_map, curr_map->all_tiles[5][4], "Inigo", ARSONIST_hp, ARSONIST_hp, ARSONIST_attack, ARSONIST, '*', FIRE);
                next_tile = curr_map->all_tiles[5][4];
            }
            else {
                if ((common_tiles = malloc(27 * sizeof(*common_tiles))) == NULL) {
                    fprintf(stderr, "FATAL: No memory for common tile symbols.\n"); // Row_number * Number_col + Col_number (11,13)
                }
                common_tiles[0] = 42, common_tiles[1] = 55, common_tiles[2] = 68, common_tiles[3] = 50, common_tiles[4] = 63, common_tiles[5] = 76, common_tiles[6] = 30, common_tiles[7] = 31, common_tiles[8] = 32, common_tiles[9] = 33, common_tiles[10] = 34, common_tiles[11] = 35, common_tiles[12] = 36, common_tiles[13] = 69, common_tiles[14] = 70, common_tiles[15] = 71, common_tiles[16] = 72, common_tiles[17] = 73, common_tiles[18] = 74, common_tiles[19] = 75, common_tiles[20] = 44, common_tiles[21] = 57, common_tiles[22] = 76, common_tiles[23] = 45, common_tiles[24] = 58, common_tiles[25] = 46, common_tiles[26] = 59;
                set_common_symbols(curr_map, common_tiles, 27, ' ', TRUE, FALSE, FALSE);
                free(common_tiles);
            }
            break;
        // 4. After winning the game and choosing a non-battle wish, load the next map and determine rewards
        case '\\':  
            if (extra_returner('\\', COMPETED)) { 
                if (extra_returner('\\', TRAINED)) {
                    printf("Matilda trained you to be an expert fighter.\n");
                    center_screen(WIDTH, "%s\n", "Your attack power increased by 2.");
                    center_screen(WIDTH, "%s\n", "Your maximum health increased by 3.");
                    sleep(3);
                    curr_map->player->attr->attack += 2;
                    curr_map->player->attr->max_health += 3;
                    curr_map->player->attr->health += 3;
                }
                next_tile = curr_map->all_tiles[0][MAX_COLUMNS / 2];
            }
            break;
        case '/':
            // certain scenarios for HACKER results in the previous lines of dialogue appearing on screen, so force a clrscr() call
            clrscr();
            break;
    }
    return next_tile;
}

/* _move_char
 * Moves the user to the next tile in the current map; moving can cause interaction with a Character, which is specified by intro(), trigger_battle(), decline() 
 *
 * _Arguments
 * curr_map is a pointer to the current struct Map, which must also contains the next tile
 * next_tile is a pointer to the next struct tile, which is to be the tile that the user must move to, which is specified by process_movement()
 * 
 * _Variables
 * trigger = Binary variable (takes values 0 or 1); an ON trigger means that further moving is to be invoked, an OFF trigger means that moving is completed */
void move_char(struct Map *curr_map, struct tile *next_tile) {
    int trigger = TRUE;
    char enemy_rep;
    if (curr_map->player->attr->occupied != NULL) { // Free up occupying space for the previous tile 
        curr_map->player->attr->occupied->prop.is_occupied = FALSE;
    }
    while (trigger) {   // While loop for moving of user (break when done; break when trigger is OFF)
        // Case where the next tile is already occupied by an enemy
        if (next_tile->prop.is_occupied) {
            struct Character *enemy_encountered = identify_rep(next_tile, curr_map->player->attr, curr_map->enemy);
            enemy_rep = enemy_encountered->rep; // Store the *rep* member before free is called to the Character
            set_state_vals(Controller, FALSE, FALSE, FALSE, TRUE, FALSE);

            // Path where the user fights the enemy
            if (intro(enemy_encountered, curr_map->player) == FALSE) {  // Display the enemy's intro and receives input from the user (to battle or not?)
                trigger_battle(curr_map, enemy_encountered, 1);
                set_state_vals(Controller, FALSE, FALSE, TRUE, FALSE, TRUE);
                enemy_encountered->occupied->prop.is_occupied = FALSE;  // Set the enemy's tile, which is *next_tile*, to unoccupied so the while loop breaks if no more events result in a movement
                free_enemy(curr_map, enemy_encountered);    // Free memory for the enemy, possibly make space for other characters in events   
                if (trigger_battle_event(curr_map, enemy_rep)) {    // Based on the game's state, search and execute possible events that happen after a battle is won
                    next_tile = trigger_battle_event(curr_map, enemy_rep);  // If there is an event triggered && the event requires further moving of the user, next_tile will be updated && the while loop continues
                }
                else {  // The case where there is no event triggered or the triggered event requires no further moving of the user 
                    trigger = FALSE;
                }
            }
            // Path where the user spares the enemy
            else {
                decline(enemy_encountered, curr_map->player);
                enemy_encountered->occupied->prop.is_occupied = FALSE;
                free_enemy(curr_map, enemy_encountered);   
                if (decline_event(curr_map, enemy_rep)) {
                    next_tile = decline_event(curr_map, enemy_rep);
                }
                else {
                    trigger = FALSE;
                }
            }
        }
        // Case where the next tile is not occupied by an enemy 
        else {
            trigger = FALSE;    // Break out of the while loop
        }
    }
    // Update the tile properties so that the user now occupies the next tile
    curr_map->player->attr->occupied = next_tile; 
    next_tile->prop.is_occupied = TRUE; 
    if (DEBUG) {
        printf("POP move_char\n");
        sleep(1);
    }
    return;
}

/* _process_movement
 * Translates the directional key input to movement of the user
 *
 * _Arguments
 * curr_map is a pointer to the current struct Map, which should be the final map after movement is processed 
 * key is the user input received 
 * 
 * _Variables
 * to_tile is a pointer to the tile the user wants to move to
 * curr_row is the y-position of the user
 * curr_column is the x-position of the user
 * */
int process_movement(struct Map *curr_map, int key) {
    struct tile *to_tile = NULL;
    int curr_row = curr_map->player->attr->occupied->y;
    int curr_column = curr_map->player->attr->occupied->x;
    switch (key) {
        case Up:
            to_tile = identify_tile(curr_map, curr_row - 1, curr_column);
            break;
        case Down:
            to_tile = identify_tile(curr_map, curr_row + 1, curr_column);
            break;
        case Left:
            to_tile = identify_tile(curr_map, curr_row, curr_column - 1);
            break;
        case Right:
            to_tile = identify_tile(curr_map, curr_row, curr_column + 1);
            break;
        default:
            return FALSE;
    }
    if (to_tile->prop.is_traversable) {
        move_char(curr_map, to_tile);
        if (karma_decay(curr_map->player->attr) <= 0) { /* trigger intro w/ JUDGE when karma -> 0 (quota not met) */
            set_state_vals(Controller, FALSE, FALSE, FALSE, TRUE, FALSE);
            allocate_enemy(curr_map, curr_map->all_tiles[0][0], "Judge", JUDGE_hp, JUDGE_hp, JUDGE_attack, JUDGE, 'X', DUE_RECOMPENSE);
            if (!intro(identify_rep(curr_map->all_tiles[0][0], curr_map->player->attr, curr_map->enemy), curr_map->player)) {
                trigger_battle(curr_map, identify_rep(curr_map->all_tiles[0][0], curr_map->player->attr, curr_map->enemy), 1);
            }
        }
        if (DEBUG) {
            printf("POP process_movement\n");
            sleep(1);
        }
        return TRUE;
    }
    if (DEBUG) {
        printf("POP process_movement\n");
        sleep(1);
    }
    return FALSE;
}

/* initialize_map : creates a new Map with # rows, #columns --- follows general structure of a box */
struct Map *initialize_map(int rows, int columns) {
    int i, t;
    struct Map *empty_map = NULL;
    if (!(empty_map = malloc(sizeof(*empty_map)))) {
        fprintf(stderr, "FATAL: No more memory to allocate Map.\n"); // Structure.
    }
    empty_map->all_tiles = malloc(sizeof(struct tile *[rows][columns]));
    if (!empty_map->all_tiles) {
        fprintf(stderr, "FATAL: No more memory to allocate 2-D array of pointers to tiles.\n");
    }
    for (i = 0; i < rows; ++i) {
        for (t = 0; t < columns; ++t) {
            if ((empty_map->all_tiles[i][t] = malloc(sizeof(*empty_map->all_tiles[i][t]))) == NULL) {
                fprintf(stderr, "FATAL: No more memory to allocate tiles.\n");    // Tiles.
            }
        }
    }
    for (i = 0; i < rows; ++i) {
        for (t = 0; t < columns; ++t) {
            if (!i) {
                empty_map->all_tiles[i][t]->symbol = '_';
                set_prop_vals(empty_map->all_tiles[i][t], TRUE, TRUE, FALSE);
            }
            else if (i == rows - 1) {
                empty_map->all_tiles[i][t]->symbol = '-';
                set_prop_vals(empty_map->all_tiles[i][t], FALSE, FALSE, FALSE);
            }
            else {
                empty_map->all_tiles[i][t]->symbol = (t % (columns - 1)) ? ' ' : '|';
                set_prop_vals(empty_map->all_tiles[i][t], (t % (columns - 1)) ? TRUE : FALSE, FALSE, FALSE);
            }
            empty_map->all_tiles[i][t]->x = t, empty_map->all_tiles[i][t]->y = i; 
        }
    }
    empty_map->enemy[0] = NULL;
    empty_map->columns = columns;
    empty_map->rows = rows;
    return empty_map;
}

/* free_map : frees the tile structures in the map's array, frees the pointer to the array, and frees the map structure */
struct Map *load_map(struct Map *old_map) {
    struct Map *next_map = NULL;

    /* Find the next map, free the current map */
    if (fate_returner('V') != THIRD_PARTY) {
        update_cop(COP_tracker, old_map);
    }
    if (old_map == Map1) {
        Map2 = next_map = initialize_Map2();
        Map1 = NULL;
    }
    else if (old_map == Map2) {
        Map3 = next_map = initialize_Map3();
        Map2 = NULL;
    }
    else if (old_map == Map3) {
        Map4 = next_map = initialize_Map4();
        Map3 = NULL;
    }
    else if (old_map == Map4) {
        Map5 = next_map = initialize_Map5();
        Map4 = NULL;
    }
    else if (old_map == Map5) {
        Map6 = next_map = initialize_Map6();
        Map5 = NULL;
    }
    else if (old_map == Map6) {
        Map7 = next_map = initialize_Map7();
        Map6 = NULL;
    }
    else if (old_map == Map7) {
        free_enemies(old_map);
        if (extra_returner('\\', COMPETED) && !extra_returner('\\', WON)) {
            Lobby = next_map = initialize_lobby();
        }
        else {
            Map8 = next_map = initialize_Map8();
        }
        Map7 = NULL;
    }
    else if (old_map == Lobby) {
        if (extra_returner('\\', TRAINED) || extra_returner('\\', FOUGHT) || extra_returner('\\', DUEL) || extra_returner('\\', HALTED)) {
            Map7 = next_map = initialize_Map7();
        }
        else {
            if (Jungle == NULL) {
                Jungle = initialize_jungle();
            }
            return Jungle;
        }
    }
    else if (old_map == Jungle) {
        if (extra_returner('\\', TRAINED) || extra_returner('\\', FOUGHT) || extra_returner('\\', DUEL) || extra_returner('\\', HALTED)) {
            Map7 = next_map = initialize_Map7();
        }
        else {
            if (Cliff == NULL) {
                Cliff = initialize_cliff();
            }
            return Cliff;
        }
    } 
    else if (old_map == Cliff) {
        if (extra_returner('\\', TRAINED) || extra_returner('\\', FOUGHT) || extra_returner('\\', DUEL) || extra_returner('\\', HALTED)) {
            printf("Initializing map 7\n");
            sleep(1);
            Map7 = next_map = initialize_Map7();
            printf("Initialized.\n");
            sleep(1);
        }
        else {
            if (Lobby == NULL) {
                Lobby = initialize_lobby();
            }
            return Lobby;
        }
    }
    else if (old_map == Map8) {
        ;
    }
    free_map(old_map);
    return next_map;
}

void update_cop(struct Enforcer *cop_ptr, struct Map *old_map) {
    old_map->player = cop_ptr;
    old_map->player->attr->occupied = old_map->all_tiles[old_map->rows - 2][old_map->columns / 2];

    int karma_cost = 0; // Cost for the path from current position of COP to the desired location
    while (old_map->enemy[0] != NULL) {
        // Hunt enemies
        //printf("Designated enemy is %s\n", old_map->enemy[0]->name);
        karma_cost = go_to_tile(old_map->player->attr->occupied->x, old_map->player->attr->occupied->y, old_map, old_map->enemy[0]->occupied->x, old_map->enemy[0]->occupied->y);
        //printf("Got cost");
        //sleep(1);
        reset_prop(old_map);
        // Traveling to enemy deduction:
        //printf("Karma cost traveling: %d\n", karma_cost);
        //sleep(2);
        cop_ptr->attr->karma -= karma_cost;
        if (cop_ptr->attr->karma <= 0) {
            // Insufficient karma implies losing 
            fate_store('V', THIRD_PARTY);
        }
        // Battle deduction:
        karma_cost = old_map->enemy[0]->max_health / 2;
        //printf("Karma cost battling: %d\n", karma_cost);
        cop_ptr->attr->karma -= karma_cost;
        cop_ptr->attr->health -= old_map->enemy[0]->attack * 4;
        if (cop_ptr->attr->health <= 0) {
            // Insufficient health implies losing
            fate_store('V', THIRD_PARTY);
        }
        else {
            // Else, obtain battle rewards:
            cop_ptr->attr->karma += (250 / old_map->enemy[0]->karma);
            cop_ptr->attr->health = minimum(cop_ptr->attr->max_health, cop_ptr->attr->health + cop_ptr->attr->max_health / 10);
            add_item(&first_enemy_item, old_map->enemy[0]->sword_buff);
            fate_store(old_map->enemy[0]->rep, THIRD_PARTY);
        }
        old_map->player->attr->occupied = old_map->enemy[0]->occupied;
        free_enemy(old_map, old_map->enemy[0]);
    }
    // Proceed to the entry point
    int i = 0, desired_x = 0;
    while (!desired_x) {
        if (old_map->all_tiles[1][old_map->columns / 2 + i]->prop.is_traversable) {
            desired_x = old_map->columns / 2 + i;
        }
        else if (old_map->all_tiles[1][old_map->columns / 2 - i]->prop.is_traversable) {
            desired_x = old_map->columns / 2 - i;
        }
        i++;
    }
    //printf("Got here, desired_x is %d\n", desired_x);
    karma_cost = go_to_tile(old_map->player->attr->occupied->x, old_map->player->attr->occupied->y, old_map, desired_x, 0);
    reset_prop(old_map);
    //printf("Karma cost traveling: %d\n", karma_cost);
    //sleep(2);
    // Traveling deduction:
    cop_ptr->attr->karma -= karma_cost;
    if (cop_ptr->attr->karma <= 0) {
        // Insufficient karma implies losing
        fate_store('V', THIRD_PARTY);
    }
    // Next map bonus
    cop_ptr->attr->health = minimum(cop_ptr->attr->max_health, cop_ptr->attr->health + NEXT_BONUS);
    cop_ptr->attr->karma += ceil(NEXT_BONUS * MULTIPLIER);

    //printf("Status: HP=%d, Karma=%d\n", cop_ptr->attr->health, cop_ptr->attr->karma);

    //struct item *selected_item = first_enemy_item;
    //while (selected_item) {
        //printf("%s\n", selected_item->description);
        //selected_item = selected_item->next_item;
    //}
}

int go_to_tile(int curr_x, int curr_y, struct Map *map, int desired_x, int desired_y) {
    struct tile *occupied_t = identify_tile(map, curr_y, curr_x);
    // Base case : Success
    if (curr_x == desired_x && curr_y == desired_y) {
        return 0;
    }
    // Base case : Failure
    else if (occupied_t == NULL) {
        return 50;
    }
    else if (occupied_t->prop.is_traversable == 0) {
        return 50;
    }

    // Recursive case :
    if (map->all_tiles[curr_y][curr_x]->prop.is_traversable) {
        set_prop_vals(map->all_tiles[curr_y][curr_x], FALSE, FALSE, TRUE);
    }

    // Enumerate paths
    if (curr_x == desired_x && curr_y + 1 == desired_y) {
        return 1;
    }
    else if (curr_x - 1 == desired_x && curr_y == desired_y) {
        return 1;
    }
    else if (curr_x == desired_x && curr_y - 1 == desired_y) {
        return 1;
    }
    else if (curr_x + 1 == desired_x && curr_y == desired_y) {
        return 1;
    }
    else {
        return minimum(minimum(1 + go_to_tile(curr_x, curr_y + 1, map, desired_x, desired_y), 1 + go_to_tile(curr_x, curr_y - 1, map, desired_x, desired_y)), minimum(1 + go_to_tile(curr_x - 1, curr_y, map, desired_x, desired_y), 1 + go_to_tile(curr_x + 1, curr_y, map, desired_x, desired_y)));
    }
}

void reset_prop(struct Map *map) {
    for (int j = 0; j < map->rows; j++) {
        for (int i = 0; i < map->columns; i++) {
            if (!map->all_tiles[j][i]->prop.is_traversable && !map->all_tiles[j][i]->prop.is_entry_map && map->all_tiles[j][i]->prop.is_occupied) {
                set_prop_vals(map->all_tiles[j][i], TRUE, FALSE, FALSE);
            }
        }
    }
}

// While not aligned x-wise and y-wise:
// Align y-wise
// If aligned y-wise, try to align x-wise
// ->If unable to align x-wise from that selected y-choice, explore y-wise
// If unable to align y-wise, explore x-wise

// "Unable to align"->The optimal x-choice is not possible 
// "Explore"->Choose x/y that is unexplored specific to the x/y path



/* _free_enemy
 * Delete dynamically allocated memory for a specified type struct Character
 * 
 * _Arguments
 * curr_map is a pointer to the current struct Map, which must contain the specified type struct Character argument
 * enemy is a pointer to the type struct Character, which is in curr_map, to be freed
 * 
 * _Variables
 * i is an iterative variable
 * 
 * */
void free_enemy(struct Map *curr_map, struct Character *enemy) {
    int i, t;
    enemy->occupied->prop.is_occupied = FALSE;
    for (i = 0; i < MAX_ENEMIES && curr_map->enemy[i]; ++i) { /* free current map's enemies */
        if (curr_map->enemy[i] == enemy) {
            free(curr_map->enemy[i]->name);
            curr_map->enemy[i] = NULL;
            free(curr_map->enemy[i]);
            for (t = i + 1; curr_map->enemy[t] && t < MAX_ENEMIES; ++t) { // Left shift all enemies after the enemy that free called
                curr_map->enemy[t - 1] = curr_map->enemy[t];
            }
            curr_map->enemy[t - 1] = NULL;
        }
    }
    return;
}

void free_enemies(struct Map *curr_map) {
    int i;
    for (i = 0; i < MAX_ENEMIES && curr_map->enemy[i] != NULL; i++) {
        free(curr_map->enemy[i]->name);
        curr_map->enemy[i] = NULL;
        free(curr_map->enemy[i]);  
    }

    return;
}

void free_map(struct Map *old_map) {
    int i, t;
    for (i = 0; i < old_map->rows; ++i) {
        for (t = 0; t < old_map->columns; ++t) {
            free(old_map->all_tiles[i][t]); // Free each tile that was dynamically allocated
        }
    }
    free(old_map->all_tiles);   // Free the array of tiles
    free(old_map);  // Free the memory used to allocate the Map structure

    return;
}

void set_common_symbols(struct Map *curr_map, int *indices, int num_length, char common_symbol, int set_traversable, int set_entry, int set_occupied) {
    int row, col;
    if (!num_length || !indices) {
        return;
    }
    for (int i = 0; i < num_length; i++) {
        curr_map->all_tiles[row = indices[i] / curr_map->columns][col = indices[i] - curr_map->columns * row]->symbol = common_symbol;
        set_prop_vals(curr_map->all_tiles[row][col], set_traversable, set_entry, set_occupied);
    }
}

struct Map *initialize_Map8(void) {
    struct Map *empty_map = NULL;
    empty_map = initialize_map(MAP8Y, MAX_COLUMNS);
    set_prop_vals(empty_map->all_tiles[COP_y][COP_x], TRUE, FALSE, TRUE);
    COP_tracker->attr->occupied = empty_map->all_tiles[COP_y][COP_x];
    for (int i = 1, t = 1; t < empty_map->columns - 1; t++) {
        if (t != COP_x) {
            empty_map->all_tiles[i][t]->symbol = '_';
            set_prop_vals(empty_map->all_tiles[i][t], FALSE, FALSE, FALSE);
        }
    }
    empty_map->enemy[0] = COP_tracker->attr;
    return empty_map;
}

struct Map *initialize_jungle(void) {
    int i, t;
    struct Map *empty_map = NULL;
    empty_map = initialize_map(JUNGLE_Y, MAX_COLUMNS);
    if (fate_returner('#') != CONVICTED) {
        allocate_enemy(empty_map, empty_map->all_tiles[FERAL_y][FERAL_x], "Chike", FERAL_hp, FERAL_hp, FERAL_attack, FERAL, '#', DRAIN);
    }
    /*for (i = 2; i < 7; i += 4) {
        for (t = 1; t < 11; t += 3) {
            empty_map->all_tiles[i][t]->symbol = '{';
            set_prop_vals(empty_map->all_tiles[i][t], FALSE, FALSE, FALSE);
            empty_map->all_tiles[i][t + 1]->symbol = '}';
            set_prop_vals(empty_map->all_tiles[i][t + 1], FALSE, FALSE, FALSE);
        }
    } */
    // index = i * MAX_COLUMNS + t 
    // { : 27, 30, 33, 36, 79, 82, 85, 88, 55, 58, 61, 106, 109, 112, 46, 66, 101, 124, 115
    // } : 28, 31, 34, 37, 80, 83, 86, 89, 56, 59, 62, 107, 110, 113, 47, 67, 102, 125, 53
    int *common_tiles = NULL;
    if ((common_tiles = malloc(19 * sizeof(common_tiles))) == NULL) {
        fprintf(stderr, "FATAL: No memory for common tile symbols.\n");
    }
    common_tiles[0] = 27, common_tiles[1] = 30, common_tiles[2] = 33, common_tiles[3] = 36, common_tiles[4] = 79, common_tiles[5] = 82, common_tiles[6] = 85, common_tiles[7] = 88, common_tiles[8] = 55, common_tiles[9] = 58, common_tiles[10] = 61, common_tiles[11] = 106, common_tiles[12] = 109, common_tiles[13] = 112, common_tiles[14] = 46, common_tiles[15] = 66, common_tiles[16] = 101, common_tiles[17] = 124, common_tiles[18] = 115;
    set_common_symbols(empty_map, common_tiles, 19, '{', FALSE, FALSE, FALSE);

    for (i = 0; i < 18; i++) {
        common_tiles[i]++;
    }
    common_tiles[18] = 53;
    set_common_symbols(empty_map, common_tiles, 19, '}', FALSE, FALSE, FALSE);
    
    /*for (i = 4, t = 3; t < 10; t += 3) {
        empty_map->all_tiles[i][t]->symbol = '{';
        set_prop_vals(empty_map->all_tiles[i][t], FALSE, FALSE, FALSE);
        empty_map->all_tiles[i][t + 1]->symbol = '}';
        set_prop_vals(empty_map->all_tiles[i][t + 1], FALSE, FALSE, FALSE);
    }
    empty_map->all_tiles[4][1]->symbol = '}';
    set_prop_vals(empty_map->all_tiles[4][1], FALSE, FALSE, FALSE);
    for (i = 8, t = 2; t < 9; t += 3) {
        empty_map->all_tiles[i][t]->symbol = '{';
        set_prop_vals(empty_map->all_tiles[i][t], FALSE, FALSE, FALSE);
        empty_map->all_tiles[i][t + 1]->symbol = '}';
        set_prop_vals(empty_map->all_tiles[i][t + 1], FALSE, FALSE, FALSE);
    }
    empty_map->all_tiles[8][11]->symbol = '{';
    set_prop_vals(empty_map->all_tiles[8][11], FALSE, FALSE, FALSE);
    empty_map->all_tiles[3][7]->symbol = '{';
    set_prop_vals(empty_map->all_tiles[3][7], FALSE, FALSE, FALSE);
    empty_map->all_tiles[3][8]->symbol = '}';
    set_prop_vals(empty_map->all_tiles[3][8], FALSE, FALSE, FALSE);
    empty_map->all_tiles[5][1]->symbol = '{';
    set_prop_vals(empty_map->all_tiles[5][1], FALSE, FALSE, FALSE);
    empty_map->all_tiles[5][2]->symbol = '}';
    set_prop_vals(empty_map->all_tiles[5][2], FALSE, FALSE, FALSE);
    empty_map->all_tiles[7][10]->symbol = '{';
    set_prop_vals(empty_map->all_tiles[7][10], FALSE, FALSE, FALSE);
    empty_map->all_tiles[7][11]->symbol = '}';
    set_prop_vals(empty_map->all_tiles[7][11], FALSE, FALSE, FALSE);
    empty_map->all_tiles[9][7]->symbol = '{';
    set_prop_vals(empty_map->all_tiles[9][7], FALSE, FALSE, FALSE);
    empty_map->all_tiles[9][8]->symbol = '}';
    set_prop_vals(empty_map->all_tiles[9][8], FALSE, FALSE, FALSE);*/
    return empty_map;
}

struct Map *initialize_cliff(void) {
    int i, j, t;
    struct Map *empty_map = NULL;
    empty_map = initialize_map(CLIFF_Y, MAX_COLUMNS);
    if (fate_returner('&') != CONVICTED) {
        allocate_enemy(empty_map, empty_map->all_tiles[ARTIST_y][ARTIST_x], "Faiza", ARTIST_hp, ARTIST_hp, ARTIST_attack, ARTIST, '&', UNCENSOR);
    }
    for (i = 1, t = 5; i < 4; ++i, t += 2) {
        empty_map->all_tiles[i][t]->symbol = '|';
        set_prop_vals(empty_map->all_tiles[i][t], FALSE, FALSE, FALSE);
        empty_map->all_tiles[i][t + 1]->symbol = '_';
        set_prop_vals(empty_map->all_tiles[i][t + 1], FALSE, FALSE, FALSE);
    }
    empty_map->all_tiles[3][11]->symbol = '_';
    set_prop_vals(empty_map->all_tiles[3][11], FALSE, FALSE, FALSE);
    for (i = 6, t = 11; i < 11; ++i, --t) {
        empty_map->all_tiles[i][t]->symbol = '|';
        set_prop_vals(empty_map->all_tiles[i][t], FALSE, FALSE, FALSE);
    }
    for (i = 1, t = 7; i < 3; ++i, t += 2) {
        for (j = t; j < 12; ++j) {
            empty_map->all_tiles[i][j]->symbol = '~';
            set_prop_vals(empty_map->all_tiles[i][j], FALSE, FALSE, FALSE);
        }
    }
    for (i = 7, t = 11; i < 11; ++i, --t) {
        for (j = t ; j < 12; ++j) {
            empty_map->all_tiles[i][j]->symbol = '~';
            set_prop_vals(empty_map->all_tiles[i][j], FALSE, FALSE, FALSE);
        }
    }
    return empty_map;
}

struct Map *initialize_lobby(void) {
    int i, t;
    struct Map *empty_map = NULL;
    empty_map = initialize_map(LOBBY_Y, MAX_COLUMNS);
    if (fate_returner('/') != SPARED) {
        allocate_enemy(empty_map, empty_map->all_tiles[HACKER_y][HACKER_x], "Lachlan", HACKER_hp, HACKER_hp, HACKER_attack, HACKER, '/', ALCHEMY);
    }
    if (fate_returner('!') != CONVICTED) {
        allocate_enemy(empty_map, empty_map->all_tiles[POLITICIAN_y][POLITICIAN_x], "Nicolau", POLITICIAN_hp, POLITICIAN_hp, POLITICIAN_attack, POLITICIAN, '!', MILK);
    }
    for (i = 4; i < 9; ++i) {
        switch (i) {
            case 4:
                empty_map->all_tiles[i][3]->symbol = '_';
                set_prop_vals(empty_map->all_tiles[i][3], FALSE, FALSE, FALSE);
                empty_map->all_tiles[i][9]->symbol = '_';
                set_prop_vals(empty_map->all_tiles[i][9], FALSE, FALSE, FALSE);
                break;
            default:
                empty_map->all_tiles[i][2]->symbol = '|';
                set_prop_vals(empty_map->all_tiles[i][2], FALSE, FALSE, FALSE);
                empty_map->all_tiles[i][3]->symbol = (i == 8) ? '_' : 'O';
                set_prop_vals(empty_map->all_tiles[i][3], FALSE, FALSE, FALSE);
                empty_map->all_tiles[i][4]->symbol = '|';
                set_prop_vals(empty_map->all_tiles[i][4], FALSE, FALSE, FALSE);
                empty_map->all_tiles[i][8]->symbol = '|';
                set_prop_vals(empty_map->all_tiles[i][8], FALSE, FALSE, FALSE);
                empty_map->all_tiles[i][9]->symbol = (i == 8) ? '_' : 'O';
                set_prop_vals(empty_map->all_tiles[i][9], FALSE, FALSE, FALSE);
                empty_map->all_tiles[i][10]->symbol = '|';
                set_prop_vals(empty_map->all_tiles[i][10], FALSE, FALSE, FALSE);
                break;
        }
    }
    for (i = 9, t = 1; t < 12; ++t) {
        switch (t) {
            case 5: case 7:
                empty_map->all_tiles[i][t]->symbol = '|';
                set_prop_vals(empty_map->all_tiles[i][t], FALSE, FALSE, FALSE);
                break;
            case 6:
                break;
            default:
                empty_map->all_tiles[i][t]->symbol = '_';
                set_prop_vals(empty_map->all_tiles[i][t], FALSE, FALSE, FALSE);
                break;
        }
    }
    // Remove later
    return empty_map;
}

struct Map *initialize_Map7(void) {
    int i, t;
    struct Map *empty_map = NULL;
    empty_map = initialize_map(MAP7Y, MAX_COLUMNS);
    for (i = 1; i < MAP7Y - 1; ++i) {
        if (i < 6 || i > 7) {
            empty_map->all_tiles[i][5]->symbol = '|';
            set_prop_vals(empty_map->all_tiles[i][5], FALSE, FALSE, FALSE);
        }
    }
    for (t = 1; t < 5; ++t) {
        empty_map->all_tiles[5][t]->symbol = '_';
        set_prop_vals(empty_map->all_tiles[5][t], FALSE, FALSE, FALSE);
        empty_map->all_tiles[7][t]->symbol = '_';
        set_prop_vals(empty_map->all_tiles[7][t], FALSE, FALSE, FALSE);
    }
    if (fate_returner('\\') == NEVER_MET) {
        allocate_enemy(empty_map, empty_map->all_tiles[6][1], "Matilda", ASSASSIN_hp, ASSASSIN_hp, ASSASSIN_attack, ASSASSIN, '\\', BULLET);
    }
    if (extra_returner('M', RETURNING) && fate_returner('M') == SPARED) {
        allocate_enemy(empty_map, empty_map->all_tiles[9][10], "Lille", QUEEN_hp, QUEEN_hp, QUEEN_attack, QUEEN, 'M', GUARD);
    }
    return empty_map;
}

struct Map *initialize_Map6(void) {
    int i, t;
    struct Map *empty_map = NULL;
    empty_map = initialize_map(MAP6Y, MAX_COLUMNS);
    for (i = 3; i < 6; ++i) {
        empty_map->all_tiles[i][3]->symbol = '|';
        set_prop_vals(empty_map->all_tiles[i][3], FALSE, FALSE, FALSE);
        empty_map->all_tiles[i][11]->symbol = '|';
        set_prop_vals(empty_map->all_tiles[i][11], FALSE, FALSE, FALSE);
    }
    for (t = 4; t < 11; ++t) {
        empty_map->all_tiles[2][t]->symbol = '_';
        set_prop_vals(empty_map->all_tiles[2][t], FALSE, FALSE, FALSE);
        if (t != 4) {
            empty_map->all_tiles[5][t]->symbol = '_';
            set_prop_vals(empty_map->all_tiles[5][t], FALSE, FALSE, FALSE);
        }
    }
    for (i = 3; i < 6; ++i) {
        empty_map->all_tiles[i][5]->symbol = '|';
        set_prop_vals(empty_map->all_tiles[i][5], FALSE, FALSE, FALSE);
    }
    empty_map->all_tiles[3][6]->symbol = '[';
    empty_map->all_tiles[4][6]->symbol = '[';
    empty_map->all_tiles[3][7]->symbol = ']';
    empty_map->all_tiles[4][7]->symbol = ']';
    allocate_enemy(empty_map, empty_map->all_tiles[5][4], "Grischa", BAT_hp, BAT_hp, BAT_attack, BAT, '@', RANG);
    return empty_map;
}

struct Map *initialize_Map5(void) {
    struct Map *empty_map = NULL;
    empty_map = initialize_map(MAP5Y, MAX_COLUMNS);
    allocate_enemy(empty_map, empty_map->all_tiles[NAPPER_y][NAPPER_x], "Lucinda", NAPPER_hp, NAPPER_hp, NAPPER_attack, NAPPER, 'C', GOLEM);
    allocate_enemy(empty_map, empty_map->all_tiles[FRAUDSTER_y][FRAUDSTER_x], "Dalia", FRAUDSTER_hp, FRAUDSTER_hp, FRAUDSTER_attack, FRAUDSTER, '?', MOON);
    return empty_map;
}

struct Map *initialize_Map4(void) {
    int i, t;
    struct Map *empty_map = NULL;
    empty_map = initialize_map(MAP4Y, MAX_COLUMNS);
    for (t = 5; t < MAX_COLUMNS - 1; ++t) {
        empty_map->all_tiles[1][t]->symbol = '_';
        set_prop_vals(empty_map->all_tiles[1][t], FALSE, FALSE, FALSE);
    }
    for (t = 5; t < MAX_COLUMNS - 1; ++t) {
        empty_map->all_tiles[7][t]->symbol = '_';
        set_prop_vals(empty_map->all_tiles[7][t], FALSE, FALSE, FALSE);
    }
    for (i = 2; i < 8; ++i) {
        if (i != 4) {
            empty_map->all_tiles[i][4]->symbol = '|';
            set_prop_vals(empty_map->all_tiles[i][4], FALSE, FALSE, FALSE);
        }
    }
    for (i = 3; i < 6; ++i) {
        empty_map->all_tiles[i][MAX_COLUMNS - 3]->symbol = '|';
        set_prop_vals(empty_map->all_tiles[i][MAX_COLUMNS - 3], FALSE, FALSE, FALSE);
    }
    if (!extra_returner('+', ALERTED) || fate_returner('+') == CONVICTED) {
        allocate_enemy(empty_map, empty_map->all_tiles[ARSONIST_y][ARSONIST_x], "Inigo", ARSONIST_hp, ARSONIST_hp, ARSONIST_attack, ARSONIST, '*', FIRE);
    }
    else {
        extra_store('*', TRAILED);
    }
    allocate_enemy(empty_map, empty_map->all_tiles[LUNCHLADY_y][LUNCHLADY_x], "Lexa", LUNCHLADY_hp, LUNCHLADY_hp, LUNCHLADY_attack, LUNCHLADY, 'L', FAMINE);
    return empty_map;
}
struct Map *initialize_Map3(void) {
    struct Map *empty_map = NULL;
    empty_map = initialize_map(MAP3Y, MAX_COLUMNS);
    allocate_enemy(empty_map, empty_map->all_tiles[LORD_y][LORD_x], "Severino", LORD_hp, LORD_hp, LORD_attack, LORD, '%', RECOVER);
    return empty_map;
}

/* initialize_Map2 : contains the box-formatted map w/ different tiles */
struct Map *initialize_Map2(void) {
    int i, t;
    struct Map *empty_map = NULL;
    empty_map = initialize_map(MAP2Y, MAX_COLUMNS);
    for (t = 3; t < 8; ++t) {
        empty_map->all_tiles[2][t]->symbol = '_';
        set_prop_vals(empty_map->all_tiles[2][t], FALSE, FALSE, FALSE);
    }
    for (t = 3; t < 6; ++t) {
        empty_map->all_tiles[5][t]->symbol = '-';
        set_prop_vals(empty_map->all_tiles[5][t], FALSE, FALSE, FALSE);
    }
    for (i = 3; i < 5; ++i) {
        empty_map->all_tiles[i][3]->symbol = '|';
        set_prop_vals(empty_map->all_tiles[i][3], FALSE, FALSE, FALSE);
        empty_map->all_tiles[i][7]->symbol = '|';
        set_prop_vals(empty_map->all_tiles[i][7], FALSE, FALSE, FALSE);
    }
    allocate_enemy(empty_map, empty_map->all_tiles[DOC_y][DOC_x], "Hayato", DOC_hp, DOC_hp, DOC_attack, DOC, '+', HEAL);
    allocate_enemy(empty_map, empty_map->all_tiles[QUEEN_y][QUEEN_x], "Lille", QUEEN_hp, QUEEN_hp, QUEEN_attack, QUEEN, 'M', GUARD);
    return empty_map;
}

/* allocate_enemy : assigns values to a pointer to a Character in enemies */
void allocate_enemy(struct Map *new_map, struct tile *posn, char *name, int hp, int max_hp, unsigned int atk, int karma_id, char representation, unsigned int sword_id) {
    int i;
    // Fixes ~i~ to the index of the first NULL ~*Character~ in ~new_map~; if no NULL ~*Character~ is found, ~i~ is fixed to ~MAX_ENEMIES~
    for (i = 0; new_map->enemy[i] != NULL && i < MAX_ENEMIES; ++i) {
        ;
    }
    if ((new_map->enemy[i] = malloc(sizeof(*new_map->enemy[i]))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate an enemy.\n");
    }
    else {
        new_map->enemy[i]->name = str_dup(name), new_map->enemy[i]->attack = atk, new_map->enemy[i]->health = hp, new_map->enemy[i]->max_health = max_hp;
        new_map->enemy[i]->karma = karma_id, new_map->enemy[i]->occupied = posn, new_map->enemy[i]->rep = representation;
        new_map->enemy[i]->buff = NONE, new_map->enemy[i]->sword_buff = sword_id, new_map->enemy[i]->sword_debuff = NONE, new_map->enemy[i]->debuff = NONE;
        posn->prop.is_occupied = TRUE;
        if (i + 1 < MAX_ENEMIES) {
            new_map->enemy[i + 1] = NULL;
        }
    }
}

/* next_map : draws out the next map */
struct Map *find_next_map(struct Map *curr_map) {
    int i, hor, ver;
    char line[MAX_COLUMNS + 1];
    struct Map *next_map_ptr = NULL;
    struct Enforcer *temp_char = NULL;
    if (curr_map->player->attr->occupied->prop.is_entry_map) {
        temp_char = curr_map->player;
        // Check if the current map is the lobby - if yes, heals for more because there's free food there (Acts like hub of game)
        if (Lobby != NULL && curr_map == Lobby) {
            temp_char->attr->health = minimum(temp_char->attr->max_health, temp_char->attr->health += 30);    
        }
        else {
            temp_char->attr->health = minimum(temp_char->attr->max_health, temp_char->attr->health += ceil(NEXT_BONUS * MULTIPLIER));
        }
        next_map_ptr = load_map(curr_map);
        next_map_ptr->player = temp_char;
        temp_char->attr->karma += ceil(NEXT_BONUS * MULTIPLIER);
        move_char(next_map_ptr, next_map_ptr->all_tiles[next_map_ptr->rows - 2][next_map_ptr->columns / 2]);
        curr_map = next_map_ptr;
    }
    for (ver = 0; ver < curr_map->rows; ++ver) {   // Pointer to tile exists?
        for (hor = 0; hor < curr_map->columns; ++hor) {  // Tile exists?
            if (curr_map->all_tiles[ver][hor]->prop.is_occupied) { // Tile is occupied by a Character: search for it.
                line[hor] = identify_rep(curr_map->all_tiles[ver][hor], curr_map->player->attr, curr_map->enemy)->rep;
            }
            else {
                line[hor] = curr_map->all_tiles[ver][hor]->symbol; // Else, it has normal representation.
            }
        }
        line[hor] = '\0';
        center_screen(WIDTH, "%s\n", line);
    }
    if (DEBUG) {
        printf("POP find_next_map\n");
        sleep(1);
    }
    return curr_map; 
}

/* identify_rile : when a tile's position is known, return a pointer to it */
struct tile *identify_tile(struct Map *curr_map, unsigned int ver, unsigned int hor) {
    struct tile *return_ptr = NULL;
    if (ver < 0 || ver >= curr_map->rows) {
        return NULL;
    }
    else if (hor < 0 || hor >= curr_map->columns) {
        return NULL;
    }
    if (return_ptr = curr_map->all_tiles[ver][hor]) {
        return return_ptr;
    }
}

/* fmt_map : formats the map to the center, as well as the player UI underneath it; receives directional input */
void fmt_map(struct Map *map_ptr) {
    clrscr();
    int keystr;
    if (!Controller->player_state.is_map) {
        printf("POP fmt_map\n");
        sleep(1);
        return;
    }
    map_ptr = find_next_map(map_ptr);
    ui_enforcer(map_ptr->player);
    set_state_vals(Controller, FALSE, FALSE, TRUE, FALSE, FALSE);
    while ((keystr = getaction(Controller)) != _EOF) {
        if (process_movement(map_ptr, keystr)) {
            clrscr();
            map_ptr = find_next_map(map_ptr);
            ui_enforcer(map_ptr->player);
            set_state_vals(Controller, FALSE, FALSE, TRUE, FALSE, FALSE);
        }
    }
    quit();
}

void swap_enemy(struct Map *map_ptr, char rep1, char rep2) {
    int i, first = -1, second = -1;
    struct Character *temp = NULL;
    if (rep1 == rep2) {
        return;
    }
    else {
        for (i = 0; i < MAX_ENEMIES && map_ptr->enemy[i]; i++) {
            if (rep1 == map_ptr->enemy[i]->rep) {
                first = i;
            }
            else if (rep2 == map_ptr->enemy[i]->rep) {
                second = i;
            }
        }
    }
    
    if (first != -1 && second != -1) {
        temp = map_ptr->enemy[first];
        map_ptr->enemy[first] = map_ptr->enemy[second];
        map_ptr->enemy[second] = temp;
    }
    return;
}

struct tile *trigger_battle_event(struct Map *curr_map, char enemy_rep) {
    struct tile *next_tile = NULL;
    // 1. Defeating the LORD will cause the STOWAWAY to appear (as if the STOWAWAY has nothing to steal now that the LORD has been absorbed)
        if (enemy_rep == '%') {
            allocate_enemy(curr_map, curr_map->all_tiles[STOWAWAY_y][STOWAWAY_x], "Nemo", STOWAWAY_hp, STOWAWAY_hp, STOWAWAY_attack, STOWAWAY, '$', TRICK);
        }
        // 2. Defeating the LUNCHLADY when the ARSONIST has been hired by the DOC will cause a fight with the ARSONIST (as if the ARSONIST has seen a perfect opportunity to get rid of his targets) 
        else if (enemy_rep == 'L' && extra_returner('*', TRAILED)) {
            allocate_enemy(curr_map, curr_map->all_tiles[4][9], "Inigo", ARSONIST_hp, ARSONIST_hp, ARSONIST_attack, ARSONIST, '*', FIRE);
            next_tile = curr_map->all_tiles[4][9];
        }
        // 3. Defeating the BAT when the ARSONIST has been hired by the DOC will cause a fight with the ARSONIST (as if the ARSONIST has seen a perfect opportunity to get rid of his targets)
        else if (enemy_rep == '@' && extra_returner('*', TRAILED) && fate_returner('*') != CONVICTED) {
            allocate_enemy(curr_map, curr_map->all_tiles[BAT_y - 3][BAT_x], "Inigo", ARSONIST_hp, ARSONIST_hp, ARSONIST_attack, ARSONIST, '*', FIRE);
            next_tile = curr_map->all_tiles[BAT_y - 3][BAT_x];
            curr_map->all_tiles[2][4]->symbol = ' ';
            set_prop_vals(curr_map->all_tiles[2][4], TRUE, FALSE, FALSE);
        }
        // 4. Defeating the last remaining criminal on the island will cause dialogue with the Australian siblings (as if the user has won their game)
        else if (enemy_rep == '!' || enemy_rep == '#' || enemy_rep == '&') {
            if ((fate_returner('!') == CONVICTED) && (fate_returner('#') == CONVICTED) && (fate_returner('&') == CONVICTED)) {
                extra_store('\\', WON);
                allocate_enemy(curr_map, curr_map->all_tiles[0][MAX_COLUMNS / 2], "Matilda", ASSASSIN_hp, ASSASSIN_hp, ASSASSIN_attack, ASSASSIN, '\\', BULLET);
                next_tile = curr_map->all_tiles[0][MAX_COLUMNS / 2];
            }
        }
        // 5. Defeating the ASSASSIN when the user has chosen a fight with the Australian siblings will cause a fight with the HACKER (as if the event is a boss rush)
        else if (enemy_rep == '\\' && extra_returner('\\', FOUGHT)) {
            allocate_enemy(curr_map, curr_map->all_tiles[0][MAX_COLUMNS / 2], "Lachlan", HACKER_hp, HACKER_hp, HACKER_attack, HACKER, '/', ALCHEMY);
            next_tile = curr_map->all_tiles[0][MAX_COLUMNS / 2];
        }
    return next_tile;
}

/* trigger_battle : formats the battle screen, receives battle input; can trigger game over or victory */
void trigger_battle(struct Map *map_ptr, struct Character *enemy_ptr, int turn_no) {
    static int player_strength;
    struct item *selected_item = NULL;
    struct tile *temp = NULL;
    int selected_sword;
    char *player_buff = NULL, *player_debuff = NULL, *enemy_buff = NULL, *enemy_debuff = NULL;
    // Save permanent stats of the player on first turn
    if (turn_no == 1) {
        player_strength = map_ptr->player->attr->attack;
        if (enemy_ptr->rep == '#') {
            debuff_char(enemy_ptr, map_ptr->player->attr, RESTRAINED);
        }
    }
    if (map_ptr->player->attr->health <= 0) { // lost the battle -> game is over
        player_death();
    }
    /* won the battle -> trigger some event, prevent soft-locking w/ second condition */
    else if (enemy_ptr->health <= 0 || enemy_ptr->health < 3 && enemy_ptr->buff == REGEN) {
        map_ptr->player->attr->attack = player_strength;
        finished(enemy_ptr, map_ptr->player);
        identify_tile(map_ptr, enemy_ptr->occupied->y, enemy_ptr->occupied->x)->prop.is_occupied = FALSE;
        map_ptr->player->attr->buff = NONE, map_ptr->player->attr->debuff = NONE, map_ptr->player->attr->sword_buff = NONE, map_ptr->player->attr->sword_debuff = NONE;
        if (DEBUG) {
            printf("POP trigger_battle\n");
            sleep(1);
        }
        return;
    }
    player_buff = buff_status(map_ptr->player->attr, enemy_ptr);
    player_debuff = debuff_status(map_ptr->player->attr, enemy_ptr);
    enemy_buff = buff_status(enemy_ptr, map_ptr->player->attr);
    enemy_debuff = debuff_status(enemy_ptr, map_ptr->player->attr);
    clrscr();
    int keystr;
    center_screen(WIDTH, "%s\n%s\n", enemy_buff, enemy_debuff);
    if (enemy_ptr->rep != 'V') {
        ui(enemy_ptr);
    }
    else {
        ui_enforcer(COP_tracker);
    }
    center_screen(WIDTH, "%c\n\n", enemy_ptr->rep);
    center_screen(WIDTH, "%c\n", map_ptr->player->attr->rep);
    ui_enforcer(map_ptr->player);
    center_screen(WIDTH, "%s\n%s\n", player_buff, player_debuff);
    center_screen(WIDTH, "%s\n", "r : Attack / i : Inventory / s : Sword");
    set_state_vals(Controller, FALSE, FALSE, FALSE, TRUE, FALSE);
    while ((keystr = getaction(Controller)) != _EOF) {
        if (map_ptr->player->attr->debuff == RESTRAINED || map_ptr->player->attr->debuff == ASLEEP) {
            center_screen(WIDTH, "%s\n", "You can't fight back!");
            if (map_ptr->player->attr->buff == SUMMON) {
                center_screen(WIDTH, "%s\n", "Yosef slams his fist down!");
                enemy_ptr->health -= map_ptr->player->attr->ally_attack;
            }
            if (enemy_ptr->health > 0) {
                if (enemy_ptr->rep != 'V') {
                    (*enemy_ptr->behavior)(map_ptr->player, enemy_ptr, turn_no);
                }
                else {
                    COP_behavior(map_ptr->player, COP_tracker, turn_no);
                }
            }
            free(player_buff);
            free(player_debuff);
            free(enemy_buff);
            free(enemy_debuff);
            trigger_battle(map_ptr, enemy_ptr, turn_no + 1);
            return;
            break;
        }
        switch (keystr) {
            case Select:
                set_state_vals(Controller, TRUE, FALSE, FALSE, TRUE, FALSE);
                damage_dealt(map_ptr->player->attr, enemy_ptr, map_ptr->player->attr->attack);
                if (map_ptr->player->attr->sword_debuff != DISABLED) {
                    increment_sword(map_ptr->player);
                }
                if (map_ptr->player->attr->sword_buff == DEMONIC) {
                    map_ptr->player->attr->karma += (pow(2, total_fate(CONVICTED)) / 5);
                }
                if (map_ptr->player->attr->buff == SUMMON) {
                    center_screen(WIDTH, "%s\n", "Yosef slams his fist down!");
                    enemy_ptr->health -= map_ptr->player->attr->ally_attack;
                    sleep(2);
                }
                break;
            case Inventory:
                if (map_ptr->player->attr->debuff != ITEMLESS) {
                    free(player_buff);
                    free(player_debuff);
                    free(enemy_buff);
                    free(enemy_debuff);
                    if (selected_item = inventory_screen(first_item, map_ptr->player, enemy_ptr, turn_no)) {
                        item_turn(selected_item, map_ptr->player, enemy_ptr, turn_no);
                        trigger_battle(map_ptr, enemy_ptr, ++turn_no);
                    }
                    else {
                        trigger_battle(map_ptr, enemy_ptr, turn_no);    // Reload battle window 
                    }
                    return;
                }
                break;
            case Sword:
                if (map_ptr->player->sword_points != MAX_SWORD || map_ptr->player->attr->sword_debuff == DISABLED) {
                    center_screen(WIDTH, "%s\n", "Sword is not charged.");
                    sleep(1);
                    free(player_buff);
                    free(player_debuff);
                    free(enemy_buff);
                    free(enemy_debuff);
                    trigger_battle(map_ptr, enemy_ptr, turn_no);
                    return;
                }
                if ((selected_sword = sword_screen(map_ptr, enemy_ptr, turn_no)) == 0) {
                    free(player_buff);
                    free(player_debuff);
                    free(enemy_buff);
                    free(enemy_debuff);
                    trigger_battle(map_ptr, enemy_ptr, turn_no);
                    return;
                }
                else {
                    map_ptr->player->sword_points = 0;
                    switch (selected_sword) {
                        case ATTACK_ID:
                            map_ptr->player->attack_cd = ATTACK_CD;
                            if (map_ptr->player->alignment < MAX_ALIGNMENT / 3) {
                                center_screen(WIDTH, "%s\n", "You ravage your enemy.");
                                enemy_ptr->health -= total_fate(CONVICTED);
                                if (enemy_ptr->health <= 0) {
                                    center_screen(WIDTH, "%s\n", "You celebrate your victory.");
                                    health_healed(map_ptr->player->attr, enemy_ptr, map_ptr->player->attr->max_health / 10);
                                }
                            }
                            else if (map_ptr->player->alignment < 2 * (MAX_ALIGNMENT / 3)) {
                                center_screen(WIDTH, "%s\n", "You dealt out your special move.");
                                enemy_ptr->health -= -1 * 3 * abs(map_ptr->player->alignment - 7) + 9;
                            }
                            else {
                                center_screen(WIDTH, "%s\n", "You smite your opponent.");
                                enemy_ptr->health -= 2 * total_fate(SPARED);
                            }
                            break;
                        case SIGNATURE_ID:
                            map_ptr->player->signature_cd = SIGNATURE_CD;
                            if (map_ptr->player->alignment < MAX_ALIGNMENT / 3) {
                                center_screen(WIDTH, "%s\n", "You infuse your sword with demonic energy.");
                                s_buff_char(map_ptr->player->attr, enemy_ptr, DEMONIC);
                            }
                            else if (map_ptr->player->alignment < 2 * (MAX_ALIGNMENT / 3)) {
                                center_screen(WIDTH, "%s\n", "You bend the temporal laws.");
                                s_buff_char(map_ptr->player->attr, enemy_ptr, TEMPORAL);
                                free(player_buff);
                                free(player_debuff);
                                free(enemy_buff);
                                free(enemy_debuff);
                                trigger_battle(map_ptr, enemy_ptr, turn_no);
                                return;
                            }
                            else {
                                center_screen(WIDTH, "%s\n", "You receive a holy blessing from somewhere.");
                                health_healed(map_ptr->player->attr, enemy_ptr, pow(total_fate(SPARED), 2));
                                map_ptr->player->attr->karma += pow(total_fate(SPARED), 2) / 2;
                            }
                            break;
                        case DEFENSE_ID:
                            map_ptr->player->defense_cd = DEFENSE_CD;
                            if (map_ptr->player->alignment < MAX_ALIGNMENT / 3) {
                                center_screen(WIDTH, "%s\n", "You get ready to parry an enemy's attack.");
                                s_buff_char(map_ptr->player->attr, enemy_ptr, PARRYING);
                            }
                            else if (map_ptr->player->alignment < 2 * (MAX_ALIGNMENT / 3)) {
                                center_screen(WIDTH, "%s\n", "You brace yourself for any attack.");
                                s_buff_char(map_ptr->player->attr, enemy_ptr, BLOCKING);
                            }
                            else {
                                center_screen(WIDTH, "%s\n", "You calmed down your opponent a bit.");
                                s_debuff_char(map_ptr->player->attr, enemy_ptr, PACIFIED);
                            }
                            break;
                    }
                    sleep(1);
                }
                break;
        }
        if (keystr == Select || keystr == Sword) {
            if (enemy_ptr->health > 0) {
                if (enemy_ptr->rep != 'V') {
                    (*enemy_ptr->behavior)(map_ptr->player, enemy_ptr, turn_no);
                }
                else {
                    COP_behavior(map_ptr->player, COP_tracker, turn_no);
                }
            }
            map_ptr->player->attack_cd = maximum(0, map_ptr->player->attack_cd - 1);
            map_ptr->player->signature_cd = maximum(0, map_ptr->player->signature_cd - 1);
            map_ptr->player->defense_cd = maximum(0, map_ptr->player->defense_cd - 1);
            free(player_buff);
            free(player_debuff);
            free(enemy_buff);
            free(enemy_debuff);
            trigger_battle(map_ptr, enemy_ptr, ++turn_no);
            return;
        }
    }
    quit();
}

enum item_attempts {
    LEGAL,
    CONSUMED,
    UNAFFORDABLE
};

/* inventory_screen : displays the current item selected; receives input to use item, select item, or exit screen */
struct item *inventory_screen(struct item *first_item_slot, struct Enforcer *user_ptr, struct Character *enemy_ptr, int turn_no) {
    int keystr = Placeholder;
    struct item *selected_item = first_item_slot;
    do {
        switch (keystr) {
            case Right:
                selected_item = selected_item->next_item;
                break;
            case Inventory:
                return NULL;    // Canceled using an item
                break;
            case Select:
                switch (return_item_attempt(selected_item, user_ptr)) {
                    case LEGAL:
                        return selected_item;
                        break;
                    case CONSUMED:
                        center_screen(WIDTH, "%s\n", "No more item uses.");
                        sleep(1);
                        break;
                    case UNAFFORDABLE:
                        center_screen(WIDTH, "%s\n", "Not enough karma.");
                        sleep(1);
                        break;
                }
                break;
        }
        if (selected_item == NULL) { /* move back to the first item (DAMAGE) when last item has been displayed and player inputs 'd' */
            selected_item = first_item_slot;
        }
        clrscr();
        puts(selected_item->image);
        if (selected_item->uses != INFINITE) { /* instead of [x karma], we have finite uses to be displayed [x uses] */
            printf("[%d uses]\n", selected_item->uses);
        }
        puts(selected_item->description);
        center_screen(WIDTH, "%s\n", "r : Use item / d : Next item / i : Exit inventory");
    } while ((keystr = getaction(Controller)) != _EOF);
    quit();
}

int return_item_attempt(struct item *selected_item, struct Enforcer *user_ptr) {
    if (!selected_item->uses) {
        return CONSUMED;
    }
    if (user_ptr->attr->karma >= selected_item->karma_cost || user_ptr->attr->sword_buff == TEMPORAL || user_ptr->attr->sword_buff == TEMPORAL2) {
        return LEGAL;
    }
    else {
        return UNAFFORDABLE;
    }
}

/* sword_screen: displays alignment meter, available sword skills, and sword skill cooldowns; then, receives input to use skill or exit the screen */
int sword_screen(struct Map *map_ptr, struct Character *enemy_ptr, int turn_no) {
    int keystr = Placeholder;
    char z[30];
    char x[30];
    char c[30];
    if (map_ptr->player->attack_cd) {
        sprintf(z, "[Ready in %d turns]Attack", map_ptr->player->attack_cd);
    }
    else {
        sprintf(z, "[Ready,CD=%d turns]Attack", ATTACK_CD);
    }
    if (map_ptr->player->signature_cd) {
        sprintf(x, "[Ready in %d turns]Signature", map_ptr->player->signature_cd);
    }
    else {
        sprintf(x, "[Ready,CD=%d turns]Signature", SIGNATURE_CD);
    }
    if (map_ptr->player->defense_cd) {
        sprintf(c, "[Ready in %d turns]Defense", map_ptr->player->defense_cd);
    }
    else {
        sprintf(c, "[Ready,CD=%d turns]Defense", DEFENSE_CD);
    }

    char z_l[] = "(Ravage) Deal flat damage equal to the number of enemies defeated and heal for 10% of maximum health if finishing blow";
    char z_m[] = "(Peacekeeper's Strike) Deal flat damage equal to with closeness of affinity to the middle (3, 6, 9, 6, 3)";
    char z_r[] = "(Smite) Deal flat damage equal to twice the number of opponents talked to that were not battled";

    char x_l[] = "(Demonic Infusion) Normal attacks restore karma equal to the square of the number of enemies defeated divided by 5";
    char x_m[] = "(Temporal Tear) Gain two consecutive turns before your enemy where items cost no karma";
    char x_r[] = "(Holy Blessing) Restore health equal to the square of number of opponents talked to that were not battled and\nrestore karma equal to the square of the number of opponents talked to that were not battled divided by 2";

    char c_l[] = "(Retaliate) Reduce non-flat damage taken by 50% on the next non-flat hit and automatically deal back the damage received.";
    char c_m[] = "(Block) Reduce non-flat damage taken by 100% on the next non-flat hit.";
    char c_r[] = "(Pacify) Reduce opponent's attack by 1 for 5 turns";
    
    do {
        switch(keystr) {
            case Attack:
                if (!map_ptr->player->attack_cd) {
                    return ATTACK_ID;
                }
                else {
                    center_screen(WIDTH, "%s\n", "On cooldown.");
                    sleep(1);
                }
                break;
            case Signature:
                if (!map_ptr->player->signature_cd) {
                    return SIGNATURE_ID;
                }
                else {
                    center_screen(WIDTH, "%s\n", "On cooldown.");
                    sleep(1);
                }
                break;
            case Defense:
                if (!map_ptr->player->defense_cd) {
                    return DEFENSE_ID;
                }
                else {
                    center_screen(WIDTH, "%s\n", "On cooldown.");
                    sleep(1);
                }
                break;
            case Sword:
                return NONE;
                break;
        }
        clrscr();
        alignment_meter(map_ptr->player->alignment);
        puts(z);
        if (map_ptr->player->alignment < MAX_ALIGNMENT / 3) {
            puts(z_l);
            puts(x);
            puts(x_l);
            puts(c);
            puts(c_l);
        }
        else if (map_ptr->player->alignment < 2 * (MAX_ALIGNMENT / 3)) {
            puts(z_m);
            puts(x);
            puts(x_m);
            puts(c);
            puts(c_m);
        }
        else {
            puts(z_r);
            puts(x);
            puts(x_r);
            puts(c);
            puts(c_r);
        }
        center_screen(WIDTH, "%s\n", "z : Attack Skill / x : Signature Skill / c : Defense Skill / s : Exit sword");
    } while ((keystr = getaction(Controller)) != _EOF);
    quit();
}