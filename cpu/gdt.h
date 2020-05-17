#ifndef GDT_H
#define GDT_H

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10

#include "../kernel/types.h"

typedef struct {
  uint16_t limit;
  uint32_t offset;
} __attribute__((packed)) gdt_descriptor;

typedef struct {
  uint16_t limit0;
  uint16_t base0;
  uint8_t base1;
  uint8_t access;
  uint8_t limit1flags;
  uint8_t base2;
} __attribute__((packed)) gdt_entry;

gdt_entry gdt[3];
gdt_descriptor gdt_desc;

void loadGdt(uint64_t next);
#endif
