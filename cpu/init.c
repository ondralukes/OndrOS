#include "init.h"

void loadGdt(uint64_t next){
  asm("cli");
  gdt.gdt[0].limit0 = 0;
  gdt.gdt[0].base0 = 0;
  gdt.gdt[0].base1 = 0;
  gdt.gdt[0].access = 0;
  gdt.gdt[0].limit1flags = 0;
  gdt.gdt[0].base2 = 0;

  //kernel code
  gdt.gdt[1].limit0 = 0xffff;
  gdt.gdt[1].base0 = 0;
  gdt.gdt[1].base1 = 0;
  gdt.gdt[1].access = 0b10011010;
  gdt.gdt[1].limit1flags = 0b10101111;
  gdt.gdt[1].base2 = 0;

  //kernel data
  gdt.gdt[2].limit0 = 0xffff;
  gdt.gdt[2].base0 = 0;
  gdt.gdt[2].base1 = 0;
  gdt.gdt[2].access = 0b10010010;
  gdt.gdt[2].limit1flags = 0b11001111;
  gdt.gdt[2].base2 = 0;

  //user code
  gdt.gdt[3].limit0 = 0xffff;
  gdt.gdt[3].base0 = 0;
  gdt.gdt[3].base1 = 0;
  gdt.gdt[3].access = 0b11111010;
  gdt.gdt[3].limit1flags = 0b10101111;
  gdt.gdt[3].base2 = 0;

  //user data
  gdt.gdt[4].limit0 = 0xffff;
  gdt.gdt[4].base0 = 0;
  gdt.gdt[4].base1 = 0;
  gdt.gdt[4].access = 0b11110010;
  gdt.gdt[4].limit1flags = 0b11001111;
  gdt.gdt[4].base2 = 0;

  //tss
  uint64_t tssOffset = (uint64_t)&tss;
  uint64_t tssLimit = sizeof(tss_t) - 1;

  gdt.tss_desc.limit0 = tssLimit & 0xffff;
  gdt.tss_desc.base0 = tssOffset & 0xffff;
  gdt.tss_desc.base1 = (tssOffset >> 16) & 0xff;
  gdt.tss_desc.access = 0b10001001;
  gdt.tss_desc.limit1flags = (0b0001 << 4) | ((tssLimit >> 16) & 0b1111);
  gdt.tss_desc.base2 = (tssOffset >> 24) & 0xff;
  gdt.tss_desc.base3 = (tssOffset >> 32) & 0xffffffff;
  gdt.tss_desc.reserved = 0;

  gdt_desc.offset = (uint64_t)&gdt;
  gdt_desc.limit = sizeof(struct full_gdt) - 1;

  uint8_t temp[8];
  uint64_t gdtaddr =(uint64_t) &gdt_desc;
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

void loadTss(){
  tss.rsp0 = kernelStackTop;
  tss.rsp1 = kernelStackTop;
  tss.rsp2 = kernelStackTop;

  tss.ist1 = kernelStackTop;
  tss.ist2 = kernelStackTop;
  tss.ist3 = kernelStackTop;
  tss.ist4 = kernelStackTop;
  tss.ist5 = kernelStackTop;
  tss.ist6 = kernelStackTop;
  tss.ist7 = kernelStackTop;

  tss.reserved0 = 0;
  tss.reserved1 = 0;
  tss.reserved2 = 0;
  tss.reserved3 = 0;

  tss.iopboff = sizeof(tss_t);
  asm volatile("ltr %%ax" : : "a"(TSS_SEG));
}

void allowKernelPageAccess(){
  //Allow kernel to write everywhere
  asm volatile (
    "mov %cr0, %rax\n\t"
    "andq $0xfffffffffffeffff, %rax\n\t"
    "mov %rax, %cr0"
  );
}
