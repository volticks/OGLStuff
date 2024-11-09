#include "Texture.h"

Texture::Texture(std::string fPath, GLenum texLoc, GLenum colourType, GLenum texType, GLenum wrapModeS, GLenum wrapModeT, GLenum minFil, GLenum magFil) : 
	fpath(fPath), texLoc(texLoc), colType(colourType), texTarg(texType), wrapModeS(wrapModeS), wrapModeT(wrapModeT), minFilter(minFil), magFilter(magFil) {
	
	// TODO, use actual values from constructor here
	Param wrapS(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT, pINT);
	Param wrapT(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT, pINT);
	Param minF(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR, pINT);
	Param magF(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR, pINT);

	this->params.assign({wrapS, wrapT, minF, magF});

	this->setTex(this->fpath);
}

Texture::~Texture() {
	// Only freed here just in case we wanna access the data again for some reason
	std::cout << "DESTRUCTEDDDDD" << std::endl;
	stbi_image_free(this->data);
	
}

void Texture::setTex(std::string path) {
	// Delete data since we will be re-loading it
	if (this->data) stbi_image_free(this->data);

	if (path.empty()) 
		path = this->fpath;
	else 
		this->fpath = path;

	this->data = stbi_load(path.c_str(), &this->x, &this->y, &this->channels, 0);
	if (!data)
		throw std::exception("Texture::setTex couldnt load file"); 

	glGenTextures(1, &this->texID);
	glBindTexture(this->texTarg, this->texID);

	if (!Util::handleGLErrs())
		throw std::exception("Texture::setTex GL errors occurred");

	// Set all parameters on the texture
	for (auto p : this->getParams()) {
		p.set();
	}

	glTexImage2D(this->texTarg, 0, GL_RGB, this->x, this->y, 0, this->colType, GL_UNSIGNED_BYTE, this->data);

	if (!Util::handleGLErrs())
		throw std::exception("Texture::setTex GL errors occurred");

	glGenerateMipmap(this->texTarg);

	if (!Util::handleGLErrs())
		throw std::exception("Texture::setTex GL errors occurred");
}

// Binds the texture for a single drawing call
void Texture::bind() {
	std::cout << "Binding texture" << std::endl;
	glActiveTexture(this->texLoc);
	glBindTexture(this->texTarg, this->texID);
}

void Texture::unbind() {
	std::cout << "Unbinding texture" << std::endl;
	// Also clear it out, next shape shouldnt have stale stuff where tex is
	
	glBindTexture(this->texTarg, 0);
	glDeleteTextures(1, &this->texTarg);
}

bool Texture::mag(Verts& v, float thresh) {
	// TODO, normalize thresh
	int texIdx = v.getTexIdx();
	// Not set yet
	if (texIdx == -1)
		return false;

	// Normalize to value between 0 and 1
	thresh /= 100.0f;
	thresh = std::fmin(thresh, 1.0f);

	std::cout << "Thresh: " << thresh << std::endl;

	float* verts = v.getVerts();
	uint32_t vSize = v.getVertLen() / sizeof(float);

	VertAttribute texAttrib = v.getVas()[texIdx];
	uint32_t tOff = texAttrib.off / sizeof(float);
	uint32_t tStride = texAttrib.stride / sizeof(float);
	uint32_t tSize = texAttrib.vaSize;

	std::cout << "Size: " << tSize << std::endl;

	int lastSkip = 0;
	int off;
	for (int i = tOff; i < vSize; i++) {
		off = i - tOff;
		if (off != 0 && off == tSize) {
			// xtra -1 cuz i be that way
			i += tStride-tSize-1;
			tOff = i+1;
			continue;
		}
		std::cout << "off = " << off << std::endl;
		std::cout << "i = " << i << std::endl;
		std::cout << "Verts[i] = " << verts[i] << std::endl;

		// TODO, since some of the coords are 0 this has no effect, fix that so zoom works correctly to the center of the shape
		// If less than 0, we min instead of mag.
		if (thresh < 0.0f) verts[i] /= thresh;
		else verts[i] *= thresh;
	}

	return true;

}

// Getters and setters

std::vector<Param> Texture::getParams() {
	return this->params;
}

void Texture::setParams(std::vector<Param> p) {
	this->params = p;
}

GLuint Texture::getId() {
	return this->texID;
}