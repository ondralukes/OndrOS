#include "memory.h"

memory_block memoryBlocks[BLOCK_COUNT];

uint64_t safeMemoryOffset;

void memoryInit(void* safeMemory){
  for(uint64_t i = 0;i<BLOCK_COUNT;i++){
    memoryBlocks[i].base = 0;
    memoryBlocks[i].size = 0;
    memoryBlocks[i].next = S_EMPTY;
  }
  memoryBlocks[0].base = (uint64_t)safeMemory;
  memoryBlocks[0].size = 0;
  memoryBlocks[0].next = S_END;
  safeMemoryOffset = (uint64_t) safeMemory;
}

uint16_t getFreeIndex(){
  for(uint64_t i = 0;i<BLOCK_COUNT;i++){
    if(memoryBlocks[i].next == S_EMPTY){
      return i;
    }
  }
}

void* malloc(uint64_t size){
  return mallocAligned(size, 1);
}

void* mallocAligned(uint64_t size,uint64_t align){
  memory_block* block = &memoryBlocks[0];
  uint64_t base = 0;
  uint64_t max = 0;
  while(block->next != S_EMPTY){

    base = block->base+block->size;
    if(base % align != 0){
      base += align - (base % align);
    }
    if(block->next == S_END){
      max = ~0;
    } else {
      max = memoryBlocks[block->next].base;
    }
    uint64_t possibleSize = max - base;
    if(possibleSize > size && max > base && base > safeMemoryOffset){
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

void* mallocAt(uint64_t addr, uint64_t size){
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
    if(addr >= base && addr < max){
      uint64_t possibleSize = max - addr;
      if(possibleSize > size){
        break;
      } else {
        return 0;
      }
    }

    if(block->next == S_END) break;
    block = &memoryBlocks[block->next];
  }

  uint64_t freeIndex = getFreeIndex();

  memory_block* new = &memoryBlocks[freeIndex];
  new->base = addr;
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
    // print("Block at ");
    // printHex((uint64_t)block->base);
    // print(" size ");
    // printNum(block->size);
    // print("\n");
    allocated += block->size;
    blocks++;
  }
  print("Total allocated ");
  printNum(allocated);
  print(" in ");
  printNum(blocks);
  print(" blocks.\n");
}

void memcpy(void* dest, void* src, uint64_t size){
  uint64_t qlimit = (uint64_t)src + (size/8)*8;
  uint64_t dlimit = (uint64_t)src + (size/4)*4;
  uint64_t wlimit = (uint64_t)src + (size/2)*2;
  uint64_t blimit = (uint64_t)src + size;

  asm volatile(
    "movq %4, %%rsi\n\t"
    "movq %5, %%rdi\n\t"
    "cld\n\t"
    "1:\n\t"
    "cmpq %%rsi, %0\n\t"
    "je 2f\n\t"
    "movsq\n\t"
    "jmp 1b\n\t"
    "2:\n\t"

    "cmpq %%rsi, %1\n\t"
    "je 3f\n\t"
    "movsl\n\t"
    "jmp 2b\n\t"
    "3:\n\t"

    "cmpq %%rsi, %2\n\t"
    "je 4f\n\t"
    "movsw\n\t"
    "jmp 3b\n\t"
    "4:\n\t"

    "cmpq %%rsi, %3\n\t"
    "je 5f\n\t"
    "movsb\n\t"
    "jmp 4b\n\t"
    "5:\n\t"
    :: "m"(qlimit), "m"(dlimit), "m"(wlimit), "m"(blimit), "m"(src), "m"(dest));
}

void memset(void* dest, uint64_t val, uint64_t size){
  uint64_t qlimit = (uint64_t)dest + (size/8)*8;
  uint64_t dlimit = (uint64_t)dest + (size/4)*4;
  uint64_t wlimit = (uint64_t)dest + (size/2)*2;
  uint64_t blimit = (uint64_t)dest + size;

  asm volatile(
    "movq %4, %%rdi\n\t"
    "cld\n\t"
    "1:\n\t"
    "cmpq %%rdi, %0\n\t"
    "je 2f\n\t"
    "stosq\n\t"
    "jmp 1b\n\t"
    "2:\n\t"

    "cmpq %%rdi, %1\n\t"
    "je 3f\n\t"
    "stosl\n\t"
    "jmp 2b\n\t"
    "3:\n\t"

    "cmpq %%rdi, %2\n\t"
    "je 4f\n\t"
    "stosw\n\t"
    "jmp 3b\n\t"
    "4:\n\t"

    "cmpq %%rdi, %3\n\t"
    "je 5f\n\t"
    "stosb\n\t"
    "jmp 4b\n\t"
    "5:\n\t"
    :: "m"(qlimit), "m"(dlimit), "m"(wlimit), "m"(blimit), "m"(dest), "a"(val));
}
