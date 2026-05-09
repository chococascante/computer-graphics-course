/*
 * tipos.h — Tipos de dominio compartidos por todos los módulos.
 * Esfera, Luz, Escena, Impacto, modos de render y de sombreado.
 */
#ifndef TIPOS_H
#define TIPOS_H

#include "vec3.h"

#define MAX_ESFERAS 256
#define MAX_LUCES   32
#define EPS         1e-4f

typedef struct {
    Vec3 centro;
    float radio;
    Vec3 Kd;       /* coeficiente difuso (RGB) */
    float Ks;      /* coeficiente especular */
    float n;       /* exponente de brillo (shininess) */
} Esfera;

typedef struct {
    Vec3 posicion;
    Vec3 intensidad;
} Luz;

typedef enum {
    MODO_ARCHIVO = 1,
    MODO_VENTANA = 2,
    MODO_AMBOS   = 3
} ModoRender;

typedef enum {
    SOMBREADO_PHONG = 0,
    SOMBREADO_PLANO = 1
} ModoSombreado;

typedef struct {
    int ancho, alto;
    Vec3 ojo;
    float fov_grados;
    Vec3 Ia;            /* luz ambiental global */
    Vec3 fondo;
    Esfera esferas[MAX_ESFERAS];
    int num_esferas;
    Luz luces[MAX_LUCES];
    int num_luces;
    char ruta_salida[512];
    ModoRender modo;
    ModoSombreado sombreado;
} Escena;

typedef struct {
    float t;
    int indice;
    Vec3 N;             /* normal en el punto de impacto */
    Vec3 P;             /* punto de impacto */
} Impacto;

#endif
