
#include "scene/bvh.h"
#include <algorithm>
#include <QMessageBox>

Scene *BVH::scene = NULL;

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
    pNode = new BVHNode();
    pNode->pGeometries = &geometries;
    pNode->pBBox = new BoundingBox( BoundingBox::combine( geometries ) );
    pNode->pBBox->create();
    scene->allBBoxes.push_back( pNode->pBBox );

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

    QList< Geometry * > lGeometries( geometries.mid( 0, mid_size ) );
    QList< Geometry * > rGeometries( geometries.mid( mid_size, size ) );

    //---build tree recurrently---
    build( lGeometries, pNode->left_child, depth + 1 );
    build( rGeometries, pNode->right_child, depth + 1 );

}

void BVH::clear( BVHNode *pNode ){

    //---empty node---
    if( pNode == NULL ) return;

    //---clear nodes recurrently---
    clear( pNode->left_child );
    clear( pNode->right_child );

    //---clear current node---
    delete pNode->pBBox;
    delete pNode;
}
