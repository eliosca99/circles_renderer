#ifndef PARALLEL_RENDERER_BASE_H
#define PARALLEL_RENDERER_BASE_H

#include "renderer.h"
#include "utils.h"

class ParallelRendererBase : public Renderer {
    public:
        ParallelRendererBase(int width, int height, int numCircles);
        virtual ~ParallelRendererBase();
        void setCircles(Circle* circles);
        void generateCircles() override;
        void generateImage() override;
        void render() override;

    protected:
        Circle* circles;
        Circle*** grid;
        void assignCirclesToGrid() override;
        void overlappedCells(int* xMin, int* xMax, int* yMin, int* yMax, int i) override;
};


#endif