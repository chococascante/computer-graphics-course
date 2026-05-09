/*
 * geometria.h — Intersección rayo-esfera y consultas de visibilidad.
 *
 * Resuelve la cuadrática estándar:
 *     A = D·D
 *     B = 2·D·(O − C)
 *     C = (O − C)·(O − C) − r²
 *     Δ = B² − 4·A·C
 *     t = (−B ± √Δ) / (2A)
 */
#ifndef GEOMETRIA_H
#define GEOMETRIA_H

#include "tipos.h"

int intersectar_esfera(const Esfera *e, Vec3 O, Vec3 D,
                       float tmin, float tmax, Impacto *salida);

int intersectar_escena(const Escena *esc, Vec3 O, Vec3 D,
                       float tmin, float tmax, Impacto *salida);

int hay_sombra(const Escena *esc, Vec3 P, Vec3 L, float dist_a_luz);

#endif
