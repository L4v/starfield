#include "common.h"

v3 *sfV3ArenaAlloc(Arena *arena, unsigned count) {
  v3 *v = (v3 *)sfArenaAlloc(arena, sizeof(v3) * count);
  return v;
}
