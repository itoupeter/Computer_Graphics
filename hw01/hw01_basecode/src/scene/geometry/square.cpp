#include <scene/geometry/square.h>

Intersection SquarePlane::GetIntersection(Ray r)
{
    //---Q5---
    //TODO
    Ray rInWorld = r;
    Intersection result;

    r = r.GetTransformedCopy( transform.invT() );
    //result.t = dot( N, ( 0 - r.origin ) ) / dot( N, r.direction );
    result.t = -r.origin[ 2 ] / r.direction[ 2 ];
    result.point = r.origin + result.t * r.direction;

    if( glm::abs( result.point[ 0 ] ) > .5f ) return Intersection();
    if( glm::abs( result.point[ 1 ] ) > .5f ) return Intersection();

    result.point = glm::vec3( transform.T() * glm::vec4( result.point, 1.f ) );
    result.normal = glm::vec3( transform.invTransT() * glm::vec4( 0.f, 0.f, 1.f, 0.f ) );
    result.t = glm::length( result.point - rInWorld.origin );
    result.object_hit = this;

    return result;
}

void SquarePlane::create()
{
    GLuint cub_idx[6];
    glm::vec3 cub_vert_pos[4];
    glm::vec3 cub_vert_nor[4];
    glm::vec3 cub_vert_col[4];

    cub_vert_pos[0] = glm::vec3(-0.5f, 0.5f, 0);  cub_vert_nor[0] = glm::vec3(0, 0, 1); cub_vert_col[0] = material->base_color;
    cub_vert_pos[1] = glm::vec3(-0.5f, -0.5f, 0); cub_vert_nor[1] = glm::vec3(0, 0, 1); cub_vert_col[1] = material->base_color;
    cub_vert_pos[2] = glm::vec3(0.5f, -0.5f, 0);  cub_vert_nor[2] = glm::vec3(0, 0, 1); cub_vert_col[2] = material->base_color;
    cub_vert_pos[3] = glm::vec3(0.5f, 0.5f, 0);   cub_vert_nor[3] = glm::vec3(0, 0, 1); cub_vert_col[3] = material->base_color;

    cub_idx[0] = 0; cub_idx[1] = 1; cub_idx[2] = 2;
    cub_idx[3] = 0; cub_idx[4] = 2; cub_idx[5] = 3;

    count = 6;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(cub_idx, 6 * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(cub_vert_pos, 4 * sizeof(glm::vec3));

    bufNor.create();
    bufNor.bind();
    bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufNor.allocate(cub_vert_nor, 4 * sizeof(glm::vec3));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(cub_vert_col, 4 * sizeof(glm::vec3));
}
