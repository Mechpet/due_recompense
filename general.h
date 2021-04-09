/* general.h
Functions that provide use that can be attributed to uses of many headers/files. 
*/

#define clrscr() printf("\e[1;1H\e[2J") // Macro to clear the terminal of text
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define SDL_MAIN_HANDLED 1  // Guards against a possible error from including SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#ifndef GENERAL_H    
#define GENERAL_H   // Guards against redefinition

#define FALSE 0 
#define TRUE 1

int WIDTH = 210;    // Number used to base a centering of a string on
int WAIT = 1;   // Number used to base how long a message persists 

void center_screen(int decided_output_width, char *format, ...);
int num_digits(int number_to_get_digits_of);
int first_one_bit(unsigned int bits, unsigned int max_bits);
char *str_dup(char *pointer_to_string_to_duplicate);
int minimum(int a, int b);
int maximum(int a, int b);
void flush_input(void);
void play_sound_fx(char *file_name);

/* play_sound_fx :
 * Arguments: file_name - string of the file path (extension .wav) to be played by the function
 * Implementation: Attempt to load the file based on the argument; if able to play, do so
 * Purpose: Acts as a convenient function; plays a wav file with one call instead of copy-pasting this function's contents */
void play_sound_fx(char *file_name) {
    Mix_Chunk *fx_wav = NULL;
    if ((fx_wav = Mix_LoadWAV(file_name)) == NULL) {
        fprintf(stderr, "Could not load %s wav file.\n", file_name);
    }

    if (Mix_PlayChannel(-1, fx_wav, 0) == -1) {
        fprintf(stderr, "Could not play %s wav file.\n", file_name);
    }

    return;
}

/* center_screen : 
 * Arguments: screen_width - To be the width of the terminal window (tries to center to this argument)
              fmt - The output format desired (can be "%s" or "%s\n" or whatnot)
              ... - Additional arguments to format fmt (what's to be formatted per '%' passed)
 * Implementation: Iteration - parses the contents of fmt and prints the contents with a justification calculated given WIDTH
 * Purpose: returns the right justified minimum field width to approximately center the string to the console */
void center_screen(int screen_width, char *fmt, ...) {
    va_list arguments;
    va_start(arguments, fmt);
    union {
        char *buffer;
        int buf_int;
    } stored_arg;   // Will contain the contents of a component of fmt (expects only strings and integers)
    /* Iterate through fmt */
    for (char *arg_p = fmt; *arg_p; ++arg_p) {
        if (*arg_p == '%') {
            switch (*++arg_p) {
                case 's':
                    stored_arg.buffer = va_arg(arguments, char *);
                    printf("%*s", 
                        screen_width / 2 + strlen(stored_arg.buffer) / 2,
                        stored_arg.buffer);
                    break;
                case 'd':
                    stored_arg.buf_int = va_arg(arguments, int);
                    printf("%*d",
                        screen_width / 2 + num_digits(stored_arg.buf_int) / 2,
                        stored_arg.buf_int);
                    break;
                case 'c':
                    stored_arg.buf_int = va_arg(arguments, int);
                    printf("%*c",
                        screen_width / 2,
                        stored_arg.buf_int);
                    break;
            }
        }
        else {
            fputc(*arg_p, stdout);
        }
    }
    va_end(arguments);
}

/* str_dup :
 * Arguments: string is the string to be duplicated
 * Implementation: Dynamic allocation
 * Purpose: Duplicates a string and dynamically allocates memory for it (from K&R) */
char *str_dup(char *string) {
    char *p;
    p = malloc(strlen(string) + 1);
    if (p != NULL) {
        strcpy(p, string);
    }
    else {
        fprintf(stderr, "FATAL: No more memory to allocate string.\n");
    }
    return p;
}

/* num_digits : 
 * Arguments: num is the integer number to analyze
 * Implementation: Determine the number of places (base 10) num has by dividing by 10 
 * Purpose: Returns the number of digits in num */
int num_digits(int num) {
    int digits = 0;
    while (num) {
        num /= 10;
        ++digits;
    }
    return digits;
}

/* first_one_bit :
 * Arguments: bits is the unsigned integer to analyze
              max_bits is the number of bits that bits has (includes leading zeroes)
 * Implementation: Right-shift a bit mask until a bit in bits is 1
 * Purpose: Returns the position of the first one bit in the unsigned integer provided as an argument (position = displacement from the left-most bit given that there are a maximum of max_bits in the unsigned integer) */
int first_one_bit(unsigned int bits, unsigned int max_bits) {
    int return_val = 0;
    unsigned int mask = pow(2, max_bits - 1);
    if (!bits) {
        return -1;
    }
    else {
        while (!((bits & (mask >> return_val)))) {
            return_val++;
        }
        return return_val;
    }
}

/* minimum :
 * Arguments: a is an integer to be compared to b
              b is an integer to be compared to a
 * Implementation: Ternary conditional operator
 * Purpose: Return the lesser integer of the provided integers */
int minimum(int a, int b) {
    return (a >= b) ? b : a;
}

/* maximum :
 * Arguments: a is an integer to be compared to b
              b is an integer to be compared to a
 * Implementation: Ternary conditional operator
 * Purpose: Return the greater integer of the provided integers */
int maximum(int a, int b) {
    return (a >= b) ? a : b;
}

/* flush_input:
 * Arguments:
 * Implementation: Get characters from the input stream until a newline character is reached
 * Purpose: Empty the input stream that is not useful (until a newline character is reached) */
void flush_input(void) {
    while (getchar() != '\n') {
        ;
    }
}
#endif