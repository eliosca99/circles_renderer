#include <iostream>
#include "parallel_renderer_over_pixels.h"

ParallelRendererOverPixels::ParallelRendererOverPixels(int width, int height, int numCircles) : 
    ParallelRendererBase(width, height, numCircles) {
}

void ParallelRendererOverPixels::setCircles(Circle* circles) {
    this->circles = circles;
    assignCirclesToGrid();
}

void ParallelRendererOverPixels::generateImage() {
    //metodo che itera sulle celle della griglia e per ogni cella
    //itera sui pixel che la compongono, verifica a quali cerchi appartiene,
    //fa il blending e assegna il pixel all'immagine
    
    //itero su ogni cella della griglia
    //e per ogni cella prendo itero sui pixel che la compongono
    //e prendo i cerchi a cui il pixel appartiene
    int cellWidth = width / GRID_SIZE;
    int cellHeight = height / GRID_SIZE;
    #pragma omp parallel for schedule(dynamic)
    for(int index = 0; index < GRID_SIZE * GRID_SIZE * cellHeight * cellWidth; index++) {
        int row = (index / (GRID_SIZE * cellHeight * cellWidth)) % GRID_SIZE;
        int col = (index / (cellHeight * cellWidth)) % GRID_SIZE;
        int i = (index / cellWidth) % cellHeight + row * cellHeight;
        int j = index % cellWidth + col * cellWidth;

        sf::Color finalColor = sf::Color::Transparent;
        for(int k = 0; k < counter[row][col]; k++) {
            if ((i - grid[row][col][k].y) * (i - grid[row][col][k].y) + 
                (j - grid[row][col][k].x) * (j - grid[row][col][k].x) <= 
                grid[row][col][k].radius * grid[row][col][k].radius) {
                
                sf::Color color = sf::Color(grid[row][col][k].r, 
                                            grid[row][col][k].g, 
                                            grid[row][col][k].b, 
                                            grid[row][col][k].a);
                finalColor = blend(finalColor, color);
            }
        }
        image.setPixel(sf::Vector2u(j, i), finalColor);
    }
    texture.update(image);
    sprite.setTexture(texture);
}