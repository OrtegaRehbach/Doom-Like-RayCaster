#pragma once

#include <cmath>

#include "globals.h"

struct Player {
	int x;
	int y;
	double a;
	float fov;
	float movementSpeed;
	float turningSpeed;
	int blockSize;
	std::vector<std::string> map;

	void move(int _x, int _y) {
		x = _x;
		y = _y;
	}

	bool canMove(int newX, int newY) {
		int i = static_cast<int>(newX / blockSize);
		int j = static_cast<int>(newY / blockSize);
		if (map[j][i] != ' ')
			return false;
		return true;
	}

	void moveForward() {
		float newX = x + movementSpeed * cos(a) * deltaTime;
		float newY = y + movementSpeed * sin(a) * deltaTime;
		if (canMove(newX, newY))
			move(newX, newY);
	}

	void moveBackward() {
		float newX = x - movementSpeed * cos(a) * deltaTime;
		float newY = y - movementSpeed * sin(a) * deltaTime;
		if (canMove(newX, newY))
			move(newX, newY);
	}

    void moveLeft() {
        double turnAngle = a - M_PI / 2;
		float newX = x + movementSpeed * cos(turnAngle) * deltaTime;
		float newY = y + movementSpeed * sin(turnAngle) * deltaTime;
		if (canMove(newX, newY))
			move(newX, newY);
	}

    void moveRight() {
        double turnAngle = a + M_PI / 2;
		float newX = x + movementSpeed * cos(turnAngle) * deltaTime;
		float newY = y + movementSpeed * sin(turnAngle) * deltaTime;
		if (canMove(newX, newY))
			move(newX, newY);
	}

	void turnLeft() {
		a -= (M_PI / turningSpeed) * deltaTime;
	}

	void turnRight() {
		a += (M_PI / turningSpeed) * deltaTime;
	}
};