#include "../lineas.h"
#include <math.h>

void brute_force(Linea linea, PlotFunc plot) {
    if (draw_vertical_or_normalize(&linea, plot)) return;

    long double m = (long double)(linea.y1 - linea.y0) / (linea.x1 - linea.x0);
    long double b = linea.y0 - m * linea.x0;

    for (int x = linea.x0; x <= linea.x1; x++) {
        long double y = m * x + b;
        plot(x, (int)round(y));
    }
}
