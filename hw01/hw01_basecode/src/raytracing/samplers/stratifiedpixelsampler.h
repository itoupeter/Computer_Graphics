#pragma once
#include <raytracing/samplers/pixelsampler.h>
#include <random>

class StratifiedPixelSampler : public PixelSampler
{
public:
    StratifiedPixelSampler();
    StratifiedPixelSampler(int samples);
    virtual QList<glm::vec2> GetSamples(int x, int y);

protected:
    std::mt19937 generator;
    std::uniform_real_distribution<float> distribution;
};
