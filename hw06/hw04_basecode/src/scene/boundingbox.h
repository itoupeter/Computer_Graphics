#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "openGL/drawable.h"
#include "scene/geometry/geometry.h"

class Geometry;
class Triangle;

class BoundingBox : public Drawable{
public:
    BoundingBox();
    BoundingBox( const glm::vec3 &max_bound, const glm::vec3 &min_bound );

    void setBounds( const glm::vec3 &max_bound, const glm::vec3 &min_bound );

    bool getIntersection( const Ray &r );

    static BoundingBox combine( const BoundingBox &a, const BoundingBox &b );
    static BoundingBox combine( const QList< Geometry * > &geometries );
    static BoundingBox combine( const QList< Triangle * > &triangles );
//    static BoundingBox mesh_combine( const QList< Geometry * > &geometries );

    glm::vec3 max_bound;
    glm::vec3 min_bound;
    glm::vec3 center;

    //---derived from Drawable---
    virtual void create();
    virtual GLenum drawMode();
};

#endif // BOUNDINGBOX_H
