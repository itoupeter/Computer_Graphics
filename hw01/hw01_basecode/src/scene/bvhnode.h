#ifndef BVHNODE_H
#define BVHNODE_H

#include "scene/boundingbox.h"
#include "scene/geometry/geometry.h"

class BoundingBox;

class BVHNode{
public:
    BVHNode();

    BVHNode *left_child;
    BVHNode *right_child;

    BoundingBox *bbox;

    QList< Geometry * > *pGeometries;
};

#endif // BVHNODE_H
