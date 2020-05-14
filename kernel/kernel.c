#include "types.h"
#include "../io/console.h"
#include "../io/keyboard.h"
#include "../cpu/interrupts.h"
#include "time.h"
#include "memory.h"

void main(struct kernel_args args){
  consoleInit(&args);
  clear();
  memoryInit(args.safeMemoryOffset);
  print("Resolution ");
  printNum(args.videoWidth);
  print("x");
  printNum(args.videoHeight);
  print(" PPSL ");
  printNum(args.pixelsPerScanLine);
  print("\n");
  print("Safe memory starts at ");
  printHex((uint64_t)args.safeMemoryOffset);
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
  print("Enabling interrupts.\n");
  asm("sti");
  setConsolePrefix("[Kernel setup] ");
  print("The time is now ");
  printNum(getMicroseconds());
  print(" us\n");
  print("Press 'e' to divide by zero.\n");
  print("Testing malloc\n");
  void* ptr = malloc(5);
  free(ptr);
  void* a = malloc(5);
  free(malloc(5));
  void* b = malloc(1024);
  printMemory();
  free(a);
  free(b);
  print("Freeing all.\n");
  printMemory();
  print("Halting CPU.");
  setConsolePrefix("kernel> ");
  setForeground(255, 255, 255);
  print("\n");
  asm("hlt");
}
