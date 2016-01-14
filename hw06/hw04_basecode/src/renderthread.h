#pragma once

#include <QThread>
#include <QMutex>
#include <raytracing/film.h>
#include <scene/scene.h>
#include <raytracing/integrator.h>

glm::vec3 ComponentMult(const glm::vec3 &a, const glm::vec3 &b);

class RenderThread : public QThread
{
public:
    RenderThread(
            unsigned int samplesSqrt, unsigned int depth,
            Film* f, Camera* c, Integrator* i);

    //---pixel coordinates---
    static QList< int > pixel_coords;

    //---mutex---
    static QMutex mutex;

protected:
    //This overrides the functionality of QThread::run
    virtual void run();
    glm::vec3 TraceRay(Ray r, unsigned int depth);// IntersectionEngine* intersection_engine, Scene* scene);



    unsigned int samples_sqrt;//The square root of the number of rays to cast per pixel
    unsigned int max_depth;
    Film* film;
    Camera* camera;
    Integrator* integrator;
};
