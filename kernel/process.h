#ifndef PROCESS_H
#define PROCESS_H

#include "../cpu/interrupts.h"
#include "../io/stream.h"
#include "memory.h"

#include <stddef.h>

struct process{
  char* name;
  uint64_t rax, rcx, rdx, rbx, rsi, rdi, rsp, rbp;
  uint64_t rip;
  void* stack;
  stream* out;
};

struct process* createProcess();
void pauseProcess(registers regs);
void resumeProcess(struct process* p);
#endif
