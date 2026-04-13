#ifndef CLIPPING_H
#define CLIPPING_H

#include "transform.h"

typedef struct {
    double x, y, u, v; /* x,y mundo; u,v para textura (relleno) */
} Vertice;

#define MAX_VERTICES_RECORTE 512

int recortar_arista_cs(Vertice *a, Vertice *b, const VentanaMundo *ventana);

int recortar_poligono_sh(const Vertice *entrada, int n_entrada, Vertice *salida, int max_salida,
                         const VentanaMundo *ventana);

#endif
