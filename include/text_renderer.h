#pragma once

#include <string>
#include <SDL2/SDL_ttf.h>

class TextRenderer {
public:
    TextRenderer(SDL_Renderer* renderer, const std::string& fontPath, int fontSize);
    ~TextRenderer();

    void renderText(const std::string& text, int x, int y, SDL_Color color);

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
};

// Implementation

TextRenderer::TextRenderer(SDL_Renderer* renderer, const std::string& fontPath, int fontSize) {
    this->renderer = renderer;
    TTF_Init();
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
}

TextRenderer::~TextRenderer() {
    TTF_CloseFont(font);
    TTF_Quit();
}

void TextRenderer::renderText(const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
