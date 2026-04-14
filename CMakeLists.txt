cmake_minimum_required(VERSION 3.5)

project(primo_lab_parallel)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

find_package(OpenMP REQUIRED)

include_directories((include))

add_executable(renderer src/main.cpp src/renderer.cpp src/sequential_renderer_base.cpp src/parallel_renderer_base.cpp 
                src/sequential_renderer_mem_opt.cpp src/utils.cpp src/parallel_renderer_over_pixels.cpp src/parallel_renderer_SoA.cpp
                src/parallel_renderer_SoA_over_pixels.cpp)

target_link_libraries(renderer SFML::Graphics SFML::Window SFML::System)

target_link_libraries(renderer OpenMP::OpenMP_CXX)
