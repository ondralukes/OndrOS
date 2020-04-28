#include <efi.h>
#include <efilib.h>

#include "mem.h"
#include "print.h"

void* loadFile(EFI_SYSTEM_TABLE* ST, EFI_FILE_PROTOCOL* fp, CHAR16* filename);
