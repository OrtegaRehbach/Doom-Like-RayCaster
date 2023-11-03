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

		player.movementSpeed = 6;
		player.turningSpeed = 48.0f;

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
	}

	void point(int x, int y, Color c) {
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		SDL_RenderDrawPoint(renderer, x, y);
	}

	void rect(int x, int y, Color c) {
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
		SDL_Rect rect = {x, y, BLOCK, BLOCK};
		SDL_RenderFillRect(renderer, &rect);
	}

	Impact cast_ray(float a) {
		float d = 0;
		std::string mapHit;
		while (true) {
			int x = static_cast<int>(player.x + d * cos(a));
			int y = static_cast<int>(player.y + d * sin(a));

			int i = static_cast<int>(x / BLOCK);
			int j = static_cast<int>(y / BLOCK);

			if (map[j][i] != ' ') {
				mapHit = map[j][i];
				break;
			}

			point(x, y, W);

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
		float minViewDistance = 250.0f;
		float maxViewDistance = 650.0f;
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

	void render() {
		// draw left side of the screen

		for (int x = 0; x < SCREEN_WIDTH; x += BLOCK) {
			for (int y = 0; y < SCREEN_HEIGHT; y += BLOCK) {
				int i = static_cast<int>(x / BLOCK);
				int j = static_cast<int>(y / BLOCK);
				if (map[j][i] != ' ') {
					std::string mapHit;
					mapHit = map[j][i];
					Color c = colors[mapHit];
					rect(x, y, c);
				}
			}
		}
		for (int i = 1; i < SCREEN_WIDTH; i++) {
			float a = player.a + player.fov / 2 - player.fov * i / SCREEN_WIDTH;
			cast_ray(a);
		}

		// draw right side of the screen

		for (int i = 1; i < SCREEN_WIDTH; i++) {
			double a = player.a + player.fov / 2.0 - player.fov * i / SCREEN_WIDTH;
			Impact impact = cast_ray(a);
			float d = impact.d;
			Color c = impact.c;
			c = c * get_brightness(d);

			if (d == 0) {
				std::cout << "you lose" << std::endl;
				exit(1);
			}
			int x = SCREEN_WIDTH * 2 - i;
			float h = static_cast<float>(SCREEN_HEIGHT) / static_cast<float>(d) * static_cast<float>(scale);
			draw_stake(x, h, c);
		}
	}

private:
	int scale;
	SDL_Renderer *renderer;
	std::vector<std::string> map;
};
