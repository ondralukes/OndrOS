#ifndef CONSOLE_H
#define CONSOLE_H

#include "../utils/bmp.h"

struct pixel{
  uint8_t B;
  uint8_t G;
  uint8_t R;
  uint8_t Reserved;
} __attribute__((__packed__));

extern struct pixel fg;
extern struct pixel bg;

extern uint64_t fontSize;
extern uint64_t maxCurX;
extern uint64_t maxCurY;
extern uint64_t minCurX;
extern uint64_t curX;
extern uint64_t curY;
extern struct bmp font;
extern struct pixel* videoMem;
extern uint64_t pixelsPerScanLine;
extern uint64_t screenHeight;

void consoleInit(struct kernel_args* args);
void clear();
void print(uint8_t* s);
void printNum(uint64_t n);
void printHex(uint64_t n);
void printHexByte(uint8_t b);
void printChar(uint8_t ch);
void printCharAt(uint64_t x, uint64_t y, uint8_t ch);
void setConsolePrefix(char * prefix);
void setSize(uint64_t size);
void setForeground(uint8_t r, uint8_t g, uint8_t b);
void setBackground(uint8_t r, uint8_t g, uint8_t b);
void setCursorPosition(uint64_t x, uint8_t y);
void scroll();
void beginError();
#endif
