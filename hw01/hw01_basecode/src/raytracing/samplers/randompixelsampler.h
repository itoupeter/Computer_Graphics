#ifndef RANDOMPIXELSAMPLER_H
#define RANDOMPIXELSAMPLER_H

#include "pixelsampler.h"
#include <random>

class RandomPixelSampler : public PixelSampler{
public:
    RandomPixelSampler();
    RandomPixelSampler( int samples );

    virtual QList< glm::vec2 > GetSamples( int x, int y );

protected:
    std::mt19937 generator;
    std::uniform_real_distribution< float > distribution;
};

#endif // RANDOMPIXELSAMPLER_H

