#include "particles.h"

void initBuffers(Particles *particles) {
  glGenVertexArrays(1, &particles->vao);
  glBindVertexArray(particles->vao);

  glGenBuffers(1, &particles->positionsVbo);
  glBindBuffer(GL_ARRAY_BUFFER, particles->positionsVbo);
  glBufferData(GL_ARRAY_BUFFER, particles->count * sizeof(v3),
               particles->positions, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(v3),
                        (void *)0);
  glEnableVertexAttribArray(POSITION_LOCATION);

  glGenBuffers(1, &particles->velocitiesVbo);
  glBindBuffer(GL_ARRAY_BUFFER, particles->velocitiesVbo);
  glBufferData(GL_ARRAY_BUFFER, particles->count * sizeof(v3),
               particles->velocities, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(VELOCITY_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(v3),
                        (void *)0);
  glEnableVertexAttribArray(VELOCITY_LOCATION);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // glGenBuffers(1, &particles->transformsVbo);
  // glBindBuffer(GL_ARRAY_BUFFER, particles->transformsVbo);
  // glBufferData(GL_ARRAY_BUFFER, particles->count * sizeof(m44),
  //              particles->transforms, GL_DYNAMIC_DRAW);
  //
  // for (int i = 0; i < 4; ++i) {
  //   glVertexAttribPointer(INSTANCE_TRANSFORM_LOCATION + i, 4, GL_FLOAT,
  //                         GL_FALSE, sizeof(m44), (void *)(sizeof(v4) * i));
  //   glEnableVertexAttribArray(INSTANCE_TRANSFORM_LOCATION + i);
  //   glVertexAttribDivisor(INSTANCE_TRANSFORM_LOCATION + i, 1);
  // }
}

Particles *sfParticlesArenaAlloc(Arena *arena, unsigned int count) {
  Particles *particles = (Particles *)sfArenaAlloc(arena, sizeof(Particles));
  particles->count = count;
  particles->positions = sfV3ArenaAlloc(arena, count);
  particles->velocities = sfV3ArenaAlloc(arena, count);

  initBuffers(particles);

  return particles;
}

void sfParticlesDestroy(Particles *particles) {
  free(particles->positions);
  free(particles->velocities);
}

void sfParticlesRender(const Particles *particles) {
  glBindVertexArray(particles->vao);

  glBindBuffer(GL_ARRAY_BUFFER, particles->positionsVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v3) * particles->count,
                  particles->positions);

  glBindBuffer(GL_ARRAY_BUFFER, particles->velocitiesVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v3) * particles->count,
                  particles->velocities);

  glDrawArrays(GL_POINTS, 0, particles->count);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
