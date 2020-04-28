#include <efi.h>
#include <efilib.h>

#include "elf.h"
#include "print.h"
#include "mem.h"
#include "kernel.h"
#include "file.h"


EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS res;
  EFI_INPUT_KEY key;
  ST=SystemTable;

  clear(ST);
  print(ST, L"OndrOS EFI bootloader\n\r");
  res = ST->ConIn->Reset(ST->ConIn, FALSE);
  if(EFI_ERROR(res))
    return res;

  EFI_GUID gopguid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
  print(ST, L"Locating Graphics Output Protocol.\n\r");
  res = ST->BootServices->LocateProtocol(
    &gopguid,
    NULL,
    &gop
  );
  if(EFI_ERROR(res)){
    if(res == EFI_NOT_FOUND){
      print(ST, L"EFI_NOT_FOUND\n\r");
    }
    if(res == EFI_INVALID_PARAMETER){
      print(ST, L"EFI_INVALID_PARAMETER\n\r");
    }
    print(ST, L"Failed\n\r");
    while(1);
    return res;
  }
  print(ST, L"Success\n\r");
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE* mode = gop->Mode;
  uint64_t graphicsMode = gop->Mode->MaxMode - 1;
  uint8_t first = 1;
  while(1){
    res = ST->ConIn->ReadKeyStroke(ST->ConIn, &key);
    if(res == EFI_NOT_READY && first == 0) continue;
    first = 0;
    clear(ST);
    print(ST, L"OndrOS EFI bootloader\n\r");
    print(ST, L"Use arrows to select graphics mode, ENTER to boot, ESC to exit.\n\r");
    print(ST, L"Mode ");
    printNum(ST, graphicsMode);
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* modeInfo;
    uint64_t infoSize;
    gop->QueryMode(
      gop,
      graphicsMode,
      &infoSize,
      &modeInfo
    );
    print(ST, L" : ");
    printNum(ST, modeInfo->HorizontalResolution);
    print(ST, L" x ");
    printNum(ST, modeInfo->VerticalResolution);
    print(ST, L"\n\r");
    ST->ConIn->Reset(ST->ConIn, FALSE);
    if(key.ScanCode == 0x17) return EFI_SUCCESS;
    if(key.ScanCode == 0x01 || key.ScanCode == 0x03) graphicsMode++;
    if(key.ScanCode == 0x02 || key.ScanCode == 0x04) graphicsMode--;
    if(key.UnicodeChar == '\r') break;
    if(graphicsMode == gop->Mode->MaxMode) graphicsMode = 0;
    if(graphicsMode == -1) graphicsMode = gop->Mode->MaxMode - 1;
  }

  print(ST, L"Setting mode ");
  printNum(ST, graphicsMode);
  print(ST, L"\n\r");
  gop->SetMode(gop, graphicsMode);

  print(ST, L"\n\rGetting Loaded Image Protocol\n\r");
  EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
  EFI_GUID loadedImageGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  ST->BootServices->HandleProtocol(
    ImageHandle,
    &loadedImageGuid,
    &loadedImage
  );
  if(EFI_ERROR(res)){
    print(ST, L"Failed\n\r");
    while(1);
    return res;
  }

  print(ST, L"Getting File System Protocol\n\r");
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
  EFI_GUID fsguid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

  res = ST->BootServices->HandleProtocol(
    loadedImage->DeviceHandle,
    &fsguid,
    &fs
  );
  if(EFI_ERROR(res)){
    print(ST, L"Failed\n\r");
    while(1);
    return res;
  }
  print(ST, L"\n\rOpening volume\n\r");
  EFI_FILE_PROTOCOL* fp;
  res = fs->OpenVolume(fs, &fp);
  if(EFI_ERROR(res)){
    print(ST, L"Failed\n\r");
    while(1);
    return res;
  }
  print(ST, L"Loading assets\n\r");
  void* fontImage = loadFile(ST, fp, L"\\EFI\\BOOT\\font.bmp");
  print(ST, L"Opening kernel\n\r");
  EFI_FILE_PROTOCOL* file;
  res = fp->Open(
    fp,
    &file,
    L"\\EFI\\BOOT\\kernel.elf",
    EFI_FILE_MODE_READ,
    0
  );
  if(EFI_ERROR(res)){
    if(res == EFI_NOT_FOUND){
      print(ST, L"EFI_NOT_FOUND\n\r");
    }
    print(ST, L"Failed\n\r");
    while(1);
    return res;
  }
  clear(ST);
  print(ST, L"Reading kernel header\n\r");
  struct elf_header header;
  uint64_t bufSize = sizeof(struct elf_header);
  res = file->Read(
    file,
    &bufSize,
    &header
  );
  if(EFI_ERROR(res)){
    print(ST, L"Failed\n\r");
    while(1);
    return res;
  }

  elfCheckHeader(ST, &header);
  print(ST, L"Program header at offset ");
  printNum(ST, header.e_phoff);
  //Jump to program header
  file->SetPosition(
    file,
    header.e_phoff
  );
  print(ST, L"\n\rLoading program headers\n\r");
  uint64_t vmemSize = 0;
  struct elf_p_header* pheaders = malloc(ST, header.e_phnum * sizeof(struct elf_p_header));
  uint64_t pheaderCount = 0;
  struct elf_p_header pheader;
  for(uint16_t i = 0; i < header.e_phnum; i++){
    uint64_t phsize = sizeof(struct elf_p_header);
    file->Read(
      file,
      &phsize,
      &pheader
    );
    if(pheader.p_type != 0x00000001){
      continue;
    }
    print(ST, L"<off=");
    printNum(ST, pheader.p_offset);
    print(ST, L" vaddr=");
    printNum(ST, pheader.p_vaddr);
    print(ST, L" paddr=");
    printNum(ST, pheader.p_paddr);
    print(ST, L" filesz=");
    printNum(ST, pheader.p_filesz);
    print(ST, L" memsz=");
    printNum(ST, pheader.p_memsz);
    print(ST, L">\n\r");
    if(pheader.p_vaddr + pheader.p_memsz > vmemSize){
      vmemSize = pheader.p_vaddr + pheader.p_memsz;
    }
    pheaders[pheaderCount] = pheader;
    pheaderCount++;
  }
  uint64_t pages = (vmemSize + 3999) / 4000;
  print(ST, L"Total ");
  printNum(ST, vmemSize);
  print(ST, L" bytes (");
  printNum(ST, pages);
  print(ST, L" pages) of virtual memory.\n\r");

  print(ST, L"Allocating virtual memory.\n\r");
  EFI_PHYSICAL_ADDRESS virtMemBase = 0x400000;
  res = ST->BootServices->AllocatePages(
    AllocateAnyPages,
    EfiLoaderData,
    pages,
    &virtMemBase
  );
  if(EFI_ERROR(res)){
    print(ST, L"Failed\n\r");
    while(1);
    return res;
  }
  print(ST, L"Allocated at address ");
  printNum(ST, virtMemBase);

  print(ST, L"\n\rCopying segments data.");
  for(uint64_t i = 0;i < pheaderCount;i++){
    struct elf_p_header* pheader = &pheaders[i];
    res = file->SetPosition(
      file,
      pheader->p_offset
    );
    if(EFI_ERROR(res)){
      print(ST, L"Failed\n\r");
      while(1);
      return res;
    }
    void* buffer = malloc(ST, pheader->p_filesz);
    uint64_t size = pheader->p_filesz;
    res = file->Read(
      file,
      &size,
      buffer
    );
    if(EFI_ERROR(res)){
      print(ST, L"Failed\n\r");
      while(1);
      return res;
    }
    ST->BootServices->CopyMem(
      virtMemBase + pheader->p_vaddr,
      buffer,
      size
    );
    if(EFI_ERROR(res)){
      print(ST, L"Failed\n\r");
      while(1);
      return res;
    }
    free(ST, buffer);
  }
  file->Close(file);
  print(ST, L"Success.");
  print(ST, L"\n\rEntrypoint at virtual address ");
  printNum(ST, header.e_entry);
  print(ST, L" physical ");
  printNum(ST, virtMemBase + header.e_entry);

  print(ST, L"\n\rEntering kernel.");
  print(ST, L"Exiting boot services and launching kernel.\n\r");

 uint64_t memMapSize = sizeof(EFI_MEMORY_DESCRIPTOR);
 EFI_MEMORY_DESCRIPTOR* memMap = malloc(ST, memMapSize);
 uint64_t mapKey;
 uint64_t descriptorSize;
 uint32_t descriptorVer;

 //Prompt for size
 res = ST->BootServices->GetMemoryMap(
   &memMapSize,
   memMap,
   &mapKey,
   &descriptorSize,
   &descriptorVer
 );
 free(ST, memMap);

 //Get with correct size
 memMap = malloc(ST, memMapSize);
 res = ST->BootServices->GetMemoryMap(
   &memMapSize,
   memMap,
   &mapKey,
   &descriptorSize,
   &descriptorVer
 );
 res = ST->BootServices->ExitBootServices(
   ImageHandle,
   mapKey
 );
 if(EFI_ERROR(res)){
   print(ST, L"Failed\n\r");
   while(1);
   return res;
 }
  typedef void __attribute((sysv_abi)) f(struct kernel_args);
  f* kernel = (f*)(virtMemBase + header.e_entry);
  struct kernel_args args;
  args.videoMemory = gop->Mode->FrameBufferBase;
  args.pixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;
  args.videoWidth = gop->Mode->Info->HorizontalResolution;
  args.videoHeight = gop->Mode->Info->VerticalResolution;
  args.fontImage = fontImage;
  kernel(args);
  while(1);
}
