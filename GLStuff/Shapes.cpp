#include "Shapes.h"
#include <stdlib.h>
#include <memory.h>
#include <iostream>
#include "Texture.h"

// Shape implementations

Shape::Shape(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, uint32_t stride, uint32_t vertOff, Shader& prog, bool collidable, ShapeCollisionFuncT cFunc, bool isTextured, uint32_t texID, bool gravEnabled)
: vaoID(vaoID), vboID(vboID), verts(startingVert, vertLen, stride, vertOff), shader(prog), collidable(collidable), collisionFunc(cFunc), isTextured(isTextured), texID(texID), gravEnabled(gravEnabled) {
	this->modelMat = glm::mat4(1.0f);
	this->position = glm::vec3(0.0f);
	this->boundMax = glm::vec3(0.0f);
	this->boundMin = glm::vec3(0.0f);
	this->gravPower = Util::gravPower;
}

Shape::Shape(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, bool collidable, std::vector<VertAttribute> vas, ShapeCollisionFuncT cFunc, bool isTextured, uint32_t texID, bool gravEnabled)
	: vaoID(vaoID), vboID(vboID), verts(startingVert, vertLen, vas), shader(prog), collidable(collidable), collisionFunc(cFunc), isTextured(isTextured), texID(texID), gravEnabled(gravEnabled) {
	// So it can be drawn correctly
	this->modelMat = glm::mat4(1.0f);
	this->position = glm::vec3(0.0f);
	this->boundMax = glm::vec3(0.0f);
	this->boundMin = glm::vec3(0.0f);
	this->gravPower = Util::gravPower;
}

void Shape::unbind() {

	if (this->isTextured) {
		this->getShader().setUniformVar("doTexturing", std::vector<uint32_t>{0});
		for (auto tex : this->texes)
			tex->unbind();
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	this->isBound = false;
}

void Shape::bind() {
	glBindVertexArray(this->getVao());
	glBindBuffer(GL_ARRAY_BUFFER, this->getVbo());
	this->isBound = true;

	// If we are textured, indicate to the fragment shader to draw textures.
	if (this->isTextured)
		this->getShader().setUniformVar("doTexturing", std::vector<uint32_t>{1});
}

void Shape::draw() {
	bind();
	drawImpl();
	unbind();
}

void Shape::setupOnce() {
	uint32_t vlen = this->getVerts().getVertLen();
	float* verts = this->getVerts().getVerts();
	this->copyToBuffer(GL_ARRAY_BUFFER, verts, vlen);
	this->getVerts().deploy(this->isTextured);

	// If textured, and we have textures, activate them now.
	if (this->isTextured && this->texes.size() > 0) {
		for (auto tex : this->texes) {
			tex->bind();
		}
	}
	// Err checking
	Util::handleGLErrs();
	//glUseProgram(this->getProgID());
	this->getShader().use();

	// Redundant on projectile instances being managed by projectilemanager
	if (this->gravEnabled) {
		this->position.y -= this->gravPower;
		std::cout << "(Shape::setupOnce) gravPower: " << Util::gravPower << std::endl;
	}

	// If we supply a position vector, use it
	glm::mat4 currModelMat = glm::translate(modelMat, position);
	this->getShader().setUniformVar("modelMat", &currModelMat);
}
void Shape::drawMany(std::vector<glm::vec3> &positions) {

	bind();
	setupOnce();
	for (auto pos : positions) {
		// Translate the position every time
		glm::mat4 model(1.0f);
		model = glm::translate(model, pos);
		this->getShader().setUniformVar("modelMat", &model);
		drawOnce();
	}
	unbind();
}

void Shape::copyToBuffer(uint32_t glBufferType, void* conts, uint32_t contsLen) {
	// VAO/VBO not bound, dont copy
	if (!this->getIsBound()) return;
	glBufferData(glBufferType, contsLen, conts, GL_DYNAMIC_DRAW);
}

void Shape::defaultCollision(Shape& s, Shape& s1) {
	std::cout << "Now colliding" << std::endl;
}

bool Shape::positionOverlaps(Shape& s1) {

	// If we have bounding box, check against that instead.
	if (s1.boundMax != s1.boundMin)
		return checkBB(s1);

	glm::vec3 ourPos = this->position;
	glm::vec3 sPos = s1.position;

	
	float thisXSize = this->getVerts().size.x;
	float thisYSize = this->getVerts().size.y;
	float thisZSize = this->getVerts().size.z;

	float sXSize = raiseTo(s1.getVerts().size.x, 0.1f);
	float sYSize = raiseTo(s1.getVerts().size.y, 0.1f);
	float sZSize = raiseTo(s1.getVerts().size.z, 0.1f);
	
	// Had to add and subtract the sizes from the positions because as it turns out the positions are at the opposite ends 
	// to where i thought they were, this means instead of just being able to take sPos.x to get the lhs X i have to ADD the size to get

	std::cout << "(Shape::positionOverlaps) ourPos.x: " << ourPos.x << "\tourPos.x + thisXSize: " << ourPos.x + thisXSize << std::endl;
	std::cout << "(Shape::positionOverlaps) sPos.x: " << sPos.x << "\tsPos.x + sXSize: " << sPos.x + sXSize << std::endl;
	bool x = ourPos.x + thisXSize >= sPos.x - sXSize && sPos.x + sXSize >= ourPos.x - thisXSize;
	//std::cout << "(Shape::positionOverlaps) ourPos.y: " << ourPos.y << "\tourPos.y + thisYSize: " << ourPos.y + thisYSize << std::endl;
	//std::cout << "(Shape::positionOverlaps) sPos.y: " << sPos.y << "\tsPos.y + sYSize: " << sPos.y + sYSize << std::endl;
	bool y = ourPos.y + thisYSize >= sPos.y - sYSize && sPos.y + sYSize >= ourPos.y - thisYSize;
	//std::cout << "(Shape::positionOverlaps) ourPos.z: " << ourPos.z << "\tourPos.z + thisZSize: " << ourPos.z + thisZSize << std::endl;
	//std::cout << "(Shape::positionOverlaps) sPos.z: " << sPos.z << "\tsPos.z + sZSize: " << sPos.z + sZSize << std::endl;
	bool z = ourPos.z + thisZSize >= sPos.z - sZSize && sPos.z + sZSize >= ourPos.z - thisZSize;


	return x && y && z;
	//returnx;
}

bool Shape::checkBB(Shape& s1) {

	std::cout << "(Shape::checkBB) using BB" << std::endl;

	if (s1.boundMax == s1.boundMin) {
		// TODO: handle case when s1 has no BB
		return true;
	}

	glm::vec3 thisSz = this->getVerts().size;

	float thisX = this->position.x;
	float thisXMax = thisX + thisSz.x;

	float thisY = this->position.y;
	float thisYMax = thisY + thisSz.y;

	float thisZ = this->position.z;
	float thisZMax = thisZ + thisSz.z;

	std::cout << "(Shape::checkBB) xmin " << s1.boundMin.x << "xmax " << s1.boundMax.x << std::endl;

	bool x = thisXMax > s1.boundMin.x && this->position.x - thisSz.z < s1.boundMax.x;
	bool y = thisYMax > s1.boundMin.y && this->position.y - thisSz.y < s1.boundMax.y;
	bool z = thisZMax > s1.boundMin.z && this->position.z - thisSz.z < s1.boundMax.z;

	return x && y && z;
}

bool Shape::collidesWith(Shape &s) {
	// Delegate this to the verts class, since this mainly involves checking vertices.
	// Another method here allows us to use the vec position to check collision.
	bool isInside = /*this->getVerts().isInside(s.getVerts()) || */positionOverlaps(s);
	if (this->getCollidable() && isInside) {
		this->setColliding(isInside);
		// Gotta deref this to pass as a ref.
		this->collisionFunc(*this, s);
	}
	return isInside;
}

void Shape::setVerts(Verts verts) {
	this->verts = verts;
}

Verts& Shape::getVerts() {
	return this->verts;
}

void Shape::setVao(uint32_t vao) {
	this->vaoID = vao;
}

uint32_t Shape::getVao() {
	return this->vaoID;
}

void Shape::setVbo(uint32_t vbo) {
	this->vboID = vbo;
}

uint32_t Shape::getVbo() {
	return this->vboID;
}

Shader& Shape::getShader() {
	return this->shader;
}

void Shape::setShader(Shader& s) {
	this->shader = s;
}

bool Shape::getIsBound() {
	return this->isBound;
}

void Shape::setThresh(float thresh) {
	this->thresh = thresh;
}

float Shape::getThresh() {
	return this->thresh;
}

void Shape::setCollidable(bool val) {
	this->collidable = val;
}

bool Shape::getCollidable() {
	return this->collidable;
}

void Shape::setColliding(bool val) {
	this->colliding = val;
}

bool Shape::getColliding() {
	return this->colliding;
}

void Shape::setShapeCollisionFunc(Shape::ShapeCollisionFuncT func) {
	this->collisionFunc = func;
}

// TODO, draw the points to make sure they are correct, probably use triangles, set position to the final min/max vec3's
void Shape::findBB(Shape &s1, Shape &s2) {
	this->findBB();

	std::vector<glm::vec3>pos{ this->boundMin, this->boundMax };

	s1.drawMany(pos);
}

// Does everything the above does, without drawing any shapes
void Shape::findBB() {
	float posX = position.x;
	float posY = position.y;
	float posZ = position.z;

	// https://gamedev.stackexchange.com/questions/20703/bounding-box-of-a-rotated-rectangle-2d
	// Values with no distortion
	float maxXSize = this->getVerts().size.x + posX;
	float maxYSize = this->getVerts().size.y + posY;
	float maxZSize = this->getVerts().size.z + posZ;

	// Not sure this is right
	float minXSize = posX - this->getVerts().size.x;
	float minYSize = posY - this->getVerts().size.y;
	//float minZSize = posZ - this->getVerts().size.z;
	float minZSize = posZ - this->getVerts().size.z;

	// Below need to be mat4 as we cant multiply a mat4 by vec3 (column issue)
	// Top left, at pos 0.
	glm::vec4 tl(minXSize, minYSize, minZSize, 1.0f);
	tl = modelMat * tl;
	glm::vec4 tr(maxXSize, minYSize, minZSize, 1.0f);
	tr = modelMat * tr;
	glm::vec4 bl(minXSize, maxYSize, minZSize, 1.0f);
	bl = modelMat * bl;
	glm::vec4 br(maxXSize, maxYSize, maxZSize, 1.0f);
	br = modelMat * br;

	std::cout << tr.x << " " << tr.y << " " << tr.z << " " << tl.x << " " << tl.y << " " << tl.z << std::endl;

	// Minimum x value
	float minX = std::min(tl.x, std::min(tr.x, std::min(bl.x, br.x)));
	float maxX = std::max(tl.x, std::max(tr.x, std::max(bl.x, br.x)));
	// Same for y and z
	float minY = std::min(tl.y, std::min(tr.y, std::min(bl.y, br.y)));
	float maxY = std::max(tl.y, std::max(tr.y, std::max(bl.y, br.y)));
	float minZ = std::min(tl.z, std::min(tr.z, std::min(bl.z, br.z)));
	float maxZ = std::max(tl.z, std::max(tr.z, std::max(bl.z, br.z)));
	// Need 6 vec3s to store min/max for points on all axis. These need to be transformed to world space using the model matrix
	glm::vec3 max(maxX, maxY, maxZ);
	glm::vec3 min(minX, minY, minZ);

	// Now we *should* have the corners as they should be
	std::cout << "(Shape::findBB) MAX: " << max.x << " " << max.y << " " << max.z << std::endl;
	std::cout << "(Shape::findBB) MIN: " << min.x << " " << min.y << " " << min.z << std::endl;

	this->boundMax = max;
	this->boundMin = min;
}

// Here we essentially say T comes from controllable, so hopefully whatever type we set there should be used
// as default, prolly uint32_t.
template<typename T, typename F>
std::unordered_map<T, F>& Controllable<T, F>::getKeyMap() {
	return this->keyMap;
}

// This sucks and is probably more trouble than its worth
template<typename T, typename F>
void Controllable<T, F>::setKeyMap(std::unordered_map<T, F> newMap) {
	this->keyMap = newMap;
}


// Right now does absolutely nothing.
template<typename T, typename F>
void Controllable<T, F>::initKeyMap() {
	//this->keyMap = *new std::unordered_map<T, F>();

	/*this->keyMap.insert({GLFW_KEY_UP, VertMoveDirection::Up});
	this->keyMap.insert({ GLFW_KEY_DOWN, VertMoveDirection::Down});
	this->keyMap.insert({ GLFW_KEY_LEFT, VertMoveDirection::Left});
	this->keyMap.insert({ GLFW_KEY_RIGHT, VertMoveDirection::Right});*/
}

template<typename T, typename F>
Controllable<T, F>::~Controllable() {
	//std::cout << "DESTRUCTED" << std::endl;
}

Triangle::Triangle(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, uint32_t stride, uint32_t vertOff, Shader& prog, bool collidable, ShapeCollisionFuncT cFunc, bool isTextured, uint32_t texID, bool gravEnabled) :
	Shape(vaoID, vboID, startingVert, vertLen, stride, vertOff, prog, collidable, cFunc, isTextured, texID) {
	this->initKeyMap();
}

//Shape::Shape(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, bool collidable, std::vector<VertAttribute> vas, ShapeCollisionFuncT cFunc, bool isTextured, uint32_t texID)
Triangle::Triangle(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, bool collidable, std::vector<VertAttribute> vas, ShapeCollisionFuncT cFunc, bool isTextured, uint32_t texID, bool gravEnabled) :
	Shape(vaoID, vboID, startingVert, vertLen, prog, collidable, vas, cFunc, isTextured, texID) {
	this->initKeyMap();
}

// Triangle implementations
void Triangle::drawOnce() {
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
// Triangle implementations
void Triangle::drawImpl() {
	setupOnce();
	drawOnce();
	Util::handleGLErrs();
}

bool Triangle::processKeyInput(uint32_t key) {
	auto res = this->getKeyMap().find(key);
	if (res == this->getKeyMap().end()) return false;
	res->second(1337);
	return true;
	
}

void Triangle::initKeyMap() {
	// Do non-specific key map init
	Controllable::initKeyMap();
	// Now, triangle specific stuff.

	// Movement
	auto moveFunc = [this](unsigned long dir) {
		VertMoveDirection vmd = (VertMoveDirection)dir;
		// Should be able to handle collision in here.... *Should* be
		this->getVerts().move(this->getThresh(), vmd);
		std::cout << vmd << std::endl;
		return;
		
	};

	// Use bind to bind a parameter so we dont have to re-write moveFunc 4 times slightly differently.
	this->getKeyMap().insert({ GLFW_KEY_UP, std::bind(moveFunc, VertMoveDirection::Up)});
	this->getKeyMap().insert({ GLFW_KEY_DOWN, std::bind(moveFunc, VertMoveDirection::Down)});
	this->getKeyMap().insert({ GLFW_KEY_LEFT, std::bind(moveFunc, VertMoveDirection::Left)});
	this->getKeyMap().insert({ GLFW_KEY_RIGHT, std::bind(moveFunc, VertMoveDirection::Right)});
}

// Quad implementations

// Impl for VA version
void Quad::drawImpl() {
	setupOnce();
	//this->copyToBuffer(GL_ARRAY_BUFFER, verts, vlen);
	// Remember to do this after we copy stuff into VBO otherwise problems yadda yadda 
	if (this->getIndiLen()) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->getEboID());
		this->copyToBuffer(GL_ELEMENT_ARRAY_BUFFER, this->getIndices(), this->getIndiLen());
	}
	drawOnce();
}

// DO NOT CALL THIS BEFORE YOU SETUP!!!!
void Quad::drawOnce() {
	uint32_t vlen = this->getVerts().getVertLen();
	float* verts = this->getVerts().getVerts();
	if (this->getIndiLen())
		glDrawElements(GL_TRIANGLES, this->getIndiLen() / sizeof(int), GL_UNSIGNED_INT, 0);
	else {
		//std::cout << "VERTS: " << vlen / this->getVerts().getStride();
		glDrawArrays(GL_TRIANGLES, 0, vlen / this->getVerts().getStride());
	}
}

void Quad::bind() {
	Shape::bind();
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->getEboID());
}

void Quad::unbindEbo() {
	if (!this->getIsBound()) return;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Quad::setEboID(uint32_t eboID) {
	this->eboID = eboID;
}

uint32_t Quad::getEboID() {
	return this->eboID;
}

void Quad::setIndiLev(uint32_t indiLen) {
	this->indiLen = indiLen;
}

uint32_t Quad::getIndiLen() {
	return this->indiLen;
}

void Quad::setIndices(int* indices) {
	this->indicesReset = true;
	this->indices = indices;
}

int* Quad::getIndices() {
	return this->indices;
}

// For VA version.
Quad::Quad(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, std::vector<VertAttribute> vas, int* indices, uint32_t indiLen, uint32_t eboID, bool collidable, ShapeCollisionFuncT cFunc, bool isTextured, uint32_t texID, bool gravEnabled) :
	Shape(vaoID, vboID, startingVert, vertLen, prog, collidable, vas, cFunc, isTextured, texID, gravEnabled) {
	if (indices) {
		this->indices = (int*)Util::allocAndCopyConts(indices, indiLen);
	}
	this->indiLen = indiLen;
	this->indicesReset = false;
	this->eboID = eboID;
	this->initKeyMap();
}

Quad::~Quad() {
	uint32_t vao = this->getVao();
	uint32_t vbo = this->getVbo();
	uint32_t ebo = this->getEboID();
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	// If we set indices from another source, we shouldnt free them since we dont own that allocation.
	// If set inside the class (via constructor allocAndCopyConts) we can free.
	if (this->indices && !this->indicesReset) free(this->indices);
}

bool Quad::processKeyInput(uint32_t key) {
	auto res = this->getKeyMap().find(key);
	if (res == this->getKeyMap().end()) return false;
	res->second(1337);
	return true;
}

void Quad::initKeyMap() {
	// Do non-specific key map init
	Controllable::initKeyMap();
	// Now, quad specific stuff.

	// Movement
	auto moveFunc = [this](unsigned long dir) {
		VertMoveDirection vmd = (VertMoveDirection)dir;
		this->getVerts().move(this->getThresh(), vmd);
		std::cout << vmd << std::endl;
		return;

		};

	// Use bind to bind a parameter so we dont have to re-write moveFunc 4 times slightly differently.
	this->getKeyMap().insert({ GLFW_KEY_UP, std::bind(moveFunc, VertMoveDirection::Up) });
	this->getKeyMap().insert({ GLFW_KEY_DOWN, std::bind(moveFunc, VertMoveDirection::Down) });
	this->getKeyMap().insert({ GLFW_KEY_LEFT, std::bind(moveFunc, VertMoveDirection::Left) });
	this->getKeyMap().insert({ GLFW_KEY_RIGHT, std::bind(moveFunc, VertMoveDirection::Right) });
}

// Verts stuff

Verts::Verts(float* newVerts, uint32_t vLen, uint32_t stride, uint32_t vertOff) {
	this->size = glm::vec3(0.0f);
	this->vertLen = vLen;
	this->stride = stride;
	this->vertOff = vertOff;
	this->copyVerts(newVerts, vLen);
}

// Copy constructor
Verts::Verts(Verts &v) {
	this->setVertLen(v.getVertLen());
	this->setStride(v.getStride());
	this->setVertOff(v.getVertOff());
	this->copyVerts(v.getVerts(), v.getVertLen());
}

// For vert attribs, TODO: make all verts code use vas
Verts::Verts(float *newVerts, uint32_t vLen, std::vector<VertAttribute> vas) {
	this->copyVerts(newVerts, vLen);
	this->vertLen = vLen;
	// Copy in
	this->vas = vas;

	// By default, default stride will take from vas[0]
	int tmpStride = vas[0].stride;

	// In the process, determine what each VA represents.
	for (int i = 0; i < vas.size(); i++) {
		VertAttribute va = vas[i];

		switch (va.kind) {
		case VERTS:
			this->vertsIdx = i;
			// When specified, stride is the Verts stride
			tmpStride = va.stride;
			break;
		case COLOUR:
			this->colourIdx = i;
			break;
		case TEXTURE:
			this->texIdx = i;
			break;
		}
	}

	this->stride = tmpStride;
}

// Destructor
Verts::~Verts() {
	// If verts were copied rather than being set, we can free since we know they belong to us.
	if (this->getVertsCopied())
		free(verts);
}

// Methods

void Verts::dump() {
	for (int i = 0; i < (getVertLen() / sizeof(float)); i++) {
		printf("%f\n", getVerts()[i]);
	}
}


// TODO this doesnt work with 4 rectangles AND triangles so we need some way to encode which vertex goes where (ie, is it top left, rop right, what does it mean?)
bool Verts::isInside(Verts &v) {
	uint32_t vSize = v.getVertLen() / sizeof(float);
	uint32_t thisVSize = this->getVertLen() / sizeof(float);
	float* vVerts = v.getVerts();
	float* thisVerts = this->getVerts();

	// Colliding on X+Y axis
	if (!((thisVerts[(this->getRightX()) % thisVSize] > vVerts[v.getLeftX() % vSize] &&
		thisVerts[(this->getLeftX()) % thisVSize] < vVerts[v.getRightX() % vSize]) &&
		(thisVerts[(this->getRightY()) % thisVSize] > vVerts[v.getLeftY() % vSize] &&
			thisVerts[(this->getLeftY()) % thisVSize] < vVerts[v.getRightY() % vSize]))) {
		return false;
	}

	return true;
}


void Verts::findVertPositions() {
	float* verts = this->getVerts();
	uint32_t size = this->getVertLen() / sizeof(float);
	
	// For some reason FLT_MIN is not negative by itself, so we have to negate.
	float minX = FLT_MAX, maxX = -FLT_MIN, minY = FLT_MAX, maxY = -FLT_MIN, minZ = FLT_MAX, maxZ = -FLT_MIN;
	int lhsX = 0, rhsX = 0, lhsY = 0, rhsY = 0, lhsZ = 0, rhsZ = 0;

	for (uint32_t i = 0; i < size; i += (this->getStride() / sizeof(float))) {
		if (verts[i + VertPlane::X] < minX) {
			lhsX = i + VertPlane::X;
			minX = verts[lhsX];
		} if (verts[i + VertPlane::Y] < minY) {
			lhsY = i + VertPlane::Y;
			minY = verts[lhsY];
		} if (verts[i + VertPlane::Z] < minZ) {
			lhsZ = i + VertPlane::Z;
			minZ = verts[lhsZ];
		}

		if (verts[i + VertPlane::X] > maxX) {
			rhsX = i + VertPlane::X;
			maxX = verts[rhsX];
		} if (verts[i + VertPlane::Y] > maxY) {
			rhsY = i + VertPlane::Y;
			maxY = verts[rhsY];
		} if (verts[i + VertPlane::Z] > maxZ) {
			rhsZ = i + VertPlane::Z;
			maxZ = verts[rhsZ];
		}
	}
	std::cout << "[*] LHSx -> " << lhsX << " RHSx -> " << rhsX << std::endl;
	std::cout << "[*] LHSy -> " << lhsY << " RHSy -> " << rhsY << std::endl;
	std::cout << "[*] LHSZ -> " << lhsZ << " RHSz -> " << rhsZ << std::endl;

	this->setLeftX(lhsX);
	this->setRightX(rhsX);
	this->setLeftY(lhsY);
	this->setRightY(rhsY);

	// Since were here, also set the size vec3 so it reflects size along x,y, and z axis.
	this->size.x = verts[rhsX];// -verts[lhsX];
	this->size.y = verts[rhsY];// -verts[lhsY];
	// TODO: Z
	this->size.z = verts[rhsZ];// -verts[lhsZ];

	// Verts can be silly silly values, ie less than 0.09 but still span the complete range of -1.0f->1.0f (2), this happens when
	// we supply verts we made in blender or something, so we need to normalize them somehow by bringing them to the same level as the others
	// so we can use them in comparisons with other verts (like with our collision stuff).
	/*this->size.x = raiseTo(this->size.x, 0.1f);
	this->size.y = raiseTo(this->size.y, 0.1f);
	this->size.z = raiseTo(this->size.z, 0.1f);*/

	std::cout << "X size: " << this->size.x << " y size: " << this->size.y << " z size: " << this->size.z << std::endl;
}

// Much simpler than the above, v nice
void Verts::deploy(bool textured) {
	for (auto va : vas) {
		if (va.kind == TEXTURE && !textured) continue;
		va.enable();
		va.bind();
	}
}

void Verts::copyVerts(float* src, uint32_t srcLen) {
	// +1 isnt rlly needed.
	this->setVertsCopied(true);
	uint32_t realSz = (srcLen+1);
	this->setVerts((float*)malloc(realSz));
	memcpy(this->getVerts(), src, realSz);
}

void Verts::move(float thresh, VertMoveDirection vmd) {
	switch (vmd) {
	case Up:
		addTo(thresh, VertPlane::X);
		break;
	case Down:
		addTo(-thresh, VertPlane::X);
		break;
	case Left:
		addTo(-thresh, VertPlane::Y);
		break;
	case Right:
		addTo(thresh, VertPlane::Y);
		break;
	default:
		std::cerr << "Verts::move wtf wyd lmao" << std::endl;
	}
}

// Return truwe if current row is forbidden according to vp.
bool checkRowForbidden(VertPlane vp, uint32_t currIdx) {
	const static VertPlane forbidden[] = {VertPlane::X, VertPlane::Y, VertPlane::Z};
	return forbidden[currIdx % 3] == vp;
}

/*void Verts::addTo(float thresh, VertPlane vp) {
	uint32_t lastSkip = -1;
	uint32_t vertOff = this->getVertOff() / sizeof(float);
	std::cout << "OFF: " << vertOff << std::endl;
	float *verts = this->getVerts();
	for (int i = 0; i < (this->getVertLen() / sizeof(float)); i++) {
		std::cout << "IDX: " << i << std::endl;
		// Handle offset stuff - since we dont necessarily wanna touch other vertex elements if they dont
		// denote location of shape. TODO: This and the other vertex stuf doesnt work when we have more than one
		// vertex ptr offset thing. Fix that if u can ba. Also this works but is very UGLY!!! >:(
		if (vertOff != 0 && i != 0 && (i - lastSkip) % vertOff == 0 && i-lastSkip > vertOff) {
			// Skip over the vertex elements we dont want to touch.
			lastSkip = i;
			//i += vertOff - 1; // TODO Doesnt work if theres stuff after our vert coords
			i += this->getStride()-1;
			continue;
		}
		// Dont wanna change the Z, and can also specify which other plane to avoid
		if (checkRowForbidden(vp, i) || checkRowForbidden(VertPlane::Z, i)) continue;
		// Finally, if the stars align we can add to the vertex coord.
		std::cout << "Adding to: " << verts[i] << std::endl;
		verts[i] += thresh;
	}
	std::cout << "DUN ADDING" << std::endl;
}*/

// Problem with this is that the
/*void Verts::addTo(float thresh, VertPlane vp) {
	uint32_t lastSkip = 0;
	uint32_t stride = this->getStride() / sizeof(float);
	float* verts = this->getVerts();
	uint32_t vertOff = this->getVertOff() / sizeof(float);
	//std::cout << "VOFF = " << vertOff << " STRIDE = " << stride << std::endl;
	for (int i = 0; i < (this->getVertLen() / sizeof(float)); i++) {

		uint32_t off = i - lastSkip;
		// Skip blocks of verts we dont want to touch (colours, tex coords, etc)
		if (i != 0 && off != 0 && off % vertOff == 0) {
			//std::cout << "ADDING STRIDE = " << i << " = " << i + (stride - off) << std::endl;
			// Subtract 1 cuz loop will add it back when we continue
			i += stride - off - 1;
			//std::cout << "I = " << i << std::endl;
			lastSkip = i + 1;
			continue;
		}

		//std::cout << "OOFFF = " << off << std::endl;
		//std::cout << "IDX = " << i << ", OFF = " << off << ", Forbidden plane = " << vp << ", VAL = " << verts[i] << std::endl;
		// Certain places we dont wanna add to, Z axis for example cuz we are 2d
		if (checkRowForbidden(vp, off) || checkRowForbidden(VertPlane::Z, off)) continue;
		//std::cout << "Allowed" << std::endl;
		
		verts[i] += thresh;
	}
}*/

// Impl using VA's
void Verts::addTo(float thresh, VertPlane vp) {
	uint32_t lastSkip = 0;

	// Take from first va if no vertex va is found
	int idx = this->vertsIdx;
	if (idx == -1) idx = 0;
	VertAttribute& vVa = vas[idx];

	uint32_t stride = vVa.stride / sizeof(float);
	//uint32_t vertOff = vVa.off / sizeof(float);
	// Prolly shouldnt use this lemoa.
	uint32_t vertOff = vVa.vaSize;// / sizeof(float);

	float* verts = this->getVerts();
	//std::cout << "VOFF = " << vertOff << " STRIDE = " << stride << std::endl;
	for (int i = 0; i < (this->getVertLen() / sizeof(float)); i++) {
		uint32_t off = i - lastSkip;
		// Skip blocks of verts we dont want to touch (colours, tex coords, etc)
		if (i != 0 && off != 0 && off % vertOff == 0) {
			// Subtract 1 cuz loop will add it back when we continue
			i += stride - off - 1;
			//std::cout << "I = " << i << std::endl;
			lastSkip = i + 1;
			continue;
		}
		// Certain places we dont wanna add to, Z axis for example cuz we are 2d
		if (checkRowForbidden(vp, off) || checkRowForbidden(VertPlane::Z, off)) continue;
		//std::cout << "Allowed" << std::endl;

		verts[i] += thresh;
	}
}

// Getters and setters
void Verts::setVerts(float* newVerts) {
	this->verts = newVerts;
}

float* Verts::getVerts() {
	return this->verts;
}

void Verts::setVertLen(uint32_t newVLen) {
	this->vertLen = newVLen;
}

uint32_t Verts::getVertLen() {
	return this->vertLen;
}

uint32_t Verts::getStride() {
	return this->stride;
}

void Verts::setStride(uint32_t newStride) {
	this->stride = newStride;
}

void Verts::setVertOff(uint32_t v) {
	this->vertOff = v;
}

uint32_t Verts::getVertOff() {
	return this->vertOff;
}

uint32_t Verts::getLeftX() {
	return this->leftSideX;
}

void Verts::setLeftX(uint32_t n) {
	this->leftSideX = n;
}

uint32_t Verts::getLeftY() {
	return this->leftSideY;
}

void Verts::setLeftY(uint32_t n) {
	this->leftSideY = n;
}

uint32_t Verts::getRightX() {
	return this->rightSideX;
}

void Verts::setRightX(uint32_t n) {
	this->rightSideX = n;
}

uint32_t Verts::getRightY() {
	return this->rightSideY;
}

void Verts::setRightY(uint32_t n) {
	this->rightSideY = n;
}

bool Verts::getVertsCopied() {
	return this->vertsCopied;
}

void Verts::setVertsCopied(bool b) {
	this->vertsCopied = b;
}

void Verts::setVas(std::vector<VertAttribute> vs) {
	this->vas = vs;
}

std::vector<VertAttribute>& Verts::getVas() {
	return this->vas;
}

int Verts::getColourIdx() {
	return this->colourIdx;
}
int Verts::getVertsIdx() {
	return this->vertsIdx;
}
int Verts::getTexIdx() {
	return this->texIdx;
}