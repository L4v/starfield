#include "arena.h"
#include "common.h"
#include "math3d.h"

#define MAX_CUBES 4096

typedef struct {
  unsigned count;
  float *masses;
  float *sizes;

  v3 *positions;
  v3 *velocities;
  v3 *accelerations;
} Cubes;

void sfDestroyCubes(Cubes *cubes);
Cubes *sfCubesArenaAlloc(Arena *arena, unsigned count);
