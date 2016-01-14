#include <renderthread.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>

QList< int > RenderThread::pixel_coords;
QMutex RenderThread::mutex;

RenderThread::RenderThread( unsigned int samplesSqrt, unsigned int depth, Film *f, Camera *c, Integrator *i)
    : samples_sqrt(samplesSqrt), max_depth(depth), film(f), camera(c), integrator(i)
{}

void RenderThread::run()
{
    unsigned int seed( 1000.f * distribution( generator ) );
    StratifiedPixelSampler pixel_sampler(samples_sqrt, seed);

    int x( 0 ), y( 0 ), rand( 0 );
    QList< int > &pixel_coords = RenderThread::pixel_coords;
    QMutex &mutex = RenderThread::mutex;

    for( ; ; ){
        mutex.lock();
        rand = pixel_coords.front();
        pixel_coords.pop_front();
        mutex.unlock();

        x = ( rand >> 16 ) & 0xffff;
        y = rand & 0xffff;

        glm::vec3 pixel_color;
        QList<glm::vec2> samples = pixel_sampler.GetSamples( x, y );
        for(int i = 0; i < samples.size(); i++)
        {
            Ray ray = camera->Raycast(samples[i]);
            pixel_color += integrator->TraceRay(ray, 0);
        }
        pixel_color /= samples.size();

        float weight( film->pixels_weights[ x ][ y ] );
        glm::vec3 pixel_color_o( film->pixels[ x ][ y ] );
        film->pixels_weights[ x ][ y ] += 1;
        film->pixels[ x ][ y ] = ( pixel_color + pixel_color_o * weight ) / ( weight + 1.f );

        mutex.lock();
        pixel_coords.push_back( rand );
        mutex.unlock();
    }
}
