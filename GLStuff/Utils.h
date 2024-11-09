#pragma once
#include <string>
#include <vector>
#include "GLStuff.h"

// Namespaces should be defined in a header file, implementations are supplied
// in another c++ file while also specifying the namespace before ::.
namespace Util {

	// When enabled, gravity will pull shapes down the y axis at a predefined rate, that rate is here
	extern float gravPower;

	std::string getDataFromFile(const char *fname);
	// Same as the above, but for when typical string wont work due to null/badchars
	uint8_t* getDataFromFile(const char* fname, bool useString);

	// Copy conts to new allocation	
	void* allocAndCopyConts(void* conts, uint32_t len);

	// Convert error code to string
	const char* err2str(GLenum err);
	// Handle (output mainly) errors when they arise
	bool handleGLErrs();

	// For exercise
	glm::mat4 myLookAt(glm::vec3 pos, glm::vec3 targ, glm::vec3 up);
}

