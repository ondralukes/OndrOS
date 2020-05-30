#include "pci.h"

void pciCheck(){
  for(uint32_t bus = 0; bus < 256; bus++){
    for(uint32_t slot = 0; slot < 32; slot++){
      for(uint32_t func = 0;func < 8;func++){
      uint16_t vendor = pciReadWordConfig(bus, slot, func, 0);
      if(vendor == 0xffff) continue;
      uint16_t device = pciReadWordConfig(bus, slot, func, 2);

      uint8_t classCode = pciReadWordConfig(bus, slot, func, 10) >> 8;
      printNum(bus);
      print(".");
      printNum(slot);
      print(".");
      printNum(func);
      print(": ");
      switch(classCode){
        case 0x0:
          print("Unclassified");
          break;
        case 0x1:
          print("Mass storage controller");
          break;
        case 0x2:
          print("Network controller");
          break;
        case 0x3:
          print("Display controller");
          break;
        case 0x4:
          print("Multimedia controller");
          break;
        case 0x5:
          print("Memory controller");
          break;
        case 0x6:
          print("Bridge device");
          break;
        case 0x7:
          print("Simple communication controller");
          break;
        case 0x8:
          print("Base system peripheral");
          break;
        case 0x9:
          print("Input device controller");
          break;
        case 0x0a:
          print("Docking station");
          break;
        case 0x0b:
          print("Processor");
          break;
        case 0x0c:
          print("Serial bus controller");
          break;
        case 0x0d:
          print("Wireless controller");
          break;
        case 0x0e:
          print("Intelligent controller");
          break;
        case 0x0f:
          print("Satellite communication controller");
          break;
        case 0x10:
          print("Encryption controller");
          break;
        case 0x11:
          print("Signal processing controller");
          break;
        case 0x12:
          print("Processing accelerator");
          break;
        case 0x13:
          print("Non-Essential instrumentation");
          break;
        case 0x40:
          print("Co-processor");
          break;
        case 0xff:
          print("Unassigned class");
          break;
        default:
          print("Unknown class code");
          break;
      }
      print(" (");
      printHex(classCode);
      print(") vendor=");
      printHex(vendor);
      print(" device=");
      printHex(device);
      print("\n");
    }
  }
  }
}

uint16_t pciReadWordConfig(uint32_t bus, uint32_t slot, uint32_t func, uint32_t off){
  uint32_t confAddr =
    (1 << 31) |
    (bus << 16) |
    (slot << 11) |
    (func << 8) |
    (off & 0xfc);

  dwordOut(0xcf8, confAddr);

  uint32_t in = dwordIn(0xcfc);
  in = in >> ((off & 2) * 8);
  in &= 0xffff;
  return (uint16_t)in;
}
