#include "types.h"
#include "../io/console.h"
#include "../io/keyboard.h"
#include "../cpu/interrupts.h"
#include "time.h"

void main(struct kernel_args args){
  consoleInit(&args);
  clear();
  print("Resolution ");
  printNum(args.videoWidth);
  print("x");
  printNum(args.videoHeight);
  print(" PPSL ");
  printNum(args.pixelsPerScanLine);
  print("\n");
  print("OndrOS Kernel is running and able to print stuff. Yay!\n");
  setBackground(12, 196, 43);
  setForeground(255, 0, 89);
  print("Colorful\n");
  setBackground(12, 21, 196);
  setForeground(255, 162, 0);
  setCursorPosition(2,1);
  setSize(32);
  print("Big");
  setSize(8);
  setBackground(0,0,0);
  setForeground(255, 0,0);
  setCursorPosition(0, 10);
  setConsolePrefix("[CPU setup] ");
  print("\nInstalling interrputs.\n");
  installInterrupts();
  print("Starting timer.\n");
  initTimer();
  print("Initializing keyboard.\n");
  initKeyboard();
  print("Testing interrupts.\n");
  asm("sti");
  asm("int $2");
  asm("int $3");
  asm("int $4");
  print("The time is now ");
  printNum(getMicroseconds());
  print(" us\n");
  print("Halting CPU.");
  setConsolePrefix("kernel> ");
  setForeground(255, 255, 255);
  print("\n");
  asm("hlt");
}
