#pragma once

// Shapes which can be drawn, abstract, do not instantiate
class Shape {

private:
	uint32_t vaoID, vboID, progID;
	Verts verts;
	bool isBound = false;

public:
	Shape(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, uint32_t progID)
		: vaoID(vaoID), vboID(vboID), vert(startingVert), vertLen(vertLen) vert(Verts(startingVert, vertLen)) {}

	// Make class abstract, also subclasses should provide own implementation anyway.
	virtual void drawImpl() = 0;

	void unbind();

	void bind();

	void draw();

	void copyToBuffer(uint32_t glBufferType, void* conts, uint32_t contsLen);

	void setVerts(Verts verts);

	Verts getVerts();

	void setVao(uint32_t vao);

	uint32_t getVao();

	void setVbo(uint32_t vbo);

	uint32_t getVbo();

	uint32_t getProgID();

	void setProgID(uint32_t progID);

	bool getIsBound();
};

class Triangle : public Shape {
public:
	void drawImp() override;
};

class Quad : public Triangle {
private:
	uint32_t eboID;
public:
	void drawImpl() override;

	void setEboID(uint32_t eboID);

	uint32_t getEboID();
};

// Class for managing vertices within context of OpenGL
class Verts {
private:

	// Members
	float* verts;
	uint32_t vertLen;

public:

	// Constructors

	Verts(float* newVerts, uint32_t vLen);

	// Copy constructor
	Verts(Verts& v);
}

	// Destructor
	~Verts();

	// Methods

	void copyVerts(float* src, uint32_t srcLen);

	// Getters and setters

	void setVerts(float* newVerts);

	float* getVerts();

	void setVertLen(uint32_t newVLen);

	uint32_t getVertLen();
};