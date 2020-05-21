#ifndef STREAM_H
#define STREAM_H

#include "../kernel/memory.h"
#include "../kernel/types.h"
typedef struct{
  uint8_t buffer[128];
  uint64_t size;
} stream;

stream* createStream();
void write(stream* s, uint8_t * str);
#endif
