#ifndef CONSOLE_H
#define CONSOLE_H

#include "bmp.h"

struct pixel{
  uint8_t B;
  uint8_t G;
  uint8_t R;
  uint8_t Reserved;
} __attribute__((__packed__));

struct pixel fg;
struct pixel bg;

uint64_t fontSize;
uint64_t maxCurX;
uint64_t maxCurY;
uint64_t curX;
uint64_t curY;
struct bmp font;
struct pixel* videoMem;
uint64_t pixelsPerScanLine;
uint64_t screenHeight;

void consoleInit(struct kernel_args* args);
void clear();
void print(uint8_t* s);
void printNum(uint64_t n);
void printChar(uint8_t ch);
void printCharAt(uint64_t x, uint64_t y, uint8_t ch);
void setConsolePrefix(char * prefix);
void setSize(uint64_t size);
void setForeground(uint8_t r, uint8_t g, uint8_t b);
void setBackground(uint8_t r, uint8_t g, uint8_t b);
void setCursorPosition(uint64_t x, uint8_t y);
void scroll();
#endif
