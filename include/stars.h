#define MAX_STARS 10000
#include "math3d.h"
#include <glad/glad.h>

void sfInitStarBuffers(unsigned *vao, unsigned *vbo, unsigned *vertexCount,
                       unsigned *instanceCount);

void _initStarInstanceData(m44 *data, int numInstances);
