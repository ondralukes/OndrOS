#include <efi.h>
#include <efilib.h>

void * malloc(EFI_SYSTEM_TABLE* ST, uint64_t size);

void free(EFI_SYSTEM_TABLE* ST, void* ptr);
