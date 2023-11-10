#pragma once

#include <iostream>
#include <fstream>
#include <SDL_render.h>
#include <cmath>
#include <SDL2/SDL.h>

#include "player.h"

const Color B(0, 0, 0);
const Color W;

const int SCREEN_WIDTH = screenDim.width;
const int SCREEN_HEIGHT = screenDim.height;
const int MAX_RAY_DISTANCE = 10000;

int BLOCK_AMT_X = 20;
int BLOCK_AMT_Y = 20;
int BLOCK = SCREEN_WIDTH / BLOCK_AMT_X;

class Raycaster {
public:
	Player player;

	Raycaster(SDL_Renderer *renderer)
		: renderer(renderer) {

		player.x = BLOCK + BLOCK / 2.0f;
		player.y = BLOCK + BLOCK / 2.0f;

		player.a = M_PI / 4.0f;
		player.fov = M_PI / 3.0f;

		player.movementSpeed = 200;
		player.turningSpeed = 1.5f;
		player.blockSize = BLOCK;

		scale = 100;
	}

	void load_map(const std::string &filename) {
		int blockAmtX = 0;
		int blockAmtY = 0;
		std::ifstream file(filename);
		if (file.is_open()) {
			std::string line;
			while (getline(file, line)) {
				blockAmtX = (line.length() > blockAmtX) ? line.length() : blockAmtX;
				map.push_back(line);
				blockAmtY++;
			}
			file.close();
			BLOCK_AMT_X = blockAmtX;
			BLOCK_AMT_Y = blockAmtY;
			loadedMap = map;
		} else {
			std::cerr << "Failed to open the map file." << std::endl;
		}
	}

	void point(int x, int y, Color c) {
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		SDL_RenderDrawPoint(renderer, x, y);
	}

	void rect(int x, int y, int width = BLOCK, int height = BLOCK, Color c = W) {
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		SDL_Rect rect = {x, y, width, height};
		SDL_RenderFillRect(renderer, &rect);
	}

	void rect(int x, int y, int blockSize = BLOCK, Color c = W) {
		rect(x, y, blockSize, blockSize, c);
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
		return Impact{ d, colorMap[mapHit] };
	}

	Impact cast_ray_from_point(
		int x, int y, float a, bool drawRay = false, const Color& c = W, int drawDistance = MAX_RAY_DISTANCE, 
		int blockSizeX = BLOCK, int blockSizeY = BLOCK
	) {
		float d = 0;
		std::string mapHit;
		while (d <= MAX_RAY_DISTANCE) {
			int _x = static_cast<int>(x + d * cos(a));
			int _y = static_cast<int>(y + d * sin(a));

			int i = static_cast<int>(_x / blockSizeX);
			int j = static_cast<int>(_y / blockSizeY);

			if (map[j][i] != ' ') {
				mapHit = map[j][i];
				break;
			}

			if (drawRay && d <= drawDistance)
				point(_x, _y, c);

			d += 1;
		}
		return Impact{ d, colorMap[mapHit] };
	}

	void draw_stake(int x, float h, Color c) {
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		float start = SCREEN_HEIGHT / 2.0f - h / 2.0f;
		SDL_Rect rect = {x, static_cast<int>(start), 1, static_cast<int>(h)};
		SDL_RenderFillRect(renderer, &rect);
	}

	float get_brightness(float d) {
		float minViewDistance = 16 * BLOCK; //350.0f;
		float maxViewDistance = 24 * BLOCK; //600.0f;
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
		int minimapBlockSizeX = mapWidth / blockAmount;
		int minimapBlockSizeY = mapHeight / blockAmount;
		int border = 2;
		rect(xPos, yPos, mapWidth + border, mapHeight + border, B);	// Draw minimap background
		for (int x = xPos; x < xPos + mapWidth; x += minimapBlockSizeX) {
			for (int y = yPos; y < yPos + mapHeight; y += minimapBlockSizeY) {
				int i = static_cast<int>((x - xPos) / minimapBlockSizeX);
				int j = static_cast<int>((y - yPos) / minimapBlockSizeY);
				if (map[j][i] != ' ') {
					std::string mapHit;
					mapHit = map[j][i];
					Color c = colorMap[mapHit];
					rect(x, y, minimapBlockSizeX, minimapBlockSizeY, c);
				}
			}
		}
		float mapScaleFactor = (float)mapWidth / (float)SCREEN_WIDTH;
		int playerPosX = xPos + mapScaleFactor * player.x;
		int playerPosY = yPos + mapScaleFactor * player.y;
		for (int i = 1; i < mapWidth; i++) {
			float a = player.a + player.fov / 2 - player.fov * i / mapWidth;
			if (i == 1 || i == mapWidth - 1)
				cast_ray_from_point(playerPosX, playerPosY, a, true, W, MAX_RAY_DISTANCE, minimapBlockSizeX, minimapBlockSizeY);
		}
		cast_ray_from_point(playerPosX, playerPosY, player.a, true, Color(255, 0, 0), MAX_RAY_DISTANCE, minimapBlockSizeX, minimapBlockSizeY);
	}

	void draw_player_view() {
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			double a = player.a + player.fov / 2.0 - player.fov * i / SCREEN_WIDTH;
			Impact impact = cast_ray(a);
			float d = impact.d;
			Color c = impact.c * get_brightness(d);

			// int x = SCREEN_WIDTH * 2 - i;
			int x = SCREEN_WIDTH - i;
			float h = static_cast<float>(SCREEN_HEIGHT) / static_cast<float>(d * cos(a - player.a)) * static_cast<float>(scale);
			draw_stake(x, h, c);
		}
	}

	void render() {		
		draw_player_view();
		draw_minimap(0, 0, 200, 200);
	}

private:
	int scale;
	SDL_Renderer *renderer;
	std::vector<std::string> map;
};
