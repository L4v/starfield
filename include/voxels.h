#include "arena.h"
#include "math3d.h"
#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

#define MAX_VOXELS 4096
#define VERTEX_COUNT 36

const static unsigned VERTEX_POSITION_LOCATION = 0;
const static unsigned TEX_COORD_LOCATION = 1;
const static unsigned INSTANCE_TRANSFORM_LOCATION = 2;
const static unsigned INSTANCE_COLOR_LOCATION = 6;

typedef struct {
  unsigned vao;
  unsigned vbo;
  unsigned transformsVbo;
  unsigned count;
  unsigned texture;
  m44 *transforms;
} Voxels;

/*
 *           E
 *           |
 *
 *         5------7
 *        /| C  / |
 *       2-+---3  |  <-D
 * B->   | 4---|--6
 *       |/  F | /
 *       0-----1
 *        /
 *       A
 */

static v3 vertices[VERTEX_COUNT] = {
    // A-side
    {-0.5f, -0.5f, 0.5f}, // 0
    {0.5f, -0.5f, 0.5f},  // 1
    {-0.5f, 0.5f, 0.5f},  // 2
    {-0.5f, 0.5f, 0.5f},  // 2
    {0.5f, -0.5f, 0.5f},  // 1
    {0.5f, 0.5f, 0.5f},   // 3
    // B-side
    {-0.5f, -0.5, -0.5f}, // 4
    {-0.5f, -0.5f, 0.5f}, // 0
    {-0.5f, 0.5, -0.5f},  // 5
    {-0.5f, 0.5, -0.5f},  // 5
    {-0.5f, -0.5f, 0.5f}, // 0
    {-0.5f, 0.5f, 0.5f},  // 2
                          //
    // C-side
    {0.5f, -0.5f, -0.5f}, // 6
    {-0.5f, -0.5, -0.5f}, // 4
    {0.5f, 0.5f, -0.5f},  // 7
    {0.5f, 0.5f, -0.5f},  // 7
    {-0.5f, -0.5, -0.5f}, // 4
    {-0.5f, 0.5, -0.5f},  // 5
    // D-side
    {0.5f, 0.5f, 0.5f},   // 3
    {0.5f, -0.5f, 0.5f},  // 1
    {0.5f, -0.5f, -0.5f}, // 6
    {0.5f, 0.5f, 0.5f},   // 3
    {0.5f, -0.5f, -0.5f}, // 6
    {0.5f, 0.5f, -0.5f},  // 7
    // E-side
    {-0.5f, 0.5f, 0.5f}, // 2
    {0.5f, 0.5f, 0.5f},  // 3
    {-0.5f, 0.5, -0.5f}, // 5
    {-0.5f, 0.5, -0.5f}, // 5
    {0.5f, 0.5f, 0.5f},  // 3
    {0.5f, 0.5f, -0.5f}, // 7
    // F-side
    {0.5f, -0.5f, 0.5f},  // 1
    {-0.5f, -0.5f, 0.5f}, // 0
    {0.5f, -0.5f, -0.5f}, // 6
    {0.5f, -0.5f, -0.5f}, // 6
    {-0.5f, -0.5f, 0.5f}, // 0
    {-0.5f, -0.5, -0.5f}, // 4
};

// static unsigned indices[] = {
//     0, 1, 2, 2, 1, 3, // A-side
//     4, 0, 5, 5, 0, 2, // B-side
//     6, 4, 7, 7, 4, 5, // C-side
//     3, 1, 6, 3, 6, 7, // D-side
//     5, 2, 3, 5, 3, 7, // E-side
//     4, 6, 0, 0, 6, 1  // F-side
// };

static v2 texCoords[VERTEX_COUNT] = {
    // A-side
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f}, // 0 1 2
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f}, // 2 1 3
    // B-side
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f}, // 4 0 5
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f}, // 5 0 2
    // C-side
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f}, // 6 4 7
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f}, // 7 4 5
    // D-side
    {0.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 0.0f}, // 3 1 6
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f}, // 3 6 7
    // E-side
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f}, // 2 3 5
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f}, // 5 3 7
    // F-side
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f}, // 1 0 6
    {0.0f, 1.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f}, // 6 0 4

};

void __bufferInstanceData(Voxels *voxels);
void __initBuffers(Voxels *voxels);

Voxels sfCreateVoxels(unsigned count);
void sfDestroyVoxels(Voxels *voxels);
void sfVoxelInitFloorInstances(const Voxels *voxels);
void sfRenderVoxels(const Voxels *voxels);
void sfUpdateVoxelTransforms(Voxels *voxels, const v3 *positions);

Voxels *sfVoxelsArenaAlloc(Arena *arena, unsigned count);
