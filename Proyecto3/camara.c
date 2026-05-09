#include "camara.h"

Vec3 direccion_rayo(const Escena *esc, int i, int j,
                    float ancho_plano, float alto_plano) {
    float u = ((float)i + 0.5f) / (float)esc->ancho - 0.5f;
    float v = 0.5f - ((float)j + 0.5f) / (float)esc->alto;
    Vec3 punto_plano = v3(esc->ojo.x + u * ancho_plano,
                          esc->ojo.y + v * alto_plano,
                          esc->ojo.z - 1.0f);
    return vnorm(vsub(punto_plano, esc->ojo));
}
