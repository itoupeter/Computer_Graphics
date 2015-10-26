#include <scene/geometry/geometry.h>

float Geometry::RayPDF(const Intersection &isx, const Ray &ray)
{
    //TODO
    //The isx passed in was tested ONLY against us (no other scene objects), so we test if NULL
    //rather than if != this.
    if(isx.object_hit == NULL)
    {
        return 0;
    }else{
        //Add more here
        if( fequal( isx.object_hit->area, 0.f ) ){
            isx.object_hit->ComputeArea();
        }

        float A( glm::normalize( isx.normal ) );
        float R( glm::normalize( -ray.direction ) );
        float r2( glm::length2( R ) );
        float area( isx.object_hit->area );
        float cos_theta( glm::dot( A, R ) );

        return r2 / ( cos_theta * area );
    }
}

Intersection Geometry::SampleLight( float a, float b ){

    return Intersection();
}
