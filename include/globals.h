#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "color.h"

SDL_Window *window;
SDL_Renderer *renderer;
double deltaTime;
std::vector<std::string> loadedMap;
std::unordered_map<std::string, Color> colorMap = {
	{"0", Color(3, 150, 208)},
	{"1", Color(240, 200, 0)},
	{"2", Color(220, 36, 33)},
	{"3", Color(64, 169, 68)}
};

struct Screen {
    int width;
    int height;
};

Screen screenDim = {800, 800};

struct Impact {
	float d;
	Color c;
};
