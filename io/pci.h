#ifndef PCI_H
#define PCI_H

#include "../cpu/port.h"
#include "console.h"
#include "xhci.h"

void pciCheck();

uint32_t pciReadDWordConfig(uint32_t bus, uint32_t slot, uint32_t func, uint32_t off);
void pciWriteDWordConfig(uint32_t bus, uint32_t slot, uint32_t func, uint32_t off, uint32_t val);
uint16_t pciReadWordConfig(uint32_t bus, uint32_t slot, uint32_t func, uint32_t off);
#endif
