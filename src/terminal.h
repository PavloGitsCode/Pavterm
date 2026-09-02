#ifndef PAVTERM_TERMINAL_H
#define PAVTERM_TERMINAL_H

#include <stdbool.h>

#define TERM_ROWS 25
#define TERM_COLS 80

typedef enum { PARSER_GROUND, PARSER_ESCAPE, PARSER_CSI } TerminalParserState;

typedef struct {
    TerminalParserState parser_state;
    bool is_private_mode;
    int csi_params[2];
    bool bracketed_paste_enabled;
} TerminalState;

typedef struct {
    char character;
} TerminalCell;

typedef struct {
    TerminalCell grid[TERM_ROWS][TERM_COLS];
    int cursor_row;
    int cursor_col;
} Terminal;

void terminal_init(Terminal *terminal, TerminalState *terminal_state);

void parse_char(Terminal *terminal, TerminalState *terminal_state,
                char input_char);

#endif
