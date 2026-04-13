#ifndef TRANSFORM_H
#define TRANSFORM_H

/* Qué trozo de mundo es visible; zoom/pan solo tocan esto, no los vértices del SVG. */
typedef struct VentanaMundo {
    double wxmin, wxmax;
    double wymin, wymax;
} VentanaMundo;

/* Rectángulo en píxeles del framebuffer (origen arriba-izquierda, eje y hacia abajo). */
typedef struct RectVista {
    int vxmin, vxmax;
    int vymin, vymax;
} RectVista;

void mundo_a_rectvista(double x_mundo, double y_mundo, const VentanaMundo *ventana, const RectVista *vista,
                       int *x_pix, int *y_pix);

#endif
