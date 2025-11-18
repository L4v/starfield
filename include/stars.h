#define MAX_STARS 10000
#include "arena.h"
#include "common.h"
#include "math3d.h"
#include <glad/glad.h>

typedef struct {
  unsigned count;
  float *masses;
  float *sizes;

  v3 *positions;
  v3 *velocities;
  v3 *accelerations;
} Stars;

void sfStarsDestroy(Stars *stars);
Stars *sfStarsArenaAlloc(Arena *arena, unsigned count);

void sfInitStarBuffers(unsigned *vao, unsigned *vbo, unsigned *vertexCount,
                       unsigned *instanceCount);

void _initStarInstanceData(m44 *data, int numInstances);
