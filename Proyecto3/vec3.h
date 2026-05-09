/*
 * vec3.h — Primitivas vectoriales 3D (header-only inline).
 * Todas las operaciones del raytracer se construyen sobre estas.
 */
#ifndef VEC3_H
#define VEC3_H

#include <math.h>

typedef struct { float x, y, z; } Vec3;

static inline Vec3 v3(float x, float y, float z) {
    Vec3 v = {x, y, z};
    return v;
}

static inline Vec3 vadd(Vec3 a, Vec3 b) {
    return v3(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline Vec3 vsub(Vec3 a, Vec3 b) {
    return v3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline Vec3 vmul(Vec3 a, Vec3 b) {
    return v3(a.x * b.x, a.y * b.y, a.z * b.z);
}

static inline Vec3 vscale(Vec3 a, float s) {
    return v3(a.x * s, a.y * s, a.z * s);
}

static inline float vdot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float vlen(Vec3 a) {
    return sqrtf(vdot(a, a));
}

static inline Vec3 vnorm(Vec3 a) {
    float l = vlen(a);
    if (l < 1e-12f) return v3(0, 0, 1);
    return vscale(a, 1.0f / l);
}

#endif
