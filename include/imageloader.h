#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdexcept>
#include <map>
#include <string>

#include "color.h"

class ImageLoader {
private:
    static std::map<std::string, SDL_Surface*> imageSurfaces;
public:
    // Initialize SDL_image
    static void init() {
        int imgFlags = IMG_INIT_PNG; // or IMG_INIT_JPG, depending on your needs
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            throw std::runtime_error("SDL_image could not initialize! SDL_image Error: " + std::string(IMG_GetError()));
        }
    }

    // Load an image from a given path and store with a key
    static void loadImage(const std::string& key, const char* path) {
        SDL_Surface* newSurface = IMG_Load(path);
        if (!newSurface) {
            throw std::runtime_error("Unable to load image! SDL_image Error: " + std::string(IMG_GetError()));
        }
        imageSurfaces[key] = newSurface;
    }

    // Get the color of the pixel at (x, y) from an image with a specific key
    static Color getPixelColor(const std::string& key, int x, int y) {
        auto it = imageSurfaces.find(key);
        if (it == imageSurfaces.end()) {
            throw std::runtime_error("Image key not found!");
        }

        SDL_Surface* targetSurface = it->second;
        int bpp = targetSurface->format->BytesPerPixel;
        Uint8 *p = (Uint8 *)targetSurface->pixels + y * targetSurface->pitch + x * bpp;

        Uint32 pixelColor;
        switch (bpp) {
            case 1:
                pixelColor = *p;
                break;
            case 2:
                pixelColor = *(Uint16 *)p;
                break;
            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                    pixelColor = p[0] << 16 | p[1] << 8 | p[2];
                } else {
                    pixelColor = p[0] | p[1] << 8 | p[2] << 16;
                }
                break;
            case 4:
                pixelColor = *(Uint32 *)p;
                break;
            default:
                throw std::runtime_error("Unknown format!");
        }
        
        SDL_Color color;
        SDL_GetRGB(pixelColor, targetSurface->format, &color.r, &color.g, &color.b);
        return Color{color.r, color.g, color.b};
    }

    static void renderFromMap(SDL_Renderer* renderer, const std::string& key, int x, int y) {
        auto it = imageSurfaces.find(key);
        if (it == imageSurfaces.end()) {
            throw std::runtime_error("Image key not found!");
        }

        SDL_Surface* targetSurface = it->second;

        // Convert surface to texture
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, targetSurface);
        if (!texture) {
            throw std::runtime_error("Unable to create texture from surface! SDL Error: " + std::string(SDL_GetError()));
        }

        // Set render destination and render the texture
        SDL_Rect destRect = { x, y, targetSurface->w, targetSurface->h };
        SDL_RenderCopy(renderer, texture, NULL, &destRect);

        // Free the created texture
        SDL_DestroyTexture(texture);
    }

    // Render an image from a file path at a specific position
    static void render(SDL_Renderer* renderer, const std::string& path, int x, int y) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            throw std::runtime_error("Unable to load image! SDL_image Error: " + std::string(IMG_GetError()));
        }

        // Convert surface to texture
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            throw std::runtime_error("Unable to create texture from surface! SDL Error: " + std::string(SDL_GetError()));
        }

        // Set render destination and render the texture
        SDL_Rect destRect = { x, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &destRect);

        // Free the created surface and texture
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // Render an image centered on screen
    static void renderCentered(SDL_Renderer* renderer, const std::string& path, int centerX, int centerY) {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            throw std::runtime_error("Unable to load image! SDL_image Error: " + std::string(IMG_GetError()));
        }

        // Convert surface to texture
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture) {
            throw std::runtime_error("Unable to create texture from surface! SDL Error: " + std::string(SDL_GetError()));
        }

        // Set the render destination at the center of the screen
        SDL_Rect destRect = { centerX - surface->w / 2, centerY - surface->h / 2, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &destRect);

        // Free the created surface and texture
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // Clean up
    static void cleanup() {
        for (auto& pair : imageSurfaces) {
            if (pair.second) {
                SDL_FreeSurface(pair.second);
            }
        }
        imageSurfaces.clear();
        IMG_Quit();
    }
};

std::map<std::string, SDL_Surface*> ImageLoader::imageSurfaces;
