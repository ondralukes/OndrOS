#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "console.h"
#include "interrupts.h"

void initKeyboard();

uint8_t scanCodeToChar(uint8_t scanCode);
#endif