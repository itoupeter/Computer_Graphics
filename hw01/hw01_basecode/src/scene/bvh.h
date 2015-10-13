#ifndef BVH_H
#define BVH_H

#include "scene/bvhnode.h"
#include "scene/geometry/geometry.h"

class BVH{
public:
    static Scene *scene;

    static BVHNode *build( QList< Geometry* >  &geometries, BVHNode* pNode, int depth );
    static void clear( BVHNode * pBVHNode );
};

#endif // BVH_H
