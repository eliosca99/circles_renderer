#include "parallel_renderer_SoA_over_pixels.h"

ParallelRendererSoAOverPixels::ParallelRendererSoAOverPixels(int width, int height, int numCircles) : 
    ParallelRendererSoA(width, height, numCircles) {
}

void ParallelRendererSoAOverPixels::setCircles(Circles circles) {
    //metodo che setta i cerchi e inizializza la griglia
    this->circles = circles;
    assignCirclesToGrid();
}

void ParallelRendererSoAOverPixels::generateImage() {
    //metodo che itera sulle celle della griglia e per ogni cella
    //itera sui pixel che la compongono, verifica a quali cerchi appartiene,
    //fa il blending e assegna il pixel all'immagineParallelRendererSoAOverPixels(int width, int height, int numCircles);

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
            if ((i - grid[row][col].y[k]) * (i - grid[row][col].y[k]) + 
                (j - grid[row][col].x[k]) * (j - grid[row][col].x[k]) <= 
                grid[row][col].radius[k] * grid[row][col].radius[k]) {
                
                sf::Color color = sf::Color(grid[row][col].r[k], 
                                            grid[row][col].g[k], 
                                            grid[row][col].b[k], 
                                            grid[row][col].a[k]);
                finalColor = blend(finalColor, color);
            }
        }
        image.setPixel(sf::Vector2u(j, i), finalColor);
    }
    texture.update(image);
    sprite.setTexture(texture);
}