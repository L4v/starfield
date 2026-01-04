#ifndef PARTICLES_H
#define PARTICLES_H
#include "arena.h"
#include "common.h"
#include "math3d.h"
#include <glad/glad.h>

const static unsigned POSITION_LOCATION = 0;
const static unsigned VELOCITY_LOCATION = 1;

typedef struct {
  unsigned vao;
  unsigned positionsVbo;
  unsigned velocitiesVbo;
  unsigned count;

  float *ttls;
  v3 *positions;
  v3 *velocities;
} Particles;

void initBuffers(Particles *particles);

Particles *sfParticlesArenaAlloc(Arena *arena, unsigned count);
void sfParticlesDestroy(Particles *particles);
void sfParticlesRender(const Particles *particles);

#endif
