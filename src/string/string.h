#ifndef STRING_H
#define STRING_H

#include <stdint.h>

void* memset(void* dest, int value, uint32_t size);
void* memcpy(void* dest, const void* src, uint32_t size);
int strlen(const char* str);
int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, uint32_t n);
char* strncpy(char* dest, const char* src, uint32_t n);
const char* get_last_word(const char* input);

#endif
