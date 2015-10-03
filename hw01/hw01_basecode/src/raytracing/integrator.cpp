#include <raytracing/integrator.h>


Integrator::Integrator():
    max_depth(5)
{
    scene = NULL;
    intersection_engine = NULL;
}

glm::vec3 Integrator::ShadowTest( const glm::vec3 &point ){

    static const glm::vec3 black( 0.f );
    static const glm::vec3 white( 1.f );

    QList< Geometry * >::iterator iterator;
    glm::vec3 shadow( 0.f );
    int nLights( 0 );

    for( iterator = scene->lights.begin(); iterator != scene->lights.end(); ++iterator ){

        Geometry *pLight( ( *iterator ) );
        glm::vec3 light_position( glm::vec3( pLight->transform.T() * glm::vec4( 0.f, 0.f, 0.f, 1.f ) ) );
        glm::vec3 light_direction( light_position - point );
        float distance( glm::length( light_direction ) );

        //---construct a feeler ray from the tested point to the light---
        Intersection isx( intersection_engine->GetIntersection( Ray( point, light_direction ) ) );
        assert( isx.object_hit != NULL );

        if( isx.object_hit == NULL ){
            return black;
        }else if( isx.object_hit->material->emissive > .1f ){
            return isx.object_hit->material->base_color;
        }else if( isx.object_hit->material->refract_idx_in > .1f ){
            return isx.color;
        }
//        if( isx.object_hit == NULL ){
//            return black;
//            //---no obstruction---
//            shadow = pLight->material->base_color;
//            ++nLights;
//        }else{
//            //---has obstruction---
//            float distance2( glm::length( point - isx.point ) );

//            if( distance2 > distance ){
//                return white;
//                //---obstruction behind light---
//                shadow = pLight->material->base_color;
//                ++nLights;
//            }else{
//                //---obstruction before light---
//                Geometry *pObstructingGeometry( isx.object_hit );
//                Material *pObstructingMaterial( pObstructingGeometry->material );

//                if( pObstructingGeometry->material->refract_idx_in > .1f ){
//                    //---refractive obstruction---
//                    shadow = pObstructingMaterial->base_color;
//                    ++nLights;
//                }else{
//                    //---opaque obstruction---
//                    shadow = glm::vec3( 0.f );
//                }
//            }
//        }
    }

    return shadow / ( float )nLights;
}

//Basic ray trace
glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth)
{

    static const glm::vec3 black( 0.f, 0.f, 0.f );
    static const glm::vec3 white( 1.f, 1.f, 1.f );
    static const float EPS( 1e-5 );

    //---exceed recurrence depth---
    if( depth == max_depth ) return black;

    //---compute intersection---
    Intersection isx( intersection_engine->GetIntersection( r ) );

    //---get hit geometry, material---
    Geometry *pGeometry( isx.object_hit );

    if( pGeometry == NULL ) return black;

    Material *pMaterial( pGeometry->material ); assert( pMaterial != NULL );

    //---hit light, return light color---
    if( pMaterial->emissive > .1f ){
        return pMaterial->base_color;
    }

    //---local illumination---
    glm::vec3 result( isx.color /** ShadowTest( isx.point + isx.normal * EPS )*/ );

    return result;
}

void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}
