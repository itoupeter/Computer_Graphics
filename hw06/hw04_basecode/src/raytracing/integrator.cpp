#include <raytracing/integrator.h>
#include <raytracing/directlightingintegrator.h>
#include <raytracing/globallightingintegrator.h>
#include <raytracing/bidirectionalpathtracingintegrator.h>

Integrator::Integrator():
    max_depth(5),
    scene( NULL ),
    intersection_engine( NULL ){

}

Integrator::Integrator( Scene *s ):
    Integrator(){

    scene = s;
}

glm::vec3 ComponentMult( const glm::vec3 &a, const glm::vec3 &b ){
    return glm::vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}

void Integrator::SetDepth( unsigned int depth ){
    max_depth = depth;
}

glm::vec3 Integrator::TraceRay( Ray r, unsigned int depth ){

#define INTEGRATOR 1

#if INTEGRATOR == 0
    //---direct lighting---
    static DirectLightingIntegrator directLightingIntegrator( scene, intersection_engine, max_depth );
    return directLightingIntegrator.TraceRay( r, depth );
#elif INTEGRATOR == 1
    //---global lighting---
    static GlobalLightingIntegrator globalLightingIntegrator( scene, intersection_engine, max_depth );
    return globalLightingIntegrator.TraceRay( r, depth );
#elif INTEGRATOR == 2
    //---bidirectional path tracing---
    static BidirectionalPathTracingIntegrator bidirectionalPathTracingIntegrator( scene, intersection_engine, max_depth );
    return bidirectionalPathTracingIntegrator.TraceRay( r, depth );
#endif
}
