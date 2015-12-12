
#include "bidirectionalpathtracingintegrator.h"
#include "directlightingintegrator.h"

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

    Intersection isx( intersection_engine->GetIntersection( r ) );

    //---no intersection with scene---
    if( isx.object_hit == NULL ) return black;

    //---hit light---
    if( isx.object_hit->material->is_light_source )
        return isx.object_hit->material->base_color * isx.texture_color;

    //---bidirectional path tracing---
    //---path---
    BidirectionalPath path;

    //---vertices along path---
//    vector< Intersection > vertices_eye;
//    vector< Intersection > vertices_light;

    //---pdfs of vertices---
//    vector< float > pdf_eye;
//    vector< float > pdf_light;

    //---generate path---
    GeneratePath( r, scene->lights[ 0 ], path );
//    GeneratePath( r, scene->lights[ 0 ], vertices_eye, pdf_eye, vertices_light, pdf_light );

    //---evaluate path---
    glm::vec3 result( 0.f );

    for( int i = 1; i <= max_depth; ++i ){
        for( int j = 1; j <= max_depth; ++j ){
            result += 1.f / ( i + j + 0.f ) * EvaluatePath( r, scene->lights[ 0 ], i, j, path );
//            result += 1.f / ( i + j + 0.f )
//                    * EvaluatePath( r, scene->lights[ 0 ], i, j, vertices_eye, pdf_eye, vertices_light, pdf_light );
        }
    }

    return result;
}

void BidirectionalPathTracingIntegrator::GeneratePath(
        Ray ray, Geometry *pLight, BidirectionalPath &path ){

    //---generate eye path---
    for( int i = 0; i < max_depth; ++i ){

        Intersection isx( intersection_engine->GetIntersection( ray ) );

        //---hit nothing or light---
        if( isx.object_hit == NULL || isx.object_hit->material->is_light_source ) break;

        float pdf( 0.f );
        glm::vec3 wiW( 0.f );
        glm::vec3 f( isx.object_hit->material->SampleAndEvaluateScatteredEnergy( isx, -ray.direction, wiW, pdf ) );

        if( pdf < 1e-2f ) break;

        path.vertices_eye.push_back( isx );
        path.bxdf_eye.push_back( f );
        path.pdf_eye.push_back( pdf );
        path.wiW_eye.push_back( wiW );
        if( i == 0 )
            path.woW_eye.push_back( -ray.direction );
        else
            path.woW_eye.push_back( -path.wiW_eye[ i - 1 ] );

        ray = Ray( isx.point + isx.normal * 1e-3f, wiW );
    }

    //---generate light path---
    float rand1( distribution( generator ) );
    float rand2( distribution( generator ) );
    float rand3( distribution( generator ) );
    path.isx_light = pLight->SampleLight( rand1, rand2, rand3 );
    ray = Ray( path.isx_light.point + path.isx_light.normal * 1e-3f, path.isx_light.normal );

    for( int i = 0; i < max_depth; ++i ){

        Intersection isx( intersection_engine->GetIntersection( ray ) );

        if( isx.object_hit == NULL || isx.object_hit->material->is_light_source ) break;

        float pdf( 0.f );
        glm::vec3 wiW( 0.f );
        glm::vec3 f( isx.object_hit->material->SampleAndEvaluateScatteredEnergy( isx, -ray.direction, wiW, pdf ) );

        if( pdf < 1e-2f ) break;

        path.vertices_light.push_back( isx );
        path.bxdf_light.push_back( f );
        path.pdf_light.push_back( pdf );
        path.woW_light.push_back( wiW );
        if( i == 0 )
            path.wiW_light.push_back( glm::normalize( pLight->transform.position() - isx.point ) );
        else
            path.wiW_light.push_back( -path.woW_light[ i - 1 ] );

        ray = Ray( isx.point + isx.normal * 1e-3f, wiW );
    }

}

glm::vec3 BidirectionalPathTracingIntegrator::EvaluatePath(
        Ray ray, Geometry *pLight, int I, int J, BidirectionalPath &path ){

    static const glm::vec3 black( 0.f );

    //---path too short---
    if( path.vertices_eye.size() < I || path.vertices_light.size() < J ) return black;

    //---test visibility---
    if( !Visible( path.vertices_eye[ I - 1 ], path.vertices_light[ J - 1 ] ) ) return black;

    //---direct lighting integrator---
    static DirectLightingIntegrator directLightingIntegrator( scene, intersection_engine, max_depth );

    glm::vec3 result( pLight->material->EvaluateScatteredEnergy( path.isx_light, glm::vec3( 0.f ), path.isx_light.normal ) );

    //---evaluate light path---
    for( int i = 0; i < J - 1; ++i ){

        Intersection &isx_cur = path.vertices_light[ i ];
        Ray ray_Ld( isx_cur.point + path.wiW_light[ i ] * 1e-2f, -path.wiW_light[ i ] );

        result = 1.f
                //---L---
                * result
                //---BxDF---
                * path.bxdf_light[ i ]
                //---cosine term---
                * fabsf( glm::dot( path.wiW_light[ i ], isx_cur.normal ) )
                //---PDF---
                / path.pdf_light[ i ];
    }

    //---evaluate connecting part---
    {
        Intersection &isx_I = path.vertices_eye[ I - 1 ];
        Intersection &isx_J = path.vertices_light[ J - 1 ];
        glm::vec3 I2J( glm::normalize( isx_J.point - isx_I.point ) );

        Ray ray_Ld( isx_J.point - I2J * 1e-2f, I2J );

        result = 1.f
                //---L---
                * result
                //---BxDF---
                * isx_J.object_hit->material->EvaluateScatteredEnergy( isx_J, -I2J, path.wiW_light[ J - 1 ] )
                //---cosine term---
                * fabsf( glm::dot( path.wiW_light[ J - 1 ], isx_J.normal ) );

        result = 1.f
                //---L---
                * result
                //---BxDF---
                * isx_I.object_hit->material->EvaluateScatteredEnergy( isx_I, path.woW_eye[ I - 1 ], I2J )
                //---cosine term---
                * fabsf( glm::dot( I2J, isx_I.normal ) )
                //---direct lighting---
                + directLightingIntegrator.TraceRay( ray_Ld, 0 );
    }

    //---evaluate eye path---
    for( int i = 0; i < I - 1; ++i ){

        Intersection &isx_cur = path.vertices_eye[ i ];
        Ray ray_Ld( isx_cur.point + path.wiW_eye[ i ] * 1e-2f, -path.wiW_eye[ i ] );

        result = 1.f
                //---L---
                * result
                //---BxDF---
                * path.bxdf_eye[ i ]
                //---cosine term---
                * fabsf( glm::dot( path.wiW_eye[ i ], isx_cur.normal ) )
                //--PDF---
                / path.pdf_eye[ i ]
                //---direct lighting---
                + directLightingIntegrator.TraceRay( ray_Ld, 0 );
    }

    return result;
}

//void BidirectionalPathTracingIntegrator::GeneratePath(
//        Ray ray, Geometry *pLight,
//        vector< Intersection > &vertices_eye, vector< float > &pdf_eye,
//        vector< Intersection > &vertices_light, vector< float > &pdf_light ){

//    //---initialization---
//    vertices_eye.clear();
//    vertices_light.clear();
//    pdf_eye.clear();
//    pdf_light.clear();

//    //---generate eye path---
//    for( int i = 0; i < max_depth; ++i ){

//        Intersection isx( intersection_engine->GetIntersection( ray ) );

//        //---hit nothing or light---
//        if( isx.object_hit == NULL || isx.object_hit->material->is_light_source ) break;

//        float pdf( 0.f );
//        glm::vec3 wiW( 0.f );
//        glm::vec3 f( isx.object_hit->material->SampleAndEvaluateScatteredEnergy( isx, -ray.direction, wiW, pdf ) );

//        if( pdf < 1e-2f ) break;

//        vertices_eye.push_back( isx );
//        pdf_eye.push_back( pdf );

//        ray = Ray( isx.point + isx.normal * 1e-3f, wiW );
//    }

//    //---generate light path---
//    float rand1( distribution( generator ) );
//    float rand2( distribution( generator ) );
//    float rand3( distribution( generator ) );
//    light_sample = pLight->SampleLight( rand1, rand2, rand3 );
//    ray = Ray( light_sample.point + light_sample.normal * 1e-3f, light_sample.normal );

//    for( int i = 0; i < max_depth; ++i ){

//        Intersection isx( intersection_engine->GetIntersection( ray ) );

//        if( isx.object_hit == NULL || isx.object_hit->material->is_light_source ) break;

//        float pdf( 0.f );
//        glm::vec3 wiW( 0.f );
//        glm::vec3 f( isx.object_hit->material->SampleAndEvaluateScatteredEnergy( isx, -ray.direction, wiW, pdf ) );

//        vertices_light.push_back( isx );
//        pdf_light.push_back( pdf );

//        if( pdf < 1e-2f ) break;

//        ray = Ray( isx.point + isx.normal * 1e-3f, wiW );
//    }
//}

//glm::vec3 BidirectionalPathTracingIntegrator::EvaluatePath(
//        Ray ray, Geometry *pLight, int I, int J,
//        vector< Intersection > &vertices_eye, vector< float > &pdf_eye,
//        vector< Intersection > &vertices_light, vector< float > &pdf_light ){

//    glm::vec3 result( 0.f );

//    //---path too short---
//    if( ( int )vertices_eye.size() < I || ( int )vertices_light.size() < J ) return result;

//    //---test visibility---
//    if( !Visible( vertices_eye[ I - 1 ], vertices_light[ J - 1 ] ) ) return result;

//    //---direct lighting---
//    static DirectLightingIntegrator directLightingIntegrator( scene, intersection_engine, max_depth );

//    //---evaluate light path---
//    result = pLight->material->EvaluateScatteredEnergy( light_sample, glm::vec3( 0.f ), light_sample.normal );

//    for( int i = 0; i < J - 1; ++i ){

//        Intersection &isx_cur = vertices_light[ i ];
//        Intersection &isx_pre = vertices_light[ i + 1 ];

//        glm::vec3 wiW( glm::normalize( isx_pre.point - isx_cur.point ) );
//        glm::vec3 woW( 0.f );

//        if( i == 0 ){
//            woW = glm::normalize( pLight->transform.position() - isx_cur.point );
//        }else{
//            Intersection &isx_nxt = vertices_light[ i - 1 ];
//            woW = glm::normalize( isx_nxt.point - isx_cur.point );
//        }

//        Ray ray_Ld( isx_cur.point + wiW * 1e-1f, -wiW );

//        result = 1.f
//                //---BxDF---
//                * isx_cur.object_hit->material->EvaluateScatteredEnergy( isx_cur, woW, wiW )
//                //---L---
//                * result
//                //---cosine term---
//                * fabsf( glm::dot( wiW, isx_cur.normal ) )
//                //---PDF---
//                / pdf_light[ i ]
//                //---direct lighting---
//                + directLightingIntegrator.TraceRay( ray_Ld, max_depth )
//                ;

//    }

//    //---evaluate connecting part---
//    {
//        Intersection &isx_cnnt_1 = vertices_light[ J - 1 ];
//        Intersection &isx_cnnt_2 = vertices_eye[ I - 1 ];

//        glm::vec3 wiW_cnnt_1( 0.f );
//        glm::vec3 woW_cnnt_1( glm::normalize( isx_cnnt_2.point - isx_cnnt_1.point ) );
//        glm::vec3 wiW_cnnt_2( -woW_cnnt_1 );
//        glm::vec3 woW_cnnt_2( 0.f );

//        if( J == 1 ){
//            wiW_cnnt_1 = glm::normalize( pLight->transform.position() - isx_cnnt_1.point );
//        }else{
//            wiW_cnnt_1 = glm::normalize( vertices_light[ J - 2 ].point - isx_cnnt_1.point );
//        }
//        if( I == 1 ){
//            woW_cnnt_2 = glm::normalize( ray.origin - isx_cnnt_2.point );
//        }else{
//            woW_cnnt_2 = glm::normalize( vertices_eye[ I - 2 ].point - isx_cnnt_2.point );
//        }

//        Ray ray_Ld_1( isx_cnnt_1.point + woW_cnnt_1 * 1e-1f, -woW_cnnt_1 );
//        Ray ray_Ld_2( isx_cnnt_2.point + woW_cnnt_2 * 1e-1f, -woW_cnnt_2 );

//        result = 1.f
//                //---BxDF---
//                * isx_cnnt_1.object_hit->material->EvaluateScatteredEnergy( isx_cnnt_1, woW_cnnt_1, wiW_cnnt_1 )
//                //---L---
//                * result
//                //---cosine term---
//                * fabsf( glm::dot( wiW_cnnt_1, isx_cnnt_1.normal ) )
//                //---direct lighting---
//                + directLightingIntegrator.TraceRay( ray_Ld_1, max_depth );
//                ;

//        result = 1.f
//                //---BxDF---
//                * isx_cnnt_2.object_hit->material->EvaluateScatteredEnergy( isx_cnnt_2, woW_cnnt_2, wiW_cnnt_2 )
//                //---L---
//                * result
//                //---cosine term---
//                * fabsf( glm::dot( woW_cnnt_2, isx_cnnt_2.normal ) )
//                //---direct lighting---
//                + directLightingIntegrator.TraceRay( ray_Ld_2, max_depth );
//                ;

//    }

//    //---evaluate eye path---
//    for( int i = I - 2; i >= 0; --i ){

//        Intersection &isx_cur = vertices_eye[ i ];
//        Intersection &isx_nxt = vertices_eye[ i + 1 ];

//        glm::vec3 wiW( 0.f );
//        glm::vec3 woW( glm::normalize( isx_nxt.point - isx_cur.point ) );

//        if( i == 0 ){
//            wiW = glm::normalize( ray.origin - isx_cur.point );
//        }else{
//            Intersection &isx_pre = vertices_eye[ i - 1 ];
//            wiW = glm::normalize( isx_pre.point - isx_cur.point );
//        }

//        Ray ray_Ld( isx_cur.point + wiW * 1e-1f, -wiW );

//        result = 1.f
//                //---BxDF---
//                * isx_cur.object_hit->material->EvaluateScatteredEnergy( isx_cur, woW, wiW )
//                //---L---
//                * result
//                //---cosine term---
//                * fabsf( glm::dot( wiW, isx_cur.normal ) )
//                //---PDF---
//                / pdf_eye[ i ]
//                //---direct lighting---
//                + directLightingIntegrator.TraceRay( ray_Ld, max_depth )
//                ;
//    }

//    return result;
//}

bool BidirectionalPathTracingIntegrator::Visible( const Intersection &a, const Intersection &b ){

    glm::vec3 ray_o( a.point + a.normal * 1e-3f );
    glm::vec3 ray_d( b.point - ray_o );
    Ray ray( ray_o, ray_d );

#define SIMPLE_TEST

#ifdef SIMPLE_TEST
    Intersection isx( intersection_engine->GetIntersection( ray ) );

    return isx.object_hit == b.object_hit;
#else
    QList< Intersection > isxes( intersection_engine->GetAllIntersections( ray ) );

    for( Intersection isx : isxes ){
        if( isx.object_hit == NULL ) continue;
        if( isx.object_hit->material->is_light_source ) continue;
        if( isx.object_hit != b.object_hit ) return false;
        if( glm::distance2( isx.point, b.point ) > 1e-2f ) return false;
    }

    return true;
#endif
}
