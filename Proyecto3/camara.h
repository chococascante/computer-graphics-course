/*
 * camara.h — Cámara fija mirando hacia -Z.
 *
 * Plano de proyección a distancia 1 al frente del ojo:
 *     alto_plano  = 2·tan(fov/2)
 *     ancho_plano = aspecto · alto_plano
 * Para el píxel (i, j) la dirección del rayo es:
 *     px_mundo = ojo.x + ((i + 0.5)/W − 0.5) · ancho_plano
 *     py_mundo = ojo.y + (0.5 − (j + 0.5)/H) · alto_plano
 *     pz_mundo = ojo.z − 1
 *     D = normalizar(pixel − ojo)
 */
#ifndef CAMARA_H
#define CAMARA_H

#include "tipos.h"

Vec3 direccion_rayo(const Escena *esc, int i, int j,
                    float ancho_plano, float alto_plano);

#endif
