#include <efi.h>
#include <efilib.h>

void print(EFI_SYSTEM_TABLE* ST, CHAR16* msg);
void printNum(EFI_SYSTEM_TABLE* ST, uint64_t n);

void clear(EFI_SYSTEM_TABLE* ST);
