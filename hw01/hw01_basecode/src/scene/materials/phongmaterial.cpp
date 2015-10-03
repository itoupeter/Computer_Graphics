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
    glm::vec3 H( .5f * ( outgoing_ray - incoming_ray ) );

    float D( glm::dot( -incoming_ray, isx.normal ) );
    float S( glm::pow( glm::dot( isx.normal, H ), specular_power ) );

    float Kd( .6f );
    float Ks( .3f );
    float Ka( .1f );

    return glm::vec3( D * Kd + S * Ks + Ka );
}
