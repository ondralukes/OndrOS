#include "ata.h"

uint16_t get12bit(uint64_t index, uint8_t * buffer){
  if(index % 2 == 0){
    uint64_t i = index / 2 * 3;
    uint8_t low = buffer[i];
    uint8_t high4 = buffer[i + 1] & 0xf;
    return low | (high4 << 8);
  } else {
    uint64_t i = (index-1) / 2 * 3 + 1;
    uint8_t low4 = (buffer[i] << 4) & 0xf;
    uint8_t high = buffer[i+1];
    return low4 | (high << 4);
  }
}

void AtaPollBSY(){
  while((byteIn(0x1f7) & (1 << 7)) != 0);
}

uint8_t AtaPoll(){
  byteIn(0x1f7);
  byteIn(0x1f7);
  byteIn(0x1f7);
  byteIn(0x1f7);
  while(1){
    uint8_t in = byteIn(0x1f7);
    if((in & (1 << 7)) == 0){
      break;
    }
    if((in & 1) != 0){
      return 1;
      break;
    }
    if((in & (1 << 5)) != 0){
      return 2;
      break;
    }
  }

  while(1){
    uint8_t in = byteIn(0x1f7);
    if((in & (1 << 3)) != 0){
      break;
    }
  }
  return 0;
}

void AtaIdentify(){
  byteOut(0x1f6, 0xa0);

  byteOut(0x1f2, 0);
  byteOut(0x1f3, 0);
  byteOut(0x1f4, 0);
  byteOut(0x1f5, 0);

  byteOut(0x1f7, 0xec);
  uint8_t res = byteIn(0x1f7);
  print("ATA IDENTIFY status ");
  printNum(res);
  print("\n");

  if(res == 0) return;
  if(AtaPoll() != 0){
    print("ATA IDENTIFY failed. Other drives are not yet supported.\n");
    return;
  }
  for(uint64_t i = 0;i<256;i++){
    uint16_t in = wordIn(0x1f0);
  }
  print("IDENTIFY end\n");
}

void AtaReadSectors(uint64_t lba, uint8_t count, uint8_t* buffer){
  byteOut(0x1f6, 0xe0 | ((lba >> 24) & 0xf));

  byteOut(0x1f1, 0x00);

  byteOut(0x1f2, count);
  byteOut(0x1f3, lba & 0xff);
  byteOut(0x1f4, (lba >> 8) & 0xff);
  byteOut(0x1f5, (lba >> 16) & 0xff);

  byteOut(0x1f7, 0x20);

  for(uint16_t s = 0; s<count;s++){
    AtaPoll();
    for(uint64_t i = 0;i<256;i++){
      ((uint16_t*)buffer)[s*256+i] = wordIn(0x1f0);
    }
  }
}
void AtaRead(){
  print("ATA READ\n");
  uint16_t *buffer = malloc(512);
  AtaReadSectors(0, 1, buffer);
  if(buffer[255] == 0xaa55){
    print("MBR detected\n");
    readMbr((struct mbr*)buffer);
  } else {
    print("Only MBR is supported.\n");
  }
  print("ATA END\n");
  free(buffer);
}

void readFatDir(struct mbr* mbr, struct fat_dir* d, uint8_t * path){
  uint8_t * childPath = path;
  while(*childPath != '\0') childPath++;
  uint64_t dataRegionOffset = mbr->bpb.reservedSectors + mbr->bpb.fatCount*mbr->bpb.sectorsPerFat + (mbr->bpb.maxRootDirectories * 32) / 512;
  uint64_t absoluteSector = dataRegionOffset + (d->cluster - 2) * mbr->bpb.clusterSize;

  uint8_t * buffer  = malloc(512);
  AtaReadSectors(absoluteSector, 1, buffer);
  struct fat_dir * dir = (struct fat_dir*)(buffer + 64);

  uint8_t * pathCur = childPath;
  for(uint64_t i = 0;i < 14;i++){
    pathCur = childPath;
    if(dir->attr == 0x10){
      print("Directory ");
      for(uint8_t j = 0;j<8;j++){
        if(dir->name[j] != ' '){
          *(pathCur++) = dir->name[j];
        }
      }
      *(pathCur++) = '/';
      *pathCur = '\0';
      print(path);
      print("\n");
      readFatDir(mbr, dir, path);
    }
    if(dir->attr == 0x20){
      print("File ");
      for(uint8_t j = 0;j<8;j++){
        if(dir->name[j] != ' '){
          *(pathCur++) = dir->name[j];
        }
      }
      *(pathCur++) = '.';
      *pathCur = '\0';
      for(uint8_t j = 0;j<3;j++){
        if(dir->extension[j] != ' '){
          *(pathCur++) = dir->extension[j];
        }
      }
      *pathCur = '\0';
      print(path);
      print("\n");
    }
  dir++;
}
  free(buffer);
}

void readMbr(struct mbr* mbr){
  print("Sector size: ");
  printNum(mbr->bpb.sectorSize);

  print("\nCluster size: ");
  printNum(mbr->bpb.clusterSize);
  print(" sectors");

  print("\nReserved sectors:");
  printNum(mbr->bpb.reservedSectors);

  print("\nNumber of FATs:");
  printNum(mbr->bpb.fatCount);

  print("\nTotal sectors:");
  printNum(mbr->bpb.totalSectors);

  print("\nMax root directories:");
  printNum(mbr->bpb.maxRootDirectories);

  print("\nSectors per FAT:");
  printNum(mbr->bpb.sectorsPerFat);
  print("\n");

  print("FAT at sector ");
  printNum(mbr->bpb.reservedSectors);
  print("\n");

  uint8_t * fat = malloc(mbr->bpb.sectorsPerFat * 512);
  AtaReadSectors(mbr->bpb.reservedSectors, mbr->bpb.sectorsPerFat, fat);

  uint64_t rootDirsOffset = mbr->bpb.reservedSectors + mbr->bpb.fatCount*mbr->bpb.sectorsPerFat;

  print("Root directory region at sector ");
  printNum(rootDirsOffset);
  print("\n");

  uint64_t rootDirsSectors = (mbr->bpb.maxRootDirectories * 32 + 511) / 512;
  struct fat_dir * dirs = malloc(rootDirsSectors*512);

  AtaReadSectors(rootDirsOffset, rootDirsSectors, (uint8_t*) dirs);

  uint8_t* path = malloc(2048);
  uint8_t* pathCur = path;
  for(uint64_t i = 0;i < 64;i++){
    struct fat_dir* dir = &dirs[i];
    if(dir->attr != 0x10) continue;
    print("Root directory ");
    for(uint8_t j = 0;j<8;j++) {
      printChar(dir->name[j]);
      if(dir->name[j] != ' '){
        *(pathCur++) = dir->name[j];
      }
    }
    *(pathCur++) = '/';
    *pathCur = '\0';
    print(".");
    for(uint8_t j = 0;j<3;j++) printChar(dir->extension[j]);
    print("\n");
    if(dir->attr == 0x10) readFatDir(mbr, dir, path);
  }
  free(dirs);
  free(path);
  free(fat);
}
