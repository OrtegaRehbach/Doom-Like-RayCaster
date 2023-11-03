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
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_LEFT:
                    r.player.turnLeft();
                    break;
                case SDLK_RIGHT:
                    r.player.turnRight();
                    break;
                case SDLK_w:
                    r.player.moveForward();
                    break;
                case SDLK_s:
                    r.player.moveBackward();
                    break;
                case SDLK_a:
                    r.player.moveLeft();
                    break;
                case SDLK_d:
                    r.player.moveRight();
                    break;
                default:
                    break;
                }
            }
        }

        clear();
        r.render();

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(0);
}
