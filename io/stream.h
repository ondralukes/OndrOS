#ifndef STREAM_H
#define STREAM_H

#include "../kernel/memory.h"
#include "../kernel/types.h"
#include "console.h"

typedef struct{
  uint8_t buffer[128];
  uint64_t size;
} stream;

stream* createStream();
void write(stream* s, uint8_t c);
void writeString(stream* s, uint8_t * str);
void writeNum(stream*s, uint64_t n);
uint8_t read(stream * s);
void flush(stream* s);
#endif
