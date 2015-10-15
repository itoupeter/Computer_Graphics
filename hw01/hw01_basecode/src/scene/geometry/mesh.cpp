#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>
#include "scene/bvh.h"

void Triangle::computeBounds(){

    glm::vec3 max_bound( -1e6f );
    glm::vec3 min_bound( 1e6f );

    //---decide bounds---
    for( int i = 0; i < 3; ++i ){
        max_bound = glm::max( max_bound, points[ i ] );
        min_bound = glm::min( min_bound, points[ i ] );
    }

    pBBoxInLocal = new BoundingBox( max_bound, min_bound );

}

void Triangle::computeBoundsInWorld( const glm::mat4 &t ){

    glm::vec3 max_bound( -1e6f );
    glm::vec3 min_bound( 1e6f );

    //---transform vertices to the world space---
    glm::vec4 vertices_in_world[]{
        t * glm::vec4( points[ 0 ], 1.f ),
        t * glm::vec4( points[ 1 ], 1.f ),
        t * glm::vec4( points[ 2 ], 1.f ),
    };

    //---decide bounds---
    for( int i = 0; i < 3; ++i ){
        max_bound = glm::max( max_bound, glm::vec3( vertices_in_world[ i ] ) );
        min_bound = glm::min( min_bound, glm::vec3( vertices_in_world[ i ] ) );
    }

    pBBox = new BoundingBox( max_bound, min_bound );

}

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = plane_normal;
    }
}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0)){

}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    plane_normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}

//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
glm::vec3 Triangle::GetNormal(const glm::vec3 &position)
{
    float d0 = glm::distance(position, points[0]);
    float d1 = glm::distance(position, points[1]);
    float d2 = glm::distance(position, points[2]);
    return (normals[0] * d0 + normals[1] * d1 + normals[2] * d2)/(d0+d1+d2);
}

glm::vec4 Triangle::GetNormal(const glm::vec4 &position)
{
    glm::vec3 result = GetNormal(glm::vec3(position));
    return glm::vec4(result, 0);
}

glm::vec2 Triangle::GetUVCoordinates( const glm::vec3 &point ){

    glm::vec3 v01( points[ 1 ] - points[ 0 ] );
    glm::vec3 v02( points[ 2 ] - points[ 0 ] );

    glm::vec3 p0( points[ 0 ] - point );
    glm::vec3 p1( points[ 1 ] - point );
    glm::vec3 p2( points[ 2 ] - point );

    float s( glm::length( glm::cross( v01, v02 ) ) );
    float s0( glm::length( glm::cross( p1, p2 ) ) );
    float s1( glm::length( glm::cross( p0, p2 ) ) );
    float s2( glm::length( glm::cross( p0, p1 ) ) );

    return ( s0 * uvs[ 0 ] + s1 * uvs[ 1 ] + s2 * uvs[ 2 ] ) / s;
}

//HAVE THEM IMPLEMENT THIS
//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
Intersection Triangle::GetIntersection(Ray r)
{
    //---Q5---
    //TODO
    static const float EPS = 1e-4;

    glm::vec3 v01( points[ 1 ] - points[ 0 ] );
    glm::vec3 v02( points[ 2 ] - points[ 0 ] );

    float t = glm::dot( points[ 0 ] - r.origin, plane_normal )
            / glm::dot( r.direction, plane_normal );

    if( t - EPS < 0.f ) return Intersection();

    glm::vec3 p( r.origin + t * r.direction );
    glm::vec3 vp0( points[ 0 ] - p );
    glm::vec3 vp1( points[ 1 ] - p );
    glm::vec3 vp2( points[ 2 ] - p );

    float s = glm::length( glm::cross( v01, v02 ) );
    float s0 = glm::length( glm::cross( vp1, vp2 ) ) / s;
    float s1 = glm::length( glm::cross( vp0, vp2 ) ) / s;
    float s2 = glm::length( glm::cross( vp0, vp1 ) ) / s;

    if( glm::abs( s0 + s1 + s2 - 1.f ) > EPS ){
        return Intersection();
    }

    Intersection result;

    result.normal = glm::normalize( GetNormal( p ) );
    result.point = r.origin + t * r.direction;
    result.t = t;
    result.object_hit = this;
    result.color = material->GetImageColor( GetUVCoordinates( result.point ), material->texture )
            * material->base_color;

    return result;
}

QList< BoundingBox * > Mesh::allBBoxes;

Mesh::Mesh():
    root( NULL ){
}

Mesh::~Mesh(){
    BVH::clear( root );
}

void Mesh::computeBounds(){

    glm::vec3 max_bound( -1e6f );
    glm::vec3 min_bound( 1e6f );

    for( Triangle *triangle : faces ){

        //---transform vertices to the world space---
        glm::vec4 vertices_in_world[]{
            transform.T() * glm::vec4( triangle->points[ 0 ], 1.f ),
            transform.T() * glm::vec4( triangle->points[ 1 ], 1.f ),
            transform.T() * glm::vec4( triangle->points[ 2 ], 1.f ),
        };

        //---decide bounds---
        glm::vec3 max_bound_tri_w( -1e6f );
        glm::vec3 min_bound_tri_w( 1e6f );
        glm::vec3 max_bound_tri_l( -1e6f );
        glm::vec3 min_bound_tri_l( 1e6f );

        for( int i = 0; i < 3; ++i ){
            max_bound_tri_w = glm::max( max_bound_tri_w, glm::vec3( vertices_in_world[ i ] ) );
            min_bound_tri_w = glm::min( min_bound_tri_w, glm::vec3( vertices_in_world[ i ] ) );
            max_bound_tri_l = glm::max( max_bound_tri_l, glm::vec3( triangle->points[ i ] ) );
            min_bound_tri_l = glm::min( min_bound_tri_l, glm::vec3( triangle->points[ i ] ) );
        }

        //---cast Triangle* to Geometry* and save to pFaces---
        gFaces.push_back( static_cast< Geometry * >( triangle ) );

        //---save bounding box of current triangle in world space---
        triangle->pBBox = new BoundingBox( max_bound_tri_w, min_bound_tri_w );

        //---save bounding box of current triangle in mesh space---
        triangle->pBBoxInLocal = new BoundingBox( max_bound_tri_l, min_bound_tri_l );

        //---calculate mesh bounding box in world space---
        max_bound = glm::max( max_bound, max_bound_tri_w );
        min_bound = glm::min( min_bound, min_bound_tri_w );
    }

    pBBox = new BoundingBox( max_bound + glm::vec3( .1f ), min_bound - glm::vec3( .1f ) );

    //---build BVH for mesh---
    root = BVH::mesh_build( gFaces, root, 0 );
}

glm::vec2 Mesh::GetUVCoordinates( const glm::vec3 &point ){
    return glm::vec2( 0.f );
}

Intersection Mesh::GetIntersection(Ray r)
{
    //---Q5---
    //TODO
    Intersection result;
    Ray rInWorld( r );
    float min_t( 1e6 );

    //---ray in mesh space---
    r = r.GetTransformedCopy( transform.invT() );

#define USE_BVH
#ifdef USE_BVH
    result = root->getIntersection( r );
#else
    for( QList<Triangle*>::iterator i = faces.begin(); i != faces.end(); ++i ){

        Intersection intersection( ( *i )->GetIntersection( r ) );

        if( intersection.object_hit != NULL && min_t > intersection.t ){

            min_t = intersection.t;
            result = intersection;
        }

    }
#endif

    if( result.object_hit == NULL ) return result;

    result.point = glm::vec3( transform.T() * glm::vec4( result.point, 1.f ) );
    result.normal = glm::normalize( glm::vec3( transform.invTransT() * glm::vec4( result.normal, 0.f ) ) );
    result.t = glm::length( result.point - rInWorld.origin );
    result.object_hit = this;

    return result;
}

void Mesh::SetMaterial(Material *m)
{
    this->material = m;
    for(Triangle *t : faces)
    {
        t->SetMaterial(m);
    }
}


void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2]);
                glm::vec3 p2(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2]);
                glm::vec3 p3(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2]);

                Triangle* t = new Triangle(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }
        std::cout << "" << std::endl;
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}

void Mesh::create(){
    //Count the number of vertices for each face so we can get a count for the entire mesh
        std::vector<glm::vec3> vert_pos;
        std::vector<glm::vec3> vert_nor;
        std::vector<glm::vec3> vert_col;
        std::vector<GLuint> vert_idx;

        unsigned int index = 0;

        for(int i = 0; i < faces.size(); i++){
            Triangle* tri = faces[i];
            vert_pos.push_back(tri->points[0]); vert_nor.push_back(tri->normals[0]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[1]); vert_nor.push_back(tri->normals[1]); vert_col.push_back(tri->material->base_color);
            vert_pos.push_back(tri->points[2]); vert_nor.push_back(tri->normals[2]); vert_col.push_back(tri->material->base_color);
            vert_idx.push_back(index++);vert_idx.push_back(index++);vert_idx.push_back(index++);
        }

        count = vert_idx.size();
        int vert_count = vert_pos.size();

        bufIdx.create();
        bufIdx.bind();
        bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufIdx.allocate(vert_idx.data(), count * sizeof(GLuint));

        bufPos.create();
        bufPos.bind();
        bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufPos.allocate(vert_pos.data(), vert_count * sizeof(glm::vec3));

        bufCol.create();
        bufCol.bind();
        bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufCol.allocate(vert_col.data(), vert_count * sizeof(glm::vec3));

        bufNor.create();
        bufNor.bind();
        bufNor.setUsagePattern(QOpenGLBuffer::StaticDraw);
        bufNor.allocate(vert_nor.data(), vert_count * sizeof(glm::vec3));
}

//This does nothing because individual triangles are not rendered with OpenGL; they are rendered all together in their Mesh.
void Triangle::create(){}
