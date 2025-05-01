#include "string.h"

void* memset(void* dest, int value, uint32_t size) {
    uint8_t* ptr = (uint8_t*)dest;
    for (uint32_t i = 0; i < size; i++) {
        ptr[i] = (uint8_t)value;
    }
    return dest;
}

void* memcpy(void* dest, const void* src, uint32_t size) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (uint32_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
    return dest;
}

int strlen(const char* str) {
    int len = 0;
    while (*str++) len++;
    return len;
}

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

int strncmp(const char* a, const char* b, uint32_t n) {
    while (n && *a && (*a == *b)) {
        a++;
        b++;
        n--;
    }
    if (n == 0) return 0;
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

char* strncpy(char* dest, const char* src, uint32_t n) {
    uint32_t i = 0;
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i++] = '\0';
    }
    return dest;
}

const char* get_last_word(const char* input) {
    int len = strlen(input);
    if (len == 0) return "";

    const char* p = input + len - 1;
    while (p >= input && *p != ' ') p--;
    return p + 1;
}
