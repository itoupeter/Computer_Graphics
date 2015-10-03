#include <raytracing/integrator.h>


Integrator::Integrator():
    max_depth(5)
{
    scene = NULL;
    intersection_engine = NULL;
}

//Basic ray trace
glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth)
{

    static const glm::vec3 black( 0.f, 0.f, 0.f );
    static const glm::vec3 white( 1.f, 1.f, 1.f );

    //---exceed recurrence depth---
    if( depth == max_depth ) return black;

    //---compute intersection---
    Intersection isx( intersection_engine->GetIntersection( r ) );

    return isx.color;

    return black;
}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}
