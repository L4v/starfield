#include "boids.h"

Boids *sfBoidsInit(Arena *arena, unsigned count) {
  Boids *boids = (Boids *)sfArenaAlloc(arena, sizeof(Boids));
  boids->count = count;
  boids->closeRange = 5.0f;
  boids->minSpeed = 1.0f;
  boids->maxSpeed = 5.0f;
  boids->avoidFactor = 0.1f;
  boids->alignmentFactor = 1.0f;
  boids->positions = (v3 *)sfArenaAlloc(arena, sizeof(v3) * boids->count);
  boids->velocities = (v3 *)sfArenaAlloc(arena, sizeof(v3) * boids->count);
  for (int i = 0; i < boids->count; ++i) {
    boids->velocities[i] = v3_randf_clamped(boids->minSpeed, boids->maxSpeed);
    boids->positions[i] = v3_randf_clamped(-5.0f, 5.0f);
  }

  return boids;
}

void sfBoidsUpdate(const State *state, Boids *boids) {
  for (int boidIdx = 0; boidIdx < boids->count; ++boidIdx) {
    v3 separationDp = v3_0();
    v3 avgNeighbourVelocities = v3_0();
    v3 avgNeighbourPositions = v3_0();
    unsigned neighbours = 0;
    for (int i = 0; i < boids->count; ++i) {
      if (i == boidIdx)
        continue;

      v3 boidVector = v3_sub(boids->positions[boidIdx], boids->positions[i]);
      float distance = v3_len(boidVector);
      if (distance <= boids->closeRange) {
        separationDp = v3_add(separationDp, boidVector);
      }

      if (distance > boids->closeRange && distance <= boids->viewRange) {
        ++neighbours;
        avgNeighbourVelocities =
            v3_add(avgNeighbourVelocities, boids->velocities[i]);

        avgNeighbourPositions =
            v3_add(avgNeighbourPositions, boids->positions[i]);
      }
    }

    separationDp = v3_scale(separationDp, boids->avoidFactor);
    boids->velocities[boidIdx] =
        v3_add(boids->velocities[boidIdx], separationDp);

    if (neighbours > 0) {
      avgNeighbourVelocities =
          v3_scale(avgNeighbourVelocities, 1.0f / neighbours);

      v3 alignmentDv =
          v3_scale(v3_sub(avgNeighbourVelocities, boids->velocities[boidIdx]),
                   boids->alignmentFactor);
      boids->velocities[boidIdx] =
          v3_add(boids->velocities[boidIdx], alignmentDv);

      avgNeighbourPositions =
          v3_scale(avgNeighbourPositions, 1.0f / neighbours);
      v3 cohesionDp =
          v3_scale(v3_sub(avgNeighbourPositions, boids->positions[boidIdx]),
                   boids->cohesionFactor);
      boids->velocities[boidIdx] =
          v3_add(boids->velocities[boidIdx], cohesionDp);
    }
  }

  for (int i = 0; i < boids->count; ++i) {
    v3 velocity =
        v3_clampf(boids->velocities[i], boids->minSpeed, boids->maxSpeed);
    v3 dp = v3_scale(velocity, state->dt);
    boids->positions[i] = v3_add(boids->positions[i], dp);
  }
}
