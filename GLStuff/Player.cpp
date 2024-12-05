#include "Player.h"

void Player::move(CameraDirection dir) {
	cam.move(dir);
	setPlayerSprite();
}

Player::Player(Camera& cam, Shape& s, ObjectContainer& oc) : cam(cam), sprite(s), colObjects(oc) {
	sprite.position = cam.pos;// + glm::vec3(3.0f, 0.0f, -8.0f);
	this->jumpHeight = 3.0f;
	this->originalGrav = sprite.gravPower;
	this->isJumping = false;
	this->jumpDist = 2.0f;
	this->isDescending = false;
	this->jumpBase = 0.0f;
	this->jumpCounter = 0.0f;
	// Make sure vert positions are set up.
	this->sprite.getVerts().findVertPositions();
}

void Player::setPlayerSprite() {
	// TODO: Offset this back to 0 when ur dun
	//sprite.position = cam.pos + glm::vec3(3.0f, 0.0f, -8.0f);
	sprite.position = cam.pos;// + glm::vec3(3.0f, -1.0f, 0.0f);
	sprite.draw();
}

// Returns true for processed successfully, false for no idea what this is innit.
bool Player::processInput(uint32_t key) {
	// Also need the camera mouse processing stuff in here aswell.
	bool ret = true;
	overlap_mask om;

	if (cam.processKeyInput(key)) {
		// We just wanna skip the switch at the end.
		key = 0;
	}

	// Keys not used by the camera can be processed by us instead
	switch (key) {
	case 0:
		// See cam.processKeyInput above
		break;
	// TODO, player stuff, whatever we want. Might not even use a switch statement. 
	// If we do handle any keys here doe, need to return true.
	case GLFW_KEY_SPACE:
		this->jump();
		break;
	default:
		ret = false;
		break;
	}

	// Do collision stuff for our sprite.
	handleSpriteCollision(&om);
	handleCollisionState(om);

	// Handle jumping, because this aint just a one time thing
	if (this->isJumping) {
		this->handleJumping();
	}

	setPlayerSprite();
	return ret;
}

// "Jumps" in the direction the camera is facing
void Player::jump() {
	std::cout << "(Player::jump) Jumping innit" << std::endl;
	originalGrav = sprite.gravPower;
	//sprite.gravPower = -sprite.gravPower;

	// Parabol stuff
	/*float x = sprite.position.x;
	float start = x;
	float end = x + this->jumpDist;

	float y = Util::parabol_y(x, start, this->jumpDist, this->jumpHeight);*/
	
	this->jumpBase = this->cam.pos.y;
	this->jumpCounter = 0.0f;
	this->isJumping = true;
}

void Player::handleJumping() {
	if (!this->isJumping) return;
	std::cout << "(Player::handleJumping) handling it innit" << std::endl;
	float x = cam.pos.x;
	float start = x;
	float end = x + this->jumpDist;
	float toAdd = sprite.gravPower;
	//float y = Util::parabol_y(this->jumpCounter, start, this->jumpDist, this->jumpHeight);

	if (this->isDescending)
		toAdd = -toAdd;

	this->jumpCounter += toAdd;
	this->cam.pos.y += toAdd;

	if (this->jumpCounter >= this->jumpHeight) {
		this->isDescending = true;
	}

	std::cout << "(Player::handleJumping) y: " << this->cam.pos.y << std::endl;

	// If we are back on the ground stop fucking with this.
	// TODO: May need to redefine what "ground" means as if we go below we may wanna change that
	// Dun, now have the concept of "jumpBase".
	if (this->jumpCounter <= 0.0f) {
		this->cam.pos.y = this->jumpBase;
		this->isJumping = false;
		this->isDescending = false;
	}
}

bool Player::handleSpriteCollision(overlap_mask *om_out) {

	// Will invoke the sprite collision func.
	// Auto since unsure when/if this will be changed

	// Will also store details of how we collided.
	overlap_mask om=0;
	this->sprite.findBB();
	auto col = this->colObjects.checkColliding(this->sprite, false, &om);
	if (om_out != NULL) *om_out = om;
	if (col) return true;

	return false;
}

void Player::handleCollisionState(overlap_mask& om) {
	bool x = om & OverlapMask::X;
	bool y = om & OverlapMask::Y;
	bool z = om & OverlapMask::Z;

	bool aboveGround = cam.pos.y > 0.0f;

	if (om == 0) return;
	// TODO: Finish
	if (y && aboveGround && !this->isJumping) {
		std::cout << "Player::handleCollisionState y: " << cam.pos.y << std::endl;
		this->isDescending = false;
		this->isJumping = false;
	}
	else if (y && aboveGround) {
		this->isJumping = true;
		this->isDescending = true;
	}
	//
}