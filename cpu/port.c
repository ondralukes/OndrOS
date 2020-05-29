#include "port.h"

uint8_t byteIn(uint16_t port){
  uint8_t res;
  asm volatile("inb %%dx, %%al": "=a" (res): "d" (port));
  return res;
}

uint16_t wordIn(uint16_t port){
  uint16_t res;
  asm volatile("inw %%dx, %%ax": "=a" (res): "d" (port));
  return res;
}

void byteOut(uint16_t port, uint8_t data){
  asm volatile("outb %%al, %%dx" : : "a"(data), "d"(port));
}
