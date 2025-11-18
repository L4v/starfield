#include "stars.h"

void sfStarsDestroy(Stars *stars) {
  free(stars->sizes);
  free(stars->masses);
  free(stars->positions);
  free(stars->velocities);
  free(stars->accelerations);
}

Stars *sfStarsArenaAlloc(Arena *arena, unsigned count) {
  Stars *stars = (Stars *)sfArenaAlloc(arena, sizeof(Stars));

  stars->count = count;

  stars->masses = (float *)sfArenaAlloc(arena, sizeof(float) * stars->count);
  stars->sizes = (float *)sfArenaAlloc(arena, sizeof(float) * stars->count);
  stars->positions = (v3 *)sfV3ArenaAlloc(arena, stars->count);
  stars->velocities = (v3 *)sfV3ArenaAlloc(arena, stars->count);
  stars->accelerations = (v3 *)sfV3ArenaAlloc(arena, stars->count);

  for (int i = 0; i < stars->count; ++i) {
    stars->sizes[i] = 1.0f;
  }

  return stars;
}

void _initStarInstanceData(m44 *data, int numInstances) {
  v3 positions[MAX_STARS];
  for (int i = 0; i < numInstances; ++i) {
    m44 model = m44_identity(1.0f);
    v3 position = {randf_clamped(-50.0f, 50.0f), randf_clamped(-50.0f, 50.0f),
                   randf_clamped(-50.0f, 50.0f)};

    float angle = rand_deg();

    float s = randf_clamped(0.01, 0.1);

    model = translate(&model, position.x, position.y, position.z);
    model = rotate(&model, angle, 0.0f, 0.0f, 1.0f);
    model = scale(&model, s, s, s);
    data[i] = model;
  }
}

void sfInitStarBuffers(unsigned *vao, unsigned *vbo, unsigned *vertexCount,
                       unsigned *instanceCount) {
  v3 tri00 = {-0.866f, -0.5f, 0.0f};
  v3 tri01 = {0.866f, -0.5f, 0.0f};
  v3 tri02 = {0.0f, 1.0f, 0.0f};

  v3 tri10 = {-0.866f, 0.5f, 0.0f};
  v3 tri11 = {0.866f, 0.5f, 0.0f};
  v3 tri12 = {0.0f, -1.0f, 0.0f};

  v3 tri20 = {-0.866f, 0.0f, -0.5f};
  v3 tri21 = {0.866f, 0.0f, -0.5f};
  v3 tri22 = {0.0f, 0.0f, 1.0f};

  v3 tri30 = {-0.866f, 0.0f, 0.5f};
  v3 tri31 = {0.866f, 0.0f, 0.5f};
  v3 tri32 = {0.0f, 0.0f, -1.0f};

  v3 tri40 = {0.0f, -0.5f, -0.866f};
  v3 tri41 = {0.0f, -0.5f, 0.866f};
  v3 tri42 = {0.0f, 1.0f, 0.0f};

  v3 tri50 = {0.0f, 0.5f, -0.866f};
  v3 tri51 = {0.0f, 0.5f, 0.866f};
  v3 tri52 = {0.0f, -1.0f, 0.0f};

  v3 vertices[] = {
      tri00, tri01, tri02, tri10, tri11, tri12, tri20, tri21, tri22,
      tri30, tri31, tri32, tri40, tri41, tri42, tri50, tri51, tri52,

  };
  *vertexCount = sizeof(vertices) / sizeof(v3);

  glGenVertexArrays(1, vao);
  glBindVertexArray(*vao);

  glGenBuffers(1, vbo);
  glBindBuffer(GL_ARRAY_BUFFER, *vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  m44 instanceData[MAX_STARS];
  *instanceCount = MAX_STARS;
  _initStarInstanceData(instanceData, *instanceCount);

  unsigned instanceVbo;
  glGenBuffers(1, &instanceVbo);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(instanceData), &instanceData[0],
               GL_STATIC_DRAW);
  for (unsigned int i = 0; i < 4; i++) {
    glVertexAttribPointer(1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(m44),
                          (void *)(sizeof(v4) * i));
    glEnableVertexAttribArray(1 + i);
    glVertexAttribDivisor(1 + i, 1);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
