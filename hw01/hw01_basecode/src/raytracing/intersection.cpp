#include <raytracing/intersection.h>

Intersection::Intersection():
    color( glm::vec3( 1.f ) ),
    point( glm::vec3( 0 ) ),
    normal( glm::vec3( 0 ) ),
    t( -1 )
{
    object_hit = NULL;
}

IntersectionEngine::IntersectionEngine()
{
    scene = NULL;
    root = NULL;
}

Intersection IntersectionEngine::GetIntersection(Ray r)
{
    //---Q6---
    //TODO

#define USE_BVH
#ifdef USE_BVH
    //---use BVH---
    return root->getIntersection( r );
#else
    float min_t = 1e6;
    Intersection intersection, result;

    for( QList< Geometry* >::iterator i = scene->objects.begin(); i != scene->objects.end(); ++i ){

        intersection = ( *i )->GetIntersection( r );

        if( intersection.object_hit != NULL && min_t > intersection.t ){
            min_t = intersection.t;
            result = intersection;
        }

    }

    return result;
#endif

}
