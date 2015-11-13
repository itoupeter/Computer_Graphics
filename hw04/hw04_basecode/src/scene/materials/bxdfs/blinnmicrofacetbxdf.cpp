#include <scene/materials/bxdfs/blinnmicrofacetbxdf.h>

glm::vec3 BlinnMicrofacetBxDF::EvaluateScatteredEnergy(const glm::vec3 &wo, const glm::vec3 &wi) const
{
    //TODO
    glm::vec3 wh( glm::normalize( wo + wi ) );

    return reflection_color * ( exponent + 2 )
            / TWO_PI / powf( fabsf( wh.z ), exponent );
}

glm::vec3 BlinnMicrofacetBxDF::SampleAndEvaluateScatteredEnergy(const glm::vec3 &wo, glm::vec3 &wi_ret, float rand1, float rand2, float &pdf_ret) const{

    float cos_theta( powf( rand1, 1.f / ( exponent + 1.f ) ) );
    float sin_theta( sqrtf( fmaxf( 0.f, 1.f - cos_theta * cos_theta ) ) );

    float phi( rand2 * TWO_PI );
    float cos_phi( cosf( phi ) );
    float sin_phi( sinf( phi ) );

    glm::vec3 wh( sin_theta * cos_phi, sin_theta * sin_phi, cos_theta );

    float woDwh( glm::dot( wo, wh ) );

    if( woDwh < 0.f ) wh = -wh;

    wi_ret = -wo + 2.f * woDwh * wh;
    pdf_ret = PDF( wo, wi_ret );

    return EvaluateScatteredEnergy( wo, wi_ret );
}

glm::vec3 BlinnMicrofacetBxDF::EvaluateHemisphereScatteredEnergy(const glm::vec3 &wo, int num_samples, const glm::vec2 *samples) const
{
    //TODO
    glm::vec3 color( 0.f );

    for( int i = 0; i < num_samples; ++i ){

        float cos_theta( powf( rand1, 1.f / ( exponent + 1.f ) ) );
        float sin_theta( sqrtf( fmaxf( 0.f, 1.f - cos_theta * cos_theta ) ) );

        float phi( rand2 * TWO_PI );
        float cos_phi( cosf( phi ) );
        float sin_phi( sinf( phi ) );

        glm::vec3 wh( sin_theta * cos_phi, sin_theta * sin_phi, cos_theta );

        if( glm::dot( wo, wh ) < 0.f ) wh = -wh;

        wi_ret = -wo + 2.f * woDwh * wh;

        color += EvaluateScatteredEnergy( wo, wi_ret );
    }

    return color / num_samples;
}

float BlinnMicrofacetBxDF::PDF(const glm::vec3 &wo, const glm::vec3 &wi) const{

    glm::vec3 wh( glm::normalize( wo + wi ) );
    float cos_theta( fabsf( wh.z ) );
    float woDwh( glm::dot( wo, wh ) );

    if( woDwh <= 0.f ) return 0.f;

    float PDF( ( ( exponent + 1.f ) * powf( cos_theta, exponent ) )
               / ( TWO_PI * 4.f * woDwh ) );

    return PDF;
}
