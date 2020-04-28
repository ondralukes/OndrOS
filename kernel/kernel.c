#include "types.h"
#include "console.h"

void main(struct kernel_args args){
  consoleInit(&args);
  clear();
  print("Resolution ");
  printNum(args.videoWidth);
  print("x");
  printNum(args.videoHeight);
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
  print("Halting CPU. Bye Bye.");
  asm("hlt");
}
