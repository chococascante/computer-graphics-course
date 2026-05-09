/*
 * escena.h — Carga la escena desde un archivo de texto.
 *
 * Sintaxis del archivo:
 *   RES     <ancho> <alto>
 *   OUT     <archivo.ppm>
 *   MODE    FILE | WINDOW | BOTH
 *   SHADING PHONG | FLAT
 *   AMBIENT <Ia_r Ia_g Ia_b>
 *   BG      <r g b>
 *   CAM     <ojo_x ojo_y ojo_z> <fov_grados>
 *   LIGHT   <px py pz> <Ir Ig Ib>
 *   SPHERE  <cx cy cz> <radio> <Kd_r Kd_g Kd_b> <Ks> <n>
 *
 * Líneas vacías y las que empiezan con '#' se ignoran.
 */
#ifndef ESCENA_H
#define ESCENA_H

#include "tipos.h"

int cargar_escena(const char *ruta, Escena *esc);

#endif
