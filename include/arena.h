#ifndef ARENA_H
#define ARENA_H
#include <stdio.h>
#include <stdlib.h>

#define KILOBYTE (1024)
#define MEGABYTE (1024 * KILOBYTE)

typedef struct {
  void *baseMemory;
  void *allocPosition;
  size_t size;
  size_t capacity;
} Arena;

Arena sfArenaCreate(size_t blockSize, unsigned blockCount);
void *sfArenaAlloc(Arena *arena, size_t size);
void sfArenaFree(Arena *arena);

#endif
