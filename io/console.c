#include "console.h"

char * consoleLinePrefix;

void consoleInit(struct kernel_args* args){
  videoMem = args->videoMemory;
  pixelsPerScanLine = args->pixelsPerScanLine;
  screenHeight = args->videoHeight;
  load_bmp(args->fontImage, &font);
  setCursorPosition(0,0);
  setSize(8);
  setForeground(255,255,255);
  setBackground(0,0,0);
  setConsolePrefix("");
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
  maxCurY = screenHeight/(fontSize*2);
}

void setConsolePrefix(char * prefix){
  consoleLinePrefix = prefix;
  uint64_t len = 0;
  while(*prefix != '\0'){
    len++;
    prefix++;
  }
  minCurX = len;
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

void printHex(uint64_t n){
  print("0x");
  if(n == 0UL){
    printChar('0');
    return;
  }
  uint64_t e = 0x1000000000000000;
  uint8_t started = 0;
  while(e > 0){
    uint64_t p = n/e;
    n -= p*e;
    e = e/16;
    if(p == 0 && started == 0) continue;
    started = 1;
    char c = '0' + p;
    if(p>=10) c += 39;
    printChar(c);
  }
}

void printHexByte(uint8_t b){
  uint8_t d = 0x10;
  while(d != 0){
    uint64_t p = b/d;
    b-= p*d;
    d /= 16;
    char c = '0' + p;
    if(p>=10) c += 39;
    printChar(c);
  }
}

void printChar(uint8_t ch){
  if(ch == '\n'){
    curX = 0;
    curY++;
    if(curY >= maxCurY){
      scroll();
      curY--;
    }
    print(consoleLinePrefix);
    return;
  }
  //Backspace
  if(ch == 0x08){
    if(curX > minCurX){
      curX--;
      printChar(' ');
      curX--;
    }
    return;
  }
  printCharAt(curX, curY, ch);
  curX++;
  if(curX >= maxCurX){
    curX = 0;
    curY++;
    if(curY >= maxCurY){
      scroll();
      curY--;
    }
    print(consoleLinePrefix);
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
      struct bmp_pixel* bmpPix;

      //bmp_getPixel sometimes returns invalid value, when interrupted by interrupt
      //Run until a valid value is returned
      while(1){
        bmpPix = bmp_getPixel(&font, bmpX, bmpY);
        if((uint64_t) bmpPix - (uint64_t)font.data < 256*512*4) break;
      }
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

void scroll(){
  uint64_t lineHeight = fontSize * 2;
  uint64_t size = (screenHeight - lineHeight)*pixelsPerScanLine;
  for(uint64_t i = 0;i < size;i++){
    struct pixel* src = videoMem+i+lineHeight*pixelsPerScanLine;
    struct pixel* dest = videoMem+i;
    *dest = *src;
  }
  uint64_t lineSize = lineHeight*pixelsPerScanLine;
  for(uint64_t i = 0;i < size;i++){
    struct pixel* p = videoMem+size+i;
    *p = bg;
  }
}

void beginError(){
  setBackground(255, 0, 0);
  clear();
  setCursorPosition(0, 0);
  setConsolePrefix("");
  uint64_t headingSize = pixelsPerScanLine/32;
  setSize(headingSize);
  for(uint64_t x = 0;x<32;x++){
    if((x) % 2 == 0){
      setBackground(255,255,0);
    } else {
      setBackground(0, 0, 0);
    }
    printCharAt(x,0,' ');
    printCharAt(x,maxCurY-1, ' ');
  }
  setBackground(255, 0, 0);
  setForeground(255,255,255);
  setCursorPosition(1,1);
  print("Uh Oh!");
  setSize(8);
  setConsolePrefix(" ");
  uint64_t startRow = 2*headingSize/8;
  setCursorPosition(1,startRow);
  print("The kernel has died.\n");
  print("Kernel has run into trouble and cannot continue its execution.\n");
  print("Here's a message from the kernel with further information and instructions:\n\n");
}
