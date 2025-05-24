#include "physics.h"

int gjk(const Cubes *cubes, unsigned idx1, unsigned idx2) {
  v3 direction =
      v3_norm(v3_sub(cubes->positions[idx2], cubes->positions[idx1]));
  unsigned simplexCount = 1;
  v3 simplex[4] = {0};
  simplex[0] = sfCubesSupport(cubes, idx1, idx2, direction);
  direction = v3_norm(v3_sub(v3_0(), simplex[0]));

  while (1) {
    v3 A = sfCubesSupport(cubes, idx1, idx2, direction);
    if (v3_dot(A, direction) < 0) {
      return 0;
    }
    simplex[simplexCount++] = A;
    v3 AB = v3_sub(simplex[1], simplex[0]);
    v3 A0 = v3_sub(v3_0(), simplex[0]);
    if (simplexCount == 2) {
      // Line Case
      direction = v3_tripple_cross(AB, A0, AB); // AB perp
    } else {
      // Triangle case
      v3 AC = v3_sub(simplex[2], simplex[0]);
      v3 ABPerp = v3_tripple_cross(AC, AB, AB);
      v3 ACPerp = v3_tripple_cross(AB, AC, AC);
      if (v3_dot(ABPerp, A0) > 0) {
        // AB region
        --simplexCount; // Remove C
        direction = ABPerp;
      } else if (v3_dot(ACPerp, A0) > 0) {
        // AC region
        simplex[1] = simplex[2]; // Remove B
        --simplexCount;
        direction = ACPerp;
      } else {
        return 1;
      }
    }
  }
}
