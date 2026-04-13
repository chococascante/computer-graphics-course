#ifndef SCENE_GEOMETRY_H
#define SCENE_GEOMETRY_H

#include "clipping.h"

typedef struct {
    const Vertice *puntos;
    int num_vertices;
    unsigned char r, g, b;
    int cerrado;
    int indice_textura;
} PoligonoEscena;

void construir_escena(void);
void centro_geometrico_escena_mundo(double *cx, double *cy);
int num_poligonos_escena(void);
const PoligonoEscena *poligono_escena_en(int i);

void asignar_texturas_aleatorias(int num_texturas);
void actualizar_mapa_uv_global(void);
void actualizar_mapa_uv_local(void);

void imprimir_diagnosticos_escena(void);

#endif
