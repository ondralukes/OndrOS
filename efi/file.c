#include "file.h"

void* loadFile(EFI_SYSTEM_TABLE* ST, EFI_FILE_PROTOCOL* fp, CHAR16* filename){
  EFI_STATUS res;
  EFI_FILE_PROTOCOL* file;
  res = fp->Open(
    fp,
    &file,
    filename,
    EFI_FILE_MODE_READ,
    0
  );
  if(EFI_ERROR(res)){
    print(ST, L"Open failed\n\r");
    while(1);
  }
  EFI_FILE_INFO* fileInfo = malloc(ST, 256);
  uint64_t infoSize = 256;
  EFI_GUID fileInfoGuid = EFI_FILE_INFO_ID;
  res = file->GetInfo(
    file,
    &fileInfoGuid,
    &infoSize,
    fileInfo
  );
  if(EFI_ERROR(res)){
    print(ST, L"GetInfo failed\n\r");
    while(1);
  }
  uint64_t size = fileInfo->FileSize;
  void* buffer = malloc(ST, size);
  res = file->Read(
    file,
    &size,
    buffer
  );
  free(ST, fileInfo);
  if(EFI_ERROR(res)){
    print(ST, L"Read failed\n\r");
    while(1);
  }
  file->Close(file);
  return buffer;
}
