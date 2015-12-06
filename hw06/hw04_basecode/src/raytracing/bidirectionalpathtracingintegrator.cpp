
#include "bidirectionalpathtracingintegrator.h"

BidirectionalPathTracingIntegrator::BidirectionalPathTracingIntegrator(
        Scene *scene,
        IntersectionEngine *intersection_engine,
        int max_depth ):
    Integrator(){

    this->scene = scene;
    this->intersection_engine = intersection_engine;
    SetDepth( max_depth );
}

glm::vec3 BidirectionalPathTracingIntegrator::TraceRay( Ray r, unsigned int depth ){

    static const glm::vec3 black( 0.f );

    return black;
}

void BidirectionalPathTracingIntegrator::GeneratePath( Ray ray, Geometry *pLight ){

    //---initialization---
    vertices_eye.clear();
    vertices_light.clear();

    //---generate eye path---
    for( int i = 0; i < max_depth; ++i ){

        Intersection isx( intersection_engine->GetIntersection( ray ) );
        float pdf_bxdf( 0.f );
        glm::vec3 wiW_bxdf( 0.f );
        glm::vec3 bxdf( isx.object_hit->material->SampleAndEvaluateScatteredEnergy( isx, -ray.direction, wiW_bxdf, pdf_bxdf ) );

        vertices_eye.push_back( isx );
        ray = Ray( isx.point + isx.normal * 1e-3f, wiW_bxdf );
    }

}

bool BidirectionalPathTracingIntegrator::Visible( const Intersection &a, const Intersection &b ){

    glm::vec3 ray_o( a.point + a.normal * 1e-3f );
    glm::vec3 ray_d( b.point - a.point );
    Ray ray( ray_o, ray_d );

    Intersection isx( intersection_engine->GetIntersection( ray ) );
    return isx.object_hit == b.object_hit;

    QList< Intersection > isxes( intersection_engine->GetAllIntersections( ray ) );

    for( Intersection isx : isxes ){
        if( isx.object_hit == NULL ) continue;
        if( isx.object_hit->material->is_light_source ) continue;
        if( isx.object_hit != b.object_hit ) return false;
        if( glm::distance2( isx.point, b.point ) > 1e-2f ) return false;
    }

    return true;
}
