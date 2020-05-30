#ifndef PORT_H
#define PORT_H

#include "../kernel/types.h"

uint8_t byteIn(uint16_t port);
uint16_t wordIn(uint16_t port);
uint32_t dwordIn(uint16_t port);
void byteOut(uint16_t port, uint8_t data);
void wordOut(uint16_t port, uint16_t data);
void dwordOut(uint16_t port, uint32_t data);
#endif
