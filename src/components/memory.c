#include "memory.h"

extern uint8_t _heap_start;
extern uint8_t _heap_end;

uint8_t* heap_start;
uint8_t* heap_end;
uint8_t* heap_current;

void memory_init() {
    heap_start = &_heap_start;
    heap_end = &_heap_end;
    heap_current = heap_start;
}

void* malloc(uint32_t size) {
    // Align to 4 bytes
    if (size % 4 != 0) size += 4 - (size % 4);

    if ((heap_current + size) > heap_end) return 0;

    void* alloc = heap_current;
    heap_current += size;
    return alloc;
}
