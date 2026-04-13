#ifndef LINEAS_H
#define LINEAS_H

/* Pixels; usado solo después de mundo_a_rectvista + CS. */
typedef struct {
    int x0, y0, x1, y1;
} Linea;

typedef void (*PlotFunc)(int x, int y);

static inline int int_sign(int a) {
    return (a > 0) - (a < 0);
}

static inline void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void bresenham(Linea linea, PlotFunc plot);

#endif
