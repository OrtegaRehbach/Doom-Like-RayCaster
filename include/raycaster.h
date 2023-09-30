#pragma once

#include <SDL2/SDL.h>
#include <SDL_render.h>
#include <string>
#include <vector>

struct Player {
    int x;      // X Position
    int y;      // Y Position
    float a;    // Angle
};

class RayCaster {
    public:
        RayCaster(SDL_Renderer* renderer)
            : renderer(renderer) {}
    private:
        SDL_Renderer* renderer;
        Player player;
        std::vector<std::string> map;
};