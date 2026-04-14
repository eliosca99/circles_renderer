#ifndef PARALLEL_RENDERER_SOA_H
#define PARALLEL_RENDERER_SOA_H
#include "renderer.h"
#include "utils.h"

class ParallelRendererSoA : public Renderer {
    public:
        ParallelRendererSoA(int width, int height, int numCircles);
        virtual ~ParallelRendererSoA();
        void setCircles(Circles circles);
        void generateCircles() override;
        void generateImage() override;
        void render() override;

    protected:
        Circles circles;
        Circles** grid;
        void assignCirclesToGrid() override;
        void overlappedCells(int* xMin, int* xMax, int* yMin, int* yMax, int i);
};

#endif