/*
 * sombreado.h — Modelo de iluminación Phong.
 *
 *   Difusa (Lambert):   I_d = Kd · I_luz · max(0, N·L)
 *   Especular (Phong):  I_s = Ks · I_luz · max(0, R·V)^n
 *                       R   = 2·(N·L)·N − L
 *   Ambiente:           I_a = Kd · I_ambiente   (Ka = Kd)
 *
 * Plano: el color de la esfera es directamente Kd, sin sombras ni luces.
 */
#ifndef SOMBREADO_H
#define SOMBREADO_H

#include "tipos.h"

Vec3 sombrear_plano(const Escena *esc, const Impacto *imp);
Vec3 sombrear_phong(const Escena *esc, Vec3 origen_camara, const Impacto *imp);
Vec3 trazar_color(const Escena *esc, Vec3 O, Vec3 D);

#endif
