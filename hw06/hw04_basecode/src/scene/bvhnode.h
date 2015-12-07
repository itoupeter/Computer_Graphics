#ifndef BVHNODE_H
#define BVHNODE_H

#include "scene/boundingbox.h"
#include "scene/geometry/geometry.h"
#include "raytracing/intersection.h"

class BoundingBox;
class Intersection;

class BVHNode{
public:
    BVHNode();

    BVHNode *pLeft;
    BVHNode *pRight;

    BoundingBox *pBBox;

    Geometry *pGeometry;

    Intersection getIntersection( const Ray &r );
};

#endif // BVHNODE_H
