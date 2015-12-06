#include <scene/materials/lightmaterial.h>

glm::vec3 LightMaterial::EvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, const glm::vec3 &wiW, BxDFType flags) const
{
    return glm::dot(wiW, isx.normal) > 0.0f ? (this->base_color * isx.texture_color * this->intensity) : glm::vec3(0.0f);
}

//---sample direction only---
glm::vec3 LightMaterial::SampleAndEvaluateScatteredEnergy(const Intersection &isx, const glm::vec3 &woW, glm::vec3 &wiW_ret, float &pdf_ret, BxDFType flags ) const{

    glm::mat3 w2t;

    w2t[ 0 ][ 0 ] = isx.tangent[ 0 ];
    w2t[ 1 ][ 0 ] = isx.tangent[ 1 ];
    w2t[ 2 ][ 0 ] = isx.tangent[ 2 ];

    w2t[ 0 ][ 1 ] = isx.bitangent[ 0 ];
    w2t[ 1 ][ 1 ] = isx.bitangent[ 1 ];
    w2t[ 2 ][ 1 ] = isx.bitangent[ 2 ];

    w2t[ 0 ][ 2 ] = isx.normal[ 0 ];
    w2t[ 1 ][ 2 ] = isx.normal[ 1 ];
    w2t[ 2 ][ 2 ] = isx.normal[ 2 ];

    glm::mat3 t2w( glm::transpose( w2t ) );

    float rand1( distribution( generator ) * TWO_PI );
    float rand2( distribution( generator ) * PI * .5f );

    float x( cosf( rand1 ) * sinf( rand2 ) );
    float y( sinf( rand1 ) * sinf( rand2 ) );
    float z( cosf( rand2 ) );

    wiW_ret = t2w * glm::vec3( x, y, z );

    return glm::vec3( 0.f );
}
