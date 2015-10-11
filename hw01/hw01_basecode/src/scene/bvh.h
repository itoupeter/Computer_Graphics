#ifndef BVH_H
#define BVH_H

#include "scene/bvhnode.h"
#include "scene/geometry/geometry.h"

class BVH{
public:
    static Scene *scene;

    static void build( QList< Geometry* >  &geometries, BVHNode* pBVHNode, int depth );
    static void clear( BVHNode * pBVHNode );
};

Scene *BVH::scene = NULL;

#endif // BVH_H
