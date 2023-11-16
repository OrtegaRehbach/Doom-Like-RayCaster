#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "color.h"

SDL_Window *window;
SDL_Renderer *renderer;
double deltaTime;
int gameTicks;
std::vector<std::string> loadedMap;
std::unordered_map<std::string, Color> colorMap = {
	{"0", Color(3, 150, 208)},
	{"1", Color(240, 200, 0)},
	{"2", Color(220, 36, 33)},
	{"3", Color(64, 169, 68)}
};
float mouseSensitivity = 10.0f;
bool running;

struct Screen {
    int width;
    int height;
	int centerX;
	int centerY;

	Screen(int w, int h) : width(w), height(h) {
		centerX = width / 2;
		centerY = height / 2;
	}
};

Screen screenDim = {900, 900};

struct Impact {
	float d;
	std::string mapHit;
	int tx;
};

enum GAME_STATE {
	MAIN_MENU,
	LEVEL_SELECT,
	IN_GAME,
	PAUSED,
	WIN
} currentGState;

enum INPUT_MODE {
	KB_LOOK,
	MOUSE_LOOK
} inputMode;

enum MENU_OPTION {
	M_PLAY,
	M_QUIT,
} selectedMenuOption;

enum MAP_SELECTION {
	MAP01,
	MAP02
} selectedMap;

enum PAUSE_OPTION {
	P_RESUME,
	P_MENU,
	P_QUIT
} selectedPauseOption;

void togglePause() {
	if (currentGState == IN_GAME || currentGState == PAUSED) {
		currentGState = (currentGState == PAUSED) ? IN_GAME : PAUSED;
		selectedPauseOption = P_RESUME;
	}
}

void toggleInputMode() {
	if (currentGState == IN_GAME)
		inputMode = (inputMode == KB_LOOK) ? MOUSE_LOOK : KB_LOOK;
}

void switchMenuOption(bool UP) {
	if (currentGState == MAIN_MENU) {
		if (UP) {
			if (selectedMenuOption == M_QUIT)
				selectedMenuOption = M_PLAY;
		} else {
			if (selectedMenuOption == M_PLAY)
				selectedMenuOption = M_QUIT;
		}
	}
}

void switchLevelOption(bool UP) {
	if (currentGState == LEVEL_SELECT) {
		if (UP) {
			if (selectedMap == MAP02)
				selectedMap = MAP01;
		} else {
			if (selectedMap == MAP01)
				selectedMap = MAP02;
		}
	}
}

void switchPauseOption(bool UP) {
	if (currentGState == PAUSED) {
		if (UP) {
			if (selectedPauseOption == P_QUIT)
				selectedPauseOption = P_MENU;
			else if (selectedPauseOption == P_MENU)
				selectedPauseOption = P_RESUME;
		} else {
			if (selectedPauseOption == P_RESUME)
				selectedPauseOption = P_MENU;
			else if (selectedPauseOption == P_MENU)
				selectedPauseOption = P_QUIT;
		}
	}
}

double normalizeAngle(double angle) {
    const double twoPi = 2 * M_PI;
    angle = fmod(angle, twoPi);
    if (angle < 0) {
        angle += twoPi;
    }
    return angle;
}