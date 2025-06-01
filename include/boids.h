#ifndef BOIDS_H
#define BOIDS_H
#include "arena.h"
#include "math3d.h"
#include "starfield.h"

typedef struct {
  unsigned count;
  float minSpeed;
  float maxSpeed;
  float closeRange;
  float viewRange;
  float avoidFactor;
  float alignmentFactor;
  float cohesionFactor;
  v3 *positions;
  v3 *velocities;
} Boids;

Boids *sfBoidsInit(Arena *arena, unsigned count);
void sfBoidsUpdate(const State *state, Boids *boids);

#endif
