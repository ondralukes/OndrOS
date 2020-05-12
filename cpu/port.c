#include "port.h"

uint8_t byteIn(uint16_t port){
  uint8_t res;
  asm volatile("in %%dx, %%al": "=a" (res): "d" (port));
  return res;
}

void byteOut(uint16_t port, uint8_t data){
  asm volatile("out %%al, %%dx" : : "a"(data), "d"(port));
}
