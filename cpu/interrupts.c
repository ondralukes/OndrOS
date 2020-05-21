#include "interrupts.h"

void installInterrupts(){
  //Remap PIC
  byteOut(0x20, 0x11);
  byteOut(0xA0, 0x11);

  byteOut(0x21, 0x20);
  byteOut(0xA1, 0x28);

  byteOut(0x21, 0x04);
  byteOut(0xA1, 0x02);

  byteOut(0x21, 0x01);
  byteOut(0xA1, 0x01);

  byteOut(0x21, 0x0);
  byteOut(0xA1, 0x0);

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

  setInterrupt(32, (uint64_t) irq0);
  setInterrupt(33, (uint64_t) irq1);
  setInterrupt(34, (uint64_t) irq2);
  setInterrupt(35, (uint64_t) irq3);
  setInterrupt(36, (uint64_t) irq4);
  setInterrupt(37, (uint64_t) irq5);
  setInterrupt(38, (uint64_t) irq6);
  setInterrupt(39, (uint64_t) irq7);
  setInterrupt(40, (uint64_t) irq8);
  setInterrupt(41, (uint64_t) irq9);
  setInterrupt(42, (uint64_t) irq10);
  setInterrupt(43, (uint64_t) irq11);
  setInterrupt(44, (uint64_t) irq12);
  setInterrupt(45, (uint64_t) irq13);
  setInterrupt(46, (uint64_t) irq14);
  setInterrupt(47, (uint64_t) irq15);

  setInterruptHandler(14, pageFault);

  applyInterrupts();
}

void setInterrupt(int index, uint64_t handler){
  interrupts[index].offset1 = handler&0xffff;
  interrupts[index].offset2 = (handler>>16)&0xffff;
  interrupts[index].offset3 = (handler>>32)&0xffffffff;
  interrupts[index].seg = KERNEL_CS; //Code segment
  interrupts[index].zero = 0;
  interrupts[index].ist = 1;
  interrupts[index].flags = 0x8e; //kernel, 32bit handler
}
void applyInterrupts(){
  interruptTable.base = (uint64_t) &interrupts;
  interruptTable.limit = 256 * sizeof(int_gate) - 1;

  asm volatile("lidt (%0)" : : "r" (&interruptTable));
}

void setInterruptHandler(int index, isr isr){
  isrs[index] = isr;
}

void isrHandler(registers regs){
  if(isrs[regs.intNum] != 0){
    isrs[regs.intNum](regs);
  } else if(regs.intNum != 4){
    char* exceptions[32];
    exceptions[0] = "Division by zero";
    exceptions[1] = "Debug";
    exceptions[2] = "Non-maskable interrupt";
    exceptions[3] = "Breakpoint";
    exceptions[4] = "Overflow";
    exceptions[5] = "Bound Range Exceeded";
    exceptions[6] = "Invalid Opcode";
    exceptions[7] = "Device not available";
    exceptions[8] = "Double fault";
    exceptions[9] = "Coprocessor Segment Overrrun";
    exceptions[10] = "Invalid TSS";
    exceptions[11] = "Segment not present";
    exceptions[12] = "Stack-Segment fault";
    exceptions[13] = "General Protection Fault";
    exceptions[14] = "Page Fault";
    exceptions[15] = "Reserved";
    exceptions[16] = "x87 Floating-Point Exception";
    exceptions[17] = "Alignment Check";
    exceptions[18] = "Machine Check";
    exceptions[19] = "SIMD Floating-Point Exception";
    exceptions[20] = "Virtualization Exception";
    exceptions[30] = "Security Exception";
    beginError();
    print("Kernel received interrupt #");
    printNum(regs.intNum);
    print(" which means ");
    print(exceptions[regs.intNum]);
    print(".\n");
    print("Error code is ");
    printHex(regs.errorCode);
    print("\nRegisters:\n");
    print("rax="); printHex(regs.rax);
    print(" rbx="); printHex(regs.rbx);
    print(" rcx="); printHex(regs.rcx);

    print("\nrdx="); printHex(regs.rdx);

    print("\nrsi="); printHex(regs.rsi);
    print(" rdi="); printHex(regs.rdi);
    print(" rip="); printHex(regs.rip);

    print("\ncs="); printHex(regs.cs);
    print(" flags="); printHex(regs.rflags);
    print(" userrsp="); printHex(regs.userrsp);
    print("\n128 bytes from RIP:\n");
    uint8_t *ptr = (uint8_t*)regs.rip;
    for(uint64_t i = 0;i<128;i++){
      if(i%16 == 0){
        print("\nRIP+");
        printHex(i);
        print(": ");
      }
      printHexByte(*ptr);
      print(" ");
      ptr++;
    }
    asm("hlt");
  }
}

void irqHandler(registers regs){
  //Send End Of Interrupt
  if(regs.intNum >= 40){
    byteOut(0xa0, 0x20);
  }
  byteOut(0x20, 0x20);
  if(isrs[regs.intNum] != 0){
    isrs[regs.intNum](regs);
  }
}

void pageFault(registers regs){
  uint64_t addr;
  asm volatile (
    "movq %%cr2, %%rax\n\t"
    "movq %%rax, %0"
    :"=m"(addr));
  allowPageAccess(addr);
}
