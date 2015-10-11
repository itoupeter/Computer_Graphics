
#include "boundingbox.h"
#include "geometry/geometry.h"

BoundingBox::BoundingBox():
    max_bound( glm::vec3( 0.f ) ),
    min_bound( glm::vec3( 0.f ) ),
    center( glm::vec3( 0.f ) ){
}

BoundingBox::BoundingBox(const glm::vec3 &_max_bound, const glm::vec3 &_min_bound ):
    max_bound( _max_bound ),
    min_bound( _min_bound ),
    center( .5f * ( _max_bound + _min_bound ) ){
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

 /*
BoundingBox BoundingBox::combine( const QList< Triangle * > &triangles ){

    glm::vec3 max( -1e6f );
    glm::vec3 min( 1e6f );

    for( Triangle *triangle : triangles ){
        max = glm::max( max, triangle->pBBox->max_bound );
        min = glm::min( min, triangle->pBBox->min_bound );
    }

    BoundingBox result;

    result.setBounds( max, min );

    return result;
}
*/

//---UI---
void BoundingBox::create(){

    float x0( max_bound.x ), y0( max_bound.y ), z0( max_bound.z );
    float x1( min_bound.x ), y1( min_bound.y ), z1( min_bound.z );

    glm::vec3 positions[]{
        { x0, y0, z0 }, { x0, y0, z1 }, { x0, y1, z0 }, { x0, y1, z1 },
        { x1, y0, z0 }, { x1, y0, z1 }, { x1, y1, z0 }, { x1, y1, z1 },
    };

    const glm::vec3 purple( 1.f, 0.f, 1.f );

    const int indices[]{
        0, 1, 0, 2, 1, 3, 2, 3,
        4, 5, 4, 6, 5, 7, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7,
    };

    std::vector< glm::vec3 > position;
    std::vector< glm::vec3 > color;
    std::vector< GLuint > index;

    for( int i = 0; i < 8; ++i ){
        position.push_back( positions[ i ] );
        color.push_back( purple );
    }

    for( int i = 0; i < 24; ++i ){
        index.push_back( indices[ i ] );
    }

    count = index.size();

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern( QOpenGLBuffer::StaticDraw );
    bufIdx.allocate( index.data(), count * sizeof( GLuint ) );

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern( QOpenGLBuffer::StaticDraw );
    bufPos.allocate( position.data(), position.size() * sizeof( glm::vec3 ) );

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern( QOpenGLBuffer::StaticDraw );
    bufCol.allocate( color.data(), color.size() * sizeof( glm::vec3 ) );
}

GLenum BoundingBox::drawMode(){
    return GL_LINES;
}