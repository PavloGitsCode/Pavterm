
#include "pty.h"
#include "renderer.h"
#include "terminal.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <errno.h>
#include <pty.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define EMULATOR_NAME  "Pavterm"
#define EMULATOR_V     "1.0.0"
#define TERM_WIDTH     800
#define TERM_HEIGHT    500
#define BLACK          0x00
#define MAX_TEXT_INPUT 4096
#define FONT_PATH      "/usr/share/fonts/TTF/JetBrainsMono-Regular.ttf"
#define FONT_SIZE      16.0f

int main(int argc, char *argv[]) {

    int master_fd;
    pid_t shell_pid;

    if (start_shell(&master_fd, &shell_pid) == -1) {
        return 1;
    }

    PtyInputQue input_que = {0};
    char pty_buffer[512];

    static SDL_Window *window = NULL;
    static SDL_Renderer *renderer = NULL;

    Terminal terminal;
    TerminalState terminal_state;
    terminal_init(&terminal, &terminal_state);

    bool terminal_running = true;

    SDL_Event event;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed: Init SDL: %s",
                     SDL_GetError());
        return 3;
    }

    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        SDL_Quit();
        close(master_fd);
        waitpid(shell_pid, NULL, 0);
        return 1;
    }

    if (!SDL_CreateWindowAndRenderer(EMULATOR_NAME, TERM_WIDTH, TERM_HEIGHT,
                                     SDL_WINDOW_RESIZABLE, &window,
                                     &renderer)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed: Window and/or Renderer: %s", SDL_GetError());
        SDL_Quit();
        return 3;
    }

    TerminalRenderer term_renderer;
    if (!terminal_renderer_init(&term_renderer, renderer, FONT_PATH,
                                FONT_SIZE)) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        close(master_fd);
        waitpid(shell_pid, NULL, 0);
        return 1;
    }

    SDL_StartTextInput(window);
    while (terminal_running) {
        if (SDL_WaitEventTimeout(&event, 16)) {
            do {
                if (event.type == SDL_EVENT_QUIT) {
                    terminal_running = false;
                    break;
                }
                if (event.type == SDL_EVENT_TEXT_INPUT) {
                    size_t input_size = strlen(event.text.text);
                    if (!pty_enq_input(&input_que, event.text.text,
                                       input_size)) {
                        fprintf(stderr, "PTY input que is full\n");
                    }
                }
                if (event.type == SDL_EVENT_KEY_DOWN) {
                    switch (event.key.key) {
                    case SDLK_RETURN:
                        pty_enq_input(&input_que, "\r", 1);
                    }
                }
            } while (SDL_PollEvent(&event));
        }
        if (!terminal_running) {
            break;
        }

        if (pty_flush_input(master_fd, &input_que) == -1) {
            terminal_running = false;
            break;
        }

        for (;;) {
            ssize_t bytes_read =
                read_master_fd(master_fd, pty_buffer, sizeof(pty_buffer));
            if (bytes_read > 0) {
                for (size_t i = 0; i < bytes_read; i++) {
                    parse_char(&terminal, &terminal_state, pty_buffer[i]);
                }
                continue;
            }
            if (bytes_read == 0) {
                terminal_running = false;
                break;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            perror("read master_fd");
            terminal_running = false;
            break;
        }

        terminal_renderer_draw(&term_renderer, &terminal);
        SDL_RenderPresent(renderer);
    }

    terminal_renderer_destroy(&term_renderer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    close(master_fd);
    waitpid(shell_pid, NULL, 0);

    return 0;
}
