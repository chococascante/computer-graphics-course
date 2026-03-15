#include "../lineas.h"
#include <stdlib.h>
#include <math.h>

void incremental_v2(Linea linea, PlotFunc plot) {
    long double x, y, x_step, y_step;
    int width;

    // Calcula el mayor de los deltas.
    width = fmax(abs(linea.x1 - linea.x0), abs(linea.y1 - linea.y0));
    // Si el ancho es 0, se plotea el punto inicial.
    if (width == 0) {
        plot(linea.x0, linea.y0);
        return;
    }

    // Calcula el incremento de X.
    x_step = (long double)(linea.x1 - linea.x0) / width;
    // Calcula el incremento de Y.
    y_step = (long double)(linea.y1 - linea.y0) / width;

    x = linea.x0;
    y = linea.y0;

    for (int i = 0; i < width; i++) {
        plot((int)round(x), (int)round(y));
        x += x_step;
        y += y_step;
    }
}

void incremental_v2_entero(Linea linea, PlotFunc plot) {
    int x, y, err_x, err_y;
    int width;

    int dx = abs(linea.x1 - linea.x0);
    int dy = abs(linea.y1 - linea.y0);
    int sx = int_sign(linea.x1 - linea.x0);
    int sy = int_sign(linea.y1 - linea.y0);

    width = dx > dy ? dx : dy;
    if (width == 0) {
        plot(linea.x0, linea.y0);
        return;
    }

    x = linea.x0;
    y = linea.y0;
    err_x = 0;
    err_y = 0;

    for (int i = 0; i < width; i++) {
        plot(x, y);
        err_x += dx;
        if (2 * err_x >= width) { x += sx; err_x -= width; }
        err_y += dy;
        if (2 * err_y >= width) { y += sy; err_y -= width; }
    }
}
