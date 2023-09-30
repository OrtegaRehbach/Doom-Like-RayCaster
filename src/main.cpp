#include <SDL2/SDL.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include "raycaster.h"

SDL_Window* window;
SDL_Renderer* renderer;

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("DOOM", 0, 0, 1000, 1000, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
        }
        // Present the framebuffer to the screen
        SDL_RenderPresent(renderer);
    }

    // Quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}