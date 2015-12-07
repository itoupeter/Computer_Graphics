
#include "scene/bvh.h"
#include "scene/geometry/mesh.h"
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

BVHNode *BVH::build( QList< Geometry * > &geometries, BVHNode *pNode, int depth ){

    //---no geometry---
    if( geometries.size() == 0 ){
        QMessageBox msg;
        msg.setText( "No geometry in the scene." );
        msg.exec();
        return pNode;
    }

    //---current node---
    pNode = new BVHNode();
    pNode->pBBox = new BoundingBox( BoundingBox::combine( geometries ) );
    pNode->pBBox->create();
    BVH::scene->allBBoxes.push_back( pNode->pBBox );

    //---reach leaf node---
    if( geometries.size() == 1 ){
        pNode->pGeometry = geometries.front();
        return pNode;
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

    QList< Geometry * > *lGeometries = new QList< Geometry * >( geometries.mid( 0, mid_size ) );
    QList< Geometry * > *rGeometries = new QList< Geometry * >( geometries.mid( mid_size, size ) );

    //---build tree recurrently---
    pNode->pLeft = build( *lGeometries, pNode->pLeft, depth + 1 );
    pNode->pRight = build( *rGeometries, pNode->pRight, depth + 1 );

    return pNode;
}

bool mesh_compareX( const Geometry *a, const Geometry *b ){

    const Triangle *c( static_cast< const Triangle * >( a ) );
    const Triangle *d( static_cast< const Triangle * >( b ) );

    return c->pBBoxInLocal->center[ 0 ] < d->pBBoxInLocal->center[ 0 ];
}

bool mesh_compareY( const Geometry *a, const Geometry *b ){

    const Triangle *c( static_cast< const Triangle * >( a ) );
    const Triangle *d( static_cast< const Triangle * >( b ) );

    return c->pBBoxInLocal->center[ 1 ] < d->pBBoxInLocal->center[ 1 ];
}

bool mesh_compareZ( const Geometry *a, const Geometry *b ){

    const Triangle *c( static_cast< const Triangle * >( a ) );
    const Triangle *d( static_cast< const Triangle * >( b ) );

    return c->pBBoxInLocal->center[ 2 ] < d->pBBoxInLocal->center[ 2 ];
}

BVHNode *BVH::mesh_build( QList< Geometry * > &geometries, BVHNode *pNode, int depth ){

    //---no geometry---
    if( geometries.size() == 0 ){
        QMessageBox msg;
        msg.setText( "No triangle in the mesh." );
        msg.exec();
        return pNode;
    }

    //---current node---
    pNode = new BVHNode();
    //---bounding box for visualization---
    pNode->pBBox = new BoundingBox( BoundingBox::mesh_combine( geometries ) );
    //---bounding box for intersection test---
    BoundingBox *pBBox( new BoundingBox( BoundingBox::combine( geometries ) ) );
    if( /*true ||*/ depth < 8 ){
        pBBox->create();
        Mesh::allBBoxes.push_back( pBBox );
    }

    //---reach leaf node---
    if( geometries.size() == 1 ){
        pNode->pGeometry = geometries.front();
        return pNode;
    }

    //---sort geometries according to X/Y/Z axis---
    typedef bool ( *CompareFunc )( const Geometry *, const Geometry * );
    static CompareFunc compareFunc[]{
        mesh_compareX, mesh_compareY, mesh_compareZ,
    };

    std::sort( geometries.begin(), geometries.end(), compareFunc[ depth % 3 ] );

    //---split into children nodes---
    int size( geometries.size() );
    int mid_size( size >> 1 );

    QList< Geometry * > *lGeometries = new QList< Geometry * >( geometries.mid( 0, mid_size ) );
    QList< Geometry * > *rGeometries = new QList< Geometry * >( geometries.mid( mid_size, size ) );

    //---build tree recurrently---
    pNode->pLeft = BVH::mesh_build( *lGeometries, pNode->pLeft, depth + 1 );
    pNode->pRight = BVH::mesh_build( *rGeometries, pNode->pRight, depth + 1 );

    return pNode;
}

void BVH::clear( BVHNode *pNode ){

    //---empty node---
    if( pNode == NULL ) return;

    //---clear tree nodes recurrently---
    clear( pNode->pLeft );
    clear( pNode->pRight );

    //---clear current node---
    delete pNode->pBBox;
    delete pNode;
}
