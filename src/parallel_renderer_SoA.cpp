#include "parallel_renderer_SoA.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <optional>
#include <ctime>
#include <cmath>
#include <chrono>
#include <omp.h>
#include "utils.h"

ParallelRendererSoA::ParallelRendererSoA(int width, int height, int numCircles) :
    Renderer(width, height, numCircles, "Random circles rendering - Parallel SoA"), grid(nullptr) {
}

ParallelRendererSoA::~ParallelRendererSoA() {
    if (grid) {
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                delete[] grid[i][j].x;
                delete[] grid[i][j].y;
                delete[] grid[i][j].z;
                delete[] grid[i][j].radius;
                delete[] grid[i][j].r;
                delete[] grid[i][j].g;
                delete[] grid[i][j].b;
                delete[] grid[i][j].a;
            }
            delete[] grid[i];
        }
        delete[] grid;
    }
}

void ParallelRendererSoA::generateCircles() {
    //genero i cerchi casualmente in base alla finestra e alla
    //dimensione della griglia
    
    circles.x = new short[numCircles];
    circles.y = new short[numCircles];
    circles.z = new short[numCircles];
    circles.radius = new short[numCircles];
    circles.r = new unsigned char[numCircles];
    circles.g = new unsigned char[numCircles];
    circles.b = new unsigned char[numCircles];
    circles.a = new unsigned char[numCircles];
    
    srand(static_cast<unsigned int>(time(0)));

    int minRadius = 10;
    int maxRadius = (std::min(width, height)) / (GRID_SIZE * 2);

    
    for (int i = 0; i < numCircles; i++) {
        circles.x[i] = rand() % width;
        circles.y[i] = rand() % height;
        circles.z[i] = rand() % 256;
        circles.radius[i] = minRadius + (rand() % (maxRadius - minRadius + 1));
        circles.r[i] = rand() % 256;
        circles.g[i] = rand() % 256;
        circles.b[i] = rand() % 256;
        circles.a[i] = rand() % 256;
    }

    assignCirclesToGrid();
}

void ParallelRendererSoA::setCircles(Circles circles) {
    this->circles = circles;
    assignCirclesToGrid();
}

void ParallelRendererSoA::assignCirclesToGrid() {
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
    
    //alloco
    grid = new Circles*[GRID_SIZE];
    for(int i = 0; i < GRID_SIZE; i++) {
        grid[i] = new Circles[GRID_SIZE];
        for(int j = 0; j < GRID_SIZE; j++) {
            grid[i][j].x = new short[counter[i][j]];
            grid[i][j].y = new short[counter[i][j]];
            grid[i][j].z = new short[counter[i][j]];
            grid[i][j].radius = new short[counter[i][j]];
            grid[i][j].r = new unsigned char[counter[i][j]];
            grid[i][j].g = new unsigned char[counter[i][j]];
            grid[i][j].b = new unsigned char[counter[i][j]];
            grid[i][j].a = new unsigned char[counter[i][j]];
        }
    }
    

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
                grid[gy][gx].x[counterCopy[gy][gx] - 1] = circles.x[i];
                grid[gy][gx].y[counterCopy[gy][gx] - 1] = circles.y[i];
                grid[gy][gx].z[counterCopy[gy][gx] - 1] = circles.z[i];
                grid[gy][gx].radius[counterCopy[gy][gx] - 1] = circles.radius[i];
                grid[gy][gx].r[counterCopy[gy][gx] - 1] = circles.r[i];
                grid[gy][gx].g[counterCopy[gy][gx] - 1] = circles.g[i];
                grid[gy][gx].b[counterCopy[gy][gx] - 1] = circles.b[i];
                grid[gy][gx].a[counterCopy[gy][gx] - 1] = circles.a[i];
                counterCopy[gy][gx]--;
            }
        }
    }


    //ordino i cerchi in ogni cella
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            Utils::quickSort(&grid[i][j], 0, counter[i][j] - 1);
        }
    }

    //elimino i counter e l'array di cerchi
    for (int i = 0; i < GRID_SIZE; i++) delete[] counterCopy[i];
    delete[] counterCopy;
}

void ParallelRendererSoA::overlappedCells(int* xMin, int* xMax, int* yMin, int* yMax, int i) {
    //metodo che serve per calcolare le celle della griglia a cui appartiene il cerchio i
    
    int cellWidth = width / GRID_SIZE; 
    int cellHeight = height / GRID_SIZE;
    
    *xMin = std::max(0, (circles.x[i] - circles.radius[i]) / cellWidth);
    *xMax = std::min(GRID_SIZE - 1, (circles.x[i] + circles.radius[i]) / cellWidth);
    *yMin = std::max(0, (circles.y[i] - circles.radius[i]) / cellHeight);
    *yMax = std::min(GRID_SIZE - 1, (circles.y[i] + circles.radius[i]) / cellHeight);
}

void ParallelRendererSoA::generateImage() {
    //metodo che itera sulle celle della griglia e per ogni cella
    //itera sui pixel che la compongono, verifica a quali cerchi appartiene,
    //fa il blending e assegna il pixel all'immagine

    int cellWidth = width / GRID_SIZE;
    int cellHeight = height / GRID_SIZE;

    #pragma omp parallel for collapse(2) schedule(dynamic)
    for(int row = 0; row < GRID_SIZE; row++) {
        for(int col = 0; col < GRID_SIZE; col++) {
            // hits è privato per ogni thread (dichiarato nel corpo del loop parallelo)
            int n = counter[row][col];
            std::vector<int> hits(n);
            for(int i = cellHeight * row; i < std::min(height, cellHeight * (row + 1)); i++) {
                for(int j = cellWidth * col; j < std::min(width, cellWidth * (col + 1)); j++) {
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
                            sf::Color color = sf::Color(grid[row][col].r[k], grid[row][col].g[k], grid[row][col].b[k], grid[row][col].a[k]);
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

void ParallelRendererSoA::render() {
    if (!window.isOpen())
        window.create(sf::VideoMode(sf::Vector2u(width, height)), title);
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