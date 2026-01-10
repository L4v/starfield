#ifndef MATH3D_H
#define MATH3D_H

#ifndef L4VDEF
#define L4VDEF static
#endif
#include <stdio.h>

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#define SIN(x) sin(x)
#define COS(x) cos(x)
#define SQRT(x) sqrt(x)
#define TAN(x) tanf(x)
#define ACOS(x) acos(x)

#define PI 3.14159265358979323846f
#define PI_HALF PI / 2.0f
#define RAD PI / 180.0f
#define DEG 180.0f / PI
#define MAX_RAND RAND_MAX

L4VDEF float lerp(float a, float b, float t) { return a + (b - a) * t; }

L4VDEF float clampf(float value, float min, float max) {
  const float lower = value < min ? min : value;
  return lower > max ? max : lower;
};

L4VDEF float randf_clamped(float min, float max) {
  return min + ((float)rand() / RAND_MAX) * (max - min);
};

L4VDEF float rand_deg() {
  return (float)(rand() % 360) + (float)rand() / (float)MAX_RAND;
};

L4VDEF float fsignf(float x) { return (x > 0) - (x < 0); }

struct v2 {
  union {
    float v[2];
    struct {
      float x;
      float y;
    };
  };
};
typedef struct v2 v2;

L4VDEF v2 v2_make(float x, float y) {
  v2 result;
  result.x = x;
  result.y = y;
  return result;
}

L4VDEF v2 v2_0() { return v2_make(0.0f, 0.0f); }

L4VDEF float v2_len(const v2 v) { return SQRT(v.x * v.x + v.y * v.y); }

L4VDEF v2 v2_norm(const v2 v) {
  v2 result = v;
  float l = v2_len(v);
  result.x /= l;
  result.y /= l;
  return result;
}

L4VDEF v2 v2_add(const v2 a, const v2 b) {
  v2 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}

L4VDEF v2 v2_sub(const v2 a, const v2 b) {
  v2 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  return result;
}

L4VDEF v2 v2_scale(const v2 a, float s) {
  v2 result;
  result.x = a.x * s;
  result.y = a.y * s;
  return result;
}

L4VDEF float v2_dot(const v2 a, const v2 b) { return a.x * b.x + a.y * b.y; }

L4VDEF v2 v2_lerp(const v2 a, const v2 b, float t) {
  v2 scaled_a = v2_scale(a, 1.0f - t);
  v2 scaled_b = v2_scale(b, t);
  return v2_add(scaled_a, scaled_b);
}

struct v3 {
  union {
    float v[3];
    struct {
      float x;
      float y;
      float z;
    };
  };
};
typedef struct v3 v3;

L4VDEF v3 v3_make(float x, float y, float z) {
  v3 result = {0};
  result.x = x;
  result.y = y;
  result.z = z;
  return result;
}

L4VDEF void v3_zero(v3 *v) {
  v->x = 0.0f;
  v->y = 0.0f;
  v->z = 0.0f;
}

L4VDEF v3 v3_0() { return (v3){0.0f, 0.0f, 0.0f}; }

L4VDEF float v3_len(const v3 v) {
  return SQRT(v.x * v.x + v.y * v.y + v.z * v.z);
}

L4VDEF v3 v3_norm(const v3 v) {
  v3 result = v;
  float l = v3_len(v);
  if (l == 0.0f) {
    return v3_0();
  }
  result.x /= l;
  result.y /= l;
  result.z /= l;
  return result;
}

L4VDEF v3 v3_add(const v3 a, const v3 b) {
  v3 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  return result;
}

L4VDEF v3 v3_sub(const v3 a, const v3 b) {
  v3 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  return result;
}

L4VDEF v3 v3_neg(const v3 *v) {
  v3 result = v3_sub(v3_0(), *v);
  return result;
}

L4VDEF v3 v3_scale(const v3 a, float s) {
  v3 result;
  result.x = a.x * s;
  result.y = a.y * s;
  result.z = a.z * s;
  return result;
}

L4VDEF float v3_dot(const v3 a, const v3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

L4VDEF v3 v3_cross(const v3 a, const v3 b) {
  v3 result;
  result.x = a.y * b.z - a.z * b.y;
  result.y = a.z * b.x - a.x * b.z;
  result.z = a.x * b.y - a.y * b.x;
  return result;
}

L4VDEF v3 v3_lerp(const v3 a, const v3 b, float t) {
  v3 scaled_a = v3_scale(a, 1.0f - t);
  v3 scaled_b = v3_scale(b, t);
  return v3_add(scaled_a, scaled_b);
}

L4VDEF int v3_eq(const v3 a, const v3 b) {
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

// NOTE: Element-wise multiplication
L4VDEF v3 v3_mul(const v3 a, const v3 b) {
  return v3_make(a.x * b.x, a.y * b.y, a.z * b.z);
}

// NOTE: Element-wise comparisons
L4VDEF v3 v3_min(const v3 a, const v3 b) {
  return v3_make(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z));
}

// NOTE: Element-wise comparisons
L4VDEF v3 v3_max(const v3 a, const v3 b) {
  return v3_make(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z));
}

L4VDEF float v3_minf(const v3 v) { return fminf(v.x, fminf(v.y, v.z)); }
L4VDEF float v3_maxf(const v3 v) { return fmaxf(v.x, fmaxf(v.y, v.z)); }

struct v4 {
  union {
    float v[4];
    struct {
      float x;
      float y;
      float z;
      float w;
    };
  };
};
typedef struct v4 v4;

L4VDEF v4 v4_make(float x, float y, float z, float w) {
  v4 result = {0};
  result.x = x;
  result.y = y;
  result.z = z;
  result.w = w;
  return result;
}

L4VDEF v4 v4_0() { return (v4){0.0f, 0.0f, 0.0f, 0.0f}; }

L4VDEF float v4_len(const v4 v) {
  return SQRT(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

L4VDEF v4 v4_norm(const v4 v) {
  v4 result = v;
  float l = v4_len(v);
  result.x /= l;
  result.y /= l;
  result.z /= l;
  result.w /= l;
  return result;
}

L4VDEF v4 v4_add(const v4 *a, const v4 *b) {
  v4 result;
  result.x = a->x + b->x;
  result.y = a->y + b->y;
  result.z = a->z + b->z;
  result.w = a->w + b->w;
  return result;
}

L4VDEF v4 v4_sub(const v4 *a, const v4 *b) {
  v4 result;
  result.x = a->x - b->x;
  result.y = a->y - b->y;
  result.z = a->z - b->z;
  result.w = a->w - b->w;
  return result;
}

L4VDEF v4 v4_mul_scalar(const v4 *a, float s) {
  v4 result;
  result.x = a->x * s;
  result.y = a->y * s;
  result.z = a->z * s;
  result.w = a->w * s;
  return result;
}

/* TODO|NOTE(Jovan):
 * Inconsistency between the order in which values are stored: [x y z w]
 * and the order in which values are expected when calling the constructor:
 * [w x y z]. This is because GLTF stores the values as the former and this
 * avoids "messy" loading. Should make this consistent.
 */
// TODO(Jovan): Implement quats
#if 0
struct quat {
  v3 v;
  float r;
};

quat quat_make(float x, float y, float z, float w) {
  quat result;
  result.v = v3_make(x, y, z);
  result.r = w;
  return result;
}

quat quat_normize(const quat q) {
  quat result = q;
  float l = v3_len(q.v);
  result.v = v3_scale(q.v, 1.0f / l);
  result.r /= l;
  return result;
}

float quat_magnitude(const quat q) {
  return SQRT(q.r * q.r + q.v.x * q.v.x + q.v.y * q.v.y + q.v.z * q.v.z);
}

quat quat_add(const quat a, const quat b) {
  quat result;
  result.v = v3_add(a.v, b.v);
  result.r = a.r + b.r;
  return result;
}

quat quat_sub(const quat a, const quat b) {
  quat result;
  result.v = v3_sub(a.v, b.v);
  result.r = a.r - b.r;
  return result;
}

quat quat_scale(const quat a, float s) {
  quat result;
  result.v = v3_scale(a.v, s);
  result.r = a.r * s;
  return result;
}

float quat_dot(const quat a, const quat b) {
  return a.r * b.r + v3_dot(a.v, b.v);
}

quat quat_hamilton(const quat a, const quat b) {
  quat result;
  result.r = a.r * b.r - v3_dot(a.v, b.v);
  result.v = v3_add(v3_add(v3_scale(a.v, b.r), v3_scale(b.v, a.r)),
                    v3_cross(a.v, b.v));
  return result;
}

quat quat_conjugate(const quat q) {
  quat result;
  result.r = q.r;
  result.v = v3_scale(q.v, -1.0f);
  return result;
}

quat quat_inverse(const quat q) {
  quat result = quat_conjugate(q);
  float m = quat_magnitude(q);
  result = quat_scale(result, 1.0f / (m * m));
  return result;
}

/* TODO|NOTE(Jovan):
 *  Implemented by following GLM implementation.
 *  Implement with understanding.
 */
quat quat_slerp(const quat &q1, const quat &q2, float t) {
  quat tmp = q2;
  float cosTheta = quat_dot(q1, q2);

  if (cosTheta < 0.0f) {
    tmp = quat_scale(tmp, -1.0f);
    cosTheta = -cosTheta;
  }

  float epsilon = 1e-4;
  if (cosTheta > 1 - epsilon) {
    return quat_make(lerp(q1.r, tmp.r, t), lerp(q1.v.x, tmp.v.x, t),
                     lerp(q1.v.y, tmp.v.y, t), lerp(q1.v.z, tmp.v.z, t));
    float Angle = ACOS(cosTheta);
  quat tmp1 = quat_scale()(sin((1.0f - t) * Angle) * q1
    return (sin((1.0f - t) * Angle) * q1 + SIN(t + Angle) * Tmp) / SIN(Angle);
  }

#endif

/* NOTE(Jovan):
 * Each row is a separate vector. `x`, `y`, `z` and `w` are vector
 * components.
 * Example: `ax`, `ay`, `az`, `aw` are components of vector `a`
 */
// TODO(Jovan) : FIX rows->vectors rename, recheck everything;
struct m44 {
  union {
    v4 v[4];
    struct {
      float ax, ab, az, aw;
      float bx, by, bz, bw;
      float cx, cy, cz, cw;
      float dx, dy, dz, dw;
    };
  };
};
typedef struct m44 m44;

L4VDEF m44 m44_identity(float x) {
  m44 result = {0};
  for (int i = 0; i < 4; ++i) {
    result.v[i].v[i] = x;
  }

  return result;
}

L4VDEF m44 m44_make_v4(const v4 *row0, const v4 *row1, const v4 *row2,
                       const v4 *row3) {
  m44 result = {0};
  result.v[0].x = row0->x;
  result.v[0].y = row0->y;
  result.v[0].z = row0->z;
  result.v[0].w = row0->w;

  result.v[1].x = row1->x;
  result.v[1].y = row1->y;
  result.v[1].z = row1->z;
  result.v[1].w = row1->w;

  result.v[2].x = row2->x;
  result.v[2].y = row2->y;
  result.v[2].z = row2->z;
  result.v[2].w = row2->w;

  result.v[3].x = row3->x;
  result.v[3].y = row3->y;
  result.v[3].z = row3->z;
  result.v[3].w = row3->w;
  return result;
}

L4VDEF float m44_det(const m44 *m) {
  float z2w3 = m->cz * m->dw - m->dz * m->cw;
  float z1w3 = m->bz * m->dw - m->dz * m->bw;
  float z1w2 = m->bz * m->cw - m->cz * m->bw;
  float z0w3 = m->az * m->dw - m->dz * m->aw;
  float z0w1 = m->az * m->bw - m->bz * m->aw;
  float z0w2 = m->az * m->cw - m->cz * m->aw;

  float minor0 = m->by * z2w3 - m->cy * z1w3 + m->dy * z1w2;
  float minor1 = m->ab * z2w3 - m->cy * z0w3 + m->dy * z0w2;
  float minor2 = m->ab * z1w3 - m->by * z0w3 + m->dy * z0w1;
  float minor3 = m->ab * z1w2 - m->by * z0w2 + m->cy * z0w1;

  return m->ax * minor0 - m->bx * minor1 + m->cx * minor2 - m->dx * minor3;
}

L4VDEF m44 m44_mul(const m44 *a, const m44 *b) {
  m44 result = {0};

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      float sum = 0.0f;
      for (int k = 0; k < 4; ++k) {
        sum += a->v[i].v[k] * b->v[k].v[j];
      }
      result.v[i].v[j] = sum;
    }
  }

  return result;
}

L4VDEF m44 m44_v4_map(const m44 *a, const m44 *b,
                      v4 (*operation)(const v4 *, const v4 *)) {
  v4 ab0 = operation(&a->v[0], &b->v[0]);
  v4 ab1 = operation(&a->v[1], &b->v[1]);
  v4 ab2 = operation(&a->v[2], &b->v[2]);
  v4 ab3 = operation(&a->v[3], &b->v[3]);
  return m44_make_v4(&ab0, &ab1, &ab2, &ab3);
}

L4VDEF m44 m44_add(const m44 *a, const m44 *b) {
  return m44_v4_map(a, b, v4_add);
}

L4VDEF m44 m44_sub(const m44 *a, const m44 *b) {
  return m44_v4_map(a, b, v4_sub);
}

L4VDEF m44 m44_mul_scalar(const m44 *a, float s) {
  v4 as = v4_mul_scalar(&a->v[0], s);
  v4 bs = v4_mul_scalar(&a->v[1], s);
  v4 cs = v4_mul_scalar(&a->v[2], s);
  v4 ds = v4_mul_scalar(&a->v[3], s);
  return m44_make_v4(&as, &bs, &cs, &ds);
}

L4VDEF m44 m44_transpose(const m44 m) {
  m44 result = {m.ax, m.ab, m.az, m.aw, m.bx, m.by, m.bz, m.bw,
                m.cx, m.cy, m.cz, m.cw, m.dx, m.dy, m.dz, m.dw};
  return result;
}

// NOTE(Jovan): Inverse
L4VDEF m44 m44_inverse(const m44 m) {
  // NOTE(Jovan): Determinants of 2x2
  float y1z0 = m.by * m.az - m.ab * m.bz;
  float y2z0 = m.cy * m.az - m.ab * m.cz;
  float y2z1 = m.cy * m.bz - m.by * m.cz;
  float y1w0 = m.by * m.aw - m.ab * m.bw;
  float y2w0 = m.cy * m.aw - m.ab * m.cw;
  float y2w1 = m.cy * m.bw - m.by * m.cw;
  float y3z0 = m.dy * m.az - m.ab * m.dz;
  float y3z1 = m.dy * m.bz - m.by * m.dz;
  float y3z2 = m.dy * m.cz - m.cy * m.dz;
  float y3w0 = m.dy * m.aw - m.ab * m.dw;
  float y3w1 = m.dy * m.bw - m.by * m.dw;
  float y3w2 = m.dy * m.cw - m.cy * m.dw;
  float z1w0 = m.bz * m.aw - m.az * m.bw;
  float z2w0 = m.cz * m.aw - m.az * m.cw;
  float z2w1 = m.cz * m.bw - m.bz * m.cw;
  float z3w0 = m.dz * m.aw - m.az * m.dw;
  float z3w1 = m.dz * m.bw - m.bz * m.dw;
  float z3w2 = m.dz * m.cw - m.cz * m.dw;

  // NOTE(Jovan): Matrix minors
  float min00 = m.cy * z1w0 - m.by * z2w0 + m.ab * z2w1;
  float min01 = m.dy * z1w0 - m.by * z3w0 + m.ab * z3w1;
  float min02 = m.dy * z2w0 - m.cy * z3w0 + m.ab * z3w2;
  float min03 = m.dy * z2w1 - m.cy * z3w1 + m.by * z3w2;
  float min10 = m.cx * z1w0 - m.bx * z2w0 + m.ax * z2w1;
  float min11 = m.dx * z1w0 - m.bx * z3w0 + m.ax * z3w1;
  float min12 = m.dx * z2w0 - m.cx * z3w0 + m.ax * z3w2;
  float min13 = m.dx * z2w1 - m.cx * z3w1 + m.bx * z3w2;
  float min20 = m.cx * y1w0 - m.bx * y2w0 + m.ax * y2w1;
  float min21 = m.dx * y1w0 - m.bx * y3w0 + m.ax * y3w1;
  float min22 = m.dx * y2w0 - m.cx * y3w0 + m.ax * y3w2;
  float min23 = m.dx * y2w1 - m.cx * y3w1 + m.bx * y3w2;
  float min30 = m.cx * y1z0 - m.bx * y2z0 + m.ax * y2z1;
  float min31 = m.dx * y1z0 - m.bx * y3z0 + m.ax * y3z1;
  float min32 = m.dx * y2z0 - m.cx * y3z0 + m.ax * y3z2;
  float min33 = m.dx * y2z1 - m.cx * y3z1 + m.bx * y3z2;

  // NOTE(Jovan): Determinant of 4x4
  float iDet =
      1.0f / (m.dx * min00 - m.cx * min01 + m.bx * min02 - m.ax * min03);

  m44 transposedcofactor = {-min03, min02,  -min01, min00, min13,  -min12,
                            min11,  -min10, -min23, min22, -min21, min20,
                            min33,  -min32, min31,  -min30};

  return m44_mul_scalar(&transposedcofactor, iDet);
};

L4VDEF m44 translate(const m44 *m, float x, float y, float z) {
  m44 result = *m;
  result.dx += x * m->ax + y * m->bx + z * m->cx;
  result.dy += x * m->ab + y * m->by + z * m->cy;
  result.dz += x * m->az + y * m->bz + z * m->cz;
  result.dw += x * m->aw + y * m->bw + z * m->cw;
  return result;
}

L4VDEF m44 translate_v3(const m44 *m, const v3 *v) {
  return translate(m, v->x, v->y, v->z);
}

L4VDEF m44 scale(const m44 *m, float x, float y, float z) {
  m44 result = *m;
  result.ax *= x;
  result.ab *= x;
  result.az *= x;

  result.bx *= y;
  result.by *= y;
  result.bz *= y;

  result.cx *= z;
  result.cy *= z;
  result.cz *= z;
  return result;
}

L4VDEF m44 scale_v3(const m44 *m, const v3 *v) {
  return scale(m, v->x, v->y, v->z);
}

L4VDEF m44 rotate(const m44 *m, float angle, float ax, float ay, float az) {
  angle *= RAD;
  float c = COS(angle);
  float s = SIN(angle);
  float t = 1.0f - c;
  float xx = ax * ax;
  float xy = ax * ay;
  float xz = ax * az;
  float yy = ay * ay;
  float yz = ay * az;
  float zz = az * az;

  m44 rotation = {xx * t + c,
                  xy * t - az * s,
                  xz * t + ay * s,
                  0.0,
                  xy * t + az * s,
                  yy * t + c,
                  yz * t - ax * s,
                  0.0,
                  xz * t - ay * s,
                  yz * t + ax * s,
                  zz * t + c,
                  0.0,
                  0.0,
                  0.0,
                  0.0,
                  1.0

  };

  m44 result = m44_mul(&rotation, m);

  return result;
}

L4VDEF m44 perspective(float angleFOVY, float aspectRatio, float near,
                       float far) {
  float F = 1.0f / TAN((angleFOVY * RAD) / 2.0f);
  m44 result = {F / aspectRatio,
                0.0f,
                0.0f,
                0.0f,
                0.0f,
                F,
                0.0f,
                0.0f,
                0.0f,
                0.0f,
                (far + near) / (near - far),
                -1.0f,
                0.0f,
                0.0f,
                2.0f * far * near / (near - far),
                0.0f};
  return result;
}

L4VDEF m44 frustum(float left, float right, float bottom, float top, float near,
                   float far) {
  m44 result = {2 * near / (right - left),
                0.0f,
                (right + left) / (right - left),
                0.0f,
                0.0f,
                2 * near / (top - bottom),
                (top + bottom) / (top - bottom),
                0.0f,
                0.0f,
                0.0f,
                -(far + near) / (far - near),
                -2.0f * far * near / (far - near),
                0.0f,
                0.0f,
                -1.0,
                0.0};
  return result;
}

L4VDEF m44 orthographic(float left, float right, float bottom, float top,
                        float near, float far) {
  float SX = 2.0f / (right - left);
  float SY = 2.0f / (top - bottom);
  float SZ = -2.0f / (far - near);
  float TX = -(right + left) / (right - left);
  float TY = -(top + bottom) / (top - bottom);
  float TZ = -(far + near) / (far - near);
  m44 result = {SX,   0.0f, 0.0f, 0.0f, 0.0f, SY, 0.0f, 0.0f,
                0.0f, 0.0f, SZ,   0.0f, TX,   TY, TZ,   1.0f};
  return result;
}

L4VDEF m44 lookAt(const v3 eye, const v3 center, const v3 up) {
  v3 f = v3_norm(v3_sub(center, eye));
  v3 s = v3_norm(v3_cross(f, up));
  v3 u = v3_cross(s, f);
  m44 result = {s.x,
                u.x,
                -f.x,
                0.0f,
                s.y,
                u.y,
                -f.y,
                0.0f,
                s.z,
                u.z,
                -f.z,
                0.0f,
                -v3_dot(s, eye),
                -v3_dot(u, eye),
                v3_dot(f, eye),
                1.0f};
  return result;
}

L4VDEF void m44_print(m44 m) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      printf(" %0.2f", m.v[i].v[j]);
    }
    printf("\n");
  }
}

#endif
