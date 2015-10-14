
#include "raytracing/samplers/randompixelsampler.h"
#include <random>

RandomPixelSampler::RandomPixelSampler():
    PixelSampler(){
}

RandomPixelSampler::RandomPixelSampler( int samples ):
    PixelSampler( samples ),
    generator(),
    distribution( 0.f, 1.f ){
}

QList< glm::vec2 > RandomPixelSampler::GetSamples( int x, int y ){

    QList< glm::vec2 > result;

    float a( x - .5f );
    float b( y - .5f );
    float c( 1.f / samples_sqrt );

    for( int i = 0; i < samples_sqrt; ++i ){
        for( int j = 0; j < samples_sqrt; ++j ){
            result.push_back( glm::vec2( a + distribution( generator ), b + distribution( generator ) ) );
        }
    }

    return result;
}
