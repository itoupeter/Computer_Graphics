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
    SAH(){}

    void split( QList< Geometry * > &geometries,
                QList< Geometry * > &lGeometries,
                QList< Geometry * > &rGeometries,
                int dim, BoundingBox *whole );

private:
    static const int nBuckets = 12;

    SAHBucket buckets[ nBuckets ];
    std::vector< float > costs;
};

#endif // SAH_H
