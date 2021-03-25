/* inventory.h
Functions that implement the inventory system. */

#define MAX_LENGTH 400  // Maximum length of a description for an item in inventory
#define MAX_SIZE 200    // Maximum length of an ASCII art of an item in inventory
#include <string.h> 
#include <stdlib.h>
#include "controller.h"

// Numbers that correspond to each obtainable item in the game (acts as identifiers)
enum item_effects {
    DAMAGE, 
    RUSH, 
    HEAL, 
    GUARD, 
    RECOVER, 
    TRICK,
    FEAST, 
    FAMINE,
    FIRE, 
    MOON, 
    TEMPER, 
    JUDGEMENT, 
    STRENGTH, 
    GOLEM, 
    RANG,
    BULLET,
    ALCHEMY,
    DRAIN,
    UNCENSOR,
    MILK,
    TRAP,
    DUE_RECOMPENSE,
};

// Numbers that correspond to each obtainable item's karma cost (property of an inventory item)
enum effect_costs {
    FREE = 0,
    DAMAGE_COST = 2,
    DRAIN_COST = 2, 
    STRENGTH_COST = 3, 
    TEMPER_COST = 4,
    ALCHEMY_COST = 5,
    MILK_COST = 5,
    FIRE_COST = 6, 
    FEAST_COST = 7,
    FAMINE_COST = 7,
    UNCENSOR_COST = 8,
    RUSH_COST = 9,
    MOON_COST = 9, 
    HEAL_COST = 10, 
    TRICK_COST = 10,
    GOLEM_COST = 10,
};

// Numbers that describe an obtainable item's durability (how many times can the player possibly use them - is there a limit?)
enum effect_uses {
    INFINITE = -1,
    DUE_RECOMPENSE_USES = 1,
    RANG_USES = 3,
    MILK_USES = 3,
    BULLET_USES = 6,
    RECOVER_USES = 10,
};

// Fully describes an item in the inventory 
struct item {
    char description[MAX_LENGTH];   // Details lore/functionality of an inventory item
    char image[MAX_SIZE];   // ASCII art of the inventory item to be displayed
    int karma_cost; // The affiliated karma cost of using the inventory item
    int uses;   // The affiliated number of uses of the inventory item
    unsigned int effect;    // Identifies what the item is based on item_effects
    struct item *next_item; // Pointer to the next item in the inventory (self-referential data structure)
} *first_item, *first_enemy_item;  // Always use the first_item as a base when using the inventory (scrolls through other items with next_item)

/* DAMAGING = Items that primarily inflict damage */
void chainsaw_allocate(struct item **item_ptr);
void arrow_allocate(struct item **item_ptr);
void judgedshard_allocate(struct item **item_ptr);
void pen_allocate(struct item **item_ptr);

/* OFFENSIVE = Items that boost damaging capabilities */
void boots_allocate(struct item **item_ptr);
void temperedshard_allocate(struct item **item_ptr);
void bullet_allocate(struct item **item_ptr);

/* SUPPORT = Items that primarily improve survivability */
void medicine_allocate(struct item **item_ptr);
void shield_allocate(struct item **item_ptr);
void drink_allocate(struct item **item_ptr);
void food_allocate(struct item **item_ptr);
void toxin_allocate(struct item **item_ptr);
void moonshard_allocate(struct item **item_ptr);
void doll_allocate(struct item **item_ptr);
void eye_allocate(struct item **item_ptr);

/* TACTICAL = Unique items that fit none or multiple of the other categories */
void bag_allocate(struct item **item_ptr);
void strengthshard_allocate(struct item **item_ptr);
void rang_allocate(struct item **item_ptr);
void circle_allocate(struct item **item_ptr);
void sword_allocate(struct item **item_ptr);
void milk_allocate(struct item **item_ptr);

void add_item(struct item **first_item_ptr, int item_id);
void allocate_item(struct item **empty_item_slot, int item_id);
struct item *has_item(struct item **first_item_ptr, int item_id);
void free_item(struct item **item_ptr);

/* (item)_allocate : 
 * Arguments: item_ptr is a pointer to pointer to item structure (pointer to pointer does not change the address the pointer to item structure points to)
 * Implementation: Dynamically allocate memory for the item structure, copy the description and art of the item, store the affiliated values, point the next_item to nothing
 * Purpose: Store the item with identifier (item_effects) */
void chainsaw_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate DAMAGE item.\n");
    }
    strcpy((*item_ptr)->description, "[2 Karma]\nA mini-chainsaw for dissecting the sins out of your enemies. Inflicts twice your attack power (non-flat) to the enemy.\nYou nicknamed it \"Shak Kiya Vijaya\".");
    strcpy((*item_ptr)->image, 
    " _______________\n"
    "|   ` ^-^-^-^  `|\n"
    "|  `</ ___  />  |\n"
    "|` </ / */ />  `|\n"
    "| </ /* / /> `  |\n"
    "|</ / */ /> `   |\n"
    "|/_/_ / /> `    |\n"
    "| _ _'_/>    `  |\n"
    "|o     |        |\n"
    "|____[/   `    `|\n"
    "|_______________|");
    (*item_ptr)->effect = DAMAGE, (*item_ptr)->karma_cost = DAMAGE_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

/* boots_allocate : allocates memory for an offensive-buff item in inventory 
*/
void boots_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate RUSH item.\n");
    }
    strcpy((*item_ptr)->description, "[9 Karma]\nA pair of nice-looking boots that have been well-maintained. Grants a normal buff that doubles (non-flat) attacks' power up to 5 additional damage.\nYou nicknamed it \"Nine-League Boots\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "| *  _______  * |\n"
    "|   /o o o /    |\n"
    "|*  |____./     |\n"
    "|  *|   X|  *   |\n"
    "|   |   X|    * |\n"
    "|   )    X\\     |\n"
    "| * | __   \\__* |\n"
    "|___|_l \\_____)_|\n"
    "|               |\n"
    "|_______________|");
    (*item_ptr)->effect = RUSH, (*item_ptr)->karma_cost = RUSH_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

/* medicine_allocate : allocates memory for a healing item in inventory
   Intended effect   : Heals health */
void medicine_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate HEAL item.\n");
    }
    strcpy((*item_ptr)->description, "[10 Karma]\nA shiny chalice filled to the brim with bull's blood. Heals half of your missing health (truncated).\nYou nicknamed it \"Mithras Mithridate\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "| ''          ' |\n"
    "|'   MMMMMMM   '|\n"
    "|  MMMMMMMMMM}' |\n"
    "|{MC/ + _ x \\)MM|\n"
    "|WM(~>  Y  <~)MW|\n"
    "| MM\\ \\/'\\/ /MM |\n"
    "| WMWW\\   /WWWW |\n"
    "|  W   |o|    W |\n"
    "|  '  /___\\ '  '|\n"
    "|_______________|");
    (*item_ptr)->effect = HEAL, (*item_ptr)->karma_cost = HEAL_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

/* shield_allocate : allocates memory for a defensive-buff item in inventory
   Intended effect : Take 50 - 100 % damage depending on karma */
void shield_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate GUARD item.\n");
    }
    strcpy((*item_ptr)->description, "[(Damage Taken) Karma]\nA sacred shield that's a bit battered. Reduces next (non-flat) attack to 33%-100% damage based on karma as a normal buff.\nI.e. Damage Taken = Damage - Damage fully blocked + Remaining Damage not blocked. EX: Karma = 5, incoming damage = 18, damage taken = (18 - 5 * 3) + 18 / 3 = 8.\nYou nicknamed it \"Subdititius Ancile\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|    o==O==o    |\n"
    "|   /  0 0  \\   |\n"
    "|  o 0 ___ 0 o  |\n"
    "|  :    . `  :  |\n"
    "|---)'(/*\\) (---|\n"
    "|  :   ) (   :  |\n"
    "|  o 0`    0 o  |\n"
    "|   \\  0 0  /   |\n"
    "|    o==O==o    |\n"
    "|_______________|");
    (*item_ptr)->effect = GUARD, (*item_ptr)->karma_cost = FREE, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void drink_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate RECOVER item.\n");
    }
    strcpy((*item_ptr)->description, "A strong alcoholic beverage that contains a hundred herbs. Dispels normal debuffs from self and heals 3 health.\nThe label reads \"Centerba\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|      __ _ ~   |\n"
    "|  ~   |=|_)    |\n"
    "|~    _|~|_  ~  |\n"
    "|    / -O- \\    |\n"
    "|~  |_______|  ~|\n"
    "|   |.:.:.:.|   |\n"
    "|~  | (70%) |  ~|\n"
    "|   |:.:.:.:|   |\n"
    "|___|_______|___|\n"
    "|_______________|");
    (*item_ptr)->effect = RECOVER, (*item_ptr)->karma_cost = FREE, (*item_ptr)->uses = RECOVER_USES, (*item_ptr)->next_item = NULL;
}

void bag_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate RECOVER item.\n");
    }
    strcpy((*item_ptr)->description, "[10 Karma]\nAn ancient trickster's handbag.\nAttempt to steal the enemy's normal buff. If there is a normal buff, grants invulnerability on a single enemy phase as a normal buff and then after the invulnerability wears off, the normal buff is stolen.\nYou nicknamed it \"Dokkaebi's Purse\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|//  _______  \\\\|\n"
    "|/  //`````\\\\  \\|\n"
    "|  _O_______O_  |\n"
    "| |   _.-._   | |\n"
    "| |  / \\_/ \\  | |\n"
    "| |  >-(#)-<  | |\n"
    "| l  \\_/`\\_/  i |\n"
    "| |    '-'    | |\n"
    "| L___________J |\n"
    "|_______________|");
    (*item_ptr)->effect = TRICK, (*item_ptr)->karma_cost = TRICK_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void arrow_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate FIRE item.\n");
    }
    strcpy((*item_ptr)->description, "[6 Karma]\nAn arrowhead hot to the touch. Inflicts a normal debuff that does 2 (flat) dmg at the start of turns.\nYou nicknamed it \"Agneyastra's Flint\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "| .^.   )\\ / ( /|\n"
    "|\\ ) ) (  V   ) |\n"
    "| (  (  )  ) ( /|\n"
    "|\\ )  \\/  (   ) |\n"
    "| ( (   _  ) ( /|\n"
    "|  \\ ) |o|   /  |\n"
    "| \\ \\  |||  /  /|\n"
    "|\\   \\ \\ / / /  |\n"
    "|  \\  \\ V /   / |\n"
    "|_______________|");
    (*item_ptr)->effect = FIRE, (*item_ptr)->karma_cost = FIRE_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void food_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate FEAST item.\n");
    }
    strcpy((*item_ptr)->description, "[7 Karma]\nAn endless supply of food. Grants a normal buff that adds 2 atk and reduces 2 dmg to (non-flat) attacks dealt and received respectively (after multipliers).\nYou nicknamed it \"Achelous's Horn\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|               |\n"
    "|   .-\"\"\"\"\"\")   |\n"
    "| .'.------/   |\n"
    "|//'.-----(     |\n"
    "|| / .-====-.   |\n"
    "||/ //MMMMMM\\\\  |\n"
    "| \\||MMMMMMMM|| |\n"
    "|O `\\\\MMMMMM// o|\n"
    "|  o  `=__='' O |\n"
    "|_______________|");
    (*item_ptr)->effect = FEAST, (*item_ptr)->karma_cost = FEAST_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void toxin_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate FAMINE item.\n");
    }
    strcpy((*item_ptr)->description, "[7 Karma]\nAn endless supply of weak poisons. Inflicts a normal debuff that reduces 3 dmg from (non-flat) attacks (after multipliers) after 1 turn.\nYou nicknamed it \"Achelous's Spike\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|               |\n"
    "|   .-''''''/   |\n"
    "| .'.------/    |\n"
    "|//'.-----(     |\n"
    "|| / .-====-.   |\n"
    "||/ //A~AA~A\\\\  |\n"
    "| \\||A~AAAA~A|| |\n"
    "|x `\\\\A~AA~A// o|\n"
    "|  o  `=__='' x |\n"
    "|_______________|");
    (*item_ptr)->effect = FAMINE, (*item_ptr)->karma_cost = FAMINE_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void moonshard_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate MOON item.\n");
    }
    strcpy((*item_ptr)->description, "[9 Karma]\nAn asteroid shard. Grants a normal buff that heals 2 health at the start of turns.\nYou nicknamed it \"3 Fortuna\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "| .       +    *|\n"
    "|  * __      .  |\n"
    "|.  / .\\  .     |\n"
    "|  / .  \\   +  .|\n"
    "|./ .   /\\_     |\n"
    "| \\____//` \\ *  |\n"
    "|  `-._\\ o`|.   |\n"
    "| *   . \\_/   + |\n"
    "|   +         . |\n"
    "|_______________|");
    (*item_ptr)->effect = MOON, (*item_ptr)->karma_cost = MOON_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void judgedshard_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate THUNDER item.\n");
    }
    strcpy((*item_ptr)->description, "[INPUT hp]\nAn asteroid shard. Does (flat) damage to the enemy decided by player INPUT.\nYou nicknamed it \"20 Fortuna\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "| _______    *  |\n"
    "| \\ \\: : `-.__ *|\n"
    "| \\ \\: : : : |  |\n"
    "| \\ \\: _ : : |  |\n"
    "|  \\ \\ |\\_ : |_ |\n"
    "|*  \\_\\|||   o| |\n"
    "|       ||  ` | |\n"
    "|  +  .  `--._| |\n"
    "|.      *    .  |\n"
    "|_______________|");
    (*item_ptr)->effect = JUDGEMENT, (*item_ptr)->karma_cost = FREE, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void temperedshard_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate TEMPER item.\n");
    }
    strcpy((*item_ptr)->description, "[3 Karma]\nAn asteroid shard. Increases everybody's attack power by 1 only for the duration of the battle.\nYou nicknamed it \"11 Fortuna\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|      *      * |\n"
    "|  +       .    |\n"
    "|    ____ _     |\n"
    "|.  /: : \\\\\\ *  |\n"
    "|  /: : : \\\\\\  *|\n"
    "|  | : : : \\o\\  |\n"
    "|   `-._: :/// *|\n"
    "| .  +  \\_/_/   |\n"
    "|  +   .      . |\n"
    "|_______________|");
    (*item_ptr)->effect = TEMPER, (*item_ptr)->karma_cost = TEMPER_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void strengthshard_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate TEMPER item.\n");
    }
    strcpy((*item_ptr)->description, "[4 Karma]\nAn asteroid shard. Dispels everyone's normal buff and normal debuff.\nYou nicknamed it \"14 Fortuna\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|.  _____       |\n"
    "|  /o: :\\\\  +   |\n"
    "|  |: : :\\\\   * |\n"
    "|  | : :_-\\\\_   |\n"
    "|   \\_-'   `]|  |\n"
    "|*   \\ `   // * |\n"
    "| + _/___`//    |\n"
    "|  /______/ .   |\n"
    "|   .         + |\n"
    "|_______________|");
    (*item_ptr)->effect = STRENGTH, (*item_ptr)->karma_cost = STRENGTH_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void doll_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate GOLEM item.\n");
    }
    strcpy((*item_ptr)->description, "[10 Karma + 15 hp]\nA doll of a golem. Summons / resummons a golem with 15 hp and 3 attack (flat damage) as a normal buff.\nIt attacks after your actions and receives (non-flat) attacks in place of you. You nicknamed it \"Yosef\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|     _____     |\n"
    "|   (|  o  |)   |\n"
    "|  __|\" _ \"|__  |\n"
    "| /II/-----\\II\\ |\n"
    "| || \\ |~| / || |\n"
    "| || |--+--| || |\n"
    "|(ll)|[ _ ]|(ll)|\n"
    "|    |]| |[|    |\n"
    "|___[__|_|__]___|\n"
    "|_______________|");
    (*item_ptr)->effect = GOLEM, (*item_ptr)->karma_cost = GOLEM_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void rang_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate RANG item.\n");
    }
    strcpy((*item_ptr)->description, "Custom-made metallic batarang.\nInflict (non-flat) attack power and grants a normal buff that steals 3 karma per (non-flat) attack from the enemy.");
    char bat[199];
    sprintf(bat, "%s%c%s", 
    " _______________\n"
    "|    |\\__       |\n"
    "| *  |   \\   o  |\n"
    "|     )  (      |\n"
    "|  *  |   \\   * |\n"
    "|    ", 228, 
    "     >    |\n"
    "|     |   /     |\n"
    "|     )  (   *  |\n"
    "|    | __/      |\n"
    "|*   |/         |\n"
    "|_______________|");
    strcpy((*item_ptr)->image, bat);
    (*item_ptr)->effect = RANG, (*item_ptr)->karma_cost = FREE, (*item_ptr)->uses = RANG_USES, (*item_ptr)->next_item = NULL;
}

void bullet_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate BULLET item.\n");
    }
    strcpy((*item_ptr)->description, "Silver bullets.\nInflict (non-flat) attack power and grants a normal buff that triples next (non-flat) attack's power.");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|o / \\  _  / \\ o|\n"
    "| /___\\/ \\/___\\ |\n"
    "|[llll/___\\llll]|\n"
    "||---[lllll]---||\n"
    "|||  |-----|  |||\n"
    "|||  |    ||   ||\n"
    "||___|    ||___||\n"
    "|(___|_____|___)|\n"
    "|o   (_____)   o|\n"
    "|_______________|");
    (*item_ptr)->effect = BULLET, (*item_ptr)->karma_cost = FREE, (*item_ptr)->uses = BULLET_USES, (*item_ptr)->next_item = NULL;
}

void circle_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate ALCHEMY item.\n");
    }
    strcpy((*item_ptr)->description, "[5 karma + 5 hp]\nAn advanced transmutation circle.\nReplenishes all items' uses by 1 IF not 0 uses up to its maximum capacity.\nIts alchemic name is \"The Philosophers' Stone\".");
    char alc[198];
    sprintf(alc, "%s%c%s%c%s",
    " _______________\n"
    "|    .-\"\"\"-.    |\n"
    "|  .\"   ^   \".  |\n"
    "| /(c) / \\ (o)\\ |\n"
    "|:    /___\\    :|\n"
    "||   /| _ |\\   ||\n"
    "|:  / |(", 236,")| \\  :|\n"
    "| \\/__|_\"_|__\\/ |\n"
    "|  \".  (", 197, ")  .\"  |\n"
    "|    \"-...-\"    |\n"
    "|_______________|");
    strcpy((*item_ptr)->image, alc);
    (*item_ptr)->effect = ALCHEMY, (*item_ptr)->karma_cost = ALCHEMY_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void milk_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate MILK item.\n");
    }
    strcpy((*item_ptr)->description, "A pot of milk from the cosmic Ocean of Milk.\n[5 karma] Grants a normal buff that increments attack power per any inflicted buff on self OR\napplies normal debuff on enemy such that they take 3 (flat) damage when they (non-flat) attack.");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|      ___      |\n"
    "|~~~~_/ o \\_~~~~|\n"
    "| ~ (_\\___/_) ~ |\n"
    "|~ _.-'==='-._ ~|\n"
    "| /___________\\ |\n"
    "|(\\|||||||||||/)|\n"
    "| \\_|_|_|_|_|_/ |\n"
    "|~ \\_________/ ~|\n"
    "| ~           ~ |\n"
    "|_______________|");
    (*item_ptr)->effect = MILK, (*item_ptr)->karma_cost = MILK_COST, (*item_ptr)->uses = MILK_USES, (*item_ptr)->next_item = NULL;
}

void eye_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate DRAIN item.\n");
    }
    strcpy((*item_ptr)->description, "[2 karma]\nA stone carved to the shape of the Eye of Horus.\nInflict (non-flat) half attack power and heal your health for that amount (truncated).");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|* * * ___ * * *|\n"
    "| ._.-'___'-._. |\n"
    "| __.-\".-.\"-.._ |\n"
    "|'-.  ( o )  .\" |\n"
    "|/ _'-.'-'  /  \\|\n"
    "| / \\  |  /|\\_  |\n"
    "|( (\\) / / | ( \\|\n"
    "| \\ \\_/ /  | /  |\n"
    "|/ \\___/   |/  \\|\n"
    "|_______________|");
    (*item_ptr)->effect = DRAIN, (*item_ptr)->karma_cost = DRAIN_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void pen_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate UNCENSOR item.\n");
    }
    strcpy((*item_ptr)->description, "[8 karma]\nA fountain pen with a beautiful peacock feather.\nInflicts (flat) 5 damage and removes enemy's normal buff.\n You nicknamed it \"Hera's Pen\".");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "| \\ \\/`/_\\'\\/_/ |\n"
    "| |\\\\`((w))//  ||\n"
    "||  \\_`\\_/'// | |\n"
    "|  |  \\\\_/_/ | ||\n"
    "|| ____|=|______|\n"
    "| /  / / \\     /|\n"
    "|/  /__)|(____/ |\n"
    "|      \\o/   /  |\n"
    "|       V___/   |\n"
    "|_______________|");
    (*item_ptr)->effect = UNCENSOR, (*item_ptr)->karma_cost = UNCENSOR_COST, (*item_ptr)->uses = INFINITE, (*item_ptr)->next_item = NULL;
}

void sword_allocate(struct item **item_ptr) {
    if ((*item_ptr = malloc(sizeof(**item_ptr))) == NULL) {
        fprintf(stderr, "FATAL: No more memory to allocate DUE_RECOMPENSE item.\n");
    }
    strcpy((*item_ptr)->description, "A broken sword carried by another of your kind.\nRecovers 50 karma and fully restores health.");
    strcpy((*item_ptr)->image,
    " _______________\n"
    "|`   ` (_) `    |\n"
    "|      |/    `  |\n"
    "|  `  =====O    |\n"
    "|      |||      |\n"
    "|  o   |||   .  |\n"
    "|      \\_|      |\n"
    "|   .  ~_~~~~   |\n"
    "|     ~| |~~  . |\n"
    "|     ~~V~~ .   |\n"
    "|_______________|");
    (*item_ptr)->effect = DUE_RECOMPENSE, (*item_ptr)->karma_cost = FREE, (*item_ptr)->uses = DUE_RECOMPENSE_USES, (*item_ptr)->next_item = NULL;
}

/* add_item : 
 * Arguments: first_item_ptr is the pointer to pointer to first_item
 *            item_id is the item_effect that the new item to be allocated should have
 * Implementation: Recursion
 * Purpose: allocates memory for another item in inventory */
void add_item(struct item **first_item_ptr, int item_id) {
    if (*first_item_ptr == NULL) {   // Base case: Reached an item slot in inventory that isn't occupied
        allocate_item(first_item_ptr, item_id);
    }
    else {   // Recursive case: Current item slot in inventory is occupied; keep calling add_item
        add_item(&(*first_item_ptr)->next_item, item_id);
    }
}

/* allocate_item : 
 * Arguments: empty_item_ptr is the pointer to pointer to the item slot in inventory that isn't occupied (found with add_item)
 *            item_id is the item_effect that the new item to be allocated should have
 * Implementation: Check what case the item_id fits; call another specific function 
 * Purpose: allocates memory matching the given item_id */
void allocate_item(struct item **empty_item_ptr, int item_id) {
    switch (item_id) {
        case DAMAGE:
            chainsaw_allocate(empty_item_ptr);
            break;
        case RUSH:
            boots_allocate(empty_item_ptr);
            break;
        case HEAL:
            medicine_allocate(empty_item_ptr);
            break;
        case GUARD:
            shield_allocate(empty_item_ptr);
            break;
        case RECOVER:
            drink_allocate(empty_item_ptr);
            break;
        case TRICK:
            bag_allocate(empty_item_ptr);
            break;
        case FIRE:
            arrow_allocate(empty_item_ptr);
            break;
        case FEAST:
            food_allocate(empty_item_ptr);
            break;
        case FAMINE:
            toxin_allocate(empty_item_ptr);
            break;
        case MOON:
            moonshard_allocate(empty_item_ptr);
            break;
        case JUDGEMENT:
            judgedshard_allocate(empty_item_ptr);
            break;
        case TEMPER:
            temperedshard_allocate(empty_item_ptr);
            break;
        case STRENGTH:
            strengthshard_allocate(empty_item_ptr);
            break;
        case GOLEM:
            doll_allocate(empty_item_ptr);
            break;
        case RANG:
            rang_allocate(empty_item_ptr);
            break;
        case BULLET:
            bullet_allocate(empty_item_ptr);
            break;
        case MILK:
            milk_allocate(empty_item_ptr);
            break;
        case DRAIN:
            eye_allocate(empty_item_ptr);
            break;
        case UNCENSOR:
            pen_allocate(empty_item_ptr);
            break;
        case ALCHEMY:
            circle_allocate(empty_item_ptr);
            break;
        case DUE_RECOMPENSE:
            sword_allocate(empty_item_ptr);
            break;
    }
}

void free_item(struct item **item_ptr) {
    free(*item_ptr);

    return;
}

struct item *has_item(struct item **first_item_ptr, int item_id) {
    struct item *curr_item = *first_item_ptr;
    while (curr_item) {
        if (curr_item->effect == item_id) {
            return curr_item;
        }
        curr_item = curr_item->next_item;
    }
    return NULL;
}