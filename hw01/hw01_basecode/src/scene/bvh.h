#ifndef BVH_H
#define BVH_H

#include "scene/bvhnode.h"
#include "scene/geometry/geometry.h"

class BVH{
public:
    BVH();

    void buildBVH( QList< Geometry* >  &geometries, BVHNode* pBVHNode, int depth );
    void clear();
};

#endif // BVH_H
