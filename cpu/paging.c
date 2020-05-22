#include "paging.h"

void allowPageAccess(uint64_t addr){
  setForeground(255,255,0);
  print("\n\nPage Fault\n\n");
  print("Page fault while accessing ");
  printHex(addr);
  printChar('\n');

  uint64_t cr4;
  uint32_t eferl;

  asm volatile(
    "mov %%cr4, %%rax\n\t"
    "movq %%rax, %0"
    :"=m"(cr4));

  asm volatile(
    "rdmsr"
    :"=a"(eferl):"c"(0xC0000080)
  );

  if((cr4 & (1 << 5)) != 0){
    if((eferl & (1 << 8)) != 0){
      print("4-level paging\n");
    } else {
      setForeground(255,0,0);
      print("PAE paging not supported\n");
      asm("hlt");
    }
  } else {
    setForeground(255,0,0);
    print("32-bit paging not supported\n");
    asm("hlt");
  }

  uint64_t pml4index = addr >> 39;
  uint64_t pdpIndex  = (addr >> 30)&0b111111111;
  print("PML4 #");
  printNum(pml4index);
  print(" PDP #");
  printNum(pdpIndex);
  printChar('\n');

  uint64_t* pml4table;
  asm volatile("mov %%cr3, %0" : "=r"(pml4table));
  pml4table = (uint64_t*)((uint64_t)pml4table & ~0xfff);

  uint64_t pml4 = pml4table[pml4index];
  pml4 |= 0b110;
  pml4table[pml4index] = pml4;

  uint64_t* pdpTable = (uint64_t*)((uint64_t)pml4 & 0x7FFFFFFFFFFFF000);

  uint64_t pdp = pdpTable[pdpIndex];
  pdp |= 0b110;
  pdpTable[pdpIndex] = pdp;

  if((pdp & (1<<7)) != 0){
    print("PDP references a 1GB page.\n\n");
    setForeground(255,255,255);
    return;
  }

  print("PDP references a page directory.\n");
  uint64_t dirIndex = (addr >> 21)&0b111111111;
  uint64_t* dirTable = (uint64_t*)((uint64_t)pdp & 0x7FFFFFFFFFFFF000);

  print("Dir #");
  printNum(dirIndex);
  printChar('\n');

  uint64_t dir = dirTable[dirIndex];
  dir |= 0b110;
  dirTable[dirIndex] = dir;

  if((dir & (1<<7)) != 0){
    print("Directory references a 2MB page.\n\n");
    setForeground(255,255,255);
    return;
  }

  print("Directory references a page table.\n\n");

  uint64_t * pages = (uint64_t*)((uint64_t)dir & 0x7FFFFFFFFFFFF000);

  for(uint64_t l = 0;l<512;l++){
    pages[l] = pages[l]|0b110;
  }

  setForeground(255,255,255);
}
