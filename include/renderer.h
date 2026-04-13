#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>

class Renderer {
    protected:
        int numCircles;
        int width, height;
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
        Renderer(int numCircles, int width, int height, const std::string& title);
        virtual void generateCircles() = 0;
        virtual void generateImage() = 0;
        virtual void render() = 0;
        
};

#endif