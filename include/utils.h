#ifndef UTILS_H
#define UTILS_H

struct Circle {
    short x, y, z, radius;
    unsigned char r, g, b, a;
};

struct Circles {
    short* x;
    short* y;
    short* z;
    short* radius;
    unsigned char* r;
    unsigned char* g;
    unsigned char* b;
    unsigned char* a;
};

class Utils {
    public:
        static void quickSort(Circle* arr, int low, int high);
        static void quickSort(Circles* arr, int low, int high);
        static Circle* generateCirclesAoS(int numCircles, int width, int height, int grid_size = 4);
        static Circles generateCirclesSoA(int numCircles, int width, int height, int grid_size = 4);
            
        
    private:
        static int partition(Circle* arr, int low, int high);
        static int partition(Circles* arr, int low, int high);
}; 

#endif