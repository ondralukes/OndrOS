#ifndef KERNEL_H
#define KERNEL_H
#include "types.h"
struct kernel_args args;

uint64_t kernelStackBottom;
uint64_t kernelStackTop;

void stage2();
#endif
