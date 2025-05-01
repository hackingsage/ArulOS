#include "screen/screen.h"
#include "components/keyboard.h"
#include "fs/fs.h"
#include "string/string.h"
#include "applications/editor_main.h"
#include "applications/calc_main.h"
#include "components/ports.h"

#define MAX_HISTORY 10
#define MAX_CMD_LEN 128

char history[MAX_HISTORY][MAX_CMD_LEN];
int history_count = 0;


void read_shell_input(char* buf, int max_len) {
    int idx = 0;
    int history_index = history_count;  // Start after last
    buf[0] = '\0';

    while (1) {
        uint8_t sc = keyboard_read_scancode();

        if (sc == 0x48) { // Up Arrow
            if (history_index > 0) history_index--;
            // Load from history[history_index]
            const char* hist_cmd = history[history_index];
            // Clear current line
            while (idx > 0) {
                terminal_col--;
                const uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
                vga_buffer[pos] = vga_entry(' ', terminal_color);
                idx--;
            }
            // Copy from history
            idx = 0;
            for (int i = 0; hist_cmd[i] && idx < max_len - 1; i++) {
                buf[idx++] = hist_cmd[i];
                putchar(hist_cmd[i]);
            }
            buf[idx] = '\0';
            continue;
        }

        if (sc == 0x50) { // Down Arrow
            if (history_index < history_count - 1) history_index++;
            else {
                // Reset to empty line
                history_index = history_count;
                buf[0] = '\0';
                idx = 0;
                while (terminal_col > 0) {
                    terminal_col--;
                    const uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
                    vga_buffer[pos] = vga_entry(' ', terminal_color);
                }
                continue;
            }
            const char* hist_cmd = history[history_index];
            // Clear current line
            while (idx > 0) {
                terminal_col--;
                const uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
                vga_buffer[pos] = vga_entry(' ', terminal_color);
                idx--;
            }
            // Copy from history
            idx = 0;
            for (int i = 0; hist_cmd[i] && idx < max_len - 1; i++) {
                buf[idx++] = hist_cmd[i];
                putchar(hist_cmd[i]);
            }
            buf[idx] = '\0';
            continue;
        }

        if (sc == 0x1C) { // ENTER
            buf[idx] = '\0';
            print("\n");
            // Save to history if not empty
            if (idx > 0) {
                if (history_count < MAX_HISTORY) {
                    strncpy(history[history_count++], buf, MAX_CMD_LEN);
                } else {
                    for (int i = 1; i < MAX_HISTORY; i++) {
                        strncpy(history[i - 1], history[i], MAX_CMD_LEN);
                    }
                    strncpy(history[MAX_HISTORY - 1], buf, MAX_CMD_LEN);
                }
            }
            return;
        }

        if (sc == 0x0E && idx > 0) { // BACKSPACE
            idx--;
            terminal_col = terminal_col ? terminal_col - 1 : 0;
            const uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
            vga_buffer[pos] = vga_entry(' ', terminal_color);
            continue;
        }

        if (sc == 0x0F) { // TAB key
            const char* partial = get_last_word(buf);
        
            int match_index = -1;
            int match_count = 0;
            char match_name[32] = {0};
        
            for (int i = 0; i < fs_get_file_count(); i++) {
                const char* name = fs_get_filename(i);
                if (strncmp(name, partial, strlen(partial)) == 0) {
                    if (++match_count == 1) {
                        strncpy(match_name, name, sizeof(match_name));
                        match_index = i;
                    } else {
                        set_text_color(COLOR_YELLOW,COLOR_BLACK);
                        print("\nMultiple matches:\n");
                        print(name);
                        print("\n");
                    }
                }
            }
        
            if (match_count == 1) {
                // Remove current partial word
                int plen = strlen(partial);
                while (plen-- && idx > 0) {
                    terminal_col--;
                    const uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
                    vga_buffer[pos] = vga_entry(' ', terminal_color);
                    idx--;
                }
                // Append match
                const char* full = fs_get_filename(match_index);
                for (int j = 0; full[j] && idx < max_len - 1; j++) {
                    buf[idx++] = full[j];
                    putchar(full[j]);
                }
                buf[idx] = '\0';
            }
            continue;
        }
        
        char c = scancode_to_char(sc);
        if (c && idx < max_len - 1) {
            buf[idx++] = c;
            putchar(c);
            buf[idx] = '\0';
            history_index = history_count;
        }
    }
}


int starts_with(const char* str, const char* prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++;
        prefix++;
    }
    return 1;
}

// CPU Reboot via keyboard controller
void reboot() {
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);  // Reads the keyboard status port (0x64) until the input buffer is clear (bit 1 is 0)
    outb(0x64, 0xFE); // Send reboot command
    for(;;); // Hang if fails
}

void shell_loop() {
    char input[128];

    while (1) {
        set_text_color(COLOR_LIGHT_GREY, COLOR_BLACK);
        print("> ");
        read_shell_input(input, 128);

        if (strcmp(input, "help") == 0) {
            print("\nAvailable commands:\n");
            print("- help: Show available commands\n");
            print("- clear: Clear the screen\n");
            print("- about: About ArulOS\n");
            print("- reboot: Reboot the system\n");
            print("- ls: List files\n");
            print("- cat <filename>: View file contents\n");
            print("- touch <filename(no spaces)> <content> \n");
            print("- rm: remove a file\n");
            print("- run editor: Run Editor\n");
            print("- run calc: run calculator\n");
            print("- shutdown/exit\n");
        }
        else if (strcmp(input, "ls") == 0) {
            fs_list_files();
        }
        else if (strcmp(input, "shutdown") == 0 || strcmp(input, "exit") == 0) {
            print("Shutting down...\n");
            outw(0x604, 0x2000);  // QEMU ACPI shutdown
            while (1); // Fallback loop
        }        
        else if (starts_with(input, "cat ")) {
            const char* filename = input + 4;   // skip "cat " part
            const char* data = fs_read(filename);
            if (data){
                print(data);
                print("\n");
            }
            else{ 
                set_text_color(COLOR_RED,COLOR_BLACK);
                print("File not found\n");
            }
        }
        else if (starts_with(input, "touch ")) {
            const char* filename = input + 6;
            if (fs_create(filename)) print("File created.\n");
            else print("Failed to create file.\n");
        }
        else if (strcmp(input, "run editor") == 0) {
            clear_screen();
            editor_main();
        }
        else if (strcmp(input, "run calc") == 0) {
            clear_screen();
            calc_main();
        }
        else if (strcmp(input, "clear") == 0) {
            clear_screen();
        }
        else if (strcmp(input, "about") == 0) {
            set_text_color(COLOR_BLUE,COLOR_BLACK);
            print("\nArulOS - A simple operating system project made by hackingsage.\nBuilt from scratch in C and Assembly!\n");
        }
        else if (strcmp(input, "reboot") == 0){
            reboot();
        }
        else if (starts_with(input, "rm ")) {
            const char* filename = input + 3;
            if (fs_delete(filename)) {
                print("File deleted.\n");
            } else {
                set_text_color(COLOR_RED,COLOR_BLACK);
                print("File not found.\n");
            }
        }              
        else {
            set_text_color(COLOR_RED,COLOR_BLACK);
            print("Unknown command\n");
        }
    }
}
