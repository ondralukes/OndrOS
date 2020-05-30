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

uint32_t dwordIn(uint16_t port){
  uint32_t res;
  asm volatile("inl %%dx, %%eax": "=a" (res): "d" (port));
  return res;
}

void byteOut(uint16_t port, uint8_t data){
  asm volatile("outb %%al, %%dx" : : "a"(data), "d"(port));
}

void wordOut(uint16_t port, uint16_t data){
  asm volatile("outw %%ax, %%dx" : : "a"(data), "d"(port));
}

void dwordOut(uint16_t port, uint32_t data){
  asm volatile("outl %%eax, %%dx" : : "a"(data), "d"(port));
}
