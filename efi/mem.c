#include "mem.h"
#include "print.h"

void * malloc(EFI_SYSTEM_TABLE* ST, uint64_t size){
  void * res;
  EFI_STATUS status = ST->BootServices->AllocatePool(EfiLoaderData, size, &res);
  if(EFI_ERROR(status)){
    print(ST, L"Failed to allocate memory\n\r");
    while(1);
  }
  return res;
}

void free(EFI_SYSTEM_TABLE* ST, void* ptr){
  ST->BootServices->FreePool(ptr);
}
