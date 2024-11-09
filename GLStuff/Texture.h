#pragma once
#include <string>
#include "constants.h"
#include "Utils.h"
#include <iostream>
#include "GLStuff.h"
#include "Shapes.h"

// TODO maybe expand for vector types... Maybe (ie glTexParameterfv/glTexParameteriv)
enum ParamType {
	pFLOAT,
	pINT,
};

struct Param {
	GLenum targ;
	GLenum pName;
	// Either float or int, do not matter
	DWORD pVal;
	ParamType pType;

	Param(GLenum targ, GLenum name, DWORD pVal, ParamType pt) : targ(targ), pName(name), pVal(pVal), pType(pt) {	
	}

	~Param() {
	}

	void set() {
		switch (pType) {
		case pFLOAT:
			glTexParameterf(targ, pName, pVal);
			break;
		case pINT:
			glTexParameteri(targ, pName, pVal);
			break;
		default:
			throw std::exception("Param::set unknown type");
		}

	}
};

class Texture {
private:
	GLuint texID;
	int x, y, channels;
	std::string fpath;
	uint8_t* data;

	GLenum texTarg;
	GLenum wrapModeS, wrapModeT;
	GLenum minFilter;
	GLenum magFilter;
	// Sampler location
	GLenum texLoc;
	// RGB/RGBA for now
	GLenum colType;

	std::vector<Param> params;

public:

	// Constructors
	Texture(std::string fPath, GLenum texLoc = GL_TEXTURE0, GLenum colourType = GL_RGB, GLenum texTarg = GL_TEXTURE_2D, GLenum wrapModeS = GL_REPEAT, GLenum wrapModeT = GL_REPEAT, GLenum minF = GL_LINEAR_MIPMAP_LINEAR, GLenum magF = GL_LINEAR);
	~Texture();

	// Used for when we wanna reset our texture to a different file, or if unspec reload data from the one already specified
	void setTex(std::string fpath = "");

	// Bind texture for a single drawing call
	void bind();

	// Unbind texture after draw call
	void unbind();

	// Magnify the texture by thresh %
	static bool mag(Verts& v, float thresh);

	// Getters and setters
	std::vector<Param> getParams();

	void setParams(std::vector<Param> p);

	GLenum getId();
};
