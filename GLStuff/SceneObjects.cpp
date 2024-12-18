#include <functional>
#include "SceneObjects.h"
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
static void wallCollisionFunc(Shape& s1, Shape& s2) {
    std::vector<float> args{ (float)s1.getVerts().getVerts()[0], (float)s1.getVerts().getVerts()[1], (float)s1.getVerts().getVerts()[2] };
    // Try to set the uniform if it exists - if uniform isnt used to make any meaningful diff gl will optimize it
    // out so if ur seeing errs thats why.
    if (!s1.getShader().setUniformVar("doTexturing", std::vector<int>{0})) std::cout << "(Colfunc) cant disable texturing" << std::endl;
    if (!s2.getShader().setUniformVar("useUniformColours", std::vector<int>{1})) std::cout << "(Colfunc) cant set useUniformColours" << std::endl;
    //s1.colour = args;
    if (!s1.getShader().setUniformVar("aColour", args)) std::cout << "huh" << std::endl;
}

// TODO: need to add wall collision function so something can actually happen if needs be
Wall::Wall(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, std::vector<VertAttribute> vas, bool collidable, bool isTextured, uint32_t texID, ShapeCollisionFuncT cFunc) : Quad::Quad(vaoID, vboID, startingVert, vertLen, prog, vas, NULL, 0, 0, collidable, cFunc, isTextured, texID) {
    if (cFunc == NULL) {
        setShapeCollisionFunc(wallCollisionFunc);
    }
    // Set default wall colour
    prog.setUniformVar("aColour", glm::value_ptr(wallColour));
}


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



Shape& ObjectContainer::checkColliding(Shape& collider, bool drawBounds, overlap_mask* om) {
    overlap_mask om1 = 0;
    glm::vec3 nill(0.0f);

    ObjContainerType::iterator beg = objects.begin();
    ObjContainerType::iterator iter = beg;
    for (; iter != objects.end(); iter++) {

        Shape& obj = *iter;

        // Dont check collisions on non-collidable objects.
        if (!obj.getCollidable()) continue;

        // First, make sure we have the vert positions and sizes figured out and then have a bounding box figured out
        if (obj.getVerts().getSize() != nill)
            obj.getVerts().findVertPositions();
        //if (obj->boundMin == obj->boundMax)
        // Where tf is the othe triangle lmfao
        if (drawBounds)
            obj.findBB(*this->t1, *this->t2);
        else
            obj.findBB();


        // Also invokes the collisionFunc for that object, so be careful
        om1 = collider.collidesWith(obj);
        if (om1 == OverlapMask::allColliding) {
            break;
        }
    }

    if (om1 != 0 && om != NULL) *om = om1;

    if (iter == objects.end()) iter = beg;
    return *iter;
}

void ObjectContainer::addObject(Shape& s) {
    objects.push_back(s);
}

// Draws all objects contained inside
void ObjectContainer::renderObjects() {
    for (Shape& obj : objects) {
        obj.draw();
    }
}

void ObjectContainer::findBBs() {
    for (Shape& obj : objects) {
        obj.draw();
    }
}

ObjectContainer::ObjectContainer(Triangle* t1, Triangle* t2) {
    this->t1 = t1;
    this->t2 = t2;
}