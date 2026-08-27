

#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>

#define EMULATOR_NAME "Pavterm"
#define EMULATOR_V    "1.0.0"
#define TERM_WIDTH    800
#define TERM_HEIGHT   500
#define BLACK         0x00

int main(int argc, char *argv[]) {

    static SDL_Window *window = NULL;
    static SDL_Renderer *renderer = NULL;
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
    while (terminal_running) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            break;
        }
        SDL_SetRenderDrawColor(renderer, BLACK, BLACK, BLACK, BLACK);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
