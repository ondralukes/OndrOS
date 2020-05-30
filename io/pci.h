#ifndef PCI_H
#define PCI_H

#include "../cpu/port.h"
#include "console.h"

void pciCheck();

uint16_t pciReadWordConfig(uint32_t bus, uint32_t slot, uint32_t func, uint32_t off);
#endif
