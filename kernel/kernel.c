#include "kernel.h"
#include "types.h"
#include "process.h"
#include "../io/console.h"
#include "../io/keyboard.h"
#include "../cpu/interrupts.h"
#include "../cpu/init.h"
#include "time.h"
#include "memory.h"

void testProcessA(struct process* p){
  stream* out = p->out;
  uint64_t i = 0;
    while(i < 5){
      sleep(p, 500000);
      write(out, "Process A\n");
      i++;
    }
    write(out, "Process A go bye bye.\n");
}

void testProcessB(struct process* p){
  stream* out = p->out;
  uint64_t i = 0;
    while(i < 5){
      sleep(p, 1000000);
      write(out, "Process B\n");
      i++;
    }
  write(out, "Process B go bye bye.\n");
}

void main(struct kernel_args a){
  args = a;
  memoryInit(0x80000000);
  consoleInit(&args);
  clear();

  //Protect video memory
  mallocAt((uint64_t)args.videoMemory, args.pixelsPerScanLine*sizeof(struct pixel)*args.videoHeight);
  kernelStackBottom = (uint64_t)malloc(2048);
  kernelStackTop = kernelStackBottom + 2048 - 8;
  loadGdt((uint64_t)&stage2);
}

void stage2(){
  loadTss();
  allowKernelPageAccess();

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
  setBackground(0,0,0);
  setForeground(255, 255,0);
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
  print("Creating process.\n");
  struct process* p = createProcess("testA", &testProcessA);
  struct process* p2 = createProcess("testB", &testProcessB);
  print("Starting scheduler.\n");
  initScheduler();

  registerProcess(p);
  registerProcess(p2);
  setConsolePrefix("");
  setForeground(255, 255, 255);
  startScheduler();

  while(1)asm("hlt");
}
