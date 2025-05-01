#include <stdint.h>

// Multiboot header
__attribute__((section(".multiboot")))
const uint32_t multiboot_header[] = {
    0x1BADB002,     // Identifies kernel as multiboot-compliant
    0x00,           // Flags
    -(0x1BADB002)   // checksum
};

#include "screen/screen.h"
#include "components/keyboard.h"
#include "fs/fs.h"
#include "components/memory.h"
#include "shell/shell.h"

void print_banner() {
    set_text_color(COLOR_LIGHT_CYAN, COLOR_BLACK);
    print("\n");
    print("    _    ____  _   _ _     ___  ____\n");  
    print("   / \\  |  _ \\| | | | |   / _ \\/ ___|\n"); 
    print("  / _ \\ | |_) | | | | |  | | | \\___ \\\n"); 
    print(" / ___ \\|  _ <| |_| | |__| |_| |___) |\n");
    print("/_/   \\_\\_| \\_\\___/|_____\\___/|____/\n"); 
    print("\n");
    set_text_color(COLOR_YELLOW, COLOR_BLACK);
    print("Welcome to ArulOS Terminal\n\n");
}

void kernel_main(uint32_t magic, uint32_t addr){
    fs_init();
    memory_init();

    clear_screen();
    print_banner();

    set_text_color(COLOR_LIGHT_GREEN, COLOR_BLACK);
    print("Type 'help' for a list of commands.\n\n");

    shell_loop();
}
