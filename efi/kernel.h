#include <efi.h>
#include <efilib.h>

struct kernel_args{
  void * videoMemory;
  uint64_t videoWidth;
  uint64_t videoHeight;
  uint64_t pixelsPerScanLine;

  void * fontImage;

  void * safeMemoryOffset;
};
