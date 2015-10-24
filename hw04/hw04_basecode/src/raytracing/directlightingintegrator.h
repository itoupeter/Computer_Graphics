#ifndef DIRECTLIGHTINGINTEGRATOR_H
#define DIRECTLIGHTINGINTEGRATOR_H

#include "raytracing/integrator.h"

class DirectLightingIntegrator : Integrator{
public:
    glm::vec3 TraceRay( Ray r, unsigned int depth );
};

#endif // DIRECTLIGHTINGINTEGRATOR_H
