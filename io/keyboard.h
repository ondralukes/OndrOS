#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "console.h"
#include "../cpu/interrupts.h"
#include "stream.h"

void initKeyboard();

uint8_t scanCodeToChar(uint8_t scanCode);

extern stream* keyboardInput;
#endif
