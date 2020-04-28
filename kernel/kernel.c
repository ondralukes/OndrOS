#include "types.h"
#include "bmp.h"

struct pixel{
  uint8_t B;
  uint8_t G;
  uint8_t R;
  uint8_t Reserved;
} __attribute__((__packed__));

void main(struct kernel_args args){
  uint16_t j = 0;
  struct pixel * videoMem = args.videoMemory;
  struct bmp image;
  load_bmp(args.testImage, &image);
  while(1){
    for(uint32_t x = 0;x<image.width;x++){
      for(uint32_t y = 0;y<image.height;y++){
        struct pixel * pixel = videoMem + y*args.pixelsPerScanLine+x;
        struct bmp_pixel* bmpPixel = bmp_getPixel(&image, x, y);
        pixel->R = bmpPixel->R;
        pixel->B = bmpPixel->B;
        pixel->G = bmpPixel->G;
      }
    }
    j+=16;
  }
}
