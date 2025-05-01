#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>

// Read a byte from the specified I/O port
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    /*
    inb(port):
        Reads a byte from the specified port
        the result is stored in 'result' (mapped to the eax register, denoted by '=a')
    */
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Write a byte to the specified I/O port
static inline void outb(uint16_t port, uint8_t data) {
    /*
    outb(port, data):
        Writes a byte (data) to the specified port
        uses the outb instruction, with data in eax ("a") and port in dx ("Nd")
    */
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t data) {
    /*
    outw(port,data):
        writes a 16-bit word to a specified I/O port.
        uses the outw instruction, with data in eax ("a") and port in dx ("Nd")
    */
    __asm__ volatile ("outw %0, %1" : : "a"(data), "Nd"(port));
}


#endif
