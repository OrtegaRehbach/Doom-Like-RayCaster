#pragma once

#include <cmath>

struct Player {
	int x;
	int y;
	float a;
	float fov;
	float movementSpeed;
	float turningSpeed;

	void moveForward() {
		x += movementSpeed * cos(a);
		y += movementSpeed * sin(a);
	}

	void moveBackward() {
		x -= movementSpeed * cos(a);
		y -= movementSpeed * sin(a);
	}

    void moveLeft() {
        float turnAngle = a + M_PI / 2;
		x += movementSpeed * cos(turnAngle);
		y += movementSpeed * sin(turnAngle);
	}

    void moveRight() {
        float turnAngle = a - M_PI / 2;
		x += movementSpeed * cos(turnAngle);
		y += movementSpeed * sin(turnAngle);
	}

	void turnLeft() {
		a += M_PI / turningSpeed;
	}

	void turnRight() {
		a -= M_PI / turningSpeed;
	}
};