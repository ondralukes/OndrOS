#include "bmp.h"

void load_bmp(void* file, struct bmp* bmp){
  struct bmp_header* header = (struct bmp_header*)file;
  bmp->width = header->width;
  bmp->height = header->height;
  bmp->data = file + header->offset;

  //Protect image data
  mallocAt((uint64_t)bmp->data, header->width*header->height*3);
}

struct bmp_pixel* bmp_getPixel(struct bmp* bmp, uint64_t x, uint64_t y){
  uint64_t rowSize = ((bmp->width*3+3)/4)*4;
  void* res = bmp->data + (bmp->height - 1 - y)*rowSize + x*3;
  return (struct bmp_pixel*) res;
}
