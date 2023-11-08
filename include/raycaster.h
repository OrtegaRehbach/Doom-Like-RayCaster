#pragma once

#include <iostream>
#include <fstream>
#include <SDL_render.h>
#include <string>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
#include <unordered_map>

#include "color.h"
#include "player.h"

const Color B(0, 0, 0);
const Color W;

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
const int BLOCK = 25;
const int MAX_RAY_DISTANCE = 10000;


std::unordered_map<std::string, Color> colors = {
	{"0", Color(3, 150, 208)},
	{"1", Color(240, 200, 0)},
	{"2", Color(220, 36, 33)},
	{"3", Color(64, 169, 68)}
};

struct Impact {
	float d;
	Color c;
};

class Raycaster {
public:
	Player player;

	Raycaster(SDL_Renderer *renderer)
		: renderer(renderer) {

		player.x = BLOCK + BLOCK / 2;
		player.y = BLOCK + BLOCK / 2;

		player.a = M_PI / 4.0f;
		player.fov = M_PI / 3.0f;

		player.movementSpeed = 200;
		player.turningSpeed = 1.5f;
		player.blockSize = BLOCK;

		scale = 100;
	}

	void load_map(const std::string &filename) {
		std::ifstream file(filename);
		if (file.is_open()) {
			std::string line;
			while (getline(file, line)) {
				map.push_back(line);
			}
			file.close();
		} else {
			std::cerr << "Failed to open the map file." << std::endl;
		}
		loadedMap = map;
		player.map = loadedMap;
	}

	void point(int x, int y, Color c) {
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		SDL_RenderDrawPoint(renderer, x, y);
	}

	void rect(int x, int y, int blockSize = BLOCK, Color c = W) {
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		SDL_Rect rect = {x, y, blockSize, blockSize};
		SDL_RenderFillRect(renderer, &rect);
	}

	Impact cast_ray(float a, bool drawRay = false, const Color& c = W, int drawDistance = MAX_RAY_DISTANCE, int blockSize = BLOCK) {
		float d = 0;
		std::string mapHit;
		while (d <= MAX_RAY_DISTANCE) {
			int x = static_cast<int>(player.x + d * cos(a));
			int y = static_cast<int>(player.y + d * sin(a));

			int i = static_cast<int>(x / blockSize);
			int j = static_cast<int>(y / blockSize);

			if (map[j][i] != ' ') {
				mapHit = map[j][i];
				break;
			}

			if (drawRay && d <= drawDistance)
				point(x, y, c);

			d += 1;
		}
		return Impact{ d, colors[mapHit] };
	}

	Impact cast_ray_from_point(int x, int y, float a, bool drawRay = false, const Color& c = W, int drawDistance = MAX_RAY_DISTANCE, int blockSize = BLOCK) {
		float d = 0;
		std::string mapHit;
		while (d <= MAX_RAY_DISTANCE) {
			int _x = static_cast<int>(x + d * cos(a));
			int _y = static_cast<int>(y + d * sin(a));

			int i = static_cast<int>(_x / blockSize);
			int j = static_cast<int>(_y / blockSize);

			if (map[j][i] != ' ') {
				mapHit = map[j][i];
				break;
			}

			if (drawRay && d <= drawDistance)
				point(_x, _y, c);

			d += 1;
		}
		return Impact{ d, colors[mapHit] };
	}

	void draw_stake(int x, float h, Color c) {
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		float start = SCREEN_HEIGHT / 2.0f - h / 2.0f;
		SDL_Rect rect = {x, static_cast<int>(start), 1, static_cast<int>(h)};
		SDL_RenderFillRect(renderer, &rect);
	}

	float get_brightness(float d) {
		float minViewDistance = 350.0f;
		float maxViewDistance = 600.0f;
		int stepAmount = 4;
		float stepSize = (maxViewDistance - minViewDistance) / stepAmount;
		if (d < minViewDistance)
			return 1.0f;
		if (d > maxViewDistance)
			return 0.0f;
		if (d >= minViewDistance && d < minViewDistance + stepSize)
			return 0.75f;
		if (d >= minViewDistance + stepSize && d < minViewDistance + stepSize * 2)
			return 0.5f;
		if (d >= minViewDistance + stepSize * 2 && d < minViewDistance + stepSize * 3)
			return 0.25f;
		if (d >= minViewDistance + stepSize * 3 && d <= minViewDistance + stepSize * 4)
			return 0.10f;
		return 1.0f;
	}

	void draw_minimap(int xPos = 0, int yPos = 0, int mapWidth = 500, int mapHeight = 500) {
		int blockAmount = SCREEN_WIDTH / BLOCK;
		int minimapBlockSize = mapWidth / blockAmount;
		rect(xPos, yPos, mapWidth, B);	// Draw minimap background
		for (int x = xPos; x < xPos + mapWidth; x += minimapBlockSize) {
			for (int y = yPos; y < yPos + mapHeight; y += minimapBlockSize) {
				int i = static_cast<int>((x - xPos) / minimapBlockSize);
				int j = static_cast<int>((y - yPos) / minimapBlockSize);
				if (map[j][i] != ' ') {
					std::string mapHit;
					mapHit = map[j][i];
					Color c = colors[mapHit];
					rect(x, y, minimapBlockSize, c);
				}
			}
		}
		float mapScaleFactorX = (float)mapWidth / (float)SCREEN_WIDTH;
		float mapScaleFactorY = (float)mapHeight / (float)SCREEN_HEIGHT;
		int playerPosX = xPos + mapScaleFactorX * player.x;
		int playerPosY = yPos + mapScaleFactorY * player.y;
		for (int i = 1; i < mapWidth; i++) {
			float a = player.a + player.fov / 2 - player.fov * i / mapWidth;
			if (i == 1 || i == mapWidth - 1)
				cast_ray_from_point(playerPosX, playerPosY, a, true, W, MAX_RAY_DISTANCE, minimapBlockSize);
		}
		cast_ray_from_point(playerPosX, playerPosY, player.a, true, Color(255, 0, 0), MAX_RAY_DISTANCE, minimapBlockSize);
	}

	void render() {		
		// draw right side of the screen
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			double a = player.a + player.fov / 2.0 - player.fov * i / SCREEN_WIDTH;
			Impact impact = cast_ray(a);
			float d = impact.d;
			Color c = impact.c * get_brightness(d);

			int x = SCREEN_WIDTH * 2 - i;
			// int x = SCREEN_WIDTH - i;
			float h = static_cast<float>(SCREEN_HEIGHT) / static_cast<float>(d * cos(a - player.a)) * static_cast<float>(scale);
			draw_stake(x, h, c);
		}
		draw_minimap(0, 0, 200, 200);
	}

private:
	int scale;
	SDL_Renderer *renderer;
	std::vector<std::string> map;
};
