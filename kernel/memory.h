#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include "../io/console.h"

#define BLOCK_COUNT 65536
#define S_EMPTY BLOCK_COUNT+1
#define S_END BLOCK_COUNT+2


typedef struct {
  uint64_t base;
  uint64_t size;
  uint32_t next;
} memory_block;

memory_block memoryBlocks[BLOCK_COUNT];

uint64_t safeMemoryOffset;
void memoryInit(void* safeMemory);

void* malloc(uint64_t size);
void* mallocAligned(uint64_t size, uint64_t align);
void* mallocAt(uint64_t addr, uint64_t size);
void free(void* base);
void printMemory();

void memcpy(void* dest, void* src, uint64_t size);
void memset(void* dest, uint64_t val, uint64_t size);
#endif
