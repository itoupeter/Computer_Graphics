#ifndef BIDIRECTIONALPATHTRACINGINTEGRATOR_H
#define BIDIRECTIONALPATHTRACINGINTEGRATOR_H

#include <vector>
using std::vector;

#include "scene/geometry/geometry.h"
#include "raytracing/integrator.h"
#include "raytracing/bidirectionalpath.h"

class BidirectionalPathTracingIntegrator : public Integrator{
public:
    BidirectionalPathTracingIntegrator( Scene *scene, IntersectionEngine *intersection_engine, int max_depth );

    glm::vec3 TraceRay( Ray r, unsigned int depth );

    //---generate paths from eye and light---
    void GeneratePath(
            Ray ray, Geometry *pLight,
            vector< Intersection > &vertices_eye, vector< float > &pdf_eye,
            vector< Intersection > &vertices_light, vector< float > &pdf_light );

    void GeneratePath( Ray ray, Geometry *pLight, BidirectionalPath &path );

    //---evaluate energy along path---
    glm::vec3 EvaluatePath(
            Ray ray, Geometry *pLight, int I, int J,
            vector< Intersection > &vertices_eye, vector< float > &pdf_eye,
            vector< Intersection > &vertices_light, vector< float > &pdf_light );

    glm::vec3 EvaluatePath( Ray ray, Geometry *pLight, int I, int J, BidirectionalPath &path );

    //---test if b is visible to a---
    bool Visible( const Intersection &a, const Intersection &b );

    //---sample on light---
    Intersection light_sample;
};

#endif // BIDIRECTIONALPATHTRACINGINTEGRATOR_H

