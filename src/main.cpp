#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include "color.h"
#include "raycaster.h"

SDL_Window *window;
SDL_Renderer *renderer;

void clear() {
    SDL_SetRenderDrawColor(renderer, B.r, B.g, B.b, B.a);
    SDL_RenderClear(renderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("DOOM", 0, 0, 1000, 500, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Raycaster r = {renderer};
    r.load_map("../assets/map.txt");

    bool running = true;
    while (running) {
        SDL_Event event;
        const Uint8* KeyboardState = SDL_GetKeyboardState(nullptr);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    break;
                }
            }
        }
        if (KeyboardState[SDL_SCANCODE_LEFT])
            r.player.turnLeft();
        if (KeyboardState[SDL_SCANCODE_RIGHT])
            r.player.turnRight();
        if (KeyboardState[SDL_SCANCODE_W])
            r.player.moveForward();
        if (KeyboardState[SDL_SCANCODE_S])
            r.player.moveBackward();
        if (KeyboardState[SDL_SCANCODE_A])
            r.player.moveLeft();
        if (KeyboardState[SDL_SCANCODE_D])
            r.player.moveRight();

        clear();
        r.render();

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(0);
}
