#ifndef PORT_H
#define PORT_H

#include "../kernel/types.h"

uint8_t byteIn(uint16_t port);
uint16_t wordIn(uint16_t port);
void byteOut(uint16_t port, uint8_t data);

#endif
