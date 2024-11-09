#pragma once
#include <string>
#include "constants.h"
#include "Utils.h"
#include <iostream>
#include "GLStuff.h"

enum SHADERVAR_TYPE {
	FLOAT,
	INT,
};

// Holds info for uniform variables 
#define UNIFORM_NAME_MAX 1024
struct UniformInfo {
	char nameBuf[UNIFORM_NAME_MAX];
	GLsizei numWritten;
	GLint uniformSz;
	GLenum uType;
};

class Shader {
	SHADERPROG_T shaderProgID;
	std::string fragShaderSrc;
	std::string vertShaderSrc;

	SHADER_T vShaderID, fShaderID;
	std::string vShaderFname;
	std::string fShaderFname;

	bool usable = false;

public:

	// Constructor doesnt compile shaders yet. just loads the stuff from file
	Shader(const char* fname_vertShader, const char* fname_fragShader);

	// Handles destruction, deletes our shader program from openGL
	~Shader();

	bool compile();

	bool loadShaderFromFile(const char* fname_vertShader, const char* fname_fragShader);

	bool initShaderProgram();

	static bool checkErrors(SHADER_T id);

	SHADERPROG_T compileShader(ShaderType glShaderType);

	bool checkShaderLinkErrors();

	void use();

	// Used in destruction, and also whenever the hell we want >:)
	void delShaders();

	SHADERPROG_T getShaderProgID();

	std::string& getVShaderFname();

	std::string& getFShaderFname();

	std::string& getFragShaderSrc();

	std::string& getVertShaderSrc();

	SHADER_T getVertShaderID();

	SHADER_T getFragShaderID();

	bool getUsable();

	void setVertShaderSrc(std::string src);

	void setFragShaderSrc(std::string src);

	void setShaderProgID(SHADERPROG_T id);

	void setVertShaderFname(std::string f);

	void setFragShaderFname(std::string f);
	
	void setVertShaderID(SHADER_T id);

	void setFragShaderID(SHADER_T id);

	void setUsable(bool v);

	// Since this is templated we need to declare it in the header file to make the implementation accessable for the compiler: https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
	// This function is fairly stupid since it just acts as a wrapper around the more specific uniform var funcs. There are so many of these that its probably just better
	// to call the one you want, but whatever.
	template <typename T>
	bool setUniformVar(const char* varName, std::vector<T> const& args) {
		// Start using the shader before we set any uniforms.
		this->use();
		// Get the var location
		GLint loc = glGetUniformLocation(this->getShaderProgID(), varName);
		std::cout << "LOC = " << loc << std::endl;
		if (loc < 0) {
			std::cout << "No shader uniform found" << std::endl;
			return false;
		}

		// Remember to init.
		UniformInfo uInfo{ 0 };
		// Retrieve details
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetActiveUniform.xhtml
		std::cout << "PROG = " << this->getShaderProgID() << std::endl;
		glGetActiveUniform(this->getShaderProgID(), loc, sizeof(uInfo.nameBuf) - 1, &uInfo.numWritten, &uInfo.uniformSz, &uInfo.uType, uInfo.nameBuf);

		// Handle any GL errors this may cause.
		if (!Util::handleGLErrs()) return false;

		// TODO: Make this into a map or something similar in future so i dont have to switch case all this shit.
		switch (uInfo.uType) {
		case GL_FLOAT:
			glUniform1f(loc, args[0]);
			break;
		case GL_FLOAT_VEC2:
			glUniform2f(loc, args[0], args[1]);
			break;
		case GL_FLOAT_VEC3:
			glUniform3f(loc, args[0], args[1], args[2]);
			break;
		case GL_FLOAT_VEC4:
			glUniform4f(loc, args[0], args[1], args[2], args[3]);
			break;
		case GL_SAMPLER_2D:
		case GL_INT:
			glUniform1i(loc, args[0]);
			break;
		case GL_UNSIGNED_INT:
			glUniform1ui(loc, args[0]);
			break;
		//case GL_FLOAT_MAT4:
		//	glUniformMatrix4fv(loc, (GLboolean)args[0], (GLsizei)args[1], args[2]);
		default:
			// Unsupported
			std::cout << "Shader::setUniformVar, Unsupported type" << std::endl;
			return false;
		}

		std::cout << "v2 ";
		if (!Util::handleGLErrs()) return false;

		return true;
	}

	// For matrices
	template <typename T>
	bool setUniformVar(const char *varName, T* args) {
		// Start using the shader before we set any uniforms.
		this->use();
		GLint loc = glGetUniformLocation(this->getShaderProgID(), varName);
		std::cout << "(Mat) LOC = " << loc << std::endl;
		if (loc < 0) {
			std::cout << "(Mat) No shader uniform found" << std::endl;
			return false;
		}

		// Remember to init.
		UniformInfo uInfo{ 0 };
		// Retrieve details
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetActiveUniform.xhtml
		std::cout << "(Mat) PROG = " << this->getShaderProgID() << std::endl;
		glGetActiveUniform(this->getShaderProgID(), loc, sizeof(uInfo.nameBuf) - 1, &uInfo.numWritten, &uInfo.uniformSz, &uInfo.uType, uInfo.nameBuf);

		// Handle any GL errors this may cause.
		if (!Util::handleGLErrs()) return false;

		switch (uInfo.uType) {
		case GL_FLOAT_MAT2:
			glUniformMatrix2fv(loc, 1, GL_FALSE, (GLfloat*)args);
			break;
		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(loc, 1, GL_FALSE, (GLfloat*)args);
			break;
		case GL_FLOAT_MAT4:
			std::cout << "(Mat) Mat4 type." << std::endl;
			glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)args);
			break;
		default:
			// Unsupported
			std::cout << "(Mat) Shader::setUniformVar, Unsupported type" << std::endl;
			return false;
		}

		// Handle any GL errors this may cause.
		if (!Util::handleGLErrs()) return false;

		return true;

	}
};