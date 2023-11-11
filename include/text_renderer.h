#pragma once

#include <string>
#include <SDL2/SDL_ttf.h>

class TextRenderer {
public:
    TextRenderer(SDL_Renderer* renderer, const std::string& fontPath, int fontSize);
    ~TextRenderer();

    void renderText(const std::string& text, int x, int y, SDL_Color color);
    void renderTextCentered(const char* text, int x, int y, const Color& color);

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

// Render text centered at a specific position
void TextRenderer::renderTextCentered(const char* text, int x, int y, const Color& color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, { color.r, color.g, color.b });
    if (!surface) {
        throw std::runtime_error("Unable to render text! SDL_ttf Error: " + std::string(TTF_GetError()));
    }

    // Calculate the center position
    int centerX = x - surface->w / 2;
    int centerY = y - surface->h / 2;

    // Convert surface to texture
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        throw std::runtime_error("Unable to create texture from surface! SDL Error: " + std::string(SDL_GetError()));
    }

    // Set render destination and render the texture
    SDL_Rect destRect = { centerX, centerY, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    // Free the created surface and texture
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
