#include "cubes.h"

Cubes *sfCubesArenaAlloc(Arena *arena, unsigned count) {
  Cubes *cubes = (Cubes *)sfArenaAlloc(arena, sizeof(Cubes));

  cubes->count = count;
  cubes->speeds = (float *)sfArenaAlloc(arena, sizeof(float) * cubes->count);
  cubes->positions = sfV3ArenaAlloc(arena, cubes->count);
  cubes->velocities = sfV3ArenaAlloc(arena, cubes->count);
  cubes->accelerations = sfV3ArenaAlloc(arena, cubes->count);
  cubes->vertices = sfV3ArenaAlloc(arena, cubes->count * 8);

  return cubes;
}

v3 sfCubeSupport(const Cubes *cubes, unsigned index, v3 direction) {
  direction = v3_norm(direction);
  v3 result = cubes->vertices[CUBE_VERTEX_COUNT * index];
  double maxDot = v3_dot(v3_norm(result), direction);
  for (int i = 1; i < 8; ++i) {
    v3 vertex = cubes->vertices[CUBE_VERTEX_COUNT * index + i];
    double dot = v3_dot(v3_norm(vertex), direction);
    if (dot > maxDot) {
      result = vertex;
      maxDot = dot;
    }
  }
  return result;
}

v3 sfCubesSupport(const Cubes *cubes, unsigned idx1, unsigned idx2,
                  v3 direction) {
  v3 support1 = sfCubeSupport(cubes, idx1, direction);
  v3 support2 = sfCubeSupport(cubes, idx2, v3_neg(&direction));
  return v3_sub(support1, support2);
}

void sfCubesUpdateVertices(Cubes *cubes) {
  for (int cubeIdx = 0; cubeIdx < cubes->count; ++cubeIdx) {
    for (int i = 0; i < CUBE_VERTEX_COUNT; ++i) {
      unsigned vertexIdx = cubeIdx * CUBE_VERTEX_COUNT + i;
      cubes->vertices[vertexIdx] =
          v3_add(CUBE_LOCAL_VERTICES[i], cubes->positions[cubeIdx]);
    }
  }
}
