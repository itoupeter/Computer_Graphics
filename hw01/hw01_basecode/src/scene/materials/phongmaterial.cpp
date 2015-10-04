#include <scene/materials/phongmaterial.h>

PhongMaterial::PhongMaterial():
    PhongMaterial(glm::vec3(0.5f, 0.5f, 0.5f))
{}

PhongMaterial::PhongMaterial(const glm::vec3 &color):
    Material(color),
    specular_power(10)
{}

glm::vec3 PhongMaterial::EvaluateReflectedEnergy(const Intersection &isx, const glm::vec3 &outgoing_ray, const glm::vec3 &incoming_ray)
{
    glm::vec3 L( glm::normalize( -incoming_ray ) );
    glm::vec3 V( glm::normalize( outgoing_ray ) );
    glm::vec3 N( isx.normal );
    glm::vec3 H( .5f * ( V + L ) );

    float D( glm::max( glm::dot( L, N ), 0.f ) );
    float S( glm::pow( glm::max( glm::dot( N, H ), 0.f ), specular_power ) );

    float Kd( .6f );
    float Ks( .3f );
    float Ka( .1f );

    return glm::vec3( D * Kd + S * Ks + Ka );
}
