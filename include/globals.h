#pragma once

#include <stdint.h>
#include <string>
#include <vector>

double deltaTime;

struct Screen {
    int width;
    int height;
};

Screen screenDim = {1000, 1000};
std::vector<std::string> loadedMap;