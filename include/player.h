#pragma once

#include <cmath>

#include "globals.h"

struct Player {
	float x;
	float y;
	double a;
	float fov;
	float forwardX;
	float forwardY;
	float rightX;
	float rightY;
	float movementSpeed;
	float turningSpeed;
	int blockSize;

	void updateVectors() {
		forwardX = cos(a);
		forwardY = sin(a);
		rightX = forwardY;
		rightY = -forwardX;
	}

	void move(float _x, float _y) {
		x = _x;
		y = _y;
		updateVectors();
	}

	bool canMove(float newX, float newY) {
		int i = static_cast<int>(newX / blockSize);
		int j = static_cast<int>(newY / blockSize);
		if (loadedMap[j][i] != ' ')
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
		double newAngle = a - (M_PI / turningSpeed) * deltaTime;
		a = normalizeAngle(newAngle);
		updateVectors();
	}

	void turnLeftMouse() {
		double newAngle = a - (M_PI / turningSpeed) * (1 / mouseSensitivity) * deltaTime;
		a = normalizeAngle(newAngle);
		updateVectors();
	}

	void turnRight() {
		double newAngle = a + (M_PI / turningSpeed) * deltaTime;
		a = normalizeAngle(newAngle);
		updateVectors();
	}

	void turnRightMouse() {
		double newAngle = a + (M_PI / turningSpeed) * (1 / mouseSensitivity) * deltaTime;
		a = normalizeAngle(newAngle);
		updateVectors();
	}

	bool isMoving() {
		return(	
			KeyboardState[SDL_SCANCODE_W] || 
			KeyboardState[SDL_SCANCODE_A] || 
			KeyboardState[SDL_SCANCODE_D] || 
			KeyboardState[SDL_SCANCODE_S]
		);
	}
};