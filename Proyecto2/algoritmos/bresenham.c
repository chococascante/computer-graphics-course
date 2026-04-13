#include "../lineas.h"
#include <stdlib.h>

/* Enteros; el callback ya clampa al framebuffer. Cortamos si el segmento sería absurdo de largo. */
void bresenham(Linea linea, PlotFunc plot) {
    long long dxl = (long long)linea.x1 - (long long)linea.x0;
    long long dyl = (long long)linea.y1 - (long long)linea.y0;
    long long adxl = dxl >= 0 ? dxl : -dxl;
    long long adyl = dyl >= 0 ? dyl : -dyl;
    int sx = (dxl > 0) - (dxl < 0);
    int sy = (dyl > 0) - (dyl < 0);

    int xp = linea.x0;
    int yp = linea.y0;

    /* Límite de pasos: evita ciclos infinitos si abs(x1-x0) desborda en int. */
    long long max_ll = adxl + adyl + 4;
    int max_steps = max_ll > 4000000 ? 4000000 : (int)max_ll;
    int steps = 0;

    int dx = (int)adxl;
    int dy = (int)adyl;
    if (dx != (int)adxl || dy != (int)adyl) {
        plot(xp, yp);
        return;
    }

    if (dx >= dy) {
        int delta_E = 2 * dy;
        int delta_NE = 2 * (dy - dx);
        int d = 2 * dy - dx;

        plot(xp, yp);
        while (xp != linea.x1) {
            if (++steps > max_steps)
                return;
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
        int delta_N = 2 * dx;
        int delta_NE = 2 * (dx - dy);
        int d = 2 * dx - dy;

        plot(xp, yp);
        while (yp != linea.y1) {
            if (++steps > max_steps)
                return;
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
