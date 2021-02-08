/* character.h
Functions that implement every character in the game.
Also implements the fundamentals of the battle system.
*/

#include <stdio.h>
#include <stdlib.h>
#include "character_lines.c"
#include "fate.h"
#include "general.h"
#include "controller.h"
#include "rockpaperscissors.h"
#include "tictactoe.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

#define INIT_HEALTH 45  // Default starting maximum health of player
#define INIT_ATTACK 2   // Default starting attack of player
#define MAX_ENEMIES 3   // Maximum number of enemies in a map 
#define INIT_KARMA 50   // Default starting karma of player

enum max_health {   // These are the starting maximum health of each Character
    QUEEN_hp = 14,
    KID_hp = 17,
    ARSONIST_hp = 20,
    FRAUDSTER_hp = 23,
    STOWAWAY_hp = 26,
    NAPPER_hp = 27,
    HACKER_hp = 28,
    LORD_hp = 29,
    DOC_hp = 30,
    ARTIST_hp = 30,
    FERAL_hp = 31,
    LUNCHLADY_hp = 32,
    BAT_hp = 33,
    ASSASSIN_hp = 33,
    POLITICIAN_hp = 35,
    COP_hp = 40,
    JUDGE_hp = 65,
};

enum init_attack {  // These are the starting attack of each Character (first attack)
    KID_attack = 1,
    ARTIST_attack = 1,
    ARSONIST_attack = 2,
    COP_attack = 2,
    QUEEN_attack = 3,
    DOC_attack = 3,
    LORD_attack = 3,
    STOWAWAY_attack = 3,
    FRAUDSTER_attack = 3,
    FERAL_attack = 3,
    LUNCHLADY_attack = 4,
    NAPPER_attack = 4,
    POLITICIAN_attack = 4,
    BAT_attack = 5,
    ASSASSIN_attack = 6,
    JUDGE_attack = 7,
    HACKER_attack = 10,
};

enum buffs {    // These are the integers that correspond to buffs 
    INVULNERABLE = 1, 
    RUSHING, 
    GUARDED, 
    ENLIGHTENED, 
    HEROIC, 
    FULL, 
    REGEN, 
    SUMMON, 
    DOUBLESHOT, 
    TRIPLESHOT, 
    CONFIDENT, 
    RIVALED, 
    PIERCE
};

enum debuff_or_ailment {    // These are the integers that correspond to debuffs/ailments
    ACIDIFIED = 1, 
    RESTRAINED, 
    ITEMLESS, 
    BURNING, 
    HUNGRY, 
    ALMOST_SICK, 
    SICK, 
    MELTING, 
    UNLUCKY, 
    DOUBLEOPEN, 
    SPIKES, 
    DOZING, 
    ASLEEP
};

/* 
BUFF | DEBUFF for each Character
KID:        RUSHING |
DOC:         |
QUEEN:      GUARDED | ACIDIFIED
LORD:       ENLIGHTENED, GUARDED, RUSHING | RESTRAINED
STOWAWAY:   *INVULNERABLE | *ITEMLESS
ARSONIST:   *HEROIC | BURNING, *MELTING
LUNCHLADY:  FULL | *HUNGRY, *ALMOST_SICK, *SICK
FRAUDSTER:  REGEN, FULL, ENLIGHTENED, RUSHING | *UNLUCKY, RESTRAINED, ACIDIFIED, BURNING
NAPPER:     SUMMON, REGEN, GUARDED | 
BAT:         | RESTRAINED
ASSASSIN:   DOUBLESHOT, TRIPLESHOT, GUARDED |
FERAL:      *CONFIDENT | RESTRAINED, *DOUBLEOPEN
ARTIST:     RUSHING, GUARDED, ENLIGHTENED, FULL, REGEN, SUMMON, DOUBLESHOT, TRIPLESHOT | ACIDIFIED
POLITICIAN: *RIVALRY | 
HACKER:     *PIERCE | *DOZING, *ASLEEP
COP:         |
JUDGE:       | *DEATH
*/

struct tile_prop {                      // Describes a tile (position on the map where character can traverse)
    unsigned int is_traversable : 1;    // Describes whether a tile can be replaced by a Character.
    unsigned int is_entry_map   : 1;    // Describes whether a tile is an entry point to another map.
    unsigned int is_occupied    : 1;    // Describes whether a tile is currently replaced by a Character.
};
struct tile {               // Describes a tile's primary details (position, appearance, properties)
    unsigned int x;         // Horizontal spot of an array
    unsigned int y;         // Vertical spot of an array
    char symbol;            // The character that symbolizes the tile (lower priority)
    struct tile_prop prop;  // Description of what the tile can do
};
struct Character {
    char *name;                     
    int health;                 // Amount that can be sustained from the opposite Character's attack
    int max_health;             // Maximum amount that can be sustained from the opposite Character's attacks
    int ally_health;            // Current health of ally (needs buff to verify ally exists)
    unsigned int attack;        // Amount that can be subtracted from the opposite Character's health
    unsigned int ally_attack;   // Flat amount that ally can inflict on opposite Character's health
    int karma;                  // Measure of secondary resource karma, which is used for specific events
    char rep;                   // char that is representative of the Character (higher priority)
    unsigned int buff;          // Integer value that corresponds to a defined buff
    unsigned int debuff;        // Integer value that corresponds to a defined debuff
    struct tile *occupied;      // Describes what tile the Character is on
    void (*ai) (struct Character *, struct Character *, int);   // Points to function that defines the Character's behavior
};

int skip_scene = FALSE; // Tracks whether the player has chosen to skip a scene

struct Character *initialize_player(void);
void set_prop_vals(struct tile *tile_ptr, int new_is_traversable, int new_is_entry_map, int new_is_occupied);
struct Character *identify_rep(struct tile *tile_ptr, struct Character *player_ptr, struct Character *enemy_ptr[MAX_ENEMIES]);

int ui(struct Character *char_ptr);
int karma_decay(struct Character *player_ptr);
void trigger_recompense(void);
void damage_dealt(struct Character *char1, struct Character *char2, int damage);
void health_healed(struct Character *char1, struct Character *char2, int heal);

char *debuff_status(struct Character *debuffed_char, struct Character *char2);
char *buff_status(struct Character *buffed_char, struct Character *char2);
void debuff_char(struct Character *char1, struct Character *char2, int debuff_no);
void buff_char(struct Character *char1, struct Character *char2, int buff_no);
int intro(struct Character *enemy_ptr, struct Character *player_ptr);
void decline(struct Character *enemy_ptr, struct Character *player_ptr);
void finished(struct Character *player_ptr, struct item *first_item_ptr, struct Character *enemy_ptr);
void character_line(char rep, char *line);

/* enemy turn-based battle behavior */
void KID_behavior(struct Character *player_ptr, struct Character *KID_ptr, int turn_no);
void DOC_behavior(struct Character *player_ptr, struct Character *DOC_ptr, int turn_no);
void QUEEN_behavior(struct Character *player_ptr, struct Character *QUEEN_ptr, int turn_no);
void LORD_behavior(struct Character *player_ptr, struct Character *LORD_ptr, int turn_no);
void STOWAWAY_behavior(struct Character *player_ptr, struct Character *STOWAWAY_ptr, int turn_no);
void ARSONIST_behavior(struct Character *player_ptr, struct Character *ARSONIST_ptr, int turn_no);
void LUNCHLADY_behavior(struct Character *player_ptr, struct Character *LUNCHLADY_ptr, int turn_no);
void FRAUDSTER_behavior(struct Character *player_ptr, struct Character *FRAUDSTER, int turn_no);
void NAPPER_behavior(struct Character *player_ptr, struct Character *NAPPER_ptr, int turn_no);
void BAT_behavior(struct Character *player_ptr, struct Character *BAT_ptr, int turn_no);
void ASSASSIN_behavior(struct Character *player_ptr, struct Character *ASSASSIN_ptr, int turn_no);
void HACKER_behavior(struct Character *player_ptr, struct Character *HACKER_ptr, int turn_no);
void JUDGE_behavior(struct Character *player_ptr, struct Character *JUDGE_ptr, int turn_no);

/* KID_behavior : Weak fighter */
void KID_behavior(struct Character *player_ptr, struct Character *KID_ptr, int turn_no) {
    char message[55];
    buff_char(KID_ptr, player_ptr, RUSHING);
    switch (turn_no % 4) {
        case 0:
            sprintf(message, "Helix rushes in and stabs your %s!", (turn_no < 4) ? "right leg" : "right arm");
            center_screen(WIDTH, "%s\n", message);
            damage_dealt(KID_ptr, player_ptr, KID_ptr->attack);
            KID_ptr->attack /= 3;
            break;
        case 1:
            sprintf(message, "Helix recklessly %s your way!", (turn_no > 1) ? "throws a mean hook" : "throws an uppercut");
            center_screen(WIDTH, "%s\n", message);
            damage_dealt(KID_ptr, player_ptr, KID_ptr->attack);
            break;
        case 2:
            center_screen(WIDTH, "%s\n", "Helix licks his wounds, revealing his endowed tongue.");
            health_healed(KID_ptr, player_ptr, 1);
            if (turn_no == 2) 
                center_screen(WIDTH, "%s\n", "Helix stops to grab his phone from his right pocket with his right hand.");
            else if (turn_no == 6) {
                center_screen(WIDTH, "%s\n", "Helix quickly taps on his phone.");
                center_screen(WIDTH, "%s\n", "Could it be a final message before his judgment?");
                sleep(1);
                extra_store('+', ALERTED);
            }
            sleep(1);
            break;
        case 3:
            center_screen(WIDTH, "%s\n", "Helix readies his knife in his left hand...");
            KID_ptr->attack *= 3;
            break;
    }
    sleep(3);
}

/* DOC_behavior : Increases own attack while decreasing player's attack */
void DOC_behavior(struct Character *player_ptr, struct Character *DOC_ptr, int turn_no) {
    int operation_commence = FALSE;
    static int injected = FALSE;
    switch (turn_no % 4) {
        case 0:
            if (!operation_commence && injected) {
                center_screen(WIDTH, "%s\n", "Arata injects some strange catalyst into his arms.");
                DOC_ptr->attack *= 2;
                injected = FALSE;
            }
            if (DOC_ptr->health <= 8 && !operation_commence) {
                center_screen(WIDTH, "%s\n", "Arata fled the room but soon returns with a surgery kit. What's he planning?");
                operation_commence = TRUE;
            }
            else {
                center_screen(WIDTH, "%s\n", "Arata's biceps surge as he looks on with an evil eye.");
                ++DOC_ptr->attack;
            }
            sleep(1);
            break;
        case 1:
            center_screen(WIDTH, "%s\n", "Arata slashes with his scissors!");
            damage_dealt(DOC_ptr, player_ptr, DOC_ptr->attack);
            --DOC_ptr->attack;
            break;
        case 2:
            if (DOC_ptr->health < 9) {
                center_screen(WIDTH, "%s\n", "Arata slices your dominant hand with his scissors!");
                if (player_ptr->attack > 1) {
                    --player_ptr->attack;
                    center_screen(WIDTH, "%s\n", "Your attack decreased by 1 for this battle.");
                    sleep(1);
                }
            }
            else
                center_screen(WIDTH, "%s\n", "Arata lunges at you with a scalpel."); 
            damage_dealt(DOC_ptr, player_ptr, DOC_ptr->attack);
            ++DOC_ptr->attack;
            if (operation_commence)
                DOC_ptr->attack *= 2;
            break;
        case 3:
            if (!operation_commence && DOC_ptr->health > 10) {
                center_screen(WIDTH, "%s\n", "Arata injects some amber fluid labeled Synthol into his arms.");
                damage_dealt(DOC_ptr, DOC_ptr, 5);
                injected = TRUE;
                extra_store('+', SYNTHOL);
            }
            else if (operation_commence) {
                center_screen(WIDTH, "%s\n", "Arata performs surgery on you at a surprising pace!");
                damage_dealt(DOC_ptr, player_ptr, DOC_ptr->attack);
                DOC_ptr->attack /= 2;
            }
            else {
                center_screen(WIDTH, "%s\n", "Arata takes a dose of Dexedrine.");
                health_healed(DOC_ptr, player_ptr, 5);
            }
            sleep(1);
            break;
    }
    sleep(3);
}

/* QUEEN_behavior : Stalls the fight with acid and guard/heals */
void QUEEN_behavior(struct Character *player_ptr, struct Character *QUEEN_ptr, int turn_no) {
    switch (turn_no % 2) {
        case 0:
            if (QUEEN_ptr->buff == GUARDED) {
                center_screen(WIDTH, "%s\n", "The fire ants break formation, supplying Lille with sustenance!");
                health_healed(QUEEN_ptr, QUEEN_ptr, 3);
                buff_char(QUEEN_ptr, player_ptr, NONE);
                ++QUEEN_ptr->attack;
            }
            else {
                center_screen(WIDTH, "%s\n", "Lille's fire ants form a protective jacket around her!");
                buff_char(QUEEN_ptr, player_ptr, GUARDED);
                --QUEEN_ptr->attack;
            }
            break;
        case 1:
            if (turn_no == 1) {
                center_screen(WIDTH, "%s\n", "Lille opens her leather suitcase, revealing an ant army!");
                sleep(1);
            }
            center_screen(WIDTH, "%s\n", "Fire ants bite into you!");
            center_screen(WIDTH, "%s\n", "You've been injected with acid.");
            damage_dealt(QUEEN_ptr, player_ptr, QUEEN_ptr->attack);
            debuff_char(QUEEN_ptr, player_ptr, ACIDIFIED);
            break;
    }
    sleep(3);
}

/* LORD_behavior : Overwhelm player with defeatist style */
void LORD_behavior(struct Character *player_ptr, struct Character *LORD_ptr, int turn_no) {
    if (LORD_ptr->health > LORD_hp / 1.7) {
        center_screen(WIDTH, "%s\n", "Severino is really into the fight!");
        buff_char(LORD_ptr, player_ptr, ENLIGHTENED);
    }
    else if (LORD_ptr->buff == ENLIGHTENED) {
        center_screen(WIDTH, "%s\n", "Severino is weak and doubting himself!");
        buff_char(LORD_ptr, player_ptr, NONE);
    }
    sleep(1);
    switch (turn_no % 5) {
        case 0:
            if (player_ptr->debuff == RESTRAINED) {
                damage_dealt(LORD_ptr, player_ptr, LORD_ptr->attack);
                center_screen(WIDTH, "%s\n", "You finally manage to break out of Severino's chokehold!");
                LORD_ptr->attack += 2;
                debuff_char(LORD_ptr, player_ptr, NONE);
            }
            else {
                center_screen(WIDTH, "%s\n", "Severino pummels you with the blunt part of his switchblade!");
                damage_dealt(LORD_ptr, player_ptr, LORD_ptr->attack);
                ++LORD_ptr->attack;
            }
            if (LORD_ptr->buff != ENLIGHTENED)
                buff_char(LORD_ptr, player_ptr, RUSHING);
            break;
        case 1:
            center_screen(WIDTH, "%s\n", "Severino slashes with a hidden switchblade!");
            damage_dealt(LORD_ptr, player_ptr, LORD_ptr->attack);
            LORD_ptr->attack -= 2;
            break;
        case 2:
            center_screen(WIDTH, "%s\n", "Severino bobs and weaves, throwing jabs at you!");
            damage_dealt(LORD_ptr, player_ptr, LORD_ptr->attack);
            if (LORD_ptr->buff != ENLIGHTENED)
                buff_char(LORD_ptr, player_ptr, GUARDED);
            break;
        case 3:
            center_screen(WIDTH, "%s\n", "Severino strikes with his elbow and holds you in a choke!");
            damage_dealt(LORD_ptr, player_ptr, LORD_ptr->attack);
            debuff_char(LORD_ptr, player_ptr, RESTRAINED);
            break;
        case 4:
            damage_dealt(LORD_ptr, player_ptr, LORD_ptr->attack);
            if (player_ptr->buff == RUSHING) {
                center_screen(WIDTH, "%s\n", "You manage to break out of Severino's chokehold!");
                ++LORD_ptr->attack;
                debuff_char(LORD_ptr, player_ptr, NONE);
            }
            else
                center_screen(WIDTH, "%s\n", "You struggle to overpower Severino out of the chokehold!");
            break;
    }
    sleep(3);
}

/* STOWAWAY_behavior : Disables item usage early */
void STOWAWAY_behavior(struct Character *player_ptr, struct Character *STOWAWAY_ptr, int turn_no) {
    switch (turn_no % 4) {
        case 0:
            if (turn_no == 4) {
                center_screen(WIDTH, "%s\n", "Nemo seems to have vanished from your gaze.");
                buff_char(STOWAWAY_ptr, player_ptr, INVULNERABLE);
            }
            else {
                center_screen(WIDTH, "%s\n", "Nemo's father seems to have encouraged him.");
                buff_char(STOWAWAY_ptr, player_ptr, DOUBLESHOT);
            }
            break;
        case 1:
            if (turn_no == 5) {
                center_screen(WIDTH, "%s\n", "Nemo pops up again, holding all of your items.");
                debuff_char(STOWAWAY_ptr, player_ptr, ITEMLESS);
                buff_char(STOWAWAY_ptr, player_ptr, NONE);
            }
            else
                center_screen(WIDTH, "%s\n", "Nemo gets ready to make his next move.");
            break;
        case 2:
            center_screen(WIDTH, "%s\n", "Nemo grips his hatchet with both hands, looking straight at you.");
            STOWAWAY_ptr->attack *= 2;
            break;
        case 3:
            center_screen(WIDTH, "%s\n", "Nemo brings his axe down on you swiftly!");
            damage_dealt(STOWAWAY_ptr, player_ptr, STOWAWAY_ptr->attack);
            STOWAWAY_ptr->attack /= 2;
            break;
    }
    sleep(3);
}

/* ARSONIST_behavior : Weak direct attacks, strong damage over time attacks */
void ARSONIST_behavior(struct Character *player_ptr, struct Character *ARSONIST_ptr, int turn_no) {
    int mod_debuff = (extra_returner('*', TRAILED)) ? MELTING : BURNING;
    if (fate_returner('c') == CONVICTED || fate_returner('M') == CONVICTED) {
        buff_char(ARSONIST_ptr, player_ptr, HEROIC);
        center_screen(WIDTH, "%s\n", "Inigo is fired up and raring to fight!");
        sleep(1);
    }
    switch (turn_no % 4) {
        case 0:
            center_screen(WIDTH, "%s\n", "Inigo loads his flamethrower with fluids as he smokes a joint.");
            health_healed(ARSONIST_ptr, player_ptr, 2);
            break;
        case 1:
            center_screen(WIDTH, "%s\n", "Inigo took a molotov cocktail and lit it.");
            ++ARSONIST_ptr->attack;
            break;
        case 2:
            center_screen(WIDTH, "%s\n", "Inigo threw the molly directly at you, causing a small explosion!");
            damage_dealt(ARSONIST_ptr, player_ptr, ARSONIST_ptr->attack);
            debuff_char(ARSONIST_ptr, player_ptr, mod_debuff);
            if (turn_no != 2)
                ++ARSONIST_ptr->attack;
            break;
        case 3:
            if (turn_no == 3)
                center_screen(WIDTH, "%s\n", "Inigo put on his flamethrower!");
            else {
                center_screen(WIDTH, "%s\n", "Inigo fires a stream of flames towards you!");
                damage_dealt(ARSONIST_ptr, player_ptr, ARSONIST_ptr->attack);
                debuff_char(ARSONIST_ptr, player_ptr, mod_debuff);
            }
            --ARSONIST_ptr->attack;
            break;
    }
    sleep(3);
}

/* LUNCHLADY_behavior : Lots of buffs/debuffs/tradeoffs */
void LUNCHLADY_behavior(struct Character *player_ptr, struct Character *LUNCHLADY_ptr, int turn_no) {
    static int served = FALSE;
    int keystr;
    if (served == TRUE && player_ptr->debuff != ALMOST_SICK && player_ptr->debuff != SICK)
        served = FALSE;
    if (player_ptr->debuff == ALMOST_SICK || player_ptr->debuff == SICK && player_ptr->buff != GUARDED && player_ptr->buff != INVULNERABLE) {
        center_screen(WIDTH, "%s\n", "You're incredibly full after that meal.");
        sleep(1);
        buff_char(player_ptr, LUNCHLADY_ptr, FULL);
    }
    switch (turn_no % 3) {
        case 0:
            if (served == FALSE && turn_no % 6 == 0) {
                center_screen(WIDTH, "%s\n", "Your stomach rumbles as the aroma of food surrounds you.");
                debuff_char(LUNCHLADY_ptr, player_ptr, HUNGRY);
                sleep(2);
            }
            center_screen(WIDTH, "%s\n", "Lexa stabs you with a skewer!");
            damage_dealt(LUNCHLADY_ptr, player_ptr, LUNCHLADY_ptr->attack);
            break;
        case 1:
            if (served == FALSE) {
                center_screen(WIDTH, "%s\n", "Lexa asks you whether you want to eat together, to which you say...");
                while ((keystr = getaction(Controller)) != _EOF && keystr != Yes && keystr != No)
                    ;
                switch (keystr) {
                    case _EOF:
                        quit();
                        break;
                    case Yes:
                        served = TRUE;
                        center_screen(WIDTH, "%s\n", "You eat together awkwardly. It tastes strange but filling...");
                        buff_char(LUNCHLADY_ptr, player_ptr, FULL);
                        debuff_char(LUNCHLADY_ptr, player_ptr, ALMOST_SICK);
                        break;
                    case No:
                        center_screen(WIDTH, "%s\n", "She stabs you with her kitchen knife disappointedly.");
                        damage_dealt(LUNCHLADY_ptr, player_ptr, LUNCHLADY_ptr->attack);
                        break;
                } 
            }
            else
                center_screen(WIDTH, "%s\n", "You feel sick...");
            --LUNCHLADY_ptr->attack;
            break;
        case 2:
            center_screen(WIDTH, "%s\n", "Lexa batters you with her spatula!");
            damage_dealt(LUNCHLADY_ptr, player_ptr, LUNCHLADY_ptr->attack);
            ++LUNCHLADY_ptr->attack;
            break;
    }
    sleep(3);
}

/* FRAUDSTER_behavior : Random chaos */
void FRAUDSTER_behavior(struct Character *player_ptr, struct Character *FRAUDSTER_ptr, int turn_no) {
    static int roll;    // Keeps track of the current roll to determine next behavior
    int random_num, threshold;  // random_num contains the  
    int lucky_numbers[7] = {1, 3, 7, 9, 13, 15, 21};
    int unlucky_numbers[7] = {2, 4, 5, 6, 8, 12, 16};
    int neutral_numbers[8] = {0, 10, 11, 14, 17, 18, 19, 20};
    char message[25];

    switch (FRAUDSTER_ptr->debuff) {
        case UNLUCKY:
            threshold = 21;
            break;
        default:
            threshold = 3;
            if (FRAUDSTER_ptr->debuff == RESTRAINED && turn_no % 2 == 0) {
                debuff_char(player_ptr, FRAUDSTER_ptr, NONE);
                center_screen(WIDTH, "%s\n", "The mysterious force releases Dalia.");
                sleep(1);
                ++turn_no;
            }
            break;
    }
    switch (turn_no % 2) {
        case 0:
            while (roll == 10) /* FORTUNE */
                roll = rand() % 22;
            if (FRAUDSTER_ptr->buff == INVULNERABLE)
                buff_char(FRAUDSTER_ptr, player_ptr, NONE);
            switch (roll) {
                case 0: /* FOOL */
                    center_screen(WIDTH, "%s\n", "Everyone has been revitalized.");
                    health_healed(player_ptr, FRAUDSTER_ptr, INIT_HEALTH - player_ptr->health);
                    health_healed(FRAUDSTER_ptr, player_ptr, FRAUDSTER_hp - FRAUDSTER_ptr->health);
                    break;
                case 1: /* MAGICIAN */
                    center_screen(WIDTH, "%s\n", "Fortuna entered a state of deep concentration.");
                    buff_char(FRAUDSTER_ptr, player_ptr, ENLIGHTENED);
                    break;
                case 2: /* PRIESTESS */
                    center_screen(WIDTH, "%s\n", "Fortuna loses faith in herself.");
                    buff_char(FRAUDSTER_ptr, player_ptr, NONE);
                    break;
                case 3: /* EMPRESS */
                    center_screen(WIDTH, "%s\n", "Fortuna takes care of herself.");
                    health_healed(FRAUDSTER_ptr, player_ptr, 10);
                    buff_char(FRAUDSTER_ptr, player_ptr, REGEN);
                    break;
                case 4: /* EMPEROR */
                    center_screen(WIDTH, "%s\n", "Fortuna grants you a bit of her strength.");
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Your attack power increased by 1 for this battle.");
                    ++player_ptr->attack;
                    break;
                case 5: /* HIEROPHANT */
                    center_screen(WIDTH, "%s\n", "Fortuna grants you a pardon.");
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Your karma increased by 5.");
                    player_ptr->karma += 5;
                    break;
                case 6: /* LOVERS */
                    center_screen(WIDTH, "%s\n", "Fortuna tends to any wounds you have.");
                    health_healed(player_ptr, FRAUDSTER_ptr, 10);
                    buff_char(player_ptr, FRAUDSTER_ptr, NONE);
                    break;
                case 7: /* CHARIOT */
                    center_screen(WIDTH, "%s\n", "Fortuna bursts with ambition as she starts pummels.");
                    buff_char(FRAUDSTER_ptr, player_ptr, RUSHING);
                    damage_dealt(FRAUDSTER_ptr, player_ptr, FRAUDSTER_ptr->attack);
                    break;
                case 8: /* JUSTICE */
                    center_screen(WIDTH, "%s\n", "Fortuna gets struck by a lightning bolt.");
                    damage_dealt(FRAUDSTER_ptr, FRAUDSTER_ptr, 5);
                    break;
                case 9: /* HERMIT */
                    center_screen(WIDTH, "%s\n", "Fortuna is encased in a protective shell!");
                    buff_char(FRAUDSTER_ptr, player_ptr, INVULNERABLE);
                    break;
                case 11: /* STRENGTH */
                    center_screen(WIDTH, "%s\n", "Everyone loses their doubt, now dying to fight!");
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Everybody's attack power increased by 1 for THIS battle.");
                    ++player_ptr->attack;
                    ++FRAUDSTER_ptr->attack;
                    break;
                case 12: /* HANGED MAN */
                    center_screen(WIDTH, "%s\n", "Fortuna is confined by a mysterious force.");
                    debuff_char(player_ptr, FRAUDSTER_ptr, RESTRAINED);
                    break;
                case 13: /* DEATH */
                    center_screen(WIDTH, "%s\n", "Fortuna starts sapping your energy!");
                    debuff_char(FRAUDSTER_ptr, player_ptr, ACIDIFIED);
                    break;
                case 14: /* TEMPERANCE */
                    center_screen(WIDTH, "%s\n", "All buffs and debuffs have been reset.");
                    buff_char(player_ptr, FRAUDSTER_ptr, NONE);
                    debuff_char(FRAUDSTER_ptr, player_ptr, NONE);
                    buff_char(FRAUDSTER_ptr, player_ptr, NONE);
                    debuff_char(player_ptr, FRAUDSTER_ptr, NONE);
                    break;
                case 15: /* DEVIL */
                    center_screen(WIDTH, "%s\n", "Fortuna is gathering sealed power.");
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Dalia's attack power increased by 2.");
                    FRAUDSTER_ptr->attack += 2;
                    break;
                case 16: /* TOWER */
                    center_screen(WIDTH, "%s\n", "Fortuna senses upcoming disaster and chaos...");
                    debuff_char(player_ptr, FRAUDSTER_ptr, UNLUCKY);
                    break;
                case 17: /* STAR */
                    center_screen(WIDTH, "%s\n", "Everyone enters a state of content deep inspiration.");
                    buff_char(FRAUDSTER_ptr, player_ptr, FULL);
                    buff_char(player_ptr, FRAUDSTER_ptr, FULL);
                    break;
                case 18: /* MOON */
                    center_screen(WIDTH, "%s\n", "Everyone gets caught in a restorative aura.");
                    buff_char(FRAUDSTER_ptr, player_ptr, REGEN);
                    buff_char(player_ptr, FRAUDSTER_ptr, REGEN);
                    break;
                case 19: /* SUN */
                    center_screen(WIDTH, "%s\n", "Everyone gets caught in a raging blaze.");
                    debuff_char(FRAUDSTER_ptr, player_ptr, BURNING);
                    debuff_char(player_ptr, FRAUDSTER_ptr, BURNING);
                    break;
                case 20: /* JUDGMENT */
                    center_screen(WIDTH, "%s\n", "Everyone gets struck by a lightning bolt!");
                    damage_dealt(FRAUDSTER_ptr, player_ptr, 10);
                    damage_dealt(FRAUDSTER_ptr, FRAUDSTER_ptr, 10);
                    break;
                case 21: /* WORLD */
                    center_screen(WIDTH, "%s\n", "Fortuna sees an opportunity to strike!");
                    damage_dealt(FRAUDSTER_ptr, player_ptr, FRAUDSTER_ptr->attack + 2);
                    break;
            }
            break;
        case 1:
            if (FRAUDSTER_ptr->debuff == RESTRAINED)
                break;
    
            if ((random_num = rand() % 22) > threshold)
                roll = lucky_numbers[rand() % (sizeof(lucky_numbers) / sizeof(lucky_numbers[0]) + 1)];
            else if (random_num % 2 || FRAUDSTER_ptr->debuff == UNLUCKY)
                roll = unlucky_numbers[rand() % (sizeof(unlucky_numbers) / sizeof(unlucky_numbers[0]) + 1)];
            else
                roll = neutral_numbers[rand() % (sizeof(neutral_numbers) / sizeof(neutral_numbers[0]) + 1)];

            center_screen(WIDTH, "%s\n", "Dalia threw a dice directly toward you!");
            sleep(1);
            damage_dealt(FRAUDSTER_ptr, player_ptr, FRAUDSTER_ptr->attack);
            sprintf(message, "%d was rolled.", roll);
            center_screen(WIDTH, "%s\n", message);

            if (FRAUDSTER_ptr->debuff == UNLUCKY)
                debuff_char(player_ptr, FRAUDSTER_ptr, NONE);
            break;
    }
    sleep(3);
}

/* NAPPER_behavior : Hard-hitting & has trump cards */
void NAPPER_behavior(struct Character *player_ptr, struct Character *NAPPER_ptr, int turn_no) {
    char message[35];
    if (NAPPER_ptr->buff == SUMMON) {
        if (NAPPER_ptr->ally_health < 1) {
            NAPPER_ptr->ally_attack = 0;
            buff_char(NAPPER_ptr, player_ptr, NONE);
            sprintf(message, "All three gangsters are knocked out cold.");
        }
        else {
            if (NAPPER_ptr->ally_health < 5)
                NAPPER_ptr->ally_attack = 1;
            else if (NAPPER_ptr->ally_health < 9)
                NAPPER_ptr->ally_attack = 2;
            sprintf(message, "%d gangster%s beat you to a pulp.", NAPPER_ptr->ally_attack, (NAPPER_ptr->ally_attack == 1) ? "" : "s");
        }
        center_screen(WIDTH, "%s\n", message);
        damage_dealt(NAPPER_ptr, player_ptr, NAPPER_ptr->ally_attack);
        sleep(2);
    }
    switch (turn_no % 3) {
        case 0:
            if (NAPPER_ptr->buff != SUMMON) {
                if (player_ptr->buff == RUSHING || player_ptr->buff == FULL) {
                    center_screen(WIDTH, "%s\n", "Lucinda blocks her head with her arms.");
                    buff_char(NAPPER_ptr, player_ptr, GUARDED);
                }
                else if (NAPPER_ptr->health < NAPPER_ptr->max_health / 3) {
                    center_screen(WIDTH, "%s\n", "Lucinda inhales a drug on her person.");
                    buff_char(NAPPER_ptr, player_ptr, REGEN);
                }
                else {
                    center_screen(WIDTH, "%s\n", "Lucinda flails her baby doll around, hitting you.");
                    damage_dealt(NAPPER_ptr, player_ptr, NAPPER_ptr->attack);
                }
            }
            else {
                center_screen(WIDTH, "%s\n", "Lucinda is getting her breath back.");
                health_healed(NAPPER_ptr, player_ptr, 1);
            }
            break;
        case 1:
            center_screen(WIDTH, "%s\n", "Lucinda smashes you with her baby doll.");
            damage_dealt(NAPPER_ptr, player_ptr, NAPPER_ptr->attack);
            break;
        case 2:
            center_screen(WIDTH, "%s\n", "Lucinda is sizing you up...");
            if (turn_no == 2) {
                center_screen(WIDTH, "%s\n", "Lucinda pulled out her phone and rapidly tapped on the screen.");
                sleep(2);
            }
            else if (turn_no == 5) {
                center_screen(WIDTH, "%s\n", "Three gangsters emerged and joined the fight!");
                buff_char(NAPPER_ptr, player_ptr, SUMMON);
                NAPPER_ptr->ally_health = 12;
                NAPPER_ptr->ally_attack = 3;
                sleep(2);
            }
            break;
    }
    sleep(3);
}

/* BAT_behavior : Wind-up fighter */
void BAT_behavior(struct Character *player_ptr, struct Character *BAT_ptr, int turn_no) {
    switch (turn_no % 5) {
        case 0:
            if (player_ptr->debuff == RESTRAINED) {
                center_screen(WIDTH, "%s\n", "You recovered from the tasing.");
                debuff_char(BAT_ptr, player_ptr, NONE);
                sleep(1);
            }
            BAT_ptr->attack += 2;
            center_screen(WIDTH, "%s\n", "Grischa threw a batarang past you!");
            break;
        case 1:
            if (turn_no == 1)
                center_screen(WIDTH, "%s\n", "Grischa hurriedly puts on his Batman outfit.");
            else {
                center_screen(WIDTH, "%s\n", "Grischa cuts through with his bladed gauntlet!");
                damage_dealt(BAT_ptr, player_ptr, BAT_ptr->attack);
            }
            BAT_ptr->attack -= 2;
            break;
        case 2:
            if (turn_no == 2)
                center_screen(WIDTH, "%s\n", "Grischa swiftly calibrates all of his gadgets.");
            else {
                center_screen(WIDTH, "%s\n", "Grischa flinged a batarang, hitting dead-on!");
                damage_dealt(BAT_ptr, player_ptr, BAT_ptr->attack);
                if (turn_no > 2) {
                    center_screen(WIDTH, "%s\n", "The other batarang comes back, making another impact!");
                    damage_dealt(BAT_ptr, player_ptr, BAT_ptr->attack);
                }
            }
            break;
        case 3:
            if (player_ptr->buff == SUMMON)
                center_screen(WIDTH, "%s\n", "Grischa grappled himself to a better angle.");
            else if (player_ptr->buff == INVULNERABLE)
                center_screen(WIDTH, "%s\n", "Grischa points his gloved index finger somewhere.");
            else    
                center_screen(WIDTH, "%s\n", "Grischa points his gloved index finger at you...");
            break;
        case 4:
            center_screen(WIDTH, "%s\n", "Grischa activated a taser hidden in his glove!");
            debuff_char(BAT_ptr, player_ptr, RESTRAINED);
            break;
    }
    sleep(3);
}

/* ASSASSIN_behavior : Penalizes passive fighting style */
void ASSASSIN_behavior(struct Character *player_ptr, struct Character *ASSASSIN_ptr, int turn_no) {
    static int turns_bored, previous_health, bullets = 6;
    if (previous_health == ASSASSIN_ptr->health)
        ++turns_bored;
    else
        turns_bored = 0;
    switch (turn_no % 6) {
        case 0:
            switch (ASSASSIN_ptr->buff) {
                case DOUBLESHOT:
                    center_screen(WIDTH, "%s\n", "Matilda burst fires with her submachine gun!");
                    damage_dealt(ASSASSIN_ptr, player_ptr, ASSASSIN_ptr->attack);
                    break;
                case TRIPLESHOT:
                    center_screen(WIDTH, "%s\n", "Matilda spray fires with her assault rifle!");
                    damage_dealt(ASSASSIN_ptr, player_ptr, ASSASSIN_ptr->attack);
                    break;
                default:
                    if (ASSASSIN_ptr->debuff != NONE) {
                        center_screen(WIDTH, "%s\n", "Matilda injects a combat stim into herself!");
                        health_healed(ASSASSIN_ptr, player_ptr, turn_no);
                        debuff_char(player_ptr, ASSASSIN_ptr, NONE);
                        buff_char(ASSASSIN_ptr, player_ptr, NONE);
                    }
                    else {
                        center_screen(WIDTH, "%s\n", "Matilda darts around and slashes with her daggers!");
                        damage_dealt(ASSASSIN_ptr, player_ptr, ASSASSIN_ptr->attack);
                        buff_char(ASSASSIN_ptr, player_ptr, INVULNERABLE);
                    }
                    break;
            }
            break;
        case 1:
            if (ASSASSIN_ptr->buff == INVULNERABLE) {
                center_screen(WIDTH, "%s\n", "Matilda rushes to a stop.");
                buff_char(ASSASSIN_ptr, player_ptr, NONE);
            }
            center_screen(WIDTH, "%s\n", "Matilda unholsters her silenced pistol!");
            break;
        case 2:
            if (bullets) {
                center_screen(WIDTH, "%s\n", "Matilda pulls the trigger!");
                --bullets;
                damage_dealt(ASSASSIN_ptr, player_ptr, ASSASSIN_ptr->attack);
            }
            else {
                center_screen(WIDTH, "%s\n", "Matilda reloads her pistol's magazine.");
                bullets = 6;
            }
            break;
        case 3:
            turns_bored = 0;
            center_screen(WIDTH, "%s\n", "Matilda holsters, waiting for your next move.");
            break;
        case 4:
            buff_char(ASSASSIN_ptr, player_ptr, GUARDED);
            if (turns_bored == 1 || turn_no > 9) {
                center_screen(WIDTH, "%s\n", "Matilda is unimpressed right now.");
                buff_char(ASSASSIN_ptr, player_ptr, DOUBLESHOT);
            }
            else
                center_screen(WIDTH, "%s\n", "Matilda is clapping for your performance.");
            break;
        case 5:
            if (turns_bored == 2 || turn_no > 10) {
                center_screen(WIDTH, "%s\n", "Matilda seems bored out of her mind.");
                buff_char(ASSASSIN_ptr, player_ptr, TRIPLESHOT);
            }
            else if (turns_bored == 1) {
                center_screen(WIDTH, "%s\n", "Matilda became unimpressed with you.");
                buff_char(ASSASSIN_ptr, player_ptr, DOUBLESHOT);
            }
            else
                center_screen(WIDTH, "%s\n", "Matilda is slightly impressed.");
            break;
    }
    previous_health = ASSASSIN_ptr->health;
    sleep(3);
}

/* FERAL_behavior : Ambushes player, then standard fight w/ anti-debuff enemy */
void FERAL_behavior(struct Character *player_ptr, struct Character *FERAL_ptr, int turn_no) {
    switch (turn_no % 3) {
        case 0:
            if (player_ptr->debuff == RESTRAINED) {
                center_screen(WIDTH, "%s\n", "You finally free yourself of the trap.");
                debuff_char(FERAL_ptr, player_ptr, NONE);
            }
            center_screen(WIDTH, "%s\n", "Chike bashed you with a handgun he picked up.");
            damage_dealt(FERAL_ptr, player_ptr, FERAL_ptr->attack);
            debuff_char(FERAL_ptr, player_ptr, DOUBLEOPEN);
            break;
        case 1:
            center_screen(WIDTH, "%s\n", "Chike went on all fours.");
            buff_char(FERAL_ptr, player_ptr, CONFIDENT);
            sleep(1);
            center_screen(WIDTH, "%s\n", "Chike stored a dozen macadamia nuts inside his mouth.");
            break;
        case 2:
            if (FERAL_ptr->health <= FERAL_ptr->max_health / 2) {
                center_screen(WIDTH, "%s\n", "Chike cracked open and munched on the nuts.");
                health_healed(FERAL_ptr, player_ptr, FERAL_ptr->max_health / 6);
            }
            else {
                center_screen(WIDTH, "%s\n", "Chike propelled the nuts at high speeds!");
                damage_dealt(FERAL_ptr, player_ptr, FERAL_ptr->attack);
            }
            break;
    }
    sleep(3);
}

enum art_styles {
    DEPRESSING = 1, 
    REALISTIC, 
    HORRIFIC, 
    INSPIRATIONAL, 
    DRAMATIZED,
    PROPAGANDIC,
    ABSTRACT,
    GRAPHIC,
};

/* ARTIST_behavior : High risk/reward chaos */
void ARTIST_behavior(struct Character *player_ptr, struct Character *ARTIST_ptr, int turn_no) {
    static int previous_your_health = INIT_HEALTH, art_style = NONE, previous_art_style = NONE, boost = 0;
    int amt;
    if (ARTIST_ptr->buff == SUMMON) {
        if (ARTIST_ptr->ally_health < 1) {
            buff_char(ARTIST_ptr, player_ptr, NONE);
            center_screen(WIDTH, "%s\n", "The white knight blunders to the ground.");
        }
        else {
            center_screen(WIDTH, "%s\n", "The white knight moves in to attack!");
            damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->ally_attack);
        }
        sleep(2);
    }
    switch (turn_no % 2) {
        case 0:
            switch (art_style) {
                case DEPRESSING:
                    switch (previous_art_style) {
                        case DEPRESSING:
                            center_screen(WIDTH, "%s\n", "Faiza is grief incarnate.");
                            if (player_ptr->debuff != NONE)
                                damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            else
                                debuff_char(player_ptr, ARTIST_ptr, NONE);
                            break;
                        case HORRIFIC:
                            center_screen(WIDTH, "%s\n", "Faiza is trauma incarnate.");
                            damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            break;
                        case INSPIRATIONAL:
                            center_screen(WIDTH, "%s\n", "Faiza is pessimism incarnate.");
                            ++player_ptr->attack;
                            player_ptr->karma = maximum(0, player_ptr->karma - 10);
                            break;
                        case DRAMATIZED:
                            center_screen(WIDTH, "%s\n", "Faiza is disappointment incarnate.");
                            if (ARTIST_ptr->buff != NONE)
                                damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            else
                                buff_char(ARTIST_ptr, player_ptr, GUARDED);
                            break;
                        default:
                            center_screen(WIDTH, "%s\n", "Faiza is depression incarnate.");
                            health_healed(ARTIST_ptr, player_ptr, 5);
                            break;
                    }
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Faiza exudes a gloomy aura.");
                    player_ptr->karma = maximum(0, player_ptr->karma - 3);
                    break;
                case REALISTIC:
                    switch (previous_art_style) {
                        case REALISTIC:
                            center_screen(WIDTH, "%s\n", "Faiza is acceptance incarnate.");
                            ++ARTIST_ptr->attack;
                            ARTIST_ptr->karma += 10;
                            break;
                        case DRAMATIZED:
                            center_screen(WIDTH, "%s\n", "Faiza is corruption incarnate.");
                            if (player_ptr->debuff != ACIDIFIED)
                                debuff_char(ARTIST_ptr, player_ptr, ACIDIFIED);
                            else
                                damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            break;
                        case PROPAGANDIC:
                            center_screen(WIDTH, "%s\n", "Faiza is naivete incarnate.");
                            ARTIST_ptr->karma += 15;
                            ARTIST_ptr->max_health += 5;
                            break;
                        case ABSTRACT:
                            center_screen(WIDTH, "%s\n", "Faiza is judgment incarnate.");
                            if (++boost % 5 == 0)
                                debuff_char(ARTIST_ptr, player_ptr, ITEMLESS);
                            break;
                        default:
                            center_screen(WIDTH, "%s\n", "Faiza is reality incarnate.");
                            damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            break;
                    }
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Faiza exudes a generic aura.");
                    debuff_char(player_ptr, ARTIST_ptr, NONE);
                    break;
                case HORRIFIC:
                    switch (previous_art_style) {
                        case DEPRESSING:
                            center_screen(WIDTH, "%s\n", "Faiza is nightmare incarnate.");
                            break;
                        case HORRIFIC:
                            center_screen(WIDTH, "%s\n", "Faiza is hate incarnate.");
                            if (ARTIST_ptr->buff != NONE)
                                health_healed(ARTIST_ptr, player_ptr, 5);
                            else
                                buff_char(ARTIST_ptr, player_ptr, FULL);
                            break;
                        case INSPIRATIONAL:
                            center_screen(WIDTH, "%s\n", "Faiza is regret incarnate.");
                            ++ARTIST_ptr->max_health;
                            debuff_char(player_ptr, ARTIST_ptr, NONE);
                            break;
                        case PROPAGANDIC:
                            center_screen(WIDTH, "%s\n", "Faiza is stress incarnate.");
                            buff_char(ARTIST_ptr, player_ptr, REGEN);
                            break;
                        default:
                            center_screen(WIDTH, "%s\n", "Faiza is horror incarnate.");
                            damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            break;
                    }
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Faiza exudes a scary aura.");
                    ++ARTIST_ptr->max_health;
                    break;
                case INSPIRATIONAL:
                    switch (previous_art_style) {
                        case REALISTIC:
                            center_screen(WIDTH, "%s\n", "Faiza is expectation incarnate.");
                            buff_char(ARTIST_ptr, player_ptr, GUARDED);
                            break;
                        case HORRIFIC:
                            center_screen(WIDTH, "%s\n", "Faiza is spite incarnate.");
                            damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            break;
                        case INSPIRATIONAL:
                            center_screen(WIDTH, "%s\n", "Faiza is pride incarnate.");
                            ARTIST_ptr->karma = 0;
                            break;
                        case PROPAGANDIC:
                            center_screen(WIDTH, "%s\n", "Faiza is manipulation incarnate");
                            buff_char(ARTIST_ptr, player_ptr, SUMMON);
                            ARTIST_ptr->ally_health = 9;
                            ARTIST_ptr->ally_attack = 1;
                            break;
                        default:
                            center_screen(WIDTH, "%s\n", "Faiza is inspiration incarnate.");
                            buff_char(ARTIST_ptr, player_ptr, ENLIGHTENED);
                            break;
                    }
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Faiza exudes a confident aura.");
                    health_healed(ARTIST_ptr, player_ptr, 3);
                    break;
                case DRAMATIZED:
                    switch (previous_art_style) {
                        case DEPRESSING:
                            center_screen(WIDTH, "%s\n", "Faiza is foolery incarnate.");
                            damage_dealt(player_ptr, player_ptr, player_ptr->attack);
                            break;
                        case DRAMATIZED:
                            center_screen(WIDTH, "%s\n", "Faiza is insecurity incarnate.");
                            damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            buff_char(ARTIST_ptr, player_ptr, GUARDED);
                            break;
                        case PROPAGANDIC:
                            center_screen(WIDTH, "%s\n", "Faiza is genocide incarnate.");
                            damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            buff_char(ARTIST_ptr, player_ptr, DOUBLESHOT);
                            break;
                        case GRAPHIC:
                            center_screen(WIDTH, "%s\n", "Faiza is bloodthirst incarnate.");
                            damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            buff_char(ARTIST_ptr, player_ptr, TRIPLESHOT);
                            break;
                        default:
                            center_screen(WIDTH, "%s\n", "Faiza is drama incarnate.");
                            damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            buff_char(ARTIST_ptr, player_ptr, HEROIC);
                            break;
                    }
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Faiza exudes a powerful aura.");
                    ++ARTIST_ptr->attack;
                    break;
                case PROPAGANDIC:
                    switch (previous_art_style) {
                        case REALISTIC:
                            center_screen(WIDTH, "%s\n", "Faiza is pressure incarnate.");
                            if (player_ptr->buff == SUMMON)
                                damage_dealt(ARTIST_ptr, player_ptr, 15);
                            break;
                        case INSPIRATIONAL:
                            center_screen(WIDTH, "%s\n", "Faiza is abuse incarnate.");
                            if (ARTIST_ptr->buff != NONE)
                                buff_char(ARTIST_ptr, player_ptr, PIERCE);
                            else
                                damage_dealt(ARTIST_ptr, player_ptr, player_ptr->attack);
                            break;
                        case ABSTRACT:
                            center_screen(WIDTH, "%s\n", "Faiza is conspiracy incarnate.");
                            if (ARTIST_ptr->buff == SUMMON)
                                ++ARTIST_ptr->ally_attack;
                            else {
                                buff_char(ARTIST_ptr, player_ptr, SUMMON);
                                ARTIST_ptr->ally_health = 9;
                                ARTIST_ptr->ally_attack = 1;
                            }
                            break;
                        case GRAPHIC:
                            center_screen(WIDTH, "%s\n", "Faiza is aggression incarnate.");
                            if (++boost % 5 == 0)
                                debuff_char(ARTIST_ptr, player_ptr, ITEMLESS);
                            break;
                        default:
                            center_screen(WIDTH, "%s\n", "Faiza is propaganda incarnate.");
                            if (ARTIST_ptr->buff == SUMMON)
                                ARTIST_ptr->ally_health += 5;
                            else {
                                buff_char(ARTIST_ptr, player_ptr, SUMMON);
                                ARTIST_ptr->ally_health = 9;
                                ARTIST_ptr->ally_attack = 1;
                            }
                            break;
                    }
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Faiza exudes a commanding aura.");
                    ARTIST_ptr->karma = maximum(0, ARTIST_ptr->karma - 3);
                    break;
                case ABSTRACT:
                    switch (previous_art_style) {
                        case REALISTIC:
                            center_screen(WIDTH, "%s\n", "Faiza is contradiction incarnate.");
                            if (player_ptr->health > player_ptr->max_health / 2)
                                damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            else
                                health_healed(ARTIST_ptr, player_ptr, 5);
                            break;
                        case DRAMATIZED:
                            center_screen(WIDTH, "%s\n", "Faiza is denial incarnate.");
                            if (++boost % 5 == 0)
                                debuff_char(ARTIST_ptr, player_ptr, ITEMLESS);
                            break;
                        case ABSTRACT:
                            center_screen(WIDTH, "%s\n", "Faiza is creativity incarnate.");
                            health_healed(ARTIST_ptr, player_ptr, 10);
                            break;
                        case GRAPHIC:
                            center_screen(WIDTH, "%s\n", "Faiza is lust incarnate.");
                            debuff_char(ARTIST_ptr, player_ptr, ACIDIFIED);
                            break;
                        default:
                            center_screen(WIDTH, "%s\n", "Faiza is abstraction incarnate.");
                            buff_char(player_ptr, ARTIST_ptr, NONE);
                            debuff_char(ARTIST_ptr, player_ptr, NONE);
                            buff_char(ARTIST_ptr, player_ptr, NONE);
                            debuff_char(player_ptr, ARTIST_ptr, NONE);
                            break;
                    }
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Faiza exudes an unknown aura.");
                    buff_char(player_ptr, ARTIST_ptr, NONE);
                    break;
                case GRAPHIC:
                    switch (previous_art_style) {
                        case HORRIFIC:
                            center_screen(WIDTH, "%s\n", "Faiza is revenge incarnate.");
                            damage_dealt(ARTIST_ptr, player_ptr, (ARTIST_ptr->max_health - ARTIST_ptr->health) / 2);
                            break;
                        case DEPRESSING:
                            center_screen(WIDTH, "%s\n", "Faiza is masochism incarnate.");
                            amt = minimum(ARTIST_ptr->health - 1, 5);
                            ARTIST_ptr->attack += amt;
                            break;
                        case ABSTRACT:
                            center_screen(WIDTH, "%s\n", "Faiza is pain incarnate.");
                            damage_dealt(ARTIST_ptr, ARTIST_ptr, ARTIST_ptr->attack);
                            ARTIST_ptr->max_health += 3;
                            break;
                        case GRAPHIC:
                            center_screen(WIDTH, "%s\n", "Faiza is wrath incarnate.");
                            ARTIST_ptr->attack *= 4;
                            damage_dealt(ARTIST_ptr, player_ptr, ARTIST_ptr->attack);
                            break;
                        default:
                            center_screen(WIDTH, "%s\n", "Faiza is violence incarnate.");
                            player_ptr->health -= 10;
                            break;
                    }
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "Faiza exudes a fearsome aura.");
                    player_ptr->health -= 3;
                    break;
            }
            break;
        case 1:
            previous_art_style = art_style;
            if (ARTIST_ptr->debuff == BURNING && player_ptr->debuff != BURNING) {
                center_screen(WIDTH, "%s\n", "Faiza added crimson strokes and impactful words to her drawing.");
                art_style = DEPRESSING;
            }
            else if (player_ptr->debuff == BURNING || player_ptr->debuff == ACIDIFIED) {
                if (ARTIST_ptr->debuff == BURNING) {
                    center_screen(WIDTH, "%s\n", "Faiza added horrific demons and monsters to her drawing.");
                    art_style = HORRIFIC;
                }
                else {
                    center_screen(WIDTH, "%s\n", "Faiza added intricated shadowing and splatters to liven her drawing.");
                    art_style = REALISTIC;
                }
            }
            else if (previous_your_health != player_ptr->health || player_ptr->buff == REGEN || player_ptr->buff == GUARDED) {
                center_screen(WIDTH, "%s\n", "Faiza tenderly added details with subliminal meanings to her drawing.");
                art_style = INSPIRATIONAL;
            }
            else if (player_ptr->buff == RUSHING || player_ptr->buff == FULL || player_ptr->buff == HEROIC) {
                center_screen(WIDTH, "%s\n", "Faiza dramatized her drawing with assorted symbols and bold colors.");
                art_style = DRAMATIZED;
            }
            else if (player_ptr->buff == SUMMON || player_ptr->buff == INVULNERABLE) {
                center_screen(WIDTH, "%s\n", "Faiza added abstract representations and themes to her drawing.");
                art_style = ABSTRACT;
            }
            else {
                center_screen(WIDTH, "%s\n", "Faiza added gorey elements and graphic themes to her drawing.");
                art_style = GRAPHIC;
            }
            break;
    }
    previous_your_health = player_ptr->health;
    sleep(3);
}

/* POLITICIAN_behavior : Anti-buff */
void POLITICIAN_behavior(struct Character *player_ptr, struct Character *POLITICIAN_ptr, int turn_no) {
    static int bottles_littered;
    if (bottles_littered >= 3) {
        center_screen(WIDTH, "%s\n", "The lobby's floor is flooded with broken glass.");
        debuff_char(player_ptr, POLITICIAN_ptr, SPIKES);
        debuff_char(POLITICIAN_ptr, player_ptr, SPIKES);
    }
    switch (turn_no % 4) {
        case 0:
            center_screen(WIDTH, "%s\n", "Nicolau stops to check a milk bottle on the table.");
            sleep(1);
            if (turn_no % 8) {
                center_screen(WIDTH, "%s\n", "Then, he gulps the milk down in one swig.");
                health_healed(POLITICIAN_ptr, player_ptr, 5);
            }
            else {
                center_screen(WIDTH, "%s\n", "Then, he throws the bottle on the floor.");
                ++bottles_littered;
            }
            break;
        case 1:
            center_screen(WIDTH, "%s\n", "Nicolau flexes his muscles with a wide grin.");
            buff_char(POLITICIAN_ptr, player_ptr, RIVALED);
            sleep(1);
            break;
        case 2:
            center_screen(WIDTH, "%s\n", "Nicolau moves in to take a bite!");
            damage_dealt(POLITICIAN_ptr, player_ptr, POLITICIAN_ptr->attack);
            ++POLITICIAN_ptr->attack;
            break;
        case 3:
            center_screen(WIDTH, "%s\n", "Nicolau lashes out with a whip!");
            damage_dealt(POLITICIAN_ptr, player_ptr, POLITICIAN_ptr->attack);
            --POLITICIAN_ptr->attack;
            break;
    }
    sleep(3);
}
 
/* HACKER_behavior : Strength depends on degree of interruption from player */
void HACKER_behavior(struct Character *player_ptr, struct Character *HACKER_ptr, int turn_no) {
    static int previous_health, bullets = 5, tranq = 1;
    buff_char(HACKER_ptr, player_ptr, PIERCE);
    switch (turn_no % 3) {
        case 0:
            if (HACKER_ptr->debuff == BURNING) {
                center_screen(WIDTH, "%s\n", "Lachlan sprinted and dove into the ocean.");
                debuff_char(player_ptr, HACKER_ptr, NONE);
            }
            else if (tranq) {
                --tranq;
                center_screen(WIDTH, "%s\n", "Lachlan shot a tranquilizer dart at you.");
                debuff_char(HACKER_ptr, player_ptr, DOZING);
            }
            else {
                center_screen(WIDTH, "%s\n", "Lachlan loaded another tranquilizer dart into his rifle.");
                tranq = 1;
            }
            break;
        case 1:
            if (player_ptr->debuff == DOZING) {
                center_screen(WIDTH, "%s\n", "You try to fight it, but you fell asleep on the spot.");
                debuff_char(HACKER_ptr, player_ptr, ASLEEP);
            }
            center_screen(WIDTH, "%s\n", "Lachlan is aiming his sniper rifle...");
            previous_health = HACKER_ptr->health;
            break;
        case 2:
            if (bullets) {
                --bullets;
                if (HACKER_ptr->debuff == BURNING) {
                    center_screen(WIDTH, "%s\n", "Lachlan's burning arm messed up his shot!");
                    damage_dealt(HACKER_ptr, player_ptr, HACKER_ptr->attack / 2);
                }
                else {
                    if (previous_health != HACKER_ptr->health) {
                        center_screen(WIDTH, "%s\n", "Lachlan's shot goes astray!");
                        damage_dealt(HACKER_ptr, player_ptr, HACKER_ptr->attack - 3);
                    }
                    else {
                        center_screen(WIDTH, "%s\n", "Lachlan nails the perfect shot!");
                        damage_dealt(HACKER_ptr, player_ptr, HACKER_ptr->attack);
                    }
                }
            }
            else {
                center_screen(WIDTH, "%s\n", "Lachlan reloads his sniper rifle.");
                bullets = 5;
            }
            break;
    }
    sleep(3);
}

/* JUDGE_behavior : Devastate the player */
void JUDGE_behavior(struct Character *player_ptr, struct Character *JUDGE_ptr, int turn_no) {
    int keystr;
    static int first_time_destruction, first_time_rejuvenation;
    if (turn_no == 1)
        first_time_destruction = TRUE, first_time_rejuvenation = TRUE;
    char message[100];
    switch (turn_no % 7) {
        case 0:
            sprintf(message, "It %s", 
                (player_ptr->health > 40) ? "executes a three-hit combo with its balance!" : "confidently wields its balance, striking and smashing your limbs!");
            center_screen(WIDTH, "%s\n", message);
            player_ptr->health -= JUDGE_ptr->attack;
            player_ptr->karma -= JUDGE_ptr->attack / 2;
            break;
        case 1:
            if (player_ptr->karma > 10) {
                center_screen(WIDTH, "%s\n", "Its evil but bright eye shines in your face, not flinching... just judging you.");
                player_ptr->karma -= 10;
            }
            else {
                center_screen(WIDTH, "%s\n", "Suddenly, its grotesque right hand propels towards you, brutalizing your body.");
                player_ptr->health -= JUDGE_ptr->attack;
            }
            break;
        case 2:
            center_screen(WIDTH, "%s\n", "Its aggression ceases. It then asks you whether you want destruction, to which you say...");
            while ((keystr = getaction(Controller)) != _EOF && keystr != Yes && keystr != No)
                ;
            switch (keystr) {
                case _EOF:
                    quit();
                    break;
                case Yes:
                    center_screen(WIDTH, "%s", "Hah");
                    sleep(1);
                    printf("hah");
                    sleep(1);
                    printf("hah.\n");
                    sleep(1);
                    center_screen(WIDTH, "%s\n", "A divine light shines between you two before exploding into a star.");
                    player_ptr->health -= player_ptr->max_health / 3;
                    JUDGE_ptr->health -= JUDGE_ptr->max_health / 3;
                    break;
                case No:
                    center_screen(WIDTH, "%s\n", "It seems to frown straight at you.");
                    break;
            }
            break;
        case 3:
            center_screen(WIDTH, "%s\n", "The boy...");
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            center_screen(WIDTH, "%s\n", "Its rampaging ceases. It then asks you whether you want rejuvenation, to which you say...");
            while ((keystr = getaction(Controller)) != _EOF && keystr != Yes && keystr != No)
                ;
            switch (keystr) {
                case _EOF:
                    quit();
                    break;
                case Yes:
                    if (fate_returner('+') == CONVICTED) {
                        center_screen(WIDTH, "%s\n", "A shadowy figure approaches you two with a medical saw.");
                        sleep(1);
                        center_screen(WIDTH, "%s\n", "The operation has been complete. The figure leapt into the skies.");
                        health_healed(player_ptr, JUDGE_ptr, player_ptr->max_health / 3);
                        health_healed(JUDGE_ptr, player_ptr, JUDGE_ptr->max_health / 3);
                    }
                    else if (fate_returner('+') == SPARED) {
                        center_screen(WIDTH, "%s\n", "A shadowy figure reaches for your hand.");
                        sleep(1);
                        center_screen(WIDTH, "%s\n", "You feel a little bit better after that. The figure walked away.");
                        health_healed(player_ptr, JUDGE_ptr, player_ptr->max_health / turn_no);
                    }
                    else {
                        center_screen(WIDTH, "%s\n", "Nothing happened.");
                        sleep(1);
                    }
                    break;
                case No:
                    center_screen(WIDTH, "%s\n", "Let us continue then.");
                    break;
            }
            break;
    }
    sleep(3);
}

/* intro : cues dialogue that introduces the enemy - comments will try to explain each line of dialogue */
int intro(struct Character *enemy_ptr, struct Character *player_ptr) {
    int keystr, symbol;
    skip_scene = FALSE;
    clrscr();
    fate_store(symbol = enemy_ptr->rep, VICTORIOUS);

    char intro_file[28];
    char battle_file[28];
    sprintf(intro_file, "Music\\Wav Files\\%c\\%c_i.wav", symbol, symbol);
    sprintf(battle_file, "Music\\Wav Files\\%c\\%c_b.wav", symbol, symbol);
    //PlaySound(TEXT(intro_file), NULL, SND_ASYNC | SND_LOOP);
    switch (symbol) {
        case 'c':
            enemy_ptr->ai = &KID_behavior;
            character_line(' ', "(While walking through the town, I spotted a strange young boy...)");  // The reason KID is "strange" is because if he's visible, he must have commited a crime
            character_line(symbol, "My name is Cue Helix. Do you need anything? I need to get to work now.");   // KID is trying to get to his shoe polish spot he's known for
            character_line('^', "That's for fate to decide, boy."); // "Fate" is the power of the user to choose KID's fate
            character_line('|', KID_intro_1);   // Don't explain what '|' is immediately; the user will learn as they keep progressing
            character_line('|', KID_intro_2);   // '|' is the sword given to the user; the sword represents being a pawn of judgment and grants the user special powers
            printf("Will you let Cue proceed to his next job? (y / (n = battle))\n");   // It's made ambigious what job KID is going to because the user doesn't know where the barbershop is at
            break;
        case '+':
            enemy_ptr->ai = &DOC_behavior;
            character_line(' ', "(After patrolling a few blocks, I stepped inside a building with inviting open doors.)");  // Pretend that the transition to the next map was a few blocks worth of walking
            character_line(symbol, "Welcome to Dantalion's Spire, the best oriental barbershop this side of the globe. If you would,\nplease take off your boots, sir.");   // Japanese etiquette calls for removal of shoes before entering a private space
            /* Scenario : User has interacted with KID before arriving to see DOC (user has a hint of who DOC is already) */
            if (fate_returner('c') != NEVER_MET) {  // Dantalion's Spire is a name that refers to the demon's (that can teach an art or science) spire, where a spire can refer to Helix, KID's surname, or the barber pole spirals you typically see (red, white, blue stripes)
                character_line('^', "Could he be...?"); // Hint that DOC is somebody important / known already (learnt about him in KID's intro)
                character_line('|', "Yep, I think it must be him.");
                /* Scenario : KID messaged DOC on his phone before his capture */
                if (extra_returner('+', ALERTED)) {
                    character_line(symbol, "Perchance, did you see a young boy down the street? I'm a tad worried about him."); // Referring to KID after KID messaged DOC about something concerning
                    character_line('^', "Possibly. So, could you give us some more details?");  // Feigning ignorance; can't outright say "no" to a civilian when the question is not malicious, right?
                    character_line(symbol, "Oh, yes, of course. With all the young boys on the streets, it is hard to set him apart...");
                    character_line(symbol, "Ah! He is always wearing shiny boots that catch your eyes immediately. How is that?");
                    character_line('^', "Let me recall... this happenened... and that..."); // Supposedly stalling for the DOC's intro 
                }
            }
            /* Scenario : User hasn't interacted with KID before arriving to see DOC (user has no hint of who DOC is) */
            else {
                character_line(' ', "(There were a couple of customizable haircuts advertised on a poster on the wall.)");  // DOC is supposed to be known for being very attentive with his customers (this is one example of his treatment)
                character_line('^', "Maybe I should get a cut. What do you think?");    // Referring to the haircut or a piece of something for sword?
                character_line('|', "Now's not the time to be slacking off, you've yet to finish your quota."); // Sword is talking about the karma decay that happens in the game (quota)
            }
            character_line('|', DOC_intro_1);
            character_line('|', DOC_intro_2);
            printf("Will you leave the barbershop without a new \"cut\" from Arata? (y / (n = battle))\n"); // "cut" refers to cutting of DOC as in battling him
            break;
        case 'M':
            enemy_ptr->ai = &QUEEN_behavior;
            if (extra_returner(symbol, RETURNING)) {
                character_line(' ', "(It's the girl again, but now, she's frolicking in the community park with her suitcase.)");
                character_line(symbol, "It's such a wonderful day today, isn't it, guys? Just wait, we'll have snacks soon.");
                character_line('|', "Did you hear that? Snacks equals strays. What'll you do this time?");
                character_line('^', "Excuse me, miss. What're you doing here all alone? This place can be dangerous.");
                character_line(symbol, "You again! Who do you think you are? I told you already, my parents are busy!");
                character_line('^', "Shouldn't you be at home then? Nobody would leave their kid alone in a public place like this.");
                character_line(symbol, "My parents are nice, and they love me as I love them. Don't act like you know them!");
                character_line('^', "Just trying to help out here.");
                character_line('|', "Think you've worked up the guts to take her on yet? Well?");
                character_line(symbol, "Soldier, you're staring at me again! Mayhaps you've got to say something?!");
            }
            else {
                character_line(' ', "(A young girl immediately caught my eye. She was skipping around merrily.)");
                if (fate_returner('c') != NEVER_MET) {
                    character_line('^', "Another kid. Seriously, what has the world come to?");
                    character_line('|', "Beats me. Maybe humans are just naturally evil.");
                }
                character_line('^', "Excuse me, miss. Where are your parents? This place is dangerous.");
                character_line(symbol, "Who are you to speak to me like that? My parents are handling important business.");
                character_line('^', "We're here to guarantee the safety of the innocent! As for you...");
                character_line('|', QUEEN_intro_1);
                character_line('|', QUEEN_intro_2);
            }
            printf("Will you allow Lille's colony to antagonize more animals? (y / (n = battle))\n");
            break;
        case '%':
            enemy_ptr->ai = &LORD_behavior;
            character_line(' ', "(After passing the many buildings, the town square was ahead. A loud voice could be heard.)");
            character_line(symbol, "Where is the goombah at? I don't got time to be dawdlin' in the square.");
            character_line('^', "Can I help you, good citizen? You seem a bit distressed based on your tone of voice.");
            character_line(symbol, "No, I'm alright, thanks. Don't need help from you, so scram!");
            character_line('^', "I didn't catch that. Could you speak a bit louder, please?");
            character_line(symbol, "Awright, officer. What do you want from ME? I don't want your help. Actually, don't need.\n"
                                   "This is MY responsibility, not yours. Don't act like you gots time to deal with my issues.");
            character_line('^', "What's with the undeserved hostility? Fine, let's play that game.");
            character_line(symbol, "I'm right though. You've got a job to do yourself.");
            character_line('|', LORD_intro_1);
            character_line('|', LORD_intro_2);
            printf("Will you let Severino go free? (y / (n = battle))\n");
            break;
        case '$':
            enemy_ptr->ai = &STOWAWAY_behavior;
            character_line('^', "Hey, where'd you come from?!");
            character_line(symbol, "Huh. Talk to me?");
            character_line('^', "That is my intention, yes. Were you sneaking up on me, mister?");
            character_line(symbol, "You talking to me or who?");
            character_line('^', "Yes, I SEE you. Know what? Hold on a second, you're exhibiting very suspicious behavior.");
            character_line('|', STOWAWAY_intro_1);
            character_line('|', STOWAWAY_intro_2);
            character_line(symbol, "Uh oh. Why you look at me so?");
            printf("Will you permit more of Nemo's thefts? (y / (n = battle))\n");
            break;
        case '*':
            enemy_ptr->ai = &ARSONIST_behavior;
            // 
            if (!extra_returner('*', TRAILED)) {
                character_line(' ', "(A suspicious man is looking inside the building over there. This is very concerning...)");
                character_line('^', "Excuse me, sir. I just have to ask, what're you doing here?");
                character_line(symbol, "I could ask you the same thing. This place should be quite desolate about now.");
                character_line('^', "I'm on my shift, my first one actually.");
                character_line(symbol, "That's great. Congratulations, fair citizen!");
                character_line('^', "So, what're you doing here by the shelter's wall? I expect you to answer after I answered your question.");
                character_line(symbol, "Right. I'm here to check the shelter's electronics, wires, and all that nonsense.");
                character_line('^', "Let's check that testimony. Right now.");
                character_line('|', ARSONIST_intro_1);
                character_line('|', ARSONIST_intro_2);
                printf("Will you allow Inigo to investigate his case further? (y / (n = battle))\n");
            }
            else {
                // Interaction w/ BAT immediately precedes the encounter w/ ARSONIST (TRAILED mode)
                if (fate_returner('L') == NEVER_MET) {
                    character_line(' ', "(I heard loud blasting noises! This is bad!)");
                    character_line('^', "Who are you, a terrorist?!");
                    character_line(symbol, "Nope. I'm a glorified pest exterminator, how about you?");
                    character_line('|', "Let me explain. I'm sure nobody will mind!");
                    character_line('|', ARSONIST_intro_1);
                    character_line('|', ARSONIST_intro_2);
                    if (fate_returner('@') == CONVICTED) {
                        character_line('^', "... Same. You can say that we're comrades.");
                        character_line(symbol, "You would've fooled me at first. However! I've got your number, I know all about you.");
                        character_line('^', "No problem. Another fight on the first day of the job. Bring it!");
                    }
                    else {
                        character_line('^', "... I'm a citizen.");
                        character_line(symbol, "Is that how you answer to prosecutors when they ask for your occupation?");
                        character_line('^', "Is that how you talk to everybody you meet?");
                        character_line(symbol, "Yes, when they're as foolish as you.");
                        character_line('^', "I may be foolish, but never in a million years would I let a terrorist get away!");
                        character_line(symbol, "Such energy! Very well, we'll see how you handle the heat in a moment.");
                    }
                }
                // Interaction w/ LUNCHLADY immediately precedes the encounter w/ ARSONIST 
                else {
                    character_line(' ', "(The homeless shelter is up in flames. Smoke is all about. It's hard to see and breathe.)");
                    character_line(symbol, "No leaving the shelter, my guy.");
                    character_line('^', "Who are you? Please tell me you're a firefighter.");
                    character_line('|', "Hah, I wish! Unfortunately for you...");
                    character_line('|', ARSONIST_intro_1);
                    character_line('|', ARSONIST_intro_2);
                    character_line(symbol, "Far from that, citizen. I'm surprised that you're still alive and kicking.");
                    character_line('^', "Put out the fire now. There's no time for needless chattering!");
                    character_line(symbol, "No can do. I've come to claim both of your heads, so no backing out.");
                    character_line('^', "What's the purpose of showing your face to me? You could've easily blocked the entrance with fire.");
                    character_line(symbol, "Because I need to confirm my contracts. And there is a, certain satisfaction to watching trash\n"
                                           "dissolve in an incinerator. Y'know? My clients have that mutual feeling too.");
                    character_line('^', "Don't be so hard on yourself when I beat you up right now.");
                }
                return FALSE;
            }
            break;
        case 'L':
            enemy_ptr->ai = &LUNCHLADY_behavior;
            character_line(' ', "(I entered a homeless shelter in the slums. Only a woman behind the counter was inside.)");
            character_line(symbol, "Ah, you're an hour early. We don't serve yet, sorry.");
            character_line('^', "I have to say, it does smell wonderful. However, why is the door open then?");
            if (total_fate(CONVICTED) > 2) {
                character_line(symbol, "So I can talk to handsome guys like you, of course!");
                character_line('|', "We're on the job here...");
            }
            else {
                character_line(symbol, "A bit of fresh air here and there doesn't hurt the shelter.");
                character_line('|', "That's why we invented windows...");
            }
            character_line('^', "Anyway, I'm gonna have to order you to stop whatever you're doing. I AM on the job, after all.");
            character_line(symbol, "Ah! Could you be here to check for our cleanliness? How sweet of you.");
            character_line('^', "Putting it simply, yes. You are correct.");
            character_line('|', LUNCHLADY_intro_1);
            character_line('|', LUNCHLADY_intro_2);
            printf("Will you leave Lexa alone in her preparations? (y / (n = battle))\n");
            break;
        case '?':
            enemy_ptr->ai = &FRAUDSTER_behavior;
            character_line(' ', "(A young lady just walked past me. I should check up on her to maintain the peace...)");
            character_line('^', "Hey, where might you be off to, lass?");
            character_line(symbol, "I'm going to a tournament near here. Never been here, but this place is cool!");
            character_line('^', "Yeah... You're not from around here, then?");
            character_line(symbol, "Nope. There's a huge tournament, so fate brought me here.");
            character_line('^', "She seems normal, but there's definitely something fishy.");
            character_line('|', FRAUDSTER_intro_1);
            character_line('|', FRAUDSTER_intro_2);
            character_line('|', FRAUDSTER_intro_3);
            character_line(symbol, "I might seem normal, mister. In battle, I'm something of a professional!");
            character_line('^', "In the tournament battles, you mean? Isn't it a card game tournament or something?");
            character_line(symbol, "Oh, so you know of it! Want to play a match really fast?");
            character_line('^', "No thanks. I don't actually play the game. Anyway...");
            character_line(symbol, "I know! Play me in any game that involves luck. I'm not a cheater, I just gotta warm up!");
            printf("Will you play a game of chance with Dalia? (y / (n = battle))\n");
            break;
        case 'C':
            enemy_ptr->ai = &NAPPER_behavior;
            character_line(' ', "(A woman was standing in the middle of the crossroads. For some reason, I could see a baby in her arms.)");
            character_line('^', "You there, with the baby.");
            character_line(symbol, "Yes? Need something from me?");
            character_line('^', "I'd like to inquire as to what you are doing right now, ma'am.");
            character_line(symbol, "I must fix my car far from here. I got a new battery a few hours ago, as you see.");
            character_line('^', "That sounds plausible. Do you want a lift there? If you've been walking for hours, the car must be far.");
            character_line(symbol, "That sounds fantastic, if you really can afford it. I need to get there quick, as you see.");
            character_line('|', "Don't offer what you can't provide, mister! And you should be more cautious. You're making me look bad.");
            character_line('^', "Can't we get somebody on the job to drive her? How bad is she, anyway?");
            character_line('|', NAPPER_intro_1);
            character_line('|', NAPPER_intro_2);
            character_line('|', NAPPER_intro_3);
            character_line(symbol, "What're you going to do with me? I need to get going soon...");
            printf("Will you let Lucinda walk off? (y / (n = battle))\n");
            break;
        case '@':
            enemy_ptr->ai = &BAT_behavior;
            character_line(' ', "(A van was stationary in the middle of the street. I decided to check on it.)");
            character_line('^', "Step out of the vehicle, sir.");
            character_line(symbol, "What do you WANT?");
            character_line('^', "Just step out of the vehicle. I'll need to search through it.");
            character_line(symbol, "Show me your warrant FIRST.");
            character_line('^', "Don't need one. There's definitely suspicious items in your trunk, don't deny it.");
            character_line(symbol, "That's still an arbitrary search. Don't reason that you'll intrude on my privacy based on sheer\n"
                                   "intuition. Just leave me alone in peace.");
            character_line('|', "As always, our intuition is correct. Don't stand down when this may be your last chance. See...");
            character_line('|', BAT_intro_1);
            character_line('|', BAT_intro_2);
            character_line('|', BAT_intro_3);
            character_line('|', BAT_intro_4);
            character_line('^', "I'll leave you alone. However, searching your trunk is a necessity and doesn't require a warrant.");
            character_line(symbol, "Grr... I'm WARNING you! If you take another step, what happens next is karma.");
            printf("Will you stop your approach towards Grischa's trunk? (y / (n = battle))\n");
            break;
        case '\\':
            enemy_ptr->ai = &ASSASSIN_behavior;
            if (extra_returner(symbol, WON)) {
                character_line('|', "That was the last one we could take out. Nice job!");
                character_line('^', "Yeah, thanks. Now what?");
                character_line(symbol, "... Good on ya! It's our winner of season 3!");
                character_line('^', "Oh... It was my pleasure.");
                character_line(symbol, "I'm stoked! What's the wish ya want granted?");
                character_line('|', "We'll need to choose a realistic wish that can be granted by them. Let's see...");
                character_line(symbol, "We'll fly ya out again, so don't worry wishing for that.");
                character_line('|', "Allow me to give you some suggestions. These are pretty plausible wishes, I'd say. First, you could ask for\n"
                                    "some lessons on fighting to get you beefed up. Second, you could ask for a fight with the siblings, so it's\n"
                                    "possible to convict them. Third, you could ask for them to permanently cancel this \"game show\". Finally, you\n"
                                    "could ask for the siblings to seriously fight one another.");
                printf("Wish for... ((l = lessons) / (f = fight) / (c = cancel) / (s = sibling fight))\n");
                while ((keystr = getaction(Controller)) != _EOF && keystr != Lessons && keystr != Fight && keystr != Cancel && keystr != Down)
                    ;
                switch (keystr) {
                    case Lessons:
                        extra_store(symbol, TRAINED);
                        character_line('^', "Alright. I want you to teach me how to fight like a pro. This is the serious stuff.");
                        character_line(symbol, "Defo! I'll show ya the ropes!");
                        sleep(2);
                        character_line(symbol, "And that's all ya gotta know. Now, I'll fly ya back out where we met.");
                        character_line('|', "Yeah, you look ready to go. You look slightly stronger now! Good on ya.");
                        return TRUE;
                    case Fight:
                        extra_store(symbol, FOUGHT);
                        character_line('^', "What I want is for you and your brother to fight me in fair fights. One at a time.");
                        character_line(symbol, "I'll give ya a fair go! Must be a tough bloke. Ready?");
                        if (player_ptr->health != player_ptr->max_health) {
                            character_line('^', "Wait, wait, wait. I wanted \"fair fights\". I'm writhing in pain over here.");
                            character_line(symbol, "Too right! Patch yerself up quickly!");
                            character_line('|', "You heard her. Get to work! We've got criminals to catch.");
                            printf("You tended to your wounds, however large they may have been. You're finally ready!\n");
                            player_ptr->health = player_ptr->max_health;
                        }
                        character_line('^', "It's go time!");
                        character_line(symbol, "Don't disappoint me.");
                        return FALSE;
                    case Cancel:
                        extra_store(symbol, HALTED);
                        character_line('^', "Can you fulfill this? ... Stop your nonsensical game. This will be the last season ever.");
                        character_line(symbol, "Crikey! A tad iffy there, wouldn't ya say?");
                        character_line('^', "It's perfectly reasonable though. You can grant this wish, right?");
                        character_line(symbol, "... Sure. I'll give it a go. No promises, mate!");
                        character_line('^', "What a bad response. I'm counting on you, so remember my wish for the rest of your life.\n"
                                            "Stop your nonsensical game. That's it.");
                        character_line(symbol, "Right... Now, I'll fly ya back out where we met.");
                        character_line('|', "Yeah, we're ready to go. Let's hope these two honor your wish.");
                        return TRUE;
                    case Down:
                        extra_store(symbol, DUEL);
                        fate_store('/', CONVICTED);
                        character_line('^', "Let's try this. Fight your brother to the death, and we'll see who is stronger.");
                        character_line(symbol, "Pig's arse! THAT'S what ya really want?!");
                        character_line('^', "Have you ever been a contestant in a game like yours? The feelings you get are awesome.");
                        character_line(symbol, "Well, no. Is it fun?");
                        character_line('^', "Indeed, indeed! Imagine walking around and encountering your enemy. The setting is phenomenal, and\n"
                                            "the challenge imposed on you pushes you to the extremes! Give it a try, and you'll be hooked.");
                        character_line('|', "Will she fall for it? She might be an oblivious or gullible gal, right?");
                        character_line(symbol, "I'm interested, especially since I didn't gun down any of yous this season.");
                        character_line('|', "She is.");
                        character_line('^', "Will you do it then? Fight your brother to the death, and crown yourself as a WINNER!");
                        character_line(symbol, "He's in his room. Time to cream him to the floor!");
                        sleep(3);
                        character_line(symbol, "G'day, brother.");
                        character_line('/', "I take it he's the winner? What's the wish?");
                        character_line(symbol, "For us to slug it out in a game! I challenge ya, brother.");
                        character_line('/', "What, THAT is the wish? This might be my chance...");
                        character_line('^', "Are you guys ready then? We'll see who the victor is, but be warned, there is no prize.");
                        character_line(symbol, "I am!");
                        character_line('/', "Sure.");
                        character_line('|', "And with that, the bloodline turns to bloodlust. There they go.\n"
                                            "Not a word is exchanged between the siblings, as they are in a deep battle trance.\n"
                                            "The brother is showing that he's not weak anymore. But his shots are meaningless against his sister.\n"
                                            "Can he keep up with her agility?");
                        character_line(symbol, "I love how it's never a bore with ya around!");
                        character_line(symbol, "Ya look mad as a cut snake! C'mon and actually try!");
                        character_line(symbol, "Give it another burl. Ya might hit me yet!");
                        sleep(1);
                        character_line('^', "They're stalling or something. It's been like 45 minutes already.");
                        character_line('/', "I'm rooted. ... No more shots left!");
                        character_line(symbol, "Is that so? It's so boring without that gun, ya bastard.");
                        character_line('/', "AGH!!!");
                        character_line('|', "Looks like it's finally over with. He can rest easy now, I think?");
                        character_line('^', "Congratulations on your flawless victory.");
                        character_line(symbol, "He had buckley's chance!");
                        character_line('^', "Now that that's done, I'm feeling good enough to fly back!");
                        return TRUE;
                    case _EOF:
                        quit();
                        break;
                }
            }
            else {
                character_line(' ', "(I walked through one of the alleys in the slum. As expected, a criminal was here.)");
                character_line(symbol, "How may I help ya?");
                character_line('^', "Hey, I'm here to investigate the premises. I'd like to ask you some questions.");
                character_line(symbol, "No worries.");
                character_line('^', "What is your occupation? Or maybe, you're unemployed?");
                character_line(symbol, "A game host.");
                character_line('^', "Oh yeah? That's kinda cool.");
                character_line('|', "No kidding. This is it, mate.");
                character_line('|', ASSASSIN_intro_1);
                character_line('|', ASSASSIN_intro_2);
                character_line('|', ASSASSIN_intro_3);
                character_line('|', ASSASSIN_intro_4);
                character_line(symbol, "Maybe ya want in the game? There's a gnarly prize.");
                character_line('^', "Oh? You actually want to invite me to your game show?");
                character_line(symbol, "There's heaps of thingies I gotta check first, ay?");
                printf("Will you check if you're eligible for Matilda's game? (y / (n =  battle))\n");
            }
            break;
        case '#':
            enemy_ptr->ai = FERAL_behavior;
            character_line(' ', "(Right outside the exit of the lobby was a jungle. We explored for a few hours with no contact.)");
            character_line('^', "Navigating through this dense wood is exhausting. Are we almost there yet?");
            character_line('|', "Don't be so lazy! Keep up what you're doing, and you'll get there in time.");
            character_line('^', "I can't help but question that stereotyped saying.");
            character_line(symbol, "pyow...");
            character_line('|', "Hear that growl? That kid might be nearby. I recommend you proceed carefully.");
            character_line('^', "I'll try my best. He's probably up on the branches or behind a tree.");
            character_line(symbol, "PYOW BANG! Hack. Pyow?");
            character_line('^', "What in the world?! Where'd this come from?!");
            character_line('|', "Hurry, and free yourself from this trap! You must win this game, man.");
            character_line(symbol, "Grrr...");
            return FALSE;
            break;
        case '&':
            enemy_ptr->ai = ARTIST_behavior;
            character_line(' ', "(The jungle ended, and I found myself on a cliff. The vast ocean could be seen from this point.)");
            character_line(symbol, "...");
            character_line('^', "She's still working on that drawing of the ocean. Although, it hasn't progressed much.");
            character_line('|', "Now's your chance to launch a surprise attack! She's cornered herself on this cliff.");
            character_line(symbol, "What do YOU want? Please, all of you, stop approaching me. I'm focused on drawing the ocean, not a massacre.");
            character_line('^', "Oh? There are others that have confronted you in this game already?");
            character_line(symbol, "Yes. A few of them were wholesome, but the others were filled with blind bloodlust.");
            character_line('^', "They're just trying to win the game. What about you?");
            character_line(symbol, "It's not my priority. After I finish my drawing, I can turn my attention to this game.");
            character_line('|', "You can't leave her here, mate! She'll be a larger threat when she's done drawing her doodles.");
            character_line('^', "Maybe you're right. Conviction is a powerful asset to have.");
            character_line(symbol, "Pardon?");
            character_line('^', "Sorry, but this drawing won't come into fruition. I came all this way to you.");
            character_line(symbol, "I've come all this way as well...");
            character_line('|', "She fights with her emotions, so it might not be clear what's happening. Fight on!");
            return FALSE;
            break;
        case '!':
            enemy_ptr->ai = &POLITICIAN_behavior;
            character_line(' ', "(The lobby was like a party house. There were decorated tables, food, and drinks all about.)");
            character_line('^', "There are still many dishes of hot food on the tables here. Seems like the lobby is pretty empty now.");
            character_line('|', "Wait. There's that guy in the corner. He's sitting comfortably, legs crossed, with a plate of barbeque.");
            character_line(symbol, "Ho! I didn't see you there. Well, what is it? I'll spare some time for you. Yes, you.");
            character_line('^', "You're not worried that I'll defeat you if we fight? I don't think you're acting urgently enough.");
            character_line(symbol, "It could be a fake facade, but it's not. I think I'll win this game. I have to.");
            character_line('^', "But why? Why are you so confident you'll win when anything can happen?");
            character_line(symbol, "There's a great, great chance that I'll win. Absolutely GREAT. That's what I mean.");
            character_line('^', "If I challenged you to a fight right now, what would be the odds that you would win?");
            character_line(symbol, "No need to address such a triviality. I wouldn't, wouldn't! Accept that offer.");
            character_line('^', "If you don't put up a fight, the odds are in my favor. Who wouldn't be opposed to that?");
            character_line(symbol, "Believe me. Have some faith. I won't hesitate to savagely kill you.");
            character_line('|', "Well, you've gotta get this guy out of the game at some point. If not now, when?");
            character_line('^', "Maybe you don't get this often, but I don't believe any words that you spout out of your mouth.");
            return FALSE;
            break;
        case '/':
            enemy_ptr->ai = &HACKER_behavior;
            if (fate_returner('\\') == CONVICTED) {
                character_line('^', "Salutations, Lachlan. Quite a beautiful view up here.");
                character_line(symbol, "What're you doing here? Did you make your wish yet?");
                character_line('^', "Yes. Didn't you see what happened to your sister? You have a clear shot from here.");
                character_line(symbol, "I did. It was a fascinating watch, but she's still very much alive. In that sword of yours.");
                character_line('|', "Oho! He's a knowledgeable criminal, the worst of the worst.");
                character_line('^', "By the will of my wish and the conviction of my contract, I demand a fight!");
                character_line(symbol, "Have it your way. I'll do what my sister couldn't do and kill you!");
                return FALSE;
            }
            else {
                character_line(symbol, "Hey, you'll be stayin' in the lobby until the others are picked up. Don't do anythin' stupid.");
                character_line('^', "Got it, you can trust on me. How long will it be until the game starts?");
                character_line(symbol, "Dunno, don't care. Sit tight, we need only a few more contestants. Feel free to eat from the buffet.");
                character_line('^', "Okay, thanks. See you.");
                character_line('|', "What in the world are we doing here? This is no vacation!");
                character_line('^', "Might as well try some new things while the changes are processing. That's my perspective.");
                character_line(symbol, "I forgot to add somethin'. There are some contestants here already, so don't be surprised. I'm off.");
                character_line('^', "It seems like we've got time to kill. Let's take a little tour around the island.");
                character_line('|', "Feel free to take a lunch break. It's been some time since you've last ate.");
                character_line('^', "Sure, the food here looks deceptively tasty.");
                sleep(1);
                character_line('^', "... And it is. It's delectable. Actually so good.");
                character_line('|', "Good on you. If you're done stuffing yourself like a pig, hustle on outside!");
                character_line('^', "I'm going, I'm going! ... Still seems a bit desolate here save some birds over there.");
                character_line('|', "Well it should SEEM desolate to YOU. Remember what we told you? You only exist in the world of crime.\n"
                                    "Guess what? Maybe some of these contestants are normal people!");
                character_line('^', "Drat, I forgot about that. What a lousy job.");
                character_line('|', "I think you meant \"LONELY\", boy. Don't take my word for it, but this job is COOL!");
                character_line('#', "Pyow hack hack pyow! bOOM BOom? Yes?");
                character_line('^', "Pardon? What was that about the hackerman?");
                character_line('#', "HACK HACK pyow! Yes!");
                character_line('^', "Please, I need assistance with this one. Now's the time to work your magic!");
                character_line('|', FERAL_intro_1);
                character_line('|', FERAL_intro_2);
                character_line('|', FERAL_intro_3);
                character_line('^', "... Nice meeting you. I'll definitely be seeing you in the game.");
                character_line('#', "BOOM! Pyow pyow pyo...");
                character_line('|', "I'm unable to decipher that language, so there's no way I'll be able to translate between you two.");
                character_line('^', "It's fine. We'll just have to keep a close eye on that Chike kid. He might be an issue in the game.");
                character_line('|', "Yep, and if Matilda was impressed by his strength, who knows what he's capable of?");
                character_line('^', "Look over yonder. By the ridge, there's a woman.");
                character_line('|', "Good eye. C'mon, let's go talk to her.");
                character_line('^', "Hey there!");
                character_line('&', "Hello.");
                character_line('^', "What are you doing here, miss? This a vacation or something?");
                character_line('&', "Not at all, haha. I'm here to draw inspiration from the ocean. It's an empty home out there.");
                character_line('^', "Right... So what's so interesting about this island, if you don't mind me asking?");
                character_line('&', "This is the most water I've ever seen. Look at how the waves complement the azure skies. It's like the\n"
                                    "two were made for each other? Yet, there's a dividing line between them. Dangerously close and so far at the\n"
                                    "same time! Nobody should relate to this, but it must be known.");
                character_line('|', "She's an interesting character, that's for sure. Oh, she started drawing on her canvas.");
                character_line('^', "Tell me more about yourself.");
                character_line('|', "She's too focused on drawing right now. Maybe if you try again, she'll respond this time?");
                character_line('^', "Pardon, will you listen to what I have to say?");
                character_line('&', "Piss off, I'm in the zone!");
                character_line('|', "I guess I'll have to tell you myself.");
                character_line('|', ARTIST_intro_1);
                character_line('|', ARTIST_intro_2);
                character_line('|', ARTIST_intro_3);
                character_line('|', ARTIST_intro_4);
                character_line('^', "I see. Well, carry on with your drawing. I'm outta here.");
                character_line('&', "Good riddance. How beautiful this sight is to behold! I hate this, I think?");
                character_line('^', "We'll leave that girl alone. She seems like she's an explosive one, to say the least.");
                character_line('|', "Shhh... Somebody's approaching you! Get ready to... TALK!");
                character_line('!', "...");
                character_line('^', "Hello, mister.");
                character_line('!', "Greetings. Now, what do you have to offer me, if anything? Anything at all?");
                character_line('^', "Excuse me? I don't understand what you mean by that. I'm just being my friendly self.");
                character_line('!', "I apologize for my aggressive nature, but I must be going now. This game is FAR too important to be slacking off.");
                character_line('^', "Really? What's an old geezer like him doing here? He actually passed the physical test?");
                character_line('|', "But wait! Here's what I've to tell you.");
                character_line('|', POLITICIAN_intro_1);
                character_line('|', POLITICIAN_intro_2);
                character_line('|', POLITICIAN_intro_3);
                character_line('|', POLITICIAN_intro_4);
                character_line('^', "Let's check for more people on the island. There might be more.");
                printf("You search the island for more contestants. You find nobody else in your search.\n");
                sleep(1);
                character_line('|', "There's nothing to it. We'll just have to wait until the game begins.");
                character_line('^', "So it's a waiting game, huh?");
                printf("You remain on the island until a bell rings loudly.\n");
                sleep(1);
                return TRUE;
            }
            break;
        case 'X':
            enemy_ptr->ai = &JUDGE_behavior;
            character_line(' ', "(I was occupied with patrolling when suddenly, cold air started to encircle me. A shadow appeared before me.)");
            character_line(symbol, "Where are my manners. I'm the boss you never knew you had!\n"
                                   "This is my balance, my scale, if you prefer that name. Do you know where I'm headed?");
            character_line('^', "Who or what ARE you?");
            character_line(symbol, "Truthfully, I don't remember. All I know is, you've been defying me, your... EVERYTHING.");
            character_line('|', "It's the one that invented me, my only enemy. I see now.");
            character_line('^', "Your enemy? What should I do?");
            character_line(symbol, "You do know you have a quota to withhold? Man, brats like you...");
            character_line(symbol, "I adore them so.");
            return FALSE;
            break;
    }
    while ((keystr = getaction(Controller)) != _EOF && keystr != Yes && keystr != No)
        ;
    PlaySound(NULL, NULL, 0);
    if (keystr != _EOF) {
        if (keystr == TRUE)
            return TRUE;
        else {
            switch (enemy_ptr->rep) {
                case '+':
                    PlaySound(TEXT("Music\\Wav Files\\+\\+_Battle_Prelude.wav"), NULL, SND_SYNC);
                    break;
            }
            //PlaySound(TEXT(battle_file), NULL, SND_ASYNC | SND_LOOP);
            return FALSE;
        }
    }
    quit();
}

/* decline : cues dialogue that happens when you spare the enemy */
void decline(struct Character *enemy_ptr, struct Character *player_ptr) {
    int keystr, symbol, a_fate, round = 1, result;
    float your_score = 0.0, foe_score = 0.0;
    skip_scene = FALSE;
    set_state_vals(Controller, FALSE, FALSE, TRUE, FALSE, FALSE);
    clrscr();
    switch (symbol = enemy_ptr->rep) {
        case 'c':
            character_line('^', "There's nothing to it, boy. Be on your way.");
            character_line(symbol, "Thank goodness. Sorry, but I thought you were going to harm me.");
            character_line('^', "Never would I do that. This is goodbye then.");
            printf("%c : Actually, I see that your shoes are a bit smudged. Care for a polish, sir? (y / n)\n", symbol);
            while ((keystr = getaction(Controller)) != _EOF && keystr != Yes && keystr != No)
                    ;
            switch (keystr) {
                case Yes:
                    extra_store(symbol, MARKED);
                    character_line('^', "Sure, I guess.");
                    character_line(symbol, "Thank you very much, mister! It'll be done in no time.");
                    character_line('|', "Well, he's fancy with his tongue, just like a house cat! Your shoes are looking lustrous.");
                    character_line(symbol, "And I'm done! A job complete is a good job, I always say.");
                    character_line('^', "Not bad, kid. How much?");
                    character_line(symbol, "A single quarter will suffice.");
                    character_line('^', "Alright. I think I've got some quarters on me. There you go.");
                    character_line(symbol, "Thanks! Have a nice day!");
                    character_line('|', "How about it? Did he make your day or what? You're marked now for surgery. Great and fantastic!");
                    break;
                case No:
                    character_line('^', "No.");
                    character_line(symbol, "It'd only cost you a quarter! How about it?");
                    character_line('^', "No means no. I waste your time and now, you waste mine? That's not nice.");
                    character_line(symbol, "It was but an offer, sir. Whatever, I'm off.");
                    character_line('|', "He's gone! Looking for oblivious victims, eh? Why didn't you stop him?");
                    break;
                case _EOF:
                    quit();
                    break;
            }
            break;
        case '+':
            if (fate_returner('+') == ALERTED) {
                character_line('^', "My apologies, but I haven't seen any boys outside.");
                character_line(symbol, "Oh. I'm afraid I have to close early today, sir. He was like a son to me.");
                character_line('^', "I'm sorry to have heard that.");
                character_line('|', "How bold of you to only get the kid. Does this barber appeal to you in some way?\n"
                                    "Or maybe his contributions to the medical field will outweigh his crimes? Can we\n"
                                    "be positive that advancement in society is always morally good?");
            }
            else {
                printf("%c : What's wrong, master? Are you actually here for a trim? (y / n)\n", symbol);
                while ((keystr = getaction(Controller)) != _EOF && keystr != Yes && keystr != No)
                    ;
                switch (keystr) {
                    case Yes:
                        character_line('^', "Yeah, I am. How much is it again?");
                        character_line(symbol, "It's always a clean $40. Make sure to take off your shoes, and I'll be with you in a bit.");
                        character_line('^', "Yep. I'll just chill over here in the meantime.");
                        character_line('|', "Seriously? Listen to my advice, man!");
                        character_line('^', "Shush it. I just really needed a haircut. I mean, look at me!");
                        character_line('|', "True. Looking at you neck up, you have a striking resemblance to bigfoot!");
                        character_line('^', "That's a bit harsh.");
                        character_line('|', "This is boring.");
                        character_line('^', "Look at the walls. Those paintings are very oriental. The flowers complement them well.\n"
                                            "Those masks on the wall are pretty as well. It's a demon, a fox, and a boar.");
                        character_line(symbol, "Alright. We're ready to start our session.");
                        if (extra_returner('c', MARKED)) {
                            extra_store(symbol, SURGERY);
                            character_line('^', "Great. Just make my ha-");
                            character_line('|', "What a great partner I've been paired with, knocked out from a random pipe.");
                            character_line('|', "Hey, wake up. He's going to drug you! Okay then, don't say I didn't warn you.");
                            character_line(symbol, "Great, you're finally here, Cue. It's been a while since we've last got a hit.");
                            character_line('c', "Oh, it's him. He randomly stopped me and agreed to have his shoes licked. I was kind of scared...");
                            character_line(symbol, "He's armed. He didn't hurt you, did he? Take those items to the couch over there.\n"
                                                           "Don't worry. We'll give it back to him after we release him.");
                            character_line('c', "I did it. What's the operation, another test case number 5 surgery?");
                            character_line(symbol, "I was thinking about something else. Here, this is what we'll do. Low chance of fatality.");
                            character_line('c', "That's relieving to hear. Operation start!");
                            character_line('|', "What are they doing to you? That's only slightly disturbing. Here's to your health!");
                            character_line(symbol, "Operation successful. You did a good job, Cue! I've got some new findings, too.");
                            character_line('c', "Glad to hear that. Is the man okay?");
                            character_line(symbol, "He made it through all the incisions. Once he wakes up, he'll lose his memory of his visit.\n"
                                                           "I'll just place him outside by the garbage bins. Right, here's your share before I forget.");
                            character_line('c', "I'm going back now, sir. Bye.");
                            character_line(symbol, "Have a good day, Cue.");
                            sleep(3);
                            character_line('^', "Ughhh, where am I?");
                            character_line('|', "Finally operational! You're by the trash, where you belong. This is your home.");
                            character_line('^', "That's not right... I remember that much at least.");
                            character_line('|', "You fell into their trap! You were operated on by a barber or maybe you wanted that?");
                            character_line('^', "My muscles are tingling, my body feels very peculiar.");
                            center_screen(WIDTH, "%s\n", "Your attack power increased by 1.");
                            center_screen(WIDTH, "%s\n", "Your maximum health decreased by 5.");
                            ++player_ptr->attack;
                            player_ptr->max_health -= 5;
                            player_ptr->health = minimum(player_ptr->max_health, player_ptr->health);
                            sleep(5);
                        }
                        else {
                            character_line('^', "Great. Just make my hair look nice, not like the mess it is now.");
                            character_line(symbol, "Of course, master. Relax, please.");
                            character_line('^', "Those masks over there are veyr cool. Could you tell me more about them?");
                            character_line(symbol, "Oh yes. The red mask is an oni mask, a creature with horns. Their hair is usually crazy, so\n"
                                                           "nobody requests that hair \"style\". The meaning of the oni depends on your culture.\n"
                                                           "Some people believe the oni is wicked and a sign of misfortune to come. Others think of the oni\n"
                                                           "as a guardian that prevents bad luck. They're completely different representations, yes?");
                            character_line('^', "Yeah. That's kind of weird.");
                            character_line(symbol, "Personally, I think of the oni as an unlucky parent. They provide for their child or other loved ones.\n"
                                                           "However, only the worst of people become oni. The oni may care for the child, but the child must\n"
                                                           "never be like the oni. It's a complicated representation, like it combined all of them together.");
                            character_line('^', "Yeah. It's an interesting take, that's for sure.");
                            character_line(symbol, "There we go! We're done, master. Take a look.");
                            character_line('^', "Wow, I look much better. Thank you very much, and here's your $40.");
                            character_line(symbol, "Thank you for your time. Don't hesitate to come back for another trim.");
                        }
                        break;
                    case No:
                        character_line('^', "Sorry to have disturbed, I'm at the wrong establishment.");
                        character_line(symbol, "Well, come by any time you need a trim. Cheers.");
                        break;
                    case _EOF:
                        quit();
                        break;
                }
                if (fate_returner('c') != NEVER_MET)
                    character_line('|', "Another one spared! What a joke. The dynamic duo, they'll kill again, you know...");
                else
                    character_line('|', "What happened? Did he not seem blatantly evil? Stop this madness.");
            }
            break;
        case 'M':
            if (!extra_returner('M', RETURNING)) {
                extra_store('M', RETURNING);
                character_line(symbol, "Yes? What's taking so long, soldier!?");
                character_line('^', "Sorry, miss. Be on the lookout for suspicious individuals and make sure your parents are fine later.");
                character_line('|', "Of course. My lovely parents, I love them! We're going to the park, everyone! Cheer up, yay!");
                character_line('^', "Have fun, I guess.");
                if (fate_returner('c') == SPARED)
                    character_line('|', "You big softie. Kids can grow up, true, but WILL they grow up? Will their parents do anything to help?");
                else
                    character_line('|', "To the park? Well, I hope her ants have enough to go around.");
            }
            else {
                character_line(symbol, "Apologize! This instance! I refuse to be treated like this a SECOND time.");
                character_line('^', "Gee, I apologize from the bottom of my heart. I take it you can get back to your parents then.");
                character_line(symbol, "But of course. Just... Come back the way I came! Isn't that right, everyone?");
                character_line('^', "If you say so, miss. I'd like to reiterate this point strongly...");
                character_line(symbol, "Yes, yes. I accept your apologies from the bottom of my heart! Is that good?");
                character_line('^', "I meant the stranger danger, not that nonsense! Look out for weird guys, alright?");
                character_line(symbol, "Don't ridicule me, I remember what you said earlier. Of course, I despise those weirdo workers\n"
                                       "that approach me at times. What business do they have with me, I always ask myself? And! It seems\n"
                                       "like I can never surmise a plausible answer to it!");
                character_line('^', "The reason is...");
                character_line(symbol, "Do I look entertained? I'd rather play than listen to your boring speech. Bye bye!");
                character_line('^', "That was pretty harsh of her.");
                character_line('|', "Ugh, this is saddening to witness. Sorry, I'm disgusted by how that went down.");
            }
            break;
        case '%':
            character_line('^', "On second thought...");
            character_line(symbol, "That's what I thought. Y'know what, forget about it. I've got work to do, officer.");
            character_line('^', "We're watching you. Don't blunder at your peak.");
            character_line(symbol, "Indeed, wiseguy. Make the most of your day, officer.");
            character_line('^', "He's off but still seems distressed at something. Ah, the struggles of working everyday...");
            character_line('|', "Mayhaps. I get why you didn't go after him, but think of the fame you would get! A news article, maybe?");
            character_line('^', "You think a newbie like me could take him? You're giving me too much credit.");
            break;
        case '$':
            character_line('^', "Nevermind. Carry on with your day, mister.");
            character_line(symbol, "Bye.");
            character_line('^', "He's quite a quiet one...");
            character_line('|', "And a bit insane, wouldn't you say? I didn't think you'd allow your winning streak to end so quickly!");
            break;
        case '*':
            character_line('^', "Carry on. You seem genuine enough.");
            if (fate_returner('c') == CONVICTED || fate_returner('M') == CONVICTED)
                character_line(symbol, "Thanks. I can't really say the same thing about you. Something feels amiss.");
            character_line('|', "All is lost! He will be the one that carries out Armageddon, mark my words!");
            character_line(symbol, "Anyway, can you leave me in peace? Actually, I'm gonna check the fuse box by the back. Peace out.");
            character_line('^', "Sure. Something feels amiss, that's for sure...");
            break;
        case 'L':
            character_line('^', "Well, it looks orderly, and I've got my next job. This is goodbye.");
            character_line(symbol, "Oh, is that all there is to the safety check? I got nervous for nothing!");
            if (total_fate(CONVICTED) > 1) {
                character_line(symbol, "Oh! Before you leave, would you care to try my salisbury steak? It's very good, I swear! (y / n)");
                while ((keystr = getaction(Controller)) != _EOF && keystr != Yes && keystr != No)
                    ;
                switch (keystr) {
                    case Yes:
                        extra_store(symbol, TREATED);
                        character_line('^', "Sure, I'll bite.");
                        character_line('|', "Already having a lunch break on the job? Geez, man.");
                        character_line(symbol, "... Well? It's world-class, isn't it!");
                        character_line('^', "It's such a moist steak. So flavorful as well, it's perfect.");
                        health_healed(player_ptr, enemy_ptr, 20);
                        character_line(symbol, "Thank you. Come back anytime, and I'll have some ready just for you, okay?");
                        character_line('^', "I'll think about it. Onwards to the next job.");
                        character_line('|', "Yeah, we're staying clear of this place from now on.");
                        character_line(symbol, "Be sure to come back! I've got to finish up the meals.");
                        break;
                    case No:
                        character_line('^', "No thanks. I don't eat food from a stranger.");
                        character_line(symbol, "Aw, you're no fun. It's clean! And tasty! Smell it.");
                        character_line('^', "If I ate that, there'd be less to go around. I can feed myself, thanks.");
                        character_line(symbol, "Okay then. I guess it's back to cooking.");
                        character_line('|', "I wonder if it was spiked. Your surname isn't Smith, is it?");
                        break;
                    case _EOF:
                        quit();
                        break;
                }
            }
            if (extra_returner('*', TRAILED)) {
                character_line('^', "Wait. Did you burn something in the kitchen? It's smelling terrible now.");
                character_line(symbol, "Oh no! Maybe. This is embarrassing, haha. I'll check up on it.");
                character_line('^', "Hey, watch out! Are you okay?!");
                character_line(symbol, "Is there a fire extinguisher somewhere here? Please? I'm trapped in here, help!");
                character_line('|', "This is why we take precautions when cooking! Hurry up and find something to put the fire out!");
                character_line('^', "I'm trying, okay?! Don't panic, don't panic...");
                sleep(1);
            }
            else {
                character_line('|', "Well, that was a waste of time. You're managing to meet your quota still, right?");
                character_line('^', "Yeah, I'm getting there, that's for sure.");
            }
            break;
        case '?':
            character_line('^', "I'll play for a bit, sure. Could use a break.");
            if (extra_returner('L', TREATED))
                character_line('|', "Oh yeah? Another break for our employee of the month!");
            else
                character_line('|', "You better not lose, y'hear? Your pride's on the line, man.");
            character_line(symbol, "What games did you have in mind? I'm down to play any of them!");
            printf("What game did I have in mind? (t = tic-tac-toe / s = rock paper scissors / R = Russian roulette)\n");
            while ((keystr = getaction(Controller)) != _EOF && keystr != TicTacToe && keystr != Down && keystr != RR)
                ;
            switch (keystr) {
                case TicTacToe:
                    character_line('^', "Tic-tac-toe, a classic.");
                    character_line(symbol, "Okay! Thoooough, it's not really a game of chance.");
                    character_line(symbol, "We'll play a best of 5 where draws give 0.5 points. How about that?");
                    while (your_score < 3.0 && foe_score < 3.0) {
                        clrscr();
                        printf("^| %.1f - %.1f |? Bo5\n", your_score, foe_score);
                        if (your_score < 1.0)
                            result = play_opponent(RECKLESS, (round % 2) ? 'X' : 'O');
                        else if (your_score >= 1.0 && your_score < 2.0)
                            result = play_opponent(STANDARD, (round % 2) ? 'X' : 'O');
                        else
                            result = play_opponent(INTELLIGENT, (round % 2) ? 'X' : 'O');
                        switch (result) {
                            case DRAW:
                                your_score += 0.5;
                                foe_score += 0.5;
                                break;
                            case WIN:
                                your_score += 1.0;
                                break;
                            case DEFEAT:
                                foe_score += 1.0;
                                break;
                        }
                        ++round;
                    }
                    clrscr();
                    printf("^| %.1f - %.1f |? Bo5\n", your_score, foe_score);
                    sleep(3);
                    if (your_score == foe_score) {
                        character_line(symbol, "Congrats, you won!");
                        character_line('^', "Good games.");
                        character_line('|', "Nice job. What're you, a tic-tac-toe champion?");
                        character_line(symbol, "That was fun! Here, take this. I've gotta run. May luck be by your side always!");
                        center_screen(WIDTH, "You got a shard of a broken die from Dalia.");
                        add_item(&first_item, STRENGTH);
                        sleep(3);
                    }
                    else if (your_score > foe_score) {
                        character_line('^', "You won. Congratulations on your victory.");
                        character_line(symbol, "Thanks. You played well too. Anyway, I've gotta run. May luck be by your side always!");
                        character_line('|', "You got destroyed, mate. Now get going!");
                    }
                    else {
                        character_line(symbol, "Darn, we tied. That's a bit disappointing.");
                        character_line('^', "Good games though.");
                        character_line(symbol, "Yup. Anyway, I've gotta run. May luck be by your side always!");
                        character_line('|', "Look at the bright side: you almost had her, just maybe. Maybe if you played better, you know.");
                    }
                    break;
                case Down:
                    character_line('^', "Rock paper scissors, a classic.");
                    character_line(symbol, "Ooh, that's one of my favorites. Most of the time it's truly a game of luck.");
                    character_line('^', "Let's play the traditional version, if that's fine with you.");
                    character_line(symbol, "Best of seven, of course. Andddd let's get into the games!");
                    while (your_score < 4.0 && foe_score < 4.0) {
                        clrscr();
                        printf("^| %d - %d |? Bo7\n", (int) your_score, (int) foe_score);
                        switch(shoot()) {
                            case 1:
                                ++your_score;
                                break;
                            case 2:
                                ++foe_score;
                                break;
                        }
                        sleep(2);
                    }
                    clrscr();
                    printf("^| %d - %d |? Bo7\n", (int) your_score, (int) foe_score);
                    sleep(3);
                    if (your_score > foe_score) {
                        character_line(symbol, "Congrats on your win!");
                        character_line('^', "Good games.");
                        character_line('|', "Nice job, luckster. We really have to get going as well. Hop to it!");
                        character_line(symbol, "I was giving it my all too. Your luck is very impressive...");
                        character_line(symbol, "Please take this gift. Anyway, I've gotta run. May luck be by your side always!");
                        center_screen(WIDTH, "%s\n", "You got a shard of a broken die from Dalia.");
                        add_item(&first_item, TEMPER);
                        sleep(3);
                    }
                    else {
                        character_line('^', "You won. Congratulations on your victory.");
                        character_line(symbol, "Thanks. I got super lucky! Anyway, I've gotta run. May luck be by your side always!");
                        character_line('|', "Of course she would win a game of chance, so don't fret. Now get going!");
                    }
                    break;
                case RR:
                    character_line('^', "Russian roulette, a classic.");
                    character_line(symbol, "Wait, isn't it a dangerous game? Um...");
                    character_line('^', "You DID say you were down to play any games where the victor is decided by luck. Come on.");
                    character_line('|', "As per the guidelines, you'll have to convince ME to be able to use your revolver.");
                    character_line('^', "All I need is one live round. Think about it. No matter the outcome, justice will be served.");
                    character_line('|', "I can't really argue against that, can I? I'll allow it, just this once. Don't lose your head now.");
                    character_line('^', "So? Will you play? Look, there's a revolver we can use.");
                    character_line(symbol, "I guess. If I don't play, *Fortuna* will look down on me. So, I must! It's my turn.");
                    while (TRUE) {
                        clrscr();
                        printf("^| %d - %d |? Bo1\n", (int) your_score, (int) foe_score);
                        printf("Dalia spins the cylinder and points the revolver toward her head. She pulls the trigger...\n");
                        sleep(2);
                        if ((result = rand() % 7) == 6) {
                            ++your_score;
                            printf("A shot rings out.\n");
                            player_ptr->karma += 7;
                            break;
                        }
                        else {
                            printf("Nothing happened.\n");
                            printf("Dalia tosses the revolver to you with a sigh of relief.\n");
                        }
                        character_line('^', "It's my turn...");
                        printf("You spin the cylinder and point the revolver toward your head. You pull the trigger...\n");
                        sleep(2);
                        if ((result = rand() % 7) == 6) {
                            ++foe_score;
                            printf("A shot rings out.\n");
                            break;
                        }
                        else {
                            printf("Nothing happened.\n");
                            printf("You toss the revolver over to Dalia.\n");
                            sleep(2);
                        }
                    }
                    printf("^| %d - %d |?\n", (int) your_score, (int) foe_score);
                    sleep(3);
                    if (foe_score == 1) {
                        player_death();
                        exit(EXIT_SUCCESS);
                    }
                    center_screen(WIDTH, "%s\n", "You find a shard of a broken die on the floor.");
                    fate_store(symbol, CONVICTED);
                    add_item(&first_item, JUDGEMENT);
                    sleep(3);
                    break;
                case _EOF:
                    quit();
                    break;
            }
            break;
        case 'C':
            character_line('^', "Nothing. You'll have to walk all the way back if you have no car.");
            character_line(symbol, "Yes, I know. Don't worry, I can handle this much at least. Pardon me...");
            character_line('^', "She's off. I wonder how far away the car is from here.");
            character_line('|', "I wonder if she got a catch this time. And also if she will get another catch during this trip.");
            break;
        case '@':
            character_line('^', "Fine, have it your way. I was clearly in the wrong here.");
            character_line(symbol, "Finally, you say something reasonable! Thanks to you, my mood is ruined.");
            character_line('^', "My apologies. Feel free to drive somewhere else if that makes you comfortable.");
            if (extra_returner('*', TRAILED) && fate_returner('*') != CONVICTED) {
                character_line(symbol, "I'll do that, yeah. Wait, is that your subordinate behind you? Tell him to stand down.");
                character_line('^', "What? Behind me...?");
                character_line('*', "Surprise! Explode in 3, 2, 1! Go!");
            }
            else {
                character_line(symbol, "I'll do that, yeah. Strange, it's like your entire character has changed. Anyway, don't bother me\n"
                                       "again if you see me. I may not look it, but I'm really busy.");
                character_line('|', "True, my man! What happened to the aggression? You were so eager to search his trunk!");
                character_line('|', "Don't give me that confused look! You should be the one who's least confused.");
            }
            break;
        case '\\':
            /* activate your wish */
            if (extra_returner(symbol, WON)) {
                character_line(symbol, "I'll fly ya back now. Your wish will be remembered!");
            }
            /* compete in the game */
            else {
                character_line('^', "Sure, I'm down to play it. Tell me the rules.");
                character_line(symbol, "Deadset, it's a secret.");
                character_line('^', "I find that unusual and creepy. So, what's with the walkie talkie?");
                if (total_fate(CONVICTED) > -1) {
                    character_line(symbol, "Good on ya! The game is yours, if ya want to play.");
                    printf("Will you play in the game? (y / n)\n");
                    while ((keystr = getaction(Controller)) != _EOF && keystr != Yes && keystr != No)
                        ;
                    switch (keystr) {
                        case Yes:
                            extra_store('\\', COMPETED);
                            character_line('^', "Count me in. This'll be an easy victory for me.");
                            character_line(symbol, "Too right! Come along, I'll fly ya to the lobby.");
                            character_line('^', "Right behind you.");
                            character_line(' ', "(Further down the alley was a jet plane. I sat near the back of the plane.)");
                            sleep(2);
                            character_line('|', "It's been 16 hours on her jet, and you're asleep, seriously? Isn't it REALLY NOISY here?\n"
                                                "Just a tad cramped too, but it's still only you and her inside.");
                            character_line(symbol, "Oi, we're here. Wake up, ay!");
                            character_line('^', "... Huh. Right.");
                            character_line(' ', "(The jet had landed on an island. Through the window, I saw a lofty building. This must be the lobby.)");
                            character_line(symbol, "This bloke of a brother will get ya settled in. Catch ya later.");
                            player_ptr->health = player_ptr->max_health;
                            break;
                        case No:
                            character_line('^', "Not interested. I don't even know what game I'll have to play.");
                            character_line(symbol, "What a shame.");
                            character_line('^', "She's vanished already. No further persuasion or elaboration? What a scam!");
                            character_line('|', "True, so maybe stop her next time, mate?");
                            break;
                        case _EOF:
                            quit();
                            break;
                    }
                }
                else {
                    character_line(symbol, "Sorry, ya can't join. Review the conditions again, please!");
                    character_line('^', "At least tell me where I can find this information. And she's gone already, really?");
                    character_line('|', "Looks like you don't strike her as a strong fellow. Better luck next time!");
                }
            }
            break;
        case '/':
            character_line(symbol, "Attention, all contestants! The 15th one has joined us.");
            character_line('\\', "Don't go crook on me when I break this rage, all!");
            character_line(symbol, "Yes, time for us to explain the game.");
            character_line('\\', "I reckon!");
            character_line('!', "We're listening intently. Carry on, lass.");
            character_line('&', "Yes, please do.");
            character_line('#', "Pyow.");
            character_line('\\', "We'll start shortly, ya bastards!");
            character_line(symbol, "The rules.");
            character_line('\\', "Right, right. Please off each other on this island! Thank ya!");
            character_line(symbol, "You heard her. Kill each other and be the last one standing to win. There are weapons scattered around.\n"
                                   "Try and leave the island, and I'll kill you myself with this here rifle.");
            character_line('#', "PYOW pyow pyo...");
            character_line('!', "Hmph. If I have to do it, I must do it.");
            character_line('&', "... I don't want to. All I want to do is draw...");
            character_line('|', "What mayhem is unraveling before us! Too bad we can't see all of the contestants.");
            character_line('^', "It doesn't matter. Let's do this!");
            character_line(symbol, "I'm going to head off. Dear sister, start the game when I'm in position.");
            character_line('\\', "Now, get on with this piece of piss! I'll be watching ya all!");
            character_line('|', "You heard the lady. Remember, all you have to do is defeat the three of them.");
            character_line('^', "If we need nourishment, we can eat in the lobby hall.");
            character_line('|', "You've still got a quota to meet, so pay attention! We go!");
            break;
    }
    fate_store(enemy_ptr->rep, SPARED);
}

/* finished : displays dialogue from a character after they have been defeated, set karma number */
void finished(struct Character *player_ptr, struct item *first_item_ptr, struct Character *enemy_ptr) {
    int keystr, symbol;
    skip_scene = FALSE;
    clrscr();
    switch (symbol = enemy_ptr->rep) {
        case 'c':
            character_line(symbol, "Fine! Fine, I relent. That hurts...");
            character_line('^', "What is it you wish to say to me, boy? You pulled a knife on me!");
            character_line(symbol, "I'll tell you about the others, my boss and the people that died to my hands. Just-");
            character_line('|', "We'll hear all about it, at the station. Now come, don't dilly-dally!");
            if (!total_fate(CONVICTED))
                character_line('^', "...my sword, it... it captured the boy. He was right there in front of me, crying.\n"
                                    "So this is the power...");
            center_screen(WIDTH, "%s", "You find a pair of boots where the boy last stood.");
            add_item(&first_item, RUSH);
            sleep(3);
            player_ptr->karma += 7;
            break;
        case '+':
            if (extra_returner('+', ALERTED)) {
                character_line(symbol, "I'll kill you, like you did to Helix!");
                character_line('^', "Hold up. Cue was just captured by my sword, drop your weapons.");
                character_line(symbol, "Captured! You expect me to trust that? Are you aware of what you people do to us?");
                character_line('|', "Here is a display! Come on in, it's paradise inside!");
            }
            else {
                character_line(symbol, "You're going to brand me as a criminal, aren't you?!");
                character_line('^', "I know of your misdeeds, yes. You need to come with me, sir, before you hurt anymore.");
                character_line(symbol, "Wait! My son, he needs me! He has no father!");
                if (fate_returner('c') == CONVICTED)
                    character_line('|', "If you insist so! Cue, here comes papa!");
                else
                    character_line('|', "What a defense mechanism. Now, come here, Hayato Arata.");
                if (!total_fate(CONVICTED))
                    character_line('^', "Wow...my sword absorbed the man. Scary...");
                else
                    character_line('^', "Good going.");
            }
            center_screen(WIDTH, "%s", "You search the barbershop and obtained a fantastical elixir.");
            add_item(&first_item, HEAL);
            sleep(3);
            player_ptr->karma += 13;
            break;
        case 'M':
            character_line('^', "You're done for. Fire ants are no match for me.");
            character_line(symbol, "No! Wake up, everybody...");
            character_line('|', "I'm sorry to say these were not cockroaches, little one!");
            character_line(symbol, "Perhaps some will be at the sandbox. I'll remember you, one with the cap!");
            character_line('|', "You can't leave now. Know what happens to losers?");
            character_line(symbol, "What's happening?!");
            if (!total_fate(CONVICTED))
                character_line('^', "What the!? The girl was launched and vanished into my sword. This power...");
            center_screen(WIDTH, "%s", "You search the suitcase and obtained a pretty shield.");
            add_item(&first_item, GUARD);
            sleep(3);
            player_ptr->karma += 9;
            break;
        case '%':
            character_line('^', "Your fighting spirit has dissipated. This is checkmate.");
            character_line(symbol, "How? Me, pinched? My other weapons up and ran! Who's the rat or whatever you paid off?");
            character_line('^', "I don't know what you speak of. We just happened to overhear your troubles.");
            character_line(symbol, "You damn pigs. What'll happen now? Don't get the wrong idea, I'm no rat. You get nothing at all.");
            character_line('|', "Well, let's see about treating that wound first. Oh. Slip of the blade. Guess you're stuck with me!");
            if (!total_fate(CONVICTED))
                character_line('^', "Some invisible force launched him into my sword. He's gone...");
            else
                character_line('^', "That was a serious threat, but I'm glad that's over with.");
            center_screen(WIDTH, "%s\n", "You find a large bottle of alcohol that is rightfully yours now.");
            add_item(&first_item, RECOVER);
            sleep(3);
            player_ptr->karma += 20;
            center_screen(WIDTH, "%s\n", "You notice a man standing by Severino's body who seemed to have suddenly popped up.");
            sleep(3);
            break;
        case '$':
            character_line('^', "Not so tough when you're in an actual fight where your foe can fight back.");
            character_line(symbol, "What to do, father? Help me!");
            character_line('|', "Not so fast! Nobody's going to allow you to make like a tree and leave. To me!");
            character_line('^', "Exactly. Didn't you see what happened to the other guy here, the one you were stealing from?");
            character_line('|', "He depends on his father too much. Think on your own and depend on yourself.");
            center_screen(WIDTH, "%s\n", "You find all your items in Nemo's handbag, which you take as well.");
            add_item(&first_item, TRICK);
            sleep(3);
            player_ptr->karma += 11;
            break;
        case '*':
            character_line('^', "Your flames did not fear me away. You've lost.");
            character_line(symbol, "What?! It's over? I'm... still weak when it counts.");
            character_line('^', "I have to say, the sheer amount of stuff you have in that toolbox is impressive.");
            character_line(symbol, "That's your compliment of choice? How pathetic of a hero I am.");
            character_line('^', "Am I supposed to pity you? Hey, you must have something in there to extinguish fires, right?");
            character_line(symbol, "Yeah, of course I do. Leave it to me!");
            character_line('|', "Good work. It's time to say goodbye now! Goodbye, \"samaritan\" Fernandez, nice knowing you.");
            if (!total_fate(CONVICTED))
                character_line('^', "He didn't put up any resistance. My sword just drained his entire body...");
            center_screen(WIDTH, "%s\n", "You find a hot arrowhead in Inigo's toolbox, which you grab.");
            add_item(&first_item, FIRE);
            sleep(3);
            if (extra_returner('*', TRAILED)) {
                character_line('|', "Some of it burned down, huh. Unlucky.");
                character_line('^', "Yeah. Time to go, it's not my job to clean up this mess.");
            }
            player_ptr->karma += 15;
            break;
        case 'L':
            if (player_ptr->buff == FULL || player_ptr->debuff == SICK || player_ptr->debuff == ALMOST_SICK) {
                character_line('^', "Thanks for the food, even if it was a bit overseasoned.");
                character_line(symbol, "Tch! I left my lethal chems at home.");
            }
            character_line('^', "Give it up. Stop your child slaughter; it won't accomplish anything!");
            character_line(symbol, "They're the PROBLEM. Don't you get it!? Why then, why would he leave me?");
            character_line('^', "If your child was the problem, why would he leave with her?");
            character_line(symbol, "Could it be...? All he wanted from me was a baby girl? Absolutely used...");
            character_line('|', "Allow me to end her suffering. This is saddening, this is riveting!");
            if (!total_fate(CONVICTED))
                character_line('^', "She was quickly absorbed by my sword. Impressive.");
            if (player_ptr->buff == SICK || player_ptr->debuff == ALMOST_SICK) {
                center_screen(WIDTH, "%s\n", "You used the shelter's restroom for a good 15 minutes before relief.");
                sleep(3);
                center_screen(WIDTH, "%s\n", "You discover a cornucopia in the restroom storing poisons.");
                add_item(&first_item, FAMINE);
            }
            else {
                center_screen(WIDTH, "%s\n", "You discover a cornucopia in the fridge bearing safe food items.");
                add_item(&first_item, FEAST);
            }
            sleep(3);
            player_ptr->karma += 13;
            if (extra_returner('*', TRAILED)) {
                character_line('^', "Woah, there's a huge fire in here! Is there a fire extinguisher in here?!");
                character_line('|', "Calm down first. Go outside for help before you suffocate in this hell!");
                character_line('^', "Okay, I'll do that. ...There's nothing here that can help!");
                character_line('|', "I told you, go outside. Who knows how fast this fire will spread.");
                character_line('^', "Got it. Stay calm. I can get through this...");
                sleep(1);
            }
            break;
        case '?':
            character_line('^', "Stop! We're done here, you've been bested.");
            character_line(symbol, "How could *Fortuna* be beaten? Impossible! Maybe... you possess the holographic *Fortuna*?!");
            character_line('^', "Of course not. Rely on luck one too many times, and it will let you down. That's the truth.");
            character_line(symbol, "I just need to practice some more. I can't let the Heart of the Cards down!");
            character_line('^', "Still, it's impressive what you can do with your die. It's borderline cheating.");
            character_line(symbol, "Thanks! I can teach you some of my tricks, if you'd like. They're hard to master though!");
            character_line('|', "Wrap it up already. I'm getting ready, so wrap it up!");
            character_line('^', "No thanks. There's no luck in my arsenal.");
            character_line(symbol, "Aww...ahhh!");
            if (!total_fate(CONVICTED))
                character_line('^', "A mysterious force propelled her to my sword, where she disappeared.");
            center_screen(WIDTH, "%s\n", "You picked up the broken die, reduced to a shard.");
            add_item(&first_item, MOON);
            sleep(3);
            player_ptr->karma += 7;
            break;
        case 'C':
            if (enemy_ptr->buff == SUMMON) {
                character_line('^', "You want some more, you weaklings punks?!");
                character_line(symbol, "Where are you all going?! Don't leave me!");
                character_line('^', "They're really just a bunch of cowards. You trust them that much?");
                character_line(symbol, "They help me when I need it. That's all.");
                character_line('^', "A business relationship, where they put themselves over you. That's what I think.");
            }
            else
                character_line('^', "That's all of them...");
            character_line(symbol, "Why are you doing this?!");
            character_line('^', "What? Did you forget everything you have done thus far? Enlighten me, if you don't mind.");
            character_line(symbol, "I... I know. I just need to feed my kids, so I do what I must!");
            character_line('|', "Oh boy. The highly coveted pity card! It's always a blast to witness.");
            character_line('^', "I really do pity you. Just because I pity you doesn't mean you're free to do as you please.\n"
                                "A pitiful person isn't neccesarily a bad or good member of society. But you, you're just bad.");
            character_line(symbol, "I hate what I do for a living, don't you?");
            character_line('^', "All I'll say is that I'm better than you. Also, you should learn from your mistakes...");
            character_line('|', "Well said! It is a fun lifestyle, huh? We've got a whole party over here, but it's over now.");
            if (!total_fate(CONVICTED))
                character_line('^', "My sword sucked the gangsters and Lucinda in like a vacuum cleaner. Good boy.");
            center_screen(WIDTH, "%s\n", "You pick up the baby doll, now morphed in form by your sword.");
            add_item(&first_item, GOLEM);
            sleep(3);
            player_ptr->karma += 20;
            break;
        case '@':
            character_line(symbol, "Gah! What a monster you are...");
            character_line('^', "Got it through your thick mask yet? You're in the wrong!");
            character_line(symbol, "You don't know me! You lucked out in your crazy accusation!");
            character_line('^', "You SHOULD know that I know your criminal activities. My sword told me.");
            character_line(symbol, "No way, that's prime bat dung...");
            character_line('^', "It's true though. My accusation was not made blindly.");
            character_line(symbol, "Still, you have no account of my victims. What if they truly changed afterward!?");
            character_line('^', "If that makes you feel better, good. However, this BLIND claim is powerless without evidence.");
            character_line(symbol, "I'm... I'm sorry.");
            character_line('^', "The best apology is showing that you've truly changed your ways, Dark Knight.");
            character_line('|', "Wow, you were pretty cool this time. As for him, time for rehabilitation! Come get it!");
            if (!total_fate(CONVICTED))
                character_line('^', "The Dark Knight's body and outfit alike was sucked up by my sword.");
            center_screen(WIDTH, "%s\n", "You search the Grimobile, picking up several batarangs.");
            add_item(&first_item, RANG);
            sleep(3);
            player_ptr->karma += 14;
            break;
        case '\\':
            if (extra_returner('\\', FOUGHT)) {
                character_line(symbol, "Ace! No wonder ya won, bloody ripper!");
                character_line('^', "You like that? I reign supreme on this island.");
                character_line(symbol, "Stop yabbering. It's not over!");
                character_line('^', "I'll end it thus!");
                character_line('|', "Following orders. Go on in, miss assassin!");
                character_line('^', "Gotcha. Now, where's her stalker brother? Off to the lobby.");
            }
            else {
                character_line(symbol, "Rack off, mate!");
                character_line('^', "Pardon? Well, I'd like to ask my next question. How'd you lose with GUNS?");
                character_line(symbol, "It's not like I was after ya. I reckon it's the other way around!");
                character_line('^', "And for good reason. Your \"game\" is a bait, a threat to society. I need to stop this.");
                character_line(symbol, "I reckon it'd stop if ya won the game, ay. Don't try to stop me, that's iffy.");
                character_line('^', "I don't need to win a game to stop you. Case in point...");
                character_line('|', "Caught her slipping. Such unprofessionalism, hahaha! You wouldn't know anything about that, huh?");
                if (!total_fate(CONVICTED))
                    character_line('^', "Matilda dropped her guard for a second and was gobbled up by my sword.");
            }
            center_screen(WIDTH, "%s\n", "You picked up some bullets that Matilda dropped.");
            add_item(&first_item, BULLET);
            sleep(3);
            player_ptr->karma += 20;
            break;
        case '#':
            character_line(symbol, "PYOW BOOM! Grr...");
            character_line('^', "Nice try, but your net trap wasn't durable enough. My sword can slice right through it.");
            character_line(symbol, "HACK HACK PYOW! You...");
            character_line('^', "Yes?");
            character_line(symbol, "boOM haCK! BOOM BOOM BOOM!");
            character_line('|', "There aren't any greater spot-nosed monkeys around these parts, so if he's trying to rally them,\n"
                                "it won't work. Game over.");
            character_line(symbol, "Pyow pyow hack!");
            character_line('^', "Just end it already.");
            character_line('|', "Yep, sure. Come over here! There might not be any monkeys, but their hunters may be in here!");
            center_screen(WIDTH, "%s\n", "You obtain a stone charm that fell out of the tree branches above.");
            add_item(&first_item, DRAIN);
            sleep(3);
            player_ptr->karma += 10;
            break;
        case '&':
            character_line(symbol, "Ugh... Ow!");
            character_line('^', "I've got you now. No more drawings! They're scarily influential for some reason.");
            character_line('|', "Even if you don't comprehend it, you can still feel it. Maybe that's why? Still...");
            character_line(symbol, "Thank you for waiting until my drawing neared its completion. Kill me if you please.");
            character_line('^', "You misunderstand the situation you're in. Is this really the time to be drawing?");
            character_line(symbol, "I would regret not following my heart, so yes, why would I hesitate to do what I love?");
            character_line('^', "I can see your point. In that case, we'll do the same. You, follow your soul!");
            character_line('|', "I thought you'd never ask! Here goes! We'll leave your chaotic drawing there to rot. You're welcome.");
            center_screen(WIDTH, "%s\n", "You grab the fallen feathered pen by the canvas.");
            add_item(&first_item, UNCENSOR);
            sleep(3);
            player_ptr->karma += 7;
            break;
        case '!':
            character_line('^', "With a bang! Stop the gang! It's over! You lecher!");
            character_line(symbol, "How can this be?! To have been bested in 1-on-1 combat...");
            character_line('^', "Age takes a toll on you. This outcome was determined the moment we met near the cliff.");
            character_line(symbol, "How did I not notice?! .. Kidding. I don't believe your prophecies. I'm still standing.\n"
                                   "You haven't seen anything yet, just you wait. They love me for my endurance.");
            character_line('^', "You didn't notice yet? Why, you're denser than I prior thought. I can stop this battle right now.");
            character_line('|', "True. Say the word, and I'll do it for free!");
            character_line(symbol, "What is your means to reach that? Maybe, it's money! It doesn't really work.");
            character_line('^', "No, not that. This is my sword, which is basically my second badge. With this, I'm free to do what I want\n"
                                "with people like you. You people may not hear its voice.");
            character_line('|', "True as you have spoke, there are always consequences to this perk. Remember that.");
            character_line(symbol, "Are you kidding me? Really want to cross me? Swords can't speak, idiot.");
            character_line('^', "This is a game of life and death, just saying. A sword can be lethal.");
            character_line(symbol, "The others that've come up to me were smacked around like a ball. I can handle a sword.");
            character_line('^', "You still lost this fight. That stuff is irrelevant right now. Focus on what's critical here.");
            character_line(symbol, "Whatever, man. I'm scripted to win this game. I'll show you what's up! Haaaaah!");
            character_line('^', "Argh! Nobody's put up such a fight before, and he's 70 years old!");
            character_line(symbol, "What now! Who do you think you are?!");
            character_line('|', "Enough of this drama. I can't believe he doesn't know of the swords.");
            character_line(symbol, "Hmm? Wha-?!");
            character_line('^', "Holy smokes, that was close.");
            character_line('|', "You shouldn't spill any more information about what you do. You'll be penalized further.");
            character_line('^', "Fine.");
            center_screen(WIDTH, "%s\n", "You decide to keep a few containers of milk.");
            add_item(&first_item, MILK);
            sleep(3);
            player_ptr->karma += 18;
            break;
        case '/':
            character_line(symbol, "How'd you manage to beat us?! You're a wicked demon, man! Who'd wish for that rubbish?!");
            character_line('^', "No need to get upset. Your wish has come true, look! Dear Matilda is gone.");
            character_line(symbol, "... I don't buy it. She wouldn't meet her end to YOU, mate.");
            character_line('^', "Rest assured that she's in a better place now. Care to join your partner in arms?");
            character_line(symbol, "How sickening it is to be called partner in arms to that witch. She was goin' to backstab me\n"
                                   "soon enough, I'm sure of it! Hecks nah, I want nothin' to do with her.");
            character_line('|', "Too bad, MATE. According to the laws of this world, you've gotta get over here. This instance!");
            character_line(symbol, "Crikeyyyyyyyyyyyy!");
            center_screen(WIDTH, "%s\n", "You find a handwritten drawing of something you can use.");
            add_item(&first_item, ALCHEMY);
            sleep(3);
            player_ptr->karma += 20;
            break;
    }
    fate_store(enemy_ptr->rep, CONVICTED);
}

/* character_line : display dialogue from a character (prompts for input to continue) */
void character_line(char rep, char *line) {
    int keystr;
    if (skip_scene)
        return;
    printf("%c : %s\n", rep, line);
    while ((keystr = getaction(Controller)) != _EOF && keystr != Select && keystr != Down) 
        ;
    if (keystr == Select)
        return;
    else if (keystr == Down) {
        skip_scene = TRUE;
        return;
    }
    quit();
}

/* damage_dealt : display message of damage done to char2 by char1 */
void damage_dealt(struct Character *char1, struct Character *char2, int damage) {
    char message[50];
    if (char1->buff == RUSHING || char1->buff == ENLIGHTENED)
        damage = (damage > 5) ? damage + 5 : damage * 2;
    else if (char1->buff == DOUBLESHOT || char1->buff == TRIPLESHOT) {
        damage *= (char1->buff == DOUBLESHOT) ? 2 : 3;
        buff_char(char1, char2, NONE);
    }
    if (char2->buff == INVULNERABLE)
        sprintf(message, "%s wasn't affected by the attack!", char2->name);
    else if (char2->buff == SUMMON) {
        if (char2->rep == 'C') {
            sprintf(message, "The gang members took %d damage.", damage);
            char2->ally_health -= damage;
        }
        else if (char2->rep == '&') {
            sprintf(message, "The white knight took %d damage.", damage);
            char2->ally_health -= damage;
        }
        else if (char2->rep == '^') {
            if (char1->buff == PIERCE) {
                sprintf(message, "The golem's armor was penetrated and destroyed.");
                buff_char(char2, char1, NONE);
            }
            else {
                sprintf(message, "The golem took %d damage.", damage);
                char2->ally_health -= damage;
            }
        }
    }
    else if (char2->buff == GUARDED || char2->buff == ENLIGHTENED) {
        if (char2->rep == '^' || char2->rep == 'V') {    /* 0 karma : full damage; damage / 3 karma : 1 / 3 damage */
            damage = (damage - minimum(damage, 3 * char2->karma)) * (2 / 3) + damage / 3;   // damage = damage not blocked (100%)+ damage blocked (33%)
            char2->karma -= minimum(damage, char2->karma);
        } 
        else 
            damage /= 2;
        if (char2->debuff == DOUBLEOPEN) {
            char2->debuff = NONE;   // Consume the debuff
            damage *= 2;
        }
        else if (char2->debuff == HUNGRY)
            damage += 2;
        if (char1->debuff == SICK)
            damage = maximum(damage - 3, 0);
        else if (char1->debuff == HUNGRY)
            damage = maximum(damage - 2, 0);
        if (char1->buff == FULL)
            damage += 2;
        if (char2->buff == FULL)
            damage = maximum(damage - 2, 0);
        sprintf(message, "%s took %d damage.", char2->name, damage);
        char2->health -= damage;
        if (char2->rep == '^' || char2->rep == 'V')
            buff_char(char2, char1, NONE);
    }
    else {
        if (char2->debuff == DOUBLEOPEN) {
            char2->debuff = NONE;   // Consume the debuff
            damage *= 2;
        }
        else if (char2->debuff == HUNGRY)
            damage += 2;
        if (char1->debuff == SICK)
            damage = maximum(damage - 3, 0);
        else if (char1->debuff == HUNGRY)
            damage = maximum(damage - 2, 0);
        if (char1->buff == FULL)
            damage += 2;
        if (char2->buff == FULL)
            damage = maximum(damage - 2, 0);
        sprintf(message, "%s took %d damage.", char2->name, damage);
        char2->health -= damage;
    }
    if (char1->buff == HEROIC) {
        char1->karma += minimum(char2->karma, 3);
        char2->karma = maximum(char2->karma - 3, 0);
    }
    if (char1->debuff == SPIKES)
        char1->health -= 3;
    if (char2->debuff == ASLEEP && char2->buff != SUMMON)
        debuff_char(char1, char2, NONE);
    center_screen(WIDTH, "%s\n", message);
    sleep(1);
}
/* health_healed : display message of health healed to char1 by char1 */
void health_healed(struct Character *char1, struct Character *char2, int heal) {
    char message[30];
    char1->health += (heal = minimum(char1->max_health - char1->health, heal));
    sprintf(message, "%s healed %d health.", char1->name, heal);
    center_screen(WIDTH, "%s\n", message);
    sleep(1);
}

/* karma_decay : decrement the player's karma and returns the new karma value */
int karma_decay(struct Character *player_ptr) {
    player_ptr->karma = maximum(player_ptr->karma - 1, 0);
    return player_ptr->karma;
}

/* trigger_recompense : displays cutscene with JUDGE */
void trigger_recompense(void) {
    clrscr();
    char *zero_karma =
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMM_____________MMMMMMMMMMM0MMMMMMMMMMMMMMMMMMM\n"
    "MMMWWMMMMMMMMMMMMMMMMMMMMMM/    `        \\MMMMMMMMMMMMMMMMMMMMMMMWMMMMMM\n"
    "MMMMMMMMMMMMMMMMMMMMMMMMMM(  `        __` \\MMMMMMMMMMMMMMMM___MMMMMMMMMM\n"
    "MMMMMMMMMMMMMMMMMMWWMMMMMM\\   _.     /  \\  )MMMMMMMMMMMMMMMM|MMMMMMMMMMM\n"
    "MMMMMMMMMMMMMMWMMMMMMMMMMMM| (  \\   ( () ) |MMMMMMMMMMMM.---|---.MMMMMMM\n"
    "MMMMMMMMMMM MMMMMMMMMMMMMMMM\\ \\_/    \\  / |MMMMMMMMMMMMM|MMM|MM_|_MMMMMM\n"
    "MMM   MMMM   MMMMMMMMMMMMMMMa| !  /|| \\/  |MMMMMMMMMMMMM|MMM|MM\\_/MMMMMM\n"
    "MMMM\\ \\MM| |MMM * MMMMMMMMMMM(_   ||l  _  /MMMMMMMMMMMM_|_MM|MMMMMMMMMMM\n"
    "MMMMM\\ \\M| |M/ /MMMMMMMMMMMMMMM\\  `\\ /WW||MMMMMMMMMMMMM\\_/MM-MMMMMMMMMMM\n"
    "MMMM/  `       \\MMMMMMMMMMMMMMMM|    \\WW|/MMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\n"
    "(___         `  |MMMMMMMMMMMMMMMM|+H+H+ /MMMMMMMMMMMMMMMWMMMMMMMMMMMMMMM\n"
    "MMMMWWWWWWWWW\\__MMMMMmMMMMMMMMMMMM\\____/MMMMMMMMMMMMMMMMMMMMMMMMMMM100MM\n"
    "MMMMMMMMwwwwwMMMMMMMMMMWMMMMMMMMMMMMMMMMMMMMMMMMMMMWMMMMMMMMMMMMMWMMMMMM\n"
    "                 \"YOUR TIME HAS COME FOR DUE RECOMPENSE\"";
    puts(zero_karma);
    sleep(8);
    set_state_vals(Controller, FALSE, FALSE, FALSE, TRUE, FALSE);
}

/* ui : prints out the Player's attributes */
int ui(struct Character *char_ptr) {
    char stats[50];
    sprintf(stats, "Health = %d | Attack = %u | Karma = %d", char_ptr->health, char_ptr->attack, char_ptr->karma);
    center_screen(WIDTH, "%s\n", stats);
    return char_ptr->health;
}

/* status : prints out the details of char1's buffs/debuffs */
char *buff_status(struct Character *char1, struct Character *char2) {
    char *message;
    if ((message = malloc(100 * sizeof(*message))) == NULL)
        fprintf(stderr, "FATAL: Not enough memory for buff message.");
    switch (char1->buff) {
        case NONE:
            strcpy(message, "");
            break;
        case RUSHING:
            sprintf(message, "%c's attacks deal x2 dmg up to a maximum of +5 bonus dmg.", char1->rep);
            break;
        case GUARDED:
            if (char1->rep == '^' || char1->rep == 'V')
                sprintf(message, "%c receives 1/3 dmg if there's enough karma.", char1->rep);
            else
                sprintf(message, "%c receives 1/2 dmg.", char1->rep);
            break;
        case ENLIGHTENED:
            sprintf(message, "%c has x2 atk and receives 1/2 dmg.", char1->rep);
            break;
        case INVULNERABLE:
            sprintf(message, "%c only receives dmg from flat dmg.", char1->rep);
            break;
        case HEROIC:
            sprintf(message, "%c's attacks steal 3 karma.", char1->rep);
            break;
        case FULL:
            sprintf(message, "%c has +2 atk and receives -2 dmg (post-multiplier).", char1->rep);
            break;
        case REGEN:
            sprintf(message, "%c heals 2 hp at the start of turns.", char1->rep);
            char1->health += minimum(char1->max_health - char1->health, 2);
            break;
        case SUMMON:
            if (char1->ally_health > 0) {
                if (char1->rep == 'C')
                    sprintf(message, "%c has gang members that tank and deal dmg (%d hp / %d flat dmg).", char1->rep, char1->ally_health, char1->ally_attack);
                else if (char1->rep == '&')
                    sprintf(message, "%c has help from a random competitor (%d hp / %d flat dmg).", char1->rep, char1->ally_health, char1->ally_attack);
                else if (char1->rep == '^')
                    sprintf(message, "%c has a golem that tanks and deals dmg (%d hp / %d flat dmg).", char1->rep, char1->ally_health, char1->ally_attack);
            }
            else {
                char1->buff = NONE;
                strcpy(message, "");
            }
            break;
        case DOUBLESHOT:
            sprintf(message, "%c's next attack deals x2 dmg.", char1->rep);
            break;
        case TRIPLESHOT:
            sprintf(message, "%c's next attack deals x3 dmg.", char1->rep);
            break;
        case CONFIDENT:
            sprintf(message, "%c gains 2 max health for all inflicted debuffs.", char1->rep);
            break;
        case RIVALED:
            sprintf(message, "%c gains 1 attack power for all inflicted buffs.", char1->rep);
            break;
        case PIERCE:
            sprintf(message, "%c's attacks instantly defeats allies.", char1->rep);
            break;
    }
    return message;
}

char *debuff_status(struct Character *char1, struct Character *char2) {
    static int turns_held;
    char *message;
    if (turns_held == 1 && char1->debuff == ALMOST_SICK) {
        debuff_char(char2, char1, SICK);
        turns_held = 0;
    }
    else if (turns_held == 1 && char1->debuff == DOZING) {
        debuff_char(char2, char1, ASLEEP);
        turns_held = 0;
    }
    if ((message = malloc(100 * sizeof(*message))) == NULL)
        fprintf(stderr, "FATAL: Not enough memory for debuff message.");
    switch (char1->debuff) {
        case NONE:
            strcpy(message, "");
            break;
        case ACIDIFIED:
            sprintf(message, "%c takes dmg equal to %c's atk at the start of turns.", char1->rep, char2->rep);
            char1->health -= char2->attack;
            break;
        case RESTRAINED:
            sprintf(message, "%c can't act yet.", char1->rep);
            break;
        case ITEMLESS:
            sprintf(message, "%c has been robbed of all their items, can't use them.", char1->rep);
            break;
        case BURNING:
            sprintf(message, "%c takes 2 flat dmg at the start of turns.", char1->rep);
            char1->health -= 2;
            break;
        case ALMOST_SICK:
            sprintf(message, "%c will get sick next turn.", char1->rep);
            ++turns_held;
            break;
        case SICK:
            sprintf(message, "%c has -3 atk (post-multiplier).", char1->rep);
            break;
        case HUNGRY:
            sprintf(message, "%c has -2 atk and receives +2 dmg (post-multiplier).", char1->rep);
            break;
        case MELTING:
            sprintf(message, "%c takes 4 flat dmg at the start of turns.", char1->rep);
            char1->health -= 4;
            break;
        case UNLUCKY:
            sprintf(message, "%c's next roll will always be harmful to them.", char1->rep);
            break;
        case DOUBLEOPEN:
            sprintf(message, "%c takes x2 dmg from next direct attack.", char1->rep);
            break;
        case SPIKES:
            sprintf(message, "%c takes 3 flat dmg if attacking.", char1->rep);
            break;
        case DOZING:
            sprintf(message, "%c will fall asleep on the next turn.", char1->rep);
            ++turns_held;
            break;
        case ASLEEP:
            sprintf(message, "%c can't act until directly attacked.", char1->rep);
            break;
    }
    return message;
}

/* debuff_char : process debuff from char1 to char2 */
void debuff_char(struct Character *char1, struct Character *char2, int debuff_no) {
    if (char2->buff != INVULNERABLE) {
        char2->debuff = debuff_no;
        if (char1->buff == CONFIDENT && debuff_no != NONE) {
            char1->max_health += 2;
            char1->health += 2;
        }
    }
}

/* buff_char : process buff from char1 */
void buff_char(struct Character *char1, struct Character *char2, int buff_no) {
    if (char1->buff == RIVALED && buff_no != NONE)
        ++char1->attack;
    char1->buff = buff_no;
}
/* set_prop_vals : assign new values to the struct tile_prop associated with the tile_ptr */
void set_prop_vals(struct tile *tile_ptr, int set_traversable, int set_entry, int set_occupied) {
    tile_ptr->prop.is_traversable = set_traversable, tile_ptr->prop.is_entry_map = set_entry, tile_ptr->prop.is_occupied = set_occupied;
}

/* identify_rep : when a tile is occupied, determine what is occupying it */
struct Character *identify_rep(struct tile *tile_ptr, struct Character *player_ptr, struct Character *enemy_ptr[MAX_ENEMIES]) {
    int i;
    if (tile_ptr == player_ptr->occupied)   /* linear search for the Character */
        return player_ptr;
    else {
        for (i = 0; enemy_ptr[i]; ++i)
            if (tile_ptr == enemy_ptr[i]->occupied)
                return enemy_ptr[i];
    }
    return NULL;
}

/* initialize_player : creates a new Character that represents the player; used throughout */
struct Character *initialize_player(void) {
    struct Character *player_ptr = malloc(sizeof(*player_ptr));
    player_ptr->health = INIT_HEALTH, player_ptr->max_health = INIT_HEALTH;
    player_ptr->attack = INIT_ATTACK, player_ptr->attack = INIT_ATTACK, player_ptr->karma = INIT_KARMA;
    player_ptr->name = str_dup("You");
    player_ptr->rep = '^';
    player_ptr->buff = NONE;
    player_ptr->debuff = NONE;
    player_ptr->occupied = NULL; 
    return player_ptr;
}