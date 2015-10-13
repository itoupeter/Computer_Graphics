#include "sphere.h"

#include <iostream>

#include <la.h>

static const int SPH_IDX_COUNT = 2280;  // 760 tris * 3
static const int SPH_VERT_COUNT = 382;

void Sphere::computeBounds(){

    glm::vec4 vertices[]{
        { .5f, .5f, .5f, 1.f }, { .5f, .5f, -.5f, 1.f },
        { .5f, -.5f, .5f, 1.f }, { .5f, .5f, -.5f, 1.f },
        { -.5f, .5f, .5f, 1.f }, { -.5f, .5f, -.5f, 1.f },
        { -.5f, -.5f, .5f, 1.f }, { -.5f, .5f, -.5f, 1.f },
    };

    glm::vec3 vertices_in_world[ 8 ];
    glm::vec3 max_bound( -1e6f );
    glm::vec3 min_bound( 1e6f );

    for( int i = 0; i < 8; ++i ){
        vertices_in_world[ i ] = glm::vec3( transform.T() * vertices[ i ] );
        max_bound = glm::max( max_bound, vertices_in_world[ i ] );
        min_bound = glm::min( min_bound, vertices_in_world[ i ] );
    }

    pBBox = new BoundingBox( max_bound, min_bound );

}

glm::vec2 Sphere::GetUVCoordinates( const glm::vec3 &point ){

    float phi( atan2f( point[ 2 ] , point[ 0 ] ) );
    float theta( acosf( 2.f * point[ 1 ] ) );

    if( phi < 0.f ) phi += TWO_PI;

    return glm::vec2( 1 - phi / TWO_PI, 1 - theta / PI );
}

Intersection Sphere::GetIntersection(Ray r)
{
    //---Q5---
    //TODO
    Ray rInWorld( r );
    r = r.GetTransformedCopy( transform.invT() );

    float A = glm::length2( r.direction );
    float B = 2.f * glm::dot( r.origin, r.direction );
    float C = glm::length2( r.origin ) - .25f;
    float D = B * B - 4.f * A * C;

    if( D < 0.f ) return Intersection();

    float t0 = ( -B - glm::sqrt( D ) ) * .5f / A;
    float t1 = ( -B + glm::sqrt( D ) ) * .5f / A;

    if( t0 >= 0.f ){

        Intersection result;
        glm::vec3 pointInLocal = r.origin + t0 * r.direction;

        result.point = glm::vec3( transform.T() * glm::vec4( pointInLocal, 1.f ) );
        result.normal = glm::normalize( glm::vec3( transform.invTransT() * glm::vec4( pointInLocal, 0.f ) ) );
        result.t = glm::distance( result.point, rInWorld.origin );
        result.object_hit = this;
        result.color = material->GetImageColor( GetUVCoordinates( pointInLocal ), material->texture ) * material->base_color;

        return result;

    }else if( t1 >= 0.f ){

        Intersection result;
        glm::vec3 pointInLocal = r.origin + t1 * r.direction;

        result.point = glm::vec3( transform.T() * glm::vec4( pointInLocal, 1.f ) );
        result.normal = glm::normalize( glm::vec3( transform.invTransT() * glm::vec4( pointInLocal, 0.f ) ) );
        result.t = glm::distance( result.point, rInWorld.origin );
        result.object_hit = this;
        result.color = material->GetImageColor( GetUVCoordinates( pointInLocal ), material->texture ) * material->base_color;

        return result;

    }

    return Intersection();

}

// These are functions that are only defined in this cpp file. They're used for organizational purposes
// when filling the arrays used to hold the vertex and index data.
void createSphereVertexPositions(glm::vec3 (&sph_vert_pos)[SPH_VERT_COUNT])
{
    // Create rings of vertices for the non-pole vertices
    // These will fill indices 1 - 380. Indices 0 and 381 will be filled by the two pole vertices.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 0.5f, 0, 1);
            sph_vert_pos[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole vertices
    sph_vert_pos[0] = glm::vec3(0, 0.5f, 0);
    sph_vert_pos[381] = glm::vec3(0, -0.5f, 0);  // 361 - 380 are the vertices for the bottom cap
}

void createSphereVertexNormals(glm::vec3 (&sph_vert_nor)[SPH_VERT_COUNT])
{
    // Unlike a cylinder, a sphere only needs to be one normal per vertex
    // because a sphere does not have sharp edges.
    glm::vec4 v;
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            v = glm::rotate(glm::mat4(1.0f), j / 20.0f * TWO_PI, glm::vec3(0, 1, 0))
                * glm::rotate(glm::mat4(1.0f), -i / 18.0f * PI, glm::vec3(0, 0, 1))
                * glm::vec4(0, 1.0f, 0, 0);
            sph_vert_nor[(i - 1) * 20 + j + 1] = glm::vec3(v);
        }
    }
    // Add the pole normals
    sph_vert_nor[0] = glm::vec3(0, 1.0f, 0);
    sph_vert_nor[381] = glm::vec3(0, -1.0f, 0);
}

void createSphereIndices(GLuint (&sph_idx)[SPH_IDX_COUNT])
{
    int index = 0;
    // Build indices for the top cap (20 tris, indices 0 - 60, up to vertex 20)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 0;
        sph_idx[index + 1] = i + 1;
        sph_idx[index + 2] = i + 2;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[57] = 0;
    sph_idx[58] = 20;
    sph_idx[59] = 1;
    index += 3;

    // Build indices for the body vertices
    // i is the Z axis rotation
    for (int i = 1; i < 19; i++) {
        // j is the Y axis rotation
        for (int j = 0; j < 20; j++) {
            sph_idx[index] = (i - 1) * 20 + j + 1;
            sph_idx[index + 1] = (i - 1) * 20 +  j + 2;
            sph_idx[index + 2] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 3] = (i - 1) * 20 +  j + 1;
            sph_idx[index + 4] = (i - 1) * 20 +  j + 22;
            sph_idx[index + 5] = (i - 1) * 20 +  j + 21;
            index += 6;
        }
    }

    // Build indices for the bottom cap (20 tris, indices 2220 - 2279)
    for (int i = 0; i < 19; i++) {
        sph_idx[index] = 381;
        sph_idx[index + 1] = i + 361;
        sph_idx[index + 2] = i + 362;
        index += 3;
    }
    // Must create the last triangle separately because its indices loop
    sph_idx[2277] = 381;
    sph_idx[2278] = 380;
    sph_idx[2279] = 361;
    index += 3;
}

void Sphere::create()
{
    GLuint sph_idx[SPH_IDX_COUNT];
    glm::vec3 sph_vert_pos[SPH_VERT_COUNT];
    glm::vec3 sph_vert_nor[SPH_VERT_COUNT];
    glm::vec3 sph_vert_col[SPH_VERT_COUNT];

    createSphereVertexPositions(sph_vert_pos);
    createSphereVertexNormals(sph_vert_nor);
    createSphereIndices(sph_idx);
    for (int i = 0; i < SPH_VERT_COUNT; i++) {
        sph_vert_col[i] = material->base_color;
    }

    count = SPH_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(sph_idx, SPH_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(sph_vert_pos, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(sph_vert_col, SPH_VERT_COUNT * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(sph_vert_nor, SPH_VERT_COUNT * sizeof(glm::vec3));
}
