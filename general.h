/* general.h
Functions that provide use that can be attributed to uses of many headers/files. */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef GENERAL_H
#define GENERAL_H

#define WIDTH 215           // Width of the terminal.

void center_screen(int decided_output_width, char *format, ...);
int num_digits(int number_to_get_digits_of);
int first_one_bit(unsigned int bits, unsigned int max_bits);
char *str_dup(char *pointer_to_string_to_duplicate);
int minimum(int a, int b);
int maximum(int a, int b);
void flush_input(void);

/* center_screen : returns the right justified minimum field width to center the string w/ output_length to the console */
void center_screen(int screen_width, char *fmt, ...) {
    va_list arguments;
    va_start(arguments, fmt);
    union {
        char *buffer;
        int buf_int;
    } stored_arg;
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
            }
        }
        else
            fputc(*arg_p, stdout);
    }
    va_end(arguments);
}

/* str_dup : duplicates a string and dynamically allocates memory for it (from K&R) */
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

/* num_digits : returns # digits in num */
int num_digits(int num) {
    int digits = 0;
    while (num) {
        num /= 10;
        ++digits;
    }
    return digits;
}

// Returns the "position" of the first one bit in the unsigned integer provided as an argument (position = displacement from the left-most bit given that there are a maximum of max_bits in the unsigned integer)
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

int minimum(int a, int b) {
    return (a >= b) ? b : a;
}

int maximum(int a, int b) {
    return (a >= b) ? a : b;
}

void flush_input(void) {
    while (getchar() != '\n') {
        ;
    }
}
#endif