#ifndef LINEAS_H
#define LINEAS_H

typedef struct {
    int x0, y0, x1, y1;
} Linea;

// Abstracción de qué función se está usando para plotear
typedef void (*PlotFunc)(int x, int y);

static inline int int_sign(int a) {
    return (a > 0) - (a < 0);
}

static inline void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static inline int draw_vertical_or_normalize(Linea *l, PlotFunc plot) {
    if (l->x0 == l->x1) {
        if (l->y0 > l->y1) swap(&l->y0, &l->y1);
        for (int y = l->y0; y <= l->y1; y++)
            plot(l->x0, y);
        return 1;
    }
    if (l->x0 > l->x1) {
        swap(&l->x0, &l->x1);
        swap(&l->y0, &l->y1);
    }
    return 0;
}

void brute_force(Linea linea, PlotFunc plot);
void incremental(Linea linea, PlotFunc plot);
void incremental_v2(Linea linea, PlotFunc plot);
void incremental_v2_entero(Linea linea, PlotFunc plot);
void bresenham(Linea linea, PlotFunc plot);

#endif
