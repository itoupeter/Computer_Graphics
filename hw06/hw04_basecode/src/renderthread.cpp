#include <renderthread.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>

RenderThread::RenderThread(unsigned int xstart, unsigned int xend, unsigned int ystart, unsigned int yend, unsigned int samplesSqrt, unsigned int depth, Film *f, Camera *c, Integrator *i)
    : x_start(xstart), x_end(xend), y_start(ystart), y_end(yend), samples_sqrt(samplesSqrt), max_depth(depth), film(f), camera(c), integrator(i)
{}

void RenderThread::run()
{
    unsigned int seed = (((x_start << 16 | x_end) ^ x_start) * ((y_start << 16 | y_end) ^ y_start));
    StratifiedPixelSampler pixel_sampler(samples_sqrt, seed);

    int pseudo_rand( 0 );
    int X_len( x_end - x_start );
    int Y_len( y_end - y_start );
    int XY( X_len * Y_len );
    int X_new( 0 ), Y_new( 0 );

    for(unsigned int Y = y_start; Y < y_end; Y++)
    {
        for(unsigned int X = x_start; X < x_end; X++)
        {
            pseudo_rand = ( pseudo_rand + 971 ) % XY;
            Y_new = pseudo_rand / X_len + y_start;
            X_new = pseudo_rand % X_len + x_start;

            glm::vec3 pixel_color;
            QList<glm::vec2> samples = pixel_sampler.GetSamples(X_new, Y_new);
            for(int i = 0; i < samples.size(); i++)
            {
                Ray ray = camera->Raycast(samples[i]);
                pixel_color += integrator->TraceRay(ray, 0);
            }
            pixel_color /= samples.size();
            film->pixels[X_new][Y_new] = pixel_color;
        }
    }
}
