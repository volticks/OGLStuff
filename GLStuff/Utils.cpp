#include "Utils.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_inverse.hpp"


using namespace std;

float Util::gravPower = 0.05f;
float Util::parabol_y(float x, float start, float xPower, float yPower){
    float mid = x + (xPower / 2);
    float end = x + xPower;

    // replaced c term with 0.1f for laffs innit.
    float y = (start * x * x) + (xPower * x) + 0.1f;

    // https://math.stackexchange.com/questions/1315260/emulating-a-parabola-in-my-game-for-a-jump
    //float y = (-yPower / ((mid - start) - (mid - end))) * (x - start) * (x - end);

    return y;
}

// Namespace specified before
string Util::getDataFromFile(const char *fname) {
  ifstream fs(fname);
  string res("");
  char *tmp;
  if (fs) {
    fs.seekg(0, ios::end);
    size_t sz = fs.tellg(); // get end pos offset/size
    fs.seekg(0, ios::beg);

    tmp = (char *)calloc(1, sz + 1);
    if (!tmp) {
      cerr << "Memory alloc fail wtf" << endl;
      return res;
    }

    fs.read(tmp, sz);
    res = string(tmp);
    free(tmp);
  } else {
    cerr << "Failed to open file specified" << endl;
  }

  return res;
}


// Namespace specified before
uint8_t* Util::getDataFromFile(const char* fname, bool useString) {
    ifstream fs(fname);
    //uint8_t *res;
    uint8_t* res = NULL;
    if (fs) {
        fs.seekg(0, ios::end);
        size_t sz = fs.tellg(); // get end pos offset/size
        fs.seekg(0, ios::beg);

        res = (uint8_t*)calloc(1, sz + 1);
        if (!res) {
            cerr << "Memory alloc fail wtf" << endl;
            return res;
        }

        fs.read((char*)res, sz);
    }
    else {
        cerr << "Failed to open file specified" << endl;
    }

    return res;
}

void* Util::allocAndCopyConts(void* conts, uint32_t len) {
    void* newPtr = malloc(len);
    memcpy(newPtr, conts, len);
    return newPtr;
}

// Convert error to string
// For some reason these arent found
//case GL_STACK_OVERFLOW: return "STACK OVERFLOW";
//case GL_STACK_UNDERFLOW: return "STACK UNDERFLOW";
const char* Util::err2str(GLenum err) {
    switch (err) {
    case GL_INVALID_ENUM: return "INVALID ENUM";
    case GL_INVALID_VALUE: return "INVALID VALUE";
    case GL_INVALID_OPERATION: return "INVALID OP";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID FRAMEBUF OP";
    default: return "";
    }
}

// TODO: Need to put this everywhere thats important
// Returns true on no errors, false on error
bool Util::handleGLErrs() {
    GLenum errCode = glGetError();
    stringstream ss;
    if (errCode == GL_NO_ERROR) return true;
    cerr << "[!] Errors: " << endl;
    do {
        ss << err2str(errCode);
    } while ((errCode = glGetError()) != GL_NO_ERROR);
    cerr << ss.str() << endl;
    return false;
}

glm::mat4 Util::myLookAt(glm::vec3 pos, glm::vec3 targ, glm::vec3 up) {
    // Actually inverse direction camera is pointing, but whatever. 
    // Pointing down the positive Z axis, as a camera should be.
    glm::vec3 camDirection = glm::normalize(pos - targ);
    // Use cross prod to get a vec pointing to the right (positive X axis)
    // Cross prod of positive Y and Z will give us positive X, obv
    glm::vec3 xVec = glm::normalize(glm::cross(up, camDirection));

    glm::vec3 upVec = glm::normalize(glm::cross(camDirection, xVec));

    // AKA rotation
    glm::mat4 lookAtMat(1.0f);
    lookAtMat[0][0] = xVec.x;
    lookAtMat[1][0] = xVec.y;
    lookAtMat[2][0] = xVec.z;

    lookAtMat[0][1] = upVec.x;
    lookAtMat[1][1] = upVec.y;
    lookAtMat[2][1] = upVec.z;

    lookAtMat[0][2] = camDirection.x;
    lookAtMat[1][2] = camDirection.y;
    lookAtMat[2][2] = camDirection.z;

    lookAtMat[3][3] = 1;

    glm::mat4 cameraPosTranslation(0.0f);
    cameraPosTranslation[0][0] = 1.0f;
    cameraPosTranslation[1][1] = 1.0f;
    cameraPosTranslation[2][2] = 1.0f;
    cameraPosTranslation[3][3] = 1.0f;

    // Now add the camera position to be translated. Remember to negate, we want the world to move in 
    // the OPPOSITE direction to where we move the camera to give the illusion of movement.
    cameraPosTranslation[3][0] = -pos.x;
    cameraPosTranslation[3][1] = -pos.y;
    cameraPosTranslation[3][2] = -pos.z;

    // Translate our 
    return lookAtMat * cameraPosTranslation;

    //glm::mat4 lookat = glm::mat4(xVec, up, camDirection );
}