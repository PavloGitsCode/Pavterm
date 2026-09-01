#ifndef PAVTERM_TERMINAL_H
#define PAVTERM_TERMINAL_H

#define TERM_ROWS 25
#define TERM_COLS 80

typedef struct {
    char character;
} TerminalCell;

typedef struct {
    TerminalCell grid[TERM_ROWS][TERM_COLS];
    int cursor_row;
    int cursor_col;
} Terminal;

void terminal_init(Terminal *terminal);

void terminal_put_char(Terminal *terminal, char input_char);

#endif
