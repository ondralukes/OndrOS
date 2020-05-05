#include "time.h"

uint64_t timerTicks = 0;
const uint64_t freq = 14914;
static void timerTick(registers reg){
  timerTicks++;
}

void initTimer(){
  setInterruptHandler(32, timerTick);

  uint64_t d = 1193180 / freq;
  uint8_t low = (uint8_t)(d&0xff);
  uint8_t high = (uint8_t)((d>>8)&0xff);

  byteOut(0x43, 0x36);
  byteOut(0x40, low);
  byteOut(0x40, high);
}

uint64_t getMicroseconds(){
  return timerTicks*1000000/(freq);
}
