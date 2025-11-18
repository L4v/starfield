#ifndef OCTREE_H
#define OCTREE_H
#include "arena.h"
#include "common.h"
#include "float.h"
#include "math3d.h"
#include "string.h"

typedef struct {
  float size;
  v3 center;
} Octant;

typedef struct {
  unsigned *children;
  unsigned *parents;
  v3 *positions;
  float *masses;
  Octant *octants;
  unsigned *nexts;
  unsigned count;
  unsigned parentsCount;
  unsigned maxCount;

  float thetaSquared;
  float epsilonSquared;
} Octree;

static unsigned findOctant(const v3 position, const v3 center);
static void octantSubdivide(const Octant *original, Octant *octants);
void octreeInsertParent(Octree *octree, unsigned node);
unsigned octreeSubdivide(Octree *octree, unsigned node);

Octree *sfOctreeArenaAlloc(Arena *arena, float theta, float epsilon,
                           unsigned maxCount);
void sfOctreeInsert(Octree *octree, const v3 position, float mass);
Octant sfOctantContaining(const v3 *positions, unsigned count);
void sfOctreePropagate(Octree *octree);
v3 sfOctreeAcceleration(const Octree *octree, const v3 position);
void sfOctreeClear(Octree *octree, const Octant *octant);

#endif
