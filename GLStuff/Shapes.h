#pragma once
#include <cstdint>
#include <unordered_map>
#include <functional>
#include "constants.h"
#include "Shader.h"
#include "Utils.h"
#include "GLStuff.h"

// Forward declaration to avoid circular dependancy, since texture also needs Shapes.h
class Texture;

enum VAKind {
	VERTS,
	COLOUR,
	TEXTURE,
	UNSPEC,
};

struct VertAttribute {
	// Offset into the verts array for this vertex attrib
	uint32_t off;
	// Location of this vertex in the shader
	uint32_t vertLoc;
	// Number of elements in this attrib
	uint32_t vaSize;
	// Data type of this vertex attrib. Default to float cuz we use 
	// that all the time.
	GLenum vaType = GL_FLOAT;
	// What does this vert specifically represent? Doesnt have to be specified.
	VAKind kind = UNSPEC;
	// Idk what this is so imma just say it always false
	bool normalized = GL_FALSE;
	// This should be the same for all attribs, but can be configured
	uint32_t stride;

	VertAttribute(uint32_t off, uint32_t vertLoc, uint32_t vaSize, uint32_t stride, VAKind kind = UNSPEC, GLenum vaType = GL_FLOAT, bool normalized = GL_FALSE) :
		off(off), vertLoc(vertLoc), vaSize(vaSize), vaType(vaType), normalized(normalized), stride(stride), kind(kind) {}

	void bind() { 
		glVertexAttribPointer(vertLoc, vaSize, vaType, normalized, stride, (void*)off);
	}
	void enable() { glEnableVertexAttribArray(vertLoc); }
	void disable() { glDisableVertexAttribArray(vertLoc); }
};

// Class for managing vertices within context of OpenGL
class Verts {
private:

	// Members
	float* verts;
	uint32_t vertLen, stride, vertOff;
	// IDX's to track what side is where, used mainly for collision detection.
	// Only 4 so we dont have to complicate things
	uint32_t leftSideX, rightSideX, leftSideY, rightSideY;
	// Did we copy verts in or are they set from another source, in which case we 
	// cant free them when done.
	bool vertsCopied;

	// Vertex attribs
	std::vector<VertAttribute> vas;
	// Vertex attrib indexes
	int vertsIdx = -1, colourIdx = -1, texIdx = -1;

public:
	glm::vec3 size;

	// Constructors
	Verts(float* newVerts, uint32_t vLen, uint32_t stride, uint32_t vertOff);

	// Copy constructor
	Verts(Verts& v);

	// Constructor from vert attributes
	Verts(float* newVerts, uint32_t vLen, std::vector<VertAttribute> vas);

	// Destructor
	~Verts();

	// Methods

	// Configure vertices attributes, enable
	//void deploy();
	// (dun) TODO, replace above w/this
	void deploy(bool textured = false);

	// Check if one set of vertices are inside ours
	bool isInside(Verts& v);

	// Wanna find and assign the left and right of our shape.
	void findVertPositions();

	// Copy vertexes from a float* to us
	void copyVerts(float* src, uint32_t srcLen);

	// Move vertices by defined threshold, in direction specified
	void move(float thresh, VertMoveDirection vmd);
	// Adds a float to all verts, 2d only tho so nothing beyond x and y
	void addTo(float thresh, VertPlane vp);
	// TODO, replace above w/this
	void addTo(float thresh, VertPlane vp, bool useVas);

	// Getters and setters

	void setVerts(float* newVerts);

	float* getVerts();

	void setVertLen(uint32_t newVLen);

	uint32_t getVertLen();

	uint32_t getStride();

	void setStride(uint32_t newStride);

	uint32_t getVertOff();

	void setVertOff(uint32_t v);

	uint32_t getLeftX();

	void setLeftX(uint32_t newLeft);

	uint32_t getRightX();

	void setRightX(uint32_t newRight);

	uint32_t getLeftY();

	void setLeftY(uint32_t newLeft);

	uint32_t getRightY();

	void setRightY(uint32_t newLeft);

	bool getVertsCopied();

	void setVertsCopied(bool b);

	void setVas(std::vector<VertAttribute> vs);

	// Return a ref so we dont copy it
	std::vector<VertAttribute>& getVas();

	int getColourIdx();
	int getVertsIdx();
	int getTexIdx();

	// For debugging
	void dump();

};

inline float raiseTo(float val, float to) {
	while (val != 0.0f && val <= to) {
		val = val * 10.0f;
	}
	return val;
}

// Type for collision function in shape.
//using ShapeCollisionArgT = Shape&;
//using ShapeCollisionFuncT = std::function<void(Shape&, Shape&)>;

// Shapes which can be drawn, abstract, do not instantiate
class Shape {
public:
	using ShapeCollisionFuncT = std::function<void(Shape&, Shape&)>;
	Shape(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, uint32_t stride, uint32_t vertOff, Shader& prog, bool collidable, ShapeCollisionFuncT cFunc = &Shape::defaultCollision, bool isTextured=false, uint32_t texID = 0, bool gravEnabled = false);
	// TODO, replace above w/this
	Shape(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, bool collidable, std::vector<VertAttribute> vas, ShapeCollisionFuncT cFunc = &Shape::defaultCollision, bool isTextured = false, uint32_t texID = 0, bool gravEnabled = false);

	// Make class abstract, also subclasses should provide own implementation anyway.
	virtual void drawImpl() = 0;

	void unbind();

	void bind();

	void draw();

	// Should be implemented similar to drawImpl, except with only essential drawing code
	virtual void drawOnce() = 0;
	void setupOnce();
	void drawMany(std::vector<glm::vec3> &positions);

	void copyToBuffer(uint32_t glBufferType, void* conts, uint32_t contsLen);

	// Default collision function, if collision enabled
	static void defaultCollision(Shape& s, Shape& s1);

	// Using the vec position, check if our shape collides with another
	bool positionOverlaps(Shape& s1);

	// Does this shape collide with another shape?
	bool collidesWith(Shape& s);

	bool handleCollision(Shape& s);

	void setVerts(Verts verts);

	// Wanna return a reference here since we DO NOT want to pass by value and have the ptr freed when it gets destructed. 
	// This results in us copying our verts ptr into the gl array buffer then freeing it, aka, not a good time.
	Verts& getVerts();

	void setVao(uint32_t vao);

	uint32_t getVao();

	void setVbo(uint32_t vbo);

	uint32_t getVbo();

	Shader& getShader();

	void setShader(Shader& s);

	bool getIsBound();

	void setThresh(float thresh);

	float getThresh();

	void setCollidable(bool val);

	bool getCollidable();

	void setColliding(bool val);

	bool getColliding();

	void setShapeCollisionFunc(Shape::ShapeCollisionFuncT func);

	// Using the model matrix and position, find the location of the new bounding box.
	// Make sure this only gets called AFTER findVertPositions as we need the size computed there
	void findBB(Shape& s1, Shape& s2);
	// Check the above bounding box for collisions with s1
	bool checkBB(Shape& s1);
	// TODO encapsulate this as part of a texture class eventually.
	// Another TODO, make this private, use getters & setters
	uint32_t texID;
	bool isTextured;
	// Textures for the shape, if any
	std::vector<Texture*> texes;
	// Position of this shape in space
	glm::vec3 position;
	glm::mat4 modelMat;
	glm::vec3 boundMax, boundMin;
	bool gravEnabled;
private:
	uint32_t vaoID, vboID;
	Verts verts;
	bool isBound = false;
	float thresh = 0.1f;
	Shader& shader;
	// If overlapping with other verts, collide
	bool collidable;
	// If the above is set, are we actually colliding?
	bool colliding = false;
	// TODO: Function called on collision - if enabled
	ShapeCollisionFuncT collisionFunc;
};



// Tweak this if we ever need to change function type. Unsigned long arg means we can pass whatever we want basically
using ControllableFuncType = void(unsigned long);
// Class used to define an object which can be controlled by keyboard input
template <typename T = uint32_t, typename F = std::function<ControllableFuncType>>
class Controllable {
	// Allows us to assign integers representing behaviours to keys
	// doesnt have to be integers, but by default it is
	std::unordered_map<T, F> keyMap;
public:
	// Just here so we can delete keymap when no longer needed.
	~Controllable();
	virtual bool processKeyInput(T key) = 0;
	std::unordered_map<T, F>& getKeyMap();
	void setKeyMap(std::unordered_map<T, F> newMap);
	// Add default movement key behaviours to keymap, also initialize if we need to.
	virtual void initKeyMap();
};


class Triangle : public Shape, public Controllable<> {
public:
	virtual void drawImpl() override;
	virtual void drawOnce() override;

	virtual bool processKeyInput(uint32_t key) override;
	void initKeyMap() override;
	//virtual void drawTest();
	// Inherit Shape constructor
	//using Shape::Shape;
	Triangle(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, uint32_t stride, uint32_t vertOff, Shader& prog, bool collidable, ShapeCollisionFuncT cFunc = &Shape::defaultCollision, bool isTextured = false, uint32_t texID = 0, bool gravEnabled = false);
	// VA ver
	Triangle(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, bool collidable, std::vector<VertAttribute> vas, ShapeCollisionFuncT cFunc = &Shape::defaultCollision, bool isTextured = false, uint32_t texID = 0, bool gravEnabled = false);
};

// Shape with 4 sides, composed of 2 triangles
class Quad : public Shape, public Controllable<> {
private:
	uint32_t eboID;

	int* indices;
	uint32_t indiLen;
public:

	bool indicesReset;
	void drawImpl() override;
	void drawOnce() override;

	virtual bool processKeyInput(uint32_t key) override;
	void initKeyMap() override;
	// Re-do bind since we also need to bind vbo
	void bind();

	// Shouldnt rlly be called, since EBO lasts as long as the VAO does even when
	// the VAO is unbound, but still here so use sparingly.
	void unbindEbo();

	void setEboID(uint32_t eboID);

	uint32_t getEboID();

	void setIndices(int* indices);

	int* getIndices();

	void setIndiLev(uint32_t indiLen);

	uint32_t getIndiLen();

	// Constructor, just wraps Shape
	//Quad(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& s, uint32_t stride, uint32_t vertOff, int* indices, uint32_t indiLen, uint32_t eboID, bool collidable, ShapeCollisionFuncT cFunc = &Shape::defaultCollision, bool isTextured = false, uint32_t texID = 0);
	// For VA stuff
	Quad(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, std::vector<VertAttribute> vas, int* indices, uint32_t indiLen, uint32_t eboID, bool collidable, ShapeCollisionFuncT cFunc = &Shape::defaultCollision, bool isTextured = false, uint32_t texID = 0, bool gravEnabled = false);

	// Destructor, needs to free some stuff and do some opengl things aswell
	~Quad();
};
