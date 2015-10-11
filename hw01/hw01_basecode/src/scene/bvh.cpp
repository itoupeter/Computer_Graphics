
#include "scene/bvh.h"
#include <algorithm>
#include <QMessageBox>


bool compareX( const Geometry *a, const Geometry *b ){
    return a->pBBox->center[ 0 ] < b->pBBox->center[ 0 ];
}

bool compareY( const Geometry *a, const Geometry *b ){
    return a->pBBox->center[ 1 ] < b->pBBox->center[ 1 ];
}

bool compareZ( const Geometry *a, const Geometry *b ){
    return a->pBBox->center[ 2 ] < b->pBBox->center[ 2 ];
}

void BVH::build( QList< Geometry * > &geometries, BVHNode *pNode, int depth ){

    //---no geometry---
    if( geometries.size() == 0 ){
        QMessageBox msg;
        msg.setText( "No geometry in the scene." );
        msg.exec();
        return;
    }

    //---current node---
    pNode->pGeometries = &geometries;
    pNode->pBBox = new BoundingBox( BoundingBox::combine( geometries ) );

    //---reach leaf node---
    if( geometries.size() == 1 ){
        return;
    }

    //---sort geometries according to X/Y/Z axis---
    typedef bool ( *CompareFunc )( const Geometry *, const Geometry * );
    static CompareFunc compareFunc[]{
        compareX, compareY, compareZ,
    };

    std::sort( geometries.begin(), geometries.end(), compareFunc[ depth % 3 ] );

    //---split into children nodes---
    int size( geometries.size() );
    int mid_size( size >> 1 );

    BVHNode *pLeft( pNode->left_child = new BVHNode() );
    BVHNode *pRight( pNode->right_child = new BVHNode() );

    pLeft->pGeometries = new QList< Geometry * >( geometries.mid( 0, mid_size ) );
    pRight->pGeometries = new QList< Geometry * >( geometries.mid( mid_size, size ) );

    pLeft->pBBox = NULL;
    pRight->pBBox = NULL;

    //---build tree recurrently---
    build( *pLeft->pGeometries, pLeft, depth + 1 );
    build( *pRight->pGeometries, pRight, depth + 1 );

}
