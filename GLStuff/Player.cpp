#include "Player.h"

void Player::move(CameraDirection dir) {
	cam.move(dir);
	setPlayerSprite();
}

Player::Player(Camera& cam, Shape& s, ObjectContainer& oc) : cam(cam), sprite(s), colObjects(oc) {
	cam.pos.x -= 5.0f;
	sprite.position = cam.pos;// + glm::vec3(3.0f, 0.0f, -8.0f);
	// Just to make sure
	sprite.gravEnabled = true;

	this->jumpHeight = 3.0f;
	this->originalGrav = sprite.gravPower;
	this->isJumping = false;
	this->jumpDist = 2.0f;
	this->isDescending = false;
	this->jumpBase = 0.0f;
	this->jumpCounter = 0.0f;
	this->jumpRequested = false;
	this->oldSpritePos = glm::vec3(0.0f);
	// Make sure vert positions are set up.
	this->sprite.getVerts().findVertPositions();
	// Make sure we only store the size AFTER we compute the size in findVertPositions lol.
	this->oldSpriteSz = sprite.getVerts().getSize();

	// TODO: translate sprite model matrix so we are above our player.
	sprite.modelMat = glm::translate(sprite.modelMat, glm::vec3(0.0f, 2.0f, 0.0f));

	this->shrink(0.25f, 1.0f, 0.25f);
}

void Player::setPlayerSprite() {
	// TODO: Offset this back to 0 when ur dun
	//sprite.position = cam.pos + glm::vec3(3.0f, 0.0f, -8.0f);
	this->sprite.position = this->cam.pos;
	this->sprite.draw();
}

// Need the camera position to follow the sprite position as that will be used for collision checking.
void Player::setPos() {
	this->sprite.position = oldSpritePos;
	this->cam.pos = oldSpritePos;
}

// Returns true for processed successfully, false for no idea what this is innit.
bool Player::processInput(uint32_t key) {
	// Also need the camera mouse processing stuff in here aswell.
	bool ret = true;
	overlap_mask om;

	if (cam.processKeyInput(key)) {
		// If we moved, we should update the sprite position to mirror that.
		this->sprite.position = this->cam.pos;
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

	// Do collision stuff for our sprite. Also record our last valid non-colliding position
	if (!handleSpriteCollision(&om)) {
		//this->oldCamPos = this->cam.pos;
		this->oldSpritePos = this->sprite.position;
	}
	handleCollisionState(om);

	// Handle jumping (if we are jumping), because this aint just a one time thing
	this->handleJumping();

	setPlayerSprite();
	return ret;
}

// "Jumps" in the direction the camera is facing
void Player::jump() {
	std::cout << "(Player::jump) Jumping innit" << std::endl;
	//originalGrav = sprite.gravPower;
	//sprite.gravPower = -sprite.gravPower;

	// Parabol stuff
	/*float x = sprite.position.x;
	float start = x;
	float end = x + this->jumpDist;

	float y = Util::parabol_y(x, start, this->jumpDist, this->jumpHeight);*/
	
	// Jump base should *always* be the ground, unless we have some strange situation which we can adapt to later.
	this->jumpBase = 0.0f;
	this->jumpCounter = 0.0f;
	this->isJumping = true;
	// Starting a new jump, dont descend yet
	this->isDescending = false;
	// Indicate we have requested a jump
	this->jumpRequested = true;
	// Also reset gravity when we try to jump so we have the original jumping POWA.
	this->sprite.gravPower = this->originalGrav;
}

void Player::handleJumping() {
	if (!this->isJumping) return;
	std::cout << "(Player::handleJumping) handling it innit" << std::endl;
	float toAdd = sprite.gravPower;

	if (this->isDescending)
		toAdd = -toAdd;

	this->jumpCounter += toAdd;
	this->cam.pos.y += toAdd;

	if (this->jumpCounter >= this->jumpHeight) {
		this->isDescending = true;
	}

	std::cout << "(Player::handleJumping) y: " << this->cam.pos.y << std::endl;

	// If we are back on the ground stop fucking with this.
	if (this->jumpCounter >= 0.0f) return;
	// Flatten us out to where we should be - IF y is too small, since we shouldnt be below 0.
	if (this->cam.pos.y <= 0.0f) this->cam.pos.y = 0.0f;
	this->isJumping = false;
	this->isDescending = false;
	// Reset the grav power back to what it should be too
	this->sprite.gravPower = this->originalGrav;
}

bool Player::handleSpriteCollision(overlap_mask *om_out) {

	// Will invoke the sprite collision func.
	// Will also store details of how we collided.
	overlap_mask om=0;
	this->sprite.findBB();
	// Auto since unsure when/if this will be changed
	this->colObjects.checkColliding(this->sprite, false, &om);
	if (om_out != NULL) *om_out = om;
	//if (col) return true;

	return om == OverlapMask::allColliding;
}

void Player::handleCollisionState(overlap_mask& om) {
	bool all = om == OverlapMask::allColliding;

	bool aboveGround = cam.pos.y > 0.0f;

	// No collision whatsoever <---- prolly redundant since even if theres no collision we still need to handle jumping
	//if (om == 0)
	//	return;

	std::cout << "Player::handleCollisionState om: " << om << std::endl;

	//// Falling and jumping logic: TODO: What about banging our heads on the ceiling??? <-- Handled automatically it seems
	// We are above ground and colliding, meaning we have fallen onto something AND are not trying
	// to jump off it.
	if (aboveGround && all && !this->jumpRequested) {
		std::cout << "Player::handleCollisionState y: " << cam.pos.y << std::endl;

		// If we fall onto an object, nerf our gravity so we arent bouncing on top like a creature
		this->sprite.gravPower = this->originalGrav / 8.0f;

		this->isDescending = false;
		this->isJumping = false;
	}
	// Not colliding, but above the ground
	else if (aboveGround && !this->isJumping) {
		std::cout << "Player::handleCollisionState falling y: " << cam.pos.y << std::endl;

		// Slowly return our gravity to its original value (from what it was changed to above) as we fall
		float gravInc = (this->originalGrav / 8.0f);
		this->sprite.gravPower = std::fmin((this->sprite.gravPower + gravInc), this->originalGrav);

		this->isDescending = true;
		this->isJumping = true;
	}
	// Our job here is done; we have allowed our ascent.
	this->jumpRequested = false;

	//// Now we need to update the position to what it was pre collision, if we did collide.
	if (!all) return;
	/*this->jumpRequested = true;
	this->isJumping = false;
	this->isDescending = true;*/
	setPos();
}

// Shrinks our sprite size down a bit by the provided magnitutes
void Player::shrink(float x, float y, float z) {
	glm::vec3 spriteSz = this->sprite.getVerts().getSize();
	this->oldSpriteSz = spriteSz;
	spriteSz *= glm::vec3(x, y, z);
	this->sprite.getVerts().setSize(spriteSz);
}

void Player::unshrink() {
	this->sprite.getVerts().setSize(this->oldSpriteSz);
}