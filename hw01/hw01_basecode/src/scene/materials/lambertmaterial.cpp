#include <scene/materials/lambertmaterial.h>

LambertMaterial::LambertMaterial():Material()
{}

LambertMaterial::LambertMaterial(const glm::vec3 &color):Material(color)
{}

glm::vec3 LambertMaterial::EvaluateReflectedEnergy(const Intersection &isx, const glm::vec3 &outgoing_ray, const glm::vec3 &incoming_ray)
{
    glm::vec3 L( glm::normalize( -incoming_ray ) );
    glm::vec3 N( glm::normalize( isx.normal ) );

    return glm::vec3( glm::max( glm::dot( L, N ), 0.f ) );
}
