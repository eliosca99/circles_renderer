#ifndef SEQUENTIAL_RENDERER_MEM_OPT_H
#define SEQUENTIAL_RENDERER_MEM_OPT_H

#include "renderer.h"
#include "utils.h"

class SequentialRendererMemOpt : public Renderer {
    public:
        SequentialRendererMemOpt(int width, int height, int numCircles);
        void setCircles(Circles circles);
        void generateCircles() override;
        void generateImage() override;
        void render() override;

    private:
        Circles circles;
        Circles** grid;
        void assignCirclesToGrid() override;
        void overlappedCells(int* xMin, int* xMax, int* yMin, int* yMax, int i);
};

#endif