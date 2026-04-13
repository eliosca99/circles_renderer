#include "parallel_renderer_base.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <optional>
#include <ctime>
#include <cmath>
#include <omp.h>
#include "utils.h"

ParallelRendererBase::ParallelRendererBase(int width, int height, int numCircles) : 
    Renderer(width, height, numCircles, "Random circles rendering - Parallel") {
    }

void ParallelRendererBase::generateCircles() {
    //genero i cerchi casualmente in base alla finestra e alla
    //dimensione della griglia
    
    circles = new Circle[numCircles];
    
    srand(static_cast<unsigned int>(time(0)));

    int minRadius = 10;
    int maxRadius = (std::min(width, height)) / (GRID_SIZE * 2);

    
    for (int i = 0; i < numCircles; i++) {
        circles[i].x = rand() % width;
        circles[i].y = rand() % height;
        circles[i].z = rand() % 256;
        circles[i].radius = minRadius + (rand() % (maxRadius - minRadius + 1));
        circles[i].r = rand() % 256;
        circles[i].g = rand() % 256;
        circles[i].b = rand() % 256;
        circles[i].a = rand() % 256;
    }

    assignCirclesToGrid();
}

void ParallelRendererBase::setCircles(Circle* circles) {
    this->circles = circles;
    assignCirclesToGrid();
}

void ParallelRendererBase::assignCirclesToGrid() {
    //metodo che assegna i cerchi alle celle della griglia a cui appartengono
    
    int cellWidth = width / GRID_SIZE; 
    int cellHeight = height / GRID_SIZE;

    //counter mi serve per contare quanti cerchi andranno in ogni cella in modo da allocarla
    counter = new int*[GRID_SIZE];
    for(int i = 0; i < GRID_SIZE; i++) {
        counter[i] = new int[GRID_SIZE];
    }
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            counter[i][j] = 0;
        }
    }

    //conto
    for(int i = 0; i < numCircles; i++) {
        int xMin, xMax, yMin, yMax;
        overlappedCells(&xMin, &xMax, &yMin, &yMax, i);

        for(int gy = yMin; gy <= yMax; gy++) {
            for(int gx = xMin; gx <= xMax; gx++) {
                counter[gy][gx]++;
            }
        }
    }
    
    int cnt = 0;
    //alloco
    grid = new Circle**[GRID_SIZE];
    for(int i = 0; i < GRID_SIZE; i++) {
        grid[i] = new Circle*[GRID_SIZE];
        for(int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = new Circle[counter[i][j]];
            cnt += counter[i][j];
        }
    }
    //std::cout << "Redundancy ratio: " << cnt / (float)numCircles << std::endl;
    

    //popolo la griglia, ma prima mi copio counter dato che qui lo decremento via via
    int** counterCopy = new int*[GRID_SIZE];
    for(int i = 0; i < GRID_SIZE; i++) {
        counterCopy[i] = new int[GRID_SIZE];
    }
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            counterCopy[i][j] = counter[i][j];
        }
    }

    for(int i = 0; i < numCircles; i++) {
        int xMin, xMax, yMin, yMax;
        overlappedCells(&xMin, &xMax, &yMin, &yMax, i);
        
        for(int gy = yMin; gy <= yMax; gy++) {
            for(int gx = xMin; gx <= xMax; gx++) {
                grid[gy][gx][counterCopy[gy][gx] - 1] = circles[i];
                counterCopy[gy][gx]--;
            }
        }
    }


    //ordino i cerchi in ogni cella
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            Utils::quickSort(grid[i][j], 0, counter[i][j] - 1);
        }
    }

    //elimino i counter e l'array di cerchi
    delete[] counterCopy;
    //delete[] circles;
}

void ParallelRendererBase::overlappedCells(int* xMin, int* xMax, int* yMin, int* yMax, int i) {
    //metodo che serve per calcolare le celle della griglia a cui appartiene il cerchio i
    
    int cellWidth = width / GRID_SIZE; 
    int cellHeight = height / GRID_SIZE;
    
    *xMin = std::max(0, (circles[i].x - circles[i].radius) / cellWidth);
    *xMax = std::min(GRID_SIZE - 1, (circles[i].x + circles[i].radius) / cellWidth);
    *yMin = std::max(0, (circles[i].y - circles[i].radius) / cellHeight);
    *yMax = std::min(GRID_SIZE - 1, (circles[i].y + circles[i].radius) / cellHeight);
}

void ParallelRendererBase::generateImage() {
    //metodo che itera sulle celle della griglia e per ogni cella
    //itera sui pixel che la compongono, verifica a quali cerchi appartiene,
    //fa il blending e assegna il pixel all'immagine
    
    
    //itero su ogni cella della griglia
    //e per ogni cella prendo itero sui pixel che la compongono
    //e prendo i cerchi a cui il pixel appartiene
    int cellWidth = width / GRID_SIZE;
    int cellHeight = height / GRID_SIZE;
    
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for(int row = 0; row < GRID_SIZE; row++) {
        for(int col = 0; col < GRID_SIZE; col++) {
            for(int i = (cellHeight * row); i < (cellHeight) * (row + 1); i++) {
                for(int j = (cellWidth * col); j < (cellWidth) * (col + 1); j++) {
                    sf::Color finalColor = sf::Color::Transparent;
                    for(int k = 0; k < counter[row][col]; k++) {
                        if ((i - grid[row][col][k].y) * (i - grid[row][col][k].y) + (j - grid[row][col][k].x) * (j - grid[row][col][k].x) <= grid[row][col][k].radius * grid[row][col][k].radius) {
                            //se il pixel i,j è dentro il cerchio k allora faccio il blending
                            sf::Color color = sf::Color(grid[row][col][k].r, grid[row][col][k].g, grid[row][col][k].b, grid[row][col][k].a);
                            finalColor = blend(finalColor, color);
                        }
                    }
                    image.setPixel(sf::Vector2u(j, i), finalColor);
                }
            }
            
        }
    }
    texture.update(image);
    sprite.setTexture(texture);
}

void ParallelRendererBase::render() {
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}