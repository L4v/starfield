
#include "arena.h"

Arena sfArenaCreate(size_t blockSize, unsigned blockCount) {
  Arena arena = {0};
  arena.capacity = blockSize * blockCount;
  arena.baseMemory = calloc(blockSize, blockCount);
  arena.allocPosition = arena.baseMemory;
  arena.size = 0;
  return arena;
}

void *sfArenaAlloc(Arena *arena, size_t size) {
  size_t newArenaSize = arena->size + size;
  if (newArenaSize > arena->capacity) {
    fprintf(
        stderr,
        "ERROR: Failed arena allocation. Capacity: %ld, requested size: %ld\n",
        arena->capacity, size);
    return NULL;
  }

  /* arena->allocPosition += newArenaSize; */
  void *currentPosition = arena->allocPosition;
  arena->allocPosition = (char *)arena->allocPosition + size;
  arena->size = arena->allocPosition - arena->baseMemory;
  return currentPosition;
}

void sfArenaFree(Arena *arena) { free(arena->baseMemory); }
