
#include "scene/bvh.h"
#include <algorithm>

bool compareX( const Geometry *a, const Geometry *b ){
    return a->pBBox->center[ 0 ] < b->pBBox->center[ 0 ];
}

bool compareY( const Geometry *a, const Geometry *b ){
    return a->pBBox->center[ 1 ] < b->pBBox->center[ 1 ];
}

bool compareZ( const Geometry *a, const Geometry *b ){
    return a->pBBox->center[ 2 ] < b->pBBox->center[ 2 ];
}

void BVH::buildBVH( QList< Geometry * > &geometries, BVHNode *pNode, int depth ){

    //---no geometries---
    if( geometries.size() == 0 ){
        return;
    }

    //---attach geometris array to current node---
    pNode->pGeometries = &geometries;
    pNode->bbox = new BoundingBox( BoundingBox::combine( geometries ) );

    //---reach leaf node---
    if( geometries.size() == 1 ){
        return;
    }

    //---sort geometries according to X/Y/Z axis---
//    bool ( *compareFunc )( const Geometry *, const Geometry * )[]{
//        compareX, compareY, compareX,
//    };

    std::sort( geometries.begin(), geometries.end(), compareX );

    //---split into children nodes---
    int size( geometries.size() );
    int mid_size( size >> 1 );

    BVHNode *pLeft( pNode->left_child = new BVHNode() );
    BVHNode *pRight( pNode->right_child = new BVHNode() );

    pLeft->pGeometries = new QList< Geometry * >( geometries.mid( 0, mid_size ) );
    pRight->pGeometries = new QList< Geometry * >( geometries.mid( mid_size, size ) );

    pLeft->bbox = NULL;
    pRight->bbox = NULL;

    //---build tree recurrently---
    buildBVH( *pLeft->pGeometries, pLeft, depth + 1 );
    buildBVH( *pRight->pGeometries, pRight, depth + 1 );
}
