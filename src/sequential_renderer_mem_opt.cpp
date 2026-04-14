#include "sequential_renderer_mem_opt.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <optional>
#include <ctime>
#include <cmath>
#include <chrono>
#include "utils.h"

SequentialRendererMemOpt::SequentialRendererMemOpt(int width, int height, int numCircles) :
    Renderer(width, height, numCircles, "Random circles rendering - Sequential Memory Optimization"), grid(nullptr) {
}

SequentialRendererMemOpt::~SequentialRendererMemOpt() {
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

void SequentialRendererMemOpt::generateCircles() {
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

void SequentialRendererMemOpt::setCircles(Circles circles) {
    this->circles = circles;
    assignCirclesToGrid();
}

void SequentialRendererMemOpt::assignCirclesToGrid() {
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

void SequentialRendererMemOpt::overlappedCells(int* xMin, int* xMax, int* yMin, int* yMax, int i) {
    //metodo che serve per calcolare le celle della griglia a cui appartiene il cerchio i
    
    int cellWidth = width / GRID_SIZE; 
    int cellHeight = height / GRID_SIZE;
    
    *xMin = std::max(0, (circles.x[i] - circles.radius[i]) / cellWidth);
    *xMax = std::min(GRID_SIZE - 1, (circles.x[i] + circles.radius[i]) / cellWidth);
    *yMin = std::max(0, (circles.y[i] - circles.radius[i]) / cellHeight);
    *yMax = std::min(GRID_SIZE - 1, (circles.y[i] + circles.radius[i]) / cellHeight);
}

void SequentialRendererMemOpt::generateImage() {
    //metodo che itera sulle celle della griglia e per ogni cella
    //itera sui pixel che la compongono, verifica a quali cerchi appartiene,
    //fa il blending e assegna il pixel all'immagine
    
    
    //itero su ogni cella della griglia
    //e per ogni cella prendo itero sui pixel che la compongono
    //e prendo i cerchi a cui il pixel appartiene

    

    int cellWidth = width / GRID_SIZE;
    int cellHeight = height / GRID_SIZE;
    for(int row = 0; row < GRID_SIZE; row++) {
        for(int col = 0; col < GRID_SIZE; col++) {
            int iEnd = (row == GRID_SIZE - 1) ? height : cellHeight * (row + 1);
            int jEnd = (col == GRID_SIZE - 1) ? width : cellWidth * (col + 1);
            for(int i = cellHeight * row; i < iEnd; i++) {
                for(int j = cellWidth * col; j < jEnd; j++) {
                    sf::Color finalColor = sf::Color::Transparent;
                    for(int k = 0; k < counter[row][col]; k++) {
                        if ((i - grid[row][col].y[k]) * (i - grid[row][col].y[k]) + (j - grid[row][col].x[k]) * (j - grid[row][col].x[k]) <= grid[row][col].radius[k] * grid[row][col].radius[k]) {
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

void SequentialRendererMemOpt::render() {
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