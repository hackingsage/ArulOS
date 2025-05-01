#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

uint8_t keyboard_read_scancode();
char scancode_to_char(uint8_t sc);

extern int shift_pressed;

#endif
