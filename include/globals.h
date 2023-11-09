#pragma once

#include <stdint.h>
#include <string>
#include <vector>

double deltaTime;

struct Screen {
    int width;
    int height;
};

Screen screenDim = {800, 800};
std::vector<std::string> loadedMap;