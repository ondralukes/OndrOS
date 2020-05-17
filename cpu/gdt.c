#include "gdt.h"

void loadGdt(uint64_t next){
  asm("cli");
  gdt[0].limit0 = 0;
  gdt[0].base0 = 0;
  gdt[0].base1 = 0;
  gdt[0].access = 0;
  gdt[0].limit1flags = 0;
  gdt[0].base2 = 0;

  gdt[1].limit0 = 0xffff;
  gdt[1].base0 = 0;
  gdt[1].base1 = 0;
  gdt[1].access = 0b10011010;
  gdt[1].limit1flags = 0b10101111;
  gdt[1].base2 = 0;

  gdt[2].limit0 = 0xffff;
  gdt[2].base0 = 0;
  gdt[2].base1 = 0;
  gdt[2].access = 0b10010010;
  gdt[2].limit1flags = 0b11001111;
  gdt[2].base2 = 0;

  gdt_desc.offset = (uint64_t)gdt;
  gdt_desc.limit = sizeof(gdt_entry) * 3 - 1;

  uint8_t temp[8];
  uint64_t gdtaddr = &gdt_desc;
  asm volatile (
    "lgdtq %0\n\t"
    "mov %%ax, %%ds\n\t"
    "mov %%ax, %%es\n\t"
    "mov %%ax, %%fs\n\t"
    "mov %%ax, %%gs\n\t"
    "mov %%ax, %%ss\n\t"
    "mov %%ebx, (%%rcx)\n\t"
    "mov %%dx, 0x4(%%rcx)\n\t"
    "ljmp *(%%rcx)\n\t"
    : :  "m"(gdt_desc), "a"(KERNEL_DS), "b"(next), "c"(temp), "d"(KERNEL_CS));
}
