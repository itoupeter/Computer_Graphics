#ifndef DIRECTLIGHTINGINTEGRATOR_H
#define DIRECTLIGHTINGINTEGRATOR_H

#include "raytracing/integrator.h"

class DirectLightingIntegrator : Integrator{
public:
    DirectLightingIntegrator( Scene *scene, IntersectionEngine *intersection_engine, int max_depth );

    glm::vec3 ShadowTest( const glm::vec3 &o, const glm::vec3 &d, const Geometry *pLight );
    glm::vec3 TraceRay( Ray r, unsigned int depth );

private:
    //---number of sample per light---
    static const int N;
};

#endif // DIRECTLIGHTINGINTEGRATOR_H
