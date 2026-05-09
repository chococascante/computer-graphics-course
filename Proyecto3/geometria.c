#include "geometria.h"
#include <math.h>

int intersectar_esfera(const Esfera *e, Vec3 O, Vec3 D,
                       float tmin, float tmax, Impacto *salida) {
    Vec3 OC = vsub(O, e->centro);

    float A = vdot(D, D);
    float B = 2.0f * vdot(D, OC);
    float C = vdot(OC, OC) - e->radio * e->radio;
    float disc = B * B - 4.0f * A * C;

    if (disc < 0.0f) return 0;

    float sd = sqrtf(disc);
    float t1 = (-B - sd) / (2.0f * A);
    float t2 = (-B + sd) / (2.0f * A);

    float t = t1;
    if (t < tmin || t > tmax) {
        t = t2;
        if (t < tmin || t > tmax) return 0;
    }

    salida->t = t;
    salida->P = vadd(O, vscale(D, t));
    salida->N = vnorm(vsub(salida->P, e->centro));
    return 1;
}

int intersectar_escena(const Escena *esc, Vec3 O, Vec3 D,
                       float tmin, float tmax, Impacto *salida) {
    Impacto tmp;
    int hubo = 0;
    float mejor = tmax;
    for (int i = 0; i < esc->num_esferas; i++) {
        if (intersectar_esfera(&esc->esferas[i], O, D, tmin, mejor, &tmp)) {
            mejor = tmp.t;
            tmp.indice = i;
            *salida = tmp;
            hubo = 1;
        }
    }
    return hubo;
}

int hay_sombra(const Escena *esc, Vec3 P, Vec3 L, float dist_a_luz) {
    Impacto tmp;
    return intersectar_escena(esc, P, L, EPS, dist_a_luz - EPS, &tmp);
}
