typedef unsigned char uint8_t;


typedef unsigned long long uint64_t;

typedef unsigned short uint16_t;

struct pixel{
  uint8_t B;
  uint8_t G;
  uint8_t R;
  uint8_t Reserved;
};

void main(struct pixel * videoMem, uint64_t PixelsPerScanLine){
  uint16_t j = 0;
  while(1){
    for(uint64_t x = 0;x<256;x++){
      for(uint64_t y = 0;y<256;y++){
        struct pixel * pixel = videoMem + y*PixelsPerScanLine+x;
        pixel->R = x;
        pixel->B = y;
        pixel->G = j/256;
      }
    }
    j+=16;
  }
}
