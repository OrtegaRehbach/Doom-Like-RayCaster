#pragma once

#include <cmath>

struct Player {
	int x;
	int y;
	float a;
	float fov;
	float movementSpeed;
	float turningSpeed;
	int blockSize;
	std::vector<std::string> map;

	bool canMove(int newX, int newY) {
		int i = static_cast<int>(newX / blockSize);
		int j = static_cast<int>(newY / blockSize);
		if (map[j][i] != ' ')
			return false;
		return true;
	}

	void moveForward() {
		int newX = x + movementSpeed * cos(a);
		int newY = y + movementSpeed * sin(a);
		if (canMove(newX, newY)) {
			x = newX;
			y = newY;
		}
	}

	void moveBackward() {
		int newX = x - movementSpeed * cos(a);
		int newY = y - movementSpeed * sin(a);
		if (canMove(newX, newY)) {
			x = newX;
			y = newY;
		}
	}

    void moveLeft() {
        float turnAngle = a - M_PI / 2;
		int newX = x + movementSpeed * cos(turnAngle);
		int newY = y + movementSpeed * sin(turnAngle);
		if (canMove(newX, newY)) {
			x = newX;
			y = newY;
		}
	}

    void moveRight() {
        float turnAngle = a + M_PI / 2;
		int newX = x + movementSpeed * cos(turnAngle);
		int newY = y + movementSpeed * sin(turnAngle);
		if (canMove(newX, newY)) {
			x = newX;
			y = newY;
		}
	}

	void turnLeft() {
		a -= M_PI / turningSpeed;
	}

	void turnRight() {
		a += M_PI / turningSpeed;
	}
};