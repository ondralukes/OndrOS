#include "kernel.h"
#include "types.h"
#include "process.h"
#include "../io/console.h"
#include "../io/keyboard.h"
#include "../io/ata.h"
#include "../cpu/interrupts.h"
#include "../cpu/init.h"
#include "time.h"
#include "memory.h"

void testProcessA(struct process* p){
  stream* out = p->out;
  int i = 10;
  while(i >= 0){
    sleep(p, 1000000);
    writeString(out, "Self-destruct in ");
    writeNum(out, i);
    write(out, '\n');
    i--;
  }
  i = 5/0;
}

void testProcessB(struct process* p){
  stream* out = p->out;
  stream* in = p->in;
  writeString(out, "Write something, or press enter to exit:");
  while(1){
    while(in->size == 0){
      sleep(p, 10000);
    }
    char c = read(in);
    write(out, c);
    if(c == '\n') break;
  }
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
  print("ATA setup.\n");
  AtaIdentify();
  AtaRead();
  print("Continuing in 2 seconds.\n");
  uint64_t start = getMicroseconds();
  while(getMicroseconds() - start < 2000000) asm("hlt");
  printMemory();
  print("Creating process.\n");

  struct process* p = createProcess("destructor", &testProcessA);
  struct process* p2 = createProcess("keyboardTest", &testProcessB);
  print("Starting scheduler.\n");
  initScheduler();

  registerProcess(p);
  registerProcess(p2);
  setConsolePrefix("");
  setForeground(255, 255, 255);
  startScheduler();

  while(1)asm("hlt");
}
