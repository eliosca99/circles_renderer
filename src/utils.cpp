#include "utils.h"
#include <algorithm>
#include <time.h>
#include "renderer.h"


void Utils::quickSort(Circle* arr, int low, int high) {
    if (low < high) {
        int pi = Utils::partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int Utils::partition(Circle* arr, int low, int high) {
    Circle pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if (arr[j].z > pivot.z || 
        (arr[j].z == pivot.z && arr[j].radius < pivot.radius) || 
        (arr[j].z == pivot.z && arr[j].radius == pivot.radius && arr[j].a < pivot.a) || 
        (arr[j].z == pivot.z && arr[j].radius == pivot.radius && arr[j].a == pivot.a && arr[j].x < pivot.x) || 
        (arr[j].z == pivot.z && arr[j].radius == pivot.radius && arr[j].a == pivot.a && arr[j].x == pivot.x && arr[j].y < pivot.y)) {
        i++;
        std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void Utils::quickSort(Circles* arr, int low, int high) {
    if (low < high) {
        int pi = Utils::partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int Utils::partition(Circles* arr, int low, int high) {
    int pivot = high;
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if (arr->z[j] > arr->z[pivot] || 
        (arr->z[j] == arr->z[pivot] && arr->radius[j] < arr->radius[pivot]) || 
        (arr->z[j] == arr->z[pivot] && arr->radius[j] == arr->radius[pivot] && arr->a[j] < arr->a[pivot]) || 
        (arr->z[j] == arr->z[pivot] && arr->radius[j] == arr->radius[pivot] && arr->a[j] == arr->a[pivot] && arr->x[j] < arr->x[pivot]) ||
        (arr->z[j] == arr->z[pivot] && arr->radius[j] == arr->radius[pivot] && arr->a[j] == arr->a[pivot] && arr->x[j] == arr->x[pivot] && arr->y[j] < arr->y[pivot])) {
        i++;
        std::swap(arr->x[i], arr->x[j]);
        std::swap(arr->y[i], arr->y[j]);
        std::swap(arr->z[i], arr->z[j]);
        std::swap(arr->radius[i], arr->radius[j]);
        std::swap(arr->r[i], arr->r[j]);
        std::swap(arr->g[i], arr->g[j]);
        std::swap(arr->b[i], arr->b[j]);
        std::swap(arr->a[i], arr->a[j]);
        }
    }
    std::swap(arr->x[i + 1], arr->x[high]);
    std::swap(arr->y[i + 1], arr->y[high]);
    std::swap(arr->z[i + 1], arr->z[high]);
    std::swap(arr->radius[i + 1], arr->radius[high]);
    std::swap(arr->r[i + 1], arr->r[high]);
    std::swap(arr->g[i + 1], arr->g[high]);
    std::swap(arr->b[i + 1], arr->b[high]);
    std::swap(arr->a[i + 1], arr->a[high]);
    return (i + 1);
}

Circle* Utils::generateCirclesAoS(int numCircles, int width, int height, int grid_size) {
    Circle* circles = new Circle[numCircles];

    srand(static_cast<unsigned int>(time(0)));

    int minRadius = 10;
    int maxRadius = 100;

    
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
    return circles;
}

Circles Utils::generateCirclesSoA(int numCircles, int width, int height, int grid_size) {
    Circles circles;
    
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
    int maxRadius = (std::min(width, height)) / (grid_size * 2);

    
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

    return circles;
}
