

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>

#define EMULATOR_NAME  "Pavterm"
#define EMULATOR_V     "1.0.0"
#define TERM_WIDTH     800
#define TERM_HEIGHT    500
#define BLACK          0x00
#define TERM_ROWS      25
#define TERM_COLS      80
#define MAX_TEXT_INPUT 4096

typedef struct {
    char character;
} TerminalCell;

typedef struct {
    TerminalCell grid[TERM_ROWS][TERM_COLS];
    int cursor_row;
    int cursor_col;
} Terminal;

static void render_terminal(SDL_Renderer *renderer, Terminal *term) {
    SDL_SetRenderDrawColor(renderer, BLACK, BLACK, BLACK, BLACK);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 220);

    for (int r = 0; r < TERM_ROWS; r++) {
        for (int c = 0; c < TERM_COLS; c++) {
            char character = term->grid[r][c].character;
            if (character == '\0' || character == ' ') {
                continue;
            }
            char text[2] = {character, '\0'};
            SDL_RenderDebugText(renderer, c * 8, r * 8, text);
        }
    }
}

static void term_put_char(Terminal *term, char input_char) {
    term->grid[term->cursor_row][term->cursor_col].character = input_char;
    if (term->cursor_col < TERM_COLS) {
        term->cursor_col++;
    } else {
        if (term->cursor_row < TERM_ROWS) {
            term->cursor_row++;
            term->cursor_col = 0;
        }
    }
}

int main(int argc, char *argv[]) {

    static SDL_Window *window = NULL;
    static SDL_Renderer *renderer = NULL;

    Terminal term;
    memset(&term, 0, sizeof(term));

    static bool terminal_running = true;
    SDL_Event event;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed: Init SDL: %s",
                     SDL_GetError());
        return 3;
    }

    if (!SDL_CreateWindowAndRenderer(EMULATOR_NAME, TERM_WIDTH, TERM_HEIGHT,
                                     SDL_WINDOW_RESIZABLE, &window,
                                     &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed: Window and/or Renderer: %s", SDL_GetError());
        SDL_Quit();
        return 3;
    }
    SDL_StartTextInput(window);
    while (terminal_running) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            break;
        }
        if (event.type == SDL_EVENT_TEXT_INPUT) {
            for (int event_text_index = 0;
                 event.text.text[event_text_index] != '\0';
                 event_text_index++) {
                term_put_char(&term, event.text.text[event_text_index]);
            }
        }
        render_terminal(renderer, &term);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
