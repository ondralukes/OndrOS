#include "print.h"

void print(EFI_SYSTEM_TABLE* ST, CHAR16* msg){
  ST->ConOut->OutputString(ST->ConOut, msg);
}

void printNum(EFI_SYSTEM_TABLE* ST, uint64_t n){
  CHAR16* msg = L"0";
  if(n == 0UL){
    *((char*)msg) = '0';
    print(ST, msg);
    return;
  }
  uint64_t e = 10000000000000000000UL;
  uint8_t started = 0;
  while(e > 0){
    uint64_t p = n/e;
    n -= p*e;
    e = e/10;
    if(p == 0 && started == 0) continue;
    started = 1;
    *((char*)msg) = '0' + p;
    print(ST, msg);
  }
}

void clear(EFI_SYSTEM_TABLE* ST){
  ST->ConOut->ClearScreen(ST->ConOut);
}
