#include "Shader.h"


// Constructor doesnt compile shaders yet. just loads the stuff from file
Shader::Shader(const char* fname_vertShader, const char* fname_fragShader) {
	this->loadShaderFromFile(fname_vertShader, fname_fragShader);
}

Shader::~Shader() {
	glDeleteProgram(this->getShaderProgID());
	//std::cout << "SHADER DESTRUCTED" << std::endl;
}

bool Shader::compile() {
	// Bruh lmao. 
	this->setUsable((this->compileShader(Vertex) != SHADER_ERR
		&& this->compileShader(Fragment) != SHADER_ERR
		&& this->initShaderProgram()));
	return this->getUsable();
}

bool Shader::loadShaderFromFile(const char* fname_vertShader, const char* fname_fragShader) {
	this->setVertShaderSrc(Util::getDataFromFile(fname_vertShader));
	this->setFragShaderSrc(Util::getDataFromFile(fname_fragShader));

	// Copy em over into our store
	this->setVertShaderFname(std::string(fname_vertShader));
	this->setFragShaderFname(std::string(fname_fragShader));

	// If empty, we failed somewhere
	if (this->getVertShaderSrc().empty() || this->getFragShaderSrc().empty()) {
		std::cerr << "loadShaderFromFile: Could not load shader source. Fragment shader or vertex shader are empty" << std::endl;
		return false;
	}

	return true;
}

bool Shader::initShaderProgram() {
	const SHADERPROG_T progID = glCreateProgram();
	if (progID == 0) {
		std::cerr << "Failed to create shader prog" << std::endl;
		return SHADER_ERR;
	}
		
	glAttachShader(progID, this->getVertShaderID());
	glAttachShader(progID, this->getFragShaderID());

	// Link attached shaders to program.
	glLinkProgram(progID);
	this->setShaderProgID(progID);
	if (this->checkShaderLinkErrors() == SHADER_ERR) {
		// if errors, dont save the ID cuz we broken asf.
		this->setShaderProgID(NULL);
		return false;
	}
		
	return true;
}

bool Shader::checkErrors(SHADER_T id) {
	int success = 0;
	char log[1024];

	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(id, sizeof(log), NULL, log);
		std::cerr << "Could not compile shader, log: " << log << std::endl;
		return false;
	}

	return true;
}

SHADERPROG_T Shader::compileShader(ShaderType glShaderType) {
	//this->setShaderProgID(glCreateShader(glShaderType));
	const SHADER_T shaderID = glCreateShader(glShaderType);
	const char* shaderCode;
	switch (glShaderType) {
	case Vertex:{
		this->setVertShaderID(shaderID);
		shaderCode = this->getVertShaderSrc().c_str();
		break;
	}
	case Fragment: {
		this->setFragShaderID(shaderID);
		shaderCode = this->getFragShaderSrc().c_str();
		break;
	}
	default: {
		// What?
		std::cerr << "compileShader: Shader type unknown WTF" << std::endl;
		return SHADER_ERR;
	}
	}
	//std::cout << shaderCode << std::endl;
	glShaderSource(shaderID, 1, &shaderCode, NULL);
	glCompileShader(shaderID);
	return Shader::checkErrors(shaderID) ? shaderID : SHADER_ERR;
}

bool Shader::checkShaderLinkErrors() {
	int success = 0;
	char log[1024];

	glGetProgramiv(this->getShaderProgID(), GL_LINK_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(this->getShaderProgID(), sizeof(log), NULL, log);
		std::cerr << "checkShaderLinkErrors: Could not link shaders, log: " << log << std::endl;
		return false;
	}

	return true;
}

void Shader::use() {
	if (!this->getUsable()) {
		std::cerr << "Shader unusable, did it compile?" << std::endl;
		return;
	}
	glUseProgram(this->getShaderProgID());
}

// Used after we have 
void Shader::delShaders() {
	glDeleteShader(this->getVertShaderID());
	glDeleteShader(this->getFragShaderID());
}

SHADERPROG_T Shader::getShaderProgID() {
	return this->shaderProgID;
}

std::string& Shader::getVShaderFname() {
	return this->vShaderFname;
}

std::string& Shader::getFShaderFname() {
	return this->fShaderFname;
}

std::string& Shader::getFragShaderSrc() {
	return this->fragShaderSrc;
}

std::string& Shader::getVertShaderSrc() {
	return this->vertShaderSrc;
}

SHADER_T Shader::getVertShaderID() {
	return this->vShaderID;
}

SHADER_T Shader::getFragShaderID() {
	return this->fShaderID;
}

bool Shader::getUsable() {
	return this->usable;
}

void Shader::setVertShaderSrc(std::string src) {
	// Should copy the string implicitly
	this->vertShaderSrc = src;
}

void Shader::setFragShaderSrc(std::string src) {
	// Should copy the string implicitly
	this->fragShaderSrc = src;
}

void Shader::setShaderProgID(SHADERPROG_T id) {
	this->shaderProgID = id;
}

void Shader::setVertShaderFname(std::string f) {
	this->vShaderFname = f;
}

void Shader::setFragShaderFname(std::string f) {
	this->fShaderFname = f;
}

void Shader::setUsable(bool v) {
	this->usable = v;
}

void Shader::setVertShaderID(SHADER_T id) {
	this->vShaderID = id;
}

void Shader::setFragShaderID(SHADER_T id) {
	this->fShaderID = id;
}