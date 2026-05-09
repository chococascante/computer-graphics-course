#include "sombreado.h"
#include "geometria.h"
#include <math.h>

static Vec3 saturar(Vec3 c) {
    if (c.x < 0) c.x = 0; else if (c.x > 1) c.x = 1;
    if (c.y < 0) c.y = 0; else if (c.y > 1) c.y = 1;
    if (c.z < 0) c.z = 0; else if (c.z > 1) c.z = 1;
    return c;
}

Vec3 sombrear_plano(const Escena *esc, const Impacto *imp) {
    return saturar(esc->esferas[imp->indice].Kd);
}

Vec3 sombrear_phong(const Escena *esc, Vec3 origen_camara, const Impacto *imp) {
    const Esfera *E = &esc->esferas[imp->indice];
    Vec3 N = imp->N;
    Vec3 V = vnorm(vsub(origen_camara, imp->P));

    /* Aporte ambiente: I_a = Kd · I_ambiente   (Ka = Kd) */
    Vec3 color = vmul(E->Kd, esc->Ia);

    for (int i = 0; i < esc->num_luces; i++) {
        Vec3 hacia_luz = vsub(esc->luces[i].posicion, imp->P);
        float dist = vlen(hacia_luz);
        Vec3 L = vscale(hacia_luz, 1.0f / fmaxf(dist, 1e-12f));

        Vec3 P_off = vadd(imp->P, vscale(N, EPS));
        if (hay_sombra(esc, P_off, L, dist)) continue;

        float NdotL = fmaxf(0.0f, vdot(N, L));
        Vec3 I_luz = esc->luces[i].intensidad;

        /* Difusa Lambert */
        Vec3 I_d = vscale(vmul(E->Kd, I_luz), NdotL);

        /* Especular Phong: R = 2(N·L)N − L */
        Vec3 R = vsub(vscale(N, 2.0f * NdotL), L);
        R = vnorm(R);
        float RdotV = fmaxf(0.0f, vdot(R, V));
        Vec3 I_s = vscale(I_luz, E->Ks * powf(RdotV, E->n));

        color = vadd(color, vadd(I_d, I_s));
    }
    return saturar(color);
}

Vec3 trazar_color(const Escena *esc, Vec3 O, Vec3 D) {
    Impacto imp = {0};
    if (!intersectar_escena(esc, O, D, EPS, 1e9f, &imp)) return esc->fondo;
    if (esc->sombreado == SOMBREADO_PLANO) return sombrear_plano(esc, &imp);
    return sombrear_phong(esc, O, &imp);
}
