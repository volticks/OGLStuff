#pragma once

#include "GLStuff.h"
#include "Camera.h"
// BIG todo
class Player {

public:

	// I know these shouldnt be public, just rlly rlly lazy
	// Position of camera, and therefore the player
	Camera& cam;
	// What does the player look like?
	Shape& sprite;

	// Basically just a wrapper for camera movement, except allows us to set our sprite position accordingly
	void move(CameraDirection dir);

	Player(Camera& cam, Shape& s);

	void setPlayerCam();

	// Returns true for processed successfully, false for no idea what this is innit.
	bool processInput(uint32_t key);

	// "Jumps" in the direction the camera is facing
	void jump(float vel, float amount);

};
