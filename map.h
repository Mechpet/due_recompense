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

#define MAX_COLUMNS 13
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

struct Map {
    struct tile *(*all_tiles)[MAX_COLUMNS];
    int rows;
    int columns;  
    struct Character *player;
    struct Character *enemy[MAX_ENEMIES];
} *Map1 = NULL, *Map2 = NULL, *Map3 = NULL, *Map4 = NULL, *Map5 = NULL, *Map6 = NULL, *Map7 = NULL, *Lobby = NULL, *Cliff = NULL, *Jungle = NULL, *Map8 = NULL;

struct Map *initialize_map(int rows_of_tiles, int columns_of_tiles);
struct Map *load_map(struct Map *old_map);
void swap_enemy(struct Map *map_ptr, char rep1, char rep2);
void free_enemy(struct Map *old_map, struct Character *enemy);
void free_enemies(struct Map *old_map);
void free_map(struct Map *old_map);
void allocate_enemy(struct Map *Map_n_ptr, struct tile *tile_ptr, char *name, int hp, int max_hp, unsigned int atk, int karma_id, char representation);
struct tile *identify_tile(struct Map *curr_map, unsigned int ver, unsigned int hor);
void set_common_symbols(struct Map *curr_map, int *indices, int num_length, char common_symbol, int set_traversable, int set_entry, int set_occupied);
struct tile *decline_event(struct Map *curr_map, char rep);
struct tile *trigger_battle_event(struct Map *curr_map, char rep);

struct Map *find_next_map(struct Map *Map_n_ptr);
void move_char(struct Map *Map_n_ptr, struct tile *next_tile_to_move_to);
int process_movement(struct Map *old_map, int key);
void fmt_map(struct Map *map_ptr);

struct Map *initialize_Map2(void);
struct Map *initialize_Map3(void);
struct Map *initialize_Map4(void);
struct Map *initialize_Map5(void);
struct Map *initialize_Map6(void);
struct Map *initialize_Map7(void);
struct Map *initialize_Map8(void);
struct Map *initialize_lobby(void);
struct Map *initialize_cliff(void);
struct Map *initialize_jungle(void);

void trigger_battle(struct Map *map_ptr, struct Character *enemy_ptr, int turn_number);
struct item *inventory_screen(struct item *first_item_slot, struct Map *map_ptr, struct Character *enemy_ptr, int turn_no);
void item_turn(struct item *item_used, struct Map *map_ptr, struct Character *enemy_ptr, int turn_no);

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
                allocate_enemy(curr_map, curr_map->all_tiles[4][MAX_COLUMNS - 4], "Inigo", ARSONIST_hp, ARSONIST_hp, ARSONIST_attack, ARSONIST, '*');
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
                allocate_enemy(curr_map, curr_map->all_tiles[5][4], "Inigo", ARSONIST_hp, ARSONIST_hp, ARSONIST_attack, ARSONIST, '*');
                next_tile = curr_map->all_tiles[5][4];
            }
            else {
                if ((common_tiles = malloc(27 * sizeof(*common_tiles))) == NULL)
                    fprintf(stderr, "FATAL: No memory for common tile symbols.\n"); // Row_number * Number_col + Col_number (11,13)
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
                    curr_map->player->attack += 2;
                    curr_map->player->max_health += 3;
                    curr_map->player->health += 3;
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
    if (curr_map->player->occupied != NULL)  // Free up occupying space for the previous tile 
        curr_map->player->occupied->prop.is_occupied = FALSE;
    while (trigger) {   // While loop for moving of user (break when done; break when trigger is OFF)
        // Case where the next tile is already occupied by an enemy
        if (next_tile->prop.is_occupied) {
            struct Character *enemy_encountered = identify_rep(next_tile, curr_map->player, curr_map->enemy);
            enemy_rep = enemy_encountered->rep; // Store the *rep* member before free is called to the Character
            set_state_vals(Controller, FALSE, FALSE, FALSE, TRUE, FALSE);

            // Path where the user fights the enemy
            if (intro(enemy_encountered, curr_map->player) == FALSE) {  // Display the enemy's intro and receives input from the user (to battle or not?)
                trigger_battle(curr_map, enemy_encountered, 1);
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
    curr_map->player->occupied = next_tile; 
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
    int curr_row = curr_map->player->occupied->y;
    int curr_column = curr_map->player->occupied->x;
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
        if (karma_decay(curr_map->player) <= 0) { /* trigger intro w/ JUDGE when karma -> 0 (quota not met) */
            set_state_vals(Controller, FALSE, FALSE, FALSE, TRUE, FALSE);
            allocate_enemy(curr_map, curr_map->all_tiles[0][0], "Boss", 101, 101, 10, JUDGE, 'X');
            if (!intro(identify_rep(curr_map->all_tiles[0][0], curr_map->player, curr_map->enemy), curr_map->player)) {
                trigger_recompense();
                trigger_battle(curr_map, identify_rep(curr_map->all_tiles[0][0], curr_map->player, curr_map->enemy), 1);
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
    if (!(empty_map = malloc(sizeof(*empty_map))))
        fprintf(stderr, "FATAL: No more memory to allocate Map.\n"); // Structure.
    empty_map->all_tiles = malloc(sizeof(struct tile *[rows][columns]));
    if (!empty_map->all_tiles)
        fprintf(stderr, "FATAL: No more memory to allocate 2-D array of pointers to tiles.\n");
    for (i = 0; i < rows; ++i) {
        for (t = 0; t < columns; ++t) {
            if ((empty_map->all_tiles[i][t] = malloc(sizeof(*empty_map->all_tiles[i][t]))) == NULL)
                fprintf(stderr, "FATAL: No more memory to allocate tiles.\n");    // Tiles.
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
    if (old_map == Map1) {
        if (extra_returner('\\', COMPETED) && !extra_returner('\\', WON)) {
            if (Lobby == NULL) 
                Lobby = initialize_lobby();
            return Lobby;
        }
        else {
            Map2 = next_map = initialize_Map2();
            Map1 = NULL;    
        }/*
        Map2 = next_map = initialize_Map2();
        Map1 = NULL;*/
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
        if (extra_returner('\\', COMPETED) && !extra_returner('\\', WON))
            Lobby = next_map = initialize_lobby();
        else 
            Map8 = next_map = initialize_Map8();
        Map7 = NULL;
    }
    else if (old_map == Lobby) {
        if (extra_returner('\\', TRAINED) || extra_returner('\\', FOUGHT) || extra_returner('\\', DUEL) || extra_returner('\\', HALTED))
            Map7 = next_map = initialize_Map7();
        else {
            if (Jungle == NULL)
                Jungle = initialize_jungle();
            return Jungle;
        }
    }
    else if (old_map == Jungle) {
        if (extra_returner('\\', TRAINED) || extra_returner('\\', FOUGHT) || extra_returner('\\', DUEL) || extra_returner('\\', HALTED))
            Map7 = next_map = initialize_Map7();
        else {
            if (Cliff == NULL)
                Cliff = initialize_cliff();
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
            if (Lobby == NULL)
                Lobby = initialize_lobby();
            return Lobby;
        }
    }
    free_enemies(old_map);
    free_map(old_map);

    return next_map;
}

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
    for (i = 0; i < MAX_ENEMIES; ++i) { /* free current map's enemies */
        if (curr_map->enemy[i] == enemy) {
            free(curr_map->enemy[i]->name);
            curr_map->enemy[i] = NULL;
            free(curr_map->enemy[i]);
            for (t = i + 1; t < MAX_ENEMIES; ++t) { // Left shift all enemies after the enemy that free called
                if (curr_map->enemy[t]) {
                    curr_map->enemy[t - 1] = curr_map->enemy[t];
                }
            }
            curr_map->enemy[t] = NULL;
        }
    }

    return;
}

void free_enemies(struct Map *curr_map) {
    int i;
    for (i = 0; i < MAX_ENEMIES && curr_map->enemy[i]; i++) {
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

struct Map *initialize_Map8(void) {
    struct Map *empty_map = NULL;
    empty_map = initialize_map(MAP8Y, MAX_COLUMNS);
    return empty_map;
}

void set_common_symbols(struct Map *curr_map, int *indices, int num_length, char common_symbol, int set_traversable, int set_entry, int set_occupied) {
    int row, col;
    if (!num_length || !indices)
        return;
    for (int i = 0; i < num_length; i++) {
        curr_map->all_tiles[row = indices[i] / curr_map->columns][col = indices[i] - curr_map->columns * row]->symbol = common_symbol;
        set_prop_vals(curr_map->all_tiles[row][col], set_traversable, set_entry, set_occupied);
    }
}

struct Map *initialize_jungle(void) {
    int i, t;
    struct Map *empty_map = NULL;
    empty_map = initialize_map(JUNGLE_Y, MAX_COLUMNS);
    if (fate_returner('#') != CONVICTED)
        allocate_enemy(empty_map, empty_map->all_tiles[FERAL_y][FERAL_x], "Chike", FERAL_hp, FERAL_hp, FERAL_attack, FERAL, '#');
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
    if ((common_tiles = malloc(19 * sizeof(common_tiles))) == NULL)
        fprintf(stderr, "FATAL: No memory for common tile symbols.\n");
    common_tiles[0] = 27, common_tiles[1] = 30, common_tiles[2] = 33, common_tiles[3] = 36, common_tiles[4] = 79, common_tiles[5] = 82, common_tiles[6] = 85, common_tiles[7] = 88, common_tiles[8] = 55, common_tiles[9] = 58, common_tiles[10] = 61, common_tiles[11] = 106, common_tiles[12] = 109, common_tiles[13] = 112, common_tiles[14] = 46, common_tiles[15] = 66, common_tiles[16] = 101, common_tiles[17] = 124, common_tiles[18] = 115;
    set_common_symbols(empty_map, common_tiles, 19, '{', FALSE, FALSE, FALSE);

    for (i = 0; i < 18; i++)
        common_tiles[i]++;
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
    if (fate_returner('&') != CONVICTED)
        allocate_enemy(empty_map, empty_map->all_tiles[ARTIST_y][ARTIST_x], "Faiza", ARTIST_hp, ARTIST_hp, ARTIST_attack, ARTIST, '&');
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
    if (fate_returner('/') != SPARED)
        allocate_enemy(empty_map, empty_map->all_tiles[HACKER_y][HACKER_x], "Lachlan", HACKER_hp, HACKER_hp, HACKER_attack, HACKER, '/');
    if (fate_returner('!') != CONVICTED)
        allocate_enemy(empty_map, empty_map->all_tiles[POLITICIAN_y][POLITICIAN_x], "Nicolau", POLITICIAN_hp, POLITICIAN_hp, POLITICIAN_attack, POLITICIAN, '!');
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
    if (fate_returner('\\') == NEVER_MET)
        allocate_enemy(empty_map, empty_map->all_tiles[6][1], "Matilda", ASSASSIN_hp, ASSASSIN_hp, ASSASSIN_attack, ASSASSIN, '\\');
    if (extra_returner('M', RETURNING) && fate_returner('M') == SPARED)
        allocate_enemy(empty_map, empty_map->all_tiles[9][10], "Lille", QUEEN_hp, QUEEN_hp, QUEEN_attack, QUEEN, 'M');
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
    allocate_enemy(empty_map, empty_map->all_tiles[5][4], "Grischa", BAT_hp, BAT_hp, BAT_attack, BAT, '@');
    return empty_map;
}

struct Map *initialize_Map5(void) {
    struct Map *empty_map = NULL;
    empty_map = initialize_map(MAP5Y, MAX_COLUMNS);
    allocate_enemy(empty_map, empty_map->all_tiles[FRAUDSTER_y][FRAUDSTER_x], "Dalia", FRAUDSTER_hp, FRAUDSTER_hp, FRAUDSTER_attack, FRAUDSTER, '?');
    allocate_enemy(empty_map, empty_map->all_tiles[NAPPER_y][NAPPER_x], "Lucinda", NAPPER_hp, NAPPER_hp, NAPPER_attack, NAPPER, 'C');
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
    allocate_enemy(empty_map, empty_map->all_tiles[LUNCHLADY_y][LUNCHLADY_x], "Lexa", LUNCHLADY_hp, LUNCHLADY_hp, LUNCHLADY_attack, LUNCHLADY, 'L');
    if (!extra_returner('+', ALERTED) || fate_returner('+') == CONVICTED)
        allocate_enemy(empty_map, empty_map->all_tiles[ARSONIST_y][ARSONIST_x], "Inigo", ARSONIST_hp, ARSONIST_hp, ARSONIST_attack, ARSONIST, '*');
    else 
        extra_store('*', TRAILED);
    return empty_map;
}
struct Map *initialize_Map3(void) {
    struct Map *empty_map = NULL;
    empty_map = initialize_map(MAP3Y, MAX_COLUMNS);
    allocate_enemy(empty_map, empty_map->all_tiles[LORD_y][LORD_x], "Severino", LORD_hp, LORD_hp, LORD_attack, LORD, '%');
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
    allocate_enemy(empty_map, empty_map->all_tiles[DOC_y][DOC_x], "Hayato", DOC_hp, DOC_hp, DOC_attack, DOC, '+');
    allocate_enemy(empty_map, empty_map->all_tiles[QUEEN_y][QUEEN_x], "Lille", QUEEN_hp, QUEEN_hp, QUEEN_attack, QUEEN, 'M');
    return empty_map;
}

/* allocate_enemy : assigns values to a pointer to a Character in enemies */
void allocate_enemy(struct Map *new_map, struct tile *posn, char *name, int hp, int max_hp, unsigned int atk, int karma_id, char representation) {
    int i;
    // Fixes ~i~ to the index of the first NULL ~*Character~ in ~new_map~; if no NULL ~*Character~ is found, ~i~ is fixed to ~MAX_ENEMIES~
    for (i = 0; new_map->enemy[i] != NULL && i < MAX_ENEMIES; ++i)
        ;
    if ((new_map->enemy[i] = malloc(sizeof(*new_map->enemy[i]))) == NULL)
        fprintf(stderr, "FATAL: No more memory to allocate an enemy.\n");
    else {
        new_map->enemy[i]->name = str_dup(name), new_map->enemy[i]->attack = atk, new_map->enemy[i]->health = hp, new_map->enemy[i]->max_health = max_hp;
        new_map->enemy[i]->karma = karma_id, new_map->enemy[i]->occupied = posn, new_map->enemy[i]->rep = representation;
        new_map->enemy[i]->buff = NONE, new_map->enemy[i]->debuff = NONE;
        posn->prop.is_occupied = TRUE;
        if (i + 1 < MAX_ENEMIES)
            new_map->enemy[i + 1] = NULL;
    }
}

/* next_map : draws out the next map */
struct Map *find_next_map(struct Map *curr_map) {
    int i, hor, ver;
    char line[MAX_COLUMNS + 1];
    struct Map *next_map_ptr = NULL;
    struct Character *temp_char = NULL;
    if (curr_map->player->occupied->prop.is_entry_map) {
        temp_char = curr_map->player;
        // Check if the current map is the lobby - if yes, heals for more because there's free food there (Acts like hub of game)
        if (Lobby != NULL && curr_map == Lobby)
            temp_char->health = minimum(temp_char->max_health, temp_char->health += 15);    
        else
            temp_char->health = minimum(temp_char->max_health, temp_char->health += 5);
        next_map_ptr = load_map(curr_map);
        next_map_ptr->player = temp_char;
        temp_char->karma += 6;
        move_char(next_map_ptr, next_map_ptr->all_tiles[next_map_ptr->rows - 2][next_map_ptr->columns / 2]);
        curr_map = next_map_ptr;
    }
    for (ver = 0; ver < curr_map->rows; ++ver) {   // Pointer to tile exists?
        for (hor = 0; hor < curr_map->columns; ++hor) {  // Tile exists?
            if (curr_map->all_tiles[ver][hor]->prop.is_occupied)  // Tile is occupied by a Character: search for it.
                line[hor] = identify_rep(curr_map->all_tiles[ver][hor], curr_map->player, curr_map->enemy)->rep;
            else
                line[hor] = curr_map->all_tiles[ver][hor]->symbol; // Else, it has normal representation.
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
    if (return_ptr = curr_map->all_tiles[ver][hor])
        return return_ptr;
    return NULL;
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
    ui(map_ptr->player);
    set_state_vals(Controller, FALSE, FALSE, TRUE, FALSE, FALSE);
    while ((keystr = getaction(Controller)) != _EOF) {
        if (process_movement(map_ptr, keystr)) {
            clrscr();
            map_ptr = find_next_map(map_ptr);
            ui(map_ptr->player);
            set_state_vals(Controller, FALSE, FALSE, TRUE, FALSE, FALSE);
        }
    }
    quit();
}

void swap_enemy(struct Map *map_ptr, char rep1, char rep2) {
    int i, first = -1, second = -1;
    struct Character *temp = NULL;
    if (rep1 == rep2)
        return;
    else {
        for (i = 0; i < MAX_ENEMIES && map_ptr->enemy[i]; i++) {
            if (rep1 == map_ptr->enemy[i]->rep)
                first = i;
            else if (rep2 == map_ptr->enemy[i]->rep)
                second = i;
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
        if (enemy_rep == '%')
            allocate_enemy(curr_map, curr_map->all_tiles[STOWAWAY_y][STOWAWAY_x], "Nemo", STOWAWAY_hp, STOWAWAY_hp, STOWAWAY_attack, STOWAWAY, '$');
        // 2. Defeating the LUNCHLADY when the ARSONIST has been hired by the DOC will cause a fight with the ARSONIST (as if the ARSONIST has seen a perfect opportunity to get rid of his targets) 
        else if (enemy_rep == 'L' && extra_returner('*', TRAILED)) {
            allocate_enemy(curr_map, curr_map->all_tiles[4][9], "Inigo", ARSONIST_hp, ARSONIST_hp, ARSONIST_attack, ARSONIST, '*');
            next_tile = curr_map->all_tiles[4][9];
        }
        // 3. Defeating the BAT when the ARSONIST has been hired by the DOC will cause a fight with the ARSONIST (as if the ARSONIST has seen a perfect opportunity to get rid of his targets)
        else if (enemy_rep == '@' && extra_returner('*', TRAILED) && fate_returner('*') != CONVICTED) {
            allocate_enemy(curr_map, curr_map->all_tiles[BAT_y - 3][BAT_x], "Inigo", ARSONIST_hp, ARSONIST_hp, ARSONIST_attack, ARSONIST, '*');
            next_tile = curr_map->all_tiles[BAT_y - 3][BAT_x];
            curr_map->all_tiles[2][4]->symbol = ' ';
            set_prop_vals(curr_map->all_tiles[2][4], TRUE, FALSE, FALSE);
        }
        // 4. Defeating the last remaining criminal on the island will cause dialogue with the Australian siblings (as if the user has won their game)
        else if (enemy_rep == '!' || enemy_rep == '#' || enemy_rep == '&') {
            if ((fate_returner('!') == CONVICTED) && (fate_returner('#') == CONVICTED) && (fate_returner('&') == CONVICTED)) {
                extra_store('\\', WON);
                allocate_enemy(curr_map, curr_map->all_tiles[0][MAX_COLUMNS / 2], "Matilda", ASSASSIN_hp, ASSASSIN_hp, ASSASSIN_attack, ASSASSIN, '\\');
                next_tile = curr_map->all_tiles[0][MAX_COLUMNS / 2];
            }
        }
        // 5. Defeating the ASSASSIN when the user has chosen a fight with the Australian siblings will cause a fight with the HACKER (as if the event is a boss rush)
        else if (enemy_rep == '\\' && extra_returner('\\', FOUGHT)) {
            allocate_enemy(curr_map, curr_map->all_tiles[0][MAX_COLUMNS / 2], "Lachlan", HACKER_hp, HACKER_hp, HACKER_attack, HACKER, '/');
            next_tile = curr_map->all_tiles[0][MAX_COLUMNS / 2];
        }
    return next_tile;
}

/* trigger_battle : formats the battle screen, receives battle input; can trigger game over or victory */
void trigger_battle(struct Map *map_ptr, struct Character *enemy_ptr, int turn_no) {
    static int player_strength;
    struct item *selected_item;
    struct tile *temp = NULL;
    char player_status[WIDTH], *player_buff, *player_debuff, enemy_status[WIDTH], *enemy_buff, *enemy_debuff;
    // Save permanent stats of the player on first turn
    if (turn_no == 1) {
        player_strength = map_ptr->player->attack;
        if (enemy_ptr->rep == '#')
            debuff_char(enemy_ptr, map_ptr->player, RESTRAINED);
    }
    player_buff = buff_status(map_ptr->player, enemy_ptr);
    player_debuff = debuff_status(map_ptr->player, enemy_ptr);
    enemy_buff = buff_status(enemy_ptr, map_ptr->player);
    enemy_debuff = debuff_status(enemy_ptr, map_ptr->player);
    if (map_ptr->player->health <= 0) // lost the battle -> game is over
        player_death();
    /* won the battle -> trigger some event, prevent soft-locking w/ second condition */
    else if (enemy_ptr->health <= 0 || enemy_ptr->health < 3 && enemy_ptr->buff == REGEN) {
        #if WINDOWS_MUSIC
        PlaySound(NULL, NULL, 0);
        #endif
        map_ptr->player->attack = player_strength;
        finished(map_ptr->player, first_item, enemy_ptr);
        identify_tile(map_ptr, enemy_ptr->occupied->y, enemy_ptr->occupied->x)->prop.is_occupied = FALSE;
        map_ptr->player->buff = NONE, map_ptr->player->debuff = NONE;
        if (DEBUG) {
            printf("POP trigger_battle\n");
            sleep(1);
        }
        return;
    }
    sprintf(player_status, "%s %s", player_buff, player_debuff);
    sprintf(enemy_status, "%s %s", enemy_buff, enemy_debuff);
    clrscr();
    int keystr;
    ui(enemy_ptr);
    center_screen(WIDTH, "%c\n\n", enemy_ptr->rep);
    center_screen(WIDTH, "%c\n", map_ptr->player->rep);
    ui(map_ptr->player);
    center_screen(WIDTH, "%s\n%s\n", player_status, enemy_status);
    center_screen(WIDTH, "%s\n", "r : Attack / i : Inventory");
    set_state_vals(Controller, FALSE, FALSE, FALSE, TRUE, FALSE);
    while ((keystr = getaction(Controller)) != _EOF) {
        if (map_ptr->player->debuff == RESTRAINED || map_ptr->player->debuff == ASLEEP) {
            center_screen(WIDTH, "%s\n", "You can't fight back!");
            if (map_ptr->player->buff == SUMMON) {
                center_screen(WIDTH, "%s\n", "Yosef slams his fist down!");
                enemy_ptr->health -= map_ptr->player->ally_attack;
            }
            if (enemy_ptr->health > 0)
                (*enemy_ptr->ai)(map_ptr->player, enemy_ptr, turn_no);
            trigger_battle(map_ptr, enemy_ptr, ++turn_no);

            return;
        }
        switch (keystr) {
            case Select:
                set_state_vals(Controller, TRUE, FALSE, FALSE, TRUE, FALSE);
                damage_dealt(map_ptr->player, enemy_ptr, map_ptr->player->attack);
                if (map_ptr->player->buff == SUMMON) {
                    center_screen(WIDTH, "%s\n", "Yosef slams his fist down!");
                    enemy_ptr->health -= map_ptr->player->ally_attack;
                    sleep(2);
                }
                if (enemy_ptr->health > 0)
                    (*enemy_ptr->ai)(map_ptr->player, enemy_ptr, turn_no);
                trigger_battle(map_ptr, enemy_ptr, ++turn_no);

                return;
            case Inventory:
                if (map_ptr->player->debuff != ITEMLESS) {
                    item_turn(inventory_screen(first_item, map_ptr, enemy_ptr, turn_no), map_ptr, enemy_ptr, turn_no);
                    return; // Since item_turn and inventory_screen call trigger_battle again, need to return
                }
                break;
        }
    }
    quit();
}

/* inventory_screen : displays the current item selected */
struct item *inventory_screen(struct item *first_item_slot, struct Map *map_ptr, struct Character *enemy_ptr, int turn_no) {
    int keystr = Placeholder;
    struct item *selected_item = first_item_slot;
    do {
        switch (keystr) {
            case Right:
                selected_item = selected_item->next_item;
                break;
            case Inventory:
                trigger_battle(map_ptr, enemy_ptr, turn_no);
                break;
            case Select:
                if (!selected_item->uses) {
                    center_screen(WIDTH, "%s\n", "No more item uses.");
                    sleep(1);
                    break;
                }
                if (map_ptr->player->karma >= selected_item->karma_cost) {
                    if (DEBUG) {
                        printf("POP inventory_screen\n");
                        sleep(1);
                    }
                    return selected_item;
                }
                else {
                    center_screen(WIDTH, "%s\n", "Not enough karma.");
                    sleep(1);
                }
                break;
        }
        if (selected_item == NULL) /* move back to the first item (DAMAGE) when last item has been displayed and player inputs 'd' */
            selected_item = first_item_slot;
        clrscr();
        puts(selected_item->image);
        if (selected_item->uses != INFINITE) /* instead of [x karma], we have finite uses to be displayed [x uses] */
            printf("[%d uses]\n", selected_item->uses);
        puts(selected_item->description);
        center_screen(WIDTH, "%s\n", "r : Use item / d : Next item / i : Exit inventory");
    } while ((keystr = getaction(Controller)) != _EOF);
    quit();
}

/* item_turn : activate the item effect, has the enemy act afterward if it can */
void item_turn(struct item *item_used, struct Map *map_ptr, struct Character *enemy_ptr, int turn_no) {
    struct item *item_scanned;
    int amt;
    set_state_vals(Controller, TRUE, FALSE, FALSE, TRUE, FALSE);
    map_ptr->player->karma -= item_used->karma_cost;
    switch (item_used->effect) {
        case DAMAGE:
            center_screen(WIDTH, "%s\n", "You release Vijaya's mercy!");
            damage_dealt(map_ptr->player, enemy_ptr, 2 * map_ptr->player->attack);
            break;
        case RUSH:
            center_screen(WIDTH, "%s\n", "You feel the rush of combat!");
            buff_char(map_ptr->player, enemy_ptr, RUSHING);
            break;
        case HEAL:
            center_screen(WIDTH, "%s\n", "You slurp on bull's blood!");
            health_healed(map_ptr->player, enemy_ptr, (map_ptr->player->max_health - map_ptr->player->health) / 2);
            break;
        case GUARD:
            center_screen(WIDTH, "%s\n", "You hunker down with Ancile!");
            buff_char(map_ptr->player, enemy_ptr, GUARDED);
            break;
        case RECOVER:
            --item_used->uses;
            center_screen(WIDTH, "%s\n", "You take a swig of Centerba!");
            health_healed(map_ptr->player, enemy_ptr, 3);
            debuff_char(enemy_ptr, map_ptr->player, NONE);
            break;
        case TRICK:
            if (enemy_ptr->buff != NONE) {
                center_screen(WIDTH, "%s\n", "Your body disappears as you start your heist!");
                buff_char(map_ptr->player, enemy_ptr, INVULNERABLE);
            }
            else
                center_screen(WIDTH, "%s\n", "Nothing happens.");
            break;
        case FIRE:
            center_screen(WIDTH, "%s\n", "You pierce and ignite with Agneyastra.");
            debuff_char(map_ptr->player, enemy_ptr, BURNING);
            break;
        case FEAST:
            switch (rand() % 13) {
                case 0:
                    center_screen(WIDTH, "%s\n", "You took some potato chips and ate them.");
                    break;
                case 1:
                    center_screen(WIDTH, "%s\n", "You chowed down on some Peking duck.");
                    break;
                case 2:
                    center_screen(WIDTH, "%s\n", "You dined on a slice of beef wellington.");
                    break;
                case 3:
                    center_screen(WIDTH, "%s\n", "You snacked on a few buffalo wings.");
                    break;
                case 4:
                    center_screen(WIDTH, "%s\n", "You dined on a fine wagyu steak.");
                    break;
                case 5:
                    center_screen(WIDTH, "%s\n", "You savored various types of sushi.");
                    break;
                case 6:
                    center_screen(WIDTH, "%s\n", "You chowed down on a bunny chow.");
                    break;
                case 7:
                    center_screen(WIDTH, "%s\n", "You snacked on seasoned naan bread.");
                    break;
                case 8:
                    center_screen(WIDTH, "%s\n", "You ate a traditional taco.");
                    break;
                case 9:
                    center_screen(WIDTH, "%s\n", "You snacked on a platter of poutine.");
                    break;
                case 10:
                    center_screen(WIDTH, "%s\n", "You savored some pelmeni dumplings.");
                    break;
                case 11:
                    center_screen(WIDTH, "%s\n", "You consumed a plate of fresh ceviche.");
                    break;
                case 12:
                    center_screen(WIDTH, "%s\n", "You consumed sweet pavlova.");
                    break;
            }
            buff_char(map_ptr->player, enemy_ptr, FULL);
            break;
        case MOON:
            center_screen(WIDTH, "%s\n", "You harness the power from the cosmos into your body.");
            buff_char(map_ptr->player, enemy_ptr, REGEN);
            break;
        case JUDGEMENT:
            printf("How much health points will you use?\n");
            while ((amt = getnumber()) == _EOF)
                ;
            center_screen(WIDTH, "%s\n", "You unleash lightning bolts from the cosmos.");
            map_ptr->player->health -= amt;
            enemy_ptr->health -= amt;
            sleep(1);
            break;
        case TEMPER:
            center_screen(WIDTH, "%s\n", "You activate the power from the cosmos onto the battlefield.");
            buff_char(map_ptr->player, enemy_ptr, NONE), debuff_char(enemy_ptr, map_ptr->player, NONE);
            buff_char(enemy_ptr, map_ptr->player, NONE), debuff_char(map_ptr->player, enemy_ptr, NONE);
            break;
        case STRENGTH:
            center_screen(WIDTH, "%s\n", "You spread the power from the cosmos onto the battlefield.");
            ++map_ptr->player->attack;
            ++enemy_ptr->attack;
            break;
        case GOLEM:
            map_ptr->player->health -= 15;
            center_screen(WIDTH, "%s\n", "You give the doll life as a golem!");
            map_ptr->player->ally_health = 15;
            map_ptr->player->ally_attack = 3;
            buff_char(map_ptr->player, enemy_ptr, SUMMON);
            break;
        case RANG:
            --item_used->uses;
            center_screen(WIDTH, "%s\n", "You flung a batarang, nailing your target!");
            damage_dealt(map_ptr->player, enemy_ptr, map_ptr->player->attack);
            buff_char(map_ptr->player, enemy_ptr, HEROIC);
            break;
        case BULLET:
            --item_used->uses;
            center_screen(WIDTH, "%s\n", "You shot a silver bullet right into your target!");
            damage_dealt(map_ptr->player, enemy_ptr, map_ptr->player->attack);
            buff_char(map_ptr->player, enemy_ptr, TRIPLESHOT);
            break;
        case MILK:
            --item_used->uses;
            printf("What will you do? (0 / 1 = Buff / Debuff)\n");
            while ((amt = getnumber()) == _EOF || amt != 0 && amt != 1)
                ;
            if (!amt) {
                center_screen(WIDTH, "%s\n", "You drink the pot of milk, boosting your power!");
                buff_char(map_ptr->player, enemy_ptr, RIVALED);
            }
            else {
                center_screen(WIDTH, "%s\n", "You hurl the pot onto the ground, spreading sharp shards around your enemy.");
                debuff_char(map_ptr->player, enemy_ptr, SPIKES);
            }
            break;
        case DRAIN:
            center_screen(WIDTH, "%s\n", "You turn the Eye of Horus toward your foe.");
            damage_dealt(map_ptr->player, enemy_ptr, amt = map_ptr->player->attack / 2);
            health_healed(map_ptr->player, enemy_ptr, amt);
            break;
        case UNCENSOR:
            center_screen(WIDTH, "%s\n", "You reveal their true form with Hera's Pen!");
            enemy_ptr->health -= 5;
            buff_char(enemy_ptr, map_ptr->player, NONE);
            break;
        case ALCHEMY:
            map_ptr->player->health -= 5;
            center_screen(WIDTH, "%s\n", "You drew the Philosophers' Stone and channeled your energy to it!");
            for (item_scanned = first_item; item_scanned != NULL; item_scanned = item_scanned->next_item) {
                if (item_scanned->uses != INFINITE && item_scanned->uses != 0) {
                    switch (item_scanned->effect) {
                        case RECOVER:
                            if (item_scanned->uses != RECOVER_USES) {
                                ++item_scanned->uses;
                                center_screen(WIDTH, "%s\n", "Your bottle of Centerba was refilled slightly.");
                            }
                            break;
                        case RANG:
                            if (item_scanned->uses != RANG_USES) {
                                ++item_scanned->uses;
                                center_screen(WIDTH, "%s\n", "A batarang was created out of the air.");
                            }
                            break;
                        case BULLET:
                            if (item_scanned->uses != BULLET_USES) {
                                ++item_scanned->uses;
                                center_screen(WIDTH, "%s\n", "A silver bullet was made out of the ground.");
                            }
                            break;
                        case MILK:
                            if (item_scanned->uses != MILK_USES) {
                                ++item_scanned->uses;
                                center_screen(WIDTH, "%s\n", "Another pot of milk was prepared instantly.");
                            }
                            break;
                    }
                    sleep(1);
                }
            }
            break;
        case DUE_RECOMPENSE:
            --item_used->uses;
            map_ptr->player->karma += 50;
            health_healed(map_ptr->player, enemy_ptr, map_ptr->player->max_health);
            center_screen(WIDTH, "%s\n", "You pay your due recompense.");
            break;
    }
    if (map_ptr->player->buff == SUMMON) {
        center_screen(WIDTH, "%s\n", "Yosef slams his fist down!");
        enemy_ptr->health -= map_ptr->player->ally_attack;
    }
    if (enemy_ptr->health > 0)
        (*enemy_ptr->ai)(map_ptr->player, enemy_ptr, turn_no);
    if (map_ptr->player->buff == INVULNERABLE) {
        center_screen(WIDTH, "%s\n", "The heist has finished, and your body is revealed!");
        map_ptr->player->buff = enemy_ptr->buff;
        enemy_ptr->buff = NONE;
        sleep(2);
    }
    trigger_battle(map_ptr, enemy_ptr, ++turn_no);
    if (DEBUG) {
        printf("POP item_turn\n");
        sleep(1);
    }
    return;
}