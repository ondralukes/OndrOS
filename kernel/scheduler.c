#include "scheduler.h"

struct processNode* processList;
struct processNode* currentProcessNode;
uint8_t schedulerEnabled = 0;
void schedulerTick(registers regs){
  if(schedulerEnabled == 0) return;

  pauseProcess(regs);
  if(currentProcessNode->p->state == Ended){
    flush(currentProcessNode->p->out);
    removeProcess(currentProcessNode->p);
  }
  struct process* processToRun = NULL;
  if(currentProcessNode->p == NULL){
    currentProcessNode = processList;
  }
  processToRun = currentProcessNode->p;
  if(currentProcessNode->next == NULL){
    currentProcessNode = processList;
  } else {
    currentProcessNode = currentProcessNode->next;
  }

  if(processToRun == NULL){
    asm("sti");
    asm("hlt");
  } else if(processToRun->sleepUntil < getMicroseconds()){
    resumeProcess(processToRun);
  }
}

void initScheduler(){
  processList = malloc(sizeof(struct processNode));
  processList->p = NULL;
  processList->next = NULL;
  currentProcessNode = processList;
}

void startScheduler(){
  schedulerEnabled = 1;
}

void registerProcess(struct process* p){
  struct processNode* ptr = processList;
  while(ptr->p != NULL){
    if(ptr->next == NULL){
      struct processNode* new = malloc(sizeof(struct processNode));
      new->p = p;
      new->next = NULL;
      ptr->next = new;
      return;
    }
    ptr = ptr->next;
  }
  ptr->p = p;
}

void removeProcess(struct process* p){
  struct processNode* ptr = processList;
  if(ptr->p == p){
    if(ptr->next == NULL){
      processList->p = NULL;
    } else {
      processList = ptr->next;
    }
    destroyProcess(p);
    return;
  }
  struct processNode* prev = processList;
  ptr = ptr->next;
  while(ptr->p != NULL){
    if(ptr->p == p){
      ptr->p = NULL;
      prev->next = ptr->next;
      break;
    }
    prev = ptr;
    ptr = ptr->next;
  }
  destroyProcess(p);
}

void flushProcessOut(){
  if(schedulerEnabled == 0) return;
  struct processNode* ptr = processList;
  while(ptr->p != NULL){
    stream* s = ptr->p->out;
    flush(s);
    ptr = ptr->next;
    if(ptr == NULL) break;
  }
}
