
#include "boundingbox.h"
#include "geometry/geometry.h"

BoundingBox::BoundingBox():
    max_bound( glm::vec3( 0.f ) ),
    min_bound( glm::vec3( 0.f ) ),
    center( glm::vec3( 0.f ) ){
}

void BoundingBox::setBounds( const glm::vec3 &max_bound, const glm::vec3 &min_bound ){
    this->max_bound = max_bound;
    this->min_bound = min_bound;
    this->center = .5f * ( max_bound + min_bound );
}

BoundingBox BoundingBox::combine( const BoundingBox &a, const BoundingBox &b ){

    BoundingBox result;

    result.setBounds( glm::max( a.max_bound, b.max_bound ),
                      glm::min( a.min_bound, b.min_bound ) );

    return result;
}

BoundingBox BoundingBox::combine( const QList< Geometry * > &geometries ){

    glm::vec3 max( -1e6f );
    glm::vec3 min( 1e6f );

    for( Geometry *geometry : geometries ){
        max = glm::max( max, geometry->pBBox->max_bound );
        min = glm::min( min, geometry->pBBox->min_bound );
    }

    BoundingBox result;

    result.setBounds( max, min );

    return result;
}

//BoundingBox BoundingBox::combine( const QList< Triangle * > &triangles ){

//    glm::vec3 max( -1e6f );
//    glm::vec3 min( 1e6f );

//    for( Triangle *triangle : triangles ){
//        max = glm::max( max, triangle->pBBox->max_bound );
//        min = glm::min( min, triangle->pBBox->min_bound );
//    }

//    BoundingBox result;

//    result.setBounds( max, min );

//    return result;
//}

//---UI---
void BoundingBox::create(){

}

GLenum BoundingBox::drawMode(){
    return GL_LINES;
}
