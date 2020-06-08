#include "pci.h"

void pciCheck(){
  for(uint32_t bus = 0; bus < 256; bus++){
    for(uint32_t slot = 0; slot < 32; slot++){
      uint8_t multiFunc = pciReadWordConfig(bus, slot, 0, 14) & (1 << 7);
      uint32_t funcCount = multiFunc?8:1;
      for(uint32_t func = 0;func < funcCount;func++){
        uint16_t vendor = pciReadWordConfig(bus, slot, func, 0);
        if(vendor == 0xffff) continue;
        uint16_t device = pciReadWordConfig(bus, slot, func, 2);

        uint8_t classCode = pciReadWordConfig(bus, slot, func, 10) >> 8;
        uint8_t subclass = pciReadWordConfig(bus, slot, func, 10) & 0xff;

        if(classCode == 0x0c && subclass == 0x03){
          checkXHCI(bus, slot, func);
        }
      }
    }
  }
}

uint16_t pciReadWordConfig(uint32_t bus, uint32_t slot, uint32_t func, uint32_t off){

  uint32_t in = pciReadDWordConfig(bus, slot, func, off);
  in = in >> ((off & 2) * 8);
  in &= 0xffff;
  return (uint16_t)in;
}

uint32_t pciReadDWordConfig(uint32_t bus, uint32_t slot, uint32_t func, uint32_t off){
  uint32_t confAddr =
    (1 << 31) |
    (bus << 16) |
    (slot << 11) |
    (func << 8) |
    (off & 0xfc);

  dwordOut(0xcf8, confAddr);

  uint32_t in = dwordIn(0xcfc);
  return in;
}

void pciWriteDWordConfig(uint32_t bus, uint32_t slot, uint32_t func, uint32_t off, uint32_t val){
  uint32_t confAddr =
    (1 << 31) |
    (bus << 16) |
    (slot << 11) |
    (func << 8) |
    (off & 0xfc);

  dwordOut(0xcf8, confAddr);

  dwordOut(0xcfc, val);
}
