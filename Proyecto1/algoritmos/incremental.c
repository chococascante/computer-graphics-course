#include "../lineas.h"
#include <math.h>

void incremental(Linea linea, PlotFunc plot) {
    if (draw_vertical_or_normalize(&linea, plot)) return;

    long double m = (long double)(linea.y1 - linea.y0) / (linea.x1 - linea.x0);
    long double y = linea.y0;

    for (int x = linea.x0; x <= linea.x1; x++) {
        plot(x, (int)round(y));
        y += m;
    }
}

