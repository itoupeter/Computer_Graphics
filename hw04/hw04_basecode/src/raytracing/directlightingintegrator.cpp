
#include <ctime>
#include "raytracing/directlightingintegrator.h"

const int DirectLightingIntegrator::N = 1;

DirectLightingIntegrator::DirectLightingIntegrator( Scene *scene, IntersectionEngine *intersection_engine ):
    Integrator(),
    distribution( 0.f, 1.f ),
    generator( std::time( 0 ) ){

    this->scene = scene;
    this->intersection_engine = intersection_engine;

}

glm::vec3 DirectLightingIntegrator::TraceRay( Ray r ){

    static const glm::vec3 white( 1.f );
    static const glm::vec3 black( 0.f );
    static const glm::vec3 red( 1.f, 0.f, 0.f );
    static const glm::vec3 green( 0.f, 1.f, 0.f );
    static const glm::vec3 blue( 0.f, 0.f, 1.f );
    static const glm::vec3 yellow( 1.f, 1.f, 0.f );

    //---compute intersection with scene---
    Intersection isx( intersection_engine->GetIntersection( r ) );

    //---has intersection---
    glm::vec3 light_color( 0.f );

    for( Geometry *pLight : scene->lights ){

        //---sample each light---
        for( int i = 0; i < DirectLightingIntegrator::N; ++i ){

            float a( distribution( generator ) );
            float b( distribution( generator ) );

            Intersection sample( pLight->SampleLight( a, b ) );
            glm::vec3 wiW( r.direction );
            glm::vec3 woW( glm::normalize( glm::vec3( sample.point - isx.point ) ) );
            Ray ray( isx.point, woW );
            float PDF( isx.object_hit->RayPDF( sample, ray ) );

            if( PDF < 1e-4 ) continue;

            light_color += 1.f
                    //---BRDF---
                    * isx.object_hit->material->EvaluateScatteredEnergy( isx, woW, wiW )
                    //---L---
                    * pLight->material->EvaluateScatteredEnergy( sample, woW, -wiW )
                    //---shadow---
                    * ShadowTest( isx.point + isx.normal * 1e-4f, sample.point, pLight )
                    //---lambertian term---
                    * glm::dot( isx.normal, glm::normalize( woW ) )
                    //---ray PDF---
                    / PDF
                    ;
        }
    }

    return light_color / float( DirectLightingIntegrator::N );

}

glm::vec3 DirectLightingIntegrator::ShadowTest( const glm::vec3 &o, const glm::vec3 &d, const Geometry *pLight ){

    static const glm::vec3 white( 1.f );
    static const glm::vec3 black( 0.f );
    static const glm::vec3 red( 1.f, 0.f, 0.f );
    static const glm::vec3 green( 0.f, 1.f, 0.f );
    static const glm::vec3 blue( 0.f, 0.f, 1.f );
    static const glm::vec3 yellow( 1.f, 1.f, 0.f );

    Ray ray( o, d - o );
    QList< Intersection > isxes( intersection_engine->GetAllIntersections( ray ) );

    for( const Intersection &isx : isxes ){
        //---null intersection---
        if( isx.object_hit == NULL ) return red;
        //---non-light intersection---
        if( isx.object_hit->material->is_light_source == false ) return black;
        //---unwanted light intersection---
        if( isx.object_hit != pLight ) continue;
        //---wanted light intersection---
        else return white;
    }

    return green;
}
