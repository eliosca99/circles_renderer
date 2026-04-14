#include <algorithm>
#include <vector>
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
    //fa il blending e assegna il pixel all'immagine

    int cellWidth = width / GRID_SIZE;
    int cellHeight = height / GRID_SIZE;

    // forma split: hits è privato per thread, allocato una volta e riusato
    #pragma omp parallel
    {
        std::vector<int> hits;
        #pragma omp for schedule(dynamic)
        for(int index = 0; index < height * width; index++) {
            int i = index / width;
            int j = index % width;
            int row = std::min(i / cellHeight, GRID_SIZE - 1);
            int col = std::min(j / cellWidth, GRID_SIZE - 1);
            int n = counter[row][col];

            hits.resize(n);  // non rialloca se la capacity è già sufficiente

            // fase 1: test geometrico vettorizzabile (nessuna dipendenza tra k)
            #pragma omp simd
            for(int k = 0; k < n; k++) {
                int dy = i - (int)grid[row][col].y[k];
                int dx = j - (int)grid[row][col].x[k];
                int r  = (int)grid[row][col].radius[k];
                hits[k] = (dy*dy + dx*dx <= r*r) ? 1 : 0;
            }
            // fase 2: blend sequenziale (dipendenza su finalColor)
            sf::Color finalColor = sf::Color::Transparent;
            for(int k = 0; k < n; k++) {
                if (hits[k]) {
                    sf::Color color = sf::Color(grid[row][col].r[k],
                                                grid[row][col].g[k],
                                                grid[row][col].b[k],
                                                grid[row][col].a[k]);
                    finalColor = blend(finalColor, color);
                }
            }
            image.setPixel(sf::Vector2u(j, i), finalColor);
        }
    }
    texture.update(image);
    sprite.setTexture(texture);
}