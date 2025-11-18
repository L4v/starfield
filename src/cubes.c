#include "cubes.h"

void sfDestroyCubes(Cubes *cubes) {
  free(cubes->positions);
  free(cubes->velocities);
  free(cubes->accelerations);
  free(cubes->masses);
  free(cubes->sizes);
}

Cubes *sfCubesArenaAlloc(Arena *arena, unsigned count) {
  Cubes *cubes = (Cubes *)sfArenaAlloc(arena, sizeof(Cubes));

  cubes->count = count;

  cubes->masses = (float *)sfArenaAlloc(arena, sizeof(float) * cubes->count);
  cubes->positions = sfV3ArenaAlloc(arena, cubes->count);
  cubes->velocities = sfV3ArenaAlloc(arena, cubes->count);
  cubes->accelerations = sfV3ArenaAlloc(arena, cubes->count);
  cubes->sizes = (float *)sfArenaAlloc(arena, sizeof(float) * cubes->count);

  for (int i = 0; i < cubes->count; ++i) {
    cubes->sizes[i] = 1.0f;
  }

  return cubes;
}
