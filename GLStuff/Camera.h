#pragma once
#include "GLStuff.h"
#include "Shader.h"
#include "Shapes.h"

enum CameraDirection {
	UP,
	DOWN,
	L,
	R,
	BACK,
	FORWARD
};

class Camera : Controllable<> {

public:

	// The current view matrix
	glm::mat4 currentView;
	const float aspect = (float)WINDOW_W / (float)WINDOW_H;
	// Position of camera
	glm::vec3 pos;
	// Vec pos we are looking at
	glm::vec3 targ;
	// Points "up", needed for lookAt matrix
	glm::vec3 up;

	float velocity;
	float currVelocity;

	float lastFrame = 0.0f;

	// For rotation
	float yaw, pitch, roll;
	glm::vec3 direction;

	float fov = 45.0f;
	glm::mat4 projMat;

	float step = 50.0f;

	bool canFly;

	Camera(bool fly) {
		canFly = fly;
		pos = glm::vec3(0.0f,0.0f,0.0f);
		targ = glm::vec3(0.0f,0.0f,-1.0f);
		// Will probably always be the same, unless we wanna flip upside down.
		up = glm::vec3(0.0f,0.1f,0.0f);

		// So we are pointing toward the negative Z-axis (as is the convention).
		yaw = -90.0f;
		pitch = 0.0f;
		roll = 0.0f;
		direction = glm::vec3(0.0f);

		velocity = 2.0f;
		currVelocity = velocity;

		projMat = glm::perspective(fov, aspect, 0.1f, 1000.0f);
		initKeyMap();
	}

	float getVelocity() {
		float currentFrame = glfwGetTime();
		float delta = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Slower hardware will move faster, but also be able to call processKeyInput LESS, so it balances it out
		// at least, in theory.
		currVelocity = velocity * delta;
		return currVelocity;
	}

	// Pass in shader rather than store so we can re-use the same camera class.
	bool setCam(Shader &s, const char *viewMatName = "viewMat", const char *projMatName = "projMat") {
		glm::mat4 view = Util::myLookAt(pos, pos + targ, up);
		currentView = view;
		return s.setUniformVar(projMatName, &projMat) && s.setUniformVar(viewMatName, &view);
	}

	void move(CameraDirection dir) {
		switch (dir) {
		case CameraDirection::UP:
			// Positive Y
			pos += glm::vec3(0.0f, velocity, 0.0f);
			break;
		case CameraDirection::DOWN:
			// Negative Y
			pos += glm::vec3(0.0f, -velocity, 0.0f);
			break;
		case CameraDirection::L:
			// Positive X
			pos += glm::vec3(velocity, 0.0f, 0.0f);
			break;
		case CameraDirection::R:
			// Negative X
			pos += glm::vec3(-velocity, 0.0f, 0.0f);
			break;
		case CameraDirection::BACK:
			pos += glm::vec3(0.0f, 0.0f, velocity);
			break;
		case CameraDirection::FORWARD:
			pos += glm::vec3(0.0f, 0.0f, -velocity);
			break;
		default:
			std::cerr << "(Camera) Move dir not supported" << std::endl;
			return;
		}

	}

	bool moveOnceAndSet(CameraDirection dir, Shader &s, const char* viewMatName = "viewMat", const char* projMatName = "projMat") {
		move(dir);
		return setCam(s, viewMatName, projMatName);
	}

	void initKeyMap() {
		// Do non-specific key map init
		Controllable::initKeyMap();
		// Now, quad specific stuff.

		// Movement
		auto moveFunc = [this](unsigned long dir) {

			std::cout << "(Camera) Moving: " << dir << std::endl;
			move((CameraDirection)dir);


			};

		// Use bind to bind a parameter so we dont have to re-write moveFunc 4 times slightly differently.
		this->getKeyMap().insert({ GLFW_KEY_UP, std::bind(moveFunc, CameraDirection::UP) });
		this->getKeyMap().insert({ GLFW_KEY_DOWN, std::bind(moveFunc, CameraDirection::DOWN) });
		this->getKeyMap().insert({ GLFW_KEY_LEFT, std::bind(moveFunc, CameraDirection::L) });
		this->getKeyMap().insert({ GLFW_KEY_RIGHT, std::bind(moveFunc, CameraDirection::R) });
		this->getKeyMap().insert({ GLFW_KEY_BACKSPACE, std::bind(moveFunc, CameraDirection::BACK) });
		this->getKeyMap().insert({ GLFW_KEY_ENTER, std::bind(moveFunc, CameraDirection::FORWARD) });

		auto stepFunc = [this](unsigned long mult) {

			long m = (long)mult;

			std::cout << "(Camera) stepping: " << this->step * m << std::endl;
			walk(this->step * m);
			};

		// Back and forwards (should also use A and D eventually too)
		this->getKeyMap().insert({ GLFW_KEY_W, std::bind(stepFunc, 1) });
		this->getKeyMap().insert({ GLFW_KEY_S, std::bind(stepFunc, -1) });
	}

	bool processKeyInput(uint32_t key) {

		// TODO: this doesnt actually use the velocity yet, just stores it lol
		float currvelocity = getVelocity();

		// For now, just delegate to the key map
		auto res = this->getKeyMap().find(key);
		if (res == this->getKeyMap().end()) return false;
		std::cout << "(Camera) Found key" << std::endl;
		res->second(1337);
		return true;

	}

	void rotate(float threshX, float threshY) {

		float vel = getVelocity();

		//const float mult = 12.0f;


		threshX *= vel;
		threshY *= vel;

		// Rotate by `velocity` in direction specified
		// Make sure we add rather than subtract lol
		std::cout << "(Camera) yaw thresh: " << threshX << std::endl;
		yaw += threshX;
		std::cout << "(Camera) pitch thresh: " << threshY << std::endl;
		pitch += threshY;

		// Now we can do the trig n stuff
		// restrictions on pitch so we dont rotate more than 90 and wrap around either way.
		//float pitchR = std::min(pitch, 89.0f);
		//pitchR = std::max(pitchR, -89.0f);
		
		// Lock it so we dont go too far up or down.
		if (pitch > 89.0f)
			pitch = 89.0f;
		else if (pitch < -89.0f)
			pitch = -89.0f;

		float pitchR = glm::radians(pitch);
		
		float yawR = glm::radians(yaw);

		// All values use cos and sin, this wraps us into a certain range which we wont exceed, and if we come close we will simply wrap back around,
		// like a circle, which is good considering that what our camera is, just a sphere we are orienting around.

		// X direction is a combination of where we are yaw-ing and our pitch, as both of these will change the direction
		direction.x = cos(yawR) * cos(pitchR);
		// Y isnt rlly based off anything else, so we just use sin to cap it to circle range
		direction.y = sin(pitchR);
		// Z is same as X, both yaw and pitch will change where we are facing on this axis.
		direction.z = sin(yawR) * cos(pitchR);

		// Make sure length is 1
		targ = glm::normalize(direction);
		//setCam()
	}

	void zoom(float thresh) {
		// TODO add zoom checking here
		//float currFov = fov + (thresh/3);
		fov -= thresh;

		// These are stupid and arbitrary so idc rn
		fov = std::max(fov, 1.0f);
		fov = std::min(fov, 45.0f);

		std::cout << "(Camera) zoom: " << fov << std::endl;

		projMat = glm::perspective(fov, aspect, 0.1f, 1000.0f);
	}

	// By only changing the z component we should just be able to move backwards and forwards whichever direction the camera 
	// is facing (fingers crossed)
	void walk(float step) {
		float oldY;
		step *= getVelocity();
		std::cout << "(Camera) step: " << step << " Vel: " << velocity << std::endl;
		this->pos += (this->targ * step);

		// If not fly, we shoudnt be able to walk up and down the y axis at will.
		if (!canFly) this->pos.y -= (this->targ.y * step);
		

	}


};
