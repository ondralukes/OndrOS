#include "time.h"

uint64_t timerTicks = 0;
uint64_t ticksSinceLastSched = 0;
uint64_t ticksSinceLastFlush = 0;
const uint64_t freq = 14914;
static void timerTick(registers regs){
  timerTicks++;
  ticksSinceLastSched++;
  ticksSinceLastFlush++;
  if(ticksSinceLastFlush >= (FLUSH_INTERVAL*freq)/1000000){
    ticksSinceLastFlush = 0;
    flushProcessOut();
  }
  if(ticksSinceLastSched >= (SCHEDULER_INTERVAL*freq)/1000000){
    ticksSinceLastSched = 0;
    schedulerTick(regs);
  }
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
