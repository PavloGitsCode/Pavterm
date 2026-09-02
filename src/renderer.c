#include "renderer.h"

#include <string.h>

static SDL_Texture *get_glyph_texture(TerminalRenderer *renderer,
                                      unsigned char character) {
    CachedGlyph *glyph = &renderer->glyphs[character];

    if (glyph->texture != NULL) {
        return glyph->texture;
    }

    char text[2] = {(char)character, '\0'};

    SDL_Color foreground = {255, 255, 255, 255};

    SDL_Surface *surface =
        TTF_RenderText_Blended(renderer->font_path, text, 1, foreground);

    if (surface == NULL) {
        SDL_Log("Unable to render glpyh: %s", SDL_GetError());
        return NULL;
    }

    glyph->texture =
        SDL_CreateTextureFromSurface(renderer->sdl_renderer, surface);

    if (glyph->texture != NULL) {
        glyph->width = (float)surface->w;
        glyph->height = (float)surface->h;
    }
    SDL_DestroySurface(surface);

    return glyph->texture;
}

bool terminal_renderer_init(TerminalRenderer *renderer,
                            SDL_Renderer *sdl_renderer, const char *font_path,
                            float font_size) {
    memset(renderer, 0, sizeof(*renderer));
    renderer->sdl_renderer = sdl_renderer;
    renderer->font_path = TTF_OpenFont(font_path, font_size);
    if (renderer->font_path == NULL) {
        SDL_Log("Unable to open font: %s", SDL_GetError());
        return false;
    }
    if (!TTF_GetStringSize(renderer->font_path, "M", 1, &renderer->cell_width,
                           &renderer->cell_height)) {
        SDL_Log("Unable to measure font: %s", SDL_GetError());
        TTF_CloseFont(renderer->font_path);
        renderer->font_path = NULL;
        return false;
    }
    return true;
}

void terminal_renderer_draw(TerminalRenderer *renderer,
                            const Terminal *terminal) {
    SDL_SetRenderDrawColor(renderer->sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer->sdl_renderer);
    for (int r = 0; r < TERM_ROWS; r++) {
        for (int c = 0; c < TERM_COLS; c++) {
            char character = terminal->grid[r][c].character;
            if (character == '\0' || character == ' ') {
                continue;
            }
            SDL_Texture *texture = get_glyph_texture(renderer, character);
            if (texture == NULL) {
                continue;
            }
            CachedGlyph *glyph = &renderer->glyphs[character];
            SDL_FRect destination = {.x = (float)(c * renderer->cell_width),
                                     .y = (float)(r * renderer->cell_height),
                                     .w = glyph->width,
                                     .h = glyph->height};
            SDL_RenderTexture(renderer->sdl_renderer, texture, NULL,
                              &destination);
        }
    }
}

void terminal_renderer_destroy(TerminalRenderer *renderer) {
    for (size_t i = 0; i < GLYPH_AMOUNT; i++) {
        SDL_DestroyTexture(renderer->glyphs[i].texture);
    }
    TTF_CloseFont(renderer->font_path);
    memset(renderer, 0, sizeof(*renderer));
}
