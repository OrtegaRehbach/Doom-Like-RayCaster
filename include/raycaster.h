#pragma once

#include <iostream>
#include <fstream>
#include <SDL_render.h>
#include <cmath>
#include <SDL2/SDL.h>

#include "player.h"
#include "imageloader.h"

const Color B(0, 0, 0);
const Color W;

const int SCREEN_WIDTH = screenDim.width;
const int SCREEN_HEIGHT = screenDim.height;
const int MAX_RAY_DISTANCE = 10000;

int BLOCK_AMT_X = 20;
int BLOCK_AMT_Y = 20;
int BLOCK = SCREEN_WIDTH / BLOCK_AMT_X;

const int WPN_POS_X = screenDim.centerX;
const int WPN_POS_Y = screenDim.height - 116;
int currentWpnPosX = WPN_POS_X;
int currentWpnPosY = WPN_POS_Y;
int wpnAnimStartTick = 0;

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

		scale = BLOCK;

		texSize = 128;
	}

	void load_map(const std::string &filename) {
		map.clear();
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
			player.x = BLOCK + BLOCK / 2.0f;
			player.y = BLOCK + BLOCK / 2.0f;
			player.a = M_PI_4;
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
	void rect(int x, int y, const std::string& mapHit, int width = BLOCK, int height = BLOCK) {
		Color c = ImageLoader::getPixelColor(mapHit, x, y);
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		SDL_Rect rect = {x, y, width, height};
		SDL_RenderFillRect(renderer, &rect);
	}

	void rect(int x, int y, int blockSize = BLOCK, Color c = W) {
		rect(x, y, blockSize, blockSize, c);
	}

	void drawTextureMinimap(int x, int y, const std::string& mapHit, int width = BLOCK, int height = BLOCK) {
		for (int rx = x; rx < x + width; rx++) {
			for (int ry = y; ry < y + height; ry++) {
				int tx = (rx - x) * (texSize / width);
				int ty = (ry - y) * (texSize / height);

				Color c = ImageLoader::getPixelColor(mapHit, tx, ty);
				point(rx, ry, c);
			}
		}
	}


	Impact cast_ray(float a, bool drawRay = false, const Color& c = W, int drawDistance = MAX_RAY_DISTANCE, int blockSize = BLOCK) {
		float d = 0;
		int hitX;
		int hitY;
		int tx;
		std::string mapHit;
		while (d <= MAX_RAY_DISTANCE) {
			int x = static_cast<int>(player.x + d * cos(a));
			int y = static_cast<int>(player.y + d * sin(a));

			int i = static_cast<int>(x / blockSize);
			int j = static_cast<int>(y / blockSize);

			if (map[j][i] != ' ') {
				mapHit = map[j][i];
				hitX = x - i * blockSize;
				hitY = y - j * blockSize;
				int maxHit = (hitX == 0 || hitX == blockSize - 1) ? hitY : hitX;
				tx = maxHit * (texSize / blockSize);
				break;
			}

			if (drawRay && d <= drawDistance)
				point(x, y, c);

			d += 1;
		}
		return Impact{ d, hitX, hitY, mapHit, tx };
	}

	Impact cast_ray_from_point(
		int x, int y, float a, bool drawRay = false, const Color& c = W, int drawDistance = MAX_RAY_DISTANCE, 
		int blockSizeX = BLOCK, int blockSizeY = BLOCK
	) {
		float d = 0;
		int _x;
		int _y;
		std::string mapHit;
		while (d <= MAX_RAY_DISTANCE) {
			_x = static_cast<int>(x + d * cos(a));
			_y = static_cast<int>(y + d * sin(a));

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
		return Impact{ d, _x, _y, mapHit, 0 };
	}

	void draw_stake(int x, float h, Color c) {
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		float start = SCREEN_HEIGHT / 2.0f - h / 2.0f;
		SDL_Rect rect = {x, static_cast<int>(start), 1, static_cast<int>(h)};
		SDL_RenderFillRect(renderer, &rect);
	}

	void draw_textured_stake(int x, float h, Impact i) {
		int start = std::round((SCREEN_HEIGHT / 2.0f) - (h / 2.0f));
		int end = std::round(start + h);
		for (int y = start; y < end; y++) {
			if (y < 0 || y >= SCREEN_HEIGHT) continue;
			int ty = (y - start) * (texSize / h);
			Color c = ImageLoader::getPixelColor(i.mapHit, i.tx, ty) * get_brightness_smooth(i.d);
			point(x, y, c);
		}
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

	float get_brightness_smooth(float d) {
		float minViewDistance = 16 * BLOCK; //350.0f;
		return std::min(1.0f, std::exp(0.005f * (minViewDistance - d)));
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
				if (i < 0 || j < 0) continue;
				if (map[j][i] != ' ') {
					std::string mapHit;
					mapHit = map[j][i];
					Color c = colorMap[mapHit];
					drawTextureMinimap(x, y, mapHit, minimapBlockSizeX, minimapBlockSizeY);
				}
			}
		}
		float mapScaleFactor = (float)mapWidth / (float)SCREEN_WIDTH;
		int playerPosX = xPos + mapScaleFactor * player.x;
		int playerPosY = yPos + mapScaleFactor * player.y;
		for (int i = 0; i < mapWidth; i++) {
			float a = player.a + player.fov / 2 - player.fov * i / mapWidth;
			if (i == 0 || i == mapWidth - 1)
				cast_ray_from_point(playerPosX, playerPosY, a, true, W, MAX_RAY_DISTANCE, minimapBlockSizeX, minimapBlockSizeY);
		}
		cast_ray_from_point(playerPosX, playerPosY, player.a, true, Color(255, 0, 0), MAX_RAY_DISTANCE, minimapBlockSizeX, minimapBlockSizeY);
	}

	void draw_weapon_view() {
		int xOffset = 0;
		int yOffset = 0;
		if (player.isMoving()) {
			wpnAnimStartTick = (wpnAnimStartTick == 0) ? gameTicks : wpnAnimStartTick;
			xOffset = 50 * sin((((gameTicks - wpnAnimStartTick) * 20) % 360) * M_PI / 180);
			yOffset = 25 * 0.5f * sin((((2 * (gameTicks - wpnAnimStartTick) * 20) % 360) * M_PI / 180) + M_PI_2) + 0.5f;
			currentWpnPosX = WPN_POS_X + xOffset;
			currentWpnPosY = WPN_POS_Y + yOffset;
		} else {
			int speed = 10;
			if (currentWpnPosX > WPN_POS_X) {
				currentWpnPosX -= speed;
				currentWpnPosX = (currentWpnPosX < WPN_POS_X) ? WPN_POS_X : currentWpnPosX;
			} else if (currentWpnPosX < WPN_POS_X) {
				currentWpnPosX += speed;
				currentWpnPosX = (currentWpnPosX > WPN_POS_X) ? WPN_POS_X : currentWpnPosX;
			}
			if (currentWpnPosY > WPN_POS_Y) {
				currentWpnPosY -= speed;
				currentWpnPosY = (currentWpnPosY < WPN_POS_Y) ? WPN_POS_Y : currentWpnPosY;
			} else if (currentWpnPosY < WPN_POS_Y) {
				currentWpnPosY += speed;
				currentWpnPosY = (currentWpnPosY > WPN_POS_Y) ? WPN_POS_Y : currentWpnPosY;
			}
			if (currentWpnPosX == WPN_POS_X && currentWpnPosY == WPN_POS_Y)
				wpnAnimStartTick = 0;
		}
		ImageLoader::renderCentered(renderer, "../assets/sprites/gun.png", currentWpnPosX, currentWpnPosY);
	}

	void draw_player_view() {
		for (int i = 0; i < SCREEN_WIDTH; i++) {
			double rayAngle = (player.a + (player.fov / 2.0)) - (player.fov * (double)i / (double)SCREEN_WIDTH);
			Impact impact = cast_ray(rayAngle);
			float d = impact.d;
			float p = d * cos(rayAngle) * cos(player.a) + d * sin(rayAngle) * sin(player.a);
			int h = SCREEN_HEIGHT * scale / p;
			int x = SCREEN_WIDTH - i;
			// int h = (SCREEN_HEIGHT / (d * cos(rayAngle - player.a))) * scale;
			draw_textured_stake(x, h, impact);
			draw_floor(impact, x, h);
			draw_ceiling(impact, x, h);
		}
		draw_weapon_view();
	}

	void draw_floor(const Impact& impact, int x, int h) {
		int startY = screenDim.centerY + h / 2;
		for (int y = 0; y < SCREEN_HEIGHT - startY; y++) {
			Color c = Color(50, 40, 30);
			point(x, SCREEN_HEIGHT - y, c);
		}
	}

	void draw_ceiling(const Impact& impact, int x, int h) {
		int startY = 0;
		for (int y = 0; y < screenDim.centerY - h / 2; y++) {
			Color c = C_GRAY * 0.4f;
			point(x, y, c);
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
	int texSize;
};
