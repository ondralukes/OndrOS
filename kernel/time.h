#ifndef TIME_H
#define TIME_H
#include "interrupts.h"
#include "port.h"

void initTimer();

uint64_t getMicroseconds();
#endif
