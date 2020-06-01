#ifndef KERNEL_H
#define KERNEL_H
#include "types.h"
extern struct kernel_args args;

extern uint64_t kernelStackBottom;
extern uint64_t kernelStackTop;

void stage2();
#endif
