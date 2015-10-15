#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <iostream>
#include <functional>

StratifiedPixelSampler::StratifiedPixelSampler():PixelSampler(){}

StratifiedPixelSampler::StratifiedPixelSampler(int samples) : PixelSampler(samples), generator(), distribution( 0.f, 1.f ){}

QList<glm::vec2> StratifiedPixelSampler::GetSamples(int x, int y)
{
    QList<glm::vec2> result;

    float a( x - .5f );
    float b( y - .5f );
    float c( 1.f / samples_sqrt );

    for( int i = 0; i < samples_sqrt; ++i ){
        for( int j = 0; j < samples_sqrt; ++j ){
            result.push_back( glm::vec2( a + c * i + c * distribution( generator ), b + c * i + c * distribution( generator ) ) );
        }
    }

    return result;
}
