#pragma once
#include "GLStuff.h"
#include "Shapes.h"
#include <functional>

// Stuff like walls, floor, etc


/*float wall_verts[] = {
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
};*/
// By and large this will just be a slightly more specialized version of quad
class Wall : public Quad {

public:
    const glm::vec3 wallColour = glm::vec3(0.5f, 0.5f, 0.0f);
    Wall(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, std::vector<VertAttribute> vas, bool collidable, bool isTextured, uint32_t texID, ShapeCollisionFuncT cFunc);

    // This doesnt work for numerous reasons.
    // Compact supplied vertices so they are more "wall"esque, IE: just truncate the depth/z.
    // This assumes verts are formatted as a cube would be, with 2 triangle vertices for each side
    /*void compact(float thresh) {
        float* v = this->getVerts().getVerts();
        uint32_t vlen = this->getVerts().getVertLen();
        uint32_t stride = this->getVerts().getStride() / sizeof(float);
        uint32_t vIdx = this->getVerts().getVertsIdx();

        while (vIdx < (vlen / sizeof(float))) {
            v[vIdx + 0] -= thresh;
            v[vIdx + 1] -= thresh;
            v[vIdx + 2] -= thresh;

            vIdx += stride;
            if (vIdx / 6 == stride) {
                vIdx += stride * 2;
            }
        }
        this->getVerts().deploy();
    }*/

};


//typedef std::vector<Shape&> ObjContainerType;
typedef std::vector<std::reference_wrapper<Shape>> ObjContainerType;

// TODO probably dont wanna just leave plain ptrs around and shit, fix, for i fear a uaf
// Find the FIRST colliding shape in our object container.
class ObjectContainer {
public:

    // TODO: Make smart ptr or similar, cant have vec of ref i dont think.
    ObjContainerType objects;

    // For drawing our bounding boxes
    // TODO: Make smart ptr or reference or similar
    Triangle *t1, *t2;

    Shape& checkColliding(Shape& collider, bool drawBounds = true, overlap_mask* om = NULL);

    void addObject(Shape& s);

    // Draws all objects contained inside
    void renderObjects();

    void findBBs();

    ObjectContainer(Triangle* t1, Triangle* t2);
};