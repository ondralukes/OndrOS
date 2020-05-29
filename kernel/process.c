#include "process.h"

struct process* currentProcess = NULL;

static void processEndCallback(){
  currentProcess->state = Ended;
  while(1);
}

struct process* createProcess(char* name, void* entrypoint){
  struct process* p = malloc(sizeof(struct process));
  p->name = name;
  p->state = Running;
  p->stack = malloc(1024*1024);
  p->out = createStream();
  p->in = keyboardInput;
  p->sleepUntil = 0;
  uint64_t stackBase = (uint64_t)p->stack;
  stackBase += 1024*1024 - 16;
  p->rip = (uint64_t) entrypoint;
  p->rax = 0;
  p->rcx = 0;
  p->rdx = 0;
  p->rbx = 0;
  p->rsi = 0;
  p->rdi = p;
  p->rsp = stackBase - 8;
  p->rbp = stackBase;

  *((uint64_t*)(stackBase - 8)) = &processEndCallback;

  return p;
}

void pauseProcess(registers regs){
  if(currentProcess == NULL) return;
  struct process* p = currentProcess;
  p->rax = regs.rax;
  p->rcx = regs.rcx;
  p->rdx = regs.rdx;
  p->rbx = regs.rbx;
  p->rsi = regs.rsi;
  p->rdi = regs.rdi;
  p->rsp = regs.userrsp;
  p->rbp = regs.rbp;
  p->rip = regs.rip;
}

uint64_t rax, rcx, rdx, rbx, rsi, rdi, rsp, rbp, rip;
void resumeProcess(struct process* p){
  currentProcess = p;
  rax = currentProcess->rax;
  rbx = currentProcess->rbx;
  rcx = currentProcess->rcx;
  rdx = currentProcess->rdx;
  rsi = currentProcess->rsi;
  rdi = currentProcess->rdi;
  rsp = currentProcess->rsp;
  rbp = currentProcess->rbp;
  rip = currentProcess->rip;
  asm volatile(
    "mov %%ax, %%ds\n\t"
    "mov %%ax, %%es\n\t"
    "mov %%ax, %%fs\n\t"
    "mov %%ax, %%gs\n\t"

    "pushq %%rax\n\t"
    "pushq %7\n\t"

    "pushfq\n\t"
    "popq %%rax\n\t"
    "or $0x0200, %%rax\n\t"
    "pushq %%rax\n\t"

    "pushq %%rdx\n\t"
    "pushq %8\n\t"

    "mov %0, %%rax\n\t"
    "mov %1, %%rbx\n\t"
    "mov %2, %%rcx\n\t"
    "mov %3, %%rdx\n\t"
    "mov %4, %%rsi\n\t"
    "mov %5, %%rdi\n\t"
    "mov %6, %%rbp\n\t"

    "iretq"
    : :
    "m"(rax),
    "m"(rbx),
    "m"(rcx),
    "m"(rdx),
    "m"(rsi),
    "m"(rdi),
    "m"(rbp),
    "m"(rsp),
    "m"(rip),
    "a"(USER_DS | 0b11),
    "d"(USER_CS | 0b11)
  );
}

void sleep(struct process*p, uint64_t us){
  p->sleepUntil = getMicroseconds() + us;
  while(p->sleepUntil > getMicroseconds());
}

void destroyProcess(struct process* p){
  free(p->out);
  free(p);
}
