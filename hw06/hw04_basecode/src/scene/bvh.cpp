
#include "scene.h"
#include "scene/bvh.h"
#include "scene/sah.h"
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
    if( depth < 5 ){
        pNode->pBBox->create();
        Scene::allBBoxes.push_back( pNode->pBBox );
    }

    //---reach leaf node---
    if( geometries.size() == 1 ){
        pNode->pGeometry = geometries.front();
        return pNode;
    }

    QList< Geometry * > lGeometries;
    QList< Geometry * > rGeometries;

    if( geometries.size() <= 4 ){
        //---use geometry count---
        //---sort geometries according to X/Y/Z axis---
        typedef bool ( *CompareFunc )( const Geometry *, const Geometry * );
        static CompareFunc compareFunc[]{
            compareX, compareY, compareZ,
        };

        std::sort( geometries.begin(), geometries.end(), compareFunc[ depth % 3 ] );

        //---split into children nodes---
        int size( geometries.size() );
        int mid_size( size >> 1 );

        lGeometries = QList< Geometry * >( geometries.mid( 0, mid_size ) );
        rGeometries = QList< Geometry * >( geometries.mid( mid_size, size ) );
    }else{
        //---use SAH---
        SAH sah;

        sah.split( geometries, lGeometries, rGeometries, depth % 3, pNode->pBBox );
    }

    //---build tree recurrently---
    pNode->pLeft = build( lGeometries, pNode->pLeft, depth + 1 );
    pNode->pRight = build( rGeometries, pNode->pRight, depth + 1 );

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
