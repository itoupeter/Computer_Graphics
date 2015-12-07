#pragma once
#include <QList>
#include <raytracing/ray.h>
#include <scene/scene.h>
#include <scene/bvhnode.h>

class Scene;
class Ray;


class IntersectionEngine
{
public:
    IntersectionEngine();
    Scene *scene;

    //---BVH root---
    BVHNode* root;

    Intersection GetIntersection(Ray r);
    QList<Intersection> GetAllIntersections(Ray r);
};
