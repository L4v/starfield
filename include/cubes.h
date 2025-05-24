#include "arena.h"
#include "common.h"
#include "math3d.h"

#define MAX_CUBES 4096
#define CUBE_VERTEX_COUNT 8

static const v3 CUBE_LOCAL_VERTICES[CUBE_VERTEX_COUNT] = {
    {-0.5f, -0.5f, 0.5f}, // 0
    {0.5f, -0.5f, 0.5f},  // 1
    {-0.5f, 0.5f, 0.5f},  // 2
    {0.5f, 0.5f, 0.5f},   // 3
    {-0.5f, -0.5, -0.5f}, // 4
    {-0.5f, 0.5, -0.5f},  // 5
    {0.5f, -0.5f, -0.5f}, // 6
    {0.5f, 0.5f, -0.5f},  // 7
};

typedef struct {
  unsigned count;
  float *speeds;

  v3 *positions;
  v3 *velocities;
  v3 *accelerations;
  v3 *vertices;
} Cubes;

Cubes *sfCubesArenaAlloc(Arena *arena, unsigned count);
v3 sfCubeSupport(const Cubes *cubes, unsigned index, v3 direction);
v3 sfCubesSupport(const Cubes *cubes, unsigned idx1, unsigned idx2,
                  v3 direction);
void sfCubesUpdateVertices(Cubes *cubes);
