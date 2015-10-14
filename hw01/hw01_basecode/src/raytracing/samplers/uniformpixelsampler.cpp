#include <raytracing/samplers/uniformpixelsampler.h>

UniformPixelSampler::UniformPixelSampler():PixelSampler()
{}

UniformPixelSampler::UniformPixelSampler(int samples):PixelSampler(samples)
{}

QList<glm::vec2> UniformPixelSampler::GetSamples(int x, int y)
{
    QList<glm::vec2> result;

    float a( x - .5f );
    float b( y - .5f );
    float c( 1.f / samples_sqrt );

    for( int i = 0; i < samples_sqrt; ++i ){
        for( int j = 0; j < samples_sqrt; ++j ){
            result.push_back( glm::vec2( a + c * i, b + c * j ) );
        }
    }

    return result;
}
