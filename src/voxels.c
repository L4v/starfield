#include "voxels.h"

void sfVoxelInitFloorInstances(const Voxels *voxels) {
  int width = 1, height = voxels->count;
  for (int i = 1; i * i <= voxels->count; i++) {
    if (voxels->count % i == 0) {
      width = i;
      height = voxels->count / i;
    }
  }

  float scaleFactor = 2.0f;

  v3 *positions = calloc(voxels->count, sizeof(v3));
  if (!positions) {
    fprintf(stderr, "ERROR: Failed to calloc positions\n");
  }

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      unsigned index = width * i + j;
      positions[index] =
          (v3){(float)j * scaleFactor, 0.0f, (float)i * scaleFactor};
    }
  }
  for (int i = 0; i < voxels->count; ++i) {
    m44 model = m44_identity(1.0f);
    v3 position = positions[i];

    float angle = rand_deg();

    model = translate(&model, position.x, position.y, position.z);
    model = scale(&model, scaleFactor, scaleFactor, scaleFactor);
    voxels->transforms[i] = model;
  }

  free(positions);
}

void __bufferInstanceData(Voxels *voxels) {
  glBindVertexArray(voxels->vao);
  glGenBuffers(1, &voxels->transformsVbo);
  glBindBuffer(GL_ARRAY_BUFFER, voxels->transformsVbo);
  glBufferData(GL_ARRAY_BUFFER, voxels->count * sizeof(m44), voxels->transforms,
               GL_STATIC_DRAW);
  for (unsigned int i = 0; i < 4; ++i) {
    glVertexAttribPointer(INSTANCE_TRANSFORM_LOCATION + i, 4, GL_FLOAT,
                          GL_FALSE, sizeof(m44), (void *)(sizeof(v4) * i));
    glEnableVertexAttribArray(INSTANCE_TRANSFORM_LOCATION + i);
    glVertexAttribDivisor(INSTANCE_TRANSFORM_LOCATION + i, 1);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void __initBuffers(Voxels *voxels) {
  float bufferData[5 * VERTEX_COUNT];
  for (int i = 0; i < VERTEX_COUNT; i++) {
    bufferData[i * 5 + 0] = vertices[i].x;
    bufferData[i * 5 + 1] = vertices[i].y;
    bufferData[i * 5 + 2] = vertices[i].z;

    bufferData[i * 5 + 3] = texCoords[i].x;
    bufferData[i * 5 + 4] = texCoords[i].y;
  }

  glGenVertexArrays(1, &voxels->vao);
  glBindVertexArray(voxels->vao);

  glGenBuffers(1, &voxels->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, voxels->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(bufferData), bufferData, GL_STATIC_DRAW);
  glEnableVertexAttribArray(VERTEX_POSITION_LOCATION);
  glVertexAttribPointer(VERTEX_POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(TEX_COORD_LOCATION);
  glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void *)(3 * sizeof(float)));

  __bufferInstanceData(voxels);
}

void sfDestroyVoxels(Voxels *voxels) { free(voxels->transforms); }

void sfRenderVoxels(const Voxels *voxels) {
  if (voxels->texture) {
    glBindTexture(GL_TEXTURE_2D, voxels->texture);
  }

  glBindVertexArray(voxels->vao);

  glBindBuffer(GL_ARRAY_BUFFER, voxels->transformsVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m44) * voxels->count,
                  voxels->transforms);

  glDrawArraysInstanced(GL_TRIANGLES, 0, VERTEX_COUNT, voxels->count);
}

void sfUpdateVoxelTransforms(Voxels *voxels, const v3 *positions) {
  for (int i = 0; i < voxels->count; ++i) {
    voxels->transforms[i] = m44_identity(1.0f);
    const v3 *position = &positions[i];
    voxels->transforms[i] = translate(&voxels->transforms[i], position->x,
                                      position->y, position->z);
  }
}

Voxels *sfVoxelsArenaAlloc(Arena *arena, unsigned count) {
  Voxels *voxels = (Voxels *)sfArenaAlloc(arena, sizeof(Voxels));
  voxels->count = count;
  voxels->transforms = (m44 *)sfArenaAlloc(arena, sizeof(m44) * count);
  __initBuffers(voxels);
  return voxels;
}
