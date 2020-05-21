#include "stream.h"
void writeChar(stream* s, uint8_t c){
  if(s->size >= 128) return;
  s->buffer[s->size] = c;
  s->size++;
}

void write(stream* s, uint8_t * str){
  uint8_t* c = str;
  while(*c != '\0'){
    writeChar(s, *c);
    if(s->size == 128) break;
    c++;
  }
}

stream* createStream(){
  stream* s = malloc(sizeof(stream));
  s->size = 0;
  return s;
}
