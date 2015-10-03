#include "cube.h"
#include <la.h>
#include <iostream>

static const int CUB_IDX_COUNT = 36;
static const int CUB_VERT_COUNT = 24;

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point){

    static const float a[]{
        point[ 0 ], point[ 0 ], point[ 1 ], point[ 1 ], point[ 2 ], point[ 2 ],
    };

    static const float b[]{
        -.5f, .5f, -.5f, .5f, -.5f, .5f,
    };

    int hit_face;

    for( hit_face = 0; hit_face < 6 && !fequal( a[ hit_face ], b[ hit_face ] ); ++hit_face );

    assert( hit_face < 6 );

    float x, y;

    switch( hit_face ){
    case 0:
       x = ( point[ 1 ] + .5f ) / 3.f + 1.f / 3.f;
       y = ( point[ 2 ] + .5f ) / 4.f ;
       break;
    case 1:
       x = ( point[ 1 ] + .5f ) / 3.f + 1.f / 3.f;
       y = ( .5f - point[ 2 ] ) / 4.f + 2.f / 4.f;
       break;
    case 2:
       x = ( .5f - point[ 2 ] ) / 3.f + 2.f / 3.f;
       y = ( point[ 0 ] + .5f ) / 4.f + 1.f / 4.f;
       break;
    case 3:
       x = ( point[ 2 ] + .5f ) / 3.f;
       y = ( point[ 0 ] + .5f ) / 4.f + 1.f / 4.f;
       break;
    case 4:
       x = ( point[ 2 ] + .5f ) / 3.f;
       y = ( point[ 0 ] + .5f ) / 4.f + 1.f / 4.f;
       break;
    case 5:
       x = ( point[ 1 ] + .5f ) / 3.f + 1.f / 3.f;
       y = ( .5f - point[ 0 ] ) / 4.f + 3.f / 4.f;
       break;
    }

    return glm::vec2( x, y );
}

Intersection Cube::GetIntersection(Ray r)
{
    //---Q5---
    //TODO
    Ray rInWorld( r );
    r = r.GetTransformedCopy( transform.invT() );

    static const float EPS = 1e-5;
    float t_near = -1e6;
    float t_far = 1e6;

    for( int i = 0; i < 3; ++i ){

        if( glm::abs( r.direction[ i ] ) < EPS ){
            if( r.origin[ i ] - EPS < -.5f || r.origin[ i ] + EPS > .5f ){
                return Intersection();
            }else{
                continue;
            }
        }

        float t0, t1;

        t0 = ( -.5f - r.origin[ i ] ) / r.direction[ i ];
        t1 = ( .5f - r.origin[ i ] ) / r.direction[ i ];

        if( t0 + EPS > t1 ){
            float tmp = t0;
            t0 = t1;
            t1 = tmp;
        }

        if( t0 + EPS > t_near ){
            t_near = t0;
        }

        if( t1 - EPS < t_far ){
            t_far = t1;
        }
    }

    if( t_near + EPS > t_far || t_near - EPS < 0.f ){
        return Intersection();
    }

    int hit_face = -1;
    float least_distance = 1e6;
    glm::vec3 hit = r.origin + t_near * r.direction;

    if( glm::abs( hit[ 0 ] + .5f ) < least_distance ){
        hit_face = 0;
        least_distance = glm::abs( hit[ 0 ] + .5f );
    }
    if( glm::abs( hit[ 0 ] - .5f ) < least_distance ){
        hit_face = 1;
        least_distance = glm::abs( hit[ 0 ] - .5f );
    }
    if( glm::abs( hit[ 1 ] + .5f ) < least_distance ){
        hit_face = 2;
        least_distance = glm::abs( hit[ 1 ] + .5f );
    }
    if( glm::abs( hit[ 1 ] - .5f ) < least_distance ){
        hit_face = 3;
        least_distance = glm::abs( hit[ 1 ] - .5f );
    }
    if( glm::abs( hit[ 2 ] + .5f ) < least_distance ){
        hit_face = 4;
        least_distance = glm::abs( hit[ 2 ] + .5f );
    }
    if( glm::abs( hit[ 2 ] - .5f ) < least_distance ){
        hit_face = 5;
        least_distance = glm::abs( hit[ 2 ] - .5f );
    }

    Intersection result;

    result.point = glm::vec3( transform.T() * glm::vec4( hit, 1.f ) );
    switch( hit_face ){
    case 0:
        result.normal = glm::vec3( -1, 0, 0 );
        break;
    case 1:
        result.normal = glm::vec3( 1, 0, 0 );
        break;
    case 2:
        result.normal = glm::vec3( 0, -1, 0 );
        break;
    case 3:
        result.normal = glm::vec3( 0, 1, 0 );
        break;
    case 4:
        result.normal = glm::vec3( 0, 0, -1 );
        break;
    case 5:
        result.normal = glm::vec3( 0, 0, 1 );
        break;
    }
    result.normal = glm::normalize( glm::vec3( transform.invTransT() * glm::vec4( result.normal, 0.f ) ) );
    result.t = glm::distance( result.point, rInWorld.origin );
    result.object_hit = this;
    result.color = material->GetImageColor( GetUVCoordinates( hit ), material->texture ) * material->base_color;

    return result;
}

//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createCubeVertexPositions(glm::vec3 (&cub_vert_pos)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);

    //Right face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);

    //Left face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Back face
    //UR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);

    //Top face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, -0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, 0.5f, 0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, 0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, 0.5f, -0.5f);

    //Bottom face
    //UR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, 0.5f);
    //LR
    cub_vert_pos[idx++] = glm::vec3(0.5f, -0.5f, -0.5f);
    //LL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, -0.5f);
    //UL
    cub_vert_pos[idx++] = glm::vec3(-0.5f, -0.5f, 0.5f);
}


void createCubeVertexNormals(glm::vec3 (&cub_vert_nor)[CUB_VERT_COUNT])
{
    int idx = 0;
    //Front
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,1);
    }
    //Right
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(1,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(-1,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,0,-1);
    }
    //Top
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,1,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        cub_vert_nor[idx++] = glm::vec3(0,-1,0);
    }
}

void createCubeIndices(GLuint (&cub_idx)[CUB_IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+1;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4;
        cub_idx[idx++] = i*4+2;
        cub_idx[idx++] = i*4+3;
    }
}

void Cube::create()
{
    GLuint cub_idx[CUB_IDX_COUNT];
    glm::vec3 cub_vert_pos[CUB_VERT_COUNT];
    glm::vec3 cub_vert_nor[CUB_VERT_COUNT];
    glm::vec3 cub_vert_col[CUB_VERT_COUNT];

    createCubeVertexPositions(cub_vert_pos);
    createCubeVertexNormals(cub_vert_nor);
    createCubeIndices(cub_idx);

    for(int i = 0; i < CUB_VERT_COUNT; i++){
        cub_vert_col[i] = material->base_color;
    }

    count = CUB_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, CUB_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos,CUB_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, CUB_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, CUB_VERT_COUNT * sizeof(glm::vec3));

}
