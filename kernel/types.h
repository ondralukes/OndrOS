#ifndef TYPES_H
#define TYPES_H
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef struct kernel_args{
  void * videoMemory;
  uint64_t videoWidth;
  uint64_t videoHeight;
  uint64_t pixelsPerScanLine;

  void * fontImage;
};
#endif
