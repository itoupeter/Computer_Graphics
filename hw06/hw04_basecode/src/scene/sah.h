#ifndef SAH_H
#define SAH_H

#include "scene/boundingbox.h"
#include "scene/geometry/geometry.h"

struct SAHBucket{
    SAHBucket():count( 0 ){}
    int count;
    BoundingBox bound;
};

class SAH{
public:
    SAH();

    void split( QList< Geometry * > &geometries,
                QList< Geometry * > &lGeometries,
                QList< Geometry * > &rGeometries,
                int dim );

    void initializeBucket();

private:
    const int nBuckets = 12;

    SAHBucket buckets[ nBuckets ];
};

void SAH::split(
        QList<Geometry *> &geometries,
        QList<Geometry *> &lGeometries,
        QList<Geometry *> &rGeometries,
        int dim ){

    int nGeometries( geometries.size() );
    float left_wall( 1e5f ), right_wall( 0.f );

    for( int i = 0; i < nGeometries; ++i ){
        if( )
    }

}
#endif // SAH_H

