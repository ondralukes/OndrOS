#include "interrupts.h"

void installInterrupts(){
  setInterrupt(0, (uint64_t) isr0);
  setInterrupt(1, (uint64_t) isr1);
  setInterrupt(2, (uint64_t) isr2);
  setInterrupt(3, (uint64_t) isr3);
  setInterrupt(4, (uint64_t) isr4);
  setInterrupt(5, (uint64_t) isr5);
  setInterrupt(6, (uint64_t) isr6);
  setInterrupt(7, (uint64_t) isr7);
  setInterrupt(8, (uint64_t) isr8);
  setInterrupt(9, (uint64_t) isr9);
  setInterrupt(10, (uint64_t) isr10);
  setInterrupt(11, (uint64_t) isr11);
  setInterrupt(12, (uint64_t) isr12);
  setInterrupt(13, (uint64_t) isr13);
  setInterrupt(14, (uint64_t) isr14);
  setInterrupt(15, (uint64_t) isr15);
  setInterrupt(16, (uint64_t) isr16);
  setInterrupt(17, (uint64_t) isr17);
  setInterrupt(18, (uint64_t) isr18);
  setInterrupt(19, (uint64_t) isr19);
  setInterrupt(20, (uint64_t) isr20);
  setInterrupt(21, (uint64_t) isr21);
  setInterrupt(22, (uint64_t) isr22);
  setInterrupt(23, (uint64_t) isr23);
  setInterrupt(24, (uint64_t) isr24);
  setInterrupt(25, (uint64_t) isr25);
  setInterrupt(26, (uint64_t) isr26);
  setInterrupt(27, (uint64_t) isr27);
  setInterrupt(28, (uint64_t) isr28);
  setInterrupt(29, (uint64_t) isr29);
  setInterrupt(30, (uint64_t) isr30);
  setInterrupt(31, (uint64_t) isr31);

  applyInterrupts();
}

void setInterrupt(int index, uint64_t handler){
  interrupts[index].offset1 = handler&0xffff;
  interrupts[index].offset2 = (handler>>16)&0xffff;
  interrupts[index].offset3 = (handler>>32)&0xffffffff;
  interrupts[index].seg = 0x38; //Code segment
  interrupts[index].zero = 0;
  interrupts[index].ist = 0;
  interrupts[index].flags = 0x8e; //kernel, 32bit handler
}
void applyInterrupts(){
  interruptTable.base = (uint64_t) &interrupts;
  interruptTable.limit = 256 * sizeof(int_gate) - 1;

  asm volatile("lidt (%0)" : : "r" (&interruptTable));
}

void isrHandler(registers regs){
  print("Interrupt #");
  printNum(regs.intNum);
  print("\n");
}
