#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <sstream>

#include "globals.h"
#include "color.h"
#include "raycaster.h"
#include "imageloader.h"
#include "text_renderer.h"

void init() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("DOOM", 0, 0, screenDim.width, screenDim.height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    ImageLoader::init();
}

void quit() {
    ImageLoader::cleanup();
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void clear() {
    SDL_SetRenderDrawColor(renderer, B.r, B.g, B.b, B.a);
    SDL_RenderClear(renderer);
}

int main() {
    init();
    Raycaster r = {renderer};
    r.load_map("../assets/maps/map1.txt");
    selectedMap = MAP01;

    TextRenderer textRenderer(renderer, "../assets/fonts/Pixeled.ttf", 24);

    ImageLoader::loadImage("0", "../assets/textures/wall.png");
    ImageLoader::loadImage("1", "../assets/textures/wall2.jpg");
    ImageLoader::loadImage("2", "../assets/textures/wall2.jpg");
    ImageLoader::loadImage("3", "../assets/textures/wall2.jpg");

    uint64_t perfFrequency = SDL_GetPerformanceFrequency();
    uint64_t frameStart = SDL_GetPerformanceCounter();

    running = true;
    currentGState = MAIN_MENU;
    while (running) {
        clear();
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
                if (event.key.keysym.sym == SDLK_RETURN) {
                    if (currentGState == MAIN_MENU) {
                        if (selectedOption == PLAY)
                            currentGState = LEVEL_SELECT;
                        if (selectedOption == QUIT)
                            running = false;
                            break;
                    }
                    if (currentGState == LEVEL_SELECT) {
                        if (selectedMap == MAP01)
                            r.load_map("../assets/maps/map.txt");
                        if (selectedMap == MAP02)
                            r.load_map("../assets/maps/map2.txt");
                        currentGState = IN_GAME;
                    }
                }
                if (event.key.keysym.sym == SDLK_p) {
                    togglePause();
                }
                if (event.key.keysym.sym == SDLK_i) {
                    toggleInputMode();
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    switchMenuOption(true);
                    switchLevelOption(true);
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    switchMenuOption(false);
                    switchLevelOption(false);
                }
            }
            if (event.type == SDL_MOUSEMOTION && inputMode == MOUSE_LOOK) { // Only works while mouse is inside the window
                int moveDir = event.motion.xrel;
                if (moveDir > 0)
                    r.player.turnRightMouse();
                else
                    r.player.turnLeftMouse();
            }
        }
        if (currentGState == MAIN_MENU) {
            r.rect(0, 0, screenDim.width, screenDim.height, Color(24, 24, 24));
            int titleX = screenDim.centerX - (501 / 2);
            int titleY = screenDim.centerY - (screenDim.height / 4);
            ImageLoader::render(renderer, "../assets/textures/title.png", titleX, titleY);
            Color c1 = (selectedOption == PLAY) ? Color{255, 0, 0, 255} : Color{255, 255, 255, 255};
            Color c2 = (selectedOption == QUIT) ? Color{255, 0, 0, 255} : Color{255, 255, 255, 255};
            textRenderer.renderTextCentered("PLAY", screenDim.centerX, screenDim.centerY, c1);
            textRenderer.renderTextCentered("QUIT", screenDim.centerX, screenDim.centerY + screenDim.height / 8, c2);
        }
        if (currentGState == LEVEL_SELECT) {
            r.rect(0, 0, screenDim.width, screenDim.height, Color(24, 24, 24));
            textRenderer.renderTextCentered("LEVEL SELECT:", screenDim.centerX, screenDim.centerY - screenDim.height / 4, {255, 255, 255, 255});
            Color c1 = (selectedMap == MAP01) ? Color{255, 0, 0, 255} : Color{255, 255, 255, 255};
            Color c2 = (selectedMap == MAP02) ? Color{255, 0, 0, 255} : Color{255, 255, 255, 255};
            textRenderer.renderTextCentered("MAP 1", screenDim.centerX, screenDim.centerY, c1);
            textRenderer.renderTextCentered("MAP 2", screenDim.centerX, screenDim.centerY + screenDim.height / 8, c2);
        }
        if (currentGState == IN_GAME) {
            if (KeyboardState[SDL_SCANCODE_LEFT] && !KeyboardState[SDL_SCANCODE_RIGHT])
                r.player.turnLeft();
            if (KeyboardState[SDL_SCANCODE_RIGHT] && !KeyboardState[SDL_SCANCODE_LEFT])
                r.player.turnRight();
            if (KeyboardState[SDL_SCANCODE_W] && !KeyboardState[SDL_SCANCODE_S])
                r.player.moveForward();
            if (KeyboardState[SDL_SCANCODE_S] && !KeyboardState[SDL_SCANCODE_W])
                r.player.moveBackward();
            if (KeyboardState[SDL_SCANCODE_A] && !KeyboardState[SDL_SCANCODE_D])
                r.player.moveLeft();
            if (KeyboardState[SDL_SCANCODE_D] && !KeyboardState[SDL_SCANCODE_A])
                r.player.moveRight(); 
            r.render();
        }
        if (currentGState == PAUSED) {
            r.render();
            r.rect(0, 0, screenDim.width, Color(16, 16, 16, 120));
            textRenderer.renderTextCentered("PAUSED", screenDim.centerX, screenDim.centerY - screenDim.height / 4, {255, 255, 255, 255});
        }

        SDL_RenderPresent(renderer);

        // Delta time (in seconds)

        uint64_t frameEnd = SDL_GetPerformanceCounter();
        deltaTime = (double)((frameEnd - frameStart) / (double)perfFrequency);
        
        // FPS counter

        float fps = 1 / deltaTime;
        std::ostringstream titleStream;
        if(deltaTime > 0) {
            titleStream << "FPS: " << static_cast<int>(fps);
            SDL_SetWindowTitle(window, titleStream.str().c_str());
        }
        frameStart = frameEnd;
    }

    
    exit(0);
}
