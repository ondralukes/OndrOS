#include "console.h"

void consoleInit(struct kernel_args* args){
  videoMem = args->videoMemory;
  pixelsPerScanLine = args->pixelsPerScanLine;
  screenHeight = args->videoHeight;
  load_bmp(args->fontImage, &font);
  setCursorPosition(0,0);
  setSize(8);
  setForeground(255,255,255);
  setBackground(0,0,0);
}

void clear(){
  for(uint64_t x = 0;x<pixelsPerScanLine;x++){
    for(uint64_t y = 0;y<screenHeight;y++){
      struct pixel* p = videoMem+y*pixelsPerScanLine+x;
      *p = bg;
    }
  }
}

void setCursorPosition(uint64_t x, uint8_t y){
  curX = x;
  curY = y;
}

void setSize(uint64_t size){
  fontSize = size;
  maxCurX = pixelsPerScanLine/fontSize;
}

void print(uint8_t* s){
  while(*s != '\0'){
    printChar(*s);
    s++;
  }
}

void printNum(uint64_t n){
  if(n == 0UL){
    printChar('0');
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
    printChar('0' + p);
  }
}

void printChar(uint8_t ch){
  if(ch == '\n'){
    curX = 0;
    curY++;
    return;
  }
  printCharAt(curX, curY, ch);
  curX++;
  if(curX >= maxCurX){
    curX = 0;
    curY++;
  }
}

void printCharAt(uint64_t x, uint64_t y, uint8_t ch){
  uint64_t sourceY = (ch/16)*32;
  uint64_t sourceX = (ch%16)*16;
  uint64_t destX = x*fontSize;
  uint64_t destY = y*fontSize*2;
  for(uint64_t dx = 0;dx<fontSize;dx++){
    for(uint64_t dy = 0;dy<fontSize*2;dy++){
      struct pixel* p = videoMem+(destY+dy)*pixelsPerScanLine+destX+dx;
      uint64_t bmpX = sourceX + dx*16/fontSize;
      uint64_t bmpY = sourceY + dy*32/(fontSize*2);
      struct bmp_pixel* bmpPix = bmp_getPixel(&font, bmpX, bmpY);
      if(bmpPix->R == 255){
        *p = fg;
      } else {
        *p = bg;
      }
    }
  }
}

void setForeground(uint8_t r, uint8_t g, uint8_t b){
  fg.R = r;
  fg.G = g;
  fg.B = b;
}

void setBackground(uint8_t r, uint8_t g, uint8_t b){
  bg.R = r;
  bg.G = g;
  bg.B = b;
}
