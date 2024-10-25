#include "cubes.h"

Cubes sfCreateCubes(unsigned count) {
  Cubes cubes = {0};

  cubes.count = count;
  cubes.positions = calloc(count, sizeof(v3));
  cubes.velocities = calloc(count, sizeof(v3));
  cubes.accelerations = calloc(count, sizeof(v3));
  cubes.speeds = calloc(count, sizeof(float));

  return cubes;
}

void sfDestroyCubes(Cubes *cubes) {
  free(cubes->positions);
  free(cubes->velocities);
  free(cubes->accelerations);
  free(cubes->speeds);
}

Cubes *sfCubesArenaAlloc(Arena *arena, unsigned count) {
  Cubes *cubes = (Cubes *)sfArenaAlloc(arena, sizeof(Cubes));

  cubes->count = count;

  cubes->speeds = (float *)sfArenaAlloc(arena, sizeof(float) * cubes->count);
  cubes->positions = sfV3ArenaAlloc(arena, cubes->count);
  cubes->velocities = sfV3ArenaAlloc(arena, cubes->count);
  cubes->accelerations = sfV3ArenaAlloc(arena, cubes->count);

  return cubes;
}
