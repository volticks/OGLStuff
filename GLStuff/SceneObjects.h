#pragma once
#include "GLStuff.h"
#include "Shapes.h"
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
    static void wallCollisionFunc(Shape& s1, Shape& s2) {
        std::vector<float> args{ (float)s1.getVerts().getVerts()[0], (float)s1.getVerts().getVerts()[1], (float)s1.getVerts().getVerts()[2] };
        // Try to set the uniform if it exists - if uniform isnt used to make any meaningful diff gl will optimize it
        // out so if ur seeing errs thats why.
        if (!s1.getShader().setUniformVar("aColour", args)) std::cout << "huh" << std::endl;
    }

	// TODO: need to add wall collision function so something can actually happen if needs be
	Wall(uint32_t vaoID, uint32_t vboID, float* startingVert, uint32_t vertLen, Shader& prog, std::vector<VertAttribute> vas, bool collidable, bool isTextured, uint32_t texID, ShapeCollisionFuncT cFunc) : Quad::Quad(vaoID, vboID, startingVert, vertLen, prog, vas, NULL, 0, 0, collidable, cFunc, isTextured, texID) {
        if (cFunc == NULL) {
            setShapeCollisionFunc(wallCollisionFunc);
        }
        // Set default wall colour
        prog.setUniformVar("aColour", glm::value_ptr(wallColour));
	}


    // Compact supplied vertices so they are more "wall"esque, IE: just truncate the depth/z.
    // This assumes verts are formatted as a cube would be, with 2 triangle vertices for each side
    void compact(float thresh) {
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
    }

};

// TODO probably dont wanna just leave plain ptrs around and shit, fix, for i fear a uaf
// Find the FIRST colliding shape in our object container.
class ObjectContainer {
public:

    // TODO: Make smart ptr or similar, cant have vec of ref i dont think.
    std::vector<Shape*> objects;

    // For drawing our bounding boxes
    // TODO: Make smart ptr or reference or similar
    Triangle *t1, *t2;

    Shape* checkColliding(Shape &collider, bool drawBounds = true, overlap_mask* om = NULL) {
        Shape *collidee = NULL;
        overlap_mask om1=0;
        glm::vec3 nill(0.0f);
        for (Shape* obj : objects) {

            // Dont check collisions on non-collidable objects.
            if (!obj->getCollidable()) continue;

            // First, make sure we have the vert positions and sizes figured out and then have a bounding box figured out
            if (obj->getVerts().getSize() != nill)
                obj->getVerts().findVertPositions();
            //if (obj->boundMin == obj->boundMax)
            // Where tf is the othe triangle lmfao
            if (drawBounds)
                obj->findBB(*this->t1, *this->t2);
            else
                obj->findBB();


            // Also invokes the collisionFunc for that object, so be careful
            om1 = collider.collidesWith(*obj);
            if (om1 == OverlapMask::allColliding) {
                collidee = obj;
                break;
            }
        }

        if (om1 != 0 && om != NULL) *om = om1;

        return collidee;
    }

    void addObject(Shape *s) {
        objects.push_back(s);
    }

    // Draws all objects contained inside
    void renderObjects() {
        for (Shape* obj : objects) {
            obj->draw();
        }
    }

    void findBBs() {
        for (Shape* obj : objects) {
            obj->draw();
        }
    }

    ObjectContainer(Triangle *t1, Triangle *t2) {
        this->t1 = t1;
        this->t2 = t2;
    }
};