#ifndef BIDIRECTIONALPATHTRACINGINTEGRATOR_H
#define BIDIRECTIONALPATHTRACINGINTEGRATOR_H

#include <vector>
using std::vector;

#include "scene/geometry/geometry.h"
#include "raytracing/integrator.h"

class BidirectionalPathTracingIntegrator : public Integrator{
public:
    BidirectionalPathTracingIntegrator( Scene *scene, IntersectionEngine *intersection_engine, int max_depth );

    glm::vec3 TraceRay( Ray r, unsigned int depth );

    void GeneratePath( Ray ray, Geometry *pLight );

    //---test if b is visible to a---
    bool Visible( const Intersection &a, const Intersection &b );

    vector< Intersection > vertices_eye;
    vector< glm::vec3 > Ld_eye;
    vector< Intersection > vertices_light;
    vector< glm::vec3 > Ld_light;
};

#endif // BIDIRECTIONALPATHTRACINGINTEGRATOR_H

