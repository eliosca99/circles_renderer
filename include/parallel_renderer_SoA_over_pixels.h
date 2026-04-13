#ifndef PARALLEL_RENDERER_SOA_OVER_PIXELS_H
#define PARALLEL_RENDERER_SOA_OVER_PIXELS_H

#include "parallel_renderer_SoA.h"

class ParallelRendererSoAOverPixels : public ParallelRendererSoA {
    public:
        ParallelRendererSoAOverPixels(int width, int height, int numCircles);
        void setCircles(Circles circles);
        void generateImage() override;
};

#endif