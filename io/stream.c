#include "stream.h"

void writeString(stream* s, uint8_t * str){
  uint8_t* c = str;
  while(*c != '\0'){
    write(s, *c);
    if(s->size == 128) break;
    c++;
  }
}

void write(stream* s, uint8_t c){
  if(s->size >= 128) return;
  s->buffer[s->size] = c;
  s->size++;
}

void writeNum(stream* s, uint64_t n){
  if(n == 0UL){
    write(s, '0');
    return;
  }
  uint64_t e = 10000000000000000000UL;
  uint8_t started = 0;
  while(e > 0){
    uint64_t p = n/e;
    n -= p*e;
    e = e/10;
    if(p == 0 && started == 0) continue;
    started = 1;
    write(s, '0' + p);
  }
}
uint8_t read(stream * s){
  uint8_t c = s->buffer[--s->size];
  return c;
}

stream* createStream(){
  stream* s = malloc(sizeof(stream));
  s->size = 0;
  return s;
}

void flush(stream* s){
  for(uint64_t i =0;i<s->size;i++){
     printChar(s->buffer[i]);
   }
  s->size = 0;
}
