
#include "scene/sah.h"

void SAH::split(
        QList<Geometry *> &geometries,
        QList<Geometry *> &lGeometries,
        QList<Geometry *> &rGeometries,
        int dim, BoundingBox *whole ){

    float left_wall( 1e5f ), right_wall( -1e5f );

    for( Geometry *pGeometries : geometries ){
        left_wall = glm::min( left_wall, pGeometries->pBBox->center[ dim ] );
        right_wall = glm::max( right_wall, pGeometries->pBBox->center[ dim ] );
    }

    left_wall -= .1f;
    right_wall += .1f;

    //---classify---
    for( Geometry *pGeometry : geometries ){

        int index( ( pGeometry->pBBox->center[ dim ] - left_wall )
                   / ( right_wall - left_wall ) * nBuckets );

        ++buckets[ index ].count;
        buckets[ index ].bound = BoundingBox::combine( buckets[ index ].bound, *pGeometry->pBBox );
    }

    //---compute cost---
    costs.clear();
    for( int i = 0; i < nBuckets - 1; ++i ){

        BoundingBox b0, b1;
        int count0( 0 ), count1( 0 );

        for( int j = 0; j <= i; ++j ){
            b0 = BoundingBox::combine( b0, buckets[ j ].bound );
            count0 += buckets[ j ].count;
        }

        for( int j = i + 1; j < nBuckets; ++j ){
            b1 = BoundingBox::combine( b1, buckets[ j ].bound );
            count1 += buckets[ j ].count;
        }

        costs.push_back( .125f + ( count0 * b0.area() + count1 * b1.area() ) / whole->area() );
    }

    //---find minimal cost split---
    float minCost( costs[ 0 ] );
    int minCostSplit( 0 );

    for( int i = 1; i < nBuckets - 1; ++i ){
        if( minCost > costs[ i ] ){
            minCost = costs[ i ];
            minCostSplit = i;
        }
    }

    //---split---
    for( Geometry *pGeometry : geometries ){

        int index( ( pGeometry->pBBox->center[ dim ] - left_wall )
                   / ( right_wall - left_wall ) * nBuckets );

        if( index <= minCostSplit ){
            lGeometries.push_back( pGeometry );
        }else{
            rGeometries.push_back( pGeometry );
        }
    }
}
