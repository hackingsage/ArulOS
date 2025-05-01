#include "components/keyboard.h"
#include "screen/screen.h"
#include "string/string.h"

const char* expr_ptr;

int parse_number() {
    int num = 0;
    while (*expr_ptr >= '0' && *expr_ptr <= '9') {
        num = num * 10 + (*expr_ptr - '0');
        expr_ptr++;
    }
    return num;
}

int parse_factor() {
    while (*expr_ptr == ' ') expr_ptr++;
    if (*expr_ptr == '(') {
        expr_ptr++;
        int val = parse_expression();
        if (*expr_ptr == ')') expr_ptr++;
        return val;
    }
    return parse_number();
}

int parse_term() {
    int val = parse_factor();
    while (1) {
        while (*expr_ptr == ' ') expr_ptr++;
        char op = *expr_ptr;
        if (op != '*' && op != '/') break;
        expr_ptr++;
        int rhs = parse_factor();
        if (op == '*') val *= rhs;
        else if (rhs != 0) val /= rhs;
    }
    return val;
}

int parse_expression() {
    int val = parse_term();
    while (1) {
        while (*expr_ptr == ' ') expr_ptr++;
        char op = *expr_ptr;
        if (op != '+' && op != '-') break;
        expr_ptr++;
        int rhs = parse_term();
        if (op == '+') val += rhs;
        else val -= rhs;
    }
    return val;
}

void read_calc_input(char* buf, int max_len) {
    int idx = 0;
    while (1) {
        uint8_t sc = keyboard_read_scancode();
        // Special key handling
        if (sc == 0x01) { buf[0] = '\0'; return; } // ESC
        if (sc == 0x2A || sc == 0x36) {shift_pressed = 1; continue;}
        if (sc == 0xAA || sc == 0xB6) {shift_pressed = 0; continue;}
        if (sc == 0x1C) { buf[idx] = '\0'; print("\n"); return; }
        // Backspace handling
        if (sc == 0x0E && idx > 0) {
            idx--;
            terminal_col = terminal_col ? terminal_col - 1 : 0;
            const uint16_t pos = terminal_row * VGA_WIDTH + terminal_col;
            vga_buffer[pos] = vga_entry(' ', terminal_color);
            continue;
        }
        // Normal character input
        char c = scancode_to_char(sc);
        if (c && idx < max_len - 1) {
            buf[idx++] = c;
            putchar(c);
        }
    }
}

void calc_main() {
    set_text_color(COLOR_GREEN,COLOR_BLACK);
    print("\nAdvanced Calculator (Supports +, -, *, /, and parenthesis)\n");
    print("Press ESC to exit calculator.\n\n");

    while (1) {
        print(">> ");
        char input[128];
        read_calc_input(input, 128);
        if (input[0] == '\0') break;

        expr_ptr = input;
        int result = parse_expression();
        print("Result = ");
        print_int(result);
        print("\n");
    }
    print("\n[Exiting Calculator]...\n");
}
