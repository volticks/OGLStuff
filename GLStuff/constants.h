#pragma once // include header only once in preprocessing
#include "GLStuff.h"

// Window stuff
#define WINDOW_TITLE "HIYAAAAAAA"
#define WINDOW_H 1024
#define WINDOW_W 1024*2

// Shader stuff
#define VERTSHADER_IDX 0
#define FRAGSHADER_IDX 1
#define SHADERS_ARR_SZ 2
#define SHADER_ERR UINT32_MAX
using SHADER_T = uint32_t;
using SHADERPROG_T = uint32_t;
enum ShaderType {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
};

enum VertMoveDirection {
    Up,
    Down,
    Left,
    Right
};

enum VertPlane {
    X=0,
    Y=1,
    Z=2,
};

enum VertPoint {
    LEFT=0,
    RIGHT=3,
    TOP=6,
};

// Used for like 1 thing lol
typedef unsigned int DWORD;