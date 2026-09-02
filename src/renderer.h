#ifndef PAVTERM_RENDERER_H
#define PAVTERM_RENDERER_H

#include "terminal.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#define GLYPH_AMOUNT 256

typedef struct {
    SDL_Texture *texture;
    float width;
    float height;
} CachedGlyph;

typedef struct {
    SDL_Renderer *sdl_renderer;
    TTF_Font *font_path;

    int cell_width;
    int cell_height;

    CachedGlyph glyphs[GLYPH_AMOUNT];
} TerminalRenderer;

bool terminal_renderer_init(TerminalRenderer *renderer,
                            SDL_Renderer *sdl_renderer, const char *font_path,
                            float font_size);

void terminal_renderer_draw(TerminalRenderer *renderer,
                            const Terminal *terminal);

void terminal_renderer_destroy(TerminalRenderer *renderer);

#endif
