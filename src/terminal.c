#include "terminal.h"

#include <string.h>

void terminal_init(Terminal *terminal) {
    memset(terminal, 0, sizeof(*terminal));
}

void terminal_put_char(Terminal *terminal, char input_char) {
    terminal->grid[terminal->cursor_row][terminal->cursor_col].character =
        input_char;
    if (terminal->cursor_col < TERM_COLS - 1) {
        terminal->cursor_col++;
    } else {
        if (terminal->cursor_row < TERM_ROWS - 1) {
            terminal->cursor_row++;
            terminal->cursor_col = 0;
        }
    }
}
