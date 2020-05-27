#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../io/console.h"
#include "memory.h"
#include "process.h"
#include "time.h"
#include <stddef.h>

#define SCHEDULER_INTERVAL 10000 // 50 ms
#define FLUSH_INTERVAL 10000

struct processNode{
  struct process* p;
  struct processNode* next;
};

void initScheduler();
void schedulerTick(registers regs);
void startScheduler();

void registerProcess(struct process* p);
void removeProcess(struct process* p);
void flushProcessOut();
#endif
