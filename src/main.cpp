#include <iostream>
#include <SFML/Graphics.hpp>
#include "renderer.h"
#include "sequential_renderer_base.h"
#include "sequential_renderer_mem_opt.h"
#include "parallel_renderer_base.h"
#include "parallel_renderer_over_pixels.h"
#include "parallel_renderer_SoA.h"
#include "parallel_renderer_SoA_over_pixels.h"
#include "utils.h"
#include <omp.h>
#include <chrono>
#include <fstream>
#include <vector>


int main() {
    std::vector<int> circleCounts = {1000, 5000, 10000, 20000, 50000};
    std::vector<int> gridSizes = {4, 8, 16, 32};
    std::vector<int> threadCounts = {4, 8, 12, 16};
    const int numIterations = 1;
    int width = 1920;
    int height = 1080;

    std::ofstream csvFile("benchmark_results.csv");
    if (!csvFile.is_open()) {
        std::cerr << "Errore nell'apertura del file CSV!" << std::endl;
        return 1;
    }

    csvFile << "NumCircles,GridSize,NumThreads,Sequential,SequentialMemOpt,Parallel,ParallelOverPixels,ParallelSoA,ParallelSoAOverPixels\n";

    for (int numCircles : circleCounts) {
        for (int gridSize : gridSizes) {
            std::cout << "\n--- Esecuzione Benchmark per " << numCircles << " cerchi e GridSize " << gridSize << " ---" << std::endl;

            Circle* circlesAoS = Utils::generateCirclesAoS(numCircles, width, height, gridSize);
            Circles circlesSoA = Utils::generateCirclesSoA(numCircles, width, height, gridSize);

            double avgSeq = 0, avgSeqMem = 0;

            // 1. Sequential Base
            SequentialRendererBase seqRendererBase(width, height, numCircles);
            seqRendererBase.setGridSize(gridSize);
            seqRendererBase.setCircles(circlesAoS);
            for (int i = 0; i < numIterations; ++i) {
                auto startSeq = std::chrono::high_resolution_clock::now();
                seqRendererBase.generateImage();
                auto endSeq = std::chrono::high_resolution_clock::now();
                avgSeq += std::chrono::duration<double>(endSeq - startSeq).count();
            }
            avgSeq /= numIterations;
            std::cout << "Sequential rendering time: " << avgSeq << " s" << std::endl;

            // 2. Sequential Mem Opt
            SequentialRendererMemOpt seqRendererMemOpt(width, height, numCircles);
            seqRendererMemOpt.setGridSize(gridSize);
            seqRendererMemOpt.setCircles(circlesSoA);
            for (int i = 0; i < numIterations; ++i) {
                auto startSeqMem = std::chrono::high_resolution_clock::now();
                seqRendererMemOpt.generateImage();
                auto endSeqMem = std::chrono::high_resolution_clock::now();
                avgSeqMem += std::chrono::duration<double>(endSeqMem - startSeqMem).count();
            }
            avgSeqMem /= numIterations;
            std::cout << "Sequential Mem Opt rendering time: " << avgSeqMem << " s" << std::endl;

            for (int numThreads : threadCounts) {
                omp_set_num_threads(numThreads);
                std::cout << "\n--- Test Paralleli con " << numThreads << " Threads ---" << std::endl;

                double avgPar = 0, avgParOverPixels = 0, avgParSoA = 0, avgParSoAOverPixels = 0;

                // 3. Parallel Base
                ParallelRendererBase parRendererBase(width, height, numCircles);
                parRendererBase.setGridSize(gridSize);
                parRendererBase.setCircles(circlesAoS);
                for (int i = 0; i < numIterations; ++i) {
                    auto startPar = std::chrono::high_resolution_clock::now();
                    parRendererBase.generateImage();
                    auto endPar = std::chrono::high_resolution_clock::now();
                    avgPar += std::chrono::duration<double>(endPar - startPar).count();
                }
                avgPar /= numIterations;
                std::cout << "Parallel (" << numThreads << " threads) rendering time: " << avgPar << " s" << std::endl;

                // 4. Parallel Over Pixels
                ParallelRendererOverPixels parRendererOverPixels(width, height, numCircles);
                parRendererOverPixels.setGridSize(gridSize);
                parRendererOverPixels.setCircles(circlesAoS);
                for (int i = 0; i < numIterations; ++i) {
                    auto startParOverPixels = std::chrono::high_resolution_clock::now();
                    parRendererOverPixels.generateImage();
                    auto endParOverPixels = std::chrono::high_resolution_clock::now();
                    avgParOverPixels += std::chrono::duration<double>(endParOverPixels - startParOverPixels).count();
                }
                avgParOverPixels /= numIterations;
                std::cout << "Parallel Over Pixels (" << numThreads << " threads) rendering time: " << avgParOverPixels << " s" << std::endl;

                // 5. Parallel SoA
                ParallelRendererSoA parRendererSoA(width, height, numCircles);
                parRendererSoA.setGridSize(gridSize);
                parRendererSoA.setCircles(circlesSoA);
                for (int i = 0; i < numIterations; ++i) {
                    auto startParSoA = std::chrono::high_resolution_clock::now();
                    parRendererSoA.generateImage();
                    auto endParSoA = std::chrono::high_resolution_clock::now();
                    avgParSoA += std::chrono::duration<double>(endParSoA - startParSoA).count();
                }
                avgParSoA /= numIterations;
                std::cout << "Parallel SoA (" << numThreads << " threads) rendering time: " << avgParSoA << " s" << std::endl;

                // 6. Parallel SoA Over Pixels
                ParallelRendererSoAOverPixels parRendererSoAOverPixels(width, height, numCircles);
                parRendererSoAOverPixels.setGridSize(gridSize);
                parRendererSoAOverPixels.setCircles(circlesSoA);
                for (int i = 0; i < numIterations; ++i) {
                    auto startParSoAOverPixels = std::chrono::high_resolution_clock::now();
                    parRendererSoAOverPixels.generateImage();
                    auto endParSoAOverPixels = std::chrono::high_resolution_clock::now();
                    avgParSoAOverPixels += std::chrono::duration<double>(endParSoAOverPixels - startParSoAOverPixels).count();
                }
                avgParSoAOverPixels /= numIterations;
                std::cout << "Parallel SoA Over Pixels (" << numThreads << " threads) rendering time: " << avgParSoAOverPixels << " s" << std::endl;

                // Scrittura dei dati su CSV
                csvFile << numCircles << ","
                        << gridSize << ","
                        << numThreads << ","
                        << avgSeq << ","
                        << avgSeqMem << ","
                        << avgPar << ","
                        << avgParOverPixels << ","
                        << avgParSoA << ","
                        << avgParSoAOverPixels << "\n";
                csvFile.flush();
            }
                    
            delete[] circlesAoS;
            delete[] circlesSoA.x;
            delete[] circlesSoA.y;
            delete[] circlesSoA.z;
            delete[] circlesSoA.radius;
            delete[] circlesSoA.r;
            delete[] circlesSoA.g;
            delete[] circlesSoA.b;
            delete[] circlesSoA.a;
        }
    }

    csvFile.close();
    std::cout << "\nBenchmark completato! Risultati salvati in 'benchmark_results.csv'." << std::endl;

    return 0;
}
