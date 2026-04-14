#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <string>

class Renderer {
    protected:
        int numCircles;
        int width, height;
        std::string title;
        sf::RenderWindow window;
        sf::Image image;
        sf::Texture texture;
        sf::Sprite sprite;
        int** counter;
        int GRID_SIZE;
        sf::Color blend(sf::Color& background, sf::Color& color);
        virtual void assignCirclesToGrid() = 0;
        virtual void overlappedCells(int* xMin, int* xMax, int* yMin, int* yMax, int i) = 0;

    public:
        void setGridSize(int gs) { GRID_SIZE = gs; }
        Renderer(int width, int height, int numCircles, const std::string& title);
        virtual ~Renderer();
        virtual void generateCircles() = 0;
        virtual void generateImage() = 0;
        virtual void render() = 0;

};

#endif