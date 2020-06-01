#ifndef GDT_H
#define GDT_H

#define KERNEL_CS 0x08
#define KERNEL_DS KERNEL_CS + 8
#define USER_CS   KERNEL_DS + 8
#define USER_DS   USER_CS + 8
#define TSS_SEG   USER_DS + 8

#include "../kernel/types.h"
#include "../kernel/kernel.h"
#include "../kernel/memory.h"

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

typedef struct {
  uint32_t reserved0;
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t reserved1;
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint64_t reserved2;
  uint16_t reserved3;
  uint16_t iopboff;
} __attribute__((packed)) tss_t;

typedef struct {
  uint16_t limit0;
  uint16_t base0;
  uint8_t base1;
  uint8_t access;
  uint8_t limit1flags;
  uint8_t base2;
  uint32_t base3;
  uint32_t reserved;
} __attribute__((packed)) tss_desc_t;

struct full_gdt {
  gdt_entry gdt[5];
  tss_desc_t tss_desc;
} __attribute__((packed));

extern tss_t tss;
extern struct full_gdt gdt;
extern gdt_descriptor gdt_desc;

void loadGdt(uint64_t next);
void loadTss();
void allowKernelPageAccess();
#endif
