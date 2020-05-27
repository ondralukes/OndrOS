#ifndef PROCESS_H
#define PROCESS_H

#include "../cpu/interrupts.h"
#include "../io/stream.h"
#include "memory.h"
#include "time.h"

#include <stddef.h>

enum ProcessState {Running=0, Ended=1};
struct process{
  char* name;
  uint64_t rax, rcx, rdx, rbx, rsi, rdi, rsp, rbp;
  uint64_t rip;
  void* stack;
  stream* out;
  uint64_t sleepUntil;
  enum ProcessState state;
};

struct process* createProcess();
void destroyProcess(struct process* p);
void pauseProcess(registers regs);
void resumeProcess(struct process* p);

void sleep(struct process*p, uint64_t us);
#endif
