#include "elf.h"

void elfCheckHeader(EFI_SYSTEM_TABLE* ST, struct elf_header* header){
  if(
    header->e_ident[0] != 0x7f||
    header->e_ident[1] != 0x45||
    header->e_ident[2] != 0x4c||
    header->e_ident[3] != 0x46
  ){
    print(ST, L"ELF magic number is invalid!\n\r");
    while(1);
  }
}
