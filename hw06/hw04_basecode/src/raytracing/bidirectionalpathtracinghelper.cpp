
#include "bidirectionalpathtracinghelper.h"

BidirectionalPathTracingHelper::BidirectionalPathTracingHelper():
    BidirectionalPathTracingHelper( NULL, NULL ){
}

BidirectionalPathTracingHelper::BidirectionalPathTracingHelper(
        Scene *scene,
        IntersectionEngine *intersection_engine ){

    this->scene = scene;
    this->intersection_engine = intersection_engine;
}

void BidirectionalPathTracingHelper::generatePath(
        vector< Intersection > &path_vertices,
        vector< glm::vec3 > &path_weights,
        int max_depth ){

    //---initialization---
    path_vertices.clear();
    path_weights.clear();

    int depth( 0 );
    float PDF( 0.f );
    glm::vec3 wiW( 0.f );

    //---pick a light---
    float rand0( distribution( generator ) );
    while( rand0 > 0.99f ) rand0 = distribution( generator );
    int light_idx( ( int )( scene->lights.size() * rand0 ) );
    Geometry *pLight( scene->lights[ light_idx ] );

    //---initial vertex on the light---
    float rand1( distribution( generator ) );
    float rand2( distribution( generator ) );
    float rand3( distribution( generator ) );
    Intersection isx( pLight->SampleLight( rand1, rand2, rand3 ) );

    //---generate path vertices---
    while( depth < max_depth ){

        //---sample direction from current vertex---
        isx.object_hit->material->SampleAndEvaluateScatteredEnergy( isx, glm::vec3( 0.f ), wiW, PDF );

        //---shoot ray to find next vertex---
        Ray ray( isx.point + isx.normal * 1e-4f, wiW );
        Intersection isx_new( intersection_engine->GetIntersection( ray ) );
        glm::vec3 wiW_new( 0.f );
        float PDF_new( 0.f );

        if( isx_new.object_hit == NULL ){
            //---hit nothing---
            break;
        }else if( isx_new.object_hit->material->is_light_source ){
            //---hit light---
            break;
        }

        //---hit object---
        path_vertices.push_back( isx_new );

        if( depth == 0 ){
            //---first vertex, direct lighting---
            glm::vec3 bxdf( isx_new.object_hit->material->SampleAndEvaluateScatteredEnergy( isx_new, -wiW, wiW_new, PDF_new ) );
            glm::vec3 Ld( pLight->material->EvaluateScatteredEnergy( isx, glm::vec3( 0.f ), wiW ) );
            float absdot( fabsf( glm::dot( isx.normal, -wiW ) ) );
            float light_pdf( pLight->RayPDF( isx, Ray( isx_new.point, isx.point - isx_new.point ) ) );

            path_weights.push_back( bxdf * Ld * absdot / light_pdf );

        }else{
            //---not first vertex, indirect lighting---
            glm::vec3 bxdf( isx_new.object_hit->material->SampleAndEvaluateScatteredEnergy( isx_new, -wiW, wiW_new, PDF_new ) );
            glm::vec3 Li( path_weights[ path_weights.size() - 2 ] );
            float absdot( fabsf( glm::dot( isx.normal, -wiW ) ) );

            path_weights.push_back( bxdf * Li * absdot );
        }

        //---iterate---
        isx = isx_new;
        ++depth;
    }
}
