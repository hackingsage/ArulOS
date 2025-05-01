#include "screen.h"

uint16_t* vga_buffer = (uint16_t*)0xB8000;
int terminal_row = 0;
int terminal_col = 0;
uint8_t terminal_color = 0x0F; // Light grey on black

uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}

void clear_screen() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const int index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_col = 0;
}

void scroll_screen(){
    // If the cursor is beyond the last line
    if (terminal_row >= VGA_HEIGHT){
        // Move all lines one row up
        for(uint16_t y = 1; y < VGA_HEIGHT; y++){
            for(uint16_t x = 0; x < VGA_WIDTH; x++){
                vga_buffer[(y-1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
            }
        }
        // Clear the last line
        for(uint16_t x = 0; x < VGA_WIDTH; x++){
            vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ',terminal_color);
        }
        // Stay at the last line
        terminal_row = VGA_HEIGHT - 1;
    }
}

void putchar(char c) {
    if (c == '\n') {
        terminal_row++;
        terminal_col = 0;
    } else {
        const int index = terminal_row * VGA_WIDTH + terminal_col;
        vga_buffer[index] = vga_entry(c, terminal_color);
        terminal_col++;
        if (terminal_col >= VGA_WIDTH) {
            terminal_col = 0;
            terminal_row++;
        }
    }

    scroll_screen(); // Always check after a char or newline
}

void print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
    }
}

void print_int(int n) {
    char buf[16];
    int i = 0;
    if (n == 0) {
        putchar('0');
        return;
    }
    if (n < 0) {
        putchar('-');
        n = -n;
    }
    while (n > 0) {
        buf[i++] = (n % 10) + '0';
        n /= 10;
    }
    while (i--) {
        putchar(buf[i]);
    }
}

void set_text_color(uint8_t fg, uint8_t bg) {
    terminal_color = (bg << 4) | (fg & 0x0F);
}
