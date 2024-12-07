#pragma once

#include "GLStuff.h"
#include "Camera.h"
#include "SceneObjects.h"
// BIG todo
class Player {

public:

	// I know these shouldnt be public, just rlly rlly lazy
	// Position of camera, and therefore the player
	Camera& cam;
	// What does the player look like?
	Shape& sprite;
	// List of objects the player will have its sprite collision checked against, kind of like we are a projectile.
	ObjectContainer& colObjects;

	// y
	float jumpHeight;
	// x
	float jumpDist;
	// Stores the original gravity from the sprite
	float originalGrav;
	// Offset of y from where we started jumping
	float jumpCounter;
	// Where should we end up at the end of the jump?
	float jumpBase;

	// Are we currently in the air after a jump?
	bool isJumping;
	// When we reach the peak of the arc, need to start descending.
	bool isDescending;
	// Did we request a jump? Used to store the jump state between calls.
	bool jumpRequested;
	// Last non-colliding position of camera
	glm::vec3 oldCamPos;
	// Last non-colliding position of sprite
	glm::vec3 oldSpritePos;


	Player(Camera& cam, Shape& s, ObjectContainer& oc);

	// Basically just a wrapper for camera movement, except allows us to set our sprite position accordingly
	void move(CameraDirection dir);

	void setPlayerSprite();

	void setPos();

	// Returns true for processed successfully, false for no idea what this is innit.
	bool processInput(uint32_t key);

	// "Jumps" in the direction the camera is facing
	void jump();
	// Handles the jump step by step
	void handleJumping();

	// Uses our colObjects to check agaisnt a list of objects if our player sprite is
	// colliding, if it is.... Do something.
	// Return true on collision and false if not, for if we wanna track that elsewhere.
	bool handleSpriteCollision(overlap_mask *om_out=NULL);
	void handleCollisionState(overlap_mask& om);
};