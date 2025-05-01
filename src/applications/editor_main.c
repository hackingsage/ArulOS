#include "components/keyboard.h"
#include "screen/screen.h"
#include "components/memory.h"
#include "fs/fs.h"
#include "string/string.h"

void read_editor_input(char* buf, int max_len) {
    int idx = 0;
    while (1) {
        uint8_t sc = keyboard_read_scancode();

        if (sc == 0x1C) { // ENTER
            buf[idx] = '\0';
            print("\n");
            return;
        }

        if (sc == 0x0E && idx > 0) { // BACKSPACE
            idx--;
            terminal_col = terminal_col ? terminal_col - 1 : 0;
            const uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
            vga_buffer[pos] = vga_entry(' ', terminal_color);
            continue;
        }

        char c = scancode_to_char(sc);
        if (c && idx < max_len - 1) {
            buf[idx++] = c;
            putchar(c);
        }
    }
}

void editor_main() {
    set_text_color(COLOR_WHITE,COLOR_BLACK);
    print("\nArulOS Text Editor\n\n");
    print("\nPress ESC to exit, CTRL+S to save\n\n");
    print("Enter filename: ");

    char filename[32];
    read_editor_input(filename, 32);

    const char* file_data = fs_read(filename);
    char* buffer = (char*)malloc(4096);
    uint32_t index = 0;

    if (file_data) {
        set_text_color(COLOR_LIGHT_GREEN,COLOR_BLACK);
        print("[File loaded]\n\n");
        while (file_data[index] && index < 4095) {
            buffer[index] = file_data[index];
            putchar(file_data[index]);
            index++;
        }
    } else {
        fs_create(filename);
        set_text_color(COLOR_LIGHT_GREEN,COLOR_BLACK);
        print("[New file]\n\n");
    }

    int ctrl_pressed = 0;

    while (1) {
        uint8_t sc = keyboard_read_scancode();

        if (sc == 0x01) break; // ESC
        if (sc == 0x1D) { ctrl_pressed = 1; continue; }
        if (sc == 0x9D) { ctrl_pressed = 0; continue; }
        if (sc == 0x2A || sc == 0x36) {shift_pressed = 1; continue;}
        if (sc == 0xAA || sc == 0xB6) {shift_pressed = 0; continue;}

        if (ctrl_pressed && sc == 0x1F) { // CTRL+S
            if (fs_write(filename, buffer, index)) {
                print("\n[Saved successfully]\n");
            } else {
                set_text_color(COLOR_RED,COLOR_BLACK);
                print("\n[Save failed]\n");
            }
            continue;
        }
        // Enter
        if (sc == 0x1C) {
            buffer[index++] = '\n';
            terminal_row++;
            terminal_col = 0;
            continue;
        }
        //Backspace
        if (sc == 0x0E && index > 0) {
            index--;
            terminal_col = terminal_col ? terminal_col - 1 : 0;
            const uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
            vga_buffer[pos] = vga_entry(' ', terminal_color);
            continue;
        }

        char c = scancode_to_char(sc);
        if (c && index < 4095) {
            buffer[index++] = c;
            putchar(c);
        }

        if (index >= 4095) {
            set_text_color(COLOR_RED,COLOR_BLACK);
            print("\n[Buffer full — press ESC]\n");
        }
    }

    print("\n[Exiting editor]\n");
}
