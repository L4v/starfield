#include "collision.h"

int rayRectangleIntersection(const v3 *rayOrigin, const v3 *rayDir,
                             const v3 targetBounds[2], float *tMin,
                             v3 *normal) {
  v3 inv = v3_make(1.0f / rayDir->x, 1.0f / rayDir->y, 1.0f / rayDir->z);
  v3 t0 = v3_mul(v3_sub(targetBounds[0], *rayOrigin), inv);
  v3 t1 = v3_mul(v3_sub(targetBounds[1], *rayOrigin), inv);

  v3 tNear = v3_min(t0, t1);
  v3 tFar = v3_max(t0, t1);

  *tMin = v3_maxf(tNear);
  float tMax = v3_minf(tFar);

  if (tMax < 0 || *tMin > 1.0f || *tMin > tMax) {
    return 0;
  }

  if (*tMin == tNear.x) {
    *normal = v3_make(-fsignf(rayDir->x), 0.0f, 0.0f);
  } else if (*tMin == tNear.y) {
    *normal = v3_make(0.0f, -fsignf(rayDir->y), 0.0f);
  } else {
    *normal = v3_make(0.0f, 0.0f, -fsignf(rayDir->z));
  }

  return 1;
}

int sfAabbCheckCollision(const AabbCollider *collider, const v3 targetBounds[2],
                         AabbResult *out) {

  if (v3_iszero(&collider->direction)) {
    return 0;
  }
  v3 minkBounds[2] = {v3_sub(targetBounds[0], v3_scale(collider->size, 0.5f)),
                      v3_add(targetBounds[1], v3_scale(collider->size, 0.5f))};
  int collision =
      rayRectangleIntersection(&collider->position, &collider->direction,
                               minkBounds, &out->tMin, &out->normal);

  return collision;
}
