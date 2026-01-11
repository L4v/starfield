#ifndef COLLISION_H
#define COLLISION_H
#include "math3d.h"

typedef struct {
  v3 normal;
  float tMin;
} AabbResult;

typedef struct {
  v3 position;
  v3 direction;
  v3 size;
} AabbCollider;

int rayRectangleIntersection(const v3 *rayOrigin, const v3 *rayDir,
                             const v3 targetBounds[2], float *tMin, v3 *normal);

int sfAabbCheckCollision(const AabbCollider *collider, const v3 targetBounds[2],
                         AabbResult *out);

#endif
