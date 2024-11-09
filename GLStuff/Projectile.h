#pragma once
#include "GLStuff.h"
#include "Shader.h"
#include "Shapes.h"
#include "SceneObjects.h"

struct ProjectileData {
	glm::vec3 direction;
	glm::vec3 position;
	float velocity;
};

// Projectile: a shape which moves at a velocity in a specified direction.
// This works but is kinda broken 
class Projectile : public Shape{
public:
	std::shared_ptr<ProjectileData> pd;

	void drawOnce();

	void drawImpl();

	Projectile(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, bool collidable, std::vector<VertAttribute> vas, ShapeCollisionFuncT cFunc, bool isTextured, uint32_t texID, float vel, glm::vec3 direction, glm::vec3 startingPos);

	static void projectileColFunc(Shape& s1, Shape& s2) {

		Projectile& p = static_cast<Projectile&>(s1);

		std::vector<float> args{ 0.9f, 0.3f, 0.3f };
		// Try to set the uniform if it exists - if uniform isnt used to make any meaningful diff gl will optimize it
		// out so if ur seeing errs thats why.
		if (!s1.getShader().setUniformVar("doTexturing", std::vector<int>{0})) std::cout << "(Colfunc) cant disable texturing" << std::endl;
		if (!s2.getShader().setUniformVar("useUniformColours", std::vector<int>{1})) std::cout << "(Colfunc) cant set useUniformColours" << std::endl;
		if (!s2.getShader().setUniformVar("aColour", args)) std::cout << "(Colfunc) cant set aColour" << std::endl;

		// For now, just stop in place
		p.resolveCollision(s2);
	}

	void resolveCollision(Shape& s2);
};

// Ensure this lives longer than the template and other projectiles
class ProjectileContainer {
public:

	Projectile &projTemplate;
	//std::vector<Projectile*> projectiles;
	std::vector<std::shared_ptr<ProjectileData>> projectileData;
	float farPlane;

	// This now also performs collision detection on the passed colObjects aswell, so no need to handle that elsewhere.
	void drawProjectiles(ObjectContainer& colObjects);

	void fireProjectile(glm::vec3 dir, glm::vec3 pos, float vel);

	ProjectileContainer(Projectile& ptemp, float farPlane);
};