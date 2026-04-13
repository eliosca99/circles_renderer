#ifndef PARALLEL_RENDERER_OVER_PIXELS_H
#define PARALLEL_RENDERER_OVER_PIXELS_H

#include "parallel_renderer_base.h"

class ParallelRendererOverPixels : public ParallelRendererBase {
    public:
        ParallelRendererOverPixels(int width, int height, int numCircles);
        void setCircles(Circle* circles);
        void generateImage() override;

};


#endif