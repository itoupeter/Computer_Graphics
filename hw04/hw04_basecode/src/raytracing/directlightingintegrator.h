#ifndef DIRECTLIGHTINGINTEGRATOR_H
#define DIRECTLIGHTINGINTEGRATOR_H

#include "raytracing/integrator.h"
#include <random>

class DirectLightingIntegrator : Integrator{
public:
    DirectLightingIntegrator( Scene *scene, IntersectionEngine *intersection_engine );

    glm::vec3 ShadowTest( const glm::vec3 &o, const glm::vec3 &d, const Geometry *pLight );
    glm::vec3 TraceRay( Ray r );

private:
    std::uniform_real_distribution< float > distribution;
    std::mt19937 generator;

    //---number of sample per light---
    static const int N;
};

#endif // DIRECTLIGHTINGINTEGRATOR_H
