#ifndef TIME_H
#define TIME_H
#include "../cpu/interrupts.h"
#include "../cpu/port.h"

void initTimer();

uint64_t getMicroseconds();
#endif
