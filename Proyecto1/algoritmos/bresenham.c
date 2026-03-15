#include "../lineas.h"
#include <stdlib.h>

void bresenham(Linea linea, PlotFunc plot) {
    //  Cuánto se mueve en X
    int dx = abs(linea.x1 - linea.x0);
    // Cuánto se mueve en Y
    int dy = abs(linea.y1 - linea.y0);
    // Signo de X
    int sx = int_sign(linea.x1 - linea.x0);
    // Signo de Y
    int sy = int_sign(linea.y1 - linea.y0);

    int xp = linea.x0;
    int yp = linea.y0;

    // Si el movimiento en X es mayor o igual al movimiento en Y
    if (dx >= dy) {
        int delta_E = 2 * dy;
        int delta_NE = 2 * (dy - dx);
        int d = 2 * dy - dx;

        plot(xp, yp);
        while (xp != linea.x1) {
            if (d < 0) {
                d += delta_E;
            } else {
                yp += sy;
                d += delta_NE;
            }
            xp += sx;
            plot(xp, yp);
        }
    } else {
        // Si el movimiento en Y es mayor al movimiento en X
        int delta_N = 2 * dx;
        int delta_NE = 2 * (dx - dy);
        int d = 2 * dx - dy;

        plot(xp, yp);
        while (yp != linea.y1) {
            if (d < 0) {
                d += delta_N;
            } else {
                xp += sx;
                d += delta_NE;
            }
            yp += sy;
            plot(xp, yp);
        }
    }
}