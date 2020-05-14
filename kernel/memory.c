#include "memory.h"

void memoryInit(void* safeMemory){
  for(uint64_t i = 0;i<BLOCK_COUNT;i++){
    memoryBlocks[i].base = 0;
    memoryBlocks[i].size = 0;
    memoryBlocks[i].next = S_EMPTY;
  }
  memoryBlocks[0].base = (uint64_t)safeMemory;
  memoryBlocks[0].size = 0;
  memoryBlocks[0].next = S_END;
}

uint16_t getFreeIndex(){
  for(uint64_t i = 0;i<BLOCK_COUNT;i++){
    if(memoryBlocks[i].next == S_EMPTY){
      return i;
    }
  }
}

void* malloc(uint64_t size){
  memory_block* block = &memoryBlocks[0];
  uint64_t base = 0;
  uint64_t max = 0;
  while(block->next != S_EMPTY){

    base = block->base+block->size;
    if(block->next == S_END){
      max = ~0;
    } else {
      max = memoryBlocks[block->next].base;
    }
    uint64_t possibleSize = max - base;
    if(possibleSize > size){
      break;
    }
    if(block->next == S_END) break;
    block = &memoryBlocks[block->next];
  }

  uint64_t freeIndex = getFreeIndex();

  memory_block* new = &memoryBlocks[freeIndex];
  new->base = base;
  new->size = size;
  if(block->next != S_END && block->next != S_EMPTY){
    new->next = block->next;
  } else {
    new->next = S_END;
  }
  if(block != new) block->next = freeIndex;
  return (void*)new->base;
}

void free(void* base){
  memory_block* prev = &memoryBlocks[0];
  memory_block* block = prev;
  while(block->next != S_END){
    block = &memoryBlocks[block->next];
    if(block->base == (uint64_t)base){
      prev->next = block->next;
      block->next = S_EMPTY;
      return;
    }
    prev = block;
  }
  print("Not freed\n");
}

void printMemory(){
  memory_block* block = &memoryBlocks[0];
  uint64_t allocated = 0;
  uint64_t blocks = 0;
  while(block->next != S_END){
    block = &memoryBlocks[block->next];
    print("Block at ");
    printHex((uint64_t)block->base);
    print(" size ");
    printNum(block->size);
    print("\n");
    allocated += block->size;
    blocks++;
  }
  print("Total allocated ");
  printNum(allocated);
  print(" in ");
  printNum(blocks);
  print(" blocks.\n");
}
