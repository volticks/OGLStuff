#include "Projectile.h"

void Projectile::drawOnce() {
	uint32_t vlen = this->getVerts().getVertLen();
	float* verts = this->getVerts().getVerts();
	glDrawArrays(GL_TRIANGLES, 0, vlen / this->getVerts().getStride());
}

void Projectile::drawImpl() {
	// setupOnce will translate our model, so we need to apply the change before then
	position += pd->direction * pd->velocity;
	setupOnce();
	drawOnce();
}

Projectile::Projectile(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, bool collidable, std::vector<VertAttribute> vas, ShapeCollisionFuncT cFunc, bool isTextured, uint32_t texID, float vel, glm::vec3 direction, glm::vec3 startingPos) :
	Shape(vaoID, vboID, startingVert, vertLen, prog, collidable, vas, cFunc, isTextured, texID) {
	this->pd = std::make_shared<ProjectileData>();
	this->pd->direction = direction;
	this->pd->velocity = vel;
	this->pd->position = startingPos;
	this->position = startingPos;
	// init to identity - shape constructor now does this automatically.
	//this->modelMat = glm::mat4(1.0f);

}

void Projectile::resolveCollision(Shape& s2) {
	// For now, just stop in place
	this->pd->velocity = 0.0f;
	this->pd->direction = glm::vec3(0.0f);


}


void ProjectileContainer::drawProjectiles(ObjectContainer& colObjects) {
	std::shared_ptr<ProjectileData> orig = projTemplate.pd;
	for (auto& pd : projectileData) {
		// No point drawing this if its beyond the far plane of our projection matrix.
		if (pd->position.z > farPlane) continue;
		//pd.position += pd.direction * pd.velocity;
		projTemplate.position = pd->position;
		projTemplate.pd = pd;
		// Still need to draw even if we are colliding just in case we want the projectile to stay.
		projTemplate.draw();
		// draw will change the position, so just need to save it
		pd->position = projTemplate.position;
		colObjects.checkColliding(projTemplate);
	}
	projTemplate.pd = orig;
}

void ProjectileContainer::fireProjectile(glm::vec3 dir, glm::vec3 pos, float vel) {
	std::shared_ptr<ProjectileData> pd = std::make_shared<ProjectileData>();
	pd->direction = dir;
	pd->velocity = vel;
	pd->position = pos;
	projectileData.push_back(pd);
}

ProjectileContainer::ProjectileContainer(Projectile& ptemp, float farPlane) : projTemplate(ptemp), farPlane(farPlane) {
	/*this->projTemplate = ptemp;
	this->farPlane = farPlane;*/
}