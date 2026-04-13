#ifndef SEQUENTIAL_RENDERER_BASE_H
#define SEQUENTIAL_RENDERER_BASE_H

#include "renderer.h"
#include "utils.h"


class SequentialRendererBase : public Renderer {
    public:
        SequentialRendererBase(int width, int height, int numCircles);
        void setCircles(Circle* circles);
        void generateCircles() override;
        void generateImage() override;
        void render() override;

    private:
        Circle* circles;
        Circle*** grid;
        void assignCirclesToGrid() override;
        void overlappedCells(int* xMin, int* xMax, int* yMin, int* yMax, int i) override;
};

#endif

