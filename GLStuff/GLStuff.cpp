// GLStuff.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//
#include "GLStuff.h"
#include "Camera.h"
#include "Utils.h"
#include "constants.h"
#include "Shapes.h"
#include "Shader.h"
#include "Projectile.h"
#include "Texture.h"
#include "SceneObjects.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <tuple>
#include <sstream>
#include <math.h>

using namespace std;

const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource1 =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";

Camera globalCamera(false);

// Init stuff
// cb for resizing viewport when window is resized
void winResizeCb(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

// Since this calls a plain GL function it relies on GLAD already being set up
void initViewPort(GLFWwindow *window) {
  // Unclear what this returns so idk
  //// Sets the viewport inside the window where rendering will take place.
  /// However viewport will not adjust automatically if user / adjusts window
  /// size so we need to take care of that too.
  glViewport(0, 0, WINDOW_W, WINDOW_H);
  // Here we register a callback so the window size is automatically set if user
  // changes viewport
  glfwSetFramebufferSizeCallback(window, winResizeCb);
}

int initGLAD() {
  // Gives GLAD the correct OS specific func to use for setup
  return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

// Ease window creation
GLFWwindow *makeWindow(int width, int height, char *title) {
  GLFWwindow *window;

  // Make resizable
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  // Now make the window
  window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (window == NULL) {
    cerr << "Failed to make window" << endl;
  } else {
    // Makes window the current context
    glfwMakeContextCurrent(window);
    // Now window is set up we should initialize GLAD so it can setup the
    // function ptrs for OpenGL
    if (!initGLAD()) {
      cerr << "Failed to init GLAD" << endl;
      // No point continuing if we fail, set to null.
      window = NULL;
    }
    // Dont want to try setting a viewport on a window that doesnt exist, so do
    // it here.
    initViewPort(window);
  }

  // Enable depth testing and use of the z-buffer
  glEnable(GL_DEPTH_TEST);
  return window;
}

// Default init of window, funcs arent static since imma move em to their own
// shit later
GLFWwindow *initWindow() {
  return makeWindow(WINDOW_W, WINDOW_H, (char *)WINDOW_TITLE);
}


bool initGL() {
  if (glfwInit() != GLFW_TRUE)
    return false;
  // Hints here == options for the window we will create later
  //// Tell glfw what version we r on
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //// Restrict us to only newer features and not backwards compatability shit
  /// we dont need apparently
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  return true;
}

// deinit stuff
void deinit() {
  // Free all resources used by glfw
  glfwTerminate();
}

// Called on key press
static uint32_t currentKeyPressed = 0;

void keypress_cb(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  currentKeyPressed = key;
}
// Input stuff
void processInputKey(GLFWwindow *window) {
  switch (currentKeyPressed) {
  case GLFW_KEY_HOME:
    glfwSetWindowShouldClose(window, true);
    break;
  default:
    break;
  }
}

// Barebones setup only version of setupVertShaderBuf. Mainly meaning we dont do
// any vertices manipulation or adding to the VAO/VBO.
tuple<uint32_t, uint32_t, uint32_t> onlySetupVertShaderBufAndVAO() {
  uint32_t vertBufID = 0, vertAttribObjectID = 0, elementBufferObjID = 0;
  // Wanna store all this stuff in a VAO so we dont have to re-do this process
  // again. Generate VAO
  glGenVertexArrays(1, &vertAttribObjectID);
  // Generate buffer, assign ID
  glGenBuffers(1, &vertBufID);
  // Gen our EBO, assign
  glGenBuffers(1, &elementBufferObjID);

  // Bind, all changes will now be stored in this VAO.
  //glBindVertexArray(vertAttribObjectID);
  //glEnableVertexAttribArray(0);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObjID);
  //glBindVertexArray(0);

  return make_tuple(vertAttribObjectID, vertBufID, elementBufferObjID);
}

uint32_t makeTex(const char* fpath) {
    GLuint texID;
    int x, y,chan;
    uint8_t* data = stbi_load(fpath,&x,&y,&chan,0);
    if (!data) return -1337;
    
    // Generate a single texture object, ID is in texID
    glGenTextures(1, &texID);
    // Set the generated texture object as current active texture. Specify type as 2d.
    glBindTexture(GL_TEXTURE_2D, texID);
    // Configure texture wrapping settings
    // S/T == X/Y
    // We will repeat textures if we specify coords (draw) out of the mapped texture coord range
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Minifying, we will linearly interpolate between mipmaps AND uses linear interpolation for filtering method for... Well, filtering :P.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // Magnifying, linear interp
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texID;
}

std::vector<VertAttribute> makeVAs(uint32_t stride = 8 * sizeof(float)) {
    //uint32_t stride = 8 * sizeof(float);
    // Make sure u have the correct positions lmao
    VertAttribute ver(0*sizeof(float), 0, 3, stride, VERTS);
    VertAttribute col(3*sizeof(float), 1, 3, stride, COLOUR);
    VertAttribute tex(6*sizeof(float), 2, 2, stride, TEXTURE);

    std::vector<VertAttribute> vas{ ver, col, tex };
    return vas;
}

// Look at a target using the lookAt matrix
void lookAt(glm::vec3 pos, glm::vec3 cameraPos, Shape& s) {

    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::mat4 viewMat = glm::lookAt(cameraPos, pos, up);

    s.getShader().setUniformVar("viewMat", &viewMat);
}

// Translation positions for each shape
const glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};
void go3d(Shape &s, int i) {
    assert(i < sizeof(cubePositions) / sizeof(cubePositions[0]));
    // Model matrix, transform to world space 
    //// Start as identity, then apply transformations from there
    glm::vec3 xAxis(1.0f, 0.0f, 0.0f); // dont double up brackets here lol
    glm::mat4 modelMat(1.0f);
    // Rotate on x axis 55 degrees, essentially taking our model (plane) and facing it down toward the floor
    modelMat = glm::translate(modelMat, cubePositions[i]);
    modelMat = glm::rotate(modelMat, (float)glfwGetTime(), xAxis);
    s.modelMat = modelMat;
    s.position = glm::vec3(0.0f,0.0f,0.0f);

    s.draw();
    //std::this_thread::sleep_for(chrono::milliseconds(1));
}

namespace Mouse {
    float sensitivity = 10.0f;
    double lastX = WINDOW_W/2, lastY = WINDOW_H/2;
    float fov = glm::radians(45.0f);
    bool firstMouseInput = true;
    void mouseCB(GLFWwindow* window, double x, double y) {

        // Fix the jolt at the start
        if (firstMouseInput) {
            lastX = x;
            lastY = y;
            firstMouseInput = false;
        }
        // X starts at left, goes to right
        float xoff = x - lastX;
        // Y starts at bottom, goes to top, so have to reverse the args? Idk why, offset should still be offset regardless 
        // - correct, but it'll be the wrong sign.
        float yoff = lastY - y;
        lastX = x;
        lastY = y;

        // Move us by sensitivity threshold
        xoff *= sensitivity;
        yoff *= sensitivity;

        std::cout << "(Camera) xoff = " << xoff << std::endl;
        std::cout << "(Camera) yoff = " << yoff << std::endl;

        globalCamera.rotate(xoff, yoff);
    }
    void scrollCB(GLFWwindow* window, double x, double y) {
        float off = ((float)y) / 4.0f;
        //off -= (float)y;
        globalCamera.zoom(off);
    }
}


// Rendering stuff
void renderLoop(GLFWwindow *window) {
  // Call this func on keypress, invoked in glfwPollEvents
  glfwSetKeyCallback(window, keypress_cb);
  glfwSetCursorPosCallback(window, Mouse::mouseCB);
  glfwSetScrollCallback(window, Mouse::scrollCB);
  // Turn of cursor display, lock to center of window
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  // Init some shaders

  Shader s("res/shaders/vertshader.vert", "res/shaders/fragshader.frag");
  Shader s1("res/shaders/vertshader.vert", "res/shaders/fragshader.frag");
  if (!s.compile() || !s1.compile()) {
      cerr << "Failed to create shader(s)" << endl;
      return;
  }

  // Starting vertices for our triangle
  float vertices[] = {
      -0.5f, -0.5f, 0.0f, // left
      1.0f,  0.0f,  0.0f, // right
      -0.5f, 0.5f,  0.0f  // top
  };

  float rectVertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
  };

  float rectVertices2[] = {
   // Coords             Colours              Tex coords
   0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f, // top right
   0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,  // bottom right
  -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // bottom left
  -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,    0.0f, 1.0f,// top left 
  };

  // this is absolutely fucked btw lmao
  unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3,   // second triangle

    2, 3, 5,
    3, 4, 5,

    4, 5, 7,
    5, 6, 7,

    6, 7, 9,
    7, 7, 8,

    8, 9, 11,
    9, 10, 11,

    10, 11, 13,
    11, 12, 13,

    12, 13, 15,
    13, 14, 15,

    14, 15, 17,
    15, 16, 17,

    16, 17, 19,
    17, 18, 19

  };

  float colourVertices[] = {
    // positions         // colors
    0.5f,  -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
    0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // top 
  };

  float tVertsTex[] = {
      // positions         // colors          // Texture coords
      0.5f,  -0.5f, 0.0f,  -0.7f, 1.0f, 1.0f,  1.0f, 1.0f, // bottom right
      -0.5f, -0.5f, 0.0f,  -0.8f, 1.0f, 1.0f,  0.0f, 0.0f, // bottom left
      0.0f,  0.5f, 0.0f,   -0.9f, 1.0f, 1.0f,  0.0f, 1.0f  // top 
  };

  float projectileVertsTex[] = {
      // positions         // colors          // Texture coords
      0.5f,  -0.5f, 0.0f,  -0.5, 1.0f, 1.0f,  1.0f, 1.0f, // bottom right
      -0.5f, -0.5f, 0.0f,  -0.5f, 0.255f, 1.0f,  0.0f, 0.0f, // bottom left
      0.0f,  0.5f, 0.0f,   0.0f, 1.0f, 0.255f,  0.0f, 1.0f  // top 
  };

  float strange_vertices[] = {
      // new triangle
      -0.9951330423355103f, -0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, 0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, -0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      -0.9951330423355103f, 0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, 0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, 0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      0.9951330423355103f, 0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, -0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, 0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      0.9951330423355103f, -0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, -0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, -0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      0.9951330423355103f, 0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, -0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, 0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      -0.9951330423355103f, 0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, -0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, 0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      -0.9951330423355103f, -0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, 0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, 0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      -0.9951330423355103f, 0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, 0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, 0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      0.9951330423355103f, 0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, -0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, -0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      0.9951330423355103f, -0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, -0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, -0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      0.9951330423355103f, 0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, -0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, -0.5006346106529236f, -0.04328948259353638f, 1.0f, 1.0f,
      // new triangle
      -0.9951330423355103f, 0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      -0.9951330423355103f, -0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
      0.9951330423355103f, -0.5006346106529236f, 0.04328948259353638f, 1.0f, 1.0f,
  };

  float vertices_3d_cube[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };

  uint32_t qVaoID = 0;
  uint32_t qVboID = 0;
  uint32_t qEboID = 0;
  std::tie(qVaoID, qVboID, qEboID) = onlySetupVertShaderBufAndVAO();

  // Need a at the very least a different vao on the triangle because we dont want to have texture from quad mapped onto our triangle
  uint32_t tVaoID = 0;
  uint32_t tVboID = 0;
  // Dont use an ebo for the triangle, but need a var to capture all of tuple
  uint32_t tEboID = 0;
  std::tie(tVaoID, tVboID, tEboID) = onlySetupVertShaderBufAndVAO();

  std::cout << "Q: " << qVaoID << " " << qVboID << " " << qEboID << endl;
  std::cout << "T: " << tVaoID << " " << tVboID << " " << tEboID << endl;

  srand((unsigned int)glfwGetTime()*10);
  // Example of collision function as lambda
  auto t1ColFunc = [](Shape& s1, Shape& s2) {
      std::cout << "(Colfunc) COLLIDING" << std::endl;
      //std::vector<float> args{ (float)s1.getVerts().getVerts()[0], (float)s1.getVerts().getVerts()[1], (float)s1.getVerts().getVerts()[2] };
      std::vector<float> args{ 0.7f, 0.8f, 0.9f };
      // Try to set the uniform if it exists - if uniform isnt used to make any meaningful diff gl will optimize it
      // out so if ur seeing errs thats why.
      if (!s1.getShader().setUniformVar("doTexturing", std::vector<int>{0})) std::cout << "(Colfunc) cant disable texturing" << std::endl;
      if (!s2.getShader().setUniformVar("useUniformColours", std::vector<int>{1})) std::cout << "(Colfunc) cant set useUniformColours" << std::endl;
      if (!s2.getShader().setUniformVar("aColour", args)) std::cout << "(Colfunc) cant set aColour" << std::endl;
      };

  // Class for the texture
  Texture t("res\\img\\creature.jpg");
  Texture tex1("res\\img\\awesomeface.png", GL_TEXTURE1, GL_RGBA);

  // Causes access violation in nlogv64.dll, try it, investigate
  //Texture tex1("C:\\Users\\volti\\Pictures\\bro.jpg", GL_TEXTURE1, GL_RGBA);

  //uint32_t texID = makeTex("C:\\Users\\volti\\Pictures\\bap.jpg");

  // Vec describing vert attributes.
  std::vector<VertAttribute> qVas = makeVAs();
  //std::vector<VertAttribute> tVas = makeVAs();
  Triangle t1(tVaoID, tVboID, (float*)tVertsTex, sizeof(tVertsTex), s1, true, qVas, t1ColFunc, false, 0);

  Triangle t2(tVaoID, tVboID, (float*)tVertsTex, sizeof(tVertsTex), s1, true, qVas, t1ColFunc, false, 0);
  Triangle t3(tVaoID, tVboID, (float*)tVertsTex, sizeof(tVertsTex), s1, true, qVas, t1ColFunc, false, 0);

  qVas = makeVAs(5 * sizeof(float));
  //qVas.erase(qVas.begin() + 1);
  qVas[2].off = 3 * sizeof(float);
  qVas[2].vaSize = 2;

  glm::vec3 projDirection(0.7f, 0.01f, 0.5f);
  glm::vec3 startingPos(0.0f, 0.0f, 5.0f);
  Projectile p(tVaoID, tVboID, (float*)vertices_3d_cube, sizeof(vertices_3d_cube), s1, true, qVas, &Projectile::projectileColFunc, true, t.getId(), 0.01f, projDirection, globalCamera.pos, true);

  // Passing null as indices == use drawArrays instead
  Quad q(qVaoID, qVboID, (float*)vertices_3d_cube, sizeof(vertices_3d_cube), s, qVas, (int*)NULL, 0, qEboID, true, t1ColFunc, true, t.getId());
  //q.getShader().use(); // need to call use before we can mess w/uniforms
  if (!q.getShader().setUniformVar("texture1", std::vector<uint32_t>{ 0 }) ||
      !q.getShader().setUniformVar("texture2", std::vector<uint32_t>{ 1 })) {
      std::cout << "Tex Uniform issue" << std::endl;
  }

  q.texes.push_back(&t);
  q.texes.push_back(&tex1);

  // Wanted to test our collision on a normal shape
  Quad q1(qVaoID, qVboID, (float*)vertices_3d_cube, sizeof(vertices_3d_cube), s, qVas, (int*)NULL, 0, qEboID, true, t1ColFunc, false, 0);

  // Test to find left and right hand side of a shape based on vertices.
  q.getVerts().findVertPositions();
  t1.getVerts().findVertPositions();

  glm::vec3 xAxis(1.0f, 0.0f, 0.0f);
  glm::vec3 yAxis(0.0f, 1.0f, 0.0f);
  glm::mat4 id(1.0f);

  // Since w and floorPlane both share the same shader must remember to reset any uniforms BEFORE drawing the next.
  Wall w(qVaoID, qVboID, (float*)strange_vertices, sizeof(strange_vertices), s, qVas, true, false, 0, NULL);
  Wall floorPlane(qVaoID, qVboID, (float*)strange_vertices, sizeof(strange_vertices), s, qVas, false, false, 0, NULL);
  floorPlane.modelMat = glm::rotate(floorPlane.modelMat, glm::radians(90.0f), yAxis);
  floorPlane.modelMat = glm::scale(floorPlane.modelMat, glm::vec3(50.0f, 0.01f, 200.0f));
  // Putting it a lil closer to the floor since we want it.. yknow.. on the floor.
  floorPlane.modelMat = glm::translate(floorPlane.modelMat, glm::vec3(0.0f, -60.0f, 0.0f));
  //w.compact(0.5f);

  // Magnification
  auto magFunc = [&q](float a) {
      Verts& v = q.getVerts();
      if (!Texture::mag(v, a))
          std::cout << "Mag failed innit" << std::endl;
      return;
  };
  t1.getKeyMap().insert({ GLFW_KEY_0, std::bind(magFunc, 90.0f) });
  t1.getKeyMap().insert({ GLFW_KEY_1, std::bind(magFunc, -90.0f) });

 
  // Setup perspective frustrum, only need to do this once unless we wanna change it
  glm::mat4 projectionMat(1.0f);
  // Define our perspective frustrum
  float aspect = (float)WINDOW_W / (float)WINDOW_H;
  float near = 0.1f, far = 1000.0f;
  constexpr float fov = glm::radians(-45.0f);
  projectionMat = glm::perspective(fov, aspect, near, far);

  // Set projection mat on both shape types so they can be displayed
  q.getShader().setUniformVar("projMat", &projectionMat);
  t1.getShader().setUniformVar("projMat", &projectionMat);
  t2.getShader().setUniformVar("projMat", &projectionMat);
  t3.getShader().setUniformVar("projMat", &projectionMat);

  t2.getVerts().findVertPositions();
  t3.getVerts().findVertPositions();

  // ... and the projectile too - not actually necessary as p and t1 both share the same shader, but ig its good practice
  p.getShader().setUniformVar("projMat", &projectionMat);
  q1.getShader().setUniformVar("projMat", &projectionMat);
  // ... and the wall aswell 
  w.getShader().setUniformVar("projMat", &projectionMat);

  ProjectileContainer pc(p, far);

  q.getVerts().findVertPositions();
  std::cout << "(Projectile)" << std::endl;
  p.getVerts().findVertPositions();
  w.getVerts().findVertPositions();

  q1.getVerts().findVertPositions();

  //glm::mat4 wallModel = glm::rotate(id, glm::radians((float)glfwGetTime()), xAxis);
  glm::mat4 wallModel(1.0f);
  glm::mat4 wm1(1.0f);
  //wm1 = glm::scale(wm1, glm::vec3(2.0f, 2.0f, 2.0f));

  wallModel = glm::scale(id, glm::vec3(2.0f, 2.0f, 2.0f));
  wallModel = glm::translate(wallModel, glm::vec3(-0.5f, 1.0f, 1.0f));
  w.modelMat = wallModel;
  // Wanna rotate this a lil bit too.
  q1.modelMat = wallModel;

  ObjectContainer oc(&t2, &t3);
  //oc.addObject(&t1);
  //oc.addObject(&q);
  //oc.addObject(&q1);
  //oc.addObject(&p);
  oc.addObject(&w);

  while (!glfwWindowShouldClose(window)) {

    this_thread::sleep_for(chrono::milliseconds(100));
    // Process input first, then we can render stuff based on the input
    processInputKey(window);

    // Do rendering

    // Clear screen so we dont see result of previous frame.
    // 3 possible values for this, however we only care about clearing the
    // colour for time being so whatev.
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // To test matrix stuff, lets rotate the image around the z axis every iter.
    // Make an empty matrix, setting the diagonals to 0, making it an identity matrix and setting the stage for next line
    // (need to do this as by default will be 0, and anything * 0 = 0 so we need diagonals to be 1 so they produce something when transformed)
    glm::mat4 transformation(1.0f);
    // Only z is set, as such only z will be rotated around -- essentially means we will rotate the shape counter clockwise on the flat 2d plane.
    glm::vec3 z(1.0f, 1.0f, 0.0f);
    // Combine transformation with a rotation transform of 10degrees (converted to radians) around the z axis.
    transformation = glm::rotate(transformation, (float)glfwGetTime(), z);


    glm::mat4 viewMat(1.0f);
    // Move us back in the scene
    viewMat = glm::translate(viewMat, glm::vec3(0.0f, 0.0f, -3.0f));

    float radius = 10.0f;
    float cX = sin(glfwGetTime()) * radius;
    float cZ = cos(glfwGetTime()) * radius;
    glm::vec3 cameraPos(cX, 0.0f, cZ);

    globalCamera.processKeyInput(currentKeyPressed);
    //std::this_thread::sleep_for(chrono::milliseconds(20));
    assert(globalCamera.setCam(q.getShader()));
    assert(globalCamera.setCam(t1.getShader()));
    //p.direction = globalCamera.direction;
    //assert(globalCamera.setCam(p.getShader()));

    if (currentKeyPressed == GLFW_KEY_L) {
        //Projectile *p = new Projectile(tVaoID, tVboID, (float*)vertices_3d_cube, sizeof(vertices_3d_cube), s1, true, qVas, t1ColFunc, false, 0, 0.01f, projDirection, globalCamera.pos);
        pc.fireProjectile(globalCamera.direction, globalCamera.pos, 0.5f);
    }
    pc.drawProjectiles(oc);
    // As this is the projectile template object, this will have the position vec of the projectile fired most recently, meaning it is the most recent projectile.
    // As such we can do collision checking on it.
    w.modelMat = wm1;
    //p.collidesWith(w);
    //oc.checkColliding(p);
    p.collidesWith(t1);
    p.collidesWith(q1);

    if (!p.getShader().setUniformVar("aColour", std::vector<float>{ 255.0f, 4.0f, 4.0f }))
        std::cout << "(Projectile) colour failed" << std::endl;

    wallModel = glm::scale(id, glm::vec3(2.0f, 2.0f, 2.0f));
    wallModel = glm::rotate(id, glm::radians(45.0f), xAxis);
    wallModel = glm::translate(wallModel, glm::vec3(-0.5f, 1.0f, 1.0f));

    p.draw();
    //q1.draw();
    w.draw();

    //w.findBB(t2, t3);
    //q1.findBB(t2, t3);

    //w.getShader().setUniformVar("modelMat", glm::value_ptr(wallModel));

    floorPlane.getShader().setUniformVar("useUniformColours", std::vector<int>{1});
    floorPlane.getShader().setUniformVar("aColour", std::vector<float>{0.0f, 0.0f, 0.0f});
    floorPlane.draw();

    //q1.draw();
    // Reset after, just in case a collision was had
    w.getShader().setUniformVar("useUniformColours", std::vector<int>{0});
    t1.getShader().setUniformVar("useUniformColours", std::vector<int>{0});

    for (int i = 0; i < 10; i++) {
        go3d(t1, i);
        go3d(q, 5);   
    }
    //oc.renderObjects();


    //t1.collidesWith(q);
    //q.collidesWith(t1);
    //t1.processKeyInput(currentKeyPressed);
    //q.processKeyInput(currentKeyPressed);

    //q.draw();
    currentKeyPressed = 0;
    // Swap back buffer to front buffer TODO explain this
    glfwSwapBuffers(window);

    // Monitors for events, calls callbacks we can register for said events
    // (events like keyboard input, etc)
    glfwPollEvents();
  }
  // Delete vao first since that holds references to all the other shit
  //glDeleteVertexArrays(1, &vaoID);
  //glDeleteBuffers(1, &vboID);
  //glDeleteBuffers(1, &eboID);
  //glDeleteProgram(progID);
}

int main() {
  std::cout << "Starting setup...\n";
  if (!initGL()) {
    cerr << "Failed to setup GL" << endl;
    return 1;
  }
  GLFWwindow *window = initWindow();
  if (!window) {
    // cerr << "Failed to setup window" << endl;
    deinit();
    return 1;
  }
  // Now everythings in place we can start rendering stuff
  renderLoop(window);
  // We dun
  deinit();
  return 0;
}
