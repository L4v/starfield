#include "octree.h"

Octree *sfOctreeArenaAlloc(Arena *arena, float theta, float epsilon,
                           unsigned maxCount) {
  Octree *octree = (Octree *)sfArenaAlloc(arena, sizeof(Octree));
  octree->count = 0;
  octree->maxCount = maxCount;
  octree->parentsCount = 0;
  octree->thetaSquared = theta * theta;
  octree->epsilonSquared = epsilon * epsilon;

  octree->children =
      (unsigned *)sfArenaAlloc(arena, sizeof(unsigned) * octree->maxCount);
  octree->parents =
      (unsigned *)sfArenaAlloc(arena, sizeof(unsigned) * octree->maxCount);
  octree->nexts =
      (unsigned *)sfArenaAlloc(arena, sizeof(unsigned) * octree->maxCount);
  octree->positions = sfV3ArenaAlloc(arena, octree->maxCount);
  octree->masses =
      (float *)sfArenaAlloc(arena, sizeof(float) * octree->maxCount);
  octree->octants =
      (Octant *)sfArenaAlloc(arena, sizeof(Octant) * octree->maxCount);

  return octree;
}

unsigned findOctant(const v3 position, const v3 center) {
  return ((position.z > center.z) << 2) | ((position.y > center.y) << 1) |
         (position.x > center.x);
}

void octantSubdivide(const Octant *original, Octant *octants) {
  float child_size = original->size * 0.5f;
  float offset = (child_size * 0.5f);
  for (int i = 0; i < 8; ++i) {
    octants[i].center = original->center;
    octants[i].center.x += (i & 1) ? offset : -offset;
    octants[i].center.y += (i & 2) ? offset : -offset;
    octants[i].center.z += (i & 4) ? offset : -offset;
    octants[i].size = child_size;
  }
}

Octant sfOctantContaining(const v3 *positions, unsigned count) {
  v3 min = (v3){FLT_MAX, FLT_MAX, FLT_MAX};
  v3 max = (v3){-FLT_MAX, -FLT_MAX, -FLT_MAX};
  for (int i = 0; i < count; ++i) {
    const v3 *position = &positions[i];
    min.x = fminf(position->x, min.x);
    min.y = fminf(position->y, min.y);
    min.z = fminf(position->z, min.z);

    max.x = fmaxf(position->x, max.x);
    max.y = fmaxf(position->y, max.y);
    max.z = fmaxf(position->z, max.z);
  }

  v3 center = v3_scale(v3_add(min, max), 0.5f);
  v3 sizes = v3_sub(max, min);
  float size = 0.0f;
  for (int i = 0; i < 3; ++i) {
    size = fmaxf(fabs(size), fabs(sizes.v[i]));
  }

  return (Octant){size, center};
}

void octreeInsertParent(Octree *octree, unsigned node) {
  octree->parents[octree->parentsCount++] = node;
}

unsigned octreeSubdivide(Octree *octree, unsigned node) {
  octreeInsertParent(octree, node);
  unsigned children = octree->count;
  octree->children[node] = children;

  Octant octants[8];
  octantSubdivide(&octree->octants[node], octants);

  // Insert 7 octants
  for (int i = 0; i < 7; ++i) {
    octree->nexts[octree->count] = children + i + 1;
    octree->octants[octree->count] = octants[i];
    octree->masses[octree->count] = 0.0f;
    octree->positions[octree->count] = v3_0();
    ++octree->count;
  }

  octree->nexts[octree->count] = octree->nexts[node];
  octree->octants[octree->count] = octants[7];
  octree->masses[octree->count] = 0.0f;
  octree->positions[octree->count] = v3_0();

  ++octree->count;

  return children;
}

void sfOctreeInsert(Octree *octree, const v3 position, float mass) {
  unsigned node = 0; // root
  while (octree->children[node] != 0) {
    v3 center = octree->octants[node].center;
    unsigned octant = findOctant(position, center);
    node = octree->children[node] + octant;
  }

  if (octree->masses[node] == 0) { // is empty
    octree->positions[node] = position;
    octree->masses[node] = mass;
    return;
  }

  if (v3_cmp(octree->positions[node], position)) {
    octree->masses[node] += mass;
    return;
  }

  while (1) {
    unsigned children = octreeSubdivide(octree, node);
    unsigned octant1 =
        findOctant(octree->positions[node], octree->octants[node].center);
    unsigned octant2 = findOctant(position, octree->octants[node].center);

    if (octant1 == octant2) {
      node = children + octant1;
    } else {
      unsigned n1 = children + octant1;
      unsigned n2 = children + octant2;
      octree->positions[n1] = octree->positions[node];
      octree->masses[n1] = octree->masses[node];
      octree->positions[n2] = position;
      octree->masses[n2] = mass;
      return;
    }
  }
}

void sfOctreePropagate(Octree *octree) {
  for (int node = octree->parentsCount - 1; node >= 0; --node) {
    int i = octree->children[node];
    v3 centerOfMass = v3_0();
    for (int j = 0; j < 8; ++j) {
      centerOfMass = v3_add(centerOfMass, v3_scale(octree->positions[i + j],
                                                   octree->masses[i + j]));
    }
    float mass = 0.0f;
    for (int j = 0; j < 8; ++j) {
      mass += octree->masses[i + j];
    }

    // TODO: Validate this is alright
    if (mass <= 1e-6f) {
      mass = 1.0f;
    }

    octree->positions[node] = v3_scale(centerOfMass, 1.0f / mass);
    octree->masses[node] = mass;
  }
}

v3 sfOctreeAcceleration(const Octree *octree, const v3 position) {
  v3 acceleration = v3_0();
  unsigned node = 0;

  while (1) {
    v3 d = v3_sub(octree->positions[node], position);
    float distance = v3_len(d);
    float distanceSquared = distance * distance;

    float octantSizeSquared =
        octree->octants[node].size * octree->octants[node].size;
    if (octree->children[node] == 0 ||
        octantSizeSquared < distanceSquared * octree->thetaSquared) { // is leaf
      float denom = (distanceSquared + octree->epsilonSquared) * distance;
      v3 inc = v3_scale(d, fminf((octree->masses[node] / denom), FLT_MAX));
      acceleration = v3_add(acceleration, inc);

      if (octree->nexts[node] == 0) {
        break;
      }

      node = octree->nexts[node];
    } else {
      node = octree->children[node];
    }
  }

  return acceleration;
}

void sfOctreeClear(Octree *octree, const Octant *octant) {
  memset(octree->children, 0, octree->maxCount * sizeof(unsigned));
  memset(octree->parents, 0, octree->maxCount * sizeof(unsigned));
  memset(octree->positions, 0, octree->maxCount * sizeof(v3));
  memset(octree->masses, 0, octree->maxCount * sizeof(float));
  memset(octree->octants, 0, octree->maxCount * sizeof(Octant));
  memset(octree->nexts, 0, octree->maxCount * sizeof(unsigned));

  octree->parentsCount = 0;

  octree->octants[0] = *octant;
  octree->children[0] = 0;
  octree->count = 1;
}
