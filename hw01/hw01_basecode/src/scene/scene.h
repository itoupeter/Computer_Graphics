#pragma once
#include <QList>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <raytracing/samplers/pixelsampler.h>
#include <scene/geometry/geometry.h>
#include "scene/boundingbox.h"

class Geometry;
class Material;
class BoundingBox;

class Scene
{
public:
    Scene();
    QList<Geometry*> objects;
    QList<Material*> materials;
    QList<Geometry*> lights;
    Camera camera;
    Film film;
    PixelSampler* pixel_sampler;

    //---all bounding boxes---
    QList< BoundingBox * > allBBoxes;

    void SetCamera(const Camera &c);

    void CreateTestScene();
    void Clear();
};
