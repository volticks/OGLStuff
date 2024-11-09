#include "Player.h"

void Player::move(CameraDirection dir) {
	cam.move(dir);
	setPlayerCam();
}

Player::Player(Camera& cam, Shape& s) : cam(cam), sprite(s) {
	s.position = cam.pos;
}

void Player::setPlayerCam() {
	sprite.position = cam.pos;
}

// Returns true for processed successfully, false for no idea what this is innit.
bool Player::processInput(uint32_t key) {
	if (cam.processKeyInput(key)) {
		// Chances are if camera did find this our position will have changed, so reset it
		setPlayerCam();
		return true;
	}

	// Keys not used by the camera can be processed by us instead
	switch (key) {
		// TODO, player stuff, whatever we want. Might not even use a switch statement. 
		// If we do handle any keys here doe, need to return true.
	}

	return false;



}

// "Jumps" in the direction the camera is facing
void Player::jump(float vel, float amount) {
	// How tf do i make this smooth lol
	// Ig i should prolly implement gravity before anything else huh
}