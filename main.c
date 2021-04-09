#include <stdio.h>
#include <time.h>
#include "title.h"

int main(int argc, char *argv[]) {
    // Try to initialize SDL_mixer for audio-related functions 
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Audio output not functional.\n");
		return -1;
    }
    else if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) {
        fprintf(stderr, "Audio mixer not functional.\n");
		return -1; 
    }
    srand(time(NULL));  // This sets the seed to a pseudo-random seed determined by the current time
    fate_initialize();  // This initializes the fate structure before the first Character is encountered
    Controller = initialize_controller(Controller); // This initializes the controller structure before input is accepted
    printf("This is a work of fiction. Behaviors/actions in this game may be offensive/controversial. Thank you for understanding.");
    sleep(3 * WAIT);   // This pauses the program for 3 seconds before advancing to the title screen
    title_screen(); // This line starts the game up by popping open the title screen
    SDL_Quit();
    return EXIT_SUCCESS;
}