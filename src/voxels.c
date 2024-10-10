#include "voxels.h"

void _initInstanceData(m44 *transforms, v4 *colors, int numInstances) {
  int width = 1, height = numInstances;
  for (int i = 1; i * i <= numInstances; i++) {
    if (numInstances % i == 0) {
      width = i;
      height = numInstances / i;
    }
  }

  v3 positions[MAX_VOXELS];
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      unsigned index = width * i + j;
      positions[index] = (v3){(float)j, 0.0f, (float)i};
      v4 color = {randf_clamped(0.0f, 1.0f), randf_clamped(0.0f, 1.0f),
                  randf_clamped(0.0f, 1.0f), 1.0f};
      colors[index] = color;
    }
  }
  for (int i = 0; i < numInstances; ++i) {
    m44 model = m44_identity(1.0f);
    v3 position = positions[i];

    float angle = rand_deg();

    float s = 1; // randf_clamped(0.01, 0.1);
    //
    model = translate(&model, position.x, position.y, position.z);
    // model = rotate(&model, angle, 0.0f, 0.0f, 1.0f);
    model = scale(&model, s, s, s);
    transforms[i] = model;
  }
}

void sfInitVoxelBuffers(unsigned *vao, unsigned *vbo, unsigned *instanceCount) {
  float bufferData[5 * VERTEX_COUNT];
  printf("Buffer data: \n");
  for (int i = 0; i < VERTEX_COUNT; i++) {
    bufferData[i * 5 + 0] = vertices[i].x;
    bufferData[i * 5 + 1] = vertices[i].y;
    bufferData[i * 5 + 2] = vertices[i].z;

    bufferData[i * 5 + 3] = texCoords[i].x;
    bufferData[i * 5 + 4] = texCoords[i].y;
  }

  glGenVertexArrays(1, vao);
  glBindVertexArray(*vao);

  glGenBuffers(1, vbo);
  glBindBuffer(GL_ARRAY_BUFFER, *vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(bufferData), bufferData, GL_STATIC_DRAW);
  glEnableVertexAttribArray(VERTEX_POSITION_LOCATION);
  glVertexAttribPointer(VERTEX_POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(TEX_COORD_LOCATION);
  glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE,
                        5 * sizeof(float), (void *)(3 * sizeof(float)));

  m44 instanceTransforms[MAX_VOXELS];
  v4 instanceColors[MAX_VOXELS];
  *instanceCount = MAX_VOXELS;
  _initInstanceData(instanceTransforms, instanceColors, *instanceCount);
  unsigned transformsVbo;
  glGenBuffers(1, &transformsVbo);
  glBindBuffer(GL_ARRAY_BUFFER, transformsVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(instanceTransforms),
               &instanceTransforms[0], GL_STATIC_DRAW);
  for (unsigned int i = 0; i < 4; ++i) {
    glVertexAttribPointer(INSTANCE_TRANSFORM_LOCATION + i, 4, GL_FLOAT,
                          GL_FALSE, sizeof(m44), (void *)(sizeof(v4) * i));
    glEnableVertexAttribArray(INSTANCE_TRANSFORM_LOCATION + i);
    glVertexAttribDivisor(INSTANCE_TRANSFORM_LOCATION + i, 1);
  }

  unsigned colorsVbo;
  glGenBuffers(1, &colorsVbo);
  glBindBuffer(GL_ARRAY_BUFFER, colorsVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(instanceColors), &instanceColors[0],
               GL_STATIC_DRAW);
  glVertexAttribPointer(INSTANCE_COLOR_LOCATION, 4, GL_FLOAT, GL_FALSE,
                        sizeof(v4), (void *)0);
  glEnableVertexAttribArray(INSTANCE_COLOR_LOCATION);
  glVertexAttribDivisor(INSTANCE_COLOR_LOCATION, 1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void sfRenderVoxels(const unsigned vao, const unsigned instanceCount) {
  glBindVertexArray(vao);
  glDrawArraysInstanced(GL_TRIANGLES, 0, VERTEX_COUNT, instanceCount);
}
