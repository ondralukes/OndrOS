#ifndef BMP_H
#define BMP_H

#include "../kernel/types.h"
#include "../kernel/memory.h"
struct bmp_pixel{
  uint8_t B;
  uint8_t G;
  uint8_t R;
} __attribute__((packed));

struct bmp{
  uint32_t width;
  uint32_t height;
  void* data;
};

struct bmp_header{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
    uint32_t dib_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_ppm;
    uint32_t y_ppm;
    uint32_t num_colors;
    uint32_t important_colors;
} __attribute__((__packed__));

void load_bmp(void* file, struct bmp* bmp);

struct bmp_pixel* bmp_getPixel(struct bmp* bmp, uint64_t x, uint64_t y);
#endif
