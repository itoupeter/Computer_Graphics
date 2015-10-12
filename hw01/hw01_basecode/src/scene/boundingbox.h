#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "openGL/drawable.h"
#include "scene/geometry/geometry.h"

class Geometry;

class BoundingBox : public Drawable{
public:
    BoundingBox();
    BoundingBox( const glm::vec3 &max_bound, const glm::vec3 &min_bound );

    void setBounds( const glm::vec3 &max_bound, const glm::vec3 &min_bound );

    bool getIntersection( Ray &ray );

    static BoundingBox combine( const BoundingBox &a, const BoundingBox &b );
    static BoundingBox combine( const QList< Geometry * > &geometries );

    glm::vec3 max_bound;
    glm::vec3 min_bound;
    glm::vec3 center;

    //---derived from Drawable---
    virtual void create();
    virtual GLenum drawMode();
};

#endif // BOUNDINGBOX_H
