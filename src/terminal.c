#include "terminal.h"

#include <stdbool.h>
#include <string.h>

static void inc_digit_parser(TerminalState *terminal_state, char input_char) {
    terminal_state->csi_params[0] =
        (terminal_state->csi_params[0] * 10) + (input_char - '0');
}

static void display_char(Terminal *terminal, char input_char) {
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

void terminal_init(Terminal *terminal, TerminalState *terminal_state) {
    memset(terminal, 0, sizeof(*terminal));
    memset(terminal_state, 0, sizeof(*terminal_state));
}

void parse_char(Terminal *terminal, TerminalState *terminal_state,
                char input_char) {
    switch (terminal_state->parser_state) {

    case PARSER_GROUND:
        if (input_char == '\033') {
            terminal_state->parser_state = PARSER_ESCAPE;
        } else if (input_char == '\r') {
            terminal->cursor_col = 0;
        } else if (input_char == '\n') {
            terminal->cursor_row++;
        } else {
            display_char(terminal, input_char);
        }
        break;

    case PARSER_ESCAPE:
        if (input_char == '[') {
            terminal_state->parser_state = PARSER_CSI;
            terminal_state->is_private_mode = false;
        } else {
            terminal_state->parser_state = PARSER_GROUND;
        }
        break;

    case PARSER_CSI:
        if (input_char == '?') {
            terminal_state->is_private_mode = true;
        } else if (input_char >= '0' && input_char <= '9') {
            inc_digit_parser(terminal_state, input_char);
        } else if (input_char == 'h') {
            if (terminal_state->is_private_mode &&
                terminal_state->csi_params[0] == 2004) {
                terminal_state->bracketed_paste_enabled = true;
            }
            terminal_state->parser_state = PARSER_GROUND;

        } else if (input_char == 'l') {
            if (terminal_state->is_private_mode &&
                terminal_state->csi_params[0] == 2004) {
                terminal_state->bracketed_paste_enabled = false;
            }
            terminal_state->parser_state = PARSER_GROUND;
        }
        break;
    }
}
