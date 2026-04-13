#ifndef VIEW_CONTROL_H
#define VIEW_CONTROL_H

#include "clipping.h"
#include "transform.h"

/* Tope por polígono al rotar en main; debe ser >= el max num_points del Mustang. */
#define TAM_BUF_VERTICES 128

void acercar_o_alejar(VentanaMundo *ventana, double factor);
void desplazar_ventana(VentanaMundo *ventana, double dx, double dy);
void rotar_punto(double x, double y, double cx, double cy, double rad, double *ox, double *oy);
void copiar_poligono_rotado(const Vertice *origen, int n, double cx, double cy, double rad, Vertice *destino);

#endif
