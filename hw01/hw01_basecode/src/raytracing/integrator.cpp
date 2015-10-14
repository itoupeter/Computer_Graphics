#include <raytracing/integrator.h>


Integrator::Integrator():
    max_depth(5)
{
    scene = NULL;
    intersection_engine = NULL;
}

//my
glm::vec3 Integrator::ShadowTest( const glm::vec3 &point, Geometry *pLight ){

    static const glm::vec3 black( 0.f );
    static const glm::vec3 grey( 0.f );

    glm::vec3 light_position( pLight->transform.position() );
    glm::vec3 light_direction( light_position - point );

    //---shoot a feeler ray from the tested point to the light and test intersection---
    Intersection isx( intersection_engine->GetIntersection( Ray( point, light_direction ) ) );
    assert( isx.object_hit != NULL );

    if( isx.object_hit->material->emissive > .1f ){
        //---no obstruction---
        return pLight->material->base_color;
    }else if( isx.object_hit->material->refract_idx_in > 0.1f ){
        //---refractive obstruction---
        return isx.color;
    }else{
        //---opaque obstruction---
        return black;
    }
}

//Basic ray trace
glm::vec3 Integrator::TraceRay(Ray r, unsigned int depth)
{

    static const glm::vec3 black( 0.f, 0.f, 0.f );
    static const float EPS( 1e-4 );

    glm::vec3 result( 0.f );

    //---exceed recurrence depth---
    if( depth == max_depth ) return black;

    //---compute intersection---
    Intersection isx( intersection_engine->GetIntersection( r ) );
    glm::vec3 isx_origin_out( isx.point + isx.normal * EPS );
    glm::vec3 isx_origin_in( isx.point - isx.normal * EPS );

    //---get hit geometry, material---
    Geometry *pGeometry( isx.object_hit );

    if( pGeometry == NULL ) return black;

    Material *pMaterial( pGeometry->material ); assert( pMaterial != NULL );

    //---hit light, return light color---
    if( pMaterial->emissive > .1f ){
        return isx.color;
    }

    //---hit non-light---
    for( Geometry *pLight : scene->lights ){
        //---local illumination---
        bool isEntering( glm::dot( r.direction, isx.normal ) < 0 );
        glm::vec3 dir_light_isx( isx.point - pLight->transform.position() );
        glm::vec3 ERE( pMaterial->EvaluateReflectedEnergy( isx, -r.direction, dir_light_isx ) );

        if( pMaterial->refract_idx_in < .1f ){
            //---opaque---
            result += ( 1 - pMaterial->reflectivity )
                    * isx.color
                    * ERE
                    * ShadowTest( isx_origin_out, pLight );
        }else{
            //---transparent---
            if( isEntering ){
                float eta( pMaterial->refract_idx_out / pMaterial->refract_idx_in );
                glm::vec3 new_ray_o( isx_origin_in );
                glm::vec3 new_ray_d( glm::refract( r.direction, isx.normal, eta ) );
                Ray new_ray( new_ray_o, new_ray_d );

                result += ( 1 - pMaterial->reflectivity )
                        * isx.color
                        * TraceRay( new_ray, depth + 1 );
            }else{
                float eta( pMaterial->refract_idx_in / pMaterial->refract_idx_out );
                glm::vec3 new_ray_o( isx_origin_out );
                glm::vec3 new_ray_d( glm::refract( r.direction, -isx.normal, eta ) );
                Ray new_ray( new_ray_o, new_ray_d );

                result += ( 1 - pMaterial->reflectivity )
                        * isx.color
                        * TraceRay( new_ray, depth + 1 );
            }
        }

        //---reflected color---
        bool isReflected( true );

        //---test total reflection---
        if( !isEntering ){
            float eta( pMaterial->refract_idx_in / pMaterial->refract_idx_out );
            glm::vec3 new_ray_d( glm::refract( r.direction, -isx.normal, eta ) );

            if( !fequal( glm::length2( new_ray_d ), 0.f ) ) isReflected = false;
        }

        if( isReflected && pMaterial->reflectivity > EPS ){
            glm::vec3 new_ray_o( isx_origin_out );
            glm::vec3 new_ray_d( glm::reflect( r.direction, isx.normal ) );
            Ray new_ray( new_ray_o, new_ray_d );

            result += pMaterial->reflectivity
                    * isx.color
                    * TraceRay( new_ray, depth + 1 );
        }

    }

    return result / ( float )scene->lights.size();
}


void Integrator::SetDepth(unsigned int depth)
{
    max_depth = depth;
}
