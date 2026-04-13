#include "transform.h"
#include <math.h>

/* Normalización a [0,1] dentro de la ventana mundo y escalado al rect de píxeles. Y hacia abajo. */
void mundo_a_rectvista(double x_mundo, double y_mundo, const VentanaMundo *ventana, const RectVista *vista,
                       int *x_pix, int *y_pix) {
    double ancho_m = ventana->wxmax - ventana->wxmin;
    double alto_m = ventana->wymax - ventana->wymin;
    double ancho_v = (double)(vista->vxmax - vista->vxmin);
    double alto_v = (double)(vista->vymax - vista->vymin);

    double tx = ancho_m != 0.0 ? (x_mundo - ventana->wxmin) / ancho_m : 0.0;
    double ty = alto_m != 0.0 ? (y_mundo - ventana->wymin) / alto_m : 0.0;

    /* Evita píxeles enormes (NaN/Inf o recorte numérico) que rompen Bresenham por overflow en abs(dx). */
    const double lim = 3000000.0;
    double xd = tx * ancho_v + (double)vista->vxmin;
    double yd = ty * alto_v + (double)vista->vymin;
    if (xd < -lim) xd = -lim;
    else if (xd > lim) xd = lim;
    if (yd < -lim) yd = -lim;
    else if (yd > lim) yd = lim;

    *x_pix = (int)floor(xd + 0.5);
    *y_pix = (int)floor(yd + 0.5);
}
