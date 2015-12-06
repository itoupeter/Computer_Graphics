#include "raytracing/directlightingintegrator.h"
#include "raytracing/globallightingintegrator.h"
#include "raytracing/bidirectionalpathtracinghelper.h"

GlobalLightingIntegrator::GlobalLightingIntegrator( Scene *scene, IntersectionEngine *intersection_engine, int max_depth ):
    Integrator(){

    this->scene = scene;
    this->intersection_engine = intersection_engine;
    SetDepth( max_depth );
}

glm::vec3 GlobalLightingIntegrator::TraceRay( Ray r, unsigned int depth ){

    const glm::vec3 black( 0.f );

    Intersection isx( intersection_engine->GetIntersection( r ) );

    //---no intersection with scene---
    if( isx.object_hit == NULL ) return black;

    //---hit light---
    if( isx.object_hit->material->is_light_source )
        return isx.object_hit->material->base_color * isx.texture_color;

    //---global lighting---
    static DirectLightingIntegrator directLightingIntegrator( scene, intersection_engine, max_depth );
    static BidirectionalPathTracingHelper bidirectionalPTHelper( scene, intersection_engine );
    glm::vec3 A( 0.f ), B( 1.f );
    float throughput( 1.f );

    while( depth < max_depth ){

        glm::vec3 Li( 0.f ), Ld( 0.f );

        //---direct lighting---
        Ld = directLightingIntegrator.TraceRay( r, depth );

//#define BIDIRECTIONAL

#ifdef BIDIRECTIONAL
        //---bidirectional indirect lighting---
        vector< Intersection > path_vertices;
        vector< glm::vec3 > path_weights;

        bidirectionalPTHelper.generatePath( path_vertices, path_weights, max_depth );
        assert( path_vertices.size() == path_weights.size() );

        for( int i = 0; i < path_vertices.size(); ++i ){

            Intersection &isx_nxt = path_vertices[ i ];
            glm::vec3 wiW_nxt( glm::normalize( isx_nxt.point - isx.point ) );

            if( !Visible( isx, isx_nxt ) ) continue;

            Lb += 1.f / ( i + depth + 2.f )
                    //---L---
                    * path_weights[ i ]
                    //---BxDF---
                    * isx.object_hit->material->EvaluateScatteredEnergy( isx, -r.direction, wiW_nxt )
                    //---absdot---
                    * fabsf( glm::dot( -r.direction, isx.normal ) );
        }
#endif

        //---indirect lighting---
        float pdf_bxdf( 0.f );
        glm::vec3 wiW_bxdf( 0.f );
        glm::vec3 bxdf( isx.object_hit->material->SampleAndEvaluateScatteredEnergy( isx, -r.direction, wiW_bxdf, pdf_bxdf ) );

        //---Russian Roulette---
        if( depth > 2 ){

            float tmp( glm::max( bxdf.x, glm::max( bxdf.y, bxdf.z ) ) );
            float rand( distribution( generator ) );

            throughput *= tmp * fabsf( glm::dot( wiW_bxdf, isx.normal ) );

            if( throughput < rand ){
                A += B * Ld / rand;
                break;
            }
        }

        //---pass Russian Roulette test, keep on tracing---
        Ray r_bxdf( isx.point + isx.normal * 1e-4f, wiW_bxdf );
        Intersection isx_bxdf( intersection_engine->GetIntersection( r_bxdf ) );

        if( isx_bxdf.object_hit == NULL
                || isx_bxdf.object_hit->material->is_light_source
                || pdf_bxdf < 1e-3f ){
            //---hit nothing, or light, or PDF too small---
            break;
        }else{
            Li = bxdf * fabsf( glm::dot( isx.normal, wiW_bxdf ) ) / pdf_bxdf;
            B *= Li;
        }

        A += B * Ld;

        //---iterate---
        isx = isx_bxdf;
        r = r_bxdf;
        ++depth;
    }

    return A;
}
